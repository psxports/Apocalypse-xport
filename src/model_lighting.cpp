#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "model_lighting.h"
#include "model_render.h"

// Original result storage ends at the immutable distance-root table
static ModelLightingValue lighting_vals[600];

void ResetModelLightingVals()
{
    memset(lighting_vals, 0, sizeof(lighting_vals));
}

ModelLightingValue &GetModelLightingVal(uint32 idx)
{
    if (idx >= sizeof(lighting_vals) / sizeof(lighting_vals[0]))
    {
        GameDiagnostic(("Model lighting result index exceeds its native owner: %u\n", idx));
        abort();
    }
    return lighting_vals[idx];
}

void GenerateModelNormalColors(const SVECTOR *vertices, uint32 count)
{
    FUNCTION_MARKER(0x80081794, "SLUS_003.73");
    uint32 output = 0, xy, z_pad, x, y, z;
    if (!count)
        return;
    xport_gte_write_data(6, 0xFFFFFF);
    xy = (uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16);
    z_pad = (uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16);
    do
    {
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, z_pad);
        ++vertices;
        --count;
        xport_gte_execute(0x108041B);
        xy = count ? ((uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;
        z_pad = count ? ((uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) : 0;
        x = xport_gte_read_data(9);
        y = xport_gte_read_data(10);
        z = xport_gte_read_data(11);
        ModelLightingValue &val = GetModelLightingVal(output);
        val.xy = (x & 0xFFFF) | (y << 16);
        val.z = z;
        ++output;
    } while ((sint32)count > 0);
}

uint32 ReadModelTableColor(uint32 color_offset, const ModelPalette *table)
{
    FUNCTION_MARKER(0x800821A4, "SLUS_003.73");
    return ReadModelPalColor(table, (color_offset & 0x3FC) >> 2);
}

void LoadPackedProjIr(const ModelLightingValue &val)
{
    FUNCTION_MARKER(0x800821E0, "SLUS_003.73");
    uint32 packed = val.xy;
    uint32 depth = val.z;
    xport_gte_write_data(9, packed);
    xport_gte_write_data(10, (sint32)packed >> 16);
    xport_gte_write_data(11, depth);
}

void LoadModelTableColor(uint32 color_offset, const ModelPalette *table, uint32 ir1, uint32 ir2, uint32 ir3)
{
    uint32 color;
    FUNCTION_MARKER(0x80082200, "SLUS_003.73");
    color = ReadModelPalColor(table, (color_offset & 0x3FC) >> 2);
    xport_gte_write_data(9, ir1);
    xport_gte_write_data(10, ir2);
    xport_gte_write_data(11, ir3);
    xport_gte_write_data(6, color);
}

void LoadRendererVertexLighting(uint32 color_offset, const ModelPalette *color_table, const ModelLightingValue &val)
{
    uint32 color, packed, last;
    sint32 y;
    FUNCTION_MARKER(0x80082224, "SLUS_003.73");
    color = ReadModelPalColor(color_table, (color_offset & 0x3FC) >> 2);
    packed = val.xy;
    last = val.z;
    y = (sint32)packed >> 16;
    xport_gte_write_data(6, color);
    xport_gte_write_data(9, packed);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, last);
}

void LoadModelDrawLighting()
{
    sint32 x, y, z;
    FUNCTION_MARKER(0x80082254, "SLUS_003.73");
    x = model_render_state.draw.light.x;
    y = model_render_state.draw.light.y;
    z = model_render_state.draw.light.z;
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
}
