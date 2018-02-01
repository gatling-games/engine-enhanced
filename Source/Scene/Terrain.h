#pragma once

#include "Scene/Component.h"

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

struct TerrainLayer
{
    char name[16] = "Layer";
	Vector2 minMaxHeight = Vector2(0.0f, 10.0f);
    float minHeight = 0.0f;
    float maxHeight = 10.0f;
	Vector2 minMaxAngle = Vector2(0.0f, 90.0f);
    float minAngle = 0.0f;
    float maxAngle = 90.0f;
    Texture* texture = nullptr;
    Texture* normalMap = nullptr;
};

class Terrain : public Component
{
public:
    explicit Terrain(GameObject* gameObject);
    ~Terrain() override { }

    // Draws the properties fold out
    void drawProperties() override;

    Mesh* mesh() const { return mesh_; }
    Texture* heightmap() const { return heightMap_; }
    Texture* texture() const { return baseTexture_; }
    Texture* normalMap() const { return normalMap_; }

    Vector2 textureWrapping() const { return textureWrap_; }
    Vector3 terrainDimensions() const { return dimensions_; }

    TerrainLayer* terrainLayers() { return terrainLayers_; }

private:
    Mesh* mesh_;
    Texture* heightMap_;
    Texture* baseTexture_;
    Texture* normalMap_;

    Vector2 textureWrap_;
    Vector3 dimensions_;

    TerrainLayer terrainLayers_[32];
    int layerCount_;
    
};
