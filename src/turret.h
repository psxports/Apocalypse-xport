#ifndef TURRET_H
#define TURRET_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"
#include "angle_math.h"

class Swat;
class Flamethrower;

// Resource variants share turret behavior and joint aiming
class Turret : public Enemy
{
private:
    uint16 aim_bones[2];
    Vector3 aim_pos;
    Vector3 aim_offset;
    AngleMotion joint;
    ObjectReference<Swat> operator_object;
    ModelReference tracked_target;
    ObjectReference<Flamethrower> weapon;

public:
    Turret(const RecordCursor &rec_data, uint16 idx);

private:
    uint16 SetMode(uint16 mode);
    uint32 Init();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 ReleaseResources();
    virtual uint32 Update();
    bool AttachOperator(Swat *operator_actor);
    uint32 DetachAndDestroy();

private:
    void Fire(bool fire);

public:
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);

private:
    void UpdateState();

public:
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void Aim();
    uint32 SetTargetRec(uint16 idx);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();

public:
    virtual ~Turret();
};

#endif
