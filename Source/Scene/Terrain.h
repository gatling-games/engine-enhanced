#pragma once

#include "Scene/Component.h"

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

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
    Texture* texture() const { return baseTexture_; }
	Texture* normalMap() const { return normalMap_; }

	Vector2 textureWrapping() const { return textureWrap_; }
	Vector3 terrainDimensions() const { return dimensions_; }

private:
    Mesh* mesh_;
    Texture* heightmap_;
    Texture* baseTexture_;
	Texture* normalMap_;

	Vector2 textureWrap_;
    Vector3 dimensions_;
	
};