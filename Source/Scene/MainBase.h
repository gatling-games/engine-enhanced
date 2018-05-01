#pragma once

#include "Scene/Component.h"

class MainBase : public Component
{
public:
    explicit MainBase(GameObject* gameObject);
    ~MainBase() override { };

    void update(float deltaTime) override;

    void expose();

    void initialise() { initialised_ = true; }

private:
    Transform* transform_;

    bool initialised_;
};