#include "CppUnitTest.h"

#include "Math/Color.h"

#define _USE_MATH_DEFINES  // M_PI
#include <math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(ColorTests)
    {
        const float tol = 0.000001f;

    public:

        TEST_METHOD(Constructor)
        {
            // Construct colors
            Color a;
            Color b(0.2f, 0.3f, 0.4f);
            Color c(0.7f, 0.8f, 0.9f, 0.5f);

            // Check a is black
            Assert::AreEqual(0.0f, a.r, tol);
            Assert::AreEqual(0.0f, a.g, tol);
            Assert::AreEqual(0.0f, a.b, tol);
            Assert::AreEqual(1.0f, a.a, tol);

            // Check b is (0.1, 0.2, 0.3, 1.0)
            Assert::AreEqual(0.2f, b.r, tol);
            Assert::AreEqual(0.3f, b.g, tol);
            Assert::AreEqual(0.4f, b.b, tol);
            Assert::AreEqual(1.0f, b.a, tol);

            // Check c is (0.7, 0.8, 0.9, 0.5)
            Assert::AreEqual(0.7f, c.r, tol);
            Assert::AreEqual(0.8f, c.g, tol);
            Assert::AreEqual(0.9f, c.b, tol);
            Assert::AreEqual(0.5f, c.a, tol);
        }

        TEST_METHOD(MinColourComponent)
        {
            // Construct colors
            Color a(0.2f, 0.3f, 0.4f);
            Color b(0.7f, 0.8f, 0.9f, 0.5f);

            // Check minColorComponent of a is 0.2
            Assert::AreEqual(0.2f, a.minColorComponent(), tol);

            // Check minComponent of b is 0.7
            Assert::AreEqual(0.7f, b.minColorComponent(), tol);
        }

        TEST_METHOD(MaxColorComponent)
        {
            // Construct colors
            Color a(0.2f, 0.3f, 0.4f);
            Color b(0.7f, 0.8f, 0.9f, 0.5f);

            // Check maxColorComponent of a is 0.4
            Assert::AreEqual(0.4f, a.maxColorComponent(), tol);

            // Check maxColorComponent of b is 0.9
            Assert::AreEqual(0.9f, b.maxColorComponent(), tol);
        }

        TEST_METHOD(LerpUnclamped)
        {
            // Construct colors
            Color a(0.2f, 0.3f, 0.4f);
            Color b(0.7f, 0.8f, 0.9f, 0.5f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Color::lerpUnclamped(a, b, 0.0f));
            Assert::IsTrue(b == Color::lerpUnclamped(b, a, 0.0f));
            Assert::IsTrue(b == Color::lerpUnclamped(a, b, 1.0f));
            Assert::IsTrue(a == Color::lerpUnclamped(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue((a * 0.9f) + (b * 0.1f) == Color::lerpUnclamped(a, b, 0.1f));
            Assert::IsTrue((b * 0.9f) + (a * 0.1f) == Color::lerpUnclamped(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(a + (b - a) * 2.0f == Color::lerpUnclamped(a, b, 2.0f));
            Assert::IsTrue(b + (a - b) * 2.0f == Color::lerpUnclamped(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a - (b - a) == Color::lerpUnclamped(a, b, -1.0f));
            Assert::IsTrue(b - (a - b) == Color::lerpUnclamped(b, a, -1.0f));
        }

        TEST_METHOD(Lerp)
        {
            // Construct colors
            Color a(0.2f, 0.3f, 0.4f);
            Color b(0.7f, 0.8f, 0.9f, 0.5f);

            // Check the lerped value at t=0 and b at t=1
            Assert::IsTrue(a == Color::lerp(a, b, 0.0f));
            Assert::IsTrue(b == Color::lerp(b, a, 0.0f));
            Assert::IsTrue(b == Color::lerp(a, b, 1.0f));
            Assert::IsTrue(a == Color::lerp(b, a, 1.0f));

            // Check the lerped value at t = 0.1
            Assert::IsTrue((a * 0.9f) + (b * 0.1f) == Color::lerp(a, b, 0.1f));
            Assert::IsTrue((b * 0.9f) + (a * 0.1f) == Color::lerp(b, a, 0.1f));

            // Check the lerped value at t = 2.0
            Assert::IsTrue(b == Color::lerp(a, b, 2.0f));
            Assert::IsTrue(a == Color::lerp(b, a, 2.0f));

            // Check the lerped value at t = -1.0
            Assert::IsTrue(a == Color::lerp(a, b, -1.0f));
            Assert::IsTrue(b == Color::lerp(b, a, -1.0f));
        }

        TEST_METHOD(EqualityOperators)
        {
            // Construct colors
            Color a(0.2f, 0.3f, 0.4f);
            Color b(0.7f, 0.8f, 0.9f, 0.5f);

            // Check the equality operator
            Assert::IsTrue(a == a);
            Assert::IsTrue(a == Color(0.2f, 0.3f, 0.4f));
            Assert::IsFalse(a == b);
            Assert::IsFalse(b == a);

            // Check the inequality operator
            Assert::IsFalse(a != a);
            Assert::IsFalse(a != Color(0.2f, 0.3f, 0.4f));
            Assert::IsTrue(a != b);
            Assert::IsTrue(b != a);

            // Check that different alpha values affect the equality
            Color a_trans(a.r, a.g, a.b, 0.1f);
            Assert::IsTrue(a_trans == a_trans);
            Assert::IsFalse(a == a_trans);
            Assert::IsFalse(a_trans != a_trans);
            Assert::IsTrue(a != a_trans);
        }

        TEST_METHOD(ColorOperators)
        {
            // Construct vectors
            Color a(0.2f, 0.3f, 0.4f);
            Color b(0.7f, 0.8f, 0.9f, 0.5f);

            // Check addition
            Assert::IsTrue(Color(0.9f, 1.1f, 1.3f, 1.5f) == a + b);
            Assert::IsTrue(Color(0.9f, 1.1f, 1.3f, 1.5f) == b + a);

            // Check subtraction
            Assert::IsTrue(Color(-0.5f, -0.5f, -0.5f, 0.5f) == a - b);
            Assert::IsTrue(Color(0.5f, 0.5f, 0.5f, -0.5f) == b - a);

            // Check multiplication
            Assert::IsTrue(Color(0.2f * 0.7f, 0.3f * 0.8f, 0.4f * 0.9f, 0.5f) == a * b);
            Assert::IsTrue(Color(0.2f * 0.7f, 0.3f * 0.8f, 0.4f * 0.9f, 0.5f) == b * a);

            // Check division
            Assert::IsTrue(Color(0.2f / 0.7f, 0.3f / 0.8f, 0.4f / 0.9f, 1.0f / 0.5f) == a / b);
            Assert::IsTrue(Color(0.7f / 0.2f, 0.8f / 0.3f, 0.9f / 0.4f, 0.5f / 1.0f) == b / a);
        }

        TEST_METHOD(ScalarOperations)
        {
            // Construct vectors
            Color a(0.2f, 0.3f, 0.4f);
            Color b(0.7f, 0.8f, 0.9f, 0.5f);

            // Check scalar multiplication
            Assert::IsTrue(Color(0.4f, 0.6f, 0.8f, 2.0f) == a * 2.0f);
            Assert::IsTrue(Color(0.4f, 0.6f, 0.8f, 2.0f) == 2.0f * a);
            Assert::IsTrue(Color(0.0f, 0.0f, 0.0f, 0.0f) == b * 0.0f);
            Assert::IsTrue(Color(0.0f, 0.0f, 0.0f, 0.0f) == 0.0f * b);

            // Check scalar division
            Assert::IsTrue(Color(0.1f, 0.15f, 0.2f, 0.5f) == a / 2.0f);
        }

    };
}