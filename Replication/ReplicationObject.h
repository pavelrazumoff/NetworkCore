#pragma once

class OutputMemoryBitStream;
class InputMemoryBitStream;

class IReplicationObject
{
public:
	virtual uint32_t GetClassId() const = 0;

	virtual void Write(OutputMemoryBitStream& outStream) = 0;
	virtual void Read(InputMemoryBitStream& inStream) = 0;

	virtual void SafeDestroy() = 0;
};
