#include "SceneManager.h"
#include "imgui.h"

SceneManager::SceneManager()
    : gameObjects_(),
    transforms_(),
    cameras_(),
    staticMeshes_()
{
    gameObjects_.push_back(std::shared_ptr<GameObject>(new GameObject(1, "Camera")));
    gameObjects_.push_back(std::shared_ptr<GameObject>(new GameObject(2, "Cube")));
    gameObjects_.push_back(std::shared_ptr<GameObject>(new GameObject(3, "Cube")));
    transforms_.push_back(std::shared_ptr<Transform>(new Transform(1)));
    transforms_[0]->setPositionLocal(Point3(0.5f, 2.0f, -10.0f));
    transforms_.push_back(std::shared_ptr<Transform>(new Transform(2)));
    transforms_[1]->setRotationLocal(Quaternion::euler(0.0f, 30.0f, 0.0f));
    transforms_.push_back(std::shared_ptr<Transform>(new Transform(3)));
    transforms_[2]->setPositionLocal(Point3(-4.0f, 0.0f, 0.0f));
    cameras_.push_back(std::shared_ptr<Camera>(new Camera(1)));
    staticMeshes_.push_back(std::shared_ptr<StaticMesh>(new StaticMesh(2)));
    staticMeshes_.push_back(std::shared_ptr<StaticMesh>(new StaticMesh(3)));
}

void SceneManager::drawDebugMenu()
{
    Point3 pos = transforms_[1]->positionLocal();
    Vector3 scale = transforms_[1]->scaleLocal();
    ImGui::DragFloat3("Cube Pos", (float*)&pos);
    ImGui::DragFloat3("Cube Scale", (float*)&scale);
    transforms_[1]->setPositionLocal(pos);
    transforms_[1]->setScaleLocal(scale);
}

GameObject* SceneManager::findGameObject(GameObjectID id) const
{
    // Search the game objects list for a match.
    for (unsigned int i = 0; i < gameObjects_.size(); ++i)
    {
        if (gameObjects_[i]->id() == id)
        {
            return gameObjects_[i].get();
        }
    }

    // No match
    return nullptr;
}

Transform* SceneManager::findTransform(GameObjectID gameObjectID) const
{
    // Search the component list for a match.
    for (unsigned int i = 0; i < transforms_.size(); ++i)
    {
        if (transforms_[i]->gameObjectID() == gameObjectID)
        {
            return transforms_[i].get();
        }
    }

    // No match
    return nullptr;
}

Camera* SceneManager::findCamera(GameObjectID gameObjectID) const
{
    // Search the component list for a match.
    for (unsigned int i = 0; i < cameras_.size(); ++i)
    {
        if (cameras_[i]->gameObjectID() == gameObjectID)
        {
            return cameras_[i].get();
        }
    }

    // No match
    return nullptr;
}

StaticMesh* SceneManager::findStaticMesh(GameObjectID gameObjectID) const
{
    // Search the component list for a match.
    for (unsigned int i = 0; i < staticMeshes_.size(); ++i)
    {
        if (staticMeshes_[i]->gameObjectID() == gameObjectID)
        {
            return staticMeshes_[i].get();
        }
    }

    // No match
    return nullptr;
}