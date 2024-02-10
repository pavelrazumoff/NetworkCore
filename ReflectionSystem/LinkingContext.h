#pragma once

#include <cstdint>
#include <unordered_map>

#include "Replication/ReplicationObject.h"

class LinkingContext
{
public:
	uint32_t GetNetworkId(IReplicationObject* inReplObject)
	{
		auto it = std::find_if(networkIdToReplObjectMap.begin(), networkIdToReplObjectMap.end(),
			[&inReplObject](const auto& pair) -> bool {
				return pair.second == inReplObject;
			});
			
		if (it != networkIdToReplObjectMap.end())
		{
			return it->first;
		}
		else
		{
			// TODO: Check if we actually a server.

			uint32_t newNetworkId = nextNetworkId++;
			AddReplicationObject(inReplObject, newNetworkId);
			return newNetworkId;
		}
	}

	IReplicationObject* GetReplicationObject(uint32_t inNetworkId) const
	{
		auto it = networkIdToReplObjectMap.find(inNetworkId);
		return (it != networkIdToReplObjectMap.end()) ? it->second : nullptr;
	}

	void RemoveReplicationObject(IReplicationObject* inReplObject)
	{
		RemoveReplicationObject(GetNetworkId(inReplObject));
	}

	void RemoveReplicationObject(uint32_t inNetworkId)
	{
		networkIdToReplObjectMap.erase(inNetworkId);
	}

	void AddReplicationObject(IReplicationObject* inReplObject, uint32_t inNetworkId)
	{
		networkIdToReplObjectMap[inNetworkId] = inReplObject;
	}

private:
	std::unordered_map<uint32_t, IReplicationObject*> networkIdToReplObjectMap;

	uint32_t nextNetworkId = 1;
};
