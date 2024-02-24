#pragma once

#include "ReflectionSystem/LinkingContext.h"
#include "Serialization/MemoryBitStream.h"

class ObjectCreationRegistry;

class NETWORKCORE_API ReplicationManager
{
private:
	ReplicationManager();

public:
	~ReplicationManager();

	static ReplicationManager& GetInstance();

	void ReplicateCreate(OutputMemoryBitStream& outStream, IReplicationObject* replObject);
	void ReplicateUpdate(OutputMemoryBitStream& outStream, IReplicationObject* replObject);
	void ReplicateDestroy(OutputMemoryBitStream& outStream, IReplicationObject* replObject);

	void CloseReplicationPackage(OutputMemoryBitStream& outStream);

	void ProcessReplicationAction(ObjectCreationRegistry* registry, InputMemoryBitStream& inStream);

	uint32_t GetNetworkIdForObject(IReplicationObject* obj) const;
	IReplicationObject* GetObjectFromNetworkId(uint32_t networkId) const;

private:
	LinkingContext* linkingContext = nullptr;
};
