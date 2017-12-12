#include "Helicopter.h"
#include "InputManager.h"

#include "Scene/Transform.h"
#include "imgui.h"

Helicopter::Helicopter(GameObject* gameObject)
    : Component(gameObject),
    worldVelocity_(Vector3::zero()),
    thrustSpeed_(35.0f),
    pitchSpeed_(40.0f),
    yawSpeed_(25.0f),
    rollSpeed(40.0f)
{
    transform_ = gameObject->createComponent<Transform>();
}

void Helicopter::drawProperties()
{
    // Debugging helicopter controls
    ImGui::DragFloat("Thrust speed", &thrustSpeed_, 0.1f);
    ImGui::DragFloat("Pitch speed", &pitchSpeed_, 0.1f);
    ImGui::DragFloat("Roll speed", &rollSpeed, 0.1f);
    ImGui::DragFloat("Yaw speed", &yawSpeed_, 0.1f);
    ImGui::DragFloat3("Velocity", &worldVelocity_.x, 0.1f);
}


void Helicopter::update(float deltaTime)
{
    // Setup simple helicopter controls
    const float forward = InputManager::instance()->getAxis(InputKey::G, InputKey::B);
    const float lateral = InputManager::instance()->getAxis(InputKey::V, InputKey::N);
    const float yaw = InputManager::instance()->getAxis(InputKey::H, InputKey::F);
    const float vertical = InputManager::instance()->getAxis(InputKey::T, InputKey::Y);

    // Thrust if holding appropriate key
    if (vertical > 0.01f)
    {
        worldVelocity_ += transform_->up() * thrustSpeed_ * deltaTime;
    }

    // Gravity
    worldVelocity_.y -= 9.81f * deltaTime;
    
    // Allow pitching, rolling, yawing, and thrust
    transform_->rotateLocal(forward * pitchSpeed_ * deltaTime, transform_->right());
    transform_->rotateLocal(lateral * rollSpeed * deltaTime, transform_->forwards());
    transform_->rotateLocal(yaw * yawSpeed_ * deltaTime, transform_->up());
    transform_->translateLocal(worldVelocity_ * deltaTime);
}

