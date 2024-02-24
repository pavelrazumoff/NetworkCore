#pragma once

#include <cstdint>

#include "Serialization/MemoryBitStream.h"

enum ReplicationAction
{
	RA_Create,
	RA_Update,
	RA_Destroy,

	RA_RPC,

	RA_Padding,
	RA_MAX,
};

class NETWORKCORE_API ReplicationHeader
{
public:
	ReplicationHeader() {}
	ReplicationHeader(ReplicationAction _action, uint32_t _networkId = 0, uint32_t _classId = 0) :
		action(_action), networkId(_networkId), classId(_classId) {}

	void Write(OutputMemoryBitStream& outStream);
	void Read(InputMemoryBitStream& inStream);

public:
	ReplicationAction action = ReplicationAction::RA_Create;
	uint32_t networkId = 0;
	uint32_t classId = 0;
};
