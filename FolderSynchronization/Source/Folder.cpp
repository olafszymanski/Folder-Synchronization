#include "pch.h"

#include "Folder.h"

#include "Config.h"

#include "Util/Timer.h"

#include "Util/Logger.h"

std::vector<SynchronizeItem<Folder>> Result::MissingFolders { };
std::vector<SynchronizeItem<File>> Result::MissingFiles { };
std::vector<SynchronizeItem<File>> Result::SynchronizeFiles { };

File::File(const std::filesystem::path& path)
	: Path(path), Size(static_cast<unsigned int>(std::filesystem::file_size(path)))
{
}

void File::Compare(const std::shared_ptr<File>& reference)
{
	LOG_INFO("Comparing file: '{0}' with '{1}'.", Path.u8string(), reference->Path.u8string());

	DWORD attributes = GetFileAttributes(Path.c_str());

	if (attributes & FILE_ATTRIBUTE_HIDDEN || attributes & FILE_ATTRIBUTE_READONLY || attributes & FILE_ATTRIBUTE_SYSTEM) 
		LOG_ERROR("File: '{0}' is not suitable for comparison!", Path.u8string());
	else
	{
		if (Config::FastMode)
		{
			if (Size == reference->Size) LOG_INFO("File: '{0}' and '{1}' are the same.", Path.u8string(), reference->Path.u8string());
			else Result::SynchronizeFiles.push_back({ reference, Path });
		}
		else
		{
			auto contentsFuture = std::async(&File::Read, this, *this);
			auto referenceContentsFuture = std::async(&File::Read, reference, *reference);

			std::vector<char> contents = contentsFuture.get();
			std::vector<char> referenceContents = referenceContentsFuture.get();

			if (contents.size() == referenceContents.size() && memcmp(contents.data(), referenceContents.data(), contents.size()) == 0) 
				LOG_INFO("File: '{0}' and '{1}' are the same.", Path.u8string(), reference->Path.u8string());
			else Result::SynchronizeFiles.push_back({ reference, Path });
		}
	}
}

std::vector<char> File::Read(const File& file)
{
	if (file.Size > 0)
	{
		Timer timer;

		boost::interprocess::file_mapping mappedFile(file.Path.u8string().c_str(), boost::interprocess::read_only);
		boost::interprocess::mapped_region region(mappedFile, boost::interprocess::read_only);

		std::filebuf buffer;
		buffer.open(file.Path, std::ios_base::in | std::ios_base::binary);

		std::vector<char> contents(region.get_size(), 0);
		buffer.sgetn(&contents[0], contents.size());

		region.flush();

		LOG_DEBUG("Reading file: '{0}' took {1}ms.", file.Path.u8string(), timer.Stop());

		return contents;
	}
	else
	{
		LOG_WARNING("File: '{0}' was empty.", file.Path.u8string());

		return { };
	}
}

Folder::Folder(const std::filesystem::path& path)
	: Path(path)
	, Files({ }), Folders({ })
{
	Load(*this);
}

bool Folder::IsSubfolder(const Folder& parent)
{
	std::filesystem::path path = Path;

	while (path.has_parent_path() && path.has_relative_path())
	{
		if (path == parent.Path) return true;

		path = path.parent_path();
	}

	return false;
}

