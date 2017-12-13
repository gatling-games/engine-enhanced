#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"

Terrain::Terrain(GameObject* gameObject)
	: Component(gameObject),
	textureWrap_(Vector2(128.0f,128.0f)),
	dimensions_(Vector3(256.0f, 30.0f, 256.0f)),
	normalScale_(0.15f)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");
    heightmap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");
    texture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/terrain_grass.png");

}

void Terrain::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Heightmap", "Select Heightmap", heightmap_);
	ImGui::ResourceSelect<Texture>("Texture", "Select Texture", texture_);
	ImGui::DragFloat2("Texture Wrapping", &textureWrap_.x, 0.1f, 1.0f, 1024.0f);
	ImGui::DragFloat3("Size", &dimensions_.x, 1.0f, 1.0f, 4096.0f);
	ImGui::DragFloat("Normal Scale", &normalScale_, 0.01f, 0.01f, 0.2f);
}