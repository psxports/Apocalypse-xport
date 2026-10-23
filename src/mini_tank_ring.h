#ifndef MINI_TANK_RING_H
#define MINI_TANK_RING_H

#include "vector3.h"
#include "enemy.h"
#include "mesh_ring_effect.h"
#include "mini_tank.h"

class RibbonTrail;

// TODO Recover the original name for the 0x800A3CF0 effect family
class MiniTankRing : public MeshRingEffect
{
public:
    virtual ~MiniTankRing()
    {
    }

private:
    friend class MiniTank;

    sint16 age;
    uint16 lifetime;
    sint16 radius_step;
    sint16 width_step;

public:
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    MiniTankRing(const Vector3 &velocity);
};

#endif
