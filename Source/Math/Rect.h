#pragma once

struct Point2;
struct Vector2;

struct Rect
{
    float minx;
    float miny;
    float width;
    float height;

    Rect();
    Rect(float minx, float miny, float width, float height);

    // Min and max points of the rect
    Point2 min() const;
    Point2 max() const;

    // Size of the rect
    Vector2 size() const;

    // Rounds each corner of the rect to the nearest whole number
    // Used for making the rect pixel-perfect.
    Rect roundedToPixels() const;
};