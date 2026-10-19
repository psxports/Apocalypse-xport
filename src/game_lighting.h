#ifndef GAME_LIGHTING_H
#define GAME_LIGHTING_H

extern "C"
{
#include "psx.h"
}

struct LightColor
{
    sint16 r, g, b;

    LightColor() : r(0), g(0), b(0)
    {
    }
};

struct GameLighting
{
    MATRIX directions;
    MATRIX colors;
    LightColor far_color;
    LightColor background_color;
    GameLighting();
};

extern GameLighting game_lighting;
void ResetGameLighting();
void SetLightingDirections(const sint16 *first, const sint16 *second, const sint16 *third);
void SetLightingColors(const sint16 *first, const sint16 *second, const sint16 *third);

#endif
