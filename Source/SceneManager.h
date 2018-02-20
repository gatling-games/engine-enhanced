#pragma once

#include <vector>

#include "Utils/Singleton.h"

#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/StaticMesh.h"
#include "Scene/Terrain.h"

class SceneManager : public Singleton<SceneManager>
{
public:
    SceneManager();

    // Called each frame.
    void frameStart();

    // Gets the currently loaded scene
    const Scene* currentScene() const { return currentScene_; }

    // Gets the name of the current scene
    std::string sceneName() const { return currentScene_->resourceName(); }

    // Gets all gameobjects in the current scene.
    // Note - This list does not include hidden gameobjects
    const std::vector<std::shared_ptr<GameObject>>& sceneObjects() const { return currentScene_->gameObjects(); }

    // Closes the current scene and opens the one at the specified path.
    void openScene(const std::string &scenePath);

    // Closes the current scene and creates a new one, saved at the specified path.
    void createScene(const std::string &scenePath);

    // Creates a new GameObject in the scene
    GameObject* createGameObject(const std::string &name, Transform* parent = nullptr, bool hidden = false);

    // Creates a new GameObject, based on the specified prefab
    // If hidden is specified, the gameobject will not be saved as part of the current scene.
    GameObject* createGameObject(Prefab* prefab, bool hidden = false);

    // Gets a list of all static mesh components in the scene
    const std::vector<StaticMesh*> staticMeshes() const;
    const std::vector<Terrain*> terrains() const;

private:
    // The currently loaded scene
    Scene* currentScene_;

    // A list of currently loaded gameobjects that are not associated
    // with the scene and are not serialized or saved to disk.
    std::vector<GameObject*> hiddenGameObjects_;

    // Adds a menu item for creating a new gameobject with the given component
    template<typename T>
    void addCreateGameObjectMenuItem(const std::string &gameObjectName);
};
