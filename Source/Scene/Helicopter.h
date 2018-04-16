#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

class Transform;
struct InputCmd;

class Helicopter : public Component
{
public:
    Helicopter(GameObject* gameObject);

    void drawProperties() override;

    // Serialisation method
    void serialize(PropertyTable &table) override;

    void update(float deltaTime) override;

    void handleInput(const InputCmd& inputs) override;

private:

    Transform* transform_;
    Vector3 worldVelocity_;
    Quaternion worldRotation_;

    float currentTilt_;

    float horizontalMaxSpeed_;
    float upMaxSpeed_;
    float downMaxSpeed_;
    float remainingYaw_;
    float remainingPitch_;
    float turnFactor_;
    float decelerationFactor_;
};