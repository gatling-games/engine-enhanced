#include "TerrainCollider.h"

#include "Scene/Terrain.h"

TerrainCollider::TerrainCollider(GameObject* gameObject)
	: Collider(gameObject)
{

}

bool TerrainCollider::checkForCollision(const Point3 &point) const
{
	// Get the terrain component
	const Terrain* terrain = gameObject()->findComponent<Terrain>();
	if(terrain == nullptr)
	{
		return false;
	}

	// Get the heightmap height at the point
	const float heightmapHeight = terrain->sampleHeightmap(point.x, point.z);

	// Check if the point is below the heightmap
	return (point.y < heightmapHeight);
}

