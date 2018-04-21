#include "Rigidbody.h"

#include "imgui.h"

#include "Scene/Transform.h"
#include "Collider.h"
#include "SceneManager.h"

Rigidbody::Rigidbody(GameObject* gameObject)
    : Component(gameObject),
    simulating_(true),
    velocity_(Vector3::zero())
{

}

void Rigidbody::drawProperties()
{
    ImGui::Checkbox("Simulating", &simulating_);
}

void Rigidbody::serialize(PropertyTable& table)
{
    table.serialize("simulating", simulating_, true);
}

void Rigidbody::update(float deltaTime)
{
    // Apply gravity to the velocity
    velocity_.y -= 9.81f * deltaTime;

    // Then apply the velocity to the transform position
    gameObject()->transform()->translateWorld(velocity_ * deltaTime);

    // Check for a collision with the scene
    const Point3 rigidbodyPoint = gameObject()->transform()->positionWorld();
    const float rigidbodyRadius = 0.5f;
    for(Collider* collider : SceneManager::instance()->colliders())
    {
        ColliderHit hit;
        if(!collider->checkForCollision(rigidbodyPoint, rigidbodyRadius, hit))
        {
            continue;
        }

        velocity_ *= -0.7f;
    }
}

void Rigidbody::setSimulating(bool value)
{
    simulating_ = value;
}

void Rigidbody::setVelocity(const Vector3& value)
{
    velocity_ = value;
}
