#include "CppUnitTest.h"

#include "Utils/BitWriter.h"
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UtilTests
{
    TEST_CLASS(BitWriterTests)
    {

    public:

        TEST_METHOD(Constructor)
        {
            BitWriter b;
            Assert::AreEqual(0, b.sizeBytes());
        }

        TEST_METHOD(CheckWriteByte)
        {
            BitWriter b;
            b.writeByte(4);
            b.writeByte(3);
            b.writeByte(5);
            b.writeByte(6);
            b.writeByte(7);

            uint32_t* buffer = b.getBuffer();
            Assert::AreEqual(8, b.sizeBytes());
            Assert::AreEqual(100991748u, buffer[0]);
            Assert::AreEqual(7u, buffer[1]);
        }

        TEST_METHOD(CheckSingleWrite)
        {
            BitWriter b;
            b.writeByte(4);
            uint32_t* buffer = b.getBuffer();
            Assert::AreEqual(4, b.sizeBytes());
            Assert::AreEqual(4u, buffer[0]);
        }

        TEST_METHOD(CheckWriteBits)
        {
            BitWriter b;
            b.writeBits(7, 3);
            b.writeBits(12, 10);
            b.writeBits(11873109, 24);

            uint32_t* buffer = b.getBuffer();
            Assert::AreEqual(8, b.sizeBytes());
            Assert::AreEqual(2775228519u, buffer[0]);
            Assert::AreEqual(22u, buffer[1]);
        }

    };
}