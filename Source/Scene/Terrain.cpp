#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"
#include "Renderer/Material.h"

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
    heightMap_(TextureFormat::R8, HEIGHTMAP_RESOLUTION, HEIGHTMAP_RESOLUTION),
    dimensions_(Vector3(1024.0f, 80.0f, 1024.0f))
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
}

void Terrain::generateTerrain()
{
    std::vector<uint8_t> heights(HEIGHTMAP_RESOLUTION * HEIGHTMAP_RESOLUTION);

    for(int x = 0; x < HEIGHTMAP_RESOLUTION; ++x)
    {
        for(int y = 0; y < HEIGHTMAP_RESOLUTION; ++y)
        {
            if (x < HEIGHTMAP_RESOLUTION / 2)
                heights[x * HEIGHTMAP_RESOLUTION + y] = y / 4;
            else
                heights[x*HEIGHTMAP_RESOLUTION + y] = (255 - y / 4);
        }
    }

    heightMap_.setData(heights.data(), HEIGHTMAP_RESOLUTION * HEIGHTMAP_RESOLUTION, 0);

}
