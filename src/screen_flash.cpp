extern "C"
{
#include "draft_first_signatures.h"
}
#include "screen_flash.h"

struct ScreenFlash
{
    uint8 priority;
    uint32 depth;
    uint32 flash_ticks;
    uint32 fading;
    uint32 fade_ticks;
    uint32 color[3];
    uint32 step[3];
};

static ScreenFlash screen_flash = {0};

void InitializeScreenFlash()
{
    const ScreenFlash empty = {0};
    screen_flash = empty;
}

void ResetScreenFlash()
{
    FUNCTION_MARKER(0x8001BE54, "SLUS_003.73");
    screen_flash.flash_ticks = 0;
    screen_flash.fade_ticks = 0;
    screen_flash.fading = 0;
    screen_flash.priority = 0;
}

uint32 IsScreenFadeComplete()
{
    FUNCTION_MARKER(0x8001BE6C, "SLUS_003.73");
    return (uint32)(screen_flash.fade_ticks == 0);
}

uint32 SetScreenFlash(uint32 red, uint32 green, uint32 blue, uint32 duration, uint32 priority, uint32 mode)
{
    uint32 result, red_step, green_step, blue_step;
    FUNCTION_MARKER(0x8001BE78, "SLUS_003.73");
    result = (uint32)((priority & 255) < screen_flash.priority);
    if (result || !duration)
        return result;
    red = (red & 255) << 16;
    green = (green & 255) << 16;
    blue = (blue & 255) << 16;
    red_step = red / duration;
    green_step = green / duration;
    blue_step = blue / duration;
    screen_flash.color[2] = blue;
    screen_flash.color[0] = red;
    screen_flash.color[1] = green;
    screen_flash.flash_ticks = duration;
    screen_flash.priority = priority;
    screen_flash.depth = mode;
    screen_flash.step[0] = red_step;
    screen_flash.step[1] = green_step;
    screen_flash.step[2] = blue_step;
    return mode;
}

uint32 FadeScreenToWhite(uint32 duration, uint32 mode)
{
    FUNCTION_MARKER(0x8001BEF8, "SLUS_003.73");
    uint32 step = duration ? (uint32)((sint32)0xFF0000 / (sint32)duration) : (uint32)-1;
    screen_flash.fading = 1;
    screen_flash.fade_ticks = duration;
    screen_flash.priority = 255;
    screen_flash.color[2] = 0;
    screen_flash.color[1] = 0;
    screen_flash.color[0] = 0;
    screen_flash.depth = mode;
    screen_flash.step[0] = step;
    return 255;
}

uint32 UpdateScreenFlash()
{
    uint32 count, red, green, blue, red_step, green_step, blue_step, result;
    FUNCTION_MARKER(0x8001BF34, "SLUS_003.73");
    if (screen_flash.fading)
    {
        count = screen_flash.fade_ticks;
        if (!count)
            return count - 1;
        --count;
        screen_flash.fade_ticks = count;
        if (!count)
        {
            screen_flash.color[2] = 0xFF0000;
            screen_flash.color[1] = 0xFF0000;
            screen_flash.color[0] = 0xFF0000;
            return 0xFF0000;
        }
        red = screen_flash.color[0];
        red_step = screen_flash.step[0];
        blue = screen_flash.color[2];
        screen_flash.color[0] = red + red_step;
        green = screen_flash.color[1];
        screen_flash.color[2] = blue + red_step;
        result = green + red_step;
        screen_flash.color[1] = result;
        return result;
    }
    count = screen_flash.flash_ticks;
    if (!count)
        return count - 1;
    --count;
    screen_flash.flash_ticks = count;
    if (!count)
    {
        screen_flash.priority = 0;
        return 0;
    }
    red = screen_flash.color[0];
    red_step = screen_flash.step[0];
    green_step = screen_flash.step[1];
    blue_step = screen_flash.step[2];
    screen_flash.color[0] = red - red_step;
    green = screen_flash.color[1];
    blue = screen_flash.color[2];
    result = green - green_step;
    screen_flash.color[1] = result;
    screen_flash.color[2] = blue - blue_step;
    return result;
}

uint32 DrawScreenFlash()
{
    FUNCTION_MARKER(0x8001C004, "SLUS_003.73");
    uint32 result = screen_flash.flash_ticks;
    if (!result)
    {
        result = screen_flash.fading;
        if (!result)
            return result;
    }
    uint32 depth = screen_flash.depth;
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
    uint32 red = (screen_flash.color[0] >> 16);
    w_u16(packet + 8, 0);
    w_u16(packet + 10, 0);
    w_u16(packet + 14, 0);
    w_u16(packet + 16, 0);
    w_u8(packet + 4, red);
    uint32 green = (screen_flash.color[1] >> 16);
    w_u16(packet + 12, 512);
    w_u16(packet + 20, 512);
    w_u8(packet + 5, green);
    uint32 blue = (screen_flash.color[2] >> 16);
    w_u16(packet + 18, 240);
    w_u16(packet + 22, 240);
    w_u8(packet + 6, blue);
    uint32 tag = r_u32(packet);
    uint32 head = r_u32(entry);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(entry);
    uint32 fading = screen_flash.fading;
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

