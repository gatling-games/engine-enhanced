#pragma once

#include <cstdint>

class BitReader
{
public:
	BitReader(uint32_t* buffer);
	~BitReader();

	void flush();

	size_t readBits(int bitCount);
	uint8_t readByte();
	uint16_t readShort();
	uint32_t readInt();

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