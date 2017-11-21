#pragma once

#include <string>

#include "Utils/BitReader.h"
#include "Utils/BitWriter.h"

class GameObject;

// Forward declare component types.
// If we use #include it creates a loop.
class Transform;
class Camera;
class StaticMesh;

// Identify gameobjects with a unique 32 bit ID
typedef uint32_t GameObjectID;

// Base class for all components.
//
// GameObject instances do not contain the components, they are 
// stored by the scene manager is separate lists.
// 
// This component class therefore stores the id of the gameobject
// that the component belongs to, so it can be looked up.
class GameObjectComponent
{
public:
    explicit GameObjectComponent(const GameObjectID gameObjectID);

    // Gets the id of the gameobject this component is attached to.
    GameObjectID gameObjectID() const { return gameObjectID_; }
    
    // Finds the game object that the component belongs to.
    // Note: This method can be slow. Caching the result may be worthwile.
    GameObject* gameObject() const;

private:
    const GameObjectID gameObjectID_;
};

// Class for a game object
// Most of the actual work is deferred to the scene manager.
class GameObject
{
public:
    explicit GameObject(const GameObjectID id, const std::string name);

    // Getters for basic gameobject properties
    GameObjectID id() const { return id_; }
    const std::string& name() const { return name_; }

    // Serialization methods.
    // Used for networking and saving objects to disk.
    void serialize(BitWriter &writer) const;
    void deserialize(BitReader &reader);

    // Methods for finding components attached to the gameobject.
    // Note: These methods can be slow. Caching the result may be worthwile.
    Transform* transformComponent() const;
    Camera* cameraComponent() const;
    StaticMesh* staticMeshComponent() const;

private:
    const GameObjectID id_;
    std::string name_;
};
