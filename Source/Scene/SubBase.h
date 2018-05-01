#pragma once

#include "Scene/Component.h"

class SubBase : public Component
{
public:
    explicit SubBase(GameObject* gameObject);
    ~SubBase() override { };
};