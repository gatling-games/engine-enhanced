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
    
    // Magnitude of lateral chopper vector for trigonometry
    float chopperXZ = sqrt((chopperVector.x * chopperVector.x) + (chopperVector.z * chopperVector.z));

    // Get vertical and lateral angles to chopper separately
    float verticalAngle = atan2f(chopperVector.y, chopperXZ) * (-180.0f / (float)M_PI);
    float lateralAngle = atan2f(chopperVector.x, chopperVector.z) * (180.0f / (float)M_PI);

    // Rotate first component child (turret pivot) in world space
    const std::vector<Transform*> children = transform_->children();
    children[0]->setRotationWorld(Quaternion::euler(0.0f, lateralAngle, 0.0f));

    // Rotate second component child (turret cannon) in local space
    const std::vector<Transform*> grandchildren = children[0]->children();
    grandchildren[0]->setRotationLocal(Quaternion::euler(verticalAngle, 0.0f, 0.0f));
}