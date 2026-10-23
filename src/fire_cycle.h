#ifndef FIRE_CYCLE_H
#define FIRE_CYCLE_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "animated_sprite.h"

class AnimatedSprite;

class FireCycle : public EffectController
{
private:
    sint16 age;
    uint16 lifetime;
    uint16 spawn_age;
    uint8 damage;
    bool enabled;
    ObjectReference<AnimatedSprite> sprs[20];

public:
    FireCycle(const Vector3 *pos, uint16 lifetime, uint8 enabled, uint8 damage);
    virtual uint32 Destroy(uint32 reason);

private:
    void SpawnSpr(uint32 idx);

public:
    virtual void Update();
    static FireCycle *Spawn(const Vector3 *pos, uint16 lifetime, uint8 enabled, uint8 damage);
    virtual ~FireCycle();
};

#endif
