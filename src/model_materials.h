#ifndef MODEL_MATERIALS_H
#define MODEL_MATERIALS_H

#include "model_uv_vertex.h"

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
    uint8 color_idx;
    uint8 key;
    uint8 elapsed;
    uint8 key_count;
    ModelColorKey *keys;
};

struct ModelColorAnimation
{
    uint32 count;
    ModelColorTrack *tracks;
    bool present;
};

struct ModelUvQuad
{
    ModelUvVertex vertices[4];
};

struct ModelTextureTrack
{
    uint32 object_idx;
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

void ReleaseModelTexAnimation(ModelTextureAnimation &animation);
void LoadModelTexAnimation(ModelTextureAnimation &animation, const uint8 *data, uint32 bytes, uint32 object_count);

void ReleaseModelPal(ModelPalette &pal);
void LoadModelPal(ModelPalette &pal, const uint8 *data, uint32 bytes);
uint32 ReadModelPalColor(const ModelPalette *pal, uint32 idx);
void SelectModelPal(const ModelPalette &pal);
const ModelPalette *GetCurModelPal();
void ResetModelPalSelection();
void ReleaseModelColorAnimation(ModelColorAnimation &animation);
void LoadModelColorAnimation(ModelColorAnimation &animation, const uint8 *data, uint32 bytes);
void ValidateModelColorAnimation(const ModelPalette &pal, const ModelColorAnimation &animation);
void AdvanceModelColors(ModelPalette &pal, ModelColorAnimation &animation, uint32 step);

#endif
