#pragma once

#include <vector>

#include "Utils/Singleton.h"

#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/StaticMesh.h"
#include "Scene/Terrain.h"

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

    // Gets all gameobjects that currently exist
    // Note - This includes hidden objects and objects flagged to not be saved.
    const std::vector<GameObject*>& gameObjects() const { return gameObjects_; }

    // Closes the current scene and opens the one at the specified path.
    void openScene(const std::string &scenePath);

    // Closes the current scene and creates a new one, saved at the specified path.
    void createScene(const std::string &scenePath);

    // Updates the current scenes serialized gameobject list to match the gameobjects currently in the scene.
    void saveScene();

    // Gets a list of all static mesh components in the scene
    const std::vector<StaticMesh*> staticMeshes() const;
    const Terrain* terrain() const;

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
