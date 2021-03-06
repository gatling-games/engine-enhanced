#include "SceneImporter.h"

bool SceneImporter::importFile(const std::string& sourceFile, const std::string& outputFile) const
{
    // Now copy the material source directly to the output directory.
    fs::copy(sourceFile, outputFile, fs::copy_options::overwrite_existing);

    // This importer always succeeds.
    return true;
}
