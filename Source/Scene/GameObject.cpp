#include "GameObject.h"

#include "SceneManager.h"

#include "Scene/Component.h"
#include "Scene/Transform.h"
#include "Scene/Freecam.h"
#include "Scene/Terrain.h"

GameObject::GameObject(const std::string &name)
    : name_(name)
{
    // Give every GameObject instance a transform component
    // This ensures that gameobject can be parented inside each other.
    createComponent<Transform>();
}

void GameObject::serialize(BitWriter&) const
{
    
}

void GameObject::deserialize(BitReader&)
{
    
}

void GameObject::update(float deltaTime)
{
    for(unsigned int i = 0; i < components_.size(); ++i)
    {
        components_[i]->update(deltaTime);
    }
}

Component* GameObject::createComponent(const std::string &typeName)
{
    if (typeName == "Transform")
        return createComponent<Transform>();
    
    if (typeName == "Camera")
        return createComponent<Camera>();

    if (typeName == "StaticMesh")
        return createComponent<StaticMesh>();

    if (typeName == "Freecam")
        return createComponent<Freecam>();

    const std::string errorMessage = "CreateComponent() type " + typeName + " not defined";
    throw std::exception(errorMessage.c_str());
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

Terrain* GameObject::terrain() const
{
	return findComponent<Terrain>();
}
