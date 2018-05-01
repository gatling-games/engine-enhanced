#include "ChopperGun.h"

#include "Scene/Transform.h"
#include "Scene/FriendlyRocket.h"
#include "Scene/Helicopter.h"

#include "Math/Quaternion.h"
#include "Serialization/Prefab.h"

#include "InputManager.h"

#include "imgui.h"
#include "Utils/ImGuiExtensions.h"

ChopperGun::ChopperGun(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    timeSinceShot_(0.0f),
    refireTime_(0.4f)
{
    transform_->setRotationLocal(Quaternion::identity());
}

void ChopperGun::serialize(PropertyTable &table)
{
    table.serialize("prefab", prefab_);
    table.serialize("refire_time", refireTime_, 0.4f);
}

void ChopperGun::drawProperties()
{
    ImGui::ResourceSelect<Prefab>("Prefab", "Select Prefab", prefab_);
    ImGui::DragFloat("Refire Time", &refireTime_, 0.1f);
}

void ChopperGun::update(float deltaTime)
{
    // Add delta time to elapsed time since shot
    timeSinceShot_ += deltaTime;
}

void ChopperGun::handleInput(const InputCmd &inputs)
{
    if (inputs.firing > 0.0f && timeSinceShot_ >= refireTime_)
    {
        spawnPrefab();
        timeSinceShot_ = 0.0f;
    }
}

void ChopperGun::spawnPrefab()
{
    if (prefab_ != nullptr)
    {
        // Create new gameObject using prefab and set parent transform
        GameObject* projectile = new GameObject("Projectile", prefab_);
        Vector3 vel = transform_->parentTransform()->gameObject()->findComponent<Helicopter>()->velocity();
        projectile->findComponent<FriendlyRocket>()->initRocket(transform_->positionWorld(), transform_->rotationWorld(), vel);
    }
}