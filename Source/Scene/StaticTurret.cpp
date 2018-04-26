#include "StaticTurret.h"
#include "SceneManager.h"
#include "Scene/Helicopter.h"

#include "imgui.h"

#define _USE_MATH_DEFINES
#include <math.h>

StaticTurret::StaticTurret(GameObject* gameObject)
    : Component(gameObject)
{
    transform_ = gameObject->createComponent<Transform>();
}

void StaticTurret::update(float deltaTime)
{
    choppers = SceneManager::instance()->getComponentOfType<Helicopter>();

    // Disable when no choppers in play
    if (choppers.size() == 0)
    {
        return;
    }

    // Pointer to chopper and vector storing 
    Helicopter* chopper = nullptr;
    float closestHeliDistanceSqr = 9999999.0f;

    // Find closest chopper
    for (Helicopter* helicopter : choppers)
    {
        Vector3 vectorToHeli = transform_->positionWorld() - helicopter->transform()->positionWorld();

        const float heliDistanceSqr = (vectorToHeli.sqrMagnitude());
        if (heliDistanceSqr < closestHeliDistanceSqr)
        {
            closestHeliDistanceSqr = heliDistanceSqr;
            chopper = helicopter;
        }
    }

    // Positions of turret and chopper
    const Point3 turretPosition = transform_->positionWorld();
    const Point3 chopperPosition = chopper->transform()->positionWorld();

    // Vector from turret to chopper
    Vector3 chopperVector = chopperPosition - turretPosition;

    // Vector representing turret's vertical and lateral view vectors
    Vector3 verticalViewVector = Vector3(0.0f, transform_->forwards().y, 0.0f);
    Vector3 lateralViewVector = Vector3(transform_->forwards().x, 0.0f, transform_->forwards().z).normalized();

    // Separate vectors representing direction to chopper's lateral and vertical positions
    Vector3 verticalChopperVector = Vector3(transform_->forwards().x, chopperVector.y, transform_->forwards().z).normalized();
    Vector3 lateralChopperVector = Vector3(chopperVector.x, 0.0f, chopperVector.z).normalized();

    // Magnitude of lateral chopper vector for trigonometry
    float chopperXZ = sqrt((chopperVector.x * chopperVector.x) + (chopperVector.z * chopperVector.z));

    // Get vertical and lateral angles to chopper separately
    float verticalAngle = atan2f(chopperVector.y, chopperXZ) * (-180.0f / (float)M_PI);
    float lateralAngle = atan2f(chopperVector.x, chopperVector.z) * (180.0f / (float)M_PI);

    // Set rotation to point at helicopter
    transform_->setRotationLocal(Quaternion::euler(verticalAngle, lateralAngle, 0.0f));
}