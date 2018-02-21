#pragma once

#include "ResourceManager.h"

class PrefabImporter : public ResourceImporter
{
public:
    bool importFile(const std::string &sourceFile, const std::string &outputFile) const override;
};