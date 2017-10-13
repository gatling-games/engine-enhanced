#include "CppUnitTest.h"

#include "Math/Quaternion.h"
#include "Math/Vector3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    TEST_CLASS(QuaternionTests)
    {
    public:

        TEST_METHOD(Constructor)
        {
            // Construct quaternions
            Quaternion a;
            Quaternion b(0.1f, 0.2f, 0.3f, 0.4f);

            // Check a is the identity quaternion
            Assert::AreEqual(0.0f, a.x);
            Assert::AreEqual(0.0f, a.y);
            Assert::AreEqual(0.0f, a.z);
            Assert::AreEqual(1.0f, a.w);

            // Check b has the components specified above.
            Assert::AreEqual(0.1f, b.x);
            Assert::AreEqual(0.2f, b.y);
            Assert::AreEqual(0.3f, b.z);
            Assert::AreEqual(0.4f, b.w);
        }

        TEST_METHOD(Identity)
        {
            // Construct an identity quaternion
            Quaternion q = Quaternion::identity();

            // Check the components equal [0, 0, 0, 1]
            Assert::AreEqual(0.0f, q.x);
            Assert::AreEqual(0.0f, q.y);
            Assert::AreEqual(0.0f, q.z);
            Assert::AreEqual(1.0f, q.w);
        }

        TEST_METHOD(Conjugate)
        {
            // Construct a quaternion [0, 1, 2, 3]
            Quaternion q(0.0f, 1.0f, 2.0f, 3.0f);

            // Compute its conjugate
            Quaternion conj = q.conjugate();

            // Check the conjugate is [0, -1, -2, 3]
            Assert::AreEqual(0.0f, conj.x);
            Assert::AreEqual(-1.0f, conj.y);
            Assert::AreEqual(-2.0f, conj.z);
            Assert::AreEqual(3.0f, conj.w);
        }

        TEST_METHOD(Inverse)
        {
            // Construct a unit quaternion
            Quaternion quat(sqrtf(0.1f), sqrtf(0.2f), sqrtf(0.3f), sqrtf(0.4f));

            // Compute the inverse
            Quaternion inverse = quat.inverse();

            // Check the inverse is [-0.316228 -0.447214 -0.547723 0.632456]
            const float tol = 0.000001f;
            Assert::AreEqual(-0.316228f, inverse.x, tol);
            Assert::AreEqual(-0.447214f, inverse.y, tol);
            Assert::AreEqual(-0.547723f, inverse.z, tol);
            Assert::AreEqual(0.632456f, inverse.w, tol);
        }

        TEST_METHOD(Multiplication)
        {
            // Construct 2 unit quaternions.
            Quaternion a(sqrtf(0.1f), sqrtf(0.2f), sqrtf(0.3f), sqrtf(0.4f));
            Quaternion b(sqrt(0.25f), sqrtf(0.1f), sqrtf(0.2f), sqrtf(0.45f));

            // Compute a * b and b * a
            Quaternion ab = a * b;
            Quaternion ba = b * a;

            // Check a * b is [0.555155 0.63244 0.526659 -0.12022]
            const float tol = 0.000001f;
            Assert::AreEqual(0.555155f, ab.x, tol);
            Assert::AreEqual(0.63244f, ab.y, tol);
            Assert::AreEqual(0.526659f, ab.z, tol);
            Assert::AreEqual(-0.12022f, ab.w, tol);

            // Check b * a is [0.501565 0.36756 0.773873 -0.12022]
            Assert::AreEqual(0.501565f, ba.x, tol);
            Assert::AreEqual(0.36756f, ba.y, tol);
            Assert::AreEqual(0.773873f, ba.z, tol);
            Assert::AreEqual(-0.12022f, ba.w, tol);
        }

        TEST_METHOD(Rotation)
        {
            // Construct three rotation quaternions
            Quaternion a = Quaternion::rotation(30.0f, Vector3::up());
            Quaternion b = Quaternion::rotation(55.0f, Vector3(1.0f, 1.0f, 0.0f));
            Quaternion c = Quaternion::rotation(360.0f, Vector3(1.0f, 1.0f, 1.0f));

            // Check a is [0 0.258819 0 0.9659258]
            const float tol = 0.0000001f;
            Assert::AreEqual(0.0f, a.x, tol);
            Assert::AreEqual(0.258819f, a.y, tol);
            Assert::AreEqual(0.0f, a.z, tol);
            Assert::AreEqual(0.9659258f, a.w, tol);

            // Check b is [0.3265056 0.3265056 0 0.8870108]
            Assert::AreEqual(0.3265056f, b.x, tol);
            Assert::AreEqual(0.3265056f, b.y, tol);
            Assert::AreEqual(0.0f, b.z, tol);
            Assert::AreEqual(0.8870108f, b.w, tol);

            // Check c is [0 0 0 -1]
            Assert::AreEqual(0.0f, c.x, tol);
            Assert::AreEqual(0.0f, c.y, tol);
            Assert::AreEqual(0.0f, c.z, tol);
            Assert::AreEqual(-1.0f, c.w, tol);
        }

        TEST_METHOD(Euler)
        {
            // Construct two quaternions
            Quaternion a = Quaternion::euler(30.0f, 20.0f, 0.0f);
            Quaternion b = Quaternion::euler(Vector3(180.0f, 20.0f, 100.0f));

            // Quaternion::euler uses z first, then x, then y order.

            // Check a is [ 0.254887 0.1677313 -0.0449435 0.9512512] 
            const float tol = 0.0000001f;
            Assert::AreEqual(0.254887f, a.x, tol);
            Assert::AreEqual(0.1677313f, a.y, tol);
            Assert::AreEqual(-0.0449435f, a.z, tol);
            Assert::AreEqual(0.9512512f, a.w, tol);

            // Check b is [0.6330222 -0.7544065 -0.1116189 0.1330222]
            Assert::AreEqual(0.6330222f, b.x, tol);
            Assert::AreEqual(-0.7544065f, b.y, tol);
            Assert::AreEqual(-0.1116189f, b.z, tol);
            Assert::AreEqual(0.1330222f, b.w, tol);
        }

        TEST_METHOD(MultiplyVector)
        {
            // Construct 2 unit quaternions.
            Quaternion a(sqrtf(0.1f), sqrtf(0.2f), sqrtf(0.3f), sqrtf(0.4f));
            Quaternion b(sqrt(0.25f), sqrtf(0.1f), sqrtf(0.2f), sqrtf(0.45f));

            // Construct 2 non-normalized vectors
            Vector3 v1(0.1f, 10.0f, 100.0f);
            Vector3 v2(0.0f, 0.0f, 2.0f);

            // Compute the rotated vectors
            Vector3 av1 = a * v1;
            Vector3 av2 = a * v2;
            Vector3 bv1 = b * v1;
            Vector3 bv2 = b * v2;

            // Check a * v1 is (87.10978, 11.08736, 48.87706)
            const float tol = 0.0001f;
            Assert::AreEqual(87.10978f, av1.x, tol);
            Assert::AreEqual(11.08736f, av1.y, tol);
            Assert::AreEqual(48.87706f, av1.z, tol);

            // Check a * v2 is (1.824191, 0.1797959, 0.8000001)
            Assert::AreEqual(1.824191f, av2.x, tol);
            Assert::AreEqual(0.1797959f, av2.y, tol);
            Assert::AreEqual(0.8000001f, av2.z, tol);

            // Check b * v1 is (84.35004, -37.70614, 39.53893)
            Assert::AreEqual(84.35004f, bv1.x, tol);
            Assert::AreEqual(-37.70614f, bv1.y, tol);
            Assert::AreEqual(39.53893f, bv1.z, tol);

            // Check b * v2 is (1.742955, -0.7759553, 0.6)
            Assert::AreEqual(1.742955f, bv2.x, tol);
            Assert::AreEqual(-0.7759553f, bv2.y, tol);
            Assert::AreEqual(0.6f, bv2.z, tol);
        }

    };
}