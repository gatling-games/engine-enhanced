#pragma once

#include "ResourceManager.h"

class TextureImporter : public ResourceImporter
{
public:
	bool importFile(const std::string &sourceFile, const std::string &outputFile) const override;
};