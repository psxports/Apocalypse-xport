#ifndef RING_ATTACK_H
#define RING_ATTACK_H

#include "object_reference.h"
#include "color32.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "model_instance.h"
#include "visual_object.h"

class MeshRingEffect;
class RingAttack;
extern ObjectReference<RingAttack> active_ring_attack;

// TODO Recover the original class name for 0x800A1380
class RingAttack : public EffectController
{
private:
    friend void VisualObject::FlagNearRingAttack();

private:
    ObjectReference<MeshRingEffect> ring;
    sint32 radius;
    Color32 color;
    uint32 start_damage;
    uint32 end_damage;
    sint32 falloff_distance;
    uint16 phase[16];
    sint32 width;

public:
    RingAttack(uint32 start_damage, uint32 end_damage, sint32 falloff_distance);
    virtual uint32 Destroy(uint32 reason);

private:
    void ClearHitFlags(ModelInstance *list);
    void DamageObjects(ModelInstance *list, bool effects);

public:
    virtual void Update();
    virtual ~RingAttack();
};
#endif
