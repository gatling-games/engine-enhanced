#pragma once

#include "Scene/Component.h"

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Math/Vector2.h"

struct TerrainLayer
{
    std::string name;
    float minHeight;
    float maxHeight;
    float minAngle;
    float maxAngle;
};

class Terrain : public Component
{
public:
    explicit Terrain(GameObject* gameObject);
    ~Terrain() override { }

    // Draws the properties fold out
    void drawProperties() override;

    Mesh* mesh() const { return mesh_; }
    Texture* heightmap() const { return heightmap_; }
    Texture* texture() const { return texture_; }
	Vector2 terrainDimensions() const { return dimensions_;; }
	float scale() const { return terrainScale_; }

private:
    Mesh* mesh_;
    Texture* heightmap_;
    Texture* texture_;
    Vector2 dimensions_;
	float terrainScale_;
};