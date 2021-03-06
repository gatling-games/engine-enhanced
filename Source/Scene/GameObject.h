#pragma once

#include <string>
#include <vector>

#include "Editor/EditableObject.h"
#include "Serialization/SerializedObject.h"

class Collider;
class Component;
class BitWriter;
class BitReader;

class Transform;
class Camera;
class StaticMesh;
class Terrain;

// Identify gameobjects with a unique 32 bit ID
typedef uint32_t GameObjectID;

struct InputCmd;

typedef uint32_t GameObjectFlagList;
enum class GameObjectFlag
{
    None = 0,
    NotShownInScenePanel = 1,
    NotSavedInScene = 2,
    NotShownOrSaved = NotShownInScenePanel | NotSavedInScene,
    SurviveSceneChanges = 4,
};

// Class for a game object
// Most of the actual work is deferred to the scene manager.
class GameObject : public IEditableObject, ISerializedObject
{
    friend class SceneManager;
    friend class Prefab;
    friend class PropertyTable;

public:
    GameObject();
    explicit GameObject(const std::string &name);
    explicit GameObject(const std::string &name, Prefab* prefab);

    ~GameObject();

    // Prevent a GameObject from being copied or moved
    GameObject(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    // Getters for basic gameobject properties
    const std::string& name() const { return name_; }
    Prefab* prefab() const { return prefab_; }

    // Methods for getting and setting gameobject flags
    GameObjectFlagList flags() const { return flags_; }
    bool hasFlag(GameObjectFlag flag) const;
    void setFlag(GameObjectFlag flag, bool state);
    void setFlags(GameObjectFlagList flags);

    // Draws the gameobject editor panel
    void drawEditor() override;
    void drawComponentsSection();
    void drawAddComponentSection();
    void drawSaveAsPrefabSection();
    void drawPrefabInfoSection();

    // Serialization methods.
    // Used for networking and saving objects to disk.
    void serialize(PropertyTable &table) override;

    // Called once per frame
    void update(float deltaTime);

	// Dispatches an input command to all components on the gameobject
    void handleInput(const InputCmd& inputs);

	// Dispatches a collision event to all components on the gameobject
	void handleCollision(Collider* collider);

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

    // Looks for a component with the given name on the GameObject.
    // Returns nullptr if none is found.
    Component* findComponent(const std::string &typeName);

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

    // Adds a component to the gameobject by its type name.
    Component* createComponent(const std::string &typeName);

    // Shortcut methods for finding components
    Transform* transform() const;
    Camera* camera() const;
    StaticMesh* staticMesh() const;
    Terrain* terrain() const;

    // Gets a list of all components attached to the gameobject
    const std::vector<Component*> componentList() const { return components_; }

protected:
	void removeComponent(Component* component);

private:
    std::string name_;
    GameObjectFlagList flags_;

    // The prefab that the GameObject was instantiated from
    Prefab* prefab_;

    // The components that currently exist on the GameObject
    std::vector<Component*> components_;

	friend class Component;
};
