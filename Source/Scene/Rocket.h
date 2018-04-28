#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Transform;

class Rocket : public Component
{
public:
    Rocket(GameObject* gameObject);

    void drawProperties() override;
    void handleCollision(Collider* collider) override;

    void setRocketSpeed(float speed);

private:
    Transform* transform_;
	
    float speed_;

    bool initialised_ = false;
};