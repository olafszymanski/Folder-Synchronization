#include "pch.h"

#include "FileLoader.h"

#include "Config.h"

#include "Util/Timer.h"

#include "Util/Logger.h"

File::File(const std::filesystem::path& path)
	: Path(path), Size(static_cast<unsigned int>(std::filesystem::file_size(path)))
{
}

void File::CompareAndWrite(const File& original)
{
	if (Config::FastMode)
	{
		if (Size == original.Size)
		{
			LOG_INFO("File: '{0}' and '{1}' are the same.", Path.u8string(), original.Path.u8string());
		}
		else
		{
			Timer timer;

			std::filesystem::copy_file(original.Path, Path, std::filesystem::copy_options::overwrite_existing);

			LOG_DEBUG("Overwriting file: '{0}' took {1}ms.", Path.u8string(), timer.Stop());
		}
	}
	else
	{
		auto contentsFuture = std::async(&File::Read, this, std::ref(*this));
		auto originalContentsFuture = std::async(&File::Read, original, std::ref(original));
		
		std::vector<char> contents = contentsFuture.get();
		std::vector<char> originalContents = originalContentsFuture.get();

		if (contents.size() == originalContents.size() && memcmp(contents.data(), originalContents.data(), contents.size()) == 0)
		{
			LOG_INFO("File: '{0}' and '{1}' are the same.", Path.u8string(), original.Path.u8string());
		}
		else
		{
			Timer timer;

			std::filesystem::copy_file(original.Path, Path, std::filesystem::copy_options::overwrite_existing);

			LOG_DEBUG("Overwriting file: '{0}' took {1}ms.", Path.u8string(), timer.Stop());
		}
	}
}

std::vector<char> File::Read(const File& file)
{
	std::vector<char> contents { };

	if (std::filesystem::file_size(file.Path) > 0)
	{
		Timer timer;

		boost::interprocess::file_mapping mappedFile(file.Path.u8string().c_str(), boost::interprocess::read_only);
		boost::interprocess::mapped_region region(mappedFile, boost::interprocess::read_only);

		std::filebuf buffer;
		buffer.open(file.Path, std::ios_base::in | std::ios_base::binary);

		std::vector<char> contents(region.get_size(), 0);
		buffer.sgetn(&contents[0], std::streamsize(contents.size()));

		region.flush();
		
		LOG_DEBUG("Reading file: '{0}' took {1}ms.", file.Path.u8string(), timer.Stop());

		return contents;
	}
	else return contents;
}

Folder::Folder(const std::filesystem::path& path)
	: Path(path)
	, Files({ }), Folders({ }) 
{
}

void Folder::Synchronize(const Folder& original)
{
	SynchronizeFolders(original);
}

void Folder::SynchronizeFiles(const Folder& original)
{
	LOG_INFO("Comparing files in '{0}' to files in '{1}'.", Path.u8string(), original.Path.u8string());

	if (!ContainsFiles() && original.ContainsFiles())
	{
		LOG_WARNING("Large file copy from '{0}' to '{1}'. This action might take a while.", original.Path.u8string(), Path.u8string());

		std::filesystem::copy(original.Path, Path, std::filesystem::copy_options::none);
	}
	else
	{
		std::vector<std::shared_ptr<File>> missingFiles = original.Files;

		missingFiles.erase(std::remove_if(missingFiles.begin(), missingFiles.end(),
			[this](const std::shared_ptr<File>& file)
			{
				for (unsigned int i = 0; i < Files.size(); ++i)
				{
					if (Files[i]->Path.filename() == file->Path.filename())
					{
						LOG_INFO("Comparing file: '{0}' with '{1}'.", Files[i]->Path.u8string(), file->Path.u8string());
						// TODO: Exception for files that C++ can't read eg. images.

						Files[i]->CompareAndWrite(*file);

						return true;
					}
				}

				return false;
			}), missingFiles.end());

		if (missingFiles.size() > 0)
		{
			for (const auto& file : missingFiles)
			{
				LOG_INFO("Copying file: '{0}' to '{1}'.", file->Path.u8string(), Path.u8string());

				std::filesystem::copy_file(file->Path, Path.u8string() + "\\" + file->Path.filename().u8string(), std::filesystem::copy_options::none);
			}
		}
	}
}
void Folder::SynchronizeFolders(const Folder& original)
{
	LOG_INFO("Comparing folders in '{0}' to folders in '{1}'.", Path.u8string(), original.Path.u8string());

	if (!ContainsFolders() && original.ContainsFolders())
	{
		LOG_WARNING("Large folder copy from '{0}' to '{1}'. This action might take a while.", original.Path.u8string(), Path.u8string());

		std::filesystem::copy(original.Path, Path, std::filesystem::copy_options::recursive | std::filesystem::copy_options::skip_existing);
	}
	else
	{
		SynchronizeFiles(original);

		std::vector<std::shared_ptr<Folder>> missingFolders = original.Folders;

		missingFolders.erase(std::remove_if(missingFolders.begin(), missingFolders.end(),
			[this](const std::shared_ptr<Folder>& folder)
			{
				for (unsigned int i = 0; i < Folders.size(); ++i)
				{
					if (Folders[i]->Path.filename() == folder->Path.filename())
					{
						Folders[i]->SynchronizeFolders(*folder);

						return true;
					}
				}

				return false;
			}), missingFolders.end());

		if (missingFolders.size() > 0)
		{
			for (const auto& folder : missingFolders)
			{
				LOG_WARNING("Copying folder: from '{0}' to '{1}'.", original.Path.u8string(), Path.u8string());

				std::string directory = Path.u8string() + "\\" + folder->Path.filename().u8string();

				std::filesystem::create_directory(directory);
				std::filesystem::copy(folder->Path, directory, std::filesystem::copy_options::recursive | std::filesystem::copy_options::skip_existing);
			}
		}
	}
}

Folder FolderLoader::Load(const std::filesystem::path& path)
{
	Folder folder(path);

	for (const auto& directory : std::filesystem::directory_iterator(folder.Path))
	{
		if (directory.is_regular_file()) folder.Files.push_back(std::make_shared<File>(directory.path()));

		if (directory.is_directory()) folder.Folders.push_back(std::make_shared<Folder>(Load(directory.path())));
	}

	return folder;
}