#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Application.h"
#include "Importers\ImportPipeline.h"

class Mesh;
class Texture;

typedef uint64_t ResourceID;

template <class SettingsT>
class Resource
{
public:
    virtual bool Load(const SettingsT* settings, std::ifstream &file) = 0;
    virtual void Unload() = 0;
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

    // Determines the resource id for a given source path.
    // The path should be relative to the Resources folder (eg Textures/wood_diffuse.png)
    static ResourceID pathToResourceID(const std::string &path);

private:
    // The pipeline used for (re)importing resources
    ImportPipeline importPipeline_;

    // A list of all resources, both loaded and unloaded
    std::vector<Resource*> resources_;
};