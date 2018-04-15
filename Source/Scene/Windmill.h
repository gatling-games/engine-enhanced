#pragma once

#include "Scene/Component.h"
#include "Math/Vector3.h"

class Transform;

class Windmill : public Component
{
public:
    Windmill(GameObject* gameObject);

    void drawProperties() override;

    void serialize(PropertyTable &table) override;

    void update(float deltaTime) override;

private:
    float rotationSpeed_;
    int axis_;
};