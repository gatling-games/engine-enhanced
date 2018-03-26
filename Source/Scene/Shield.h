#pragma once

#include "Component.h"

class Shield : public Component
{
public:
    explicit Shield(GameObject* gameObject);

    // Basic property getters
    float radius() const { return radius_; }

    // Draws the properties editor
    void drawProperties() override;

    // Saves and loads property values
    void serialize(PropertyTable &table) override;

private:
    float radius_;
};
