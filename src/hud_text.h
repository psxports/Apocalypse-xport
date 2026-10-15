#ifndef HUD_TEXT_H
#define HUD_TEXT_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeHudFonts();
uint32 ResetHudTextQueue();
uint32 DrawHudTextQueue();

uint32 QueueHudText(uint32 x, uint32 y, uint32 text, uint32 red, uint32 green, uint32 blue, uint32 flags);
uint32 QueueHudNumber(uint32 x, uint32 y, uint32 value, uint32 red, uint32 green, uint32 blue, uint32 flags);

uint32 AllocateAnimatedHudTextNode();
uint32 RemoveAnimatedHudTextNode(uint32 node);
void ClearAnimatedHudText();
uint32 AddAnimatedHudText(uint32 text, uint32 animation);
uint32 UpdateAnimatedHudText();
void DrawAnimatedHudText();

sint32 GetHudGlyphIndex(uint32 character);
void SetHudTextAlignment(uint32 alignment);
void SetHudTextScale(uint32 scale);
void SetHudTextDepth(uint32 depth);
uint32 SetHudTextColor(uint32 red, uint32 green, uint32 blue, uint32 semi_transparent);

uint32 DrawStyledHudText(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale);

uint32 LoadHudTextGlowResource();
uint32 InitializeHudTextGlowCurve();
uint32 UpdateHudTextGlowCurve();
uint32 MeasureStyledHudText(const char *text);

void SetHudTextShadowColor(uint32 red, uint32 green, uint32 blue);
void ResetHudTextShadowColor();

#endif
