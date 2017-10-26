#pragma once

#include <cstdint>

class BitReader
{
public:
    BitReader(uint32_t* inputBuffer);
    ~BitReader();

    void flush();

    size_t readBits(int bitCount);
    uint8_t readByte();
    uint16_t readShort();
    uint32_t readInt();

    uint32_t* getBuffer();

private:
    void ensureCapacity(size_t requiredCapacity);
    void readWord();

    uint64_t scratch_;
    int scratchBits_;
    int totalBits_;

    int numBitsRead_;
    int wordIndex_;
    uint32_t* buffer_;
};