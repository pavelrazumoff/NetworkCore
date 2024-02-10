#include "pch.h"

#include "NetworkUtility.h"

#include <cstdint>

bool NetworkUtilityLibrary::IsPlatformLittleEndian()
{
	uint16_t number = 0x1;
	char* numPtr = reinterpret_cast<char*>(&number);
	return (numPtr[0] == 1);
}
