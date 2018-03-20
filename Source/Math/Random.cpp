#include "Random.h"

float random_float()
{
    return rand() / (float)RAND_MAX;
}

float random_float(float min, float max)
{
    return min + (random_float() * (max - min));
}

Vector2 random_in_unit_circle()
{
    // Find a random direction using monte carlo
    Vector2 dir;
    float sqrMagnitude;

    do
    {
        dir.x = random_float(-1.0f, 1.0f);
        dir.y = random_float(-1.0f, 1.0f);
        sqrMagnitude = dir.sqrMagnitude();
    } while (sqrMagnitude > 1.0f || sqrMagnitude < 0.1f);

    // Return *without* normalizing it.
    return dir;
}

Vector2 random_direction_2d()
{
    float angle = random_float();
    return Vector2(cosf(angle), sinf(angle));
}

Vector3 random_direction_3d()
{
    // Find a random direction using monte carlo
    Vector3 dir;
    float sqrMagnitude;

    do
    {
        dir.x = random_float(-1.0f, 1.0f);
        dir.y = random_float(-1.0f, 1.0f);
        dir.z = random_float(-1.0f, 1.0f);
        sqrMagnitude = dir.sqrMagnitude();
    }
    while(sqrMagnitude > 1.0f || sqrMagnitude < 0.05f);

    // Normalize and return
    return dir / sqrtf(sqrMagnitude);
}