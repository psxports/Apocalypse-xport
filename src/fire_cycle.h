#ifndef FIRE_CYCLE_H
#define FIRE_CYCLE_H

#include "effect_object.h"

class AnimatedSprite;

class FireCycle : public EffectController
{
public:
    FireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count);
    virtual ~FireCycle();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    void SpawnSprite(uint32 idx);
    uint16 age;
    uint16 lifetime;
    uint16 spawn_age;
    uint8 damage;
    uint8 enabled;
    ObjectReference<AnimatedSprite> sprites[20];
};

FireCycle *SpawnFireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count);

#endif
