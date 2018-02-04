#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"
#include "Renderer/Material.h"

Terrain::Terrain(GameObject* gameObject)
    : Component(gameObject),
    textureWrap_(Vector2(10.0f,10.0f)),
    dimensions_(Vector3(256.0f, 30.0f, 256.0f)),
    layerCount_(1)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");
    heightMap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");
    terrainLayers_[0].material = ResourceManager::instance()->load<Material>("Resources/Materials/ground_grass_01.material");
}

void Terrain::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Heightmap", "Select Heightmap", heightMap_);
    ImGui::DragFloat2("Texture Repeat", &textureWrap_.x, 0.1f, 1.0f, 1024.0f);
    ImGui::DragFloat3("Size", &dimensions_.x, 1.0f, 1.0f, 4096.0f);
    if(ImGui::TreeNode("Terrain Layers"))
    {
        if(ImGui::Button("Add Layer"))
        {
            layerCount_ += 1;
        }
        for (int layer = 0; layer < layerCount_; layer++)
        {
            ImGui::PushID(layer);
            ImGui::InputText("", (char*)&terrainLayers_[layer].name, sizeof(terrainLayers_[layer].name));
            ImGui::DragFloat2("Angle Span", &terrainLayers_[layer].minMaxAngle.x, 0.1f, 0.0f, 90.0f);
            ImGui::DragFloat2("Height Span", &terrainLayers_[layer].minMaxHeight.x, 0.1f, 0.0f, 100.0f);
            ImGui::ResourceSelect<Material>("Material", "Select Layer Material",terrainLayers_[layer].material);
            ImGui::Spacing();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}