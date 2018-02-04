#include "ResourceManager.h"

#include <functional>
#include <string>
#include <fstream>
#include <fstream>

#include "Editor/MainWindowMenu.h"

#include "Serialization/SerializedObject.h"

#include "Importers/MaterialImporter.h"
#include "Importers/MeshImporter.h"
#include "Importers/TextureImporter.h"
#include "Importers/ShaderImporter.h"

#include "Renderer/Material.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

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
    resourceIDs_(),
    resourceSourcePaths_(),
    resourceImportedPaths_(),
    loadedResources_()
{
    // Register each supported resource type, file extension, and importer
    registerResourceType<Mesh, MeshImporter>(".obj");
    registerResourceType<Mesh, MeshImporter>(".mesh");
    registerResourceType<ShaderInclude, ShaderImporter>(".inc.shader");
    registerResourceType<Shader, ShaderImporter>(".shader");
    registerResourceType<Texture, TextureImporter>(".psd");
    registerResourceType<Texture, TextureImporter>(".png");
    registerResourceType<Texture, TextureImporter>(".dds");
    registerResourceType<Texture, TextureImporter>(".tga");
    registerResourceType<Texture, TextureImporter>(".bmp");
    registerResourceType<Texture, TextureImporter>(".jpg");
    registerResourceType<Texture, TextureImporter>(".jpeg");
    registerResourceType<Material, MaterialImporter>(".material");

    // Grow the loaded resources vector, so there is space for all
    // resources without shifting them about later.
    loadedResources_.reserve(256);

    // Scan the resources directory for changed files.
    executeFilesystemScan();

    // Import any resources that need it
    emptyImportQueue();

    // Ensure all resources are loaded
    for (ResourceID id : resourceIDs_)
    {
        executeResourceLoad(id);
    }
    emptyLoadQueue();

    // Start up the background thread
    importThreadRunning_ = true;
    importThread_ = std::thread(&ResourceManager::runImportThread, this);

    // Create menu items for controlling the resource manager
    MainWindowMenu::instance()->addMenuItem("File/Save All", [&] { saveAllSourceFiles(); });
    MainWindowMenu::instance()->addMenuItem("Resources/Scan For Changes", [&] { executeFilesystemScan(); });
    MainWindowMenu::instance()->addMenuItem("Resources/Reimport All", [&] { importAllResources(); });
}

ResourceManager::~ResourceManager()
{
    // Close the import thread
    importThreadRunning_ = false;
    importThread_.join();

    // Ensure all changes to source files are saved to disk.
    saveAllSourceFiles();

    // Delete all importers
    for (unsigned int i = 0; i < typeRegister_.size(); ++i)
    {
        delete typeRegister_[i].importer;
    }

    // Unload all loaded resources
    for (unsigned int i = 0; i < loadedResources_.size(); ++i)
    {
        loadedResources_[i]->unload();
        delete loadedResources_[i];
    }
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
    throw;
}

Resource* ResourceManager::load(ResourceID id)
{
    std::lock_guard<std::recursive_mutex> gate(resourceListsMutex_);

    // Check the list of resources for a match.
    for (Resource* loadedResource : loadedResources_)
    {
        if (loadedResource->resourceID() == id)
        {
            return loadedResource;
        }
    }

    // No resource with the requested ID exists.
    // Check the list of resource ids to see if there is a matching resource
    // that just hasnt been loaded yet.
    if(std::find(resourceIDs_.begin(), resourceIDs_.end(), id) != resourceIDs_.end())
    {
        // Load the resource and then try again
        executeResourceLoad(id);
        return load(id);
    }

    // No resource exists.
    return nullptr;
}

void ResourceManager::saveAllSourceFiles()
{
    std::lock_guard<std::recursive_mutex> gate(resourceListsMutex_);

    // Test every loaded resource
    for (Resource* resource : loadedResources_)
    {
        ISerializedObject* iso = dynamic_cast<ISerializedObject*>(resource);
        if(iso != nullptr)
        {
            // Serialize the object to a propertytable
            PropertyTable properties(PropertyTableMode::Writing);
            iso->serialize(properties);

            // Get the serialized string
            const std::string serializedData = properties.toString();

            // Write the string to the source file, overwriting old versions.
            std::ofstream writeStream(resource->resourcePath());
            writeStream << serializedData;
            writeStream.close();
        }
    }
}

void ResourceManager::importResource(ResourceID id)
{
    std::lock_guard<std::recursive_mutex> gate(resourceListsMutex_);
    importQueue_.emplace(id);
}

void ResourceManager::importChangedResources()
{
    // Ensure the resources list is up to date
    // This will trigger imports of resources that need it
    executeFilesystemScan();

    // Then remove unneeded files from the imported folder
    removeDeletedResources();
}

