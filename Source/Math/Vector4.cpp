#include "Vector4.h"

#include <algorithm>
#include <assert.h>
#include <math.h>

#include "Point3.h"
#include "Vector3.h"

#include <algorithm>

Vector4::Vector4()
    : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{

}

Vector4::Vector4(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{

}

Vector4::Vector4(const Point3 &p)
    : x(p.x), y(p.y), z(p.z), w(1.0f)
{
       
}

Vector4::Vector4(const Vector3 &p)
    : x(p.x), y(p.y), z(p.z), w(0.0f)
{

}

float Vector4::minComponent() const
{
    return std::min(std::min(x, y), std::min(z, w));
}

float Vector4::maxComponent() const
{
    return std::max(std::max(x, y), std::max(z, w));
}

float Vector4::sqrMagnitude() const
{
    return (x * x) + (y * y) + (z * z) + (w * w);
}

float Vector4::magnitude() const
{
    return sqrtf(sqrMagnitude());
}

Point3 Vector4::asPoint() const
{
    assert(w > 0.00000001f || w < -0.00000001f);
    return Point3(x / w, y / w, z / w);
}

Vector3 Vector4::asVector() const
{
    assert(w < 0.0000001f && w > 0.0000001f);
    return Vector3(x, y, z);
}

