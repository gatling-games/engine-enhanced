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
}

Terrain::Terrain(GameObject* gameObject)
    : Component(gameObject),
    heightMap_(TextureFormat::R16, HEIGHTMAP_RESOLUTION, HEIGHTMAP_RESOLUTION),
    dimensions_(Vector3(1024.0f, 80.0f, 1024.0f)),
    seed_(0),
    fractalSmoothness_(2.0f),
    mountainScale_(4.0f),
    islandFactor_(2.0f)
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
    ImGui::Spacing();

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

        for (unsigned int layer = 1; layer < terrainLayers_.size(); layer++)
        {
            ImGui::PushID(layer);

            ImGui::ResourceSelect<Material>("Material", "Select Layer Material", terrainLayers_[layer].material);
            ImGui::DragFloat2("Tile Size", &terrainLayers_[layer].textureTileSize.x, 0.1f, 0.5f, 50.0f);
            ImGui::DragFloat2("Tile Offset", &terrainLayers_[layer].textureTileOffset.x, 0.1f, 0.0f, 50.0f);
            ImGui::Spacing();

            ImGui::DragFloat("Altitude", &terrainLayers_[layer].altitudeBorder, 0.1f, 0.0f, 300.0f);
            ImGui::DragFloat("Transition", &terrainLayers_[layer].altitudeTransition, 0.1f, 0.0f, 20.0f);
            ImGui::Spacing();

            ImGui::DragFloat("Slope", &terrainLayers_[layer].slopeBorder, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Hardness", &terrainLayers_[layer].slopeHardness, 0.01f, 0.001f, 1.0f);
            ImGui::Spacing();

            ImGui::PopID();
        }

        ImGui::TreePop();
    }
}

void Terrain::serialize(PropertyTable &table)
{
    table.serialize("dimensions", dimensions_, Vector3(1024.0f, 80.0f, 1024.0f));
    table.serialize("layers", terrainLayers_);
    table.serialize("seed", seed_, 0);
    table.serialize("factal_smoothness", fractalSmoothness_, 2.0f);
    table.serialize("mountain_scale", mountainScale_, 4.0f);
    table.serialize("island_factor", islandFactor_, 2.0f);

    // If we read in some new properties, the terrain needs regenerating.
    if(table.mode() == PropertyTableMode::Reading)
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
    while(attempts < 2000 && placed < 30)
    {
        attempts++;

        // Pick a random point
        float x = random_float(0.0f, dimensions_.x);
        float z = random_float(0.0f, dimensions_.z);
        float y = sampleHeightmap(x, z);

        // Only place windmills hills, but not too high
        if(y < 30.0f || y > 100.0f)
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

float Terrain::sampleHeightmap(float x, float z)
{
    int xTexel = (int)((x / dimensions_.x) * HEIGHTMAP_RESOLUTION + 0.5f);
    int zTexel = (int)((z / dimensions_.z) * HEIGHTMAP_RESOLUTION + 0.5f);
    return heights_[xTexel + zTexel * HEIGHTMAP_RESOLUTION];
}
