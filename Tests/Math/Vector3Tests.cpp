#include "CppUnitTest.h"

#include "Math/Vector3.h"
#include "Math/Quaternion.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(Vector3Tests)
    {
        const float tol = 0.000001f;

    public:

        TEST_METHOD(Constructor)
        {
            // Construct vectors
            Vector3 a;
            Vector3 b(1.0f, 2.0f, 3.0f);

            // Check a contains all zeros
            Assert::AreEqual(0.0f, a.x);
            Assert::AreEqual(0.0f, a.y);
            Assert::AreEqual(0.0f, a.z);

            // Check b is (1, 2, 3)
            Assert::AreEqual(1.0f, b.x, tol);
            Assert::AreEqual(2.0f, b.y, tol);
            Assert::AreEqual(3.0f, b.z, tol);
        }

        TEST_METHOD(Constants)
        {
            // Construct vectors
            Vector3 zero = Vector3::zero();
            Vector3 one = Vector3::one();
            Vector3 up = Vector3::up();
            Vector3 down = Vector3::down();
            Vector3 forwards = Vector3::forwards();
            Vector3 backwards = Vector3::backwards();
            Vector3 left = Vector3::left();
            Vector3 right = Vector3::right();

            // Check the values are correct
            Assert::IsTrue(Vector3(0.0f, 0.0f, 0.0f) == zero);
            Assert::IsTrue(Vector3(1.0f, 1.0f, 1.0f) == one);
            Assert::IsTrue(Vector3(0.0f, 1.0f, 0.0f) == up);
            Assert::IsTrue(Vector3(0.0f, -1.0f, 0.0f) == down);
            Assert::IsTrue(Vector3(0.0f, 0.0f, 1.0f) == forwards);
            Assert::IsTrue(Vector3(0.0f, 0.0f, -1.0f) == backwards);
            Assert::IsTrue(Vector3(-1.0f, 0.0f, 0.0f) == left);
            Assert::IsTrue(Vector3(1.0f, 0.0f, 0.0f) == right);
        }

        TEST_METHOD(MinComponent)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check minComponent of a is 1.0
            Assert::AreEqual(1.0f, a.minComponent(), tol);

            // Check minComponent of b is -10.0
            Assert::AreEqual(-10.0f, b.minComponent(), tol);
        }

        TEST_METHOD(MaxComponent)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check maxComponent of a is 3.0
            Assert::AreEqual(3.0f, a.maxComponent(), tol);

            // Check maxComponent of b is 1.0
            Assert::AreEqual(1.0f, b.maxComponent(), tol);
        }

        TEST_METHOD(Magnitude)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check sqrMagnitude of a is 14
            Assert::AreEqual(14.0f, a.sqrMagnitude(), tol);

            // Check magnitude of a is sqrt(14)
            Assert::AreEqual(sqrtf(14.0f), a.magnitude(), tol);

            // Check sqrMagnitude of b is 102
            Assert::AreEqual(102.0f, b.sqrMagnitude(), tol);

            // Check magnitude of a is sqrt(102)
            Assert::AreEqual(sqrtf(102.0f), b.magnitude(), tol);
        }

        TEST_METHOD(Normalized)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Magnitude of a is sqrt(14)
            // Check a normalized is (1 / sqrt(14), 2 / sqrt(14), 3 / sqrt(14))
            float aMag = sqrtf(14.0f);
            Assert::IsTrue(Vector3(1.0f / aMag, 2.0f / aMag, 3.0f / aMag) == a.normalized());

            // Magnitude of a is sqrt(102)
            // Check a normalized is (1 / sqrt(102), 2 / sqrt(102), 3 / sqrt(102))
            float bMag = sqrtf(102.0f);
            Assert::IsTrue(Vector3(-10.0f / bMag, -1.0f / bMag, 1.0f / bMag) == b.normalized());
        }

        TEST_METHOD(Dot)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check the dot product is -9
            Assert::AreEqual(-9.0f, Vector3::dot(a, b), tol);
            Assert::AreEqual(-9.0f, Vector3::dot(b, a), tol);
        }

        TEST_METHOD(Angle)
        {
            // Construct vectors
            Vector3 up(0.0f, 1.0f, 0.0f);
            Vector3 diagonal(1.0f, 1.0f, 1.0f);
            Vector3 down(0.0f, -1.0f, 0.0f);

            // Check the angle between up and up is 0
            Assert::AreEqual(0.0f, Vector3::angle(up, up), tol);

            // Check the angle between diagonal and diagonal is 0
            Assert::AreEqual(0.0f, Vector3::angle(diagonal, diagonal), tol);

            // Check the angle between up and diagonal is 0.0.955316f rad
            Assert::AreEqual(0.955316f, Vector3::angle(up, diagonal), tol);
            Assert::AreEqual(0.955316f, Vector3::angle(diagonal, up), tol);

            // Check the angle between up and down is 180 degrees (pi rad)
            Assert::AreEqual((float)M_PI, Vector3::angle(up, down), tol);
            Assert::AreEqual((float)M_PI, Vector3::angle(down, up), tol);
        }

        TEST_METHOD(Cross)
        {
            // Construct vectors
            Vector3 up(0.0f, 1.0f, 0.0f);
            Vector3 forward(0.0f, 0.0f, 1.0f);

            // Check up x forward gives the right vector
            Assert::IsTrue(Vector3(1.0f, 0.0f, 0.0f) == Vector3::cross(up, forward));
            Assert::IsTrue(-1.0f * Vector3(1.0f, 0.0f, 0.0f) == Vector3::cross(forward, up));
        }

        TEST_METHOD(LerpUnclamped)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Vector3::lerpUnclamped(a, b, 0.0f));
            Assert::IsTrue(b == Vector3::lerpUnclamped(b, a, 0.0f));
            Assert::IsTrue(b == Vector3::lerpUnclamped(a, b, 1.0f));
            Assert::IsTrue(a == Vector3::lerpUnclamped(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue((a * 0.9f) + (b * 0.1f) == Vector3::lerpUnclamped(a, b, 0.1f));
            Assert::IsTrue((b * 0.9f) + (a * 0.1f) == Vector3::lerpUnclamped(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(a + (b - a) * 2.0f == Vector3::lerpUnclamped(a, b, 2.0f));
            Assert::IsTrue(b + (a - b) * 2.0f == Vector3::lerpUnclamped(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a - (b - a) == Vector3::lerpUnclamped(a, b, -1.0f));
            Assert::IsTrue(b - (a - b) == Vector3::lerpUnclamped(b, a, -1.0f));
        }

        TEST_METHOD(Lerp)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Vector3::lerp(a, b, 0.0f));
            Assert::IsTrue(b == Vector3::lerp(b, a, 0.0f));
            Assert::IsTrue(b == Vector3::lerp(a, b, 1.0f));
            Assert::IsTrue(a == Vector3::lerp(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue((a * 0.9f) + (b * 0.1f) == Vector3::lerp(a, b, 0.1f));
            Assert::IsTrue((b * 0.9f) + (a * 0.1f) == Vector3::lerp(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(b == Vector3::lerp(a, b, 2.0f));
            Assert::IsTrue(a == Vector3::lerp(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a == Vector3::lerp(a, b, -1.0f));
            Assert::IsTrue(b == Vector3::lerp(b, a, -1.0f));
        }

        TEST_METHOD(Quat)
        {
            // Construct euler angle rotations
            const Vector3 euler1(0.0f, 0.0f, 0.0f);
            const Vector3 euler2(32.0f, 0.0f, 0.0f);
            const Vector3 euler3(0.0f, -42.0f, 0.0f);
            const Vector3 euler4(0.0f, 0.0f, 14.0f);
            const Vector3 euler5(90.0f, 70.0f, 14.0f);

            // Convert the euler rotations to quaternions
            const Quaternion quat1 = Quaternion::euler(euler1);
            const Quaternion quat2 = Quaternion::euler(euler2);
            const Quaternion quat3 = Quaternion::euler(euler3);
            const Quaternion quat4 = Quaternion::euler(euler4);
            const Quaternion quat5 = Quaternion::euler(euler5);

            // Conver the quaternions back to euler and check
            // the result matches the original values
            Assert::IsTrue(euler1 == Vector3::quat(quat1));
            Assert::IsTrue(euler2 == Vector3::quat(quat2));
            Assert::IsTrue(euler3 == Vector3::quat(quat3));
            Assert::IsTrue(euler4 == Vector3::quat(quat4));
            Assert::IsTrue(euler5 == Vector3::quat(quat5));
        }

        TEST_METHOD(EqualityOperators)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check the equality operator
            Assert::IsTrue(a == a);
            Assert::IsTrue(a == Vector3(1.0f, 2.0f, 3.0f));
            Assert::IsFalse(a == b);
            Assert::IsFalse(b == a);

            // Check the inequality operator
            Assert::IsFalse(a != a);
            Assert::IsFalse(a != Vector3(1.0f, 2.0f, 3.0f));
            Assert::IsTrue(a != b);
            Assert::IsTrue(b != a);
        }

        TEST_METHOD(Vector3Operators)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check addition
            Assert::IsTrue(Vector3(-9.0f, 1.0f, 4.0f) == a + b);
            Assert::IsTrue(Vector3(-9.0f, 1.0f, 4.0f) == b + a);

            // Check subtraction
            Assert::IsTrue(Vector3(11.0f, 3.0f, 2.0f) == a - b);
            Assert::IsTrue(Vector3(-11.0f, -3.0f, -2.0f) == b - a);

            // Check multiplication
            Assert::IsTrue(Vector3(-10.0f, -2.0f, 3.0f) == a * b);
            Assert::IsTrue(Vector3(-10.0f, -2.0f, 3.0f) == b * a);

            // Check division
            Assert::IsTrue(Vector3(-0.1f, -2.0f, 3.0f) == a / b);
            Assert::IsTrue(Vector3(-10.0f, -0.5f, 1.0f / 3.0f) == b / a);
        }

        TEST_METHOD(ScalarOperations)
        {
            // Construct vectors
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(-10.0f, -1.0f, 1.0f);

            // Check scalar addition
            Assert::IsTrue(Vector3(3.0f, 4.0f, 5.0f) == a + 2.0f);
            Assert::IsTrue(Vector3(-20.0f, -11.0f, -9.0f) == b + (-10.0f));

            // Check scalar subtraction
            Assert::IsTrue(Vector3(-1.0f, 0.0f, 1.0f) == a - 2.0f);
            Assert::IsTrue(Vector3(0.0f, 9.0f, 11.0f) == b - (-10.0f));

            // Check scalar multiplication
            Assert::IsTrue(Vector3(2.0f, 4.0f, 6.0f) == a * 2.0f);
            Assert::IsTrue(Vector3(2.0f, 4.0f, 6.0f) == 2.0f * a);
            Assert::IsTrue(Vector3(0.0f, 0.0f, 0.0f) == b * 0.0f);
            Assert::IsTrue(Vector3(0.0f, 0.0f, 0.0f) == 0.0f * b);

            // Check scalar division
            Assert::IsTrue(Vector3(0.5f, 1.0f, 1.5f) == a / 2.0f);
            Assert::IsTrue(Vector3(6.0f, 3.0f, 2.0f) == 6.0f / a);
        }

    };
}