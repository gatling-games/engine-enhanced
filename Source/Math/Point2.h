#pragma once

#include <iostream>

struct Vector2;

struct Point2
{
    float x;
    float y;

    Point2();
    Point2(float x, float y);
    explicit Point2(const Vector2 &vec);

    static Point2 origin();

    Point2& operator += (const Vector2 &vec);
    Point2& operator -= (const Vector2 &vec);

    // Returns the square of the distance between 2 points.
    static float sqrDistance(const Point2 &a, const Point2 &b);

    // Returns the distance between 2 points.
    static float distance(const Point2 &a, const Point2 &b);

    // Linearly interpolates from point a to b, without clamping t to between 0 and 1.
    static Point2 lerpUnclamped(const Point2 &a, const Point2 &b, float t);

    // Linearly interpolates from point a to b, with t clamped to between 0 and 1.
    static Point2 lerp(const Point2 &a, const Point2 &b, float t);
};

Vector2 operator - (const Point2 &a, const Point2 &b);
Point2 operator + (const Point2 &p, const Vector2 &v);
Point2 operator - (const Point2 &p, const Vector2 &v);
Point2 operator * (const Point2 &p, float scalar);
Point2 operator * (float scalar, const Point2 &p);
Point2 operator / (const Point2 &p, float scalar);
Point2 operator / (float scalar, const Point2 &p);

std::ostream& operator << (std::ostream &os, const Point2 &p);
std::istream& operator >> (std::istream &is, Point2 &p);