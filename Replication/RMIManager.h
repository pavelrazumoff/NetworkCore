#pragma once

#include <functional>
#include <unordered_map>

#include "NetworkUtility.h"

#include "Serialization/MemoryBitStream.h"

class IReplicationObject;

template <std::size_t... Is, typename... Args>
void fill_tuple_from_stream(std::index_sequence<Is...>, std::tuple<Args...>& tup, InputMemoryBitStream& inStream) {
	((inStream.Serialize(std::get<Is>(tup)), ...));
}

class NETWORKCORE_API RMIManager
{
private:
	RMIManager();

public:
	using Method = std::function<void(IReplicationObject*, InputMemoryBitStream&)>;

	static RMIManager& GetInstance();

public:
	template <typename T, typename... Args>
	void RegisterMethod(uint32_t inRMIId, void (T::* method)(Args...))
	{
		if (rmiTable.find(inRMIId) != rmiTable.end()) return;

		rmiTable[inRMIId] = ([=](IReplicationObject* instance, InputMemoryBitStream& inStream) {

			std::tuple<Args...> args;
			fill_tuple_from_stream(std::index_sequence_for<Args...>{}, args, inStream);
			std::apply([method, instance](auto&&... args) {
				(dynamic_cast<T*>(instance)->*method)(std::forward<decltype(args)>(args)...);
			}, args);
		});
	}

	void ProcessRMI(InputMemoryBitStream& inStream, IReplicationObject* owner);

private:
	std::unordered_map<uint32_t, Method> rmiTable;
};

#define REGISTER_RMI_FUNC( rmiFunc ) \
	RMIManager::GetInstance().RegisterMethod(NetworkUtilityLibrary::StringToUint32(#rmiFunc), &rmiFunc);

#define GET_RMI_FUNC_ID( rmiFunc ) \
	NetworkUtilityLibrary::StringToUint32(#rmiFunc)
