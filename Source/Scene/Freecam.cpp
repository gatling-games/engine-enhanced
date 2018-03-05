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

    // Make 2 speeds of movement
    // Use the fast mode when the shift key is pressed
    const float moveSpeed = InputManager::instance()->isKeyDown(InputKey::LShift) ? 150.0f : 35.0f;

    // Transform the camera
    transform_->translateLocal(Vector3(lateral, vertical, forward) * moveSpeed * deltaTime);

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