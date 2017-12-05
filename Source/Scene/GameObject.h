#pragma once

#include <string>
#include <vector>

class Component;
class BitWriter;
class BitReader;

class Transform;
class Camera;
class StaticMesh;

// Identify gameobjects with a unique 32 bit ID
typedef uint32_t GameObjectID;

// Class for a game object
// Most of the actual work is deferred to the scene manager.
class GameObject
{
public:
    explicit GameObject(const GameObjectID id, const std::string &name);

    // Getters for basic gameobject properties
    GameObjectID id() const { return id_; }
    const std::string& name() const { return name_; }

    // Serialization methods.
    // Used for networking and saving objects to disk.
    void serialize(BitWriter &writer) const;
    void deserialize(BitReader &reader);

    // Looks for a component of the given type on the GameObject.
    // Returns nullptr if none is found.
    // Note: This method can be slow. Caching the result may be worthwile.
    template<class T>
    T* findComponent() const
    {
        // Check each existing component to see if any are an instance of T
        for (unsigned int i = 0; i < components_.size(); ++i)
        {
            // Try to cast to a T
            T* existing = dynamic_cast<T*>(components_[i]);
            
            // dynamic_cast gives us nullptr if it failed
            if (existing != nullptr)
            {
                return existing;
            }
        }

        // No component exists.
        return nullptr;
    }

    // Adds a component of the given type to the GameObject, if none exists already.
    // Returns the new, or existing, component.
    // Note: This method can be slow. Caching the result may be worthwile.
    template<class T>
    T* createComponent()
    {
        // Look for an existing component of the correct type.
        T* existing = findComponent<T>();
        if (existing != nullptr)
        {
            return existing;
        }

        // Not found. Create a new component and add it to the gameobject.
        T* newObject = new T(this);
        components_.push_back(newObject);
        return newObject;
    }

    // Shortcut methods for finding components
    Transform* transform() const;
    Camera* camera() const;
    StaticMesh* staticMesh() const;

private:
    const GameObjectID id_;
    const std::string name_;

    // The components that currently exist on the GameObject
    std::vector<Component*> components_;
};
