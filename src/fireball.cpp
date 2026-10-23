#include <new>

#include "game_heap.h"
#include "game_tick.h"
#include "fireball.h"
#include "smoke.h"
#include "global.h"
#include "character.h"
#include "model_attachment.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "sprite.h"

Fireball::Fireball(Character &src, sint16 bone) : Sprite(), src(&src)
{
    FUNCTION_MARKER(0x8004B428, "SLUS_003.73");
    SelectResource("Fireball");
    EnableSemiTransparency();
    frame = RandomRange(5);
    intensity = RandomRange(64) + 64;
    SetColors(128, intensity, intensity);
    rise_speed = RandomRange(2) + 5;
    size = RandomRange(512) + 1200;
    const ModelAttachmentPoint point = {0, 0, 0, (uint16)bone};
    GetModelBonePos(pos, src, point);
    pos.y = (uint32)pos.y - (0x40000);
}

uint32 Fireball::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8004B51C, "SLUS_003.73");
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void Fireball::Update()
{
    FUNCTION_MARKER(0x8004B570, "SLUS_003.73");
    ++age;
    if (age >= 9 && !(game_clock.tick & 3) && !IsGameHeapUnderPressure())
    {
        use_small_effect_pool = false;
        GameHeapBlock *allocation = EffectObject::AllocateStorage(116);
        if (allocation)
        {
            new (allocation) Smoke(&pos);
        }
        use_small_effect_pool = true;
    }
    if (!src || (src->contact_flags & 64))
    {
        Finish();
        return;
    }
    pos.y = (uint32)pos.y - (rise_speed * ONE);
    if (intensity < 8)
    {
        Finish();
        return;
    }
    intensity -= (game_clock.tick & 1) + 4;
    SetColors(128, intensity, intensity);
}
