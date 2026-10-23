#ifndef ITEMS_H
#define ITEMS_H

#include "object_reference.h"
#include "vector3.h"
#include "visual_object.h"

class OscillatingRing;

// Resource owner for the original items model
class Items : public VisualObject
{
private:
    enum ConfigIndex
    {
        DamageAmount,
        ArmRadius,
        BlastRadius,
        FuseTicks,
        LifeTicks,
        HoverHeight
    };

    struct Configuration
    {
        uint16 damage;
        uint16 arm_radius;
        uint16 blast_radius;
        uint16 fuse_ticks;
        uint16 life_ticks;
        uint16 hover_height;
    };

    sint32 hover_y;
    uint32 fuse_timer;
    bool settled;
    uint16 bob_phase;
    Configuration cfg;
    ObjectReference<OscillatingRing> ring;

public:
    Items(const RecordCursor &data, uint16 idx);
    Items(const Vector3 *pos, uint16 damage, uint16 arm_radius, uint16 blast_radius, uint16 fuse_ticks, uint16 life_ticks, uint16 hover_height);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 reason);
    virtual uint32 Update();
    virtual ~Items();
};

#endif
