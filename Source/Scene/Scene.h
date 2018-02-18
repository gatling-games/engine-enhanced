#pragma once

#include "Scene/GameObject.h"

#include "Serialization/SerializedObject.h"

#include "ResourceManager.h"

class Scene : public Resource, public IEditableObject, public ISerializedObject
{
    friend class ResourceManager;

public:
    Scene(ResourceID resourceID);

    // Gets a list of gameobjects in the scene
    const std::vector<std::shared_ptr<GameObject>>& gameObjects() const { return gameObjects_; }
    std::vector<std::shared_ptr<GameObject>>& gameObjects() { return gameObjects_; }

    // Implements a custom editor
    void drawEditor() override;

    // Reads or writes the scene objects.
    void serialize(PropertyTable &table) override;

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects_;
};