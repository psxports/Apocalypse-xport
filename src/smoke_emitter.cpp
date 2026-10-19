#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "smoke_emitter.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "effect_sprite.h"

SmokeEmitter::SmokeEmitter(const uint32 *position, uint32 duration, uint32 count, uint32 red, uint32 green, uint32 blue, uint32 speed_min, uint32 speed_range, uint32 size_min, uint32 size_range) : EffectController(), age(0), duration(duration), count(count), speed_min(speed_min), speed_range(speed_range), size_min(size_min), size_range(size_range)
{
    FUNCTION_MARKER(0x800325B0, "SLUS_003.73");
    SetPosition(position);
    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;
}

uint32 SmokeEmitter::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80032694, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : result;
}

void SmokeEmitter::Update()
{
    FUNCTION_MARKER(0x800326E8, "SLUS_003.73");
    Vector3 velocity = {0};
    for (uint32 idx = 0; (sint32)idx < (sint32)count; ++idx)
    {
        uint32 random = RandomRange(speed_range);
        velocity.words[1] = (0 - (speed_min + random)) << 12;
        GameHeapBlock *allocation = AllocateEffectStorage(112);
        MovingSprite *sprite = 0;
        if (allocation)
        {
            uint32 size = size_min + RandomRange(size_range);
            sprite = new (allocation) MovingSprite(pos.words, velocity.words, "Smoke", size, 0, 10);
        }
        if (!sprite)
        {
            GameDiagnostic("SmokeEmitter sprite allocation failed\n");
            abort();
        }
        sprite->depth_bias = 100;
        sprite->angle = RandomRange(4096);
        sprite->SetColors(rgb[0], rgb[1], rgb[2]);
    }
    ++age;
    if ((sint32)(sint16)age == (sint32)duration && age != 65535)
        Finish();
}
