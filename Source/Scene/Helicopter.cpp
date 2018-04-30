#include "Helicopter.h"
#include "InputManager.h"

#include "Scene/Transform.h"
#include "Scene/StaticMesh.h"
#include "Utils/Clock.h"

#include "imgui.h"

#include <algorithm>

Helicopter::Helicopter(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    worldVelocity_(Vector3::zero()),
    worldRotation_(Quaternion::identity()),
    HP(100.0f),
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

void Helicopter::handleCollision(Collider* collider)
{
    if (collider->gameObject()->findComponent<Terrain>() != nullptr)
    {
        die();
    }
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
    // We first need to find the world-space axes that the three inputs correspond with
    Vector3 forwardsDir = transform_->forwards();
    forwardsDir.y = 0.0f;
    forwardsDir = forwardsDir.normalized();
    Vector3 upwardsDir = Vector3::up();
    Vector3 sidewaysDir = transform_->right();
    sidewaysDir.y = 0.0f;
    sidewaysDir = sidewaysDir.normalized();

    // Combine the forwards and sideways velocities.
    // The vertical is added later.
    Vector3 desiredVelocity =
        forwardsDir * inputs.forwardsMovement
        + sidewaysDir * inputs.sidewaysMovement;

    // When the player moves diagonally, they can travel faster than 1
    // Normalize the horizontal movement to fix this
    float horizontalMoveSqrd = desiredVelocity.sqrMagnitude();
    if (horizontalMoveSqrd > 1.0f)
    {
        desiredVelocity = desiredVelocity.normalized();
    }

    // Now the horizontal velocity has been clamped at 1, add the vertical movement
    desiredVelocity += upwardsDir * inputs.verticalMovement;

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

void Helicopter::takeDamage(float damage)
{
    HP -= damage;

    if (HP <= 0.0f)
    {
        die();
    }
}

void Helicopter::die()
{
    gameObject()->findComponent<StaticMesh>()->setMaterial(ResourceManager::instance()->load<Material>("Resources/Materials/cardboard_enemy.material"));
    Clock::instance()->setPaused(true);
}