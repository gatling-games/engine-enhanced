#include "Matrix4x4.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

#include "Point3.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

Matrix4x4::Matrix4x4()
{
    for (int i = 0; i < 16; ++i)
    {
        elements[i] = 0.0f;
    }
}

float Matrix4x4::get(int row, int column) const
{
    return elements[(column * 4) + row];
}

void Matrix4x4::set(int row, int column, float value)
{
    elements[(column * 4) + row] = value;
}

void Matrix4x4::setRow(int row, const float *values)
{
    setRow(row, values[0], values[1], values[2], values[3]);
}

void Matrix4x4::setRow(int row, float a, float b, float c, float d)
{
    elements[row] = a;
    elements[row + 4] = b;
    elements[row + 8] = c;
    elements[row + 12] = d;
}

void Matrix4x4::setCol(int col, const float *values)
{
    setCol(col, values[0], values[1], values[2], values[3]);
}

void Matrix4x4::setCol(int col, float a, float b, float c, float d)
{
    elements[col * 4] = a;
    elements[col * 4 + 1] = b;
    elements[col * 4 + 2] = c;
    elements[col * 4 + 3] = d;
}

Matrix4x4 Matrix4x4::transpose() const
{
    Matrix4x4 m;
    m.elements[0] = elements[0];
    m.elements[1] = elements[4];
    m.elements[2] = elements[8];
    m.elements[3] = elements[12];
    m.elements[4] = elements[1];
    m.elements[5] = elements[5];
    m.elements[6] = elements[9];
    m.elements[7] = elements[13];
    m.elements[8] = elements[2];
    m.elements[9] = elements[6];
    m.elements[10] = elements[10];
    m.elements[11] = elements[14];
    m.elements[12] = elements[3];
    m.elements[13] = elements[7];
    m.elements[15] = elements[11];
    m.elements[15] = elements[15];
    return m;
}

Matrix4x4 Matrix4x4::identity()
{
    Matrix4x4 mat;
    mat.setRow(0, 1.0f, 0.0f, 0.0f, 0.0f);
    mat.setRow(1, 0.0f, 1.0f, 0.0f, 0.0f);
    mat.setRow(2, 0.0f, 0.0f, 1.0f, 0.0f);
    mat.setRow(3, 0.0f, 0.0f, 0.0f, 1.0f);
    return mat;
}

Matrix4x4 Matrix4x4::translation(const Vector3 &t)
{
    Matrix4x4 mat;
    mat.setRow(0, 1.0f, 0.0f, 0.0f, t.x);
    mat.setRow(1, 0.0f, 1.0f, 0.0f, t.y);
    mat.setRow(2, 0.0f, 0.0f, 1.0f, t.z);
    mat.setRow(3, 0.0f, 0.0f, 0.0f, 1.0f);
    return mat;
}

Matrix4x4 Matrix4x4::rotation(const Quaternion &q)
{
    float s = 2.0f / q.norm();

    Matrix4x4 mat = Matrix4x4::identity();
    mat.set(0, 0, 1.0f - s * (q.y * q.y + q.z * q.z));
    mat.set(0, 1, s * (q.x * q.y - q.w * q.z));
    mat.set(0, 2, s * (q.x * q.z + q.w * q.y));
    mat.set(1, 0, s * (q.x * q.y + q.w * q.z));
    mat.set(1, 1, 1.0f - s * (q.x * q.x + q.z * q.z));
    mat.set(1, 2, s * (q.y * q.z - q.w * q.x));
    mat.set(2, 0, s * (q.x * q.z - q.w * q.y));
    mat.set(2, 1, s * (q.y * q.z + q.w * q.x));
    mat.set(2, 2, 1.0f - s * (q.x * q.x + q.y * q.y));

    return mat;
}

Matrix4x4 Matrix4x4::scale(const Vector3 &s)
{
    Matrix4x4 mat;
    mat.setRow(0, s.x, 0.0f, 0.0f, 0.0f);
    mat.setRow(1, 0.0f, s.y, 0.0f, 0.0f);
    mat.setRow(2, 0.0f, 0.0f, s.z, 0.0f);
    mat.setRow(3, 0.0f, 0.0f, 0.0f, 1.0f);
    return mat;
}

Matrix4x4 Matrix4x4::trs(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale)
{
    Matrix4x4 translationMat = Matrix4x4::translation(translation);
    Matrix4x4 rotationMat = Matrix4x4::rotation(rotation);
    Matrix4x4 scaleMat = Matrix4x4::scale(scale);

    // Scale, then rotation, then translation
    return translationMat * rotationMat * scaleMat;
}

Matrix4x4 Matrix4x4::trsInverse(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale)
{
    Matrix4x4 translationMat = Matrix4x4::translation(-1.0f * translation);
    Matrix4x4 rotationMat = Matrix4x4::rotation(rotation.inverse());
    Matrix4x4 scaleMat = Matrix4x4::scale(1.0f / scale);

    // Opposite order to Matrix4x4::trs.
    // Translation, then rotation, then scale.
    return scaleMat * rotationMat * translationMat;
}

