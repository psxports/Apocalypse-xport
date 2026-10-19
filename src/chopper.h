#ifndef CHOPPER_H
#define CHOPPER_H

#include "vector3.h"
#include "enemy.h"

class MachineGun;
class Swat;

class Items;
class Tank;

class Chopper : public Enemy
{
public:
    Chopper(const RecordCursor &record_data, uint32 index);
    virtual ~Chopper();
    uint32 Init();
    virtual uint32 Destroy(uint32 reason);
    virtual void SetParameters(uint32 command);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    void UpdateState();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 SetDestinationRecord(uint32 index);
    uint32 Hover();
    uint32 Aim();
    uint32 SetTargetRecord(uint32 index);
    uint32 SetRelativeTarget(uint32 x, uint32 y, uint32 z);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();
    uint32 FireProjectile();
    uint32 SetFireDuration(uint32 seconds);
    Items *SpawnDebris();
    Swat *SpawnEnemy(uint32 index);
    Tank *SpawnTank(uint32 index);

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
    uint32 ResolveCommandValue(uint32 value);
    // Native extraction of the original adjusted slot16 call
    void SpawnRandomEffect(uint32 kind, uint32 debris);
    void SetDestinationRelative(uint32 x);
    void TrackHero();
    static uint32 SquaredDistance(const Vector3 &vector);
    void UpdateRoll(uint32 difference);
    static uint32 TraceDivide(uint32 numerator, uint32 denominator);
};

#endif