void Folder::Compare(const Folder& reference)
{
	Result::MissingFolders = { };
	Result::MissingFiles = { };
	Result::SynchronizeFiles = { };

	Timer timer;

	CompareFolders(std::make_shared<Folder>(reference));

	if (Result::MissingFolders.size() > 0)
	{
		if (Result::MissingFolders.size() > 1) LOG_WARNING("Found {0} missing folders.", Result::MissingFolders.size());
		else LOG_WARNING("Found {0} missing folder.", Result::MissingFolders.size());
	}

	if (Result::MissingFiles.size() > 0)
	{
		if (Result::MissingFiles.size() > 1) LOG_WARNING("Found {0} missing files.", Result::MissingFiles.size());
		else LOG_WARNING("Found {0} missing file.", Result::MissingFiles.size());
	}

	if (Result::SynchronizeFiles.size() > 0)
	{
		if (Result::SynchronizeFiles.size() > 1) LOG_WARNING("Found {0} files that need to be synchronized.", Result::SynchronizeFiles.size());
		else LOG_WARNING("Found {0} file that needs to be synchronized.", Result::SynchronizeFiles.size());
	}

	LOG_DEBUG("Comparison took: {0}ms.", timer.Stop());
}
void Folder::Synchronize(const Folder& reference)
{
	Timer timer;

	if (Result::MissingFolders.size() > 0)
	{
		for (const auto& folder : Result::MissingFolders)
		{
			if (folder.Create)
			{
				LOG_INFO("Copying folder: '{0}' to '{1}'.", folder.Reference->Path.u8string(), folder.Path.u8string());

				std::string directory = folder.Path.u8string() + "\\" + folder.Reference->Path.filename().u8string();

				std::filesystem::create_directory(directory);
				std::filesystem::copy(folder.Reference->Path, directory, std::filesystem::copy_options::recursive | std::filesystem::copy_options::skip_existing);
			}
			else
			{
				LOG_INFO("Synchronizing folder: '{0}' with '{1}'.", folder.Path.u8string(), folder.Reference->Path.u8string());

				std::filesystem::copy(folder.Reference->Path, folder.Path, std::filesystem::copy_options::recursive | std::filesystem::copy_options::skip_existing);
			}
		}
	}

	if (Result::MissingFiles.size() > 0)
	{
		for (const auto& file : Result::MissingFiles)
		{
			LOG_INFO("Copying file: '{0}' to '{1}'.", file.Reference->Path.u8string(), file.Path.u8string());

			std::filesystem::copy_file(file.Reference->Path, file.Path.u8string() + "\\" + file.Reference->Path.filename().u8string(), std::filesystem::copy_options::none);
		}
	}

	if (Result::SynchronizeFiles.size() > 0)
	{
		for (const auto& file : Result::SynchronizeFiles)
		{
			LOG_INFO("Synchronizing file: '{0}' with '{1}'.", file.Path.u8string(), file.Reference->Path.u8string());

			std::filesystem::copy_file(file.Reference->Path, file.Path.u8string(), std::filesystem::copy_options::overwrite_existing);
		}
	}

	Result::MissingFolders = { };
	Result::MissingFiles = { };
	Result::SynchronizeFiles = { };

	LOG_DEBUG("Synchronization took: {0}ms.", timer.Stop());
}

void Folder::Load(Folder& folder)
{
	for (const auto& directory : std::filesystem::directory_iterator(folder.Path))
	{
		if (directory.is_regular_file()) folder.Files.push_back(std::make_shared<File>(directory.path()));

		if (directory.is_directory()) folder.Folders.push_back(std::make_shared<Folder>(directory.path()));
	}
}

bool Folder::CompareFiles(const std::shared_ptr<Folder>& reference)
{
	LOG_INFO("Comparing files in '{0}' to files in '{1}'.", Path.u8string(), reference->Path.u8string());

	if (!ContainsFiles() && reference->ContainsFiles())
	{
		Result::MissingFolders.push_back({ reference, Path });
	
		return true;
	}
	else
	{
		std::vector<std::shared_ptr<File>> missingFiles = reference->Files;

		missingFiles.erase(std::remove_if(missingFiles.begin(), missingFiles.end(),
			[this](const std::shared_ptr<File>& referenceFile)
			{
				for (const auto& file : Files)
				{
					if (file->Path.filename() == referenceFile->Path.filename())
					{
						file->Compare(referenceFile);

						return true;
					}
				}

				return false;
			}), missingFiles.end());

		for (const auto& file : missingFiles)
		{
			Result::MissingFiles.push_back({ file, Path });
		}

		return false;
	}
}
void Folder::CompareFolders(const std::shared_ptr<Folder>& reference)
{
	LOG_INFO("Comparing folders in '{0}' to folders in '{1}'.", Path.u8string(), reference->Path.u8string());

	if (!ContainsFolders() && reference->ContainsFolders()) Result::MissingFolders.push_back({ reference, Path });
	else
	{
		if (!CompareFiles(reference))
		{
			std::vector<std::shared_ptr<Folder>> missingFolders = reference->Folders;

			missingFolders.erase(std::remove_if(missingFolders.begin(), missingFolders.end(),
				[this](const std::shared_ptr<Folder>& referenceFolder)
				{
					for (const auto& folder : Folders)
					{
						if (folder->Path.filename() == referenceFolder->Path.filename())
						{
							folder->CompareFolders(referenceFolder);

							return true;
						}
					}

					return false;
				}), missingFolders.end());

			for (const auto& folder : missingFolders)
			{
				Result::MissingFolders.push_back({ folder, Path, true });
			}
		}
	}
}