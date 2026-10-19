#ifndef BEAST_EFFECTS_H
#define BEAST_EFFECTS_H

#include "vector3.h"

#include "color32.h"
#include "effect_object.h"

class MeshRingEffect;

class RibbonTrail;

class BeastOrbitEffect : public EffectController
{
public:
    BeastOrbitEffect(const uint32 *position);
    virtual ~BeastOrbitEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    Vector3 center;
    ObjectReference<RibbonTrail> trail;
    uint16 angle;
    uint32 radius;
    uint32 angle_step;
};

class BeastGroundEffect : public EffectController
{
public:
    BeastGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed, uint32 lifetime);
    virtual ~BeastGroundEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    Color32 color;
    uint32 lifetime;
    uint32 damage;
    uint32 width;
    uint32 radial_speed;
    uint32 hit_player;
};

#endif
