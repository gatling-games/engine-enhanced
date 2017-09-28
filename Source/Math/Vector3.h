#pragma once

#include <iostream>

struct Point3;

struct Vector3
{
    float x;
    float y;
    float z;

    Vector3();
    Vector3(float x, float y, float z);
    explicit Vector3(const Point3 &p);

    static Vector3 zero();
    static Vector3 one();
    static Vector3 up();
    static Vector3 down();
    static Vector3 forwards();
    static Vector3 backwards();
    static Vector3 left();
    static Vector3 right();

    float minComponent() const;
    float maxComponent() const;
    float sqrMagnitude() const;
    float magnitude() const;

    Vector3 normalized() const;

    Vector3& operator += (float scalar);
    Vector3& operator -= (float scalar);
    Vector3& operator *= (float scalar);
    Vector3& operator /= (float scalar);
    Vector3& operator += (const Vector3 &vec);
    Vector3& operator -= (const Vector3 &vec);
    Vector3& operator *= (const Vector3 &vec);
    Vector3& operator /= (const Vector3 &vec);

    // Returns the dot product of two vectors.
    static float dot(const Vector3 &a, const Vector3 &b);

    // Returns the angle, in radians, between two vectors.
    static float angle(const Vector3 &a, const Vector3 &b);

    // Returns the cross product of two vectors.
    static Vector3 cross(const Vector3 &a, const Vector3 &b);

    // Linearly interpolates from a to b, without clamping t to between 0 and 1.
    static Vector3 lerpUnclamped(const Vector3 &a, const Vector3 &b, float t);

    // Linearly interpolates from a to b, with t clamped to between 0 and 1.
    static Vector3 lerp(const Vector3 &a, const Vector3 &b, float t);
};

Vector3 operator + (const Vector3 &a, const Vector3 &b);
Vector3 operator - (const Vector3 &a, const Vector3 &b);
Vector3 operator * (const Vector3 &a, const Vector3 &b);
Vector3 operator / (const Vector3 &a, const Vector3 &b);

Vector3 operator + (const Vector3 &v, const float scalar);
Vector3 operator - (const Vector3 &v, const float scalar);
Vector3 operator * (const Vector3 &v, const float scalar);
Vector3 operator * (const float scalar, const Vector3 &v);
Vector3 operator / (const Vector3 &v, const float scalar);
Vector3 operator / (const float scalar, const Vector3 &v);

std::ostream& operator << (std::ostream &os, const Vector3 &vec);
std::istream& operator >> (std::istream &is, Vector3 &vec);