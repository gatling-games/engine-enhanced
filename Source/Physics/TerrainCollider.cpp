#include "TerrainCollider.h"

#include "Scene/Terrain.h"

TerrainCollider::TerrainCollider(GameObject* gameObject)
	: Collider(gameObject)
{

}

bool TerrainCollider::checkForCollision(Point3 point, ColliderHit& hit) const
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
	if(point.y < heightmapHeight)
	{
		hit.position = Point3(point.x, heightmapHeight, point.z);
		hit.normal = terrain->sampleHeightmapNormal(point.x, point.z);
		return true;
	}
	else
	{
		return false;
	}
}

