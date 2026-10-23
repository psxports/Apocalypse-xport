#include "scene_lifecycle.h"
#include "hud_sprites.h"
#include "texture_resources.h"

POLY_FT4 *AllocateHudSpr(const TextureFrame &frame)
{
    uint8 *packet;
    FUNCTION_MARKER(0x8006D028, "SLUS_003.73");
    packet = scene_render.cursor;
    if (scene_render.limit - packet < (sint32)sizeof(POLY_FT4))
        return 0;
    scene_render.cursor = packet + sizeof(POLY_FT4);
    POLY_FT4 *poly = (POLY_FT4 *)packet;
    poly->tag = 0;
    setPolyFT4(poly);
    poly->rgbc.rgbc = 0x2C808080;
    const TextureResource &tex = *frame.tex;
    poly->p[1].uv = tex.uv[1];
    poly->p[1].attr = tex.tpage;
    poly->p[0].uv = tex.uv[0];
    poly->p[0].attr = tex.clut;
    poly->p[2].uv = tex.uv[2];
    // Original packed texture word also carries the fourth UV in this halfword
    poly->p[2].attr = tex.uv[3].uv;
    poly->p[3].uv = tex.uv[3];
    setaddr(poly, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)poly));
    return poly;
}

uint32 PlaceScaledHudSpr(uint32 x, uint32 y, POLY_FT4 *poly, const TextureFrame &frame)
{
    uint32 top, left, right;
    uint16 bottom;
    FUNCTION_MARKER(0x8006D0D4, "SLUS_003.73");
    top = y + frame.oy;
    poly->p[0].xy.y = top;
    poly->p[2].xy.y = top + frame.h;
    // Horizontal coordinates scale from 320 to 512 with division toward zero
    left = x + frame.ox * 8 / 5;
    poly->p[0].xy.x = left;
    right = left + frame.w * 8 / 5;
    poly->p[1].xy.x = right;
    bottom = poly->p[2].xy.y;
    poly->p[2].xy.x = left;
    poly->p[1].xy.y = top;
    poly->p[3].xy.x = right;
    poly->p[3].xy.y = bottom;
    return bottom;
}

uint32 PlaceHudSpr(uint32 x, uint32 y, POLY_FT4 *poly, const TextureFrame &frame)
{
    uint32 left, top;
    uint16 right, bottom;
    FUNCTION_MARKER(0x8006D164, "SLUS_003.73");
    left = x + frame.ox;
    top = y + frame.oy;
    poly->p[0].xy.x = left;
    poly->p[0].xy.y = top;
    poly->p[1].xy.x = left + frame.w;
    poly->p[2].xy.y = top + frame.h;
    right = poly->p[1].xy.x;
    bottom = poly->p[2].xy.y;
    poly->p[1].xy.y = top;
    poly->p[2].xy.x = left;
    poly->p[3].xy.x = right;
    poly->p[3].xy.y = bottom;
    return top;
}

POLY_F4 *AllocateHudRect(uint32 x, uint32 y, uint32 width, uint32 height, uint8 r, uint8 g, uint8 b, uint32 depth)
{
    uint8 *packet;
    uint32 bottom, right;
    FUNCTION_MARKER(0x8006D1C0, "SLUS_003.73");
    packet = scene_render.cursor;
    if (scene_render.limit - packet < (sint32)sizeof(POLY_F4))
        return 0;
    scene_render.cursor = packet + sizeof(POLY_F4);
    POLY_F4 *poly = (POLY_F4 *)packet;
    setPolyF4(poly);
    bottom = y + height;
    poly->rgbc.r = r;
    poly->rgbc.g = g;
    poly->rgbc.b = b;
    poly->p[2].y = bottom;
    poly->p[3].y = bottom;
    right = x + width;
    poly->p[0].x = x;
    poly->p[2].x = x;
    poly->p[0].y = y;
    poly->p[1].x = right;
    poly->p[1].y = y;
    poly->p[3].x = right;
    setaddr(poly, getaddr(scene_render.cur->ot + depth));
    setaddr(scene_render.cur->ot + depth, gpu_packet_link((uintptr_t)poly));
    return poly;
}

uint32 QueueHudGradientRect(uint32 x, uint32 y, uint32 width, uint32 height, uint8 right_r, uint8 right_g, uint8 right_b, uint8 left_r, uint8 left_g, uint8 left_b, uint32 depth)
{
    uint8 *packet;
    uint32 right, bottom;
    FUNCTION_MARKER(0x8006D294, "SLUS_003.73");
    packet = scene_render.cursor;
    if (scene_render.limit - packet < (sint32)sizeof(POLY_G4))
        return 1;
    scene_render.cursor = packet + sizeof(POLY_G4);
    POLY_G4 *poly = (POLY_G4 *)packet;
    setPolyG4(poly);
    right = x + width;
    poly->p[1].xy.x = right;
    poly->p[3].xy.x = right;
    bottom = y + height;
    poly->p[0].rgbc.r = left_r;
    poly->p[1].rgbc.r = right_r;
    poly->p[0].rgbc.g = left_g;
    poly->p[1].rgbc.g = right_g;
    poly->p[0].rgbc.b = left_b;
    poly->p[1].rgbc.b = right_b;
    poly->p[2].rgbc.r = left_r;
    poly->p[3].rgbc.r = right_r;
    poly->p[2].rgbc.g = left_g;
    poly->p[3].rgbc.g = right_g;
    poly->p[2].rgbc.b = left_b;
    poly->p[3].rgbc.b = right_b;
    poly->p[2].xy.y = bottom;
    poly->p[3].xy.y = bottom;
    poly->p[0].xy.y = y;
    poly->p[1].xy.y = y;
    poly->p[0].xy.x = x;
    poly->p[2].xy.x = x;
    uint32 *ot_entry = scene_render.cur->ot + depth;
    setaddr(poly, getaddr(ot_entry));
    setaddr(ot_entry, gpu_packet_link((uintptr_t)poly));
    return *ot_entry;
}
