#ifndef PICKUP_FACTORY_H
#define PICKUP_FACTORY_H
extern "C"
{
#include "psx.h"
}

class Pickup;

Pickup *SpawnPickup(uint32 resource, const uint32 *position, uint32 flags, uint32 lifetime, const uint32 *velocity);
#endif
