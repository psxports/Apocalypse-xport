#ifndef EXPLOSION_EFFECTS_H
#define EXPLOSION_EFFECTS_H

extern "C"
{
#include "psx.h"
}

uint32 QueueExplosion(const uint32 *position, uint32 mode, uint32 radius, uint32 strength, uint32 priority);
void ResetExplosionQueue();
// TODO Recover native object-list pointer contracts
uint32 MarkExplosionObjects(uint32 object);
uint32 SpawnExplosionParticles(const uint32 *position, uint32 count, uint32 resource, uint32 size, uint32 spread);
uint32 HandleExplosionProximity(const uint32 *position, uint32 kind);
uint32 SpawnExplosion(const uint32 *position, uint32 large);

#endif
