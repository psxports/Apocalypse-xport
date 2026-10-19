#ifndef BOUNCING_POINT_EFFECT_H
#define BOUNCING_POINT_EFFECT_H

#include "vector3.h"
#include "point_segments.h"

// TODO Recover the original name for the 0x800A1518 effect family
class BouncingPointEffect : public PointSegments
{
public:
    BouncingPointEffect(const uint32 *position, const uint16 *angles);

    virtual ~BouncingPointEffect()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    Vector3 dir;
    uint32 floor_y;
    uint32 spawn_tick;
};
#endif
