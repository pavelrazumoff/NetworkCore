#include "pch.h"

#include "ReplicationManager.h"
#include "RPCManager.h"

#include "ReplicationHeader.h"
#include "ObjectCreationRegistry.h"

#include "ReflectionSystem/DataType.h"

#include "NetworkUtility.h"

static void SerializeData(MemoryBitStream& stream, const DataType* inDataType,
	uint8_t* inData, uint64_t inProperties);

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
		ReplicationAction action = RA_Padding;
		outStream.Serialize(action);
	}
}

void ReplicationManager::ProcessReplicationAction(ObjectCreationRegistry* registry, InputMemoryBitStream& inStream)
{
	if (!registry) { DebugNetworkTrap(); return; }

	ReplicationHeader rh;
	rh.Read(inStream);

	switch (rh.action)
	{
		case RA_Create:
			{
				if (IReplicationObject* replObject =
					registry->CreateReplicationObject(rh.classId))
				{
					linkingContext->AddReplicationObject(replObject, rh.networkId);
					replObject->Read(inStream);
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

// --------------------------------------------------------------------

void SerializeData(MemoryBitStream& stream, const DataType* inDataType, uint8_t* inData, uint64_t inProperties)
{
	stream.Serialize(inProperties);

	const auto& mvs = inDataType->GetMemberVariables();
	for (int mvIndex = 0, c = mvs.size(); mvIndex < c; ++mvIndex)
	{
		if (((1 << mvIndex) & inProperties) != 0)
		{
			const auto& mv = mvs[mvIndex];
			void* mvData = inData + mv.GetOffset();

			switch (mv.GetPrimitiveType())
			{
				case EPrimitiveType::EPT_Int:
					stream.Serialize(*reinterpret_cast<int*>(mvData));
					break;
				case EPrimitiveType::EPT_Float:
					stream.Serialize(*reinterpret_cast<float*>(mvData));
					break;
				case EPrimitiveType::EPT_Bool:
					stream.Serialize(*reinterpret_cast<bool*>(mvData));
					break;
				case EPrimitiveType::EPT_String:
					stream.Serialize(*reinterpret_cast<std::string*>(mvData));
					break;
				default:
					break;
			}
		}
	}
}
