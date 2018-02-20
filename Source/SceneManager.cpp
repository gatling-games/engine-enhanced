#include "SceneManager.h"

#include "Editor/MainWindowMenu.h"
#include "Editor/PropertiesPanel.h"

#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"
#include "Scene/Terrain.h"

#include "Serialization/Prefab.h"
#include "ResourceManager.h"

#include "Utils/Clock.h"
#include "EditorManager.h"

SceneManager::SceneManager()
{
    // We require a scene loaded at all times.
    // Load the startup scene when the game starts
    openScene("Resources/Scenes/startup.scene");
    assert(currentScene_ != nullptr);

    // Register menu items for creating new gameobjects
    addCreateGameObjectMenuItem<Transform>("Blank GameObject");
    addCreateGameObjectMenuItem<Camera>("Camera");
    addCreateGameObjectMenuItem<StaticMesh>("Static Mesh");
    addCreateGameObjectMenuItem<Terrain>("Terrain");

    // Add a create scene menu item
    MainWindowMenu::instance()->addMenuItem("File/New Scene", [&] {
        const std::string path = EditorManager::instance()->showSaveDialog("New Scene", "newscene", "scene");
        if (path.empty() == false)
        {
            createScene(path);
        }
    });
}

void SceneManager::frameStart()
{
    const float deltaTime = Clock::instance()->deltaTime();

    // Trigger updates for all scene gameobjects
    for (GameObject* gameObject : sceneGameObjects_)
    {
        gameObject->update(deltaTime);
    }

    // Trigger updates for all hidden gameobjects too
    for (GameObject* gameObject : hiddenGameObjects_)
    {
        gameObject->update(deltaTime);
    }
}

void SceneManager::openScene(const std::string& scenePath)
{
    // Change the current scene
    currentScene_ = ResourceManager::instance()->load<Scene>(scenePath);

    // Delete all existing scene gameobjects
    while (sceneGameObjects_.empty() == false)
    {
        delete sceneGameObjects_[0];
    }

    // Create the new objects from the scene
    currentScene_->createGameObjects(sceneGameObjects_);
}

void SceneManager::createScene(const std::string& scenePath)
{
    ResourceManager::instance()->createResource<Scene>(scenePath);
    openScene(scenePath);
}

void SceneManager::saveScene()
{
    currentScene_->saveGameObjects(sceneGameObjects_);
}

GameObject* SceneManager::createGameObject(const std::string& name, Transform* parent, bool hidden)
{
    GameObject* go = new GameObject(name);
    if (parent != nullptr)
    {
        go->createComponent<Transform>()->setParentTransform(parent);
    }

    if (hidden)
    {
        hiddenGameObjects_.push_back(go);
    }
    else
    {
        sceneGameObjects_.push_back(go);
    }

    return go;
}

GameObject* SceneManager::createGameObject(Prefab* prefab, bool hidden)
{
    GameObject* go = new GameObject(prefab);
    if (hidden)
    {
        hiddenGameObjects_.push_back(go);
    }
    else
    {
        sceneGameObjects_.push_back(go);
    }

    return go;
}

const std::vector<StaticMesh*> SceneManager::staticMeshes() const
{
    // Make a vector to store the meshes
    std::vector<StaticMesh*> meshes;

    // Check every game object for a StaticMesh component
    for (GameObject* gameObject : sceneGameObjects_)
    {
        StaticMesh* mesh = gameObject->findComponent<StaticMesh>();
        if (mesh != nullptr)
        {
            meshes.push_back(mesh);
        }
    }
    for (GameObject* gameObject : hiddenGameObjects_)
    {
        StaticMesh* mesh = gameObject->findComponent<StaticMesh>();
        if (mesh != nullptr)
        {
            meshes.push_back(mesh);
        }
    }

    return meshes;
}

const std::vector<Terrain*> SceneManager::terrains() const
{
    // Make a vector to store the meshes
    std::vector<Terrain*> terrains;

    // Check every game object for a terrain component
    for (GameObject* gameObject : sceneGameObjects_)
    {
        Terrain* terrain = gameObject->findComponent<Terrain>();
        if (terrain != nullptr)
        {
            terrains.push_back(terrain);
        }
    }
    for (GameObject* gameObject : hiddenGameObjects_)
    {
        Terrain* terrain = gameObject->findComponent<Terrain>();
        if (terrain != nullptr)
        {
            terrains.push_back(terrain);
        }
    }

    return terrains;
}

template<typename T>
void SceneManager::addCreateGameObjectMenuItem(const std::string &gameObjectName)
{
    MainWindowMenu::instance()->addMenuItem(
        "Scene/New GameObject/" + gameObjectName,
        [=] { PropertiesPanel::instance()->inspect(createGameObject(gameObjectName)->createComponent<T>()->gameObject()); }
    );

    // Add a second button, allowing the gameobject to be created as 
    // a child of the gameobject currently in the properties panel.
    MainWindowMenu::instance()->addMenuItem(
        "Scene/New Child GameObject/" + gameObjectName,
        [=] {
        GameObject* parentGO = dynamic_cast<GameObject*>(PropertiesPanel::instance()->current());
        Transform* parentTransform = (parentGO != nullptr) ? parentGO->transform() : nullptr;
        PropertiesPanel::instance()->inspect(createGameObject(gameObjectName, parentTransform)->createComponent<T>()->gameObject());
    });
}

void SceneManager::gameObjectDeleted(GameObject* go)
{
    // If the go is in the scene list, remove it
    auto found = std::find(sceneGameObjects_.begin(), sceneGameObjects_.end(), go);
    if (found != sceneGameObjects_.end())
    {
        sceneGameObjects_.erase(found);
    }

    // Also check the hidden gameobjects list, and remove it from there if found too
    auto hiddenFound = std::find(hiddenGameObjects_.begin(), hiddenGameObjects_.end(), go);
    if (hiddenFound != hiddenGameObjects_.end())
    {
        hiddenGameObjects_.erase(hiddenFound);
    }
}
