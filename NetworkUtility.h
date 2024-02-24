#pragma once

#include "NetworkCore.h"

#include "stdint.h"

class NETWORKCORE_API NetworkUtilityLibrary
{
public:
	template <int tValue>
	static constexpr int GetRequiredBits()
	{
		return (tValue > 1) ? 1 + GetRequiredBits<(tValue >> 1)>() : 1;
	}

	static bool IsPlatformLittleEndian();

	static constexpr uint32_t StringToUint32(const char* str, int h = 0)
	{
		return !str[h] ? 5381 : (StringToUint32(str, h + 1) * 33) ^ str[h];
	}
};

#if _DEBUG
#define DebugNetworkTrap() __debugbreak()
#else // _DEBUG
#define DebugNetworkTrap() void(0)
#endif // _DEBUG

inline void network_assert(bool condition)
{
	if (!condition) DebugNetworkTrap();
}
