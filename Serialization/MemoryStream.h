#pragma once

#include "NetworkCore.h"

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <vector>
#include <string>

#include "ByteSwap.h"
#include "NetworkUtility.h"

class NETWORKCORE_API MemoryStream
{
public:
	MemoryStream() {}
	virtual ~MemoryStream() { std::free(mBuffer); }

	virtual bool Serialize(void* ioData, uint32_t inByteCount) = 0;

	template <typename T>
	bool Serialize(T& ioData)
	{
		static_assert(std::is_arithmetic<T>::value ||
			std::is_enum<T>::value,
			"Generic Serialize only supports primitive data types");

		if (NetworkUtilityLibrary::IsPlatformLittleEndian())
			return Serialize(&ioData, sizeof(ioData));

		if (IsInput())
		{
			if (!Serialize(&ioData, sizeof(ioData))) return false;
			ioData = ByteSwap(ioData);
		}
		else
		{
			T swappedData = ByteSwap(ioData);
			if (!Serialize(&swappedData, sizeof(swappedData))) return false;
		}

		return true;
	}

	virtual bool Serialize(std::string& ioStr) = 0;

	template <typename T>
	bool Serialize(std::vector<T>& ioVector)
	{
		if (IsInput())
		{
			size_t elementCount;
			if (!Serialize(elementCount)) return false;

			ioVector.resize(elementCount);
			for (T& element : ioVector)
				if (!Serialize(element)) return false;
		}
		else
		{
			size_t elementCount = ioVector.size();
			if (!Serialize(elementCount)) return false;

			for (T& element : ioVector)
				if (!Serialize(element)) return false;
		}
		return true;
	}

	virtual bool IsInput() const = 0;

public:
	bool ReallocBuffer(uint32_t inNewLength);

protected:
	char* mBuffer = nullptr;
	uint32_t mHead = 0;
	uint32_t mCapacity = 0;
};

class NETWORKCORE_API OutputMemoryStream : public MemoryStream
{
public:
	OutputMemoryStream()
	{
		ReallocBuffer(32);
	}

public:
	using MemoryStream::Serialize; // Force compiler to unhide some Serialize base class implementations that were hidden by inheritance and not reimplemented here.

	virtual bool Serialize(void* ioData, uint32_t inByteCount) override
	{
		return Write(ioData, inByteCount);
	}
	virtual bool Serialize(std::string& ioStr) override;

	virtual bool IsInput() const override { return false; }

public:
	bool Write(const void* inData, size_t inByteCount);

	const char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetLength() const { return mHead; }
};

class NETWORKCORE_API InputMemoryStream : public MemoryStream
{
public:
	InputMemoryStream() = delete;

	InputMemoryStream(char* inBuffer, uint32_t inByteCount)
	{
		mBuffer = inBuffer;
		mCapacity = inByteCount;
		mHead = 0;
	}
	InputMemoryStream(uint32_t maxByteCount)
	{
		ReallocBuffer(maxByteCount);
		mHead = 0;
	}

	char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetHandledDataSize() const { return mHead; } /** How many bytes did we already read. */
	uint32_t GetCapacity() const { return mCapacity; }

public:
	using MemoryStream::Serialize; // Force compiler to unhide some Serialize base class implementations that were hidden by inheritance and not reimplemented here.

	virtual bool Serialize(void* ioData, uint32_t inByteCount) override
	{
		return Read(ioData, inByteCount);
	}
	virtual bool Serialize(std::string& ioStr) override;

	virtual bool IsInput() const override { return true; }

public:
	bool Read(void* outData, uint32_t outByteCount);

	void Reset();
};
