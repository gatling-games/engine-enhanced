#pragma once

#include "Scene/Component.h"

#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

class StaticMesh : public Component
{
public:
    explicit StaticMesh(GameObject* gameObject);
    ~StaticMesh() override { }

    // Draws the properties fold out
    void drawProperties() override;

    // Override of Component::serialize().
    // Handles component serialization
    void serialize(PropertyTable &table) override;

    Material* material() const { return material_; }
    Mesh* mesh() const { return mesh_; }

private:
    Material* material_;
    Mesh* mesh_;
};
