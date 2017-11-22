#include "StaticMesh.h"

StaticMesh::StaticMesh(const GameObjectID gameObjectID)
    : GameObjectComponent(gameObjectID)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/cube.obj");
    shader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/ForwardPass.shader");
    texture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/test.png");
}
