#ifndef BEAST_ORBIT_EFFECT_H
#define BEAST_ORBIT_EFFECT_H

#include "object_reference.h"
#include "vector3.h"
#include "color32.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "ribbon_trail.h"

class MeshRingEffect;
class RibbonTrail;

class BeastOrbitEffect : public EffectController
{
private:
    sint16 age;
    Vector3 center;
    ObjectReference<RibbonTrail> trail;
    uint16 angle;
    sint32 radius;
    sint16 angle_step;

public:
    BeastOrbitEffect(const Vector3 *src_pos);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~BeastOrbitEffect();
};

#endif
