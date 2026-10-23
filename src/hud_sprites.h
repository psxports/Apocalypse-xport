#ifndef HUD_SPRITES_H
#define HUD_SPRITES_H

extern "C"
{
#include "psx.h"
}

uint32 QueueHudGradientRect(uint32 x, uint32 y, uint32 width, uint32 height, uint8 right_r, uint8 right_g, uint8 right_b, uint8 left_r, uint8 left_g, uint8 left_b, uint32 depth);
POLY_F4 *AllocateHudRect(uint32 x, uint32 y, uint32 width, uint32 height, uint8 r, uint8 g, uint8 b, uint32 depth);
struct TextureFrame;
POLY_FT4 *AllocateHudSpr(const TextureFrame &frame);
uint32 PlaceScaledHudSpr(uint32 x, uint32 y, POLY_FT4 *poly, const TextureFrame &frame);
uint32 PlaceHudSpr(uint32 x, uint32 y, POLY_FT4 *poly, const TextureFrame &frame);

#endif
