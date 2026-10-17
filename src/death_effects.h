#ifndef DEATH_EFFECTS_H
#define DEATH_EFFECTS_H

#include "mesh_ring_effect.h"

// TODO Recover the original resource name for key CCE0187C
class DeathPersistentRing : public MeshRingEffect
{
public:
    DeathPersistentRing(const uint32 *position);
    virtual ~DeathPersistentRing() {}
    virtual uint32 Update();
    virtual uint32 Destroy(uint32 reason);
    static DeathPersistentRing &Attach(uint32 object);
};

class DeathFadingRing : public MeshRingEffect
{
public:
    DeathFadingRing(const uint32 *position);
    virtual ~DeathFadingRing() {}
    virtual uint32 Update();
    virtual uint32 Destroy(uint32 reason);
    static DeathFadingRing &Attach(uint32 object);
};

void SpawnDeathPersistentRing(const uint32 *position);
void SpawnDeathFadingRing(const uint32 *position);

#endif
