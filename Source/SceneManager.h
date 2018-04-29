#pragma once

#include <vector>

#include "Utils/Singleton.h"

#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/StaticMesh.h"
#include "Scene/Terrain.h"
#include "Scene/Shield.h"
#include "Physics/BoxCollider.h"
#include "Physics/SphereCollider.h"

struct InputCmd;

class SceneManager : public Singleton<SceneManager>
{
    friend class GameObject;

public:
    SceneManager();

    // Called each frame.
    void frameStart();

    // Gets the currently loaded scene
    const Scene* currentScene() const { return currentScene_; }

    // Gets the name of the current scene
    std::string sceneName() const { return currentScene_->resourceName(); }
    std::string scenePath() const { return currentScene_->resourcePath(); }

    // Gets all gameobjects that currently exist
    // Note - This includes hidden objects and objects flagged to not be saved.
    const std::vector<GameObject*>& gameObjects() const { return gameObjects_; }

    // Gets a single instance of a specified component attached to a gameobject in the scene.
    // If none is found, returns null.
    template<typename T>
    T* findComponentInScene() const
    {
        for (GameObject* gameObject : gameObjects_)
        {
            T* component = gameObject->findComponent<T>();
            if (component != nullptr)
            {
                return component;
            }
        }

        return nullptr;
    }

    // Gets a list of all instances of a specified comonent, on any gameobject in the scene.
    template<typename T>
    std::vector<T*> findAllComponentsInScene() const
    {
        std::vector<T*> components;

        for (GameObject* gameObject : gameObjects_)
        {
            T* component = gameObject->findComponent<T>();
            if (component != nullptr)
            {
                components.push_back(component);
            }
        }

        return components;
    }

    // Closes the current scene and opens the one at the specified path.
    void openScene(const std::string &scenePath);

    // Closes the current scene and creates a new one, saved at the specified path.
    void createScene(const std::string &scenePath);

    // Updates the current scenes serialized gameobject list to match the gameobjects currently in the scene.
    void saveScene();

    // Gets the main camera, i.e. the scene camera that has existed for the longest
    Camera* mainCamera() const;

    // Passes input struct to all gameobjects
    void handleInput(const InputCmd& inputs);

private:
    // The currently loaded scene
    Scene* currentScene_;

    // A list of currently loaded gameobjects that *are* part of the scene.
    std::vector<GameObject*> gameObjects_;

    // Adds a menu item for creating a new gameobject with the given component
    template<typename T>
    void addCreateGameObjectMenuItem(const std::string &gameObjectName);

    // Called by GameObject upon construction
    void gameObjectCreated(GameObject* go);

    // Called by GameObject upon destruction
    void gameObjectDeleted(GameObject* go);
};
