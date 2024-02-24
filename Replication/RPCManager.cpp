#include "pch.h"

#include "RPCManager.h"

#include "Serialization/MemoryBitStream.h"

RPCManager::RPCManager()
{
}

RPCManager& RPCManager::GetInstance()
{
	static RPCManager instance;
	return instance;
}

void RPCManager::RegisterUnwrapFunction(uint32_t inRPCId, RPCFunction inFunction)
{
	network_assert(rpcTable.find(inRPCId) == rpcTable.end());

	rpcTable[inRPCId] = inFunction;
}

void RPCManager::ProcessRPC(InputMemoryBitStream& inStream)
{
	uint32_t rpcId;
	inStream.Serialize(rpcId);

	auto it = rpcTable.find(rpcId);
	if (it != rpcTable.end())
		it->second(inStream);
	else
		DebugNetworkTrap();
}
