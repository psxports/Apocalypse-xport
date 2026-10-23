#ifndef ANIMATED_HUD_TEXT_H
#define ANIMATED_HUD_TEXT_H

#include "psx.h"

struct HudTextAnimation;

struct AnimatedHudText
{
    const char *text;
    uint16 age;
    const HudTextAnimation *animation;
    AnimatedHudText *next;
    AnimatedHudText *prev;

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
};

#endif
