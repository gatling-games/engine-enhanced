#pragma once

#include "Scene/Component.h"

class ChopperGun : public Component
{
public:
    ChopperGun(GameObject* gameObject);

    void serialize(PropertyTable &table);
    void drawProperties() override;
    void update(float deltaTime) override;
    void handleInput(const InputCmd &inputs);

    void spawnPrefab();

private:
    Transform* transform_;
    Prefab* prefab_ = nullptr;

    float timeSinceShot_;
    float refireTime_;
};