#include "SceneManager.h"
#include "imgui.h"

#include "Editor/MainWindowMenu.h"
#include "Editor/PropertiesPanel.h"

#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"
#include "Scene/Terrain.h"
#include "Scene/Helicopter.h"

#include "Utils/Clock.h"
#include "Scene/Freecam.h"

SceneManager::SceneManager()
    : gameObjects_()
{
    // Create a camera in the scene
    GameObject* cameraGO = createGameObject("Camera");
    cameraGO->createComponent<Transform>()->setPositionLocal(Point3(245.0f, 16.0f, 110.0f));
	cameraGO->findComponent<Transform>()->setRotationLocal(Quaternion(0.0f, 0.5f, 0.0f, 0.866f));
    cameraGO->createComponent<Camera>();
    cameraGO->createComponent<Freecam>();

    // Create a terain
    GameObject* terrainGO = createGameObject("Terrain");
    terrainGO->createComponent<Transform>()->setPositionLocal(Point3(0.0f, 0.0f, 0.0f));
    terrainGO->createComponent<Terrain>();

	//Create helicopter gameobject
	GameObject* heliGO = createGameObject("Helicopter");
	heliGO->createComponent<Transform>()->setPositionLocal(Point3(250.0f, 15.0f, 112.0f));
	heliGO->findComponent<Transform>()->setScaleLocal(Point3(100.0f, 100.0f, 100.0f));
	heliGO->createComponent<StaticMesh>();
	heliGO->createComponent<Helicopter>();

    // Register menu items for creating new gameobjects
    addCreateGameObjectMenuItem<Transform>("Blank GameObject");
    addCreateGameObjectMenuItem<Camera>("Camera");
    addCreateGameObjectMenuItem<StaticMesh>("Static Mesh");
    addCreateGameObjectMenuItem<Terrain>("Terrain");
}

void SceneManager::frameStart()
{
    const float deltaTime = Clock::instance()->deltaTime();

    for (unsigned int i = 0; i < gameObjects_.size(); ++i)
    {
        gameObjects_[i]->update(deltaTime);
    }
}

GameObject* SceneManager::createGameObject(const std::string& name)
{
    GameObject* go = new GameObject(name);
    gameObjects_.push_back(go);

    return go;
}

Camera* SceneManager::mainCamera() const
{
    // Check every game object for a camera component
    for (unsigned int i = 0; i < gameObjects_.size(); ++i)
    {
        Camera* camera = gameObjects_[i]->findComponent<Camera>();
        if (camera != nullptr)
        {
            return camera;
        }
    }

    // No camera in the scene
    return nullptr;
}

const std::vector<StaticMesh*> SceneManager::staticMeshes() const
{
    // Make a vector to store the meshes
    std::vector<StaticMesh*> meshes;

    // Check every game object for a StaticMesh component
    for (unsigned int i = 0; i < gameObjects_.size(); ++i)
    {
        StaticMesh* mesh = gameObjects_[i]->findComponent<StaticMesh>();
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
    std::vector<Terrain*> meshes;

    // Check every game object for a StaticMesh component
    for (unsigned int i = 0; i < gameObjects_.size(); ++i)
    {
        Terrain* mesh = gameObjects_[i]->findComponent<Terrain>();
        if (mesh != nullptr)
        {
            meshes.push_back(mesh);
        }
    }

    return meshes;
}


template<typename T>
void SceneManager::addCreateGameObjectMenuItem(const std::string &gameObjectName)
{
    MainWindowMenu::instance()->addMenuItem(
        "Scene/New GameObject/" + gameObjectName,
        [=] { PropertiesPanel::instance()->inspect(createGameObject(gameObjectName)->createComponent<T>()->gameObject()); }
    );
}