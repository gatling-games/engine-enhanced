#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"
#include "Renderer/Material.h"

#include "Math/Random.h"

#include "Scene/Transform.h"
#include "Serialization/Prefab.h"

void TerrainLayer::serialize(PropertyTable& table)
{
    table.serialize("altitude_border", altitudeBorder, 0.0f);
    table.serialize("altitude_transition", altitudeTransition, 0.0f);
    table.serialize("slope_border", slopeBorder, -1.0f);
    table.serialize("slope_hardness", slopeHardness, 1.0f);
    table.serialize("texture_tile_size", textureTileSize, Vector2(10.0f, 10.0f));
    table.serialize("texture_tile_offset", textureTileOffset, Vector2::zero());
    table.serialize("material", material, (ResourcePPtr<Material>)nullptr);
    table.serialize("detail_mesh", detailMesh, (ResourcePPtr<Mesh>)nullptr);
    table.serialize("detail_material", detailMaterial, (ResourcePPtr<Material>)nullptr);
    table.serialize("detail_scale", detailScale, Vector2::one());
}

Terrain::Terrain(GameObject* gameObject)
    : Component(gameObject),
    heightMap_(TextureFormat::R16, HEIGHTMAP_RESOLUTION, HEIGHTMAP_RESOLUTION),
    dimensions_(Vector3(1024.0f, 80.0f, 1024.0f)),
    seed_(0),
    fractalSmoothness_(2.0f),
    mountainScale_(4.0f),
    islandFactor_(2.0f),
    waterColor_(Color(0.05f, 0.066f, 0.093f)),
    waterDepth_(30.0f)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");

    // Ensure bilinear filtering is used on the heightmap
    heightMap_.setFilterMode(TextureFilterMode::Bilinear);

    // Set up the default layer
    TerrainLayer layer;
    layer.material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_rock_01.material");
    terrainLayers_.push_back(layer);

    generateTerrain();
}

