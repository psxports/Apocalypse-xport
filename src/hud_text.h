#ifndef HUD_TEXT_H
#define HUD_TEXT_H

#include "animated_hud_text.h"

extern "C"
{
#include "psx.h"
}

#include "texture_resources.h"

struct HD_UI_TEXT_HISTORY;

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
    uint8 amplitude[40];
    uint8 rate[40];
    sint32 curve[41];
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
const TextureResource *GetHudGlowTex();
void ForgetHudTex(const TextureGroup *grp);
void ForgetHudTex(const TextureResource *tex);
void ResetHudTextQueue();
uint32 DrawHudTextQueue();

uint32 QueueHudText(uint16 x, uint16 y, const char *text, uint8 r, uint8 g, uint8 b, uint8 flags);
uint32 QueueHudNumber(uint16 x, uint16 y, sint32 val, uint8 r, uint8 g, uint8 b, uint8 flags);

sint32 GetHudGlyphIdx(uint8 character);
void SetHudTextAlignment(uint8 alignment);
void SetHudTextScale(uint16 scale);
void SetHudTextDepth(uint16 depth);
uint32 SetHudTextColor(uint8 r, uint8 g, uint8 b, bool semi_transparent);

uint32 DrawStyledHudText(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale);
uint32 DrawStyledHudTextPair(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale, uint32 previous_color, uint32 previous_scale);
uint32 DrawStyledHudTextHistory(HD_UI_TEXT_HISTORY &history, uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale, float text_scale);

void LoadHudTextGlowResource();
uint32 InitializeHudTextGlowCurve();
uint32 UpdateHudTextGlowCurve();
uint32 MeasureStyledHudText(const char *text);

void ResetHudTextShadowColor();

#endif
