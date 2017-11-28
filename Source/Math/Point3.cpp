#include "Point3.h"

#include <algorithm>

#include "Vector3.h"

Point3::Point3()
    : x(0.0f), y(0.0f), z(0.0f)
{

}

Point3::Point3(float x, float y, float z)
    : x(x), y(y), z(z)
{

}

Point3::Point3(const Vector3 &vec)
    : x(vec.x), y(vec.y), z(vec.z)
{

}

Point3 Point3::origin()
{
    return Point3(0.0f, 0.0f, 0.0f);
}

Point3& Point3::operator += (const Vector3 &vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

Point3& Point3::operator -= (const Vector3 &vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

Point3& Point3::operator *= (float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Point3& Point3::operator /= (float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

float Point3::sqrDistance(const Point3 &a, const Point3 &b)
{
    return (a - b).sqrMagnitude();
}

float Point3::distance(const Point3 &a, const Point3 &b)
{
    return (a - b).magnitude();
}

Point3 Point3::lerpUnclamped(const Point3 &a, const Point3 &b, float t)
{
    return a + (b - a) * t;
}

Point3 Point3::lerp(const Point3 &a, const Point3 &b, float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return lerpUnclamped(a, b, t);
}

bool operator == (const Point3 &a, const Point3 &b)
{
    return (a - b).sqrMagnitude() < 0.000001f;
}

bool operator != (const Point3 &a, const Point3 &b)
{
    return (a - b).sqrMagnitude() > 0.000001f;
}

Vector3 operator - (const Point3 &a, const Point3 &b)
{
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Point3 operator + (const Point3 &p, const Vector3 &v)
{
    return Point3(p.x + v.x, p.y + v.y, p.z + v.z);
}

Point3 operator - (const Point3 &p, const Vector3 &v)
{
    return Point3(p.x - v.x, p.y - v.y, p.z - v.z);
}

Point3 operator * (const Point3 &p, float scalar)
{
    return Point3(p.x * scalar, p.y * scalar, p.z * scalar);
}

Point3 operator * (float scalar, const Point3 &p)
{
    return Point3(p.x * scalar, p.y * scalar, p.z * scalar);
}

Point3 operator / (const Point3 &p, float scalar)
{
    return Point3(p.x / scalar, p.y / scalar, p.z / scalar);
}

Point3 operator / (float scalar, const Point3 &p)
{
    return Point3(scalar / p.x, scalar / p.y, scalar / p.z);
}

std::ostream& operator << (std::ostream &os, const Point3 &p)
{
    os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}

std::istream& operator >> (std::istream &is, Point3 &p)
{
    is >> p.x;
    is >> p.y;
    is >> p.z;

    return is;
}