#include "StaticMesh.h"

#include "Utils/ImGuiExtensions.h"

StaticMesh::StaticMesh(GameObject* gameObject)
    : Component(gameObject)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/copter2.obj");
    material_ = ResourceManager::instance()->load<Material>("Resources/Materials/default.material");
}

void StaticMesh::drawProperties()
{
    ImGui::ResourceSelect<Material>("Material", "Select Material Resource", material_);
    ImGui::ResourceSelect<Mesh>("Mesh", "Select Mesh Resource", mesh_);
}

void StaticMesh::serialize(PropertyTable &table)
{
    table.serialize("mesh", mesh_, (Mesh*)nullptr);
    table.serialize("material", material_, (Material*)nullptr);
}