#include "ShaderImporter.h"
#include <filesystem>

namespace fs = std::experimental::filesystem::v1;

bool ShaderImporter::importFile(const std::string &sourceFile, const std::string &outputFile) const
{
    // If there is already an output file, delete it.
    if (fs::exists(outputFile))
    {
        fs::remove(outputFile);
    }

    // Now copy the shader source directly to the output directory.
    // The source code is passed to opengl at run time.
    fs::copy(sourceFile, outputFile);

    // This importer always succeeds.
    return true;
}