#include "Quaternion.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

#include "Vector3.h"

Quaternion::Quaternion()
    : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{

}

Quaternion::Quaternion(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{

}

Quaternion::Quaternion(const Vector3 &v, float w)
    : x(v.x), y(v.y), z(v.z), w(w)
{

}

Quaternion Quaternion::identity()
{
    return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

float Quaternion::sqrNorm() const
{
    return (x * x) + (y * y) + (z * z) + (w * w);
}

float Quaternion::norm() const
{
    return sqrtf(sqrNorm());
}

Quaternion Quaternion::conjugate() const
{
    return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::inverse() const
{
    return (1.0f / sqrNorm()) * conjugate();
}

Quaternion Quaternion::rotation(float angle, const Vector3 &axis)
{
    float phi = (angle / 2.0f) * ((float)M_PI / 180.0f);
    float sinPhi = sinf(phi);
    float cosPhi = cosf(phi);

    return Quaternion(sinPhi * axis, cosPhi);
}

Quaternion Quaternion::euler(float x, float y, float z)
{
    Quaternion zRotation = Quaternion::rotation(z, Vector3(0.0f, 0.0f, 1.0f));
    Quaternion xRotation = Quaternion::rotation(x, Vector3(1.0f, 0.0f, 0.0f));
    Quaternion yRotation = Quaternion::rotation(y, Vector3(0.0f, 1.0f, 0.0f));

    // Rotate z, then x then y.
    return yRotation * xRotation * zRotation;
}

Quaternion Quaternion::euler(const Vector3 &euler)
{
    return Quaternion::euler(euler.x, euler.y, euler.z);
}

Quaternion operator * (const Quaternion &q, float scalar)
{
    return Quaternion(scalar * q.x, scalar * q.y, scalar * q.z, scalar * q.w);
}

Quaternion operator * (float scalar, const Quaternion &q)
{
    return q * scalar;
}

Quaternion operator * (const Quaternion &a, const Quaternion &b)
{
    float x = (a.x * b.w) + (a.w * b.x) + (a.y * b.z) - (a.z * b.y);
    float y = (a.y * b.w) + (a.w * b.y) - (a.x * b.z) + (a.z * b.x);
    float z = (a.z * b.w) + (a.w * b.z) + (a.x * b.y) - (a.y * b.x);
    float w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z - b.z);

    return Quaternion(x, y, z, w);
}

Vector3 operator * (const Quaternion &quat, const Vector3 &vec)
{
    // Convert the vector to a quaternion
    Quaternion v(vec.x, vec.y, vec.z, 0.0f);

    // Rotate via q * v * q^-1
    Quaternion rotated = (quat * v * quat.inverse());

    // Return the vector part of the rotated quaternion.
    return Vector3(rotated.x, rotated.y, rotated.z);
}

std::ostream& operator << (std::ostream &os, const Quaternion &quat)
{
    os << "( " << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << " )";
    return os;
}

std::istream& operator >> (std::istream &is, Quaternion &quat)
{
    is >> quat.x;
    is >> quat.y;
    is >> quat.z;
    is >> quat.w;
    return is;
}