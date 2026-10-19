#ifndef PLAGUE_GROUND_EFFECTS_H
#define PLAGUE_GROUND_EFFECTS_H

#include "color32.h"
#include "effect_object.h"

class MeshRingEffect;

// TODO Recover the original names for the two Plague ground effect owners
class PlagueImpactEffect : public EffectController
{
public:
    PlagueImpactEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue);
    virtual ~PlagueImpactEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    Color32 color;
    uint32 width;
};

class PlagueGroundEffect : public EffectController
{
public:
    PlagueGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed);
    virtual ~PlagueGroundEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    Color32 color;
    uint32 segment_count;
    uint32 damage;
    uint32 width;
    uint32 radial_speed;
    uint32 hit_player;
};

#endif
