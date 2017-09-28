#pragma once

struct Point3;
struct Vector3;

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    Vector4();
    Vector4(float x, float y, float z, float w);
    explicit Vector4(const Point3 &p);
    explicit Vector4(const Vector3 &v);

    float minComponent() const;
    float maxComponent() const;
    float sqrMagnitude() const;
    float magnitude() const;

    Point3 asPoint() const;
    Vector3 asVector() const;
};