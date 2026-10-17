#ifndef MODEL_MATERIALS_H
#define MODEL_MATERIALS_H

extern "C"
{
#include "psx.h"
}

struct ModelPalette
{
    uint32 count;
    uint32 *words;
};

struct ModelColorKey
{
    uint8 rgb[3];
    uint8 duration;
};

struct ModelColorTrack
{
    uint8 color_index;
    uint8 key;
    uint8 elapsed;
    uint32 key_count;
    ModelColorKey *keys;
};

struct ModelColorAnimation
{
    uint32 count;
    ModelColorTrack *tracks;
    bool present;
};

struct ModelUvVertex
{
    uint8 u;
    uint8 v;
    uint8 wave_x;
    uint8 wave_y;
};

struct ModelUvQuad
{
    ModelUvVertex vertices[4];
};

struct ModelTextureTrack
{
    uint32 object_index;
    sint16 scroll_x;
    sint16 scroll_y;
    uint32 phase_speed;
    uint32 quad_count;
    ModelUvQuad *quads;
};

struct ModelTextureAnimation
{
    uint32 count;
    ModelTextureTrack *tracks;
};

void ReleaseModelTextureAnimation(ModelTextureAnimation &animation);
void LoadModelTextureAnimation(ModelTextureAnimation &animation, const uint8 *data, uint32 bytes, uint32 object_count);

void ReleaseModelPalette(ModelPalette &palette);
void LoadModelPalette(ModelPalette &palette, const uint8 *data, uint32 bytes);
uint32 ReadModelPaletteColor(const ModelPalette *palette, uint32 index);
void SelectModelPalette(const ModelPalette &palette);
const ModelPalette *GetCurrentModelPalette();
void ResetModelPaletteSelection();
void ReleaseModelColorAnimation(ModelColorAnimation &animation);
void LoadModelColorAnimation(ModelColorAnimation &animation, const uint8 *data, uint32 bytes);
void ValidateModelColorAnimation(const ModelPalette &palette, const ModelColorAnimation &animation);
void AdvanceModelColors(ModelPalette &palette, ModelColorAnimation &animation, uint32 step);

#endif
