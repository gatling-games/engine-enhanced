#include "StaticMesh.h"

StaticMesh::StaticMesh(GameObject* gameObject)
    : Component(gameObject)
{
    mesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/cube.obj");
    shader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/ForwardPass.shader");
    texture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/test.png");
}
