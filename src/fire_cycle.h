#ifndef FIRE_CYCLE_H
#define FIRE_CYCLE_H

#include "effect_object.h"

class FireCycle : public EffectController
{
public:
    FireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count);
    virtual ~FireCycle() {}
    static FireCycle &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 SpawnSprite(uint32 index);
};

uint32 SpawnFireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count);

#endif
