#pragma once

#include "Collider.h"
#include "Math/Vector3.h"

class TerrainCollider : public Collider
{
public:
	explicit TerrainCollider(GameObject* gameObject);

	// Checks if the terrain is intersecting with a point
	bool checkForCollision(Point3 point, ColliderHit &hit) const override;
};
