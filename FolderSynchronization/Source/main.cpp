#include "pch.h"

/*#include "FileLoader.h"

#include "Util/Logger.h"

#include "Util/Timer.h"

#include "Config.h"

void PrintFolder(const std::shared_ptr<Folder>& folder)
{
	std::cout << "~ " << folder->Path << " \n";

	for (const auto& file : folder->Files)
	{
		std::cout << "~~~ " << file->Path << "				" << file->Size << " KB\n";
	}

	for (const auto& f : folder->Folders)
	{
		PrintFolder(f);
	}
}

int main()
{
	//Folder original = FolderLoader::Load("Assets\\Original");
	//Folder compare = FolderLoader::Load("Assets\\Compare");

	Timer timer;

	std::cout << Config::FastComparison << std::endl;

	//compare.Synchronize(original);
	
	LOG_DEBUG("Synchronizing took {0}ms.", timer.Stop());

	getchar();
	return 0;
}*/