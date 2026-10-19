#include "game_diagnostics.h"
#include "effect_color.h"

uint32 FadePackedColor(uint32 *color, sint32 amount)
{
    FUNCTION_MARKER(0x80032D3C, "SLUS_003.73");
    uint32 packed = *color;
    uint32 red = packed & 255;
    uint32 green = packed >> 8;
    uint32 blue = packed >> 16;
    red = (sint32)red < amount ? 0 : red - (uint32)amount;
    green = (sint32)(green & 255) < amount ? 0 : green - (uint32)amount;
    blue = (sint32)(blue & 255) < amount ? 0 : blue - (uint32)amount;
    uint32 result = red & 255;
    *color = (*color & 0xFF000000) | ((blue & 255) << 16) | ((green & 255) << 8) | result;
    return result;
}
