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
    table.serialize("material", material, (ResourcePPtr<Material>)nullptr);
}

Terrain::Terrain(GameObject* gameObject)
    : Component(gameObject),
    textureWrap_(Vector2(10.0f, 10.0f)),
    dimensions_(Vector3(1024.0f, 80.0f, 1024.0f))
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");
    heightMap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");

    // Set up the default layer
    TerrainLayer layer;
    layer.material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_rock_01.material");
    terrainLayers_.push_back(layer);
}

void Terrain::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Heightmap", "Select Heightmap", heightMap_);
    ImGui::ResourceSelect<Material>("Base Material", "Select Layer Material", terrainLayers_[0].material);
    ImGui::DragFloat2("Texture Repeat", &textureWrap_.x, 0.1f, 1.0f, 1024.0f);
    ImGui::DragFloat3("Size", &dimensions_.x, 1.0f, 1.0f, 4096.0f);

    if (ImGui::TreeNode("Terrain Layers"))
    {
        if (ImGui::Button("Add Layer"))
        {
            terrainLayers_.resize(terrainLayers_.size() + 1);
        }

        for (unsigned int layer = 1; layer < terrainLayers_.size(); layer++)
        {
            ImGui::PushID(layer);

            ImGui::DragFloat("Altitude", &terrainLayers_[layer].altitudeBorder, 0.1f, 0.0f, 300.0f);
            ImGui::DragFloat("Transition", &terrainLayers_[layer].altitudeTransition, 0.1f, 0.0f, 20.0f);
            ImGui::Spacing();

            ImGui::DragFloat("Slope", &terrainLayers_[layer].slopeBorder, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Hardness", &terrainLayers_[layer].slopeHardness, 0.01f, 0.001f, 1.0f);
            ImGui::Spacing();

            ImGui::ResourceSelect<Material>("Material", "Select Layer Material", terrainLayers_[layer].material);
            ImGui::Spacing();

            ImGui::PopID();
        }

        ImGui::TreePop();
    }
}

void Terrain::serialize(PropertyTable &table)
{
    table.serialize("heightmap", heightMap_, (ResourcePPtr<Texture>)nullptr);
    table.serialize("texture_wrap", textureWrap_, Vector2(10.0f, 10.0f));
    table.serialize("dimensions", dimensions_, Vector3(1024.0f, 80.0f, 1024.0f));
    table.serialize("layers", terrainLayers_);
}