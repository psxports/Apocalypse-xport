#ifndef ANIMATED_HUD_TEXT_H
#define ANIMATED_HUD_TEXT_H

#include "psx.h"
#include "hd.h"

struct HudTextAnimation;

struct AnimatedHudText
{
    const char *text;
    uint16 age;
    const HudTextAnimation *animation;
    AnimatedHudText *next;
    AnimatedHudText *prev;
    HD_UI_TEXT_HISTORY hd_text;
    float hd_previous_scale;

private:
    static AnimatedHudText *Allocate();

public:
    void Remove();
    static void ClearAll();
    static void Add(const char *text, const HudTextAnimation *animation);
    static uint32 UpdateAll();
    static void DrawAll();

private:
    static uint32 Interpolate(sint32 first, sint32 last, uint16 age, uint16 duration);
    static float GetHdScale(const HudTextAnimation &animation, uint16 age);
};

#endif
