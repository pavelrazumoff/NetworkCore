#include "pch.h"

#include "ReplicationManager.h"
#include "RPCManager.h"
#include "RMIManager.h"

#include "ReplicationHeader.h"
#include "ObjectCreationRegistry.h"

#include "ReflectionSystem/DataType.h"
#include "ReplicationValidation.h"

#include "NetworkUtility.h"

ReplicationManager::ReplicationManager()
{
	linkingContext = new LinkingContext();
}

ReplicationManager::~ReplicationManager()
{
	delete linkingContext;
}

ReplicationManager& ReplicationManager::GetInstance()
{
	static ReplicationManager instance;
	return instance;
}

void ReplicationManager::ReplicateCreate(OutputMemoryBitStream& outStream, IReplicationObject* replObject)
{
	ReplicationHeader rh(RA_Create,
		linkingContext->GetNetworkId(replObject),
		replObject->GetClassId());

	rh.Write(outStream);
	replObject->Write(outStream);
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& outStream, IReplicationObject* replObject)
{
	ReplicationHeader rh(RA_Update,
		linkingContext->GetNetworkId(replObject),
		replObject->GetClassId());

	rh.Write(outStream);
	replObject->Write(outStream);
}

void ReplicationManager::ReplicateDestroy(OutputMemoryBitStream& outStream, IReplicationObject* replObject)
{
	ReplicationHeader rh(RA_Destroy,
		linkingContext->GetNetworkId(replObject),
		replObject->GetClassId());

	rh.Write(outStream);
}

void ReplicationManager::CloseReplicationPackage(OutputMemoryBitStream& outStream)
{
	if (outStream.GetBitLength() % 8 != 0)
	{
		ReplicationHeader rh(RA_Padding);
		rh.Write(outStream);
	}
}

void ReplicationManager::ProcessReplicationAction(ObjectCreationRegistry* registry, InputMemoryBitStream& inStream,
	IReplicationValidation* validation)
{
	if (!registry) { DebugNetworkTrap(); return; }

	ReplicationHeader rh;
	rh.Read(inStream);

	// Check rh.networkId to actually belong to the instigator, that sent the package.
	if (rh.action != RA_Padding)
	{
		if (validation &&
			!validation->ValidateObjectOwnership(rh.networkId))
		{
			DebugNetworkTrap();
			return;
		}
	}

	switch (rh.action)
	{
		case RA_Create:
			{
				if (IReplicationObject* replObject =
					registry->CreateReplicationObject(rh.classId))
				{
					linkingContext->AddReplicationObject(replObject, rh.networkId);
					replObject->Read(inStream);

					replObject->PostReplCreate();
				}
				else DebugNetworkTrap();
			}
			break;
		case RA_Update:
			{
				if (IReplicationObject* replObject =
					linkingContext->GetReplicationObject(rh.networkId))
				{
					replObject->Read(inStream);
				}
				else
				{
					replObject = registry->CreateReplicationObject(rh.classId);

					if (replObject)
					{
						replObject->Read(inStream);
						replObject->SafeDestroy();
					}
					else DebugNetworkTrap();
				}
			}
			break;
		case RA_Destroy:
			{
				if (IReplicationObject* replObject =
					linkingContext->GetReplicationObject(rh.networkId))
				{
					linkingContext->RemoveReplicationObject(replObject);
					replObject->SafeDestroy();
				}
			}
			break;
		case RA_RPC:
			{
				RPCManager::GetInstance().ProcessRPC(inStream);
			}
			break;
		case RA_RMI:
			{
				if (IReplicationObject* replMethodOwner =
					linkingContext->GetReplicationObject(rh.networkId))
				{
					RMIManager::GetInstance().ProcessRMI(inStream, replMethodOwner);
				}
			}
			break;
		case RA_Padding:
			while (inStream.GetBitCapacityLeft() > 0)
			{
				bool paddingBit;
				inStream.Serialize(paddingBit);
			}
			break;
		default:
			break;
	}
}

uint32_t ReplicationManager::GetNetworkIdForObject(IReplicationObject* obj) const
{
	return linkingContext->GetNetworkId(obj);
}

IReplicationObject* ReplicationManager::GetObjectFromNetworkId(uint32_t networkId) const
{
	return linkingContext->GetReplicationObject(networkId);
}
