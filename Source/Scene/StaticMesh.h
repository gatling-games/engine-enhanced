#pragma once

#include "Scene/Component.h"

#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

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

    ResourcePPtr<Mesh> mesh() const { return mesh_; }
    ResourcePPtr<Shader> shader() const { return shader_; }
    ResourcePPtr<Texture> texture() const { return texture_; }
	ResourcePPtr<Texture> normalMap() const { return normalMap_; }

private:
    ResourcePPtr<Mesh> mesh_;
    ResourcePPtr<Shader> shader_;
    ResourcePPtr<Texture> texture_;
	ResourcePPtr<Texture> normalMap_;
};
