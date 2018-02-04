#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"

Terrain::Terrain(GameObject* gameObject)
    : Component(gameObject),
    textureWrap_(Vector2(10.0f, 10.0f)),
    dimensions_(Vector3(256.0f, 30.0f, 256.0f))
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");
    heightmap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");
    baseTexture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/ground_grass_01_albedo.tga");
    normalMap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/ground_grass_01_normals.tga");
}

void Terrain::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Heightmap", "Select Heightmap", heightmap_);
    ImGui::ResourceSelect<Texture>("Base Texture", "Select Texture", baseTexture_);
    ImGui::ResourceSelect<Texture>("Normal Map", "Select Normal Map", normalMap_);
    ImGui::DragFloat2("Texture Repeat", &textureWrap_.x, 0.1f, 1.0f, 1024.0f);
    ImGui::DragFloat3("Size", &dimensions_.x, 1.0f, 1.0f, 4096.0f);
}