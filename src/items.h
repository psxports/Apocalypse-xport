#ifndef ITEMS_H
#define ITEMS_H
#include "visual_object.h"

class OscillatingRing;

// Resource owner for the original items model
class Items : public VisualObject
{
public:
    virtual ~Items();
    Items(const RecordCursor &data, uint32 index);
    Items(const uint32 *pos, uint32 damage, uint32 arm_radius, uint32 blast_radius, uint32 fuse_ticks, uint32 life_ticks, uint32 hover_height);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 reason);
    virtual uint32 Update();

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

    uint32 hover_y;
    uint32 fuse_timer;
    uint8 settled;
    uint16 bob_phase;
    Configuration cfg;
    ObjectReference<OscillatingRing> ring;
};

#endif
