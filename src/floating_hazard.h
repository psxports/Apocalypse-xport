#ifndef FLOATING_HAZARD_H
#define FLOATING_HAZARD_H

#include "vector3.h"
#include "enemy.h"

class FloatingHazard : public Enemy
{
public:
    virtual ~FloatingHazard()
    {
    }

private:
    uint16 bob_phase;
    sint32 bob_offset;

    // Native helpers share original inline explosion and adjusted finish dispatch

public:
    FloatingHazard(const RecordCursor &rec_data, uint16 idx);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);
    virtual uint32 Update();

private:
    void UpdateWaterHeight();

public:
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void Explode();
};

#endif
