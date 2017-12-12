#include "Terrain.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"

Terrain::Terrain(GameObject* gameObject)
    : Component(gameObject)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/terrain.obj");
    heightmap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");
    texture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/heightmap.png");
    dimensions_ = 64;
    generateTerrain();
}

void Terrain::generateTerrain()
{
    
}
void Terrain::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Heightmap", "Select Heightmap", heightmap_);
}