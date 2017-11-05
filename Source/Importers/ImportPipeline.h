#pragma once

#include <string>
#include <vector>

class ResourceImporter;

class ImportPipeline
{
public:
    ImportPipeline(const std::string &sourceDirectory, const std::string &outputDirectory);
    ~ImportPipeline();

    // (Re)imports the resource at the given path.
    // The path MUST be relative to the Resources folder (eg Textures/wood_diffuse.png)
    void importResource(const std::string &sourcePath) const;

    // (Re)imports all resources, optionally skipping those that are up to date.
    void importAllResources(bool skipUpToDate = true);

    // Removes imported versions of source files that have been deleted.
    void removeDeletedResources();

private:
    std::string sourceDirectory_;
    std::string outputDirectory_;
    std::vector<ResourceImporter*> importers_;

    // Finds the correct importer for a resource.
    // Returns NULL if no importer is found.
    ResourceImporter* getImporter(const std::string &sourcePath) const;

    // Gets the output path for a source file.
    // The path MUST be relative to the Resources folder (eg Textures/wood_diffuse.png)
    std::string getOutputPath(const std::string &sourcePath) const;
};