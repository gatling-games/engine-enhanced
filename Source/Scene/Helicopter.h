#pragma once

#include "Scene/Component.h"
#include "Physics/Collider.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

class Transform;
struct InputCmd;

class Helicopter : public Component
{
public:
    Helicopter(GameObject* gameObject);

    void drawProperties() override;

    void handleCollision(Collider* collider) override;

    // Serialisation method
    void serialize(PropertyTable &table) override;

    void handleInput(const InputCmd& inputs) override;

    Transform* transform() { return transform_; };

    Vector3 velocity() { return worldVelocity_; }

    void takeDamage(float damage);

    void die();

private:

    Transform* transform_;
    Vector3 worldVelocity_;
    Quaternion worldRotation_;

    float health_;

    float currentTilt_;

    float horizontalMaxSpeed_;
    float upMaxSpeed_;
    float downMaxSpeed_;
    float remainingYaw_;
    float remainingPitch_;
    float turnFactor_;
    float decelerationFactor_;
};