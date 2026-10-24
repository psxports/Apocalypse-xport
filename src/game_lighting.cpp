#include <string.h>

#include "game_lighting.h"
#include "hd.h"
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

void GameLighting::CaptureHd(HD_LIGHTING &lighting, const MATRIXF &model_rotation, uint32 draw_mask, const VECTORF &constant, float fog_start, sint32 fog_shift) const
{
    MATRIXF light;
    IdentityMatrixf(&light);
    IdentityMatrixf(&lighting.directions);
    IdentityMatrixf(&lighting.colors);
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 column = 0; column < 3; ++column)
        {
            light.m[row][column] = directions.m[row][column] / 4096.0f;
            lighting.colors.m[row][column] = colors.m[row][column] / 4096.0f;
        }
    MulMatrix0f(&light, &model_rotation, &lighting.directions);
    lighting.world_directions = light;
    lighting.world_directions_valid = 1;
    lighting.background.vx = background_color.r;
    lighting.background.vy = background_color.g;
    lighting.background.vz = background_color.b;
    lighting.far_color.vx = far_color.r;
    lighting.far_color.vy = far_color.g;
    lighting.far_color.vz = far_color.b;
    lighting.constant = constant;
    lighting.draw_mask = draw_mask;
    lighting.fog_start = fog_start;
    lighting.fog_shift = fog_shift;
}
