#ifndef MODEL_LIGHTING_H
#define MODEL_LIGHTING_H

#include "model_materials.h"

extern "C"
{
#include "psx.h"
}

struct ModelLightingValue
{
    // IR1 receives this complete packed word before the SDK narrows it
    uint32 xy;

    union
    {
        uint32 z;
        uint32 uv;
    };
};

void ResetModelLightingVals();
ModelLightingValue &GetModelLightingVal(uint32 idx);
void GenerateModelNormalColors(const SVECTOR *vertices, uint32 count);
uint32 ReadModelTableColor(uint32 color_offset, const ModelPalette *table);
void LoadPackedProjIr(const ModelLightingValue &val);
void LoadModelTableColor(uint32 color_offset, const ModelPalette *table, uint32 ir1, uint32 ir2, uint32 ir3);
void LoadRendererVertexLighting(uint32 color_offset, const ModelPalette *color_table, const ModelLightingValue &val);
void LoadModelDrawLighting();

#endif
