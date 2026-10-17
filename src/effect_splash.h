#ifndef EFFECT_SPLASH_H
#define EFFECT_SPLASH_H

extern "C"
{
#include "psx.h"
}

#include "effect_sprite.h"
#include "mesh_ring_effect.h"

uint32 CreateSplash(const uint32 *position);

// Resource droplet at original address 800FF1F4
class Droplet : public Sprite
{
public:
    Droplet(const uint32 *position, uint32 angle, uint32 red, uint32 green, uint32 blue);
    static Droplet &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

};

class ImpactRing : public MeshRingEffect
{
public:
    ImpactRing(const uint32 velocity[3]);
    virtual ~ImpactRing() {}
    static ImpactRing &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

};

#endif
