#pragma once

#include "ResourceManager.h"

class MeshImporter : public ResourceImporter
{
public:
	bool canHandleFileType(const std::string &fileExtension) const;
	bool importFile(const std::string &sourceFile, const std::string &outputFile) const override;
};