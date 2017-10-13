#include "CppUnitTest.h"

#include "Math/Point3.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(Vector4Tests)
    {
        const float tol = 0.000001f;

    public:

        TEST_METHOD(Constructor)
        {
            // Construct vectors
            Vector4 a;
            Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 c(Point3(0.1f, 0.2f, 0.3f));
            Vector4 d(Vector3(10.0f, 20.0f, 30.0f));

            // Check a contains all zeros
            Assert::AreEqual(0.0f, a.x, tol);
            Assert::AreEqual(0.0f, a.y, tol);
            Assert::AreEqual(0.0f, a.z, tol);
            Assert::AreEqual(0.0f, a.w, tol);

            // Check b is (1, 2, 3, 4)
            Assert::AreEqual(1.0f, b.x, tol);
            Assert::AreEqual(2.0f, b.y, tol);
            Assert::AreEqual(3.0f, b.z, tol);
            Assert::AreEqual(4.0f, b.w, tol);

            // Check c is (0.1, 0.2, 0.3, 1.0)
            Assert::AreEqual(0.1f, c.x, tol);
            Assert::AreEqual(0.2f, c.y, tol);
            Assert::AreEqual(0.3f, c.z, tol);
            Assert::AreEqual(1.0f, c.w, tol);

            // Check d is (10.0, 20.0, 30.0, 0.0)
            Assert::AreEqual(10.0f, d.x, tol);
            Assert::AreEqual(20.0f, d.y, tol);
            Assert::AreEqual(30.0f, d.z, tol);
            Assert::AreEqual(0.0f, d.w, tol);
        }

        TEST_METHOD(MinComponent)
        {
            // Construct vectors
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(-10.0f, -1.0f, 1.0f, -20.0f);

            // Check minComponent of a is 1.0
            Assert::AreEqual(1.0f, a.minComponent(), tol);

            // Check minComponent of b is -20.0
            Assert::AreEqual(-20.0f, b.minComponent(), tol);
        }

        TEST_METHOD(MaxComponent)
        {
            // Construct vectors
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(-10.0f, -1.0f, 1.0f, -20.0f);

            // Check maxComponent of a is 4.0
            Assert::AreEqual(4.0f, a.maxComponent(), tol);

            // Check maxComponent of b is 1.0
            Assert::AreEqual(1.0f, b.maxComponent(), tol);
        }

        TEST_METHOD(AsPoint)
        {
            // Construct a vector4
            Vector4 v(1.0f, 2.0f, 3.0f, 2.0f);

            // Convert the vector4 to a point
            Point3 p3 = v.asPoint();

            // Check that the point is (0.5, 1.0, 1.5)
            // Due to homogeneous w = 2.0
            Assert::AreEqual(0.5f, p3.x, tol);
            Assert::AreEqual(1.0f, p3.y, tol);
            Assert::AreEqual(1.5f, p3.z, tol);
        }

        TEST_METHOD(AsVector)
        {
            // Construct a vector4
            Vector4 v(1.0f, 2.0f, 3.0f, 0.0f);

            // Convert the vector4 to a point
            Vector3 v3 = v.asVector();

            // Check that the point is (1.0f, 2.0f, 3.0f)
            Assert::AreEqual(1.0f, v3.x, tol);
            Assert::AreEqual(2.0f, v3.y, tol);
            Assert::AreEqual(3.0f, v3.z, tol);
        }

    };
}