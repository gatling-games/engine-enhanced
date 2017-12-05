#include "StaticMesh.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"

StaticMesh::StaticMesh(GameObject* gameObject)
    : Component(gameObject)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/cube.obj");
    shader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/ForwardPass.shader");
    texture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/test.png");
}

void StaticMesh::drawProperties()
{
    ImGui::ResourceSelect<Texture>("Texture", "Select Texture Resource", texture_);
    ImGui::ResourceSelect<Mesh>("Mesh", "Select Mesh Resource", mesh_);
}