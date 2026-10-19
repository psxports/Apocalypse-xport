#ifndef LEVEL_HAZARDS_H
#define LEVEL_HAZARDS_H

#include "enemy.h"

class FloatingHazard : public Enemy
{
public:
    FloatingHazard(const RecordCursor &record_data, uint32 index);

    virtual ~FloatingHazard()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 UpdateHeight();

private:
    uint16 bob_phase;
    uint32 bob_offset;

    // Native helpers share original inline explosion and adjusted finish dispatch
    void Explode();
};

class BallisticHazard : public Enemy
{
public:
    BallisticHazard(const uint32 *position, uint32 kind, uint32 flags, uint32 parameter, sint32 vertical_speed);

    virtual ~BallisticHazard()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 Update();

private:
    uint16 impact_flags;
    uint32 impact_radius;
};

#endif
