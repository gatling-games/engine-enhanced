#include "CppUnitTest.h"

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	TEST_CLASS(Matrix4x4Tests)
	{
		const float tol = 0.000001f;

	public:

		TEST_METHOD(Constructor)
		{
			Matrix4x4 mat;

			// All 16 values should start as 0.
			for (int row = 0; row < 4; ++row)
			{
				for (int col = 0; col < 4; ++col)
				{
					Assert::AreEqual(0.0f, mat.get(row, col));
				}
			}
		}

		TEST_METHOD(Get)
		{
			// Create a matrix
			Matrix4x4 mat;

			// The matrix is column-major
			// Set (0, 0), (3, 0) and (1, 1) to 1
			// Leave everything else as 0.
			mat.elements[0] = 1.0f;
			mat.elements[12] = 1.0f;
			mat.elements[5] = 1.0f;

			// The matrix is column-major
			// check that (0,0) (3,0), (1,1) are 1 and everything else is 0
			for (int row = 0; row < 4; ++row)
			{
				for (int column = 0; column < 4; ++column)
				{
					float expected = (row == 0 && column == 0) || (row == 0 && column == 3) || (row == 1 && column == 1) ? 1.0f : 0.0f;
					Assert::AreEqual(expected, mat.get(row, column), tol);
				}
			}
		}

		TEST_METHOD(Set)
		{
			// Create a matrix
			Matrix4x4 mat;

			// Set (0, 0), (3, 0) and (1, 1) to 1
			// Leave everything else as 0.
			mat.set(0, 0, 1.0f);
			mat.set(0, 3, 1.0f);
			mat.set(1, 1, 1.0f);

			// The matrix is column-major
			// check that (0,0) (3,0), (1,1) are 1 and everything else is 0
			for (int i = 0; i < 16; ++i)
			{
				float expected = (i == 0 || i == 12 || i == 5) ? 1.0f : 0.0f;
				Assert::AreEqual(expected, mat.elements[i], tol);
			}
		}

		TEST_METHOD(SetRow)
		{
			// Create a matrix
			Matrix4x4 mat;

			// Make the matrix values increase
			mat.setRow(0, 0.0f, 4.0f, 8.0f, 12.0f);
			mat.setRow(1, 1.0f, 5.0f, 9.0f, 13.0f);
			mat.setRow(2, 2.0f, 6.0f, 10.0f, 14.0f);
			mat.setRow(3, 3.0f, 7.0f, 11.0f, 15.0f);

			// Check that the values increase
			for (int i = 0; i < 16; ++i)
			{
				float expected = (float)i;
				Assert::AreEqual(expected, mat.elements[i], tol);
			}
		}

		TEST_METHOD(SetCol)
		{
			// Create a matrix
			Matrix4x4 mat;

			// Make the matrix values increase
			mat.setCol(0, 0.0f, 1.0f, 2.0f, 3.0f);
			mat.setCol(1, 4.0f, 5.0f, 6.0f, 7.0f);
			mat.setCol(2, 8.0f, 9.0f, 10.0f, 11.0f);
			mat.setCol(3, 12.0f, 13.0f, 14.0f, 15.0f);

			// Check that the values increase
			for (int i = 0; i < 16; ++i)
			{
				float expected = (float)i;
				Assert::AreEqual(expected, mat.elements[i], tol);
			}
		}

		TEST_METHOD(Transpose)
		{
			// Create a matrix
			Matrix4x4 mat;

			// The matrix is column-major
			// Make the matrix values increase for row-major order
			// Then the transpose will make the elements increase
			mat.setRow(0, 0.0f, 1.0f, 2.0f, 3.0f);
			mat.setRow(1, 4.0f, 5.0f, 6.0f, 7.0f);
			mat.setRow(2, 8.0f, 9.0f, 10.0f, 11.0f);
			mat.setRow(3, 12.0f, 13.0f, 14.0f, 15.0f);

			// Transpose the matrix
			Matrix4x4 transpose = mat.transpose();

			// Check that the values increase
			for (int i = 0; i < 16; ++i)
			{
				float expected = (float)i;
				Assert::AreEqual(expected, transpose.elements[i], tol);
			}
		}

		TEST_METHOD(Identity)
		{
			// Create an identity matrix
			Matrix4x4 mat = Matrix4x4::identity();

			// Check that all values are 0
			// except for 1s on the diagonal
			for (int row = 0; row < 4; ++row)
			{
				for (int column = 0; column < 4; ++column)
				{
					float expected = (row == column) ? 1.0f : 0.0f;
					Assert::AreEqual(expected, mat.get(row, column), tol);
				}
			}
		}

		TEST_METHOD(Translation)
		{
			// Create a translation matrix
			Vector3 translation(5.0f, 6.0f, 7.0f);
			Matrix4x4 mat = Matrix4x4::translation(translation);

			// Check that every element matches the identity matrix,
			// except for elements 12, 13 & 14
			for (int row = 0; row < 4; ++row)
			{
				for (int column = 0; column < 4; ++column)
				{
					// The translation values should be 5, 6 and 7
					// Everything else is the normal identity matrix
					float expected;
					if (row == 0 && column == 3) expected = 5.0f;
					else if (row == 1 && column == 3) expected = 6.0f;
					else if (row == 2 && column == 3) expected = 7.0f;
					else expected = (row == column) ? 1.0f : 0.0f;

					Assert::AreEqual(expected, mat.get(row, column), tol);
				}
			}
		}

		TEST_METHOD(Rotation)
		{
			// Create two quaternions
			Quaternion a(0.2f, 0.3f, 0.4f, sqrtf(1.0f - (0.2f * 0.2f + 0.3f * 0.3f + 0.4f * 0.4f)));
			Quaternion b(0.3f, 0.4f, 0.5f, sqrtf(1.0f - (0.3f * 0.3f + 0.4f * 0.4f + 0.5f * 0.5f)));

			// Convert the quaternions to matrices
			Matrix4x4 aMat = Matrix4x4::rotation(a);
			Matrix4x4 bMat = Matrix4x4::rotation(b);

			// Check the first matrix is correct
			// [0.5       -0.554092 0.665569  0]
			// [0.794092  0.6       -0.097046 0]
			// [-0.345569 0.577046  0.74      0]
			// [0         0         0         1]
			Assert::AreEqual(0.5f, aMat.get(0, 0), tol);
			Assert::AreEqual(-0.554092f, aMat.get(0, 1), tol);
			Assert::AreEqual(0.665569f, aMat.get(0, 2), tol);
			Assert::AreEqual(0.0f, aMat.get(0, 3), tol);
			Assert::AreEqual(0.794092f, aMat.get(1, 0), tol);
			Assert::AreEqual(0.6f, aMat.get(1, 1), tol);
			Assert::AreEqual(-0.097046f, aMat.get(1, 2), tol);
			Assert::AreEqual(0.0f, aMat.get(1, 3), tol);
			Assert::AreEqual(-0.345569f, aMat.get(2, 0), tol);
			Assert::AreEqual(0.577046f, aMat.get(2, 1), tol);
			Assert::AreEqual(0.74f, aMat.get(2, 2), tol);
			Assert::AreEqual(0.0f, aMat.get(2, 3), tol);
			Assert::AreEqual(0.0f, aMat.get(3, 0), tol);
			Assert::AreEqual(0.0f, aMat.get(3, 1), tol);
			Assert::AreEqual(0.0f, aMat.get(3, 2), tol);
			Assert::AreEqual(1.0f, aMat.get(3, 3), tol);

			// Check the second matrix is correct
			// [0.18       -0.4671068  0.8656854  0]
			// [0.9471068  0.32        -0.0242641 0]
			// [-0.2656854 0.824264    0.5      0]
			// [0          0           0         1]
			Assert::AreEqual(0.18f, bMat.get(0, 0), tol);
			Assert::AreEqual(-0.4671068f, bMat.get(0, 1), tol);
			Assert::AreEqual(0.8656854f, bMat.get(0, 2), tol);
			Assert::AreEqual(0.0f, bMat.get(0, 3), tol);
			Assert::AreEqual(0.9471068f, bMat.get(1, 0), tol);
			Assert::AreEqual(0.32f, bMat.get(1, 1), tol);
			Assert::AreEqual(-0.0242641f, bMat.get(1, 2), tol);
			Assert::AreEqual(0.0f, bMat.get(1, 3), tol);
			Assert::AreEqual(-0.2656854f, bMat.get(2, 0), tol);
			Assert::AreEqual(0.824264f, bMat.get(2, 1), tol);
			Assert::AreEqual(0.5f, bMat.get(2, 2), tol);
			Assert::AreEqual(0.0f, bMat.get(2, 3), tol);
			Assert::AreEqual(0.0f, bMat.get(3, 0), tol);
			Assert::AreEqual(0.0f, bMat.get(3, 1), tol);
			Assert::AreEqual(0.0f, bMat.get(3, 2), tol);
			Assert::AreEqual(1.0f, bMat.get(3, 3), tol);
		}

		TEST_METHOD(Scale)
		{
			// Create a scale matrix
			Vector3 scale(5.0f, 6.0f, 7.0f);
			Matrix4x4 mat = Matrix4x4::scale(scale);

			// Check that every element matches the identity matrix,
			// except for elements (0,0) (1,1) (2,2), which should contain the scale
			for (int row = 0; row < 4; ++row)
			{
				for (int column = 0; column < 4; ++column)
				{
					// The scale values are in the diagonal.
					// Everything else is the normal identity matrix
					float expected;
					if (row == 0 && column == 0) expected = scale.x;
					else if (row == 1 && column == 1) expected = scale.y;
					else if (row == 2 && column == 2) expected = scale.z;
					else expected = (row == column) ? 1.0f : 0.0f;

					Assert::AreEqual(expected, mat.get(row, column), tol);
				}
			}
		}

		TEST_METHOD(ScalarOperations)
		{
			// Create a matrix 
			Matrix4x4 mat;
			mat.setCol(0, 0.0f, 1.0f, 2.0f, 3.0f);
			mat.setCol(1, 4.0f, 5.0f, 6.0f, 7.0f);
			mat.setCol(2, 8.0f, 9.0f, 10.0f, 11.0f);
			mat.setCol(3, 12.0f, 13.0f, 14.0f, 15.0f);

			// Check Matrix * scalar works
			Matrix4x4 matrixTimesScalar = mat * 5.0f;
			for (int i = 0; i < 16; ++i)
			{
				Assert::AreEqual(i * 5.0f, matrixTimesScalar.elements[i], tol);
			}

			// Check scalar * Matrix works
			Matrix4x4 scalarTimesMatrix = 2.0f * mat;
			for (int i = 0; i < 16; ++i)
			{
				Assert::AreEqual(i * 2.0f, scalarTimesMatrix.elements[i], tol);
			}
		}

        TEST_METHOD(Invert)
		{
            Matrix4x4 a;
            a.setRow(0, 1.0f, -2.0f, -3.0f, 4.0f);
            a.setRow(1, 3.0f, 0.5f, 0.0f, -1.0f);
            a.setRow(2, 0.0f, 8.5f, 1.2f, -9.0f);
            a.setRow(3, 5.0f, -5.0f, 1.0f, -1.0f);

            Matrix4x4 b = a.invert();

            const Matrix4x4 I = a * b;

            Assert::AreEqual(1.0f, I.get(0, 0), tol);
            Assert::AreEqual(0.0f, I.get(0, 1), tol);
            Assert::AreEqual(0.0f, I.get(0, 2), tol);
            Assert::AreEqual(0.0f, I.get(0, 3), tol);
            Assert::AreEqual(0.0f, I.get(1, 0), tol);
            Assert::AreEqual(1.0f, I.get(1, 1), tol);
            Assert::AreEqual(0.0f, I.get(1, 2), tol);
            Assert::AreEqual(0.0f, I.get(1, 3), tol);
            Assert::AreEqual(0.0f, I.get(2, 0), tol);
            Assert::AreEqual(0.0f, I.get(2, 1), tol);
            Assert::AreEqual(1.0f, I.get(2, 2), tol);
            Assert::AreEqual(0.0f, I.get(2, 3), tol);
            Assert::AreEqual(0.0f, I.get(3, 0), tol);
            Assert::AreEqual(0.0f, I.get(3, 1), tol);
            Assert::AreEqual(0.0f, I.get(3, 2), tol);
            Assert::AreEqual(1.0f, I.get(3, 3), tol);
		}

	};
}