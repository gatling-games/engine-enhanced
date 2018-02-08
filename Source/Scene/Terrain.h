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
    float altitudeBorder = 0.0f;
    float altitudeTransition = 0.0f;
    float slopeBorder = -1.0f;
    float slopeHardness = 1.0f;
    Material* material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_grass_01.material");;
};

class Terrain : public Component
{
    
public:
    const static int MAX_LAYERS = 32;

    explicit Terrain(GameObject* gameObject);
    ~Terrain() override { }

    // Draws the properties fold out
    void drawProperties() override;

    // Serialisation function
    void serialize(PropertyTable &table) override;

    Mesh* mesh() const { return mesh_; }
    Texture* heightmap() const { return heightMap_; }

    // Total size of the terrain, in m, in X,Y,Z
    Vector3 terrainDimensions() const { return dimensions_; }
    
    //Number of repetitions of each texture layer in X and Z
    Vector2 textureWrapping() const { return textureWrap_; }


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
