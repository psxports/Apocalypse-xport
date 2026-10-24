#include "scene_lifecycle.h"
#include "global.h"
#include "hd.h"
#include "screen_flash.h"

struct FlashColor
{
    uint32 r, g, b;
};

struct ScreenFlash
{
    uint8 priority;
    uint32 depth;
    uint32 flash_ticks;
    bool fading;
    uint32 fade_ticks;
    FlashColor color;
    FlashColor step;
};

static ScreenFlash scr_flash = {0};

void InitializeScrFlash()
{
    const ScreenFlash empty = {0};
    scr_flash = empty;
}

void ResetScrFlash()
{
    FUNCTION_MARKER(0x8001BE54, "SLUS_003.73");
    scr_flash.flash_ticks = 0;
    scr_flash.fade_ticks = 0;
    scr_flash.fading = false;
    scr_flash.priority = 0;
}

uint32 IsScrFadeComplete()
{
    FUNCTION_MARKER(0x8001BE6C, "SLUS_003.73");
    return scr_flash.fade_ticks == 0;
}

uint32 SetScrFlash(uint8 r, uint8 g, uint8 b, uint32 duration, uint8 priority, uint32 mode)
{
    FUNCTION_MARKER(0x8001BE78, "SLUS_003.73");
    bool rejected = priority < scr_flash.priority;
    if (rejected || !duration)
        return rejected;
    FlashColor color;
    color.r = r << 16;
    color.g = g << 16;
    color.b = b << 16;
    FlashColor step = {color.r / duration, color.g / duration, color.b / duration};
    scr_flash.color.b = color.b;
    scr_flash.color.r = color.r;
    scr_flash.color.g = color.g;
    scr_flash.flash_ticks = duration;
    scr_flash.priority = priority;
    scr_flash.depth = mode;
    scr_flash.step.r = step.r;
    scr_flash.step.g = step.g;
    scr_flash.step.b = step.b;
    return mode;
}

uint32 FadeScrToWhite(uint32 duration, uint32 mode)
{
    FUNCTION_MARKER(0x8001BEF8, "SLUS_003.73");
    uint32 step = DivideSignedWord(0xFF0000, duration);
    scr_flash.fading = true;
    scr_flash.fade_ticks = duration;
    scr_flash.priority = 255;
    scr_flash.color.b = 0;
    scr_flash.color.g = 0;
    scr_flash.color.r = 0;
    scr_flash.depth = mode;
    scr_flash.step.r = step;
    return 255;
}

uint32 UpdateScrFlash()
{
    uint32 count;
    FUNCTION_MARKER(0x8001BF34, "SLUS_003.73");
    if (scr_flash.fading)
    {
        count = scr_flash.fade_ticks;
        if (!count)
            return count - 1;
        --count;
        scr_flash.fade_ticks = count;
        if (!count)
        {
            scr_flash.color.b = 0xFF0000;
            scr_flash.color.g = 0xFF0000;
            scr_flash.color.r = 0xFF0000;
            return 0xFF0000;
        }
        scr_flash.color.r += scr_flash.step.r;
        scr_flash.color.b += scr_flash.step.r;
        scr_flash.color.g += scr_flash.step.r;
        return scr_flash.color.g;
    }
    count = scr_flash.flash_ticks;
    if (!count)
        return count - 1;
    --count;
    scr_flash.flash_ticks = count;
    if (!count)
    {
        scr_flash.priority = 0;
        return 0;
    }
    scr_flash.color.r -= scr_flash.step.r;
    scr_flash.color.g -= scr_flash.step.g;
    scr_flash.color.b -= scr_flash.step.b;
    return scr_flash.color.g;
}

uint32 DrawScrFlash()
{
    FUNCTION_MARKER(0x8001C004, "SLUS_003.73");
    if (!scr_flash.flash_ticks && !scr_flash.fading)
        return 0;
    if (hd_ui_collecting())
    {
        HD_UI_QUAD quad;
        uint32 color = (scr_flash.color.r >> 16) | ((scr_flash.color.g >> 16) << 8) | ((scr_flash.color.b >> 16) << 16);
        hd_ui_image(&quad, 0, color, HD_SEMITRANSPARENT, scr_flash.depth + 1, HD_UI_STRETCH);
        quad.page = scr_flash.fading ? 0x640 : 0x620;
        hd_ui_rect(&quad, 0, 0, 512, 240);
        hd_ui_add(&quad);
        return 0;
    }
    uint32 *entry = scene_render.cur->ot + scr_flash.depth + 1;
    uint8 *packet = scene_render.cursor;
    if (scene_render.limit - packet < (sint32)(sizeof(POLY_F4) + sizeof(DR_TPAGE)))
        return 1;
    scene_render.cursor = packet + sizeof(POLY_F4) + sizeof(DR_TPAGE);
    POLY_F4 *poly = (POLY_F4 *)packet;
    setPolyF4(poly);
    poly->rgbc.code = 42;
    poly->p[0].x = 0;
    poly->p[0].y = 0;
    poly->p[1].y = 0;
    poly->p[2].x = 0;
    poly->rgbc.r = scr_flash.color.r >> 16;
    poly->p[1].x = 512;
    poly->p[3].x = 512;
    poly->rgbc.g = scr_flash.color.g >> 16;
    poly->p[2].y = 240;
    poly->p[3].y = 240;
    poly->rgbc.b = scr_flash.color.b >> 16;
    bool fading = scr_flash.fading;
    setaddr(poly, getaddr(entry));
    setaddr(entry, gpu_packet_link((uintptr_t)poly));
    DR_TPAGE *mode = (DR_TPAGE *)(packet + sizeof(POLY_F4));
    mode->tag = 0x01000000;
    mode->code[0] = fading ? 0xE1000640 : 0xE1000620;
    setaddr(mode, getaddr(entry));
    setaddr(entry, gpu_packet_link((uintptr_t)mode));
    return *entry;
}
