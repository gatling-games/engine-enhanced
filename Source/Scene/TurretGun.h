#pragma once

#include "Scene/Component.h"

class Prefab;

class TurretGun : public Component
{
public:
    TurretGun(GameObject* gameObject);

    void serialize(PropertyTable &table);
    void drawProperties() override;
    void update(float deltaTime) override;

    void spawnPrefab();

private:
    Transform* transform_;
    Prefab* prefab_ = nullptr;

    float timeSinceShot_;
    float refireTime_;
};