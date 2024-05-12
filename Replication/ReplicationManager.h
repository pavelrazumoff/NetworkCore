#pragma once

#include "ReflectionSystem/LinkingContext.h"
#include "Serialization/MemoryBitStream.h"

class ObjectCreationRegistry;
class IReplicationValidation;

class NETWORKCORE_API ReplicationManager
{
private:
	ReplicationManager();

public:
	~ReplicationManager();

	static ReplicationManager& GetInstance();

	void ReplicateCreate(OutputMemoryBitStream& outStream, IReplicationObject* replObject, ReplicationType clientReplType);
	bool ReplicateUpdate(OutputMemoryBitStream& outStream, IReplicationObject* replObject, ReplicationType clientReplType);
	void ReplicateDestroy(OutputMemoryBitStream& outStream, IReplicationObject* replObject);

	void CloseReplicationPackage(OutputMemoryBitStream& outStream);

	void ProcessReplicationAction(ObjectCreationRegistry* registry, InputMemoryBitStream& inStream,
		IReplicationValidation* validation = nullptr);

	uint32_t GetNetworkIdForObject(IReplicationObject* obj) const;
	bool DoesObjectSupportReplication(IReplicationObject* obj) const;
	IReplicationObject* GetObjectFromNetworkId(uint32_t networkId) const;

protected:
	friend class SceneObject;
	void AssignNetworkIdForReplicationObject(IReplicationObject* inReplObject, uint32_t inNetworkId);

private:
	LinkingContext* linkingContext = nullptr;
};
