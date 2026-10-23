#ifndef EXPLOSION_RING_H
#define EXPLOSION_RING_H

#include "mesh_ring_effect.h"

// TODO Recover the original name for the 0x800A15F8 effect family
class ExplosionRing : public MeshRingEffect
{
public:
    virtual ~ExplosionRing()
    {
    }

private:
    friend class ProjectileBase;
    sint16 age;
    uint16 lifetime;
    sint16 radius_step;
    sint16 width_step;

public:
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    ExplosionRing();
};

#endif
