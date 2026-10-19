#ifndef TANK_H
#define TANK_H

#include "vector3.h"
#include "enemy.h"
#include "angle_math.h"

class Flamethrower;

class Tank : public Enemy
{
public:
    Tank(const RecordCursor &record_data, uint32 index);
    virtual ~Tank();
    Tank(const uint32 *position, const RecordCursor &script);
    uint32 Init();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 FireProjectile();
    void FireBeam(uint32 fire);
    uint32 FireDebris();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    void UpdateState();
    uint32 SetMode(uint32 mode);
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 Aim(uint32 enabled);
    uint32 SetTargetRecord(uint32 index);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();
    uint32 FragmentAndFinish();

private:
    uint8 fire_mask;
    Vector3 aim_pos;
    Vector3 aim_offset;
    AngleMotion joints[2];
    uint8 debris_first;
    uint8 debris_count;
    uint32 move_sound;
    ModelReference tracked_target;
    ObjectReference<Flamethrower> weapon;
    void ClearVelocity();
    void ClearAngularVelocity();
    // Native extraction of the original adjusted slot16 call
    void SetState(uint32 state);
    static uint32 TraceDivide(uint32 numerator, uint32 denominator);
};

#endif
