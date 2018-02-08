#include "PrefabImporter.h"

bool PrefabImporter::importFile(const std::string& sourceFile, const std::string& outputFile) const
{
    // If there is already an output file, delete it.
    if (fs::exists(outputFile))
    {
        fs::remove(outputFile);
    }

    // Now copy the source directly to the output directory.
    fs::copy(sourceFile, outputFile);

    // This importer always succeeds.
    return true;
}