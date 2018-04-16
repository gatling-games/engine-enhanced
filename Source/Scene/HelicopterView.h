#pragma once

#include "GameObject.h"
#include "Component.h"

class Transform;

class HelicopterView : public Component
{
public:
    HelicopterView(GameObject* gameObject);

    // Draws the freecam properties fold out
    void drawProperties() override;

    // Serialisation function
    void serialize(PropertyTable &table) override;

    // Runs every frame to update freecam
    void update(float deltaTime) override;

private:
    Transform* transform_;

    // The current look rotation for the mouse look
    // This gets pulled back to 0 when not holding the right mouse
    float mouseLookHorizontal_;
    float mouseLookVertical_;
    float mouseLookCentringRate_;
};