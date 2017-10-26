#include "CppUnitTest.h"

#include "Utils/BitReader.h"
#include "Utils/BitWriter.h"
#include <cstring>
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UtilTests
{
	TEST_CLASS(BitReaderTests)
	{

	public:

		TEST_METHOD(Constructor)
		{
			uint32_t* buffer = new uint32_t[1];
			BitReader r = BitReader(buffer);
		}

		TEST_METHOD(CheckReadByte)
		{
			BitWriter b;
			b.writeByte(4);
			b.writeByte(3);

			uint32_t* buffer = b.getBuffer();
			BitReader r = BitReader(buffer);
			int t1 = r.readByte();
			int t2 = r.readByte();
			Assert::AreEqual(4,t1);
			Assert::AreEqual(3, t2);

		}

		TEST_METHOD(CheckReadBits)
		{
			BitWriter b;
			b.writeBits(4, 3);
			b.writeBits(3, 10);
			b.writeBits(5, 24);

			uint32_t* buffer = b.getBuffer();
			BitReader r = BitReader(buffer);
			int t1 = r.readBits(3);
			int t2 = r.readBits(10);
			int t3 = r.readBits(24);
			Assert::AreEqual(4, t1);
			Assert::AreEqual(3, t2);
			Assert::AreEqual(5, t3);
		}

	};
}