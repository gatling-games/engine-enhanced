#pragma once

#include <iostream>

struct Vector3;

struct Point3
{
    float x;
    float y;
    float z;

    Point3();
    Point3(float x, float y, float z);
    explicit Point3(const Vector3 &vec);

    static Point3 origin();

    Point3& operator += (const Vector3 &vec);
    Point3& operator -= (const Vector3 &vec);

    // Returns the square of the distance between 2 points.
    static float sqrDistance(const Point3 &a, const Point3 &b);

    // Returns the distance between 2 points.
    static float distance(const Point3 &a, const Point3 &b);

    // Linearly interpolates from point a to b, without clamping t to between 0 and 1.
    static Point3 lerpUnclamped(const Point3 &a, const Point3 &b, float t);

    // Linearly interpolates from point a to b, with t clamped to between 0 and 1.
    static Point3 lerp(const Point3 &a, const Point3 &b, float t);
};

bool operator == (const Point3 &a, const Point3 &b);
bool operator != (const Point3 &a, const Point3 &b);

Vector3 operator - (const Point3 &a, const Point3 &b);
Point3 operator + (const Point3 &p, const Vector3 &v);
Point3 operator - (const Point3 &p, const Vector3 &v);
Point3 operator * (const Point3 &p, float scalar);
Point3 operator * (float scalar, const Point3 &p);
Point3 operator / (const Point3 &p, float scalar);
Point3 operator / (float scalar, const Point3 &p);

std::ostream& operator << (std::ostream &os, const Point3 &p);
std::istream& operator >> (std::istream &is, Point3 &p);