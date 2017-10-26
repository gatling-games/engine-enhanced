#include "BitWriter.h"
#include <cstring>

BitWriter::BitWriter()
{
	capacityWords_ = 1;
	scratch_ = 0;
	scratchBits_ = 0;
	wordIndex_ = 0;
	buffer_ = new uint32_t[capacityWords_];
}

BitWriter::~BitWriter()
{
    delete[] buffer_;
}

int BitWriter::sizeBytes() const
{
	return ((scratchBits_ > 0) ? wordIndex_ + 1 : wordIndex_) * 4;
}

void BitWriter::clear()
{
    wordIndex_ = 0;
}

uint32_t* BitWriter::getBuffer()
{
	flush();
	return buffer_;
}

void BitWriter::ensureCapacity(size_t requiredCapacity)
{
	// Check to see if buffer is large enough
    if (capacityWords_ < requiredCapacity)
    {
		//double array capacity until sufficient
        size_t newCapacity = capacityWords_;
		do
		{
			newCapacity *= 2;
		} while (newCapacity < requiredCapacity);

		//Copy old data into new buffer
        uint32_t* newBuffer = new uint32_t[newCapacity];
        std::memcpy(newBuffer, buffer_, capacityWords_ * sizeof(uint32_t));
        delete[] buffer_;
        buffer_ = newBuffer;
        capacityWords_ = newCapacity;
    }
}

void BitWriter::checkOverflow()
{
	if (scratchBits_ >= 32)
	{
		// Adds 32 bits of scratch to buffer
		ensureCapacity(wordIndex_ + 1);
		buffer_[wordIndex_] = (uint32_t)scratch_;
		wordIndex_++;
		scratch_ >>= 32;
		scratchBits_ -= 32;
	}
}

void BitWriter::flush()
{
	if (scratchBits_ > 0)
	{	
		// Clear all remaining bits to buffer
		ensureCapacity(wordIndex_ + 1);
		buffer_[wordIndex_] = (uint32_t)scratch_;
		wordIndex_++;
		scratch_ = 0;
		scratchBits_ = 0;
	}
}

void BitWriter::writeBits(uint32_t value, int bitCount)
{
	//bitwise or the scratch to add new data
	scratch_ |= ((uint64_t)value << scratchBits_);
    scratchBits_ += bitCount;
	checkOverflow();
}

void BitWriter::writeByte(uint8_t byteValue)
{
	writeBits(byteValue, 8);
}

void BitWriter::writeShort(uint16_t shortValue)
{
	writeBits(shortValue, 16);
}

void BitWriter::writeInt(uint32_t intValue)
{
	writeBits(intValue, 32);
}