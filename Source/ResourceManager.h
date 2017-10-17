#pragma once

#include <cstdint>

typedef uint64_t ResourceID;

class File
{
public:
	int sizeBytes();
	void read(uint8_t* dst, int bytes);
};

template <class SettingsT>
class Resource
{
public:
	virtual bool Load(const SettingsT* settings, File &file) = 0;
	virtual void Unload() = 0;
};

template <class SettingsT>
class ResourceImporter
{
public:
	virtual bool import(SettingsT* settings, File &sourceFile, File &outputFile) = 0;
};

class ResourceManager
{

};