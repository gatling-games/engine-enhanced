#pragma once

#include <memory>
#include <vector>

#include "Application.h"
#include "Utils/Singleton.h"

#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"

class SceneManager : public ApplicationModule, public Singleton<SceneManager>
{
public:
    SceneManager();

    // ApplicationModule overrides
    std::string name() const override { return "Scene Manager"; }
    void drawDebugMenu() override;

    // Gets the first camera added to the scene.
    Camera* mainCamera() const { return cameras_[0].get(); }
    const std::vector<std::shared_ptr<StaticMesh>>& staticMeshes() const { return staticMeshes_; }

    // Finds game objects and components by id
    GameObject* findGameObject(GameObjectID id) const;
    Transform* findTransform(GameObjectID gameObjectID) const;
    Camera* findCamera(GameObjectID gameObjectID) const;
    StaticMesh* findStaticMesh(GameObjectID gameObjectID) const;

private:
    // For now, store objects in a vector of pointers
    // This should be replaced with a more cache-friendly structure
    std::vector<std::shared_ptr<GameObject>> gameObjects_;
    std::vector<std::shared_ptr<Transform>> transforms_;
    std::vector<std::shared_ptr<Camera>> cameras_;
    std::vector<std::shared_ptr<StaticMesh>> staticMeshes_;
};
