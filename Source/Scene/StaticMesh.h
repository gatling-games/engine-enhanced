#pragma once

#include "Scene/GameObject.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

class StaticMesh : public GameObjectComponent
{
public:
    StaticMesh(const GameObjectID gameObjectID);

    Mesh* mesh() const { return mesh_; }
    Shader* shader() const { return shader_; }
    Texture* texture() const { return texture_; }

private:
    Mesh* mesh_;
    Shader* shader_;
    Texture* texture_;
};
