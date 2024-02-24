#pragma once

#include <unordered_map>

#include "NetworkUtility.h"

class InputMemoryBitStream;

typedef void (*RPCFunction)(InputMemoryBitStream&);

class NETWORKCORE_API RPCManager
{
public:
	RPCManager();

	static RPCManager& GetInstance();

public:
	void RegisterUnwrapFunction(uint32_t inRPCId, RPCFunction inFunction);
	void ProcessRPC(InputMemoryBitStream& inStream);

private:
	std::unordered_map<uint32_t, RPCFunction> rpcTable;
};

#define REGISTER_RPC_FUNC( rpcFunc ) \
	RPCManager::GetInstance().RegisterUnwrapFunction(NetworkUtilityLibrary::StringToUint32(#rpcFunc), rpcFunc);

#define GET_RPC_FUNC_ID( rpcFunc ) \
	NetworkUtilityLibrary::StringToUint32(#rpcFunc)
