#pragma once

#include "ResourceManager.h"

struct MeshSettings
{
	int vertexCount;
	int elementsCount;
	bool hasNormals;
	bool hasTangents;
	bool hasTexcoords;
};

//typedef MeshElementIndex unsigned short;

class Mesh : public Resource<MeshSettings>
{
public:
	Mesh();
	~Mesh();

	bool load(const MeshSettings* settings, std::ifstream &file);
	void unload();
};