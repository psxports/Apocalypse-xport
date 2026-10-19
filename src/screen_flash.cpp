#include "scene_lifecycle.h"
#include "game_diagnostics.h"
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
    uint32 *entry = scene_render.cur->ot + depth + 1;
    uint8 *packet = scene_render.cursor;
    uint8 *limit = scene_render.limit;
    result = limit - packet < (sint32)(sizeof(POLY_F4) + sizeof(DR_TPAGE));
    if (result)
        return result;
    uint8 *next = packet + sizeof(POLY_F4) + sizeof(DR_TPAGE);
    uint8 *mode_packet = packet + sizeof(POLY_F4);
    scene_render.cursor = next;
    POLY_F4 *poly = (POLY_F4 *)packet;
    setPolyF4(poly);
    poly->rgbc.code = 42;
    uint32 red = (screen_flash.color[0] >> 16);
    poly->p[0].x = 0;
    poly->p[0].y = 0;
    poly->p[1].y = 0;
    poly->p[2].x = 0;
    poly->rgbc.r = red;
    uint32 green = (screen_flash.color[1] >> 16);
    poly->p[1].x = 512;
    poly->p[3].x = 512;
    poly->rgbc.g = green;
    uint32 blue = (screen_flash.color[2] >> 16);
    poly->p[2].y = 240;
    poly->p[3].y = 240;
    poly->rgbc.b = blue;
    uint32 fading = screen_flash.fading;
    setaddr(poly, getaddr(entry));
    setaddr(entry, gpu_packet_link((uintptr_t)poly));
    DR_TPAGE *mode = (DR_TPAGE *)mode_packet;
    mode->tag = 0x01000000;
    mode->code[0] = fading ? 0xE1000640 : 0xE1000620;
    setaddr(mode, getaddr(entry));
    setaddr(entry, gpu_packet_link((uintptr_t)mode));
    result = *entry;
    return result;
}
