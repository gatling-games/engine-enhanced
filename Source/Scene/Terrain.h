#pragma once

#include "Scene/Component.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

class Material;

struct TerrainLayer
{
    char name[16] = "Layer";
	Vector2 minMaxHeight = Vector2(0.0f, 100.0f);
	Vector2 minMaxAngle = Vector2(0.0f, 90.0f);
    Material* material;
};

class Terrain : public Component
{
    
public:
    const static int MAX_LAYERS = 32;

    explicit Terrain(GameObject* gameObject);
    ~Terrain() override { }

    // Draws the properties fold out
    void drawProperties() override;

    Mesh* mesh() const { return mesh_; }
    Texture* heightmap() const { return heightMap_; }

    Vector2 textureWrapping() const { return textureWrap_; }
    Vector3 terrainDimensions() const { return dimensions_; }

    TerrainLayer* terrainLayers() { return terrainLayers_; }
    int layerCount() const { return layerCount_; }

private:
    Mesh* mesh_;
    Texture* heightMap_;

    Vector2 textureWrap_;
    Vector3 dimensions_;

    TerrainLayer terrainLayers_[MAX_LAYERS];
    int layerCount_;
    
};
