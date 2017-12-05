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

    // Called each frame.
    void frameStart();

    // Gets the first camera added to the scene.
    Camera* mainCamera() const;
    
    // Gets a list of all game objects in the scene.
    const std::vector<GameObject*>& gameObjects() { return gameObjects_; }

    // Gets a list of all static mesh components in the scene
    const std::vector<StaticMesh*> staticMeshes() const;

    // Finds game objects and components by id
    GameObject* findGameObject(GameObjectID id) const;

private:
    // For now, store objects in a vector of pointers
    // This should be replaced with a more cache-friendly structure
    std::vector<GameObject*> gameObjects_;
};
