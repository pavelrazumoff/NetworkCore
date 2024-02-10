#pragma once

#include "ReflectionSystem/LinkingContext.h"
#include "Serialization/MemoryBitStream.h"

class ReplicationManager
{
private:
	ReplicationManager();

public:
	~ReplicationManager();

	static ReplicationManager& GetInstance();

	void ReplicateCreate(OutputMemoryBitStream& outStream, IReplicationObject* replObject);
	void ReplicateUpdate(OutputMemoryBitStream& outStream, IReplicationObject* replObject);
	void ReplicateDestroy(OutputMemoryBitStream& outStream, IReplicationObject* replObject);

	void ProcessReplicationAction(InputMemoryBitStream& inStream);

private:
	LinkingContext* linkingContext = nullptr;
};
