#pragma once

class File
{
	friend class Folder;

public:
	File(const std::filesystem::path& path);
	~File() = default;

public:
	const std::filesystem::path Path;
	const unsigned int Size;

private:
	void CompareAndWrite(const File& file);

	std::vector<char> Read(const File& path);
};

class Folder
{
public:
	Folder(const std::filesystem::path& path);
	~Folder() = default;

	void Synchronize(const Folder& original);

	bool ContainsFiles() const { return Files.size() != 0; }
	bool ContainsFolders() const { return Folders.size() != 0; }

public:
	const std::filesystem::path Path;

	std::vector<std::shared_ptr<File>> Files;
	std::vector<std::shared_ptr<Folder>> Folders;

private:
	void SynchronizeFiles(const Folder& original);
	void SynchronizeFolders(const Folder& original);
};

class FolderLoader
{
public:
	static Folder Load(const std::filesystem::path& path);

private:
	FolderLoader() = default;
	~FolderLoader() = default;
};