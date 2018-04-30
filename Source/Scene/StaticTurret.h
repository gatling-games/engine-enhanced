#pragma once

#include "Scene/Component.h"
#include "Scene/Transform.h"
#include "Scene/Helicopter.h"

class StaticTurret : public Component
{
public:
    StaticTurret(GameObject* gameObject);

    void update(float deltaTime) override;

private:
    Transform* transform_;

    float rotationCap_ = 45.0f;
    float rotationSpeed_ = 90.0f;

    Vector3 getChopperPredictedPosition(Helicopter* chopper);
};