#include "Vector2.h"

#include <algorithm>
#include <math.h>

#include "Point2.h"

Vector2::Vector2()
    : x(0.0f), y(0.0f)
{

}

Vector2::Vector2(float x, float y)
    : x(x), y(y)
{

}

Vector2::Vector2(const Point2 &p)
    : x(p.x), y(p.y)
{

}

Vector2 Vector2::zero()
{
    return Vector2(0.0f, 0.0f);
}

Vector2 Vector2::one()
{
    return Vector2(1.0f, 1.0f);
}

Vector2 Vector2::up()
{
    return Vector2(0.0f, 1.0f);
}

Vector2 Vector2::down()
{
    return Vector2(0.0f, -1.0f);
}

Vector2 Vector2::left()
{
    return Vector2(-1.0f, 0.0f);
}

Vector2 Vector2::right()
{
    return Vector2(1.0f, 0.0f);
}

float Vector2::minComponent() const
{
    return std::min(x, y);
}

float Vector2::maxComponent() const
{
    return std::max(x, y);
}

float Vector2::sqrMagnitude() const
{
    return (x * x) + (y * y);
}

float Vector2::magnitude() const
{
    return sqrtf(sqrMagnitude());
}

Vector2 Vector2::normalized() const
{
    float len = magnitude();
    return Vector2(x / len, y / len);
}

Vector2& Vector2::operator += (float scalar)
{
    *this = *this + scalar;
    return *this;
}

Vector2& Vector2::operator -= (float scalar)
{
    *this = *this - scalar;
    return *this;
}

Vector2& Vector2::operator *= (float scalar)
{
    *this = *this * scalar;
    return *this;
}

Vector2& Vector2::operator /= (float scalar)
{
    *this = *this / scalar;
    return *this;
}

Vector2& Vector2::operator += (const Vector2 &vec)
{
    x += vec.x;
    y += vec.y;
    return *this;
}

Vector2& Vector2::operator -= (const Vector2 &vec)
{
    *this = *this - vec;
    return *this;
}

Vector2& Vector2::operator *= (const Vector2 &vec)
{
    *this = *this * vec;
    return *this;
}

Vector2& Vector2::operator /= (const Vector2 &vec)
{
    *this = *this / vec;
    return *this;
}

float Vector2::dot(const Vector2 &a, const Vector2 &b)
{
    return (a.x * b.x) + (a.y * b.y);
}

float Vector2::angle(const Vector2 &a, const Vector2 &b)
{
    float aLength = a.magnitude();
    float bLength = b.magnitude();
    float dotProduct = dot(a, b);
    float cosTheta = dotProduct / (aLength * bLength);

    // Clamp to between -1 and 1 in case of floating point error.
    return acosf(std::max(-1.0f, std::min(cosTheta, 1.0f)));
}

Vector2 Vector2::lerpUnclamped(const Vector2 &a, const Vector2 &b, float t)
{
    return a + (b - a) * t;
}

Vector2 Vector2::lerp(const Vector2 &a, const Vector2 &b, float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return lerpUnclamped(a, b, t);
}

bool operator == (const Vector2 &a, const Vector2 &b)
{
    return (a - b).sqrMagnitude() < 0.000001f;
}

bool operator != (const Vector2 &a, const Vector2 &b)
{
    return (a - b).sqrMagnitude() > 0.000001f;
}

Vector2 operator + (const Vector2 &a, const Vector2 &b)
{
    return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator - (const Vector2 &a, const Vector2 &b)
{
    return Vector2(a.x - b.x, a.y - b.y);
}

Vector2 operator * (const Vector2 &a, const Vector2 &b)
{
    return Vector2(a.x * b.x, a.y * b.y);
}

Vector2 operator / (const Vector2 &a, const Vector2 &b)
{
    return Vector2(a.x / b.x, a.y / b.y);
}

Vector2 operator + (const Vector2 &v, float scalar)
{
    return Vector2(v.x + scalar, v.y + scalar);
}

Vector2 operator - (const Vector2 &v, float scalar)
{
    return Vector2(v.x - scalar, v.y - scalar);
}

Vector2 operator * (const Vector2 &v, float scalar)
{
    return Vector2(v.x * scalar, v.y * scalar);
}

Vector2 operator * (float scalar, const Vector2 &v)
{
    return Vector2(v.x * scalar, v.y * scalar);
}

Vector2 operator / (const Vector2 &v, float scalar)
{
    return Vector2(v.x / scalar, v.y / scalar);
}

Vector2 operator / (float scalar, const Vector2 &v)
{
    return Vector2(scalar / v.x, scalar / v.y);
}

std::ostream& operator << (std::ostream &os, const Vector2 &vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

std::istream& operator >> (std::istream &is, Vector2 &vec)
{
    is >> vec.x;
    is >> vec.y;
    return is;
}