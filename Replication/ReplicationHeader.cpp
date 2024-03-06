#include "pch.h"

#include "ReplicationHeader.h"
#include "NetworkUtility.h"

void ReplicationHeader::Write(OutputMemoryBitStream& outStream)
{
	outStream.Serialize(action, NetworkUtilityLibrary::GetRequiredBits<RA_MAX>());

	if (action == RA_RPC) return;

	outStream.Serialize(networkId);
	if (action == RA_Create || action == RA_Update)
		outStream.Serialize(classId);
}

void ReplicationHeader::Read(InputMemoryBitStream& inStream)
{
	inStream.Serialize(action, NetworkUtilityLibrary::GetRequiredBits<RA_MAX>());

	if (action == RA_RPC) return;

	inStream.Serialize(networkId);
	if (action == RA_Create || action == RA_Update)
		inStream.Serialize(classId);
}
