#ifndef RING_ATTACK_H
#define RING_ATTACK_H
#include "effect_object.h"

uint32 ClearRingAttackHitFlags(uint32 list);

// TODO Recover the original class name for 0x800A1380
class RingAttack : public EffectController
{
public:
    RingAttack(uint32 start_damage, uint32 end_damage, uint32 falloff_distance);
    virtual ~RingAttack() {}
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    void DamageObjects(uint32 list, uint32 effects);
    static RingAttack &Attach(uint32 object);
};
#endif
