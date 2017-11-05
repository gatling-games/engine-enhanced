#include "ResourceManager.h"

#include <functional>
#include <string>
#include <fstream>
#include <imgui.h>

ResourceManager::ResourceManager(const std::string sourceDirectory, const std::string importedDirectory)
    : importPipeline_(sourceDirectory, importedDirectory),
    resources_()
{
    // Grow the vectors, so there is space for all
    // resources without shifting them about later.
    resources_.resize(256);
}

ResourceManager::~ResourceManager()
{
    // Delete all resources
    for (unsigned int i = 0; i < resources_.size(); ++i)
    {
        resources_[i]->unload();
    }
}

void ResourceManager::drawDebugMenu()
{
    if (ImGui::Button("Import changed"))
    {
        importPipeline_.importAllResources();
    }

    if (ImGui::Button("Import all"))
    {
        importPipeline_.importAllResources(false);
    }

    if (ImGui::Button("Clean Deleted Resources"))
    {
        importPipeline_.removeDeletedResources();
    }
}

ResourceID ResourceManager::pathToResourceID(const std::string &path)
{
    // Return a hash of the path string.
    return std::hash<std::string>{}(path);
}

