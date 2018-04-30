#pragma once

#include "Scene/Component.h"

class Destructible : public Component
{
public:
    explicit Destructible(GameObject* gameObject);
    ~Destructible() override { };

    void drawProperties() override;

    void serialize(PropertyTable &table) override;

    void takeDamage(float damage);

private:
    float health_;
};
