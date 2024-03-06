#include "pch.h"

#include "RMIManager.h"

#include "ReplicationObject.h"
#include "Serialization/MemoryBitStream.h"

RMIManager::RMIManager()
{
}

RMIManager& RMIManager::GetInstance()
{
	static RMIManager instance;
	return instance;
}

void RMIManager::ProcessRMI(InputMemoryBitStream& inStream, IReplicationObject* owner)
{
	uint32_t rmiId;
	inStream.Serialize(rmiId);

	auto it = rmiTable.find(rmiId);
	if (it != rmiTable.end())
	{
		it->second(owner, inStream);
	}
	else
		DebugNetworkTrap();
}
