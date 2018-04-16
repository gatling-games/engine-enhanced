#include "SceneManager.h"

#include "Application.h"

#include "Editor/MainWindowMenu.h"
#include "Editor/PropertiesPanel.h"

#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"
#include "Scene/Terrain.h"

#include "ResourceManager.h"
#include "InputManager.h"

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

    // Trigger updates for all gameobjects
    for (GameObject* gameObject : gameObjects_)
    {
        gameObject->update(deltaTime);
    }
}

void SceneManager::openScene(const std::string& scenePath)
{
    // If we are reloading the current scene, the scene values (lighting settings etc) 
    // may need to be reset.
    ResourceManager::instance()->importResource(ResourceManager::instance()->pathToResourceID(scenePath));

    // Change the current scene
    currentScene_ = ResourceManager::instance()->load<Scene>(scenePath);

    // Delete all scene gameobjects (except ones with the SurviveSceneChanges flag)
    for(size_t i = gameObjects_.size() - 1; i < gameObjects_.size(); --i)
    {
        if(gameObjects_[i]->hasFlag(GameObjectFlag::SurviveSceneChanges) == false)
        {
            delete gameObjects_[i];
        }
    }

    // Create the new objects from the scene
    currentScene_->createGameObjects();
}

void SceneManager::createScene(const std::string& scenePath)
{
    ResourceManager::instance()->createResource<Scene>(scenePath);
    openScene(scenePath);
}

void SceneManager::saveScene()
{
    // Do not save scenes when in play mode
    if(Application::instance()->isPlaying())
    {
        return;
    }

    currentScene_->saveGameObjects();
}

Camera* SceneManager::mainCamera() const
{
    for (GameObject* gameObject : gameObjects_)
    {
        Camera* camera = gameObject->findComponent<Camera>();
        if (camera != nullptr && gameObject->hasFlag(GameObjectFlag::NotShownInScenePanel) == false)
        {
            return camera;
        }
    }

    return nullptr;
}

void SceneManager::handleInput(const InputCmd& inputs)
{
    for (GameObject* gameObject : gameObjects_)
    {
        gameObject->handleInput(inputs);
    }
}

const std::vector<StaticMesh*> SceneManager::staticMeshes() const
{
    // Make a vector to store the meshes
    std::vector<StaticMesh*> meshes;

    // Check every game object for a StaticMesh component
    for (GameObject* gameObject : gameObjects_)
    {
        StaticMesh* mesh = gameObject->findComponent<StaticMesh>();
        if (mesh != nullptr)
        {
            meshes.push_back(mesh);
        }
    }

    return meshes;
}

const Terrain* SceneManager::terrain() const
{
    // Check every game object for a terrain component
    for (GameObject* gameObject : gameObjects_)
    {
        Terrain* terrain = gameObject->findComponent<Terrain>();
        if (terrain != nullptr)
        {
            return terrain;
        }
    }

    return nullptr;
}

const std::vector<Shield*> SceneManager::shields() const
{
    // Make a vector to store the shields
    std::vector<Shield*> shields;

    // Check every game object for a Shield component
    for (GameObject* gameObject : gameObjects_)
    {
        Shield* shield = gameObject->findComponent<Shield>();
        if (shield != nullptr)
        {
            shields.push_back(shield);
        }
    }

    return shields;
}

template<typename T>
void SceneManager::addCreateGameObjectMenuItem(const std::string &gameObjectName)
{
    MainWindowMenu::instance()->addMenuItem(
        "Scene/New GameObject/" + gameObjectName,
        [=] { 
            GameObject* go = new GameObject(gameObjectName);
            go->createComponent<T>();
            PropertiesPanel::instance()->inspect(go); 
        }
    );

    // Add a second button, allowing the gameobject to be created as 
    // a child of the gameobject currently in the properties panel.
    MainWindowMenu::instance()->addMenuItem(
        "Scene/New Child GameObject/" + gameObjectName,
        [=] {
            GameObject* parentGO = dynamic_cast<GameObject*>(PropertiesPanel::instance()->current());
            Transform* parentTransform = (parentGO != nullptr) ? parentGO->transform() : nullptr;
            GameObject* go = new GameObject(gameObjectName);
            go->transform()->setParentTransform(parentTransform);
            go->createComponent<T>();
            PropertiesPanel::instance()->inspect(go);
        }
    );
}

void SceneManager::gameObjectCreated(GameObject* go)
{
    // Ensure the object does not already exist in the gameobject list
    assert(std::find(gameObjects_.begin(), gameObjects_.end(), go) == gameObjects_.end());

    // Add to the gameobjects list
    gameObjects_.push_back(go);
}

void SceneManager::gameObjectDeleted(GameObject* go)
{
    // If the go is in the scene list, remove it
    auto found = std::find(gameObjects_.begin(), gameObjects_.end(), go);
    if (found != gameObjects_.end())
    {
        gameObjects_.erase(found);
    }
}
