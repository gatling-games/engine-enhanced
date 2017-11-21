#include "SceneManager.h"

SceneManager::SceneManager()
    : gameObjects_(),
    transforms_(),
    cameras_(),
    staticMeshes_()
{
    gameObjects_.push_back(std::unique_ptr<GameObject>(new GameObject(1, "Camera")));
    gameObjects_.push_back(std::unique_ptr<GameObject>(new GameObject(2, "Cube")));
    transforms_.push_back(std::unique_ptr<Transform>(new Transform(1)));
    transforms_[0]->setPositionLocal(Point3(0.0f, 1.0f, -2.0f));
    transforms_.push_back(std::unique_ptr<Transform>(new Transform(2)));
    cameras_.push_back(std::unique_ptr<Camera>(new Camera(1)));
    staticMeshes_.push_back(std::unique_ptr<StaticMesh>(new StaticMesh(2)));
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