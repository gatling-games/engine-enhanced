#include "Bounds.h"

#include <algorithm>

Bounds::Bounds(const Point3& min, const Point3& max)
    : min_(min),
    max_(max)
{
    
}

Point3 Bounds::centre() const
{
    return (min_ + Vector3(max_)) / 2.0f;
}

Vector3 Bounds::size() const
{
    return max_ - Vector3(min_);
}

void Bounds::expandToCover(const Point3& point)
{
    // Reduce min if needed
    min_.x = std::min(min_.x, point.x);
    min_.y = std::min(min_.y, point.y);
    min_.z = std::min(min_.z, point.z);

    // Increase max if needed
    max_.x = std::max(max_.x, point.x);
    max_.y = std::max(max_.y, point.y);
    max_.z = std::max(max_.z, point.z);
}

Bounds Bounds::covering(const Point3* points, int count)
{
    // Cover the first point initially
    Bounds b(points[0], points[0]);

    // Expand to cover each additional point
    for (int i = 1; i < count; ++i)
    {
        b.expandToCover(points[i]);
    }

    return b;
}