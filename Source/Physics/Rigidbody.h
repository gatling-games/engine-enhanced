#pragma once

class Transform;

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Rigidbody : public Component
{
public:
    explicit Rigidbody(GameObject* gameObject);

    void drawProperties() override;
    void serialize(PropertyTable& table) override;
    void update(float deltaTime) override;

    // When true, the rigidbody simulates the motion of its transform
    // using its velocity (affected by gravity over time).
    bool simulating() const { return simulating_; }
    void setSimulating(bool value);

    // The world-space velocity of the rigidbody
    // This is affected by the simulation over time, due to gravity
    Vector3 velocity() const { return velocity_; }
    void setVelocity(const Vector3 &value);

private:
    bool simulating_;
    Vector3 velocity_;
};
