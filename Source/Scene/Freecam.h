#pragma once

#include "GameObject.h"
#include "Component.h"

class Transform;

class Freecam : public Component
{
public:
    explicit Freecam(GameObject* gameObject);
    ~Freecam() override;

    // Runs every frame to update freecam
    void update(float deltaTime) override;

private:

    // For transforming camera
    Transform* transform_;

    // Holds duration of movement
    float moveDuration_;
};