void ResourceManager::importAllResources()
{
    // Ensure the resources list is up to date
    executeFilesystemScan();

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
    executeFilesystemScan();

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

void ResourceManager::update()
{
    saveAllSourceFiles();
    emptyLoadQueue();
}

void ResourceManager::executeFilesystemScan()
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
            const std::string sourcePath = fs::path(file).string();
            ResourceID id = pathToResourceID(sourcePath);
            const std::string importedPath = importedResourcePath(sourcePath);

            // Save to the resource lists
            resourceIDs_.push_back(id);
            resourceSourcePaths_.push_back(sourcePath);
            resourceImportedPaths_.push_back(importedPath);
        }
    }

    // Check for resources that need to be imported or reimported
    for (unsigned int i = 0; i < resourceIDs_.size(); ++i)
    {
        const std::string& sourcePath = resourceSourcePaths_[i];
        const std::string& outputPath = resourceImportedPaths_[i];

        // Import resources that are not yet imported, or are out of date
        if (!exists(fs::path(outputPath)) || last_write_time(fs::path(outputPath)) < last_write_time(fs::path(sourcePath)))
        {
            importResource(resourceIDs_[i]);
        }
    }
}

void ResourceManager::executeResourceImport(ResourceID id)
{
    printf("Executing resource import for resource %llu \n", id);

    // Get the source and imported path
    const std::string sourcePath = resourceIDToPath(id);
    const std::string outputPath = importedResourcePath(sourcePath);

    // Make sure the output directory exists.
    create_directories(fs::path(outputPath).parent_path());

    // Look for an importer
    ResourceImporter* importer = getImporter(sourcePath);
    if(importer == nullptr)
    {
        return;
    }

    // Trigger the importer
    if (!importer->importFile(sourcePath, outputPath))
    {
        printf("Failed to import resource \n");
        return;
    }

    // Finally, enqueue the resource to be loaded or reloaded.
    std::lock_guard<std::recursive_mutex> gate(resourceListsMutex_);
    loadQueue_.emplace(id);

    printf("Resource import finished \n");
}

void ResourceManager::executeResourceLoad(ResourceID id)
{
    printf("Executing resource load for id %llu \n", id);

    // Check if a matching resource is already loaded.
    Resource* resource = nullptr;
    for (Resource* r : loadedResources_)
    {
        if (r->resourceID() == id)
        {
            resource = r;
        }
    }

    if (resource == nullptr)
    {
        // No resource exists.
        // Create a new resource using the correct instantiation function.
        const std::string sourcePath = resourceIDToPath(id);
        auto instantiationFunc = getInstantiationFunc(sourcePath);
        if(instantiationFunc == nullptr)
        {
            return;
        }

        resource = getInstantiationFunc(sourcePath)(id);
        loadedResources_.push_back(resource);
    }
    else
    {
        // The resource already exists.
        // Unload the current data.
        resource->unload();
    }

    // Open the imported file and run load
    const std::string importedPath = importedResourcePath(id);
    std::ifstream importedFileStream(importedPath, std::iostream::binary);

    // Some resources implement SerializedObject and need to be given the file as a propertytable.
    ISerializedObject* iso = dynamic_cast<ISerializedObject*>(resource);
    if (iso != nullptr)
    {
        std::stringstream stringstream;
        stringstream << importedFileStream.rdbuf();
        PropertyTable properties(PropertyTableMode::Reading);
        properties.addPropertyData(stringstream);
        iso->serialize(properties);
    }
    else
    {
        // Non-serializableobject resources are just given the file stream.
        resource->load(importedFileStream);
    }
}

void ResourceManager::emptyImportQueue()
{
    for (;;)
    {
        resourceListsMutex_.lock();

        // Keep going until the queue is empty
        if (importQueue_.empty())
        {
            resourceListsMutex_.unlock();
            return;
        };

        // Get the next item 
        const ResourceID id = importQueue_.front();
        importQueue_.pop();

        // Import, without blocking other threads.
        resourceListsMutex_.unlock();
        printf("Importing resource %s \n", resourceIDToPath(id).c_str());
        executeResourceImport(id);
    }
}

void ResourceManager::emptyLoadQueue()
{
    std::lock_guard<std::recursive_mutex> gate(resourceListsMutex_);

    while (!loadQueue_.empty())
    {
        executeResourceLoad(loadQueue_.front());
        loadQueue_.pop();
    }
}

void ResourceManager::runImportThread()
{
    while(importThreadRunning_)
    {
        Sleep(500);
        emptyImportQueue();
    }
}

void ResourceManager::reloadResourceIfLoaded(ResourceID id)
{
    std::lock_guard<std::recursive_mutex> gate(resourceListsMutex_);
    loadQueue_.emplace(id);
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
    // Look for a resource type matching the file extension
    for (const ResourceType& type : typeRegister_)
    {
        if (type.fileExtension == sourcePath.substr(sourcePath.length() - type.fileExtension.length()))
        {
            return type.importer;
        }
    }

    // No importer found.
    printf("No importer found for %s \n", sourcePath.c_str());
    return nullptr;
}

ResourceInstantiationFunc ResourceManager::getInstantiationFunc(const std::string& sourcePath) const
{
    // Look for a resource type matching the file extension
    for (const ResourceType& type : typeRegister_)
    {
        if (type.fileExtension == sourcePath.substr(sourcePath.length() - type.fileExtension.length()))
        {
            return type.instantiationFunction;
        }
    }

    // No function found.
    printf("No instantiation function found for %s \n", sourcePath.c_str());
    return nullptr;
}
