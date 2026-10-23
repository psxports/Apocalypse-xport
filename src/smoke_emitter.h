#ifndef SMOKE_EMITTER_H
#define SMOKE_EMITTER_H

#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"

// Context name follows the original Smoke resource
class SmokeEmitter : public EffectController
{
public:
    virtual ~SmokeEmitter()
    {
    }

private:
    sint16 age;
    uint16 duration;
    sint32 count;
    uint8 rgb[3];
    uint32 speed_min;
    uint32 speed_range;
    uint32 size_min;
    uint32 size_range;

public:
    SmokeEmitter(const Vector3 *pos, uint16 duration, sint32 count, uint8 r, uint8 g, uint8 b, uint32 speed_min, uint32 speed_range, uint32 size_min, uint32 size_range);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
