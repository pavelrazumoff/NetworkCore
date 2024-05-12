#pragma once

class OutputMemoryBitStream;
class InputMemoryBitStream;

enum class ReplicationType
{
	All = 0,
	Owner,
	NotOwner
};

class IReplicationObject
{
public:
	virtual bool IsReplicationEnabled() const { return false; }
	virtual uint32_t GetClassId() const = 0;

	virtual void PostReplCreate() {}

	virtual void WriteCreate(OutputMemoryBitStream& outStream, ReplicationType clientReplType) = 0;
	virtual bool WriteUpdate(OutputMemoryBitStream& outStream, ReplicationType clientReplType) = 0;
	virtual void Read(InputMemoryBitStream& inStream) = 0;

	virtual void ReplDestroy() = 0;
	virtual bool DoesSupportDestroyReplication() { return true; }

	virtual uint64_t GetReplicationPropertyMask() const { return 0; }
};
