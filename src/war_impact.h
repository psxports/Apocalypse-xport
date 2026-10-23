#ifndef WAR_IMPACT_H
#define WAR_IMPACT_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"

class MeshRingEffect;

class WarImpact : public EffectController
{
private:
    sint16 age;
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    uint16 phase;

public:
    WarImpact(const Vector3 *src_pos);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~WarImpact();
};

#endif
