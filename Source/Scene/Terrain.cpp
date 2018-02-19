#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"
#include "Renderer/Material.h"

Terrain::Terrain(GameObject* gameObject)
    : Component(gameObject),
    textureWrap_(Vector2(10.0f,10.0f)),
    dimensions_(Vector3(1024.0f, 80.0f, 1024.0f)),
    layerCount_(3)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");
    heightMap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");

    // Set up the default layers
    terrainLayers_[0].material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_rock_01.material");
    terrainLayers_[1].material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_grass_01.material");
    terrainLayers_[1].slopeBorder = 0.6f;
    terrainLayers_[1].slopeHardness = 0.3f;
    terrainLayers_[2].material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_snow.material");
    terrainLayers_[2].altitudeBorder = 35.0f;
    terrainLayers_[2].altitudeTransition = 10.0f;
    terrainLayers_[2].slopeBorder = 0.3f;
    terrainLayers_[2].slopeHardness = 0.25f;
}

void Terrain::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Heightmap", "Select Heightmap", heightMap_);
    ImGui::ResourceSelect<Material>("Base Material", "Select Layer Material", terrainLayers_[0].material);
    ImGui::DragFloat2("Texture Repeat", &textureWrap_.x, 0.1f, 1.0f, 1024.0f);
    ImGui::DragFloat3("Size", &dimensions_.x, 1.0f, 1.0f, 4096.0f);
    
    if(ImGui::TreeNode("Terrain Layers"))
    {
        if(ImGui::Button("Add Layer"))
        {
            layerCount_ += 1;
        }

        for (int layer = 1; layer < layerCount_; layer++)
        {
            ImGui::PushID(layer);
            ImGui::InputText("", (char*)&terrainLayers_[layer].name, sizeof(terrainLayers_[layer].name));
            ImGui::Spacing();

            ImGui::DragFloat("Altitude", &terrainLayers_[layer].altitudeBorder, 0.1f, 0.0f, 300.0f);
            ImGui::DragFloat("Transition", &terrainLayers_[layer].altitudeTransition, 0.1f, 0.0f, 20.0f);
            ImGui::Spacing();

            ImGui::DragFloat("Slope", &terrainLayers_[layer].slopeBorder, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Hardness", &terrainLayers_[layer].slopeHardness, 0.01f, 0.001f, 1.0f);
            ImGui::Spacing();

            ImGui::ResourceSelect<Material>("Material", "Select Layer Material",terrainLayers_[layer].material);
            ImGui::Spacing();

            ImGui::PopID();
        }

        ImGui::TreePop();
    }
}