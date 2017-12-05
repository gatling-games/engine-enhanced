#include "SceneManager.h"
#include "imgui.h"

#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"

#include "Utils/Clock.h"

SceneManager::SceneManager()
    : gameObjects_()
{
    // Create a camera in the scene
    GameObject* cameraGO = createGameObject("Camera");
    cameraGO->createComponent<Transform>()->setPositionLocal(Point3(0.5f, 2.0f, -10.0f));
    cameraGO->createComponent<Camera>();

    // Create a cube mesh infront of the camera
    GameObject* mesh1GO = createGameObject("Cube 1");
    mesh1GO->createComponent<Transform>()->setRotationLocal(Quaternion::euler(0.0f, 30.0f, 0.0f));
    mesh1GO->createComponent<StaticMesh>();

    // Create a second cube infront of the camera
    GameObject* mesh2GO = createGameObject("Cube 2");
    mesh2GO->createComponent<Transform>()->setPositionLocal(Point3(-4.0f, 0.0f, 0.0f));
    mesh2GO->createComponent<StaticMesh>();
}

void SceneManager::drawDebugMenu()
{
    Point3 pos = gameObjects_[1]->transform()->positionLocal();
    Vector3 scale = gameObjects_[1]->transform()->scaleLocal();
    ImGui::DragFloat3("Cube Pos", (float*)&pos);
    ImGui::DragFloat3("Cube Scale", (float*)&scale);
    gameObjects_[1]->transform()->setPositionLocal(pos);
    gameObjects_[1]->transform()->setScaleLocal(scale);
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