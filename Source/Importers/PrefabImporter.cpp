#include "PrefabImporter.h"

bool PrefabImporter::importFile(const std::string& sourceFile, const std::string& outputFile) const
{
    // Copy the source directly to the output directory.
    fs::copy(sourceFile, outputFile, fs::copy_options::overwrite_existing);

    // This importer always succeeds.
    return true;
}