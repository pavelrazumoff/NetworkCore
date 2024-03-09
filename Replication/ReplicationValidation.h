#pragma once

#include <stdint.h>

class IReplicationValidation
{
public:
	virtual bool ValidateObjectOwnership(uint32_t networkId) const = 0;
};
