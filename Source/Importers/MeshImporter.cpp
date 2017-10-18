#include "MeshImporter.h"

bool MeshImporter::canHandleFileType(const std::string& fileExtension) const
{
	// Support the following file extensions.
	return false;
}

bool MeshImporter::importFile(const std::string& sourceFile, const std::string& outputFile) const
{
	return true;
}
