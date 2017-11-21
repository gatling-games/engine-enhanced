#include "StaticMesh.h"

StaticMesh::StaticMesh(const GameObjectID gameObjectID)
    : GameObjectComponent(gameObjectID)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Meshes/cube.obj");
    shader_ = ResourceManager::instance()->load<Shader>("Shaders/ForwardPass.shader");
}
