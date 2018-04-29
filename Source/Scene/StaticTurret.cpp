#include "StaticTurret.h"
#include "SceneManager.h"

#include "Scene/Helicopter.h"
#include "Physics/Rigidbody.h"

#include "imgui.h"

#define _USE_MATH_DEFINES
#include <math.h>

StaticTurret::StaticTurret(GameObject* gameObject)
    : Component(gameObject)
{
    transform_ = gameObject->createComponent<Transform>();
}

void StaticTurret::update(float)
{
    // Pointer to chopper and vector storing 
    Helicopter* chopper = nullptr;
    float closestHeliDistanceSqr = 9999999.0f;

    // Find closest chopper
    for (Helicopter* helicopter : SceneManager::instance()->findAllComponentsInScene<Helicopter>())
    {
        Vector3 vectorToHeli = transform_->positionWorld() - helicopter->transform()->positionWorld();

        const float heliDistanceSqr = (vectorToHeli.sqrMagnitude());
        if (heliDistanceSqr < closestHeliDistanceSqr)
        {
            closestHeliDistanceSqr = heliDistanceSqr;
            chopper = helicopter;
        }
    }

    if(chopper == nullptr)
    {
        // The turrets remain still when no helicopters are in the scene
        return;
    }

    const std::vector<Transform*> children = transform_->children();
    const std::vector<Transform*> grandchildren = children[0]->children();

    // Vector from turret to chopper
    Vector3 chopperVector = getChopperPredictedPosition(chopper);
    
    // Magnitude of lateral chopper vector for trigonometry
    float chopperXZ = sqrt((chopperVector.x * chopperVector.x) + (chopperVector.z * chopperVector.z));

    /*float deltaH = getPredictedVerticalDisplacement(t);
    chopperVector.y -= deltaH + 3.0f;*/
    
    float verticalAngle = atan2f(chopperVector.y, chopperXZ) * (-180.0f / (float)M_PI);
    float lateralAngle = atan2f(chopperVector.x, chopperVector.z) * (180.0f / (float)M_PI);

    children[0]->setRotationWorld(Quaternion::euler(0.0f, lateralAngle, 0.0f));
    grandchildren[0]->setRotationLocal(Quaternion::euler(verticalAngle, 0.0f, 0.0f));
}

Vector3 StaticTurret::getChopperPredictedPosition(Helicopter* chopper)
{
    const Point3 turretPos = transform_->positionWorld();
    const Point3 chopperPos = chopper->transform()->positionWorld();

    Vector3 vectorToChopper = chopperPos - turretPos;

    // Get time for rocket to laterally reach chopper
    float t = vectorToChopper.magnitude() / 100.0f;

    // Predict chopper movement using time step
    Vector3 predictedChopperPos = vectorToChopper + (t * chopper->velocity());
    // Account for gravity
    predictedChopperPos.y += (0.5 * 9.7 * t * t);

    return predictedChopperPos;
}