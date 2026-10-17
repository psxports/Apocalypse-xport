#ifndef HUD_SPRITES_H
#define HUD_SPRITES_H

extern "C"
{
#include "psx.h"
}

uint32 QueueHudGradientRectangle(uint32 x, uint32 y, uint32 width, uint32 height, uint32 right_red, uint32 right_green, uint32 right_blue, uint32 left_red, uint32 left_green, uint32 left_blue, uint32 depth);
uint8 *AllocateHudRectangle(uint32 x, uint32 y, uint32 width, uint32 height, uint32 red, uint32 green, uint32 blue, uint32 depth);
struct TextureFrame;
uint8 *AllocateHudSprite(const TextureFrame &frame);
uint32 PlaceScaledHudSprite(uint32 x, uint32 y, uint8 *packet, const TextureFrame &frame);
uint32 PlaceHudSprite(uint32 x, uint32 y, uint8 *packet, const TextureFrame &frame);


#endif
