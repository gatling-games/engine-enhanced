#include "CppUnitTest.h"

#include "Math/Rect.h"
#include "Math/Point2.h"
#include "Math/Vector2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(RectTests)
    {
        const float tol = 0.000001f;

    public:

        TEST_METHOD(Constructor)
        {
            // Construct rects
            Rect a;
            Rect b(1.0f, 2.0f, 3.0f, 4.0f);

            // Check a contains all zeros
            Assert::AreEqual(0.0f, a.minx);
            Assert::AreEqual(0.0f, a.miny);
            Assert::AreEqual(0.0f, a.width);
            Assert::AreEqual(0.0f, a.height);

            // Check b is (1, 2, 3, 4)
            Assert::AreEqual(1.0f, b.minx, tol);
            Assert::AreEqual(2.0f, b.miny, tol);
            Assert::AreEqual(3.0f, b.width, tol);
            Assert::AreEqual(4.0f, b.height, tol);
        }

        TEST_METHOD(MinAndMax)
        {
            // Construct a rect
            Rect r(1.0f, 2.0f, 3.0f, 4.0f);

            // Check the min and max are correct
            Assert::IsTrue(Point2(1.0f, 2.0f) == r.min());
            Assert::IsTrue(Point2(1.0f + 3.0f, 2.0f + 4.0f) == r.max());
        }

        TEST_METHOD(Size)
        {
            // Construct a rect
            Rect r(1.0f, 2.0f, 3.0f, 4.0f);

            // Check the size is correct
            Assert::IsTrue(Vector2(3.0f, 4.0f) == r.size());
        }

        TEST_METHOD(RoundedToPixels)
        {
            // Construct rects with slightly offset coords
            Rect r1(1.0f, 2.0f, 3.0f, 4.0f);
            Rect r2(2.1f, 1.9f, 4.1f, 3.9f);
            Rect r3(3.4f, 5.8f, 200.0f, 199.9f);

            // Check that the rounded values are correct
            Assert::IsTrue(Point2(1.0f, 2.0f) == r1.roundedToPixels().min());
            Assert::IsTrue(Point2(4.0f, 6.0f) == r1.roundedToPixels().max());
            Assert::IsTrue(Vector2(3.0f, 4.0f) == r1.roundedToPixels().size());

            Assert::IsTrue(Point2(2.0f, 2.0f) == r2.roundedToPixels().min());
            Assert::IsTrue(Point2(6.0f, 6.0f) == r2.roundedToPixels().max());
            Assert::IsTrue(Vector2(4.0f, 4.0f) == r2.roundedToPixels().size());

            Assert::IsTrue(Point2(3.0f, 6.0f) == r3.roundedToPixels().min());
            Assert::IsTrue(Point2(203.0f, 206.0f) == r3.roundedToPixels().max());
            Assert::IsTrue(Vector2(200.0f, 200.0f) == r3.roundedToPixels().size());
        }
    };
}