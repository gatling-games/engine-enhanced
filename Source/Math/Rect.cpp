#include "Rect.h"

#include <math.h>

#include "Point2.h"
#include "Vector2.h"

Rect::Rect()
    : minx(0.0f), miny(0.0f), width(0.0f), height(0.0f)
{

}

Rect::Rect(float minx, float miny, float width, float height)
    : minx(minx), miny(miny), width(width), height(height)
{

}

Point2 Rect::min() const
{
    return Point2(minx, miny);
}

Point2 Rect::max() const
{
    return Point2(minx + width, miny + height);
}

Vector2 Rect::size() const
{
    return Vector2(width, height);
}

Rect Rect::roundedToPixels() const
{
    return Rect(roundf(minx), roundf(miny), roundf(width), roundf(height));
}