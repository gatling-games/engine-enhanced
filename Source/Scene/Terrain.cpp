#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"

Terrain::Terrain(GameObject* gameObject)
	: Component(gameObject),
	dimensions_(Vector2(256.0f, 256.0f)),
	terrainScale_(30.0f)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");
    heightmap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");
    texture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");

}

void Terrain::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Heightmap", "Select Heightmap", heightmap_);
	ImGui::ResourceSelect<Texture>("Texture", "Select Texture", texture_);
	ImGui::DragFloat2("Size", &dimensions_.x, 0.1f, 0.1f, 4096.0f);
	ImGui::DragFloat("Scale", &terrainScale_, 0.1f, 0.1f, 1000.0f);
}