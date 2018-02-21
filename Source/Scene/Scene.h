#pragma once

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

    // Rendering settings
    Color ambientLight() const { return ambientLight_ * ambientIntensity_; }
    Color sunColor() const { return sunColor_ * sunIntensity_; }
    Quaternion sunRotation() const { return Quaternion::euler(sunRotation_.x, sunRotation_.y, 0.0f); }
    Color skyColorTop() const { return skyColorTop_; }
    Color skyColorBottom() const { return skyColorBottom_; }
    float skySunSize() const { return skySunSize_; }
    float skySunFalloff() const { return skySunFalloff_; }
    Color fogColor() const { return fogColor_; }
    float fogDensity() const { return fogDensity_; }
    float fogHeightFalloff() const { return fogHeightFalloff_; }

    // Opens the scene when selected
    void onOpenAction() override;

    // Implements a custom editor
    void drawEditor() override;

    // Reads or writes the scene objects.
    void serialize(PropertyTable &table) override;

    // Creates new copies of all gameobjects in the scene data.
    void createGameObjects();

    // Overwrites the current scene data to match the current gameobjects.
    void saveGameObjects();

private:
    // The serialized form of the scene gameobjects
    PropertyTable gameObjects_;

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
    Color fogColor_;
    float fogDensity_;
    float fogHeightFalloff_;
};
