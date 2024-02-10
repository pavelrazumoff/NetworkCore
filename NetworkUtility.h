#pragma once

#include "NetworkCore.h"

class NETWORKCORE_API NetworkUtilityLibrary
{
public:
	template <int tValue>
	static constexpr int GetRequiredBits()
	{
		return (tValue > 1) ? 1 + GetRequiredBits<(tValue >> 1)>() : 1;
	}

	static bool IsPlatformLittleEndian();
};

#if _DEBUG
#define DebugNetworkTrap() __debugbreak()
#else // _DEBUG
#define DebugNetworkTrap() void(0)
#endif // _DEBUG