void Terrain::drawProperties()
{
    ImGui::DragFloat3("Size", &dimensions_.x, 1.0f, 1.0f, 4096.0f);
    ImGui::ColorEdit3("Water Color", &waterColor_.r);

    float prevWaterDepth = waterDepth_;
    ImGui::DragFloat("Water Depth", &waterDepth_, 0.1f, 0.0f, 100.0f);
    ImGui::Spacing();

    // If the water depth is changed, objects on the terrain need regenerating
    if (fabs(prevWaterDepth - waterDepth_) > 0.001f)
    {
        placeObjects();
        placeDetailMeshes();
    }

    // Make a pseudo-hash of the terrain generation parameters before editing
    float genParams = (float)seed_ + fractalSmoothness_ + mountainScale_ + islandFactor_;

    ImGui::InputInt("Seed", &seed_);
    ImGui::SameLine();
    if (ImGui::Button("Randomise"))
    {
        seed_ = rand();
    }

    ImGui::DragFloat("Fractal Smoothness", &fractalSmoothness_, 0.01f, 1.5f, 2.5f);
    ImGui::DragFloat("Mountain Scale", &mountainScale_, 0.05f, 1.0f, 10.0f);
    ImGui::DragFloat("Island Factor", &islandFactor_, 0.05f, 0.1f, 20.0f);

    // If any gen property was modified, regenerate the terrain.
    float genParamsNew = (float)seed_ + fractalSmoothness_ + mountainScale_ + islandFactor_;
    if (fabs(genParams - genParamsNew) > 0.001f)
    {
        generateTerrain();
    }

    ImGui::Spacing();

    ImGui::ResourceSelect<Material>("Base Material", "Select Layer Material", terrainLayers_[0].material);
    ImGui::DragFloat2("Tile Size", &terrainLayers_[0].textureTileSize.x, 0.1f, 0.5f, 50.0f);
    ImGui::DragFloat2("Tile Offset", &terrainLayers_[0].textureTileOffset.x, 0.1f, 0.0f, 50.0f);
    ImGui::Spacing();

    if (ImGui::TreeNode("Terrain Layers"))
    {
        if (ImGui::Button("Add Layer"))
        {
            terrainLayers_.resize(terrainLayers_.size() + 1);
        }

        for (unsigned int layerIndex = 1; layerIndex < terrainLayers_.size(); layerIndex++)
        {
            ImGui::PushID(layerIndex);

            TerrainLayer& layer = terrainLayers_[layerIndex];

            ImGui::ResourceSelect<Material>("Material", "Select Layer Material", layer.material);
            ImGui::DragFloat2("Tile Size", &layer.textureTileSize.x, 0.1f, 0.5f, 50.0f);
            ImGui::DragFloat2("Tile Offset", &layer.textureTileOffset.x, 0.1f, 0.0f, 50.0f);
            ImGui::Spacing();

            ImGui::DragFloat("Altitude", &layer.altitudeBorder, 0.1f, 0.0f, 300.0f);
            ImGui::DragFloat("Transition", &layer.altitudeTransition, 0.1f, 0.0f, 20.0f);
            ImGui::Spacing();

            ImGui::DragFloat("Slope", &layer.slopeBorder, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Hardness", &layer.slopeHardness, 0.01f, 0.001f, 1.0f);
            ImGui::Spacing();

            const Mesh* prevDetailMesh = layer.detailMesh;
            const Material* prevDetailMaterial = layer.detailMaterial;
            const Vector2 prevDetailScale = layer.detailScale;
            ImGui::ResourceSelect<Mesh>("Detail Mesh", "Select Detail Mesh", layer.detailMesh);
            ImGui::ResourceSelect<Material>("Detail Material", "Select Detail Material", layer.detailMaterial);
            ImGui::DragFloat2("Detail Scale", &layer.detailScale.x, 0.05f, 0.01f, 100.0f);
            ImGui::Spacing();

            // Prevent detail scale min being bigger than max
            layer.detailScale.x = layer.detailScale.minComponent();

            // Regenerate terrain layers when a change is detected
            if(layer.detailMesh != prevDetailMesh || layer.detailMaterial != prevDetailMaterial
                || layer.detailScale != prevDetailScale)
            {
                placeDetailMeshes();
            }

            ImGui::PopID();
        }

        ImGui::TreePop();
    }
}

void Terrain::serialize(PropertyTable &table)
{
    table.serialize("dimensions", dimensions_, Vector3(1024.0f, 80.0f, 1024.0f));
    table.serialize("water_color", waterColor_, Color(0.05f, 0.066f, 0.093f));
    table.serialize("water_depth", waterDepth_, 30.0f);
    table.serialize("layers", terrainLayers_);
    table.serialize("seed", seed_, 0);
    table.serialize("factal_smoothness", fractalSmoothness_, 2.0f);
    table.serialize("mountain_scale", mountainScale_, 4.0f);
    table.serialize("island_factor", islandFactor_, 2.0f);

    // If we read in some new properties, the terrain needs regenerating.
    if (table.mode() == PropertyTableMode::Reading)
    {
        generateTerrain();
    }
}

void Terrain::generateTerrain()
{
    /*
     * The terrain generation process is split into several stages.
     *
     * This process is deterministic - using the same seed and settings
     * will generate the same terrain in multiple runs.
     *
     * First, a fractal terrain generation process is run:
     *     - Create a small 2x2 heightmap
     *     - Offset each height value up or down by a small random amount
     *     - Bilearly upscale the heightmap and repeat
     * The "fractal smoothness" property determines how rapidly the magnitude
     * of the random offset is reduced by for each subsequent iteration.
     *
     * Next, tall mountains are formed by raising each height value to a power.
     * The power is called "mountain scale" - large values flatten most of the
     * terrain and make the mountains seem taller.
     *
     * The heightmap is then lowered near the edges to make it appear like an
     * island. The "island factor" property controls the falloff for the lowering
     * process.
     *
     * Finally, the maximum height in the heightmap is found. This is used to
     * normalize the heightmap prior to storing it in a gpu-memory texture.
     */

    srand(seed_);

    // Reset the heightmap to a single value.
    int resolution = 1;
    heights_.resize(1);
    heights_[0] = dimensions_.y / 2.0f;

    // Run fractal passes, doubling the heightmap size each time, until
    // we are up to the required resolution.
    float moveSize = dimensions_.y / 2.0f;
    while (resolution < HEIGHTMAP_RESOLUTION)
    {
        int newResolution = resolution * 2;
        std::vector<float> newHeights(newResolution * newResolution);
        for (int y = 0; y < newResolution; ++y)
        {
            for (int x = 0; x < newResolution; ++x)
            {
                float v1 = heights_[x / 2 + y / 2 * resolution];
                float v2 = heights_[std::min(x / 2 + 1, resolution - 1) + y / 2 * resolution];
                float interp0 = (x % 2) == 0 ? v1 : (v1 + v2) / 2.0f;

                float v3 = heights_[x / 2 + std::min(y / 2 + 1, resolution - 1) * resolution];
                float v4 = heights_[std::min(x / 2 + 1, resolution - 1) + std::min(y / 2 + 1, resolution - 1) * resolution];
                float interp1 = (x % 2) == 0 ? v3 : (v3 + v4) / 2.0f;

                float interp = (y % 2) == 0 ? interp0 : (interp0 + interp1) / 2.0f;

                newHeights[x + y * newResolution] = interp + random_float(-moveSize, moveSize);
            }
        }

        heights_ = newHeights;
        resolution = newResolution;
        moveSize /= fractalSmoothness_;
    }

    // Raise each height value to a power, allowing a controllable "mountain factor"
    // that pulls high bits up and squashes lower bits down.
    for (unsigned int i = 0; i < heights_.size(); ++i)
    {
        heights_[i] = powf(heights_[i], mountainScale_);
    }

    // Use an "island factor" to flatten parts near the edge of the heightmap
    // Just compute the distance from the island centre and flatten, with a controlable power factor.
    for (int y = 0; y < HEIGHTMAP_RESOLUTION; ++y)
    {
        for (int x = 0; x < HEIGHTMAP_RESOLUTION; ++x)
        {
            float distanceX = (x / (float)HEIGHTMAP_RESOLUTION) - 0.5f;
            float distanceY = (y / (float)HEIGHTMAP_RESOLUTION) - 0.5f;
            float distanceFromCentre = sqrtf(distanceX * distanceX + distanceY * distanceY) * 2.0f;
            if (distanceFromCentre > 1.0f)
            {
                distanceFromCentre = 1.0f;
            }

            heights_[x + y * HEIGHTMAP_RESOLUTION] *= 1.0f - powf(distanceFromCentre, islandFactor_);
        }
    }

    // Determine the maximum heightmap height
    float maxHeight = 0.0f;
    for (unsigned int i = 0; i < heights_.size(); ++i)
    {
        maxHeight = std::max(maxHeight, heights_[i]);
    }

    // Generate a uint16 version of the data (normalized) for passing to the gpu.
    std::vector<uint16_t> textureHeights(HEIGHTMAP_RESOLUTION * HEIGHTMAP_RESOLUTION);
    for (int i = 0; i < textureHeights.size(); ++i)
    {
        textureHeights[i] = (uint16_t)(heights_[i] / maxHeight * 65535.0f);

        // Also correct the on-cpu heightfield to match
        heights_[i] /= (maxHeight / dimensions_.y);
    }

    // Upload the heightmap data to the gpu
    heightMap_.setData(textureHeights.data(), 2 * HEIGHTMAP_RESOLUTION * HEIGHTMAP_RESOLUTION, 0);

    // The heightmap is now build.
    // Place objects on it.
    placeObjects();
    placeDetailMeshes();
}

void Terrain::placeObjects()
{
    // Delete any existing objects
    for (GameObject* go : placedObjects_)
    {
        delete go;
    }
    placedObjects_.clear();

    // Pick random points on the heightmap and check if they are suitable for a windmill.
    int placed = 0;
    int attempts = 0;
    while (attempts < 2000 && placed < 30)
    {
        attempts++;

        // Pick a random point
        float x = random_float(0.0f, dimensions_.x);
        float z = random_float(0.0f, dimensions_.z);
        float y = sampleHeightmap(x, z);

        // Only place windmills hills, but not too high
        if (y < 30.0f || y > 100.0f)
        {
            continue;
        }

        // Place the object at that point
        GameObject* newGO = new GameObject("Windmill", ResourceManager::instance()->load<Prefab>("Resources/Prefabs/Environment/wind_turbine.prefab"));
        newGO->setFlag(GameObjectFlag::NotShownOrSaved, true);
        newGO->transform()->setPositionLocal(Point3(x, y, z));
        newGO->transform()->setRotationLocal(Quaternion::euler(0.0f, 32.0f, 0.0f));
        placedObjects_.push_back(newGO);

        placed++;
    }
}

void Terrain::placeDetailMeshes()
{
    // Delete any existing detail batches
    detailMeshBatches_.clear();

    // Consider each layer with a valid detail mesh.
    for (const TerrainLayer& layer : terrainLayers_)
    {
        // Ignore ones that dont have a detail mesh or material
        if (layer.detailMesh == nullptr || layer.detailMaterial == nullptr)
        {
            continue;
        }

        // Split the terrain into a 16x16 grid of detail batches
        const int batchResolution = 32;
        for(int x = 0; x < batchResolution; ++x)
        {
            // Determine the bounds in the x plane
            float minX = x * dimensions_.x / (float)batchResolution;
            float maxX = (x + 1) * dimensions_.x / (float)batchResolution;

            for(int z = 0; z < batchResolution; ++z)
            {
                // Determine the bounds in the z plane
                float minZ = z * dimensions_.z / (float)batchResolution;
                float maxZ = (z + 1) * dimensions_.z / (float)batchResolution;

                // Try to create a new batch
                DetailBatch batch;
                batch.mesh = layer.detailMesh;
                batch.material = layer.detailMaterial;
                batch.bounds = Bounds(Point3(minX, 0.0f, minZ), Point3(maxX, dimensions_.y, maxZ));
                batch.drawDistance = batch.bounds.size().magnitude() * 1.5f;

                // Look for instance positions within the bounds
                generateDetailPositions(batch, layer);

                // If the batch is not empty, save it
                if (batch.count > 0)
                {
                    detailMeshBatches_.push_back(batch);
                }
            }
        }
    }
}

void Terrain::generateDetailPositions(DetailBatch& batch, const TerrainLayer& layer) const
{
    // Use the batch centre as the seed
    // This ensures that multiple runs are deterministic.
    srand((unsigned int)batch.bounds.centre().x * (unsigned int)batch.bounds.centre().y + seed_);

    // Reset the number of positions in the batch
    batch.count = 0;

    // Otherwise, pick 1024 random points on the terrain
    int attempts = 0;
    while (attempts < 2000 && batch.count < DetailBatch::MaxInstancesPerBatch)
    {
        attempts++;

        // Pick a random point
        float x = random_float(batch.bounds.min().x, batch.bounds.max().x);
        float z = random_float(batch.bounds.min().z, batch.bounds.max().z);
        float y = sampleHeightmap(x, z);

        // Respect the layer's altitude settings
        if ((y > layer.altitudeBorder + layer.altitudeTransition * 0.8f && layer.altitudeTransition >= 0.0f) || (y < layer.altitudeBorder && layer.altitudeTransition < 0.0f))
        {
            float scale = random_float(layer.detailScale.x, layer.detailScale.y);

            batch.instancePositions[batch.count] = Vector4(x, y, z, scale);
            batch.count++;
        }
    }
}

float Terrain::sampleHeightmap(float x, float z) const
{
    int xTexel = (int)((x / dimensions_.x) * (HEIGHTMAP_RESOLUTION - 1) + 0.5f);
    int zTexel = (int)((z / dimensions_.z) * (HEIGHTMAP_RESOLUTION - 1) + 0.5f);
    return heights_[xTexel + zTexel * HEIGHTMAP_RESOLUTION] - waterDepth_;
}
