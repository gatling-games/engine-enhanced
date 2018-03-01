#pragma once

#include "Scene/Component.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

class Material;

struct TerrainLayer : ISerializedObject
{
    float altitudeBorder = 0.0f;
    float altitudeTransition = 0.0f;
    float slopeBorder = -1.0f;
    float slopeHardness = 1.0f;
    Vector2 textureTileSize = Vector2(10.0f, 10.0f);
    Vector2 textureTileOffset = Vector2::zero();
    Material* material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_grass_01.material");

    // Detail mesh settings
    // The detail mesh is drawn if both the mesh & material are assigned
    Mesh* detailMesh = nullptr;
    Material* detailMaterial = nullptr;
    Vector2 detailScale = Vector2::one();

    void serialize(PropertyTable& table) override;
};

// A group of detail meshes drawn in a single batch
struct DetailBatch
{
    const static int MaxInstancesPerBatch = 1024;

    int count;
    Mesh* mesh;
    Material* material;
    Vector4 instancePositions[MaxInstancesPerBatch];
};

class Terrain : public Component
{
public:
    const static int HEIGHTMAP_RESOLUTION = 1024;
    const static int MAX_LAYERS = 32;

    explicit Terrain(GameObject* gameObject);
    ~Terrain() override { }

    // Draws the properties fold out
    void drawProperties() override;

    // Serialisation function
    void serialize(PropertyTable &table) override;

    const Mesh* mesh() const { return mesh_; }
    const Texture* heightmap() const { return &heightMap_; }

    // Total size of the terrain, in m, in X,Y,Z
    Vector3 size() const { return dimensions_; }
    
    // The base color of the water
    Color waterColor() const { return waterColor_; }

    // The depth of the water at its deepest point
    float waterDepth() const { return waterDepth_; }

    // The layers on the terrain
    const TerrainLayer* layers() const { return &terrainLayers_.front(); }
    int layerCount() const { return (int)terrainLayers_.size(); }

    // The detail mesh batches on the terrain
    const std::vector<DetailBatch>& detailBatches() const { return detailMeshBatches_; }

private:
    Mesh* mesh_;
    Texture heightMap_;
    Vector3 dimensions_;
    Color waterColor_;
    float waterDepth_;
    std::vector<TerrainLayer> terrainLayers_;

    // Terrain generation settings
    int seed_;
    float fractalSmoothness_;
    float mountainScale_;
    float islandFactor_;

    // The current heightmap
    std::vector<float> heights_;

    // A list of objects placed on the terrain
    std::vector<GameObject*> placedObjects_;

    // A list of detail mesh layers on the terrain
    std::vector<DetailBatch> detailMeshBatches_;

    // Regenerates the terrain
    void generateTerrain();
    void placeObjects();
    void placeDetailMeshes();

    // Gets the heightmap height at a specified point
    // The x and z coordinates are in world space.
    float sampleHeightmap(float x, float z);
};
