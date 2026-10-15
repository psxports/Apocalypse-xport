#ifndef PROJECTILE_WEAVING_H
#define PROJECTILE_WEAVING_H

#include "projectile_base.h"

// TODO Recover the original class name from resource350E968C
class WeavingProjectile : public ProjectileBase
{
public:
    WeavingProjectile(const uint32 *position, const sint16 *angles, uint32 reverse, uint32 trail_segments, uint32 speed, uint32 owner_pointer, const uint32 *additional_vector);
    static WeavingProjectile &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    // Native helper preserves adjusted callback dispatch
    uint32 CallAction(uint32 slot);
};

#endif
