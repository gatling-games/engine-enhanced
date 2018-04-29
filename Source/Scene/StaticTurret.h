#pragma once

#include "Scene/Component.h"
#include "Scene/Transform.h"

class StaticTurret : public Component
{
public:
    StaticTurret(GameObject* gameObject);

    void update(float deltaTime) override;

private:
    Transform* transform_;
};