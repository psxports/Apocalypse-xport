#ifndef GAME_LIGHTING_H
#define GAME_LIGHTING_H

extern "C"
{
#include "psx.h"
}

struct ShortVector3;

struct LightColor
{
    sint16 r, g, b;
};

struct GameLighting
{
    MATRIX directions;
    MATRIX colors;
    LightColor far_color;
    LightColor background_color;

public:
    GameLighting();
};

extern GameLighting game_lighting;
void ResetGameLighting();
void SetLightingDirs(const ShortVector3 &v0, const ShortVector3 &v1, const ShortVector3 &v2);
void SetLightingColors(const LightColor &v0, const LightColor &v1, const LightColor &v2);

#endif
