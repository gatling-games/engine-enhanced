#pragma once

#include <cstdint>

class BitReader
{
public:
    BitReader(uint32_t* inputBuffer);
    ~BitReader();

    size_t readBits(int bitCount);
    uint8_t readByte();
    uint16_t readShort();
    uint32_t readInt();

private:
    void readWord();

    uint64_t scratch_;
    int scratchBits_;
    int totalBits_;

    int numBitsRead_;
    int wordIndex_;
    uint32_t* buffer_;
};