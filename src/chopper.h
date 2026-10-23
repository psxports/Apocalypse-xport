#ifndef CHOPPER_H
#define CHOPPER_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"

class MachineGun;
class Swat;

class Items;
class Tank;

class Chopper : public Enemy
{
private:
    Vector3 aim_pos;
    Vector3 aim_offset;
    ModelReference tracked_target;
    uint32 fire_timer;
    ObjectReference<MachineGun> weapon;
    uint16 hover_phase;
    uint16 sound_timer;
    uint32 rotor_sound;
    uint32 sound_distance;

public:
    Chopper(const RecordCursor &rec_data, uint16 idx);

private:
    uint16 Init();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual void SetParameters(uint16 command);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);

private:
    void UpdateState();

public:
    virtual uint32 ExecuteCommand(uint16 command);

private:
    uint32 SetDst(const Vector3 *pos);
    uint32 SetDstRec(uint16 idx);
    uint32 Hover();
    void Aim();
    uint32 SetTargetRec(uint16 idx);
    uint32 SetRelativeTarget(uint32 x, uint32 y, uint32 z);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();
    void FireProjectile();
    void SetFireDuration(uint32 seconds);
    Items *SpawnItem();
    Swat *SpawnEnemy(sint32 idx);
    Tank *SpawnTank(sint32 idx);

public:
    virtual ~Chopper();

private:
    uint16 ResolveCommandVal(uint16 val);
    void SpawnRandomExplosion(bool large);
    void SetDstRelative(sint16 x);
    void TrackHero();
    void UpdateRoll(sint16 difference);
};

#endif
