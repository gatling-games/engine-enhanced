#include "ShaderImporter.h"
#include <filesystem>

namespace fs = std::experimental::filesystem::v1;

bool ShaderImporter::importFile(const std::string &sourceFile, const std::string &outputFile) const
{
    // Copy the shader source directly to the output directory.
    // The source code is passed to opengl at run time.
    fs::copy(sourceFile, outputFile, fs::copy_options::overwrite_existing);

    // This importer always succeeds.
    return true;
}