#include "Rocket.h"

#include "Scene/Transform.h"
#include "Scene/Helicopter.h"
#include "Scene/Destructible.h"

#include "Physics/Rigidbody.h"

#include "SceneManager.h"

#include "imgui.h"

Rocket::Rocket(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    speed_(100.0f),
    damage_(15.0f)
{
    // Initialise rotation
    transform_->setRotationLocal(Quaternion::identity());
}

void Rocket::drawProperties()
{
    ImGui::DragFloat("Rocket speed", &speed_, 0.1f);
}

void Rocket::handleCollision(Collider* collider)
{
    Helicopter* chopper = collider->gameObject()->findComponent<Helicopter>();
    if (chopper != nullptr)
    {
        chopper->takeDamage(damage_);
    }

    Destructible* destructible = collider->gameObject()->findComponent<Destructible>();

    if (destructible != nullptr)
    {
        destructible->takeDamage(damage_);
    }
	
    // Delete gameObject on collision
    delete gameObject();
}

void Rocket::setRocketSpeed(float speed)
{
    speed_ = speed;
}
void Rocket::initRocket(const Point3& pos, const Quaternion& rot)
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
