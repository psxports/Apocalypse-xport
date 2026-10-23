#include <string.h>

#include "game_lighting.h"
#include "vector3.h"

GameLighting game_lighting;

GameLighting::GameLighting()
{
    far_color.r = far_color.g = far_color.b = 0;
    background_color.r = background_color.g = background_color.b = 0;
    memset(&directions, 0, sizeof(directions));
    memset(&colors, 0, sizeof(colors));
    directions.m[0][0] = ONE;
    directions.m[1][1] = -ONE;
    directions.m[2][2] = ONE;
    colors.m[0][2] = ONE;
    colors.m[1][2] = ONE;
    colors.m[2][2] = ONE;
}

void ResetGameLighting()
{
    game_lighting = GameLighting();
}

void SetLightingDirs(const ShortVector3 &v0, const ShortVector3 &v1, const ShortVector3 &v2)
{
    FUNCTION_MARKER(0x8007C978, "SLUS_003.73");
    for (uint32 lane = 0; lane < 3; ++lane)
        game_lighting.directions.m[0][lane] = v0.comp[lane];
    for (uint32 lane = 0; lane < 3; ++lane)
        game_lighting.directions.m[1][lane] = v1.comp[lane];
    for (uint32 lane = 0; lane < 3; ++lane)
        game_lighting.directions.m[2][lane] = v2.comp[lane];
}

void SetLightingColors(const LightColor &v0, const LightColor &v1, const LightColor &v2)
{
    FUNCTION_MARKER(0x8007C9E4, "SLUS_003.73");
    game_lighting.colors.m[0][0] = v0.r;
    game_lighting.colors.m[0][1] = v1.r;
    game_lighting.colors.m[0][2] = v2.r;
    game_lighting.colors.m[1][0] = v0.g;
    game_lighting.colors.m[1][1] = v1.g;
    game_lighting.colors.m[1][2] = v2.g;
    game_lighting.colors.m[2][0] = v0.b;
    game_lighting.colors.m[2][1] = v1.b;
    game_lighting.colors.m[2][2] = v2.b;
}
