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

Vector4 Vector4::zero()
{
	return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
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

Vector4 Vector4::normalized() const
{
	return *this / magnitude();
}

Vector4& Vector4::operator += (Vector4 vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;
	w += vector.w;
	return *this;
}

Point3 Vector4::asPoint() const
{
    assert(w > 0.00000001f || w < -0.00000001f);
    return Point3(x / w, y / w, z / w);
}

Vector3 Vector4::asVector() const
{
    assert(w < 0.0000001f && w > -0.0000001f);
    return Vector3(x, y, z);
}

Vector4 operator * (const Vector4 &v, float scalar)
{
	return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}

Vector4 operator * (float scalar, const Vector4 &v)
{
	return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}

Vector4 operator / (const Vector4 &v, float scalar)
{
	return Vector4(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
}

Vector4 operator / (float scalar, const Vector4 &v)
{
	return Vector4(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w);
}

std::ostream& operator << (std::ostream &os, const Vector4 &vec)
{
    os << vec.x;
    os << " " << vec.y;
    os << " " << vec.z;
    os << " " << vec.w;
    return os;
}

std::istream& operator >> (std::istream &is, Vector4 &vec)
{
    is >> vec.x;
    is >> vec.y;
    is >> vec.z;
    is >> vec.w;

    return is;
}