#include "CppUnitTest.h"

#include "Math/Point3.h"
#include "Math/Vector3.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(Point3Tests)
    {
        const float tol = 0.000001f;

    public:

        TEST_METHOD(Constructor)
        {
            // Construct points
            Point3 a;
            Point3 b(1.0f, 2.0f, 3.0f);
            Point3 c(Vector3(5.0f, 4.0f, 3.0f));

            // Check a contains all zeros
            Assert::AreEqual(0.0f, a.x);
            Assert::AreEqual(0.0f, a.y);
            Assert::AreEqual(0.0f, a.z);

            // Check b is (1, 2, 3)
            Assert::AreEqual(1.0f, b.x, tol);
            Assert::AreEqual(2.0f, b.y, tol);
            Assert::AreEqual(3.0f, b.z, tol);

            // CHeck c is (5, 4, 3)
            Assert::AreEqual(5.0f, c.x, tol);
            Assert::AreEqual(4.0f, c.y, tol);
            Assert::AreEqual(3.0f, c.z, tol);
        }

        TEST_METHOD(Constants)
        {
            // Construct points
            Point3 origin = Point3::origin();

            // Check the values are correct
            Assert::IsTrue(Point3(0.0f, 0.0f, 0.0f) == origin);
        }

        TEST_METHOD(SqrDistance)
        {
            // Construct points
            Point3 a(1.0f, 2.0f, 3.0f);
            Point3 b(-10.0f, -1.0f, 1.0f);

            // Check the square distance between the points
            Assert::AreEqual(11.0f * 11.0f + 3.0f * 3.0f + 2.0f * 2.0f, Point3::sqrDistance(a, b), tol);
            Assert::AreEqual(11.0f * 11.0f + 3.0f * 3.0f + 2.0f * 2.0f, Point3::sqrDistance(b, a), tol);
        }

        TEST_METHOD(Distance)
        {
            // Construct points
            Point3 a(1.0f, 2.0f, 3.0f);
            Point3 b(-10.0f, -1.0f, 1.0f);

            // Check the distance between the points
            Assert::AreEqual(sqrtf(11.0f * 11.0f + 3.0f * 3.0f + 2.0f * 2.0f), Point3::distance(a, b), tol);
            Assert::AreEqual(sqrtf(11.0f * 11.0f + 3.0f * 3.0f + 2.0f * 2.0f), Point3::distance(b, a), tol);
        }

        TEST_METHOD(LerpUnclamped)
        {
            // Construct points
            Point3 a(1.0f, 2.0f, 3.0f);
            Point3 b(-10.0f, -1.0f, 1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Point3::lerpUnclamped(a, b, 0.0f));
            Assert::IsTrue(b == Point3::lerpUnclamped(b, a, 0.0f));
            Assert::IsTrue(b == Point3::lerpUnclamped(a, b, 1.0f));
            Assert::IsTrue(a == Point3::lerpUnclamped(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue(a + (b - a) * 0.1f == Point3::lerpUnclamped(a, b, 0.1f));
            Assert::IsTrue(b + (a - b) * 0.1f == Point3::lerpUnclamped(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(a + (b - a) * 2.0f == Point3::lerpUnclamped(a, b, 2.0f));
            Assert::IsTrue(b + (a - b) * 2.0f == Point3::lerpUnclamped(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a - (b - a) == Point3::lerpUnclamped(a, b, -1.0f));
            Assert::IsTrue(b - (a - b) == Point3::lerpUnclamped(b, a, -1.0f));
        }

        TEST_METHOD(Lerp)
        {
            // Construct points
            Point3 a(1.0f, 2.0f, 3.0f);
            Point3 b(-10.0f, -1.0f, 1.0f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Point3::lerp(a, b, 0.0f));
            Assert::IsTrue(b == Point3::lerp(b, a, 0.0f));
            Assert::IsTrue(b == Point3::lerp(a, b, 1.0f));
            Assert::IsTrue(a == Point3::lerp(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue(a + (b - a) * 0.1f == Point3::lerp(a, b, 0.1f));
            Assert::IsTrue(b + (a - b) * 0.1f == Point3::lerp(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(b == Point3::lerp(a, b, 2.0f));
            Assert::IsTrue(a == Point3::lerp(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a == Point3::lerp(a, b, -1.0f));
            Assert::IsTrue(b == Point3::lerp(b, a, -1.0f));
        }

        TEST_METHOD(EqualityOperators)
        {
            // Construct points
            Point3 a(1.0f, 2.0f, 3.0f);
            Point3 b(-10.0f, -1.0f, 1.0f);

            // Check the equality operator
            Assert::IsTrue(a == a);
            Assert::IsTrue(a == Point3(1.0f, 2.0f, 3.0f));
            Assert::IsFalse(a == b);
            Assert::IsFalse(b == a);

            // Check the inequality operator
            Assert::IsFalse(a != a);
            Assert::IsFalse(a != Point3(1.0f, 2.0f, 3.0f));
            Assert::IsTrue(a != b);
            Assert::IsTrue(b != a);
        }

        TEST_METHOD(Point3Operators)
        {
            // Construct points
            Point3 a(1.0f, 2.0f, 3.0f);
            Point3 b(-10.0f, -1.0f, 1.0f);

            // Check subtraction
            Assert::IsTrue(Vector3(11.0f, 3.0f, 2.0f) == a - b);
            Assert::IsTrue(Vector3(-11.0f, -3.0f, -2.0f) == b - a);
        }

        TEST_METHOD(Vector3Operators)
        {
            // Construct a point and vector
            Point3 p(1.0f, 2.0f, 3.0f);
            Vector3 v(-10.0f, -1.0f, 1.0f);

            // Check addition
            Assert::IsTrue(Point3(-9.0f, 1.0f, 4.0f) == p + v);

            // Check subtraction
            Assert::IsTrue(Point3(11.0f, 3.0f, 2.0f) == p - v);
        }

        TEST_METHOD(ScalarOperations)
        {
            // Construct points
            Point3 a(1.0f, 2.0f, 3.0f);
            Point3 b(-10.0f, -1.0f, 1.0f);

            // Check scalar multiplication
            Assert::IsTrue(Point3(2.0f, 4.0f, 6.0f) == a * 2.0f);
            Assert::IsTrue(Point3(2.0f, 4.0f, 6.0f) == 2.0f * a);
            Assert::IsTrue(Point3(0.0f, 0.0f, 0.0f) == b * 0.0f);
            Assert::IsTrue(Point3(0.0f, 0.0f, 0.0f) == 0.0f * b);

            // Check scalar division
            Assert::IsTrue(Point3(0.5f, 1.0f, 1.5f) == a / 2.0f);
            Assert::IsTrue(Point3(6.0f, 3.0f, 2.0f) == 6.0f / a);
        }

    };
}