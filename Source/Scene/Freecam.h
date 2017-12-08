#pragma once

#include "GameObject.h"
#include "Component.h"

class Transform;

class Freecam : public Component
{
public:
    explicit Freecam(GameObject* gameObject);
    ~Freecam() override { }

    // Draws the freecam properties fold out
    void drawProperties() override;

    // Runs every frame to update freecam
    void update(float deltaTime) override;

private:
    bool timeScaleIndependent_;

    // For transforming camera
    Transform* transform_;

    // Holds duration of movement
    float moveDuration_;
};