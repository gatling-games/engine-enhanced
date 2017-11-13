#pragma once

#include "ResourceManager.h"

#include <GL/gl3w.h>

struct MeshSettings
{
	int vertexCount;
	int elementsCount;
	bool hasNormals;
	bool hasTangents;
	bool hasTexcoords;
};

typedef unsigned short MeshElementIndex;

class Mesh : public Resource
{
private:
    // Vertex attribute buffers
    const static int PositionsBuffer = 0;
    const static int NormalsBuffer = 1;
    const static int TangentsBuffer = 2;
    const static int TexcoordsBuffer = 3;
    const static int AttributeBufferCount = 4; // Must keep up to date.

public:
    explicit Mesh(ResourceID id);
    ~Mesh();

    // Handles resource loading and unloading
    void load(std::ifstream &file) override;
	void unload() override;

    // Basic mesh information
    int vertexCount() const { return settings_.vertexCount; }
    int elementsCount() const { return settings_.elementsCount; }
    bool hasNormals() const { return settings_.hasNormals; }
    bool hasTangents() const { return settings_.hasTangents; }
    bool hasTexcoords() const { return settings_.hasTexcoords; }

    // Attaches the vbo and elements buffer for use.
    void bind() const;

private:
    bool loaded_;
    MeshSettings settings_;
    GLuint vertexArray_;
    GLuint attributeBuffers_[AttributeBufferCount];
    GLuint elementsBuffer_;
};