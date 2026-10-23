#ifndef PLAGUE_IMPACT_EFFECT_H
#define PLAGUE_IMPACT_EFFECT_H

#include "object_reference.h"
#include "vector3.h"
#include "color32.h"
#include "effect_object.h"
#include "effect_controller.h"

class MeshRingEffect;

// Ground rings and the secondary ring created on player impact
class PlagueImpactEffect : public EffectController
{
private:
    sint16 age;
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    Color32 color;
    uint32 width;

public:
    PlagueImpactEffect(const Vector3 *src_pos, uint32 r, uint32 g, uint32 b);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~PlagueImpactEffect();
};

#endif
