#include "ImportPipeline.h"

#include "ResourceManager.h"
#include "MeshImporter.h"
#include "TextureImporter.h"

#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

ImportPipeline::ImportPipeline(const std::string &sourceDirectory, const std::string &outputDirectory)
    : sourceDirectory_(sourceDirectory),
    outputDirectory_(outputDirectory),
    importers_()
{
    // Register importers with the pipeline.
    // They will be used automatically for resource formats they support.
    importers_.push_back(new MeshImporter());
    importers_.push_back(new TextureImporter());

    // Ensure the source and output directories exist.
    create_directories(fs::path(sourceDirectory_));
    create_directories(fs::path(outputDirectory_));
}

ImportPipeline::~ImportPipeline()
{
    for (unsigned int i = 0; i < importers_.size(); ++i)
    {
        delete importers_[i];
    }
}

void ImportPipeline::importResource(const std::string &sourcePath) const
{
    printf("%s - ", sourcePath.c_str());

    // Look for an importer for the file.
    ResourceImporter* importer = getImporter(sourcePath);
    if (importer == nullptr)
    {
        printf("ERROR: No importer found \n");
        return;
    }

    // Make sure the output directory exists.
    std::string outputPath = getOutputPath(sourcePath);
    create_directories(fs::path(outputPath).parent_path());

    // Trigger the importer
    if (!importer->importFile(sourcePath, outputPath))
    {
        // Error
        return;
    }

    // Print a finished message
    printf("Done \n");
}

void ImportPipeline::importAllResources(bool skipUpToDate)
{
    // Loop through every file in the source directory
    for (auto& file : fs::recursive_directory_iterator(sourceDirectory_))
    {
        // Skip directories
        if (is_directory(file))
        {
            continue;
        }

        std::string sourcePath = fs::path(file).string();

        // Skip files that have already been imported and are up to date.
        fs::path outputPath = getOutputPath(sourcePath);
        if (skipUpToDate && exists(outputPath) && last_write_time(outputPath) > last_write_time(file))
        {
            continue;
        }

        // Import the resource file.
        importResource(sourcePath);
    }
}

void ImportPipeline::removeDeletedResources()
{
    // Build a list of valid output paths
    std::vector<fs::path> validPaths;
    for (auto& file : fs::recursive_directory_iterator(sourceDirectory_))
    {
        if (!is_directory(file))
        {
            validPaths.push_back(getOutputPath(fs::path(file).string()));
        }
    }

    // Check every file in the output folder and delete unexpected ones.
    for (auto& file : fs::recursive_directory_iterator(outputDirectory_))
    {
        // Get the file path
        const fs::path filePath(file);

        // Check if it exists in the list of valid paths
        if (find(validPaths.begin(), validPaths.end(), filePath) == validPaths.end())
        {
            printf("Removing %s \n", filePath.string().c_str());
            remove_all(filePath);
        }
    }
}

ResourceImporter* ImportPipeline::getImporter(const std::string &sourcePath) const
{
    // Get the file extension of the source file.
    const std::string extension = fs::path(sourcePath).extension().string();

    // Look for an importer that can handle it.
    for (unsigned int i = 0; i < importers_.size(); ++i)
    {
        if (importers_[i]->canHandleFileType(extension))
        {
            return importers_[i];
        }
    }

    // No importer found.
    return nullptr;
}

std::string ImportPipeline::getOutputPath(const std::string &sourcePath) const
{
    // Remove the root path, if the source path begins with it
    std::string relativeSourcePath = sourcePath;
    if (sourcePath.compare(0, sourceDirectory_.length(), sourceDirectory_))
    {
        relativeSourcePath = std::string(sourcePath.c_str() + sourceDirectory_.length());
    }

    // Hash the source path.
    const ResourceID resourceID = ResourceManager::pathToResourceID(relativeSourcePath);

    // Construct the output path
    return (fs::path(outputDirectory_) / (std::to_string(resourceID) + ".resource")).string();
}