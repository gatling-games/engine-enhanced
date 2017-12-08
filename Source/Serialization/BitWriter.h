#pragma once

#include <cstdint>

class BitWriter
{
public:
    BitWriter();
    ~BitWriter();

    void clear();
    void flush();
    int sizeBytes() const;

    void writeBits(uint32_t value, int bitCount);
    void writeByte(uint8_t byteValue);
    void writeShort(uint16_t byteValue);
    void writeInt(uint32_t byteValue);

    uint32_t* getBuffer();

private:
    void ensureCapacity(size_t requiredCapacity);
    void checkOverflow();

    size_t capacityWords_;
    uint64_t scratch_;
    int scratchBits_;
    int wordIndex_;
    uint32_t* buffer_;
};