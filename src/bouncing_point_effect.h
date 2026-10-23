#ifndef BOUNCING_POINT_EFFECT_H
#define BOUNCING_POINT_EFFECT_H

#include "vector3.h"
#include "point_segments.h"

// Short line particle with floor bounce and randomized deflection
class BouncingPointEffect : public PointSegments
{
public:
    virtual ~BouncingPointEffect()
    {
    }

private:
    Vector3 dir;
    sint32 floor_y;
    uint32 spawn_tick;

public:
    BouncingPointEffect(const Vector3 *pos, const ShortVector3 *angles);
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
};
#endif
