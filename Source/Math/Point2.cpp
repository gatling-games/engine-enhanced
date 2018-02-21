#include "Point2.h"

#include <algorithm>

#include "Vector2.h"

Point2::Point2()
    : x(0.0f), y(0.0f)
{

}

Point2::Point2(float x, float y)
    : x(x), y(y)
{

}

Point2::Point2(const Vector2 &vec)
    : x(vec.x), y(vec.y)
{

}

Point2 Point2::origin()
{
    return Point2(0.0f, 0.0f);
}

Point2& Point2::operator += (const Vector2 &vec)
{
    *this = *this + vec;
    return *this;
}

Point2& Point2::operator -= (const Vector2 &vec)
{
    *this = *this - vec;
    return *this;
}

float Point2::sqrDistance(const Point2 &a, const Point2 &b)
{
    return (a - b).sqrMagnitude();
}

float Point2::distance(const Point2 &a, const Point2 &b)
{
    return (a - b).magnitude();
}

Point2 Point2::lerpUnclamped(const Point2 &a, const Point2 &b, float t)
{
    return a + (b - a) * t;
}

Point2 Point2::lerp(const Point2 &a, const Point2 &b, float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return lerpUnclamped(a, b, t);
}

bool operator == (const Point2 &a, const Point2 &b)
{
    return (a - b).sqrMagnitude() < 0.000001f;
}

bool operator != (const Point2 &a, const Point2 &b)
{
    return (a - b).sqrMagnitude() > 0.000001f;
}

Vector2 operator - (const Point2 &a, const Point2 &b)
{
    return Vector2(a.x - b.x, a.y - b.y);
}

Point2 operator + (const Point2 &p, const Vector2 &v)
{
    return Point2(p.x + v.x, p.y + v.y);
}

Point2 operator - (const Point2 &p, const Vector2 &v)
{
    return Point2(p.x - v.x, p.y - v.y);
}

Point2 operator * (const Point2 &p, float scalar)
{
    return Point2(p.x * scalar, p.y * scalar);
}

Point2 operator * (float scalar, const Point2 &p)
{
    return Point2(p.x * scalar, p.y * scalar);
}

Point2 operator / (const Point2 &p, float scalar)
{
    return Point2(p.x / scalar, p.y / scalar);
}

Point2 operator / (float scalar, const Point2 &p)
{
    return Point2(scalar / p.x, scalar / p.y);
}

std::ostream& operator << (std::ostream &os, const Point2 &p)
{
    os << p.x;
    os << " " << p.y;
    return os;
}

std::istream& operator >> (std::istream &is, Point2 &p)
{
    is >> p.x;
    is >> p.y;
    return is;
}