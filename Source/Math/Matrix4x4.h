#pragma once

#include <iostream>

struct Point3;
struct Vector3;
struct Vector4;
struct Quaternion;

struct Matrix4x4
{
    // Elements stored in column-major order.
    float elements[16];

    Matrix4x4();

    float get(int row, int column) const;
    void set(int row, int column, float value);
    void setRow(int row, const float* values);
    void setRow(int row, float a, float b, float c, float d);
    void setCol(int col, const float* values);
    void setCol(int col, float a, float b, float c, float d);
    
    Matrix4x4 transpose() const;

    static Matrix4x4 identity();
    static Matrix4x4 translation(const Vector3 &t);
    static Matrix4x4 rotation(const Quaternion &q);
    static Matrix4x4 scale(const Vector3 &s);

    Matrix4x4 invert() const;
    static float getCofactor(float m0, float m1, float m2,
        float m3, float m4, float m5,
        float m6, float m7, float m8);

    // Constructs a matrix for performing a scale, then a rotation, then a translation.
    static Matrix4x4 trs(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale);

    // Constructs the inverse of a trs matrix for the given scale, rotation and translation.
    static Matrix4x4 trsInverse(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale);

    // Constructs an orthographic projection matrix
    static Matrix4x4 orthographic(float left, float right, float bottom, float top, float near, float far);

    // Constructs a perspective projection matrix
    // Z depth is in the range 0 to 1.
    static Matrix4x4 perspective(float fov, float aspectRatio, float near, float far);

    // Constructs the inverse of a perspective projection matrix.
    static Matrix4x4 perspectiveInverse(float fov, float aspectRatio, float near, float far);
};

// Matrix and scalar operations
Matrix4x4 operator * (const Matrix4x4 &mat, float scalar);
Matrix4x4 operator * (float scalar, const Matrix4x4 &mat);

// Matrix and Matrix operations
Matrix4x4 operator * (const Matrix4x4 &a, const Matrix4x4 &b);
Matrix4x4 operator + (const Matrix4x4 &a, const Matrix4x4 &b);
Matrix4x4 operator - (const Matrix4x4 &a, const Matrix4x4 &b);

// Matrix and vector / point operations
Vector4 operator * (const Matrix4x4 &mat, const Vector4 &v);
Point3 operator * (const Matrix4x4 &mat, const Point3 &p);
Vector3 operator * (const Matrix4x4 &mat, const Vector3 &v);

std::ostream& operator << (std::ostream &os, const Matrix4x4 &mat);