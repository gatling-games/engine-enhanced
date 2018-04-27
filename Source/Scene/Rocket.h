#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Transform;
class RigidBody;

class Rocket : public Component
{
public:
    Rocket(GameObject* gameObject);

    void drawProperties() override;
    void handleCollision(Collider* collider) override;

private:
    Transform* transform_;

    Vector3 direction_;
    float speed_;

    bool initialised_ = false;
};