#pragma once

#include <cstdint>
#include <string>

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

class ResourceManager
{
public:

    // Determines the resource id for a given source path.
    // The path MUST be relative to the Resources folder (eg Textures/wood_diffuse.png)
    static ResourceID pathToResourceID(const std::string &path);
};