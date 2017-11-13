#include "ShaderImporter.h"

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