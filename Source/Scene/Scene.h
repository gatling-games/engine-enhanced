#pragma once

#include "Scene/GameObject.h"

#include "Serialization/SerializedObject.h"

#include "ResourceManager.h"

#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Math/Quaternion.h"
#include "Renderer/UniformBuffer.h"

class Scene : public Resource, public IEditableObject, public ISerializedObject
{
    friend class ResourceManager;

public:
    Scene(ResourceID resourceID);

    // Gets a list of gameobjects in the scene
    const std::vector<std::shared_ptr<GameObject>>& gameObjects() const { return gameObjects_; }
    std::vector<std::shared_ptr<GameObject>>& gameObjects() { return gameObjects_; }

    // Rendering settings
    Color ambientLight() const { return ambientLight_ * ambientIntensity_; }
    Color sunColor() const { return sunColor_ * sunIntensity_; }
    Quaternion sunRotation() const { return Quaternion::euler(sunRotation_.x, sunRotation_.y, 0.0f); }
    Color skyColorTop() const { return skyColorTop_; }
    Color skyColorBottom() const { return skyColorBottom_; }
    float skySunSize() const { return skySunSize_; }
    float skySunFalloff() const { return skySunFalloff_; }

    // Implements a custom editor
    void drawEditor() override;

    // Reads or writes the scene objects.
    void serialize(PropertyTable &table) override;

private:
    // All gameobjects saved in the scene
    std::vector<std::shared_ptr<GameObject>> gameObjects_;

    // Rendering settings
    Color ambientLight_;
    float ambientIntensity_;
    Color sunColor_;
    float sunIntensity_;
    Vector2 sunRotation_;
    Color skyColorTop_;
    Color skyColorBottom_;
    float skySunSize_;
    float skySunFalloff_;
};