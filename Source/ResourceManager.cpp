#include "ResourceManager.h"

#include <functional>
#include <string>
#include <fstream>
#include <imgui.h>

#include "Importers/MeshImporter.h"
#include "Importers/TextureImporter.h"
#include "Importers/ShaderImporter.h"

#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

std::string Resource::resourceName() const
{
    return fs::path(resourcePath()).filename().string();
}

std::string Resource::resourcePath() const
{
    return ResourceManager::instance()->resourceIDToPath(id_);
}

ResourceManager::ResourceManager(const std::string sourceDirectory, const std::string importedDirectory)
    : sourceDirectory_(sourceDirectory),
    importedDirectory_(importedDirectory),
    importers_(),
    resourceIDs_(),
    resourceSourcePaths_(),
    resourceImportedPaths_(),
    loadedResources_()
{
    // Create the importer instances
    importers_.push_back(new MeshImporter());
    importers_.push_back(new TextureImporter());
    importers_.push_back(new ShaderImporter());

    // Grow the loaded resources vector, so there is space for all
    // resources without shifting them about later.
    loadedResources_.reserve(256);

    // Scan the resources directory for changed files.
    importChangedResources();
}

ResourceManager::~ResourceManager()
{
    // Delete all importers
    for (unsigned int i = 0; i < importers_.size(); ++i)
    {
        delete importers_[i];
    }

    // Unload all loaded resources
    for (unsigned int i = 0; i < loadedResources_.size(); ++i)
    {
        loadedResources_[i]->unload();
        delete loadedResources_[i];
    }
}

void ResourceManager::drawDebugMenu()
{
    if (ImGui::Button("Import changed"))
    {
        importChangedResources();
    }

    if (ImGui::Button("Import all"))
    {
        importAllResources();
    }

    if (ImGui::Button("Clean Deleted Resources"))
    {
        removeDeletedResources();
    }

    for (unsigned int i = 0; i < resourceSourcePaths_.size(); ++i)
    {
        ImGui::Text("%llu", resourceIDs_[i]);
        ImGui::Text(resourceSourcePaths_[i].c_str());
        ImGui::Text(resourceImportedPaths_[i].c_str());
    }
}

bool ResourceManager::resourceExists(ResourceID id) const
{
    // Check if the resource is in the list of all ids
    for (unsigned int i = 0; i < resourceIDs_.size(); ++i)
    {
        if (resourceIDs_[i] == id)
        {
            return true;
        }
    }

    // Id not found
    return false;
}

bool ResourceManager::resourceLoaded(ResourceID id) const
{
    // Check if the resource is in the list of loaded resources
    for (unsigned int i = 0; i < loadedResources_.size(); ++i)
    {
        if (loadedResources_[i]->resourceID() == id)
        {
            return true;
        }
    }

    // Not found
    return false;
}

ResourceID ResourceManager::pathToResourceID(const std::string &sourcePath) const
{
    // Return a hash of the path string.
    return std::hash<std::string>{}(fs::path(sourcePath).string());
}

std::string ResourceManager::resourceIDToPath(ResourceID id) const
{
    // Check if the resource id is in the list of all resources.
    for (unsigned int i = 0; i < resourceIDs_.size(); ++i)
    {
        if (resourceIDs_[i] == id)
        {
            return resourceSourcePaths_[i];
        }
    }

    // Id not found
    printf("Could not find path for resource %llu \n", id);
    return "";
}

void ResourceManager::importResource(ResourceID id)
{
    // Get the source and imported path
    std::string sourcePath = resourceIDToPath(id);
    std::string outputPath = importedResourcePath(sourcePath);

    printf("Importing %s - ", sourcePath.c_str());

    // Look for an importer for the file.
    ResourceImporter* importer = getImporter(sourcePath);
    if (importer == nullptr)
    {
        printf("ERROR: No importer found \n");
        return;
    }

    // Make sure the output directory exists.
    create_directories(fs::path(outputPath).parent_path());

    // Trigger the importer
    if (!importer->importFile(sourcePath, outputPath))
    {
        // Error
        return;
    }

    // Finally, reload the asset if it is loaded.
    reloadResourceIfLoaded(id);

    // Print a finished message
    printf("Done \n");
}

