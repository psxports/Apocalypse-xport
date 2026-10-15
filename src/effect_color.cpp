extern "C"
{
#include "draft_first_signatures.h"
}
#include "effect_color.h"
#include "native_object_storage.h"
#include <string.h>

uint32 FadePackedColor(uint32 *color, sint32 amount)
{
    uint32 mapped, packed, red, green, blue, result;
    FUNCTION_MARKER(0x80032D3C, "SLUS_003.73");
    mapped = GuestMemoryAddress(color);
    if (mapped)
    {
        packed = r_u32(mapped);
        red = r_u8(mapped);
    }
    else
    {
        memcpy(&packed, color, 4);
        red = *(const uint8 *)color;
    }
    green = packed >> 8;
    blue = packed >> 16;
    red = (sint32)red < amount ? 0 : red - (uint32)amount;
    green = (sint32)(green & 255) < amount ? 0 : green - (uint32)amount;
    blue = (sint32)(blue & 255) < amount ? 0 : blue - (uint32)amount;
    if (mapped)
        packed = r_u32(mapped);
    else
        memcpy(&packed, color, 4);
    result = red & 255;
    packed = (packed & 0xFF000000) | ((blue & 255) << 16) | ((green & 255) << 8) | result;
    if (mapped)
        w_u32(mapped, packed);
    else
        memcpy(color, &packed, 4);
    return result;
}

extern "C" uint32 sub_80032D3C(uint32 color, uint32 amount)
{
    FUNCTION_MARKER(0x80032D3C, "SLUS_003.73");
    return FadePackedColor((uint32 *)psx_addr(color, 4), (sint32)amount);
}
