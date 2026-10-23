#ifndef BALLISTIC_HAZARD_H
#define BALLISTIC_HAZARD_H

#include "vector3.h"
#include "enemy.h"

class BallisticHazard : public Enemy
{
public:
    virtual ~BallisticHazard()
    {
    }

private:
    uint16 impact_flags;
    uint32 impact_radius;

public:
    BallisticHazard(const Vector3 *pos, uint32 kind, uint16 flags, uint32 radius, sint32 vertical_accel);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);
    virtual uint32 Update();
};

#endif
