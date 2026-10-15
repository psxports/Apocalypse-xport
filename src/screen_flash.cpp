extern "C"
{
#include "draft_first_signatures.h"
}
#include "screen_flash.h"

void ResetScreenFlash()
{
    FUNCTION_MARKER(0x8001BE54, "SLUS_003.73");
    w_u32(0x800FF1E4, 0);
    w_u32(0x800FF1EC, 0);
    w_u32(0x800FF1E8, 0);
    w_u8(0x800FF1DC, 0);
}

uint32 IsScreenFadeComplete()
{
    FUNCTION_MARKER(0x8001BE6C, "SLUS_003.73");
    return (uint32)(r_u32(0x800FF1EC) == 0);
}

uint32 SetScreenFlash(uint32 red, uint32 green, uint32 blue, uint32 duration, uint32 priority, uint32 mode)
{
    uint32 result, red_step, green_step, blue_step;
    FUNCTION_MARKER(0x8001BE78, "SLUS_003.73");
    result = (uint32)((priority & 255) < r_u8(0x800FF1DC));
    if (result || !duration)
        return result;
    red = (red & 255) << 16;
    green = (green & 255) << 16;
    blue = (blue & 255) << 16;
    red_step = red / duration;
    green_step = green / duration;
    blue_step = blue / duration;
    w_u32(0x800FFB68, blue);
    w_u32(0x800FFB60, red);
    w_u32(0x800FFB64, green);
    w_u32(0x800FF1E4, duration);
    w_u8(0x800FF1DC, priority);
    w_u32(0x800FF1E0, mode);
    w_u32(0x800FFB6C, red_step);
    w_u32(0x800FFB70, green_step);
    w_u32(0x800FFB74, blue_step);
    return mode;
}

uint32 FadeScreenToWhite(uint32 duration, uint32 mode)
{
    FUNCTION_MARKER(0x8001BEF8, "SLUS_003.73");
    uint32 step = duration ? (uint32)((sint32)0xFF0000 / (sint32)duration) : (uint32)-1;
    w_u32(0x800FF1E8, 1);
    w_u32(0x800FF1EC, duration);
    w_u8(0x800FF1DC, 255);
    w_u32(0x800FFB68, 0);
    w_u32(0x800FFB64, 0);
    w_u32(0x800FFB60, 0);
    w_u32(0x800FF1E0, mode);
    w_u32(0x800FFB6C, step);
    return 255;
}

uint32 UpdateScreenFlash()
{
    uint32 count, red, green, blue, red_step, green_step, blue_step, result;
    FUNCTION_MARKER(0x8001BF34, "SLUS_003.73");
    if (r_u32(0x800FF1E8))
    {
        count = r_u32(0x800FF1EC);
        if (!count)
            return count - 1;
        --count;
        w_u32(0x800FF1EC, count);
        if (!count)
        {
            w_u32(0x800FFB68, 0xFF0000);
            w_u32(0x800FFB64, 0xFF0000);
            w_u32(0x800FFB60, 0xFF0000);
            return 0xFF0000;
        }
        red = r_u32(0x800FFB60);
        red_step = r_u32(0x800FFB6C);
        blue = r_u32(0x800FFB68);
        w_u32(0x800FFB60, red + red_step);
        green = r_u32(0x800FFB64);
        w_u32(0x800FFB68, blue + red_step);
        result = green + red_step;
        w_u32(0x800FFB64, result);
        return result;
    }
    count = r_u32(0x800FF1E4);
    if (!count)
        return count - 1;
    --count;
    w_u32(0x800FF1E4, count);
    if (!count)
    {
        w_u8(0x800FF1DC, 0);
        return 0;
    }
    red = r_u32(0x800FFB60);
    red_step = r_u32(0x800FFB6C);
    green_step = r_u32(0x800FFB70);
    blue_step = r_u32(0x800FFB74);
    w_u32(0x800FFB60, red - red_step);
    green = r_u32(0x800FFB64);
    blue = r_u32(0x800FFB68);
    result = green - green_step;
    w_u32(0x800FFB64, result);
    w_u32(0x800FFB68, blue - blue_step);
    return result;
}

extern "C" void sub_8001BE54(void)
{
    FUNCTION_MARKER(0x8001BE54, "SLUS_003.73");
    ResetScreenFlash();
}
extern "C" uint32 sub_8001BE6C(void)
{
    FUNCTION_MARKER(0x8001BE6C, "SLUS_003.73");
    return IsScreenFadeComplete();
}
extern "C" uint32 sub_8001BE78(uint32 red, uint32 green, uint32 blue, uint32 duration, uint32 priority, uint32 mode)
{
    FUNCTION_MARKER(0x8001BE78, "SLUS_003.73");
    return SetScreenFlash(red, green, blue, duration, priority, mode);
}
extern "C" uint32 sub_8001BEF8(uint32 duration, uint32 mode)
{
    FUNCTION_MARKER(0x8001BEF8, "SLUS_003.73");
    return FadeScreenToWhite(duration, mode);
}
extern "C" uint32 sub_8001BF34(void)
{
    FUNCTION_MARKER(0x8001BF34, "SLUS_003.73");
    return UpdateScreenFlash();
}

uint32 DrawScreenFlash()
{
    FUNCTION_MARKER(0x8001C004, "SLUS_003.73");
    uint32 result = r_u32(0x800FF1E4);
    if (!result)
    {
        result = r_u32(0x800FF1E8);
        if (!result)
            return result;
    }
    uint32 depth = r_u32(0x800FF1E0);
    uint32 base = r_u32(0x800FF660);
    uint32 packet = r_u32(0x800FF668);
    uint32 entry = base + 4 * depth + 116;
    uint32 limit = r_u32(0x800FF374);
    uint32 next = packet + 32;
    result = limit < next;
    if (result)
        return result;
    uint32 mode_packet = packet + 24;
    w_u32(0x800FF668, next);
    w_u8(packet + 3, 5);
    w_u8(packet + 7, 42);
    uint32 red = r_u16(0x800FFB62);
    w_u16(packet + 8, 0);
    w_u16(packet + 10, 0);
    w_u16(packet + 14, 0);
    w_u16(packet + 16, 0);
    w_u8(packet + 4, red);
    uint32 green = r_u16(0x800FFB66);
    w_u16(packet + 12, 512);
    w_u16(packet + 20, 512);
    w_u8(packet + 5, green);
    uint32 blue = r_u16(0x800FFB6A);
    w_u16(packet + 18, 240);
    w_u16(packet + 22, 240);
    w_u8(packet + 6, blue);
    uint32 tag = r_u32(packet);
    uint32 head = r_u32(entry);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(entry);
    uint32 fading = r_u32(0x800FF1E8);
    w_u32(entry, (head & 0xFF000000) | (packet & 0xFFFFFF));
    w_u32(mode_packet, 0x1000000);
    w_u32(mode_packet + 4, fading ? 0xE1000640 : 0xE1000620);
    tag = r_u32(mode_packet);
    head = r_u32(entry);
    w_u32(mode_packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(entry);
    result = (head & 0xFF000000) | (mode_packet & 0xFFFFFF);
    w_u32(entry, result);
    return result;
}

extern "C" uint32 sub_8001C004(void)
{
    FUNCTION_MARKER(0x8001C004, "SLUS_003.73");
    return DrawScreenFlash();
}
