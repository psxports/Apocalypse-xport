#ifndef PLAGUE_GROUND_EFFECT_H
#define PLAGUE_GROUND_EFFECT_H

#include "object_reference.h"
#include "vector3.h"
#include "color32.h"
#include "effect_object.h"
#include "effect_controller.h"

class MeshRingEffect;

class PlagueGroundEffect : public EffectController
{
private:
    sint16 age;
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    Color32 color;
    uint32 seg_count;
    uint32 damage;
    uint32 width;
    uint32 radial_speed;
    bool hit_player;

public:
    PlagueGroundEffect(const Vector3 *src_pos, uint32 damage, uint32 r, uint32 g, uint32 b, uint32 segs, uint32 radial_speed);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~PlagueGroundEffect();
};

#endif
