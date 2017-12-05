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
    gameObjects_.push_back(new GameObject(1, "Camera"));
    gameObjects_.push_back(new GameObject(2, "Cube"));
    gameObjects_.push_back(new GameObject(3, "Cube"));
    gameObjects_[0]->createComponent<Transform>()->setPositionLocal(Point3(0.5f, 2.0f, -10.0f));
    gameObjects_[1]->createComponent<Transform>()->setRotationLocal(Quaternion::euler(0.0f, 30.0f, 0.0f));
    gameObjects_[2]->createComponent<Transform>()->setPositionLocal(Point3(-4.0f, 0.0f, 0.0f));
    gameObjects_[0]->createComponent<Camera>();
    gameObjects_[1]->createComponent<StaticMesh>();
    gameObjects_[2]->createComponent<StaticMesh>();
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

    for(unsigned int i = 0; i < gameObjects_.size(); ++i)
    {
        gameObjects_[i]->update(deltaTime);
    }
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

GameObject* SceneManager::findGameObject(GameObjectID id) const
{
    // Search the game objects list for a match.
    for (unsigned int i = 0; i < gameObjects_.size(); ++i)
    {
        if (gameObjects_[i]->id() == id)
        {
            return gameObjects_[i];
        }
    }

    // No match
    return nullptr;
}