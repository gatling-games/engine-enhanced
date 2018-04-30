#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Transform;
class Quaternion;

class FriendlyRocket : public Component
{
public:
    FriendlyRocket(GameObject* gameObject);

    void drawProperties() override;
    void handleCollision(Collider* collider) override;

    void setRocketSpeed(float speed);
    void initRocket(const Point3& pos, const Quaternion& rot);

private:
    Transform* transform_;

    float speed_;
    float damage_;

    bool initialised_ = false;
};