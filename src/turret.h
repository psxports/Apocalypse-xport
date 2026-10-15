#ifndef TURRET_H
#define TURRET_H
#include "enemy.h"

// Resource variants share the existing turret guest object methods
class Turret : public Enemy
{
public:
    Turret(const uint8 *record_data, uint32 index);
    static Turret &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void ResetWeapon();
    virtual uint32 Update();
    uint32 DetachAndDestroy();
    uint32 AttachOperator(uint32 operator_object);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 ExecuteCommand(uint32 command);

private:
    uint32 SetMode(uint32 mode);
    uint32 Init();
    void Fire(uint32 fire);
    uint32 UpdateState();
    uint32 Aim();
    uint32 SetTargetRecord(uint32 index);
    uint32 SetTargetObject(uint32 target);
    uint32 ClearTarget();
};

#endif
