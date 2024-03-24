#include "pch.h"

#include "MemoryBitStream.h"
#include "minwindef.h"

bool MemoryBitStream::Serialize(void* inData, uint32_t inBitCount)
{
	uint8_t* srcByte = static_cast<uint8_t*>(inData);

	while (inBitCount > 8)
	{
		Serialize(*srcByte, 8);
		++srcByte;
		inBitCount -= 8;
	}

	if (inBitCount > 0)
		Serialize(*srcByte, inBitCount);

	return true;
}

void MemoryBitStream::ReallocBuffer(uint32_t inNewBitCapacity)
{
	if (mBuffer == nullptr)
	{
		mBuffer = static_cast<char*>(std::malloc(inNewBitCapacity >> 3));
		if (mBuffer)
			memset(mBuffer, 0, inNewBitCapacity >> 3);
		else
		{
			// TODO: Handle error.
			DebugNetworkTrap();
		}
	}
	else
	{
		char* tempBuffer = static_cast<char*>(std::malloc(inNewBitCapacity >> 3));
		if (tempBuffer)
		{
			memset(tempBuffer, 0, inNewBitCapacity >> 3);
			memcpy(tempBuffer, mBuffer, mBitCapacity >> 3);
			std::free(mBuffer);
			mBuffer = tempBuffer;
		}
		else
		{
			// TODO: Handle error.
			DebugNetworkTrap();
		}
	}

	mBitCapacity = inNewBitCapacity;
}

// ---------------------------------------------------------------------------

bool OutputMemoryBitStream::Serialize(std::string& str)
{
	uint32_t strBitLen = (uint32_t)str.length() << 3;
	if (!Serialize(strBitLen)) return false;
	if ((strBitLen > 0) &&
		!Serialize(&str.front(), strBitLen)) return false;

	return true;
}

bool OutputMemoryBitStream::WriteBits(uint8_t inData, uint32_t inBitCount)
{
	uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
	if (nextBitHead > mBitCapacity)
	{
		ReallocBuffer(max(mBitCapacity * 2, nextBitHead));
	}

	uint32_t byteOffset = mBitHead >> 3;
	uint32_t bitOffset = mBitHead & 0x7;

	uint8_t currentMask = ~(0xff << bitOffset);
	mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) |
		(inData << bitOffset);

	uint32_t bitsFreeThisByte = 8 - bitOffset;

	if (bitsFreeThisByte < inBitCount)
	{
		mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
	}

	mBitHead = nextBitHead;

	return true;
}

// ---------------------------------------------------------------------------

bool InputMemoryBitStream::Serialize(std::string& str)
{
	uint32_t strBitLen;
	if (!Serialize(strBitLen)) return false;

	str.resize(strBitLen >> 3);
	if ((strBitLen > 0) &&
		!Serialize(&str.front(), strBitLen)) return false;

	return true;
}

bool InputMemoryBitStream::ReadBits(uint8_t& outData, uint32_t inBitCount)
{
	if (mBitHead + inBitCount > mBitCapacity) return false;

	uint32_t byteOffset = mBitHead >> 3;
	uint32_t bitOffset = mBitHead & 0x7;

	outData = static_cast<uint8_t>(mBuffer[byteOffset]) >> bitOffset;

	uint32_t bitsFreeThisByte = 8 - bitOffset;
	if (bitsFreeThisByte < inBitCount)
	{
		outData |= static_cast<uint8_t>(mBuffer[byteOffset + 1]) << bitsFreeThisByte;
	}

	outData &= (~(0x00ff << inBitCount));

	mBitHead += inBitCount;

	return true;
}

void InputMemoryBitStream::Reset(uint32_t newByteCount)
{
	mBitHead = 0;
	if (newByteCount > 0)
	{
		std::free(mBuffer); mBuffer = nullptr;
		ReallocBuffer(newByteCount << 3);
	}
}
