#ifndef EXPLOSION_EFFECTS_H
#define EXPLOSION_EFFECTS_H

#include "vector3.h"

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

struct ExplosionEvent
{
    Vector3 pos;
    uint32 radius;
    uint32 strength;
    uint32 priority;
};

struct ExplosionQueue
{
    ExplosionEvent entries[3];
    uint32 count;
};

extern ExplosionQueue explosion_queue;
void ResetExplosionState();
void QueueExplosion(const uint32 *position, uint32 radius, uint32 strength, uint32 priority);
void ResetExplosionQueue();
uint32 SpawnExplosionParticles(const uint32 *position, uint32 count, uint32 resource, uint32 size, uint32 spread);
uint32 HandleExplosionProximity(const uint32 *position, uint32 kind);
void SpawnExplosion(const uint32 *position, uint32 large);

#endif
