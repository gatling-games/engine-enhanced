#pragma once

#include <iostream>

struct Color
{
    float r;
    float g;
    float b;
    float a;

    Color();
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);

    static Color white();
    static Color black();
    static Color red();
    static Color green();
    static Color blue();
    static Color gray();
    static Color transparent();

    float minColorComponent() const;
    float maxColorComponent() const;

    Color& operator *= (float scalar);
    Color& operator /= (float scalar);
    Color& operator += (const Color &c);
    Color& operator -= (const Color &c);
    Color& operator *= (const Color &c);
    Color& operator /= (const Color &c);

    // Linearly interpolates between two colours.
    static Color lerpUnclamped(const Color &a, const Color &b, float t);

    // Linearlly interpolates between two colours, with
    // t clamped to between 0 and 1.
    static Color lerp(const Color &a, const Color &b, float t);
};

bool operator == (const Color &a, const Color &b);
bool operator != (const Color &a, const Color &b);

Color operator * (const Color &c, float scalar);
Color operator / (const Color &c, float scalar);
Color operator + (const Color &a, const Color &b);
Color operator - (const Color &a, const Color &b);
Color operator * (const Color &a, const Color &b);
Color operator / (const Color &a, const Color &b);

std::ostream& operator << (std::ostream &os, const Color &col);
std::istream& operator >> (std::istream &is, Color &col);