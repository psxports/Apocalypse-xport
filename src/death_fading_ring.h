#ifndef DEATH_FADING_RING_H
#define DEATH_FADING_RING_H

#include "vector3.h"
#include "mesh_ring_effect.h"

class DeathFadingRing : public MeshRingEffect
{
public:
    virtual ~DeathFadingRing()
    {
    }

private:
    sint16 age;

public:
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    DeathFadingRing(const Vector3 *pos);
    static void Spawn(const Vector3 *pos);
};

#endif
