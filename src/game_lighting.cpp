#include <string.h>

#include "game_lighting.h"

GameLighting game_lighting;

GameLighting::GameLighting()
{
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

void SetLightingDirections(const sint16 *first, const sint16 *second, const sint16 *third)
{
    FUNCTION_MARKER(0x8007C978, "SLUS_003.73");
    for (uint32 lane = 0; lane < 3; ++lane)
        game_lighting.directions.m[0][lane] = first[lane];
    for (uint32 lane = 0; lane < 3; ++lane)
        game_lighting.directions.m[1][lane] = second[lane];
    for (uint32 lane = 0; lane < 3; ++lane)
        game_lighting.directions.m[2][lane] = third[lane];
}

void SetLightingColors(const sint16 *first, const sint16 *second, const sint16 *third)
{
    FUNCTION_MARKER(0x8007C9E4, "SLUS_003.73");
    for (uint32 lane = 0; lane < 3; ++lane)
    {
        game_lighting.colors.m[lane][0] = first[lane];
        game_lighting.colors.m[lane][1] = second[lane];
        game_lighting.colors.m[lane][2] = third[lane];
    }
}
