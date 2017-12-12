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

Quaternion Quaternion::identity()
{
    return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

void Quaternion::normalize()
{
    const float length = sqrtf(x*x + y*y + z*z + w*w);

    if (fabs(length) > 0.0001f)
    {
        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }
}

Quaternion Quaternion::conjugate() const
{
    return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::inverse() const
{
    // All quaternions are unit quaternions, so inverse = conjugate.
    return conjugate();
}

Quaternion Quaternion::rotation(float angle, const Vector3 &axis)
{
    // Angle is in degrees
    float angleRadians = angle * ((float)M_PI / 180.0f);

    // Quaternions apply a rotation of 2xphi
    float phi = angleRadians * 0.5f;
    float sinPhi = sinf(phi);
    float cosPhi = cosf(phi);

    // Normalize the axis vector to ensure the quaternion
    // is a unit quaternion.
    Vector3 normalizedAxis = axis.normalized();

    // Construct the final quaternion.
    // [sinPhi * v, cosPhi]
    float x = sinPhi * normalizedAxis.x;
    float y = sinPhi * normalizedAxis.y;
    float z = sinPhi * normalizedAxis.z;
    float w = cosPhi;
    return Quaternion(x, y, z, w);
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

bool operator == (const Quaternion &a, const Quaternion &b)
{
    return fabs(a.x - b.x) < 0.001f && fabs(a.y - b.y) < 0.001f && fabs(a.z - b.z) < 0.001f && fabs(a.w-b.w) < 0.001f;
}

bool operator != (const Quaternion &a, const Quaternion &b)
{
    return fabs(a.x - b.x) > 0.001f || fabs(a.y - b.y) > 0.001f || fabs(a.z - b.z) > 0.001f || fabs(a.w - b.w) > 0.001f;
}

Quaternion operator * (const Quaternion &a, const Quaternion &b)
{
    float x = (a.x * b.w) + (a.w * b.x) + (a.y * b.z) - (a.z * b.y);
    float y = (a.y * b.w) + (a.w * b.y) - (a.x * b.z) + (a.z * b.x);
    float z = (a.z * b.w) + (a.w * b.z) + (a.x * b.y) - (a.y * b.x);
    float w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);

    // Result is a unit quaternion, if a and b are.
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