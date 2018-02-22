#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

class Transform;

class Helicopter : public Component
{
public:
    Helicopter(GameObject* gameObject);

    void drawProperties() override;

    // Serialisation method
    void serialize(PropertyTable &table) override;

    void update(float deltaTime) override;

private:

    Transform* transform_;
    Vector3 worldVelocity_;
    Quaternion worldRotation_;

    float horizontalMaxSpeed_;
    float upMaxSpeed_;
    float downMaxSpeed_;
    float remainingRotation_;
    float turnFactor_;
    float decelerationFactor_;
};