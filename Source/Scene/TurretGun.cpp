#include "TurretGun.h"

#include "Scene/Transform.h"
#include "Math/Quaternion.h"
#include "Serialization/Prefab.h"

#include "imgui.h"
#include "Utils/ImGuiExtensions.h"

TurretGun::TurretGun(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    timeSinceShot_(0.0f),
    refireTime_(2.5f)
{
    transform_->setRotationLocal(Quaternion::identity());
}

void TurretGun::serialize(PropertyTable &table)
{
    table.serialize("prefab", prefab_);
    table.serialize("refire_time", refireTime_, 2.5f);
}
void TurretGun::drawProperties()
{
    ImGui::ResourceSelect<Prefab>("Prefab", "Select Prefab", prefab_);
    ImGui::DragFloat("Refire time", &refireTime_, 0.1f);
}

void TurretGun::update(float deltaTime)
{
    // Add delta time to elapsed time since shot
    timeSinceShot_ += deltaTime;
    
    // If elapsed time exceeds refire time, fire projectile
    if (timeSinceShot_ >= refireTime_)
    {
        spawnPrefab();
        timeSinceShot_ = 0.0f;
    }
}

void TurretGun::spawnPrefab()
{
    if (prefab_ != nullptr)
    {
        // Create new gameObject using prefab and set parent transform
        GameObject* projectile = new GameObject("Projectile", prefab_);
        projectile->transform()->setParentTransform(transform_);
    }
}