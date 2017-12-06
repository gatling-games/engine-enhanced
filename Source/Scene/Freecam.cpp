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
    const float vertical = InputManager::instance()->getAxis(InputKey::E, InputKey::Q);

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

    // Determine the direction to move in by applying the 3 input
    // axis in the 3 local axis of the camera transform.
    Vector3 moveDirection = ((transform_->forwards() * forward)
        + (transform_->right() * lateral)
        + (transform_->up() * vertical));
  
    // Normalize the move direction, if nonzero
    float moveDirectionMagnitude = moveDirection.magnitude();
    if (moveDirectionMagnitude > 0.1f)
    {
        moveDirection /= moveDirectionMagnitude;
    }

    // Transform the camera
    // Use moveDuration_ so the camera gets faster as it moves for longer.
    transform_->translateLocal(moveDirection * moveDuration_);

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