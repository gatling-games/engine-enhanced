#pragma once

#include "Vector2.h"
#include "Vector3.h"

// Returns a random number between 0 and 1.
float random_float();

// Returns a random number between min and max.
float random_float(float min, float max);

// Returns a random direction vector of unit length.
Vector2 random_direction_2d();

// Returns a random direction vector of unit length.
Vector3 random_direction_3d();

