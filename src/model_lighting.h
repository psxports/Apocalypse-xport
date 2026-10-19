#ifndef MODEL_LIGHTING_H
#define MODEL_LIGHTING_H

#include "model_materials.h"

extern "C"
{
#include "psx.h"
}

struct ModelLightingValue
{
    uint32 xy;

    union
    {
        uint32 z;
        uint32 uv;
    };
};

void ResetModelLightingValues();
ModelLightingValue &GetModelLightingValue(uint32 index);
void ReadRendererNormal(const ModelLightingValue &value, uint32 normal[3]);
void LoadRendererVertexLighting(uint32 color_offset, const ModelPalette *color_table, const ModelLightingValue &value);
void LoadModelDrawLighting();

#endif
