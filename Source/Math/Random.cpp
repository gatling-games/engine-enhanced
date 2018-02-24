#include "Random.h"

float random_float()
{
    return rand() / (float)RAND_MAX;
}

float random_float(float min, float max)
{
    return min + (random_float() * (max - min));
}

Vector2 random_direction_2d()
{
    float angle = random_float();
    return Vector2(cosf(angle), sinf(angle));
}