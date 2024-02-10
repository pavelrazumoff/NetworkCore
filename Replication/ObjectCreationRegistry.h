#pragma once

#include "ReplicationObject.h"

#include <unordered_map>

typedef IReplicationObject* (*ReplicationObjectCreationFunc)();

class ObjectCreationRegistry
{
private:
	ObjectCreationRegistry() {}

public:
	static ObjectCreationRegistry& GetInstance()
	{
		static ObjectCreationRegistry instance;
		return instance;
	}

	template <class T>
	void RegisterCreationFunction()
	{
		_ASSERT(objClassIdToCreationFuncMap.find(T::kClassId) ==
			objClassIdToCreationFuncMap.end());

		objClassIdToCreationFuncMap[T::kClassId] = T::CreateReplicationInstance;
	}

	IReplicationObject* CreateReplicationObject(uint32_t inClassId)
	{
		auto it = objClassIdToCreationFuncMap.find(inClassId);
		if (it != objClassIdToCreationFuncMap.end())
		{
			auto creationFunc = it->second;
			return creationFunc();
		}

		return nullptr;
	}

private:
	std::unordered_map<uint32_t, ReplicationObjectCreationFunc> objClassIdToCreationFuncMap;
};
