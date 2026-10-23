#include "sprite.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "textured_effect.h"

Sprite::Sprite() : TexturedEffect(), age(0), lifetime(0), angle(0), angle_speed(0), angle_accel(0), forced_depth(0), blend_mode(32), clut(0)
{
    FUNCTION_MARKER(0x80034D88, "SLUS_003.73");
    AddToList(effect_lists.sprs);
    scale[0] = ONE;
    scale[1] = ONE;
}

uint32 Sprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80034DE8, "SLUS_003.73");
    RemoveFromList(effect_lists.sprs);
    uint32 result = TexturedEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}
