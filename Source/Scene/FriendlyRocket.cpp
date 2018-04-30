#include "FriendlyRocket.h"

#include "Scene/Transform.h"
#include "Scene/Helicopter.h"
#include "Scene/Destructible.h"

#include "Physics/Rigidbody.h"

#include "SceneManager.h"

#include "imgui.h"

FriendlyRocket::FriendlyRocket(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    speed_(100.0f),
    damage_(15.0f)
{
    // Initialise rotation
    transform_->setRotationLocal(Quaternion::identity());
}

void FriendlyRocket::drawProperties()
{
    ImGui::DragFloat("Rocket speed", &speed_, 0.1f);
}

void FriendlyRocket::handleCollision(Collider* collider)
{
    Destructible* destructible = collider->gameObject()->findComponent<Destructible>();

    if (destructible != nullptr)
    {
        destructible->takeDamage(damage_);
    }
	
    // Delete gameObject on collision
    if (collider->gameObject()->findComponent<Helicopter>() == nullptr)
    {
        delete gameObject();
    }
}

void FriendlyRocket::setRocketSpeed(float speed)
{
    speed_ = speed;
}

void FriendlyRocket::initRocket(const Point3& pos, const Quaternion& rot)
{
    transform_->setPositionLocal(pos);
    transform_->setRotationLocal(rot);

    // Create rigidbody for collisions
    gameObject()->createComponent<Rigidbody>();
    Rigidbody* collider = gameObject()->findComponent<Rigidbody>();

    // Calculate velocity and send to rigidbody component
    Vector3 velocity = transform_->forwards() * speed_;
    collider->setVelocity(velocity);
}
