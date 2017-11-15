#include "ShaderImporter.h"
#include <filesystem>

namespace fs = std::experimental::filesystem::v1;

bool ShaderImporter::canHandleFileType(const std::string& fileExtension) const
{
    // Support the following file extensions.
    const int numTypes = 2;
    const std::string supportedTypes[numTypes] =  {".shader", ".shaderinclude"};

    // Check if the extension is supported.
    for (int i = 0; i < numTypes; ++i)
    {
        if (supportedTypes[i] == fileExtension)
        {
            return true;
        }
    }

    // Not supported.
    return false;
}

bool ShaderImporter::importFile(const std::string &sourceFile, const std::string &outputFile) const
{
    fs::copy(sourceFile, outputFile);
    return true;
}