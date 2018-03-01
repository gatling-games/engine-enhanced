#include "Freecam.h"

#include <imgui.h>

#include "Scene/Transform.h"
#include "InputManager.h"
#include "Utils/Clock.h"

Freecam::Freecam(GameObject* gameObject)
    : Component(gameObject),
    timeScaleIndependent_(true),
    moveDuration_(0.0f)
{
    // Attach transform component
    transform_ = gameObject->createComponent<Transform>();
}

void Freecam::drawProperties()
{
    ImGui::Checkbox("Timescale Independent", &timeScaleIndependent_);
}

void Freecam::serialize(PropertyTable &table)
{
    table.serialize("timescale_independent", timeScaleIndependent_, true);
}

void Freecam::update(float deltaTime)
{
    // Calculate forward, vertical and lateral movement using pressed keys
    const float forward = InputManager::instance()->getAxis(InputKey::W, InputKey::S);
    const float lateral = InputManager::instance()->getAxis(InputKey::D, InputKey::A);
    const float vertical = InputManager::instance()->getAxis(InputKey::E, InputKey::Q);

    // Deltatime is scaled by time scale
    if(timeScaleIndependent_)
    {
        deltaTime = Clock::instance()->realDeltaTime();
    }

    // Add deltatime to movement duration and reset to 0 if stopped
    if (fabs(forward) > 0.01f || fabs(lateral) > 0.01f || fabs(vertical) > 0.01f)
    {
        if (fabs(moveDuration_) < 3.0f)
        {
            moveDuration_ += deltaTime;
        }        
    }
    else
    {
        moveDuration_ = 0.0f;        
    }

    // Transform the camera
    // Use moveDuration_ so the camera gets faster as it moves for longer.
    const float moveSpeed = powf(moveDuration_, 1.5f);
    transform_->translateLocal(Vector3(lateral, vertical, forward) * moveSpeed * deltaTime * 60.0f);

    // Apply mouse look when the right mouse button is held down
    if (InputManager::instance()->mouseButtonDown(MouseButton::Right))
    {
        const float mouseDeltaX = InputManager::instance()->mouseDeltaX();
        const float mouseDeltaY = InputManager::instance()->mouseDeltaY();
        const float sensitivity = 0.5f;

        transform_->rotateLocal(mouseDeltaX * sensitivity, Vector3::up());
        transform_->rotateLocal(mouseDeltaY * sensitivity, transform_->right());
    }
}