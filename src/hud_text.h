#ifndef HUD_TEXT_H
#define HUD_TEXT_H

extern "C"
{
#include "psx.h"
}

#include "texture_resources.h"

struct HudTextAnimation
{
    uint16 duration;
    uint8 semi;
    uint8 align;
    uint16 scale[2];
    uint8 rgb[2][3];
    uint16 filter[2];
    sint16 scr_pos[2][2];
    uint8 blink;
    sint8 step;
};

struct HudTextStyle
{
    uint8 align;
    RGBC color;
    uint16 scale;
    uint16 depth;
    RGBC shadow;
};

struct HudTextGlow
{
    uint32 amplitude[40];
    uint32 rate[40];
    uint32 curve[41];
    uint32 timer;
    // TODO Recover the producer of the alternate glow cycle flag
    uint32 alternate;
};

extern HudTextStyle hud_text_style;
extern HudTextGlow hud_text_glow;
void ResetHudTextState();

extern const HudTextAnimation hud_pickup_animation[3];
extern const HudTextAnimation hud_extra_life_animation[3];
extern const HudTextAnimation hud_weapon_animation[3];
extern const HudTextAnimation hud_pulse_laser_animation[3];
extern const HudTextAnimation hud_flamethrower_animation[3];

namespace HudMessage
{
extern const char checkpoint[];
extern const char health20[];
extern const char health50[];
extern const char health100[];
extern const char smart_bomb[];
extern const char extra_life[];
extern const char flamethrower[];
extern const char pulse_laser[];
extern const char particle_beam[];
extern const char rip_laser[];
extern const char homing_missiles[];
extern const char rockets[];
extern const char grenades[];
} // namespace HudMessage

void InitializeHudFonts();
const TextureGroup *GetHudFont();
const TextureResource *GetHudGlowTexture();
void ForgetHudTexture(const TextureGroup *grp);
void ForgetHudTexture(const TextureResource *tex);
void ClearHudTextureBindings();
void ResetHudTextQueue();
uint32 DrawHudTextQueue();

uint32 QueueHudText(uint32 x, uint32 y, const char *text, uint32 red, uint32 green, uint32 blue, uint32 flags);
uint32 QueueHudNumber(uint32 x, uint32 y, uint32 value, uint32 red, uint32 green, uint32 blue, uint32 flags);

void ClearAnimatedHudText();
void AddAnimatedHudText(const char *text, const HudTextAnimation *animation);
uint32 UpdateAnimatedHudText();
void DrawAnimatedHudText();

sint32 GetHudGlyphIndex(uint32 character);
void SetHudTextAlignment(uint32 alignment);
void SetHudTextScale(uint32 scale);
void SetHudTextDepth(uint32 depth);
uint32 SetHudTextColor(uint32 red, uint32 green, uint32 blue, uint32 semi_transparent);

uint32 DrawStyledHudText(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale);

void LoadHudTextGlowResource();
uint32 InitializeHudTextGlowCurve();
uint32 UpdateHudTextGlowCurve();
uint32 MeasureStyledHudText(const char *text);

void ResetHudTextShadowColor();

#endif
