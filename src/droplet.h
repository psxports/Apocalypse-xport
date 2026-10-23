#ifndef DROPLET_H
#define DROPLET_H

#include "vector3.h"
#include "sprite.h"
#include "mesh_ring_effect.h"
#include "effect_splash.h"

// Resource droplet at original address 800FF1F4
class Droplet : public Sprite
{
private:
    sint32 floor_y;
    uint8 rgb[3];

public:
    Droplet(const Vector3 *pos, uint32 angle, uint8 r, uint8 g, uint8 b);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
