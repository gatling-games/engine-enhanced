#pragma once

#include "Point3.h"
#include "Vector3.h"

class Bounds
{
public:
    Bounds();
    Bounds(const Point3 &min, const Point3 &max);

    // Covered region
    Point3 min() const { return min_; }
    Point3 max() const { return max_; }
    Point3 centre() const;

    // Size
    Vector3 size() const;

    // Expands the bounds to cover the given point
    void expandToCover(const Point3 &point);

    // Creates a Bounds instance covering the given points
    static Bounds covering(const Point3* points, int count);

private:
    Point3 min_;
    Point3 max_;
};