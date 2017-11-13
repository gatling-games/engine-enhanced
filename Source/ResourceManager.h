#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

#include "Application.h"

// Resources are identified by a 64 bit hash of their path.
typedef uint64_t ResourceID;

// For now, resources are never moved in memory, so a standard
// pointer is sufficient.
template<typename T>
using ResourcePPtr = T*;

class Resource
{
public:
    explicit Resource(ResourceID id)
        : id_(id)
    {

    }

    // Remove copy and move constructors.
    Resource(const Resource&) = delete;
    Resource(Resource&&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource& operator=(Resource&&) = delete;

    // The unique resource id.
    ResourceID id() const { return id_; }

    // Loading and unloading the processed binary resource file.
    virtual bool load(std::ifstream &file) = 0;
    virtual void unload() = 0;

private:
    ResourceID id_;
};

class ResourceImporter
{
public:
    virtual bool canHandleFileType(const std::string &fileExtension) const = 0;
    virtual bool importFile(const std::string &sourceFile, const std::string &outputFile) const = 0;
};

class ResourceManager : public ApplicationModule
{
public:
    ResourceManager(const std::string sourceDirectory, const std::string importedDirectory);
    ~ResourceManager();

    // ApplicationModule overrides
    std::string name() const override { return "Resource Manager"; }
    void drawDebugMenu() override;

    // Path to directories for resource source files and imported binary files
    std::string sourceDirecory() const { return sourceDirectory_; }
    std::string importedDirectory() const { return importedDirectory_; }

    // Gets a list of all resource source files in the project.
    const std::vector<std::string>* allSourceFiles() const { return &resourceSourcePaths_; }

    // Checks if a specified resource exists in the system.
    bool resourceExists(ResourceID id) const;

    // Checks if a specified resource is currently loaded.
    bool resourceLoaded(ResourceID id) const;

    // Converts a resource source path into the corresponding resource ID.
    ResourceID pathToResourceID(const std::string &sourcePath) const;
   
    // Converts a resource id into the corresponding source path.
    std::string resourceIDToPath(ResourceID id) const;

    // (Re)imports the specified resource.
    void importResource(ResourceID id);

    // (Re)imports all resources, skipping changed ones.
    void importChangedResources();

    // (Re)imports all resources.
    void importAllResources();

    // Removes imported versions of source files that have been deleted.
    void removeDeletedResources();

    // Loads the resource of type T with the given ID.
    template<typename T>
    ResourcePPtr<T> load(ResourceID id)
    {
        // Check all loaded resources for a match.
        for (unsigned int i = 0; i < loadedResources_.size(); ++i)
        {
            if (loadedResources_[i]->id() == id)
            {
                return loadedResources_[i];
            }
        }

        // No existing result. Create a new resource.
        T* r = new T(id);
        loadedResources_.push_back(r);

        // Load the compiled resource file.
        const std::string resourcePath = importedResourcePath(id);
        std::ifstream resourceFile(resourcePath);
        r->load(resourceFile);
        resourceFile.close();
        return r;
    }

    // Loads the resource of type T with the given path
    // The path should be relative to the Resources folder (eg Textures/wood_diffuse.png)
    template<typename T>
    ResourcePPtr<T> load(const std::string &sourcePath)
    {
        return load<T>(pathToResourceID(sourcePath));
    }

private:
    std::string sourceDirectory_;
    std::string importedDirectory_;

    // A list of all resource importers
    std::vector<ResourceImporter*> importers_;

    // Lists of resource ids, source paths and imported paths
    // The same location in each list refers to the same resource
    std::vector<ResourceID> resourceIDs_;
    std::vector<std::string> resourceSourcePaths_;
    std::vector<std::string> resourceImportedPaths_;

    // A list of *currently loaded* resources
    std::vector<Resource*> loadedResources_;

    // Updates the list of all resources in the project.
    void updateResourcesList();

    // Gets the compiled binary file path for a resource
    std::string importedResourcePath(ResourceID id) const;
    std::string importedResourcePath(const std::string &sourcePath) const;

    // Finds the correct importer for a resource.
    ResourceImporter* getImporter(const std::string &sourcePath) const;
};