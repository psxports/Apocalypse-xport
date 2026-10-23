#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "smoke_emitter.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "moving_sprite.h"
#include "effect_object.h"
#include "effect_controller.h"

SmokeEmitter::SmokeEmitter(const Vector3 *pos, uint16 duration, sint32 count, uint8 r, uint8 g, uint8 b, uint32 speed_min, uint32 speed_range, uint32 size_min, uint32 size_range) : EffectController(), age(0), duration(duration), count(count), speed_min(speed_min), speed_range(speed_range), size_min(size_min), size_range(size_range)
{
    FUNCTION_MARKER(0x800325B0, "SLUS_003.73");
    SetPos(pos);
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

uint32 SmokeEmitter::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80032694, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : result;
}

void SmokeEmitter::Update()
{
    FUNCTION_MARKER(0x800326E8, "SLUS_003.73");
    Vector3 velocity = {0};
    for (sint32 idx = 0; idx < count; ++idx)
    {
        uint32 random = RandomRange(speed_range);
        velocity.y = -((speed_min + random) << 12);
        GameHeapBlock *allocation = EffectObject::AllocateStorage(112);
        MovingSprite *spr = 0;
        if (allocation)
        {
            uint32 size = size_min + RandomRange(size_range);
            spr = new (allocation) MovingSprite(&pos, &velocity, "Smoke", size, 0, 10);
        }
        if (!spr)
        {
            GameDiagnostic(("SmokeEmitter sprite allocation failed\n"));
            abort();
        }
        spr->depth_bias = 100;
        spr->angle = RandomRange(4096);
        spr->SetColors(rgb[0], rgb[1], rgb[2]);
    }
    ++age;
    if (age == duration && age != -1)
        Finish();
}
