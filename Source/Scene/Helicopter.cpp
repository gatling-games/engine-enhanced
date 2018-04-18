#include "Helicopter.h"
#include "InputManager.h"

#include "Scene/Transform.h"
#include "imgui.h"

#include <algorithm>

Helicopter::Helicopter(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
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

}

void Helicopter::drawProperties()
{
    // Debugging helicopter controls
    ImGui::DragFloat("Max lateral speed", &horizontalMaxSpeed_, 0.1f);
    ImGui::DragFloat("Max upward speed", &upMaxSpeed_, 0.1f);
    ImGui::DragFloat("Max downward speed", &downMaxSpeed_);
    ImGui::DragFloat("Deceleration factor", &decelerationFactor_, 0.1f);
    ImGui::DragFloat("Yaw rotation factor", &turnFactor_, 0.1f);
}

void Helicopter::serialize(PropertyTable &table)
{
    table.serialize("horizontal_max_speed", horizontalMaxSpeed_, 60.0f);
    table.serialize("up_max_speed", upMaxSpeed_, 25.0f);
    table.serialize("down_max_speed", downMaxSpeed_, 80.0f);
    table.serialize("turn_factor", turnFactor_, 0.2f);
    table.serialize("deceleration_factor", decelerationFactor_, 0.08f);
}

void Helicopter::handleInput(const InputCmd& inputs)
{
    // Get input axes (scale from -1 to 1)
    Vector3 desiredVelocity;
    desiredVelocity.z = inputs.forwardsMovement;
    desiredVelocity.x = inputs.sidewaysMovement;
    desiredVelocity.y = inputs.verticalMovement;

    // When the player moves diagonally, they can travel faster than 1
    // Normalize the horizontal movement to fix this
    float horizontalMoveSqrd = Vector2(desiredVelocity.x, desiredVelocity.z).sqrMagnitude();
    if (horizontalMoveSqrd > 1.0f)
    {
        desiredVelocity.x /= sqrtf(horizontalMoveSqrd);
        desiredVelocity.z /= sqrtf(horizontalMoveSqrd);
    }

    // Multiply desired velocity by max move speed
    desiredVelocity.x *= horizontalMaxSpeed_;
    desiredVelocity.y *= (desiredVelocity.y < 0.0f) ? downMaxSpeed_ : upMaxSpeed_;
    desiredVelocity.z *= horizontalMaxSpeed_;

    // Get yaw rotation based on mouse movement
    float yaw = inputs.horizontalRotation;
    remainingYaw_ += yaw;

    // Get pitch rotation based on mouse movement
    float pitch = inputs.verticalRotation;
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
    // Determine the current horizontal rotation of the helicopter and change the rotation by it
    Quaternion yawQuaternion = worldRotation_;
    yawQuaternion.x = 0.0f;
    yawQuaternion.z = 0.0f;
    desiredVelocity = yawQuaternion * desiredVelocity;

    // Lerp world velocity and translate, modifying horizontal velocity to account for orientation
    worldVelocity_ = Vector3::lerp(worldVelocity_, desiredVelocity, 0.5f * inputs.deltaTime);
    transform_->translateWorld(worldVelocity_ * inputs.deltaTime);

    // Rotate percentage of remaining yaw
    transform_->rotateLocal(remainingYaw_ * turnFactor_ * inputs.deltaTime, Vector3::up());

    // Construct yaw rotation and modify world rotation
    // This forces translation to be dependent on rotation
    Quaternion newRotation = Quaternion::rotation(remainingYaw_ * turnFactor_ * inputs.deltaTime, Vector3::up());

    worldRotation_ = newRotation * worldRotation_;

    // Reduce remaining yaw
    remainingYaw_ -= remainingYaw_ * decelerationFactor_;

    // Rotate percentage of remaining pitch and modify tracked forward tilt angle
    transform_->rotateLocal(remainingPitch_ * turnFactor_ * inputs.deltaTime, transform_->right());
    currentTilt_ += remainingPitch_ * turnFactor_ * inputs.deltaTime;

    // Reduce remaining pitch
    remainingPitch_ -= remainingPitch_ * decelerationFactor_;
}

