#ifndef HOVR_SWAT_H
#define HOVR_SWAT_H
#include "enemy.h"

class HovrSwat : public Enemy
{
public:
    static HovrSwat &Attach(uint32 object);
    HovrSwat(const uint8 *record_data, uint32 index);
    uint32 Init();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    uint32 Fragment();
    uint32 InitEffects();
    uint32 DestroyEffects();
    uint32 UpdateEffects();
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 SetTargetObject(uint32 target);
    uint32 ClearTarget();
    uint32 Fire();
    uint32 UpdateState();

};

#endif
