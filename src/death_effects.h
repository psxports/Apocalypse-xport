#ifndef DEATH_EFFECTS_H
#define DEATH_EFFECTS_H

#include "mesh_ring_effect.h"

// TODO Recover the original resource name for key CCE0187C
class DeathPersistentRing : public MeshRingEffect
{
public:
    DeathPersistentRing(const uint32 *position);

    virtual ~DeathPersistentRing()
    {
    }

    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    uint16 age;
};

class DeathFadingRing : public MeshRingEffect
{
public:
    DeathFadingRing(const uint32 *position);

    virtual ~DeathFadingRing()
    {
    }

    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    uint16 age;
};

void SpawnDeathPersistentRing(const uint32 *position);
void SpawnDeathFadingRing(const uint32 *position);

#endif
