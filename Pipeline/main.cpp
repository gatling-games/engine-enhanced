#include <iostream>
#include <filesystem>

#include "ResourceManager.h"

#include "Importers/TextureImporter.h"
#include "Importers/MeshImporter.h"

#include <chrono>
#include <thread>

using namespace std::experimental::filesystem::v1;

path getProjectRoot(const path &pipelinePath)
{
	// Pipeline.exe is output 3 subdirectories from the proiect root.
	return pipelinePath.parent_path().parent_path().parent_path().parent_path();
}

path getSourceRoot(const path &projectRoot)
{
	// Source resource files are in the Resources directory
	return projectRoot / path("Resources");
}

path getOutputRoot(const path &projectRoot)
{
	// Output to Build/Resources
	// Game exes are in Build/x64Debug/Game/
	return projectRoot / path("Build/Resources");
}

path getFileRelativePath(const path &fileFullPath, const path &sourceRoot)
{
	// Remove the sourceRoot from the full file path
	return path(fileFullPath.c_str() + sourceRoot.string().length() + 1);
}

// Detects resource files and directories in the output directory that no 
// longer exist in the source directory and deletes them.
//
// Returns the number of deleted resources.
int processDeletedResources(const path &sourceRoot, const path& outputRoot, int& removedCount)
{
	// Delete files in the output directory that no longer exist
	for (auto& file : recursive_directory_iterator(outputRoot))
	{
		// Get the relative path and source path
		path filePath(file);
		path fileRelativePath = getFileRelativePath(file, outputRoot);
		path sourcePath = sourceRoot / fileRelativePath;

		// Check if the file no longer exists
		if (exists(sourcePath) == false)
		{
			std::cout << "Removing file " << fileRelativePath << std::endl;
			remove_all(filePath);

			removedCount++;
		}
	}

	return removedCount;
}

ResourceImporter* getImporterForFile(const std::vector<ResourceImporter*> &importers,
	const path& sourceRoot)
{
	// Get the file extension of the source file.
	const std::string extension = sourceRoot.extension().string();

	// Look for an importer that can handle it.
	for (unsigned int i = 0; i < importers.size(); ++i)
	{
		if (importers[i]->canHandleFileType(extension))
		{
			return importers[i];
		}
	}

	// No importer found.
	return nullptr;
}

// Detects resource files that have been added or modified and outputs their
// processed version to the output directory.
//
// Returns the number of processed resources.
void processChangedResources(const std::vector<ResourceImporter*> &importers,
	const path& sourceRoot, const path& outputRoot, int& processedCount, int& upToDateCount, int& errorCount)
{
	for (auto& file : recursive_directory_iterator(sourceRoot))
	{
		path filePath(file);

		// Skip directories
		if (is_directory(filePath))
		{
			continue;
		}

		// Get the relative path and output path
		path fileRelativePath = getFileRelativePath(file, sourceRoot);
		path outputPath = outputRoot / fileRelativePath;

		// Output the file now being processed.
		std::cout << fileRelativePath;

		// Skip files that have already been imported
		// and are up to date.
		if (exists(outputPath) && last_write_time(outputPath) > last_write_time(filePath))
		{
			std::cout << " - already up to date" << std::endl;
			upToDateCount++;
			continue;
		}

		// Look for an importer for the file.
		ResourceImporter* importer = getImporterForFile(importers, fileRelativePath);
		if (importer == nullptr)
		{
			std::cout << " - ERROR: No importer found" << std::endl;
			errorCount++;
			continue;
		}

		// Make sure the output directory exists.
		create_directories(outputPath.parent_path());

		// Trigger the importer
		if (!importer->importFile(filePath.string(), outputPath.string()))
		{
			errorCount++;
			continue;
		}

		// Mark as done
		std::cout << " - done" << std::endl;
		processedCount++;
	}
}

int main(int argc, const char* argv[])
{
	// Determine needed paths
	path pipelinePath(argv[0]);
	path projectRoot = getProjectRoot(pipelinePath);
	path sourceRoot = getSourceRoot(projectRoot);
	path outputRoot = getOutputRoot(projectRoot);

	// Output the paths to aid debugging
	std::cout << "Determining resource locations " << std::endl;
	std::cout << "Pipeline Path: " << pipelinePath.parent_path() << std::endl;
	std::cout << "Project Root: " << projectRoot << std::endl;
	std::cout << "Source Root: " << sourceRoot << std::endl;
	std::cout << "Output Root: " << outputRoot << std::endl;

	// Ensure all directories exist
	create_directories(sourceRoot);
	create_directories(outputRoot);

	// Create resource importer classes
	std::vector<ResourceImporter*> importers;
	importers.push_back(new TextureImporter());
	importers.push_back(new MeshImporter());

	// Keep count of how many files are processed
	int removedCount = 0;
	int processedCount = 0;
	int upToDateCount = 0;
	int errorCount = 0;

	// Remove resource files that no longer exist.
	std::cout << std::endl << "Scanning for resource files that no longer exist" << std::endl;
	processDeletedResources(sourceRoot, outputRoot, removedCount);

	// Import modified and created files in the source directory.
	std::cout << std::endl << "Scanning for resource files that have been changed" << std::endl;
	processChangedResources(importers, sourceRoot, outputRoot, processedCount, upToDateCount, errorCount);

	// Output finished message
	std::cout << std::endl << "Resource processing finished: ";
	std::cout << processedCount << " succeeded, ";
	std::cout << errorCount << " failed, ";
	std::cout << upToDateCount << " up to date, ";
	std::cout << removedCount << " removed." << std::endl;

	int i;
	std::cin >> i;
}