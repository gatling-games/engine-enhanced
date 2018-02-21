#pragma once

#include <assert.h>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

#include "Utils/Singleton.h"

// Resources are identified by a 64 bit hash of their path.
typedef uint64_t ResourceID;

// For now, resources are never moved in memory, so a standard
// pointer is sufficient.
template<typename T>
using ResourcePPtr = T*;

class Resource
{
public:
    // Identifies a resource that is created at runtime
    // but is not saved to disk.
    const static int NOT_SAVED_RESOURCE = 0;

public:
    explicit Resource(ResourceID id)
        : id_(id)
    {

    }

    virtual ~Resource() = default;

    // Remove copy and move constructors.
    Resource(const Resource&) = delete;
    Resource(Resource&&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource& operator=(Resource&&) = delete;

    // The unique resource id.
    ResourceID resourceID() const { return id_; }

    // The name of the resource file.
    std::string resourceName() const;

    // The path to the resource file, from the resources directory root.
    std::string resourcePath() const;

    // Loading and unloading the processed binary resource file.
    virtual void load(std::ifstream &) { };
    virtual void unload() { };

private:
    ResourceID id_;
};

class ResourceImporter
{
public:
    virtual ~ResourceImporter() { }
    virtual bool importFile(const std::string &sourceFile, const std::string &outputFile) const = 0;
};

// A function used for instantiating a resource of a particular type.
typedef std::function<Resource*(ResourceID)> ResourceInstantiationFunc;

// A struct storing information for a registered resource type.
// It allows resources to be routed to the correct importer and lets
// resources be instantiated at runtime.
struct ResourceType
{
    // The file extension used by source files.
    std::string fileExtension;

    // The importer used for the resource
    ResourceImporter* importer;

    // A function that instantiates a new instance of the resource.
    ResourceInstantiationFunc instantiationFunction;
};

class ResourceManager : public Singleton<ResourceManager>
{
public:
    ResourceManager(const std::string sourceDirectory, const std::string importedDirectory);
    ~ResourceManager();

    // Gets the path to the source resources directory.
    const std::string sourceDirectory() const { return sourceDirectory_; }

    // Gets a list of all resource source files in the project.
    const std::vector<std::string>* allSourceFiles() const { return &resourceSourcePaths_; }

    // Functions for converting to and from resource ids
    ResourceID pathToResourceID(const std::string &sourcePath) const;
    std::string resourceIDToPath(ResourceID id) const;

    // Loads the resource with the given id
    Resource* load(ResourceID id);

    // Loads a resource of the given type and the given ID.
    template<typename T>
    ResourcePPtr<T> load(ResourceID id)
    {
        return dynamic_cast<ResourcePPtr<T>>(load(id));
    }

    // The path should be relative to the Resources folder (eg Textures/wood_diffuse.png)
    template<typename T>
    ResourcePPtr<T> load(const std::string &sourcePath)
    {
        return load<T>(pathToResourceID(sourcePath));
    }

    // Creates a new resource of the given type and writes it to disk at the given path.
    // If a resource already exists at the path, it will be overwritten.
    template<typename T>
    ResourcePPtr<T> createResource(const std::string &sourcePath)
    {
        // Create a blank resource file.
        std::ofstream fileStream(sourcePath);
        fileStream << "{\n}";
        fileStream.close();

        // Cause the path file to be imported
        importChangedResources();

        // Load and return the now blank resource
        return load<T>(sourcePath);
    }

    // Saves all resources whose source files have been modified.
    // This affects all ISerializedObject-based resources
    void saveAllSourceFiles();

    // (Re)imports the specified resource.
    void importResource(ResourceID id);

    // (Re)imports all resources, skipping changed ones.
    void importChangedResources();

    // (Re)imports all resources.
    void importAllResources();

    // Removes imported versions of source files that have been deleted.
    void removeDeletedResources();

    // Returns a list of all the resources of type T that are currently loaded.
    template<typename T>
    std::vector<T*> loadedResourcesOfType() const
    {
        // Attempt to convert every resource to an instance of T.
        std::vector<T*> results;
        for (Resource* resource : loadedResources_)
        {
            if (dynamic_cast<T*>(resource) != nullptr)
            {
                results.push_back(dynamic_cast<T*>(resource));
            }
        }

        return results;
    }

    // Called frequently on the main thread to finish resource loading jobs
    void update();

private:
    std::string sourceDirectory_;
    std::string importedDirectory_;

    // A mutex used when accessing the list of resources and import queue.
    std::recursive_mutex resourceListsMutex_;

    // A list of all registered resource types.
    std::vector<ResourceType> typeRegister_;

    // Lists of resource ids, source paths and imported paths
    // The same location in each list refers to the same resource
    std::vector<ResourceID> resourceIDs_;
    std::vector<std::string> resourceSourcePaths_;
    std::vector<std::string> resourceImportedPaths_;

    // A list of *currently loaded* resources
    std::vector<Resource*> loadedResources_;

    // A queue of resources waiting to be imported or reimported.
    std::queue<ResourceID> importQueue_;

    // A queue of resources waiting to be loaded or reloaded
    std::queue<ResourceID> loadQueue_;

    // A thread running background resource imports
    bool importThreadRunning_;
    std::thread importThread_;

    // Registers a menu item for creating new instances of a resource type
    template<typename ResourceT>
    void registerResourceCreateMenuItem(const std::string &resourceName, const std::string &fileExtension)
    {
        MainWindowMenu::instance()->addMenuItem("Resources/Create Resource/" + resourceName, [=] {
            const std::string savePath = EditorManager::instance()->showSaveDialog("New " + resourceName, resourceName, fileExtension);
            if (savePath.empty() == false)
            {
                PropertiesPanel::instance()->inspect(createResource<ResourceT>(savePath));
            }
        });
    }

    // Registers a resource importer for handling a particular resource type.
    template<typename ResourceT, typename ImporterT>
    void registerResourceType(const std::string &fileExtension)
    {
        // Gather the importer info and add to the list
        ResourceType data;
        data.fileExtension = fileExtension;
        data.importer = new ImporterT();
        data.instantiationFunction = [](ResourceID id) { return new ResourceT(id); };
        typeRegister_.push_back(data);
    }

    // Executes the steps of the resource loading process
    void executeFilesystemScan();
    void executeResourceImport(ResourceID id);
    void executeResourceLoad(ResourceID id);

    // Executes queued import/load steps
    void emptyImportQueue();
    void emptyLoadQueue();

    // Repeatedly clears the import queue on a background thread.
    void runImportThread();

    // Unloads and reloads the resource with the given id, if it is currently loaded.
    // Used for hot-reloading of resources when the change at runtime.
    void reloadResourceIfLoaded(ResourceID id);

    // Gets the compiled binary file path for a resource
    std::string importedResourcePath(ResourceID id) const;
    std::string importedResourcePath(const std::string &sourcePath) const;

    // Finds the correct importer for a resource.
    ResourceImporter* getImporter(const std::string &sourcePath) const;

    // Finds the correct instantiation func for a resource at the given path.
    ResourceInstantiationFunc getInstantiationFunc(const std::string &sourcePath) const;
};