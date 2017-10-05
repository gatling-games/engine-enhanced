#include "Vector3.h"

#include <algorithm>
#include <math.h>

#include "Point3.h"

Vector3::Vector3()
    : x(0.0f), y(0.0f), z(0.0f)
{

}

Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
{

}

Vector3::Vector3(const Point3 &p)
    : x(p.x), y(p.y), z(p.z)
{
       
}

Vector3 Vector3::zero()
{
    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Vector3::one()
{
    return Vector3(1.0f, 1.0f, 1.0f);
}

Vector3 Vector3::up()
{
    return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::down()
{
    return Vector3(0.0f, -1.0f, 0.0f);
}

Vector3 Vector3::forwards()
{
    return Vector3(0.0f, 0.0f, 1.0f);
}

Vector3 Vector3::backwards()
{
    return Vector3(0.0f, 0.0f, -1.0f);
}

Vector3 Vector3::left()
{
    return Vector3(-1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::right()
{
    return Vector3(1.0f, 0.0f, 0.0f);
}

float Vector3::minComponent() const
{
    return std::min(x, std::min(y, z));
}

float Vector3::maxComponent() const
{
    return std::max(x, std::max(y, z));
}

float Vector3::sqrMagnitude() const
{
    return (x * x) + (y * y) + (z * z);
}

float Vector3::magnitude() const
{
    return sqrtf(sqrMagnitude());
}

Vector3 Vector3::normalized() const
{
    return *this / magnitude();
}

Vector3& Vector3::operator += (float scalar)
{
    x += scalar;
    y += scalar;
    z += scalar;
    return *this;
}

Vector3& Vector3::operator -= (float scalar)
{
    x -= scalar;
    y -= scalar;
    z -= scalar;
    return *this;
}

Vector3& Vector3::operator *= (float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator /= (float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator += (const Vector3 &vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

Vector3& Vector3::operator -= (const Vector3 &vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

Vector3& Vector3::operator *= (const Vector3 &vec)
{
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    return *this;
}

Vector3& Vector3::operator /= (const Vector3 &vec)
{
    x /= vec.x;
    y /= vec.y;
    z /= vec.z;
    return *this;
}

float Vector3::dot(const Vector3 &a, const Vector3 &b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float Vector3::angle(const Vector3 &a, const Vector3 &b)
{
    float aLength = a.magnitude();
    float bLength = b.magnitude();
    float dotProduct = dot(a, b);

    return acosf(dotProduct / (aLength * bLength));
}

Vector3 Vector3::cross(const Vector3 &a, const Vector3 &b)
{
    Vector3 v;
    v.x = (a.y * b.z) - (a.z * b.y);
    v.y = (a.z * b.x) - (a.x * b.z);
    v.z = (a.x * b.y) - (a.y * b.x);
    return v;
}

Vector3 Vector3::lerpUnclamped(const Vector3 &a, const Vector3 &b, float t)
{
    return a + (b - a) * t;
}

Vector3 Vector3::lerp(const Vector3 &a, const Vector3 &b, float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return lerpUnclamped(a, b, t);
}

Vector3 operator + (const Vector3 &a, const Vector3 &b)
{
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator - (const Vector3 &a, const Vector3 &b)
{
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator * (const Vector3 &a, const Vector3 &b)
{
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 operator / (const Vector3 &a, const Vector3 &b)
{
    return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vector3 operator + (const Vector3 &v, float scalar)
{
    return Vector3(v.x + scalar, v.y + scalar, v.z + scalar);
}

Vector3 operator - (const Vector3 &v, float scalar)
{
    return Vector3(v.x - scalar, v.y - scalar, v.z - scalar);
}

Vector3 operator * (const Vector3 &v, float scalar)
{
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

Vector3 operator * (float scalar, const Vector3 &v)
{
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

Vector3 operator / (const Vector3 &v, float scalar)
{
    return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
}

Vector3 operator / (float scalar, const Vector3 &v)
{
    return Vector3(scalar / v.x, scalar / v.y, scalar / v.z);
}

std::ostream& operator << (std::ostream &os, const Vector3 &vec)
{
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

std::istream& operator >> (std::istream &is, Vector3 &vec)
{
    is >> vec.x;
    is >> vec.y;
    is >> vec.z;

    return is;
}