#ifndef TURRET_H
#define TURRET_H

#include "vector3.h"
#include "enemy.h"
#include "angle_math.h"

class Swat;
class Flamethrower;

// Resource variants share turret behavior and joint aiming
class Turret : public Enemy
{
public:
    Turret(const RecordCursor &record_data, uint32 index);
    virtual ~Turret();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 ReleaseResources();
    virtual uint32 Update();
    uint32 DetachAndDestroy();
    uint32 AttachOperator(Swat *operator_object);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 ExecuteCommand(uint32 command);

private:
    uint16 aim_bones[2];
    Vector3 aim_pos;
    Vector3 aim_offset;
    AngleMotion joint;
    ObjectReference<Swat> operator_object;
    ModelReference tracked_target;
    ObjectReference<Flamethrower> weapon;
    uint32 SetMode(uint32 mode);
    uint32 Init();
    void Fire(uint32 fire);
    uint32 UpdateState();
    void Aim();
    uint32 SetTargetRecord(uint32 index);
    uint32 SetTargetObject(ModelInstance *target);
    uint32 ClearTarget();
};

#endif