Matrix4x4 Matrix4x4::orthographic(float left, float right, float bottom, float top, float near, float far)
{
    Matrix4x4 mat = Matrix4x4::identity();

    // translation
    mat.set(0, 3, -(right + left) / (right - left));
    mat.set(1, 3, -(top + bottom) / (top - bottom));
    mat.set(2, 3, -(far + near) / (far - near));

    // scale
    mat.set(0, 0, 2.0f / (right - left));
    mat.set(1, 1, 2.0f / (top - bottom));
    mat.set(2, 2, 2.0f / (far - near));

    return mat;
}

Matrix4x4 Matrix4x4::perspective(float fov, float aspect, float near, float far)
{
    // Compute the size of the image plane
    float halfFov = (fov / 2.0f) * ((float)M_PI / 180.0f);
    float h = 2.0f * near * tan(halfFov);
    float w = h * aspect;

    // Create the perspective projection matrix
    Matrix4x4 mat = Matrix4x4::identity();
    mat.set(0, 0, (2.0f * near) / w);
    mat.set(1, 1, (2.0f * near) / h);
    mat.set(2, 2, (far + near) / (far - near));
    mat.set(3, 3, 0.0f);
    mat.set(3, 2, 1.0f);
    mat.set(2, 3, -(2.0f * far * near) / (far - near));

    return mat;
}

Matrix4x4 Matrix4x4::perspectiveInverse(float fov, float aspect, float near, float far)
{
    // Transforms x,y,z from [0 to 1] range to [-1 to 1]
    Matrix4x4 toNDC;
    toNDC.setRow(0, 2.0f, 0.0f, 0.0f, -1.0f);
    toNDC.setRow(1, 0.0f, 2.0f, 0.0f, -1.0f);
    toNDC.setRow(2, 0.0f, 0.0f, 2.0f, -1.0f);
    toNDC.setRow(3, 0.0f, 0.0f, 0.0f, 1.0f);

    // Compute the size of the image plane
    float halfFov = (fov / 2.0f) * ((float)M_PI / 180.0f);
    float h = 2.0f * near * tan(halfFov);
    float w = h * aspect;

    // Create the inverse projection matrix
    Matrix4x4 mat;
    mat.setRow(0, w / (2.0f * near), 0.0f, 0.0f, 0.0f);
    mat.setRow(1, 0.0f, h / (2.0f * near), 0.0f, 0.0f);
    mat.setRow(2, 0.0f, 0.0f, 0.0f, 1.0f);
    mat.setRow(3, 0.0f, 0.0f, -(far - near) / (2.0f * far * near), (far + near) / (2.0f * far * near));

    // Convert to ndc, then use inverse projection
    return mat * toNDC;
}

Matrix4x4 operator * (const Matrix4x4 &mat, float scalar)
{
    Matrix4x4 result;

    for (int i = 0; i < 16; ++i)
        result.elements[i] = mat.elements[i] * scalar;

    return result;
}

Matrix4x4 operator * (float scalar, const Matrix4x4 &mat)
{
    return mat * scalar;
}

Matrix4x4 operator * (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.set(i, j, a.get(i, 0) * b.get(0, j)
                + a.get(i, 1) * b.get(1, j)
                + a.get(i, 2) * b.get(2, j)
                + a.get(i, 3) * b.get(3, j));

    return result;
}

Matrix4x4 operator + (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;

    for (int i = 0; i < 16; ++i)
        result.elements[i] = a.elements[i] + b.elements[i];

    return result;
}

Matrix4x4 operator - (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;

    for (int i = 0; i < 16; ++i)
        result.elements[i] = a.elements[i] - b.elements[i];

    return result;
}

Vector4 operator * (const Matrix4x4 &mat, const Vector4 &v)
{
    Vector4 result;
    result.x = (mat.get(0, 0) * v.x) + (mat.get(0, 1) * v.y) + (mat.get(0, 2) * v.z) + (mat.get(0, 3) * v.w);
    result.y = (mat.get(1, 0) * v.x) + (mat.get(1, 1) * v.y) + (mat.get(1, 2) * v.z) + (mat.get(1, 3) * v.w);
    result.z = (mat.get(2, 0) * v.x) + (mat.get(2, 1) * v.y) + (mat.get(2, 2) * v.z) + (mat.get(2, 3) * v.w);
    result.w = (mat.get(3, 0) * v.x) + (mat.get(3, 1) * v.y) + (mat.get(3, 2) * v.z) + (mat.get(3, 3) * v.w);
    return result;
}

Point3 operator * (const Matrix4x4 &mat, const Point3 &p)
{
    return (mat * Vector4(p)).asPoint();
}

Vector3 operator * (const Matrix4x4 &mat, const Vector3 &v)
{
    return (mat * Vector4(v)).asVector();
}

std::ostream& operator << (std::ostream &os, const Matrix4x4 &mat)
{
    os << "(" << mat.get(0, 0) << ", " << mat.get(0, 1) << ", " << mat.get(0, 2) << ", " << mat.get(0, 3) << ")" << std::endl;
    os << "(" << mat.get(1, 0) << ", " << mat.get(1, 1) << ", " << mat.get(1, 2) << ", " << mat.get(1, 3) << ")" << std::endl;
    os << "(" << mat.get(2, 0) << ", " << mat.get(2, 1) << ", " << mat.get(2, 2) << ", " << mat.get(2, 3) << ")" << std::endl;
    os << "(" << mat.get(3, 0) << ", " << mat.get(3, 1) << ", " << mat.get(3, 2) << ", " << mat.get(3, 3) << ")";
    return os;
}