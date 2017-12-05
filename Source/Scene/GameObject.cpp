#include "GameObject.h"

#include "SceneManager.h"
#include "Scene/Transform.h"

GameObject::GameObject(const GameObjectID id, const std::string &name)
    : id_(id),
    name_(name)
{
    // Give every GameObject instance a transform component
    // This ensures that gameobject can be parented inside each other.
    createComponent<Transform>();
}

void GameObject::serialize(BitWriter& writer) const
{
    
}

void GameObject::deserialize(BitReader& reader)
{
    
}

Transform* GameObject::transform() const
{
    return findComponent<Transform>();
}

Camera* GameObject::camera() const
{
    return findComponent<Camera>();
}

StaticMesh* GameObject::staticMesh() const
{
    return findComponent<StaticMesh>();
}
