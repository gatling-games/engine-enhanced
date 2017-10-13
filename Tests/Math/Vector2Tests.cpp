#include "CppUnitTest.h"

#include "Math/Vector2.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(Vector2Tests)
    {
        const float tol = 0.000001f;

    public:

        TEST_METHOD(Constructor)
        {
            // Construct vectors
            Vector2 a;
            Vector2 b(1.0f, 2.0f);

            // Check a contains all zeros
            Assert::AreEqual(0.0f, a.x);
            Assert::AreEqual(0.0f, a.y);

            // Check b is (1, 2, 3)
            Assert::AreEqual(1.0f, b.x, tol);
            Assert::AreEqual(2.0f, b.y, tol);
        }

        TEST_METHOD(Constants)
        {
            // Construct vectors
            Vector2 zero = Vector2::zero();
            Vector2 one = Vector2::one();
            Vector2 up = Vector2::up();
            Vector2 down = Vector2::down();
            Vector2 left = Vector2::left();
            Vector2 right = Vector2::right();

            // Check the values are correct
            Assert::IsTrue(Vector2(0.0f, 0.0f) == zero);
            Assert::IsTrue(Vector2(1.0f, 1.0f) == one);
            Assert::IsTrue(Vector2(0.0f, 1.0f) == up);
            Assert::IsTrue(Vector2(0.0f, -1.0f) == down);
            Assert::IsTrue(Vector2(-1.0f, 0.0f) == left);
            Assert::IsTrue(Vector2(1.0f, 0.0f) == right);
        }

        TEST_METHOD(MinComponent)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check minComponent of a is 1.0
            Assert::AreEqual(1.0f, a.minComponent(), tol);

            // Check minComponent of b is -10.0
            Assert::AreEqual(-10.0f, b.minComponent(), tol);
        }

        TEST_METHOD(MaxComponent)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check maxComponent of a is 2.0
            Assert::AreEqual(2.0f, a.maxComponent(), tol);

            // Check maxComponent of b is -1.0
            Assert::AreEqual(-1.0f, b.maxComponent(), tol);
        }

        TEST_METHOD(SqrMagnitude)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check sqrMagnitude of a is 5
            Assert::AreEqual(5.0f, a.sqrMagnitude(), tol);

            // Check sqrMagnitude of b is 101
            Assert::AreEqual(101.0f, b.sqrMagnitude(), tol);
        }

        TEST_METHOD(Magnitude)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check magnitude of a is sqrt(5)
            Assert::AreEqual(sqrtf(5.0f), a.magnitude(), tol);

            // Check magnitude of a is sqrt(101)
            Assert::AreEqual(sqrtf(101.0f), b.magnitude(), tol);
        }

        TEST_METHOD(Normalized)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Magnitude of a is sqrt(5)
            // Check a normalized is (1 / sqrt(5), 2 / sqrt(5))
            float aMag = sqrtf(5.0f);
            Assert::IsTrue(Vector2(1.0f / aMag, 2.0f / aMag) == a.normalized());

            // Magnitude of a is sqrt(102)
            // Check a normalized is (1 / sqrt(101), 2 / sqrt(101))
            float bMag = sqrtf(101.0f);
            Assert::IsTrue(Vector2(-10.0f / bMag, -1.0f / bMag) == b.normalized());
        }

        TEST_METHOD(Dot)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check the dot product is -12
            Assert::AreEqual(-12.0f, Vector2::dot(a, b), tol);
            Assert::AreEqual(-12.0f, Vector2::dot(b, a), tol);
        }

        TEST_METHOD(Angle)
        {
            // Construct vectors
            Vector2 up(0.0f, 1.0f);
            Vector2 diagonal(1.0f, 1.0f);
            Vector2 down(0.0f, -1.0f);

            // Check the angle between up and up is 0
            Assert::AreEqual(0.0f, Vector2::angle(up, up), tol);

            // Check the angle between diagonal and diagonal is 0
            Assert::AreEqual(0.0f, Vector2::angle(diagonal, diagonal), tol);

            // Check the angle between up and diagonal is PI/4 rad
            Assert::AreEqual((float)M_PI / 4.0f, Vector2::angle(up, diagonal), tol);
            Assert::AreEqual((float)M_PI / 4.0f, Vector2::angle(diagonal, up), tol);

            // Check the angle between up and down is 180 degrees (pi rad)
            Assert::AreEqual((float)M_PI, Vector2::angle(up, down), tol);
            Assert::AreEqual((float)M_PI, Vector2::angle(down, up), tol);
        }

        TEST_METHOD(LerpUnclamped)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Vector2::lerpUnclamped(a, b, 0.0f));
            Assert::IsTrue(b == Vector2::lerpUnclamped(b, a, 0.0f));
            Assert::IsTrue(b == Vector2::lerpUnclamped(a, b, 1.0f));
            Assert::IsTrue(a == Vector2::lerpUnclamped(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue((a * 0.9f) + (b * 0.1f) == Vector2::lerpUnclamped(a, b, 0.1f));
            Assert::IsTrue((b * 0.9f) + (a * 0.1f) == Vector2::lerpUnclamped(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(a + (b - a) * 2.0f == Vector2::lerpUnclamped(a, b, 2.0f));
            Assert::IsTrue(b + (a - b) * 2.0f == Vector2::lerpUnclamped(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a - (b - a) == Vector2::lerpUnclamped(a, b, -1.0f));
            Assert::IsTrue(b - (a - b) == Vector2::lerpUnclamped(b, a, -1.0f));
        }

        TEST_METHOD(Lerp)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Vector2::lerp(a, b, 0.0f));
            Assert::IsTrue(b == Vector2::lerp(b, a, 0.0f));
            Assert::IsTrue(b == Vector2::lerp(a, b, 1.0f));
            Assert::IsTrue(a == Vector2::lerp(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue((a * 0.9f) + (b * 0.1f) == Vector2::lerp(a, b, 0.1f));
            Assert::IsTrue((b * 0.9f) + (a * 0.1f) == Vector2::lerp(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(b == Vector2::lerp(a, b, 2.0f));
            Assert::IsTrue(a == Vector2::lerp(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a == Vector2::lerp(a, b, -1.0f));
            Assert::IsTrue(b == Vector2::lerp(b, a, -1.0f));
        }

        TEST_METHOD(EqualityOperators)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check the equality operator
            Assert::IsTrue(a == a);
            Assert::IsTrue(a == Vector2(1.0f, 2.0f));
            Assert::IsFalse(a == b);
            Assert::IsFalse(b == a);

            // Check the inequality operator
            Assert::IsFalse(a != a);
            Assert::IsFalse(a != Vector2(1.0f, 2.0f));
            Assert::IsTrue(a != b);
            Assert::IsTrue(b != a);
        }

        TEST_METHOD(Vector2Operators)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check addition
            Assert::IsTrue(Vector2(-9.0f, 1.0f) == a + b);
            Assert::IsTrue(Vector2(-9.0f, 1.0f) == b + a);

            // Check subtraction
            Assert::IsTrue(Vector2(11.0f, 3.0f) == a - b);
            Assert::IsTrue(Vector2(-11.0f, -3.0f) == b - a);

            // Check multiplication
            Assert::IsTrue(Vector2(-10.0f, -2.0f) == a * b);
            Assert::IsTrue(Vector2(-10.0f, -2.0f) == b * a);

            // Check division
            Assert::IsTrue(Vector2(-0.1f, -2.0f) == a / b);
            Assert::IsTrue(Vector2(-10.0f, -0.5f) == b / a);
        }

        TEST_METHOD(ScalarOperations)
        {
            // Construct vectors
            Vector2 a(1.0f, 2.0f);
            Vector2 b(-10.0f, -1.0f);

            // Check scalar addition
            Assert::IsTrue(Vector2(3.0f, 4.0f) == a + 2.0f);
            Assert::IsTrue(Vector2(-20.0f, -11.0f) == b + (-10.0f));

            // Check scalar subtraction
            Assert::IsTrue(Vector2(-1.0f, 0.0f) == a - 2.0f);
            Assert::IsTrue(Vector2(0.0f, 9.0f) == b - (-10.0f));

            // Check scalar multiplication
            Assert::IsTrue(Vector2(2.0f, 4.0f) == a * 2.0f);
            Assert::IsTrue(Vector2(2.0f, 4.0f) == 2.0f * a);
            Assert::IsTrue(Vector2(0.0f, 0.0f) == b * 0.0f);
            Assert::IsTrue(Vector2(0.0f, 0.0f) == 0.0f * b);

            // Check scalar division
            Assert::IsTrue(Vector2(0.5f, 1.0f) == a / 2.0f);
            Assert::IsTrue(Vector2(6.0f, 3.0f) == 6.0f / a);
        }

    };
}