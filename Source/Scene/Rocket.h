#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Transform;
class Quaternion;

class Rocket : public Component
{
public:
    Rocket(GameObject* gameObject);

    void drawProperties() override;
    void handleCollision(Collider* collider) override;

    void setRocketSpeed(float speed);
    void initRocket(const Point3& pos, const Quaternion& rot);

private:
    Transform* transform_;
	
    float speed_;

    bool initialised_ = false;
};