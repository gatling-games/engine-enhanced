#include "HelicopterView.h"

#include "InputManager.h"
#include "Scene/Transform.h"
#include "Utils/Clock.h"
#include "imgui.h"

HelicopterView::HelicopterView(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->transform()),
    mouseLookHorizontal_(0.0f),
    mouseLookVertical_(0.0f),
    mouseLookCentringRate_(2.0f)
{

}

void HelicopterView::drawProperties()
{
    ImGui::SliderFloat("MouseLook Centering", &mouseLookCentringRate_, 0.5f, 10.0f);
}

void HelicopterView::serialize(PropertyTable& table)
{
    table.serialize("mouse_look_centring_rate", mouseLookCentringRate_, 1.0f);
}

void HelicopterView::update(float deltaTime)
{
    // Skip when the game is paused
    if(Clock::instance()->paused())
    {
        return;
    }

    // If we have the right mouse button held down, apply mouse look
    if (InputManager::instance()->mouseButtonDown(MouseButton::Right))
    {
        mouseLookHorizontal_ += InputManager::instance()->mouseDeltaX();
        mouseLookVertical_ += InputManager::instance()->mouseDeltaY();

        // Clamp the rotation to prevent the player looking backwards
        if (fabs(mouseLookHorizontal_) > 80.0f) mouseLookHorizontal_ = (mouseLookHorizontal_ > 0.0f ? 80.0f : -80.0f);
        if (fabs(mouseLookVertical_) > 80.0f) mouseLookVertical_ = (mouseLookVertical_ > 0.0f ? 80.0f : -80.0f);
    }
    // Otherwise, move the mouse look back towards 0
    else
    {
        mouseLookHorizontal_ *= (1.0f - deltaTime * mouseLookCentringRate_);
        mouseLookVertical_ *= (1.0f - deltaTime * mouseLookCentringRate_);
    }

    transform_->setRotationLocal(Quaternion::euler(mouseLookVertical_, mouseLookHorizontal_, 0.0f));
}
