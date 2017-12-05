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
    
    static Quaternion identity();

    // Adjusts the quaternion to have a norm of 1.0
    // This should be rarely used, all quaternions are unit length already
    // unless manually modifying the components.
    void normalize();

    Quaternion conjugate() const;
    Quaternion inverse() const;

    // Creates a rotation for the specified angle (degrees) around the given axis
    static Quaternion rotation(float angle, const Vector3 &axis);

    // Creates a rotation of z degrees around the z axis, followed by y degrees
    // around the y axis, and then x degrees around the x axis.
    static Quaternion euler(float x, float y, float z);
    static Quaternion euler(const Vector3 &euler);
};

Quaternion operator * (const Quaternion &a, const Quaternion &b);

Vector3 operator * (const Quaternion &quat, const Vector3 &vec);

std::ostream& operator << (std::ostream &os, const Quaternion &quat);
std::istream& operator >> (std::istream &is, Quaternion &quat);