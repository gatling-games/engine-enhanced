#include "StaticMesh.h"

#include "Utils/ImGuiExtensions.h"

StaticMesh::StaticMesh(GameObject* gameObject)
    : Component(gameObject),
    mesh_(nullptr),
    material_(nullptr)
{

}

void StaticMesh::drawProperties()
{
    ImGui::ResourceSelect<Material>("Material", "Select Material Resource", material_);
    ImGui::ResourceSelect<Mesh>("Mesh", "Select Mesh Resource", mesh_);
}

void StaticMesh::serialize(PropertyTable &table)
{
    table.serialize("mesh", mesh_);
    table.serialize("material", material_);
}

void StaticMesh::setMaterial(Material* material)
{
    material_ = material;
}