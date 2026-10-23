#ifndef IMPACT_RING_H
#define IMPACT_RING_H

#include "vector3.h"
#include "effect_object.h"
#include "mesh_ring_effect.h"
#include "effect_splash.h"

class ImpactRing : public MeshRingEffect
{
public:
    virtual ~ImpactRing()
    {
    }

    sint16 age;
    uint16 lifetime;
    sint16 radius_step;
    sint16 width_step;

public:
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    ImpactRing(const Vector3 &velocity);
};

#endif
