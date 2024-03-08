#pragma once

#include "NetworkCore.h"

#include <cstdint>
#include <type_traits>
#include <string>

#include "ByteSwap.h"
#include "NetworkUtility.h"

class NETWORKCORE_API MemoryBitStream
{
public:
	MemoryBitStream() {}
	virtual ~MemoryBitStream() { std::free(mBuffer); }

public:
	virtual bool Serialize(void* inData, uint32_t inBitCount);
	virtual bool SerializeBytes(void* inData, uint32_t inByteCount)
	{
		return Serialize(inData, inByteCount << 3);
	}
	virtual bool Serialize(uint8_t& inData, uint32_t inBitCount) = 0;

	template <typename T>
	bool Serialize(T& inData, uint32_t inBitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic<T>::value ||
			std::is_enum<T>::value,
			"Generic Write only supports primitive data types");

		if (NetworkUtilityLibrary::IsPlatformLittleEndian() || (inBitCount <= 8))
			return Serialize(&inData, inBitCount);

		if (inBitCount == sizeof(T) * 8)
		{
			if (IsInput())
			{
				if (!Serialize(&inData, inBitCount)) return false;
				inData = ByteSwap(inData);
			}
			else
			{
				T swappedData = ByteSwap(inData);
				if (!Serialize(&swappedData, inBitCount)) return false;
			}
		}
		else
		{
			DebugNetworkTrap();
			Serialize(&inData, inBitCount);
		}

		return true;
	}

	virtual bool Serialize(bool& inData)
	{
		return Serialize(&inData, 1);
	}

	virtual bool Serialize(std::string& str) = 0;

public:
	virtual bool IsInput() const = 0;

	char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetCapacity() const { return mBitCapacity >> 3; }

protected:
	void ReallocBuffer(uint32_t inNewBitCapacity);

protected:
	char* mBuffer = nullptr;
	uint32_t mBitHead = 0;
	uint32_t mBitCapacity = 0;
};

class NETWORKCORE_API OutputMemoryBitStream : public MemoryBitStream
{
public:
	OutputMemoryBitStream() 
	{
		ReallocBuffer(1500 * 8);
	}

public:
	using MemoryBitStream::Serialize;

	virtual bool Serialize(uint8_t& inData, uint32_t inBitCount) override
	{
		return WriteBits(inData, inBitCount);
	}

	virtual bool Serialize(std::string& str) override;

	bool WriteBits(uint8_t inData, uint32_t inBitCount);

public:
	virtual bool IsInput() const override { return false; }

	const char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetBitLength() const { return mBitHead; }
	uint32_t GetByteLength() const { return (mBitHead + 7) >> 3; }
};

class NETWORKCORE_API InputMemoryBitStream : public MemoryBitStream
{
public:
	InputMemoryBitStream(uint32_t maxByteCount)
	{
		ReallocBuffer(maxByteCount << 3);
	}
	InputMemoryBitStream(char* inBuffer, uint32_t inBitCount)
	{
		mBuffer = inBuffer;
		mBitCapacity = inBitCount;
		mBitHead = 0;
	}

public:
	using MemoryBitStream::Serialize;

	virtual bool Serialize(uint8_t& inData, uint32_t inBitCount) override
	{
		return ReadBits(inData, inBitCount);
	}

	virtual bool Serialize(std::string& str) override;

	bool ReadBits(uint8_t& outData, uint32_t inBitCount);

public:
	virtual bool IsInput() const override { return true; }
	uint32_t GetBitCapacityLeft() const { return mBitCapacity - mBitHead; }
	uint32_t GetByteCapacityLeft() const { return GetBitCapacityLeft() >> 3; }

	void Reset(uint32_t newByteCount = 0);
};
