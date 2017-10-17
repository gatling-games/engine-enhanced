#pragma once

#include "ResourceManager.h"

struct TextureSettings;

class TextureImporter : public ResourceImporter<TextureSettings>
{
public:
	virtual bool import(TextureSettings* settings, File &sourceFile, File &processedFile) override;
};