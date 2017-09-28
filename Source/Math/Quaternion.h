#pragma once

#include <iostream>

struct Vector3;

struct Quaternion
{
    float x;
    float y;
    float z;
    float w;

    Quaternion();
    Quaternion(float x, float y, float z, float w);
    Quaternion(const Vector3 &v, float w);

    static Quaternion identity();

    float sqrNorm() const;
    float norm() const;
    Quaternion conjugate() const;
    Quaternion inverse() const;

    // Creates a rotation for the specified angle (degrees) around the given axis
    static Quaternion rotation(float angle, const Vector3 &axis);

    // Creates a rotation of z degrees around the z axis, followed by y degrees
    // around the y axis, and then x degrees around the x axis.
    static Quaternion euler(float x, float y, float z);
    static Quaternion euler(const Vector3 &euler);
};

Quaternion operator * (const Quaternion &quat, float scalar);
Quaternion operator * (float scalar, const Quaternion &quat);

Quaternion operator + (const Quaternion &a, const Quaternion &b);
Quaternion operator * (const Quaternion &a, const Quaternion &b);

std::ostream& operator << (std::ostream &os, const Quaternion &quat);
std::istream& operator >> (std::istream &is, Quaternion &quat);