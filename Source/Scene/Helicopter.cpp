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
    remainingYaw_(0.0f),
    remainingPitch_(0.0f),
    currentTilt_(0.0f),
    turnFactor_(0.2f),
    decelerationFactor_(0.08f)
{
    transform_ = gameObject->createComponent<Transform>();
    transform_->setRotationLocal(Quaternion::identity());
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

    // Get yaw rotation based on mouse movement
    float yaw = InputManager::instance()->inputs.yawAcceleration;
    remainingYaw_ += yaw;

    // Get pitch rotation based on mouse movement
    float pitch = InputManager::instance()->inputs.pitchAcceleration;
    remainingPitch_ += pitch;

    // Clamp tilt to max angle of 45 degrees
    if (currentTilt_ >= 45.0f)
    {
        remainingPitch_ = std::min(remainingPitch_, 0.0f);
    }

    if (currentTilt_ <= -45.0f)
    {
        remainingPitch_ = std::max(remainingPitch_, 0.0f);
    }

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

    // Copy world rotation and extract rotation about y axis
    Quaternion yawQuaternion = worldRotation_;
    yawQuaternion.x = 0.0f;
    yawQuaternion.z = 0.0f;

    yawQuaternion.normalize();

    // Lerp world velocity and translate, modifying horizontal velocity to account for orientation
    worldVelocity_ = Vector3::lerp(worldVelocity_, desiredVelocity, 0.5f * deltaTime);
    transform_->translateWorld(yawQuaternion * worldVelocity_ * deltaTime);

    // Rotate percentage of remaining yaw
    transform_->rotateLocal(remainingYaw_ * turnFactor_ * deltaTime, Vector3::up());

    // Construct yaw rotation and modify world rotation
    // This forces translation to be dependent on rotation
    Quaternion newRotation = Quaternion::rotation(remainingYaw_ * turnFactor_ * deltaTime, Vector3::up());
    worldRotation_ = newRotation * worldRotation_;
    `
    // Reduce remaining yaw
    remainingYaw_ -= remainingYaw_ * decelerationFactor_;

    // Rotate percentage of remaining pitch and modify tracked forward tilt angle
    transform_->rotateLocal(remainingPitch_ * turnFactor_ * deltaTime, transform_->right());
    currentTilt_ += remainingPitch_ * turnFactor_ * deltaTime;

    // Reduce remaining pitch
    remainingPitch_ -= remainingPitch_ * decelerationFactor_;
}

