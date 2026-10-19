#ifndef WAR_EFFECTS_H
#define WAR_EFFECTS_H

#include "effect_object.h"

class MeshRingEffect;

class WarImpact : public EffectController
{
public:
    WarImpact(const uint32 *position);
    virtual ~WarImpact();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    ObjectReference<MeshRingEffect> ring;
    uint32 radius;
    uint32 phase;
};

#endif
