#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"

#include "game_diagnostics.h"

#include "fireball_effects.h"
#include "game_random.h"
#include "character.h"
#include "model_attachment.h"
#include "effect_lifecycle.h"

Fireball::Fireball(Character &source, sint16 bone) : Sprite(), source(&source)
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
    GetModelBonePosition(pos.words, source, point);
    pos.words[1] -= 0x40000;
}

uint32 Fireball::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8004B51C, "SLUS_003.73");
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void Fireball::Update()
{
    FUNCTION_MARKER(0x8004B570, "SLUS_003.73");
    ++age;
    if ((sint16)age >= 9 && !(game_clock.tick & 3) && !IsGameHeapUnderPressure())
    {
        use_small_effect_pool = false;
        GameHeapBlock *allocation = AllocateEffectStorage(116);
        if (allocation)
        {
            new (allocation) Smoke(pos.words);
        }
        use_small_effect_pool = true;
    }
    if (!source || (source->contact_flags & 64))
    {
        Finish();
        return;
    }
    pos.words[1] -= (uint32)rise_speed << 12;
    if (intensity < 8)
    {
        Finish();
        return;
    }
    intensity -= ((uint8)game_clock.tick & 1) + 4;
    SetColors(128, intensity, intensity);
}

Smoke::Smoke(const uint32 *position) : Sprite()
{
    uint32 x, y, z, random;
    FUNCTION_MARKER(0x8004B68C, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->pos.words[0] = x;
    this->pos.words[1] = y;
    this->pos.words[2] = z;
    SelectResource("Smoke");
    EnableSemiTransparency();
    blend_mode = 64;
    random = RandomRange(3);
    this->frame = random;
    SetColors(32, 32, 32);
    this->frame_step = 85;
    random = RandomRange(3);
    rise_speed = random + 7;
    this->size = 2048;
}

uint32 Smoke::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8004B744, "SLUS_003.73");
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void Smoke::Update()
{
    FUNCTION_MARKER(0x8004B798, "SLUS_003.73");
    ++age;
    pos.words[1] -= (uint32)rise_speed << 12;
    AdvanceFrame();
    if (frame >= frame_count)
        Finish();
}
