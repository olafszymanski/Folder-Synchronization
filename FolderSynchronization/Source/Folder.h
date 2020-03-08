#pragma once

template<typename T>
struct SynchronizeItem
{
	std::shared_ptr<T> Reference;
	std::filesystem::path Path;
	std::optional<bool> Create;
};

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
	void Compare(const std::shared_ptr<File>& reference);

	std::vector<char> Read(const File& file);
};

class Folder
{
public:
	Folder(const std::filesystem::path& path);
	~Folder() = default;

	bool IsSubfolder(const Folder& parent);

	void Compare(const Folder& reference);
	void Synchronize(const Folder& reference);

	inline bool ContainsFiles() const { return Files.size() > 0; }
	inline bool ContainsFolders() const { return Folders.size() > 0; }

public:
	const std::filesystem::path Path;

	std::vector<std::shared_ptr<File>> Files;
	std::vector<std::shared_ptr<Folder>> Folders;

private:
	void Load(Folder& folder);

	bool CompareFiles(const std::shared_ptr<Folder>& reference);
	void CompareFolders(const std::shared_ptr<Folder>& reference);
};

class Result
{
public:
	static std::vector<SynchronizeItem<Folder>> MissingFolders;
	static std::vector<SynchronizeItem<File>> MissingFiles;
	static std::vector<SynchronizeItem<File>> SynchronizeFiles;

private:
	Result() = default;
	~Result() = default;
};