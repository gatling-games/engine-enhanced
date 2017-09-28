#pragma once

#include <iostream>

struct Point2;

struct Vector2
{
    float x;
    float y;

    Vector2();
    Vector2(float x, float y);
    explicit Vector2(const Point2 &p);

    static Vector2 zero();
    static Vector2 one();
    static Vector2 up();
    static Vector2 down();
    static Vector2 left();
    static Vector2 right();

    float minComponent() const;
    float maxComponent() const;
    float sqrMagnitude() const;
    float magnitude() const;

    Vector2 normalized() const;

    Vector2& operator += (float scalar);
    Vector2& operator -= (float scalar);
    Vector2& operator *= (float scalar);
    Vector2& operator /= (float scalar);
    Vector2& operator += (const Vector2 &vec);
    Vector2& operator -= (const Vector2 &vec);
    Vector2& operator *= (const Vector2 &vec);
    Vector2& operator /= (const Vector2 &vec);

    // Returns the dot product of two vectors.
    static float dot(const Vector2 &a, const Vector2 &b);

    // Returns the angle, in radians, between two vectors.
    static float angle(const Vector2 &a, const Vector2 &b);

    // Linearly interpolates from a to b, without clamping t to between 0 and 1.
    static Vector2 lerpUnclamped(const Vector2 &a, const Vector2 &b, float t);

    // Linearly interpolates from a to b, with t clamped to between 0 and 1.
    static Vector2 lerp(const Vector2 &a, const Vector2 &b, float t);
};

Vector2 operator + (const Vector2 &a, const Vector2 &b);
Vector2 operator - (const Vector2 &a, const Vector2 &b);
Vector2 operator * (const Vector2 &a, const Vector2 &b);
Vector2 operator / (const Vector2 &a, const Vector2 &b);

Vector2 operator + (const Vector2 &v, const float scalar);
Vector2 operator - (const Vector2 &v, const float scalar);
Vector2 operator * (const Vector2 &v, const float scalar);
Vector2 operator * (const float scalar, const Vector2 &v);
Vector2 operator / (const Vector2 &v, const float scalar);
Vector2 operator / (const float scalar, const Vector2 &v);

std::ostream& operator << (std::ostream &os, const Vector2 &vec);
std::istream& operator >> (std::istream &is, Vector2 &vec);