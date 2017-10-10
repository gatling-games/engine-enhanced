#include "Color.h"

#include <algorithm>

Color::Color()
    : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
{

}

Color::Color(float r, float g, float b)
    : r(r), g(g), b(b), a(1.0f)
{

}

Color::Color(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a)
{

}

Color Color::white()
{
    return Color(1.0f, 1.0f, 1.0f, 1.0f);
}

Color Color::black()
{
    return Color(0.0f, 0.0f, 0.0f, 1.0f);
}

Color Color::red()
{
    return Color(1.0f, 0.0f, 0.0f, 1.0f);
}

Color Color::green()
{
    return Color(0.0f, 1.0f, 0.0f, 1.0f);
}

Color Color::blue()
{
    return Color(0.0f, 0.0f, 1.0f, 1.0f);
}

Color Color::gray()
{
    return Color(0.5f, 0.5f, 0.5f, 1.0f);
}

Color Color::transparent()
{
    return Color(0.0f, 0.0f, 0.0f, 0.0f);
}

float Color::minColorComponent() const
{
    return std::min(r, std::min(g, b));
}

float Color::maxColorComponent() const
{
    return std::max(r, std::max(g, b));
}

Color& Color::operator *= (float scalar)
{
    *this = *this * scalar;
    return *this;
}

Color& Color::operator /= (float scalar)
{
    *this = *this / scalar;
    return *this;
}

Color& Color::operator += (const Color &c)
{
    *this = *this + c;
    return *this;
}

Color& Color::operator -= (const Color &c)
{
    *this = *this - c;
    return *this;
}

Color& Color::operator *= (const Color &c)
{
    *this = *this * c;
    return *this;
}

Color& Color::operator /= (const Color &c)
{
    *this = *this / c;
    return *this;
}

Color Color::lerpUnclamped(const Color &a, const Color &b, float t)
{
    return a + (b - a) * t;
}

Color Color::lerp(const Color &a, const Color &b, float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return lerpUnclamped(a, b, t);
}

bool operator == (const Color &a, const Color &b)
{
    Color diff = a - b;
    return (diff.r * diff.r) + (diff.g * diff.g) + (diff.b * diff.b) + (diff.a * diff.a) < 0.00001f;
}

bool operator != (const Color &a, const Color &b)
{
    return !(a == b);
}

Color operator * (const Color &a, float scalar)
{
    return Color(a.r * scalar, a.g * scalar, a.b * scalar, a.a * scalar);
}

Color operator * (float scalar, const Color &a)
{
    return Color(a.r * scalar, a.g * scalar, a.b * scalar, a.a * scalar);
}

Color operator / (const Color &a, float scalar)
{
    return Color(a.r / scalar, a.g / scalar, a.b / scalar, a.a / scalar);
}

Color operator + (const Color &a, const Color &b)
{
    return Color(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}

Color operator - (const Color &a, const Color &b)
{
    return Color(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
}

Color operator * (const Color &a, const Color &b)
{
    return Color(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
}

Color operator / (const Color &a, const Color &b)
{
    return Color(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a);
}

std::ostream& operator << (std::ostream &os, const Color &col)
{
    os << "(" << col.r << ", " << col.g << ", " << col.b << ", " << col.a << ")";
    return os;
}

std::istream& operator >> (std::istream &is, Color &col)
{
    is >> col.r;
    is >> col.g;
    is >> col.b;
    is >> col.a;

    return is;
}