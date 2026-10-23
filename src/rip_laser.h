#ifndef RIP_LASER_H
#define RIP_LASER_H

#include "object_reference.h"
#include "vector3.h"
#include "object.h"
#include "model_instance.h"
#include "weapon_type.h"
#include "weapon_node.h"

class EffectObject;
class RingEffect;
class BeamSegments;

class RipLaser : public WeaponNode
{
private:
    ObjectReference<BeamSegments> beam[2];
    Vector3 impact_pos;
    uint32 retarget_timer;
    uint32 impact_active;
    uint32 impact_count;

    struct Color
    {
        uint8 r, g, b;
    } colors[2];

    // Native helpers retain original inline call setup

public:
    RipLaser(ModelInstance **list);
    virtual uint32 Destroy(uint32 reason);

private:
    void SetColors(uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1);
    void ClearChildren();

public:
    virtual uint32 Fire(const Vector3 &input_pos, const ShortVector3 &input_angles);
    virtual uint32 Update(const Vector3 *pos);
    virtual uint32 ReadLight(uint8 *r, uint8 *g, uint8 *b);
    virtual ~RipLaser();

private:
    void DamageTarget(ModelInstance *hit);
};

#endif
