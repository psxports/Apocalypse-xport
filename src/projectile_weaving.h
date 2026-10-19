#ifndef PROJECTILE_WEAVING_H
#define PROJECTILE_WEAVING_H

#include "projectile_base.h"

// TODO Recover the original class name from resource350E968C
struct WeavingState
{
    Vector3 axis[2];
    Vector3 offset[2];
    uint32 phase[2];
    uint32 trail_segments;

    WeavingState() : trail_segments(0)
    {
        for (uint32 i = 0; i < 2; ++i)
        {
            phase[i] = 0;
            for (uint32 j = 0; j < 3; ++j)
            {
                axis[i].words[j] = 0;
                offset[i].words[j] = 0;
            }
        }
    }
};

class WeavingProjectile : public ProjectileBase
{
public:
    WeavingProjectile(const uint32 *position, const sint16 *angles, uint32 reverse, uint32 trail_segments, uint32 speed, ModelInstance **owner_pointer, const uint32 *additional_vector);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    WeavingState weaving;
};

#endif
