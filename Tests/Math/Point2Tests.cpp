#include "CppUnitTest.h"

#include "Math/Point2.h"
#include "Math/Vector2.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(Point2Tests)
    {
        const float tol = 0.000001f;

    public:

        TEST_METHOD(Constructor)
        {
            // Construct points
            Point2 a;
            Point2 b(1.0f, 2.0f);
            Point2 c(Vector2(5.0f, 4.0f));

            // Check a contains all zeros
            Assert::AreEqual(0.0f, a.x);
            Assert::AreEqual(0.0f, a.y);

            // Check b is (1, 2)
            Assert::AreEqual(1.0f, b.x, tol);
            Assert::AreEqual(2.0f, b.y, tol);

            // CHeck c is (5, 4)
            Assert::AreEqual(5.0f, c.x, tol);
            Assert::AreEqual(4.0f, c.y, tol);
        }

        TEST_METHOD(Constants)
        {
            // Construct points
            Point2 origin = Point2::origin();

            // Check the values are correct
            Assert::IsTrue(Point2(0.0f, 0.0f) == origin);
        }

        TEST_METHOD(SqrDistance)
        {
            // Construct points
            Point2 a(1.0f, 2.0f);
            Point2 b(-10.0f, -1.0f);

            // Check the square distance between the points
            Assert::AreEqual(11.0f * 11.0f + 3.0f * 3.0f, Point2::sqrDistance(a, b), tol);
            Assert::AreEqual(11.0f * 11.0f + 3.0f * 3.0f, Point2::sqrDistance(b, a), tol);
        }

        TEST_METHOD(Distance)
        {
            // Construct points
            Point2 a(1.0f, 2.0f);
            Point2 b(-10.0f, -1.0f);

            // Check the distance between the points
            Assert::AreEqual(sqrtf(11.0f * 11.0f + 3.0f * 3.0f), Point2::distance(a, b), tol);
            Assert::AreEqual(sqrtf(11.0f * 11.0f + 3.0f * 3.0f), Point2::distance(b, a), tol);
        }

        TEST_METHOD(LerpUnclamped)
        {
            // Construct points
            Point2 a(1.0f, 2.0f);
            Point2 b(-10.0f, -1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Point2::lerpUnclamped(a, b, 0.0f));
            Assert::IsTrue(b == Point2::lerpUnclamped(b, a, 0.0f));
            Assert::IsTrue(b == Point2::lerpUnclamped(a, b, 1.0f));
            Assert::IsTrue(a == Point2::lerpUnclamped(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue(a + (b - a) * 0.1f == Point2::lerpUnclamped(a, b, 0.1f));
            Assert::IsTrue(b + (a - b) * 0.1f == Point2::lerpUnclamped(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(a + (b - a) * 2.0f == Point2::lerpUnclamped(a, b, 2.0f));
            Assert::IsTrue(b + (a - b) * 2.0f == Point2::lerpUnclamped(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a - (b - a) == Point2::lerpUnclamped(a, b, -1.0f));
            Assert::IsTrue(b - (a - b) == Point2::lerpUnclamped(b, a, -1.0f));
        }

        TEST_METHOD(Lerp)
        {
            // Construct points
            Point2 a(1.0f, 2.0f);
            Point2 b(-10.0f, -1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Point2::lerp(a, b, 0.0f));
            Assert::IsTrue(b == Point2::lerp(b, a, 0.0f));
            Assert::IsTrue(b == Point2::lerp(a, b, 1.0f));
            Assert::IsTrue(a == Point2::lerp(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue(a + (b - a) * 0.1f == Point2::lerp(a, b, 0.1f));
            Assert::IsTrue(b + (a - b) * 0.1f == Point2::lerp(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(b == Point2::lerp(a, b, 2.0f));
            Assert::IsTrue(a == Point2::lerp(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a == Point2::lerp(a, b, -1.0f));
            Assert::IsTrue(b == Point2::lerp(b, a, -1.0f));
        }

        TEST_METHOD(EqualityOperators)
        {
            // Construct points
            Point2 a(1.0f, 2.0f);
            Point2 b(-10.0f, -1.0f);

            // Check the equality operator
            Assert::IsTrue(a == a);
            Assert::IsTrue(a == Point2(1.0f, 2.0f));
            Assert::IsFalse(a == b);
            Assert::IsFalse(b == a);

            // Check the inequality operator
            Assert::IsFalse(a != a);
            Assert::IsFalse(a != Point2(1.0f, 2.0f));
            Assert::IsTrue(a != b);
            Assert::IsTrue(b != a);
        }

        TEST_METHOD(Point2Operators)
        {
            // Construct points
            Point2 a(1.0f, 2.0f);
            Point2 b(-10.0f, -1.0f);

            // Check subtraction
            Assert::IsTrue(Vector2(11.0f, 3.0f) == a - b);
            Assert::IsTrue(Vector2(-11.0f, -3.0f) == b - a);
        }

        TEST_METHOD(Vector2Operators)
        {
            // Construct a point and vector
            Point2 p(1.0f, 2.0f);
            Vector2 v(-10.0f, -1.0f);

            // Check addition
            Assert::IsTrue(Point2(-9.0f, 1.0f) == p + v);

            // Check subtraction
            Assert::IsTrue(Point2(11.0f, 3.0f) == p - v);
        }

        TEST_METHOD(ScalarOperations)
        {
            // Construct points
            Point2 a(1.0f, 2.0f);
            Point2 b(-10.0f, -1.0f);

            // Check scalar multiplication
            Assert::IsTrue(Point2(2.0f, 4.0f) == a * 2.0f);
            Assert::IsTrue(Point2(2.0f, 4.0f) == 2.0f * a);
            Assert::IsTrue(Point2(0.0f, 0.0f) == b * 0.0f);
            Assert::IsTrue(Point2(0.0f, 0.0f) == 0.0f * b);

            // Check scalar division
            Assert::IsTrue(Point2(0.5f, 1.0f) == a / 2.0f);
            Assert::IsTrue(Point2(6.0f, 3.0f) == 6.0f / a);
        }

    };
}