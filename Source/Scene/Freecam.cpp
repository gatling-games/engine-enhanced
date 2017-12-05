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

    // Variables for checking whether or not camera is moving
    bool ifForward = forward > 0.1f || forward < -0.1f;
    bool ifLateral = lateral > 0.1f || lateral < -0.1f;
    bool ifVertical = vertical > 0.1f || vertical < -0.1f;

    // Add deltatime to movement duration and reset to 0 if stopped
    if (ifForward || ifLateral || ifVertical)
    {
        moveDuration_ += deltaTime;
    }
    else
    {
        moveDuration_ = 0.0f;
    }

    // Add sprint factor if key is held down
    const float speed = InputManager::instance()->isKeyDown(InputKey::LShift) ? 20.0f : 1.0f;

    float mouseDeltaX = InputManager::instance()->mouseDeltaX();
    float mouseDeltaY = InputManager::instance()->mouseDeltaY();

    // Transform camera
    transform_->translateLocal(Vector3(lateral, vertical, forward)*moveDuration_*speed);
    transform_->rotateLocal(mouseDeltaX, Vector3::up());
    transform_->rotateLocal(mouseDeltaY, transform_->right());
}