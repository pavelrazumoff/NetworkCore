#pragma once

class OutputMemoryBitStream;
class InputMemoryBitStream;

class IReplicationObject
{
public:
	virtual uint32_t GetClassId() const = 0;

	virtual void PostReplCreate() {}

	virtual void Write(OutputMemoryBitStream& outStream) = 0;
	virtual void WriteCreate(OutputMemoryBitStream& outStream) = 0;
	virtual void Read(InputMemoryBitStream& inStream) = 0;

	virtual void ReplDestroy() = 0;
	virtual bool DoesSupportDestroyReplication() { return true; }
};
