#ifndef RING_ATTACK_H
#define RING_ATTACK_H
#include "color32.h"
#include "effect_object.h"
#include "model_instance.h"

class MeshRingEffect;
class VisualObject;
class RingAttack;
extern ObjectReference<RingAttack> active_ring_attack;

// TODO Recover the original class name for 0x800A1380
class RingAttack : public EffectController
{
public:
    RingAttack(uint32 start_damage, uint32 end_damage, uint32 falloff_distance);
    virtual ~RingAttack();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    void DamageObjects(ModelInstance *list, uint32 effects);

private:
    friend void FlagObjectNearReference(VisualObject &object);
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    Color32 color;
    uint32 start_damage;
    uint32 end_damage;
    uint32 falloff_distance;
    uint32 phase[16];
    uint32 width;
};
#endif
