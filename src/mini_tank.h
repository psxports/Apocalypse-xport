#ifndef MINI_TANK_H
#define MINI_TANK_H
#include "enemy.h"
#include "mesh_ring_effect.h"

class RibbonTrail;

// TODO Recover the original name for the 0x800A3CF0 effect family
class MiniTankRing : public MeshRingEffect
{
public:
    MiniTankRing(const uint32 acceleration[3]);

    virtual ~MiniTankRing()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint16 age;
    uint16 lifetime;
    sint16 radius_step;
    sint16 width_step;
};

class MiniTank : public Enemy
{
public:
    MiniTank(const RecordCursor &record_data, uint32 index);
    virtual ~MiniTank();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Aim();
    void UpdateState();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    uint32 SetTarget(const uint32 *target);
    uint32 Fire();

private:
    // Transient trail is released by effect lifecycle after the death sequence
    ObjectReference<RibbonTrail> trail;
    ShortVector3 target_angles;
    uint32 state_timer;
    uint32 engine_sound;
    uint32 sound_period;
    uint32 bounce_speed;
    uint32 age;
    uint32 Init();
    void Burst(uint32 green);
    // Native extraction of the original adjusted slot16 call
    static uint32 Divide(uint32 numerator, uint32 denominator);
    static uint32 Remainder(uint32 numerator, uint32 denominator);
};

#endif
