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
    sint32 priority;
};

struct ExplosionQueue
{
    ExplosionEvent entries[3];
    uint32 count;
};

extern ExplosionQueue explosion_queue;
void ResetExplosionState();
void QueueExplosion(const Vector3 *pos, uint32 radius, uint32 strength, sint32 priority);
void ResetExplosionQueue();
uint32 SpawnExplosionParticles(const Vector3 *pos, uint32 count, uint32 resource, uint16 size, uint32 spread);
void HandleExplosionProximity(const Vector3 *pos, uint32 kind);
void SpawnExplosion(const Vector3 *pos, bool large);

#endif
