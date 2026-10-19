#include <string.h>

#include "scene_lifecycle.h"
#include "game_diagnostics.h"
#include "hud_sprites.h"
#include "texture_resources.h"

POLY_FT4 *AllocateHudSprite(const TextureFrame &frame)
{
    uint8 *packet, *next, *limit;
    uint32 uv0, uv1, uv2, tail;
    FUNCTION_MARKER(0x8006D028, "SLUS_003.73");
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)sizeof(POLY_FT4))
        return 0;
    next = packet + sizeof(POLY_FT4);
    scene_render.cursor = next;
    POLY_FT4 *poly = (POLY_FT4 *)packet;
    poly->tag = 0;
    setPolyFT4(poly);
    poly->rgbc.rgbc = 0x2C808080;
    const TextureResource &tex = *frame.tex;
    uv0 = PackTextureUv(tex, 0);
    uv1 = PackTextureUv(tex, 1);
    uv2 = PackTextureUv(tex, 2);
    tail = tex.uv[3].uv;
    poly->p[1].uv.uv = uv1;
    poly->p[1].attr = uv1 >> 16;
    poly->p[0].uv.uv = uv0;
    poly->p[0].attr = uv0 >> 16;
    poly->p[2].uv.uv = uv2;
    poly->p[2].attr = uv2 >> 16;
    poly->p[3].uv.uv = tail;
    setaddr(poly, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)poly));
    return poly;
}

uint32 PlaceScaledHudSprite(uint32 x, uint32 y, POLY_FT4 *poly, const TextureFrame &frame)
{
    uint32 offset, width, top, height, left, right, bottom;
    sint32 offset_high, width_high;
    FUNCTION_MARKER(0x8006D0D4, "SLUS_003.73");
    offset = (uint32)(sint32)frame.ox << 9;
    offset_high = (sint32)(((sint64)(sint32)offset * (sint64)0x66666667) >> 32);
    width = (uint32)frame.w << 9;
    top = y + (uint32)(sint32)frame.oy;
    width_high = (sint32)(((sint64)(sint32)width * (sint64)0x66666667) >> 32);
    height = (uint32)frame.h;
    poly->p[0].xy.y = top;
    poly->p[2].xy.y = top + height;
    left = x + (uint32)(offset_high >> 7) - (uint32)((sint32)offset >> 31);
    poly->p[0].xy.x = left;
    right = left + (uint32)(width_high >> 7);
    poly->p[1].xy.x = right;
    bottom = (uint16)poly->p[2].xy.y;
    poly->p[2].xy.x = left;
    poly->p[1].xy.y = top;
    poly->p[3].xy.x = right;
    poly->p[3].xy.y = bottom;
    return bottom;
}

uint32 PlaceHudSprite(uint32 x, uint32 y, POLY_FT4 *poly, const TextureFrame &frame)
{
    uint32 width, height, left, top, right, bottom;
    FUNCTION_MARKER(0x8006D164, "SLUS_003.73");
    width = (uint32)frame.w;
    height = (uint32)frame.h;
    left = x + (uint32)(sint32)frame.ox;
    top = y + (uint32)(sint32)frame.oy;
    poly->p[0].xy.x = left;
    poly->p[0].xy.y = top;
    poly->p[1].xy.x = left + width;
    poly->p[2].xy.y = top + height;
    right = (uint16)poly->p[1].xy.x;
    bottom = (uint16)poly->p[2].xy.y;
    poly->p[1].xy.y = top;
    poly->p[2].xy.x = left;
    poly->p[3].xy.x = right;
    poly->p[3].xy.y = bottom;
    return top;
}

POLY_F4 *AllocateHudRectangle(uint32 x, uint32 y, uint32 width, uint32 height, uint32 red, uint32 green, uint32 blue, uint32 depth)
{
    uint8 *packet, *limit, *next;
    uint32 bottom, right;
    FUNCTION_MARKER(0x8006D1C0, "SLUS_003.73");
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)sizeof(POLY_F4))
        return 0;
    next = packet + sizeof(POLY_F4);
    scene_render.cursor = next;
    POLY_F4 *poly = (POLY_F4 *)packet;
    setPolyF4(poly);
    bottom = y + height;
    poly->rgbc.r = red;
    poly->rgbc.g = green;
    poly->rgbc.b = blue;
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

uint32 QueueHudGradientRectangle(uint32 x, uint32 y, uint32 width, uint32 height, uint32 right_red, uint32 right_green, uint32 right_blue, uint32 left_red, uint32 left_green, uint32 left_blue, uint32 depth)
{
    uint8 *packet, *limit, *next;
    uint32 right, bottom;
    FUNCTION_MARKER(0x8006D294, "SLUS_003.73");
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)sizeof(POLY_G4))
        return 1;
    next = packet + sizeof(POLY_G4);
    scene_render.cursor = next;
    POLY_G4 *poly = (POLY_G4 *)packet;
    setPolyG4(poly);
    right = x + width;
    poly->p[1].xy.x = right;
    poly->p[3].xy.x = right;
    bottom = y + height;
    poly->p[0].rgbc.r = left_red;
    poly->p[1].rgbc.r = right_red;
    poly->p[0].rgbc.g = left_green;
    poly->p[1].rgbc.g = right_green;
    poly->p[0].rgbc.b = left_blue;
    poly->p[1].rgbc.b = right_blue;
    poly->p[2].rgbc.r = left_red;
    poly->p[3].rgbc.r = right_red;
    poly->p[2].rgbc.g = left_green;
    poly->p[3].rgbc.g = right_green;
    poly->p[2].rgbc.b = left_blue;
    poly->p[3].rgbc.b = right_blue;
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
