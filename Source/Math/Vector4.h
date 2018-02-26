#pragma once

#include <iostream>

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
    Vector4(const Vector3 &xyz, float w);

	static Vector4 zero();

    float minComponent() const;
    float maxComponent() const;
    float sqrMagnitude() const;
    float magnitude() const;

	Vector4 normalized() const;

	Vector4& operator += (Vector4 vector);

    Point3 asPoint() const;
    Vector3 asVector() const;
};

Vector4 operator * (const Vector4 &v, const float scalar);
Vector4 operator * (const float scalar, const Vector4 &v);
Vector4 operator / (const Vector4 &v, const float scalar);
Vector4 operator / (const float scalar, const Vector4 &v);

std::ostream& operator << (std::ostream &os, const Vector4 &vec);
std::istream& operator >> (std::istream &is, Vector4 &vec);