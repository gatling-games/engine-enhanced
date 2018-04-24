#include "Rigidbody.h"

#include "imgui.h"

#include "Scene/Transform.h"
#include "Collider.h"
#include "SceneManager.h"

Rigidbody::Rigidbody(GameObject* gameObject)
    : Component(gameObject),
    velocity_(Vector3::zero())
{

}

void Rigidbody::update(float deltaTime)
{
    // Apply gravity to the velocity
    velocity_.y -= 9.81f * deltaTime;

    // Then apply the velocity to the transform position
    gameObject()->transform()->translateWorld(velocity_ * deltaTime);

    // Check for a collision with the scene
	// The rigidbody is approximated as a point, for simplicity.
    const Point3 rigidbodyPoint = gameObject()->transform()->positionWorld();
    for(Collider* collider : SceneManager::instance()->colliders())
    {
        if(collider->checkForCollision(rigidbodyPoint))
		{
			// Move the rigidbody back one timestep to before there was a collision
			gameObject()->transform()->translateWorld(velocity_ * -deltaTime);

			// Make the rigidbody velocity change to simulate a bounce
			// Note - This is basic, we should instead split the velocity into components
			//              that are parallel and perpendicular with the collider normal
			velocity_ *= -0.7f;

			// Call the handleCollision callbacks
			gameObject()->handleCollision(collider);

			// Limit to one collision per frame
			return;
        }
    }
}

void Rigidbody::setVelocity(const Vector3& value)
{
    velocity_ = value;
}
