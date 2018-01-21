#include "StaticMesh.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"

StaticMesh::StaticMesh(GameObject* gameObject)
    : Component(gameObject)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/choppa.obj");
    shader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/ForwardPass.shader");
    texture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/default.tga");
	normalMap_ = ResourceManager::instance()->load<Texture>("Resources/Textures/default_normals.tga");
}

void StaticMesh::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Texture", "Select Texture Resource", texture_);
	ImGui::ResourceSelect<Texture>("Normal Map", "Select Texture Resource", normalMap_);
    ImGui::ResourceSelect<Mesh>("Mesh", "Select Mesh Resource", mesh_);
}

void StaticMesh::serialize(PropertyTable &table)
{
   table.serialize("Mesh", mesh_, ResourceManager::instance()->load<Mesh>("Resources/Meshes/cube.obj"));
   table.serialize("Shader", shader_, ResourceManager::instance()->load<Shader>("Resources/Shaders/ForwardPass.shader"));
   table.serialize("Texture", texture_, ResourceManager::instance()->load<Texture>("Resources/Textures/default.tga"));
   table.serialize("Normal Map", normalMap_, ResourceManager::instance()->load<Texture>("Resources/Textures/default_normals.tga"));
}