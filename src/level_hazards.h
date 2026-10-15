#ifndef LEVEL_HAZARDS_H
#define LEVEL_HAZARDS_H

#include "enemy.h"

class FloatingHazard : public Enemy
{
public:
    FloatingHazard(const uint8 *record_data, uint32 index);
    virtual ~FloatingHazard() {}
    static FloatingHazard &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 UpdateHeight();

private:
    // Native helpers share original inline explosion and adjusted finish dispatch
    void FinishFromTable();
    void Explode();
};

class BallisticHazard : public Enemy
{
public:
    BallisticHazard(const uint32 *position, uint32 kind, uint32 flags, uint32 parameter, sint32 vertical_speed);
    virtual ~BallisticHazard() {}
    static BallisticHazard &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 Update();

private:
    // Native helper retains the original adjusted finish call
    uint32 FinishFromTable();
};

#endif
