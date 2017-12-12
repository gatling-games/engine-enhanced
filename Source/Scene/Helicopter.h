#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Transform;

class Helicopter : public Component
{
public:
    Helicopter(GameObject* gameObject);

    void drawProperties() override;

    void update(float deltaTime) override;


private:

    Transform* transform_;
    Vector3 worldVelocity_;

    float thrustSpeed_;
    float pitchSpeed_;
    float yawSpeed_;
    float rollSpeed;
};