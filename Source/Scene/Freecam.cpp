#include "Freecam.h"

#include "Scene/Transform.h"
#include "InputManager.h"

Freecam::Freecam(GameObject* gameObject)
    : Component(gameObject)
{
    // Attach transform component
    transform_ = gameObject->createComponent<Transform>();
    moveDuration_ = 0.0f;
}

Freecam::~Freecam()
{
}

void Freecam::update(float deltaTime)
{
    // Calculate forward, vertical and lateral movement using pressed keys
    const float forward = InputManager::instance()->getAxis(InputKey::W, InputKey::S);
    const float lateral = InputManager::instance()->getAxis(InputKey::D, InputKey::A);
    const float vertical = InputManager::instance()->getAxis(InputKey::Q, InputKey::E);

    // Add deltatime to movement duration and reset to 0 if stopped
    if (fabs(forward) > 0.01f || fabs(lateral) > 0.01f || fabs(vertical) > 0.01f)
    {
        moveDuration_ += deltaTime;
    }
    else
    {
        moveDuration_ -= deltaTime * 5.0f;
        if (moveDuration_ < 0.0f)
        {
            moveDuration_ = 0.0f;
        }
    }

    // Transform the camera
    // Use moveDuration_ so the camera gets faster as it moves for longer.
    transform_->translateLocal(Vector3(lateral, vertical, forward) * moveDuration_);

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