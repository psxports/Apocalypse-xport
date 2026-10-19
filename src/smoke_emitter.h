#ifndef SMOKE_EMITTER_H
#define SMOKE_EMITTER_H

#include "effect_object.h"

// Context name follows the original Smoke resource
class SmokeEmitter : public EffectController
{
public:
    SmokeEmitter(const uint32 *position, uint32 duration, uint32 count, uint32 red, uint32 green, uint32 blue, uint32 speed_min, uint32 speed_range, uint32 size_min, uint32 size_range);

    virtual ~SmokeEmitter()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    uint16 duration;
    uint32 count;
    uint8 rgb[3];
    uint32 speed_min;
    uint32 speed_range;
    uint32 size_min;
    uint32 size_range;
};

#endif
