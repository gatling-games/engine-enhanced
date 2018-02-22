#include "Helicopter.h"
#include "InputManager.h"

#include "Scene/Transform.h"
#include "imgui.h"

#include <algorithm>

Helicopter::Helicopter(GameObject* gameObject)
    : Component(gameObject),
    worldVelocity_(Vector3::zero()),
    worldRotation_(Quaternion::identity()),
    horizontalMaxSpeed_(60.0f),
    upMaxSpeed_(25.0f),
    downMaxSpeed_(80.0f),
    remainingRotation_(0.0f),
    turnFactor_(0.2f),
    decelerationFactor_(0.08f)
{
    transform_ = gameObject->createComponent<Transform>();
}

void Helicopter::drawProperties()
{
    // Debugging helicopter controls
    ImGui::DragFloat("Thrust speed", &horizontalMaxSpeed_, 0.1f);
    ImGui::DragFloat3("Velocity", &worldVelocity_.x, 0.1f);
}

void Helicopter::serialize(PropertyTable &table)
{
    table.serialize("horizontal_max_speed", horizontalMaxSpeed_, 60.0f);
    table.serialize("up_max_speed", upMaxSpeed_, 25.0f);
    table.serialize("down_max_speed", downMaxSpeed_, 80.0f);
    table.serialize("turn_factor", turnFactor_, 0.2f);
    table.serialize("deceleration_factor", decelerationFactor_, 0.08f);
}

void Helicopter::update(float deltaTime)
{
    // Get input axes (scale from -1 to 1)
    Vector3 axis = InputManager::instance()->inputs.axes;
    Vector3 desiredVelocity;
    desiredVelocity.z = axis.z;
    desiredVelocity.x = axis.x;
    desiredVelocity.y = axis.y;

    // Get frame rotation based on mouse movement
    float yaw = InputManager::instance()->inputs.rotationalAcceleration;
    remainingRotation_ += yaw;

    // Ensure total horizontal movement is no greater than thrust speed
    float horizontalMoveSqrd = Vector2(desiredVelocity.x, desiredVelocity.z).sqrMagnitude();
    if (horizontalMoveSqrd > 1.0f)
    {
        // Scale factor to ensure magnitude of horizontal velocity <= thrustSpeed_
        desiredVelocity.z /= horizontalMoveSqrd;
        desiredVelocity.x /= horizontalMoveSqrd;
    }

    // Multiply desired velocity by max move speed
    desiredVelocity.x *= horizontalMaxSpeed_;
    desiredVelocity.z *= horizontalMaxSpeed_;

    // Simulate effects of gravity
    if (desiredVelocity.y > 0.0f)
    {
        desiredVelocity.y *= upMaxSpeed_;
    }
    else
    {
        desiredVelocity.y *= downMaxSpeed_;
    }

    // Lerp world velocity and translate
    worldVelocity_ = Vector3::lerp(worldVelocity_, desiredVelocity, 0.5f * deltaTime);
    transform_->translateLocal(worldVelocity_ * deltaTime);

    // Rotate percentage of remaining rotation and reduce remaining rotation amount
    transform_->rotateLocal(remainingRotation_ * turnFactor_ * deltaTime, Vector3(0.0f, 1.0f, 0.0f));
    remainingRotation_ -= remainingRotation_ * decelerationFactor_;
}

