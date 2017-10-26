#include "BitReader.h"
#include <cstdint>

BitReader::BitReader(uint32_t* inputBuffer)
{
    scratch_ = 0;
    scratchBits_ = 0;
    totalBits_ = sizeof(inputBuffer) * 8;
    numBitsRead_ = 0;
    wordIndex_ = 0;
    buffer_ = inputBuffer;
}

BitReader::~BitReader()
{

}

void BitReader::readWord()
{
    scratch_ |= (buffer_[wordIndex_] << scratchBits_);
    scratchBits_ += 32;
    wordIndex_ += 1;
}

size_t BitReader::readBits(int bitcount)
{
    if (scratchBits_ - bitcount < 0)
    {
        readWord();
    }

    uint32_t mask = ~(-1 << bitcount);
    size_t bits = (scratch_ & mask);
    scratch_ >>= bitcount;
    scratchBits_ -= bitcount;
    return bits;
}

uint8_t BitReader::readByte()
{
    return (uint8_t)readBits(8);
}

uint16_t BitReader::readShort()
{
    return (uint16_t)readBits(16);
}

uint32_t BitReader::readInt()
{
    return (uint32_t)readBits(32);
}