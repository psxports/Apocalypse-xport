#ifndef MINI_TANK_H
#define MINI_TANK_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"
#include "mesh_ring_effect.h"
#include "ribbon_trail.h"

class RibbonTrail;

class MiniTank : public Enemy
{
private:
    // Transient trail is released by effect lifecycle after the death sequence
    ObjectReference<RibbonTrail> trail;
    ShortVector3 target_angles;
    uint32 state_timer;
    uint32 engine_sound;
    uint32 sound_period;
    sint32 bounce_speed;
    uint32 age;

public:
    MiniTank(const RecordCursor &rec_data, uint16 idx);
    virtual uint32 Destroy(uint32 reason);

private:
    uint32 Init();

public:
    virtual uint32 Update();

private:
    uint16 Aim();
    void UpdateState();

public:
    virtual uint32 Damage(uint32 damage, const Vector3 *input_vector, uint32 kind);

private:
    uint32 SetTarget(const Vector3 *target);
    uint32 Fire();

public:
    virtual ~MiniTank();

private:
    void Burst(uint8 g);
};

#endif
