#pragma once

class Transform;

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Rigidbody : public Component
{
public:
    explicit Rigidbody(GameObject* gameObject);

    void update(float deltaTime) override;

    // The world-space velocity of the rigidbody
    // This is affected by the simulation over time, due to gravity
    Vector3 velocity() const { return velocity_; }
    void setVelocity(const Vector3 &value);

private:
    Vector3 velocity_;
};
