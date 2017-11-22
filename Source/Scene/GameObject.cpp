#include "GameObject.h"

#include "SceneManager.h"

GameObjectComponent::GameObjectComponent(const GameObjectID gameObjectID)
    : gameObjectID_(gameObjectID)
{
    
}

GameObject* GameObjectComponent::gameObject() const
{
    return SceneManager::instance()->findGameObject(gameObjectID_);
}

GameObject::GameObject(const GameObjectID id, const std::string name)
    : id_(id),
    name_(name)
{
    
}

void GameObject::serialize(BitWriter& writer) const
{
    
}

void GameObject::deserialize(BitReader& reader)
{
    
}

Transform* GameObject::transform() const
{
    // Check the scene transform list for one with the
    // correct game object id.
    return SceneManager::instance()->findTransform(id_);
}

Camera* GameObject::camera() const
{
    // Check the scene camera list for one with the
    // correct game object id.
    return SceneManager::instance()->findCamera(id_);
}

StaticMesh* GameObject::staticMesh() const
{
    // Check the scene static mesh list for one with the
    // correct game object id.
    return SceneManager::instance()->findStaticMesh(id_);
}
