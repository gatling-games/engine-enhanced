#pragma once

#include "Utils/Singleton.h"

class PhysicsManager : public Singleton<PhysicsManager>
{
public:
    PhysicsManager();

    // When true, physics colliders are rendered in the scene as wireframes.
    bool physicsDebugEnabled() const { return physicsDebugEnabled_; }

private:
    bool physicsDebugEnabled_;
};