#ifndef MINI_TANK_H
#define MINI_TANK_H
#include "enemy.h"
#include "mesh_ring_effect.h"

// TODO Recover the original name for the 0x800A3CF0 effect family
class MiniTankRing : public MeshRingEffect
{
public:
    MiniTankRing(const uint32 acceleration[3]);
    virtual ~MiniTankRing() {}
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    static MiniTankRing &Attach(uint32 object);
};

class MiniTank : public Enemy
{
public:
    MiniTank(const uint8 *record_data, uint32 index);
    static MiniTank &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Aim();
    void UpdateState();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    uint32 SetTarget(const uint32 *target);
    uint32 Fire();

private:
    uint32 Init();
    void Burst(uint32 green);
    // Native extraction of the original adjusted slot16 call
    void FinishFromTable();
    static uint32 Divide(uint32 numerator, uint32 denominator);
    static uint32 Remainder(uint32 numerator, uint32 denominator);
};

#endif
