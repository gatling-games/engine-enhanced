#pragma once

#include "Collider.h"
#include "Math/Vector3.h"

class TerrainCollider : public Collider
{
public:
	explicit TerrainCollider(GameObject* gameObject);

	// Checks if the terrain is intersecting with a point
	bool checkForCollision(const Point3 &point) const override;
};
