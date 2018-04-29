#pragma once

#include "Scene/Component.h"
#include "Scene/Transform.h"
#include "Scene/Helicopter.h"

class StaticTurret : public Component
{
public:
    StaticTurret(GameObject* gameObject);

    void update(float deltaTime) override;

    std::vector<Helicopter*> choppers;

private:

    Transform* transform_;
};