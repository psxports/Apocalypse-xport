#ifndef TANK_H
#define TANK_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"
#include "angle_math.h"

class Flamethrower;

class Tank : public Enemy
{
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
    // Native helper for repeated state and phase stores

public:
    Tank(const RecordCursor &rec_data, uint16 idx);
    Tank(const Vector3 *pos_vector, const RecordCursor &script);

private:
    uint32 Init();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    void FireProjectile();
    void FireBeam(bool fire);
    void SpawnItem();

public:
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);

private:
    void UpdateState();
    uint32 SetMode(uint16 mode);

public:
    virtual uint32 ExecuteCommand(uint16 command);

private:
    uint32 SetDst(const Vector3 *pos);
    void Aim(uint8 enabled);
    uint32 SetTargetRec(uint16 idx);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();
    uint32 FragmentAndFinish();

public:
    virtual ~Tank();

private:
    void ClearVelocity();
    void ClearAngularVelocity();
    void SetState(uint16 state);
};

#endif
