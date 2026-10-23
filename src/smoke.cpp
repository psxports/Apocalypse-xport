#include <new>

#include "game_heap.h"
#include "game_tick.h"
#include "smoke.h"
#include "global.h"
#include "character.h"
#include "model_attachment.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "sprite.h"

Smoke::Smoke(const Vector3 *pos) : Sprite()
{
    FUNCTION_MARKER(0x8004B68C, "SLUS_003.73");
    this->pos = *pos;
    SelectResource("Smoke");
    EnableSemiTransparency();
    blend_mode = 64;
    frame = RandomRange(3);
    SetColors(32, 32, 32);
    frame_step = 85;
    rise_speed = RandomRange(3) + 7;
    size = 2048;
}

uint32 Smoke::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8004B744, "SLUS_003.73");
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void Smoke::Update()
{
    FUNCTION_MARKER(0x8004B798, "SLUS_003.73");
    ++age;
    pos.y = (uint32)pos.y - (rise_speed * ONE);
    AdvanceFrame();
    if (frame >= frame_count)
        Finish();
}