void ResourceManager::importChangedResources()
{
    // Ensure the resources list is up to date
    updateResourcesList();

    // Loop through every resource in the resources list
    for (unsigned int i = 0; i < resourceIDs_.size(); ++i)
    {
        // Skip files that have already been imported and are up to date.
        std::string sourcePath = resourceSourcePaths_[i];
        std::string outputPath = resourceImportedPaths_[i];
        if (exists(fs::path(outputPath)) && last_write_time(fs::path(outputPath)) > last_write_time(fs::path(sourcePath)))
        {
            // Already up to date
            continue;
        }

        // Import the resource file.
        importResource(resourceIDs_[i]);
    }

    // Finally, remove unneeded files from the imported folder
    removeDeletedResources();
}

void ResourceManager::importAllResources()
{
    // Ensure the resources list is up to date
    updateResourcesList();

    // Import every resource in the resources list
    for (unsigned int i = 0; i < resourceIDs_.size(); ++i)
    {
        importResource(resourceIDs_[i]);
    }

    // Finally, remove unneeded files from the imported folder
    removeDeletedResources();
}

void ResourceManager::removeDeletedResources()
{
    // Rebuild a list of valid output paths
    updateResourcesList();

    // Check every file in the output folder and delete unexpected ones.
    for (auto& file : fs::recursive_directory_iterator(importedDirectory_))
    {
        // Skip directories
        if (is_directory(file))
        {
            continue;
        }

        // Get the file path
        const fs::path filePath(file);

        // Remove the file if it does not exist in the imported paths list.
        if (find(resourceImportedPaths_.begin(), resourceImportedPaths_.end(), filePath.string()) == resourceImportedPaths_.end())
        {
            printf("Removing %s \n", filePath.string().c_str());
            remove_all(filePath);
        }
    }
}

void ResourceManager::updateResourcesList()
{
    // Recreate the lists from scratch
    resourceIDs_.clear();
    resourceSourcePaths_.clear();
    resourceImportedPaths_.clear();

    // Loop through every file in the source directory add add them to the lists
    for (auto& file : fs::recursive_directory_iterator(sourceDirectory_))
    {
        if (!is_directory(file))
        {
            // Get the resource id and imported path from the source paht
            std::string sourcePath = fs::path(file).string();
            ResourceID id = pathToResourceID(sourcePath);
            std::string importedPath = importedResourcePath(sourcePath);

            // Save to the resource lists
            resourceIDs_.push_back(id);
            resourceSourcePaths_.push_back(sourcePath);
            resourceImportedPaths_.push_back(importedPath);
        }
    }
}

void ResourceManager::reloadResourceIfLoaded(ResourceID id)
{
    // Check if the resource is in the list of loaded resources.
    for (unsigned int i = 0; i < loadedResources_.size(); ++i)
    {
        if (loadedResources_[i]->resourceID() == id)
        {
            // If the resource is found, first unload it.
            loadedResources_[i]->unload();

            // Now find the imported path and reload the resource.
            const std::string importedPath = importedResourcePath(id);

            // Finally, load the resource again.
            std::ifstream importedFile(importedPath, std::ifstream::binary);
            loadedResources_[i]->load(importedFile);
        }
    }
}

std::string ResourceManager::importedResourcePath(ResourceID id) const
{
    return (fs::path(importedDirectory_) / (std::to_string(id) + ".resource")).string();
}

std::string ResourceManager::importedResourcePath(const std::string &sourcePath) const
{
    return importedResourcePath(pathToResourceID(sourcePath));
}

ResourceImporter* ResourceManager::getImporter(const std::string &sourcePath) const
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