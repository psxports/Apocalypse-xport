#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "model_lighting.h"
#include "model_render.h"

// Original result storage ends at the immutable distance-root table
static ModelLightingValue lighting_values[600];

void ResetModelLightingValues()
{
    memset(lighting_values, 0, sizeof(lighting_values));
}

ModelLightingValue &GetModelLightingValue(uint32 index)
{
    if (index >= sizeof(lighting_values) / sizeof(lighting_values[0]))
    {
        GameDiagnostic("Model lighting result index exceeds its native owner: %u\n", index);
        abort();
    }
    return lighting_values[index];
}

void ReadRendererNormal(const ModelLightingValue &value, uint32 normal[3])
{
    uint32 packed, last;
    FUNCTION_MARKER(0x800821B8, "SLUS_003.73");
    packed = value.xy;
    last = value.z;
    normal[0] = packed;
    normal[1] = (uint32)((sint32)packed >> 16);
    normal[2] = last;
}

void LoadRendererVertexLighting(uint32 color_offset, const ModelPalette *color_table, const ModelLightingValue &value)
{
    uint32 color, packed, last, y;
    FUNCTION_MARKER(0x80082224, "SLUS_003.73");
    color = ReadModelPaletteColor(color_table, (color_offset & 0x3FC) >> 2);
    packed = value.xy;
    last = value.z;
    y = (uint32)((sint32)packed >> 16);
    xport_gte_write_data(6, color);
    xport_gte_write_data(9, packed);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, last);
}

void LoadModelDrawLighting()
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x80082254, "SLUS_003.73");
    x = model_render_state.draw.light.x;
    y = model_render_state.draw.light.y;
    z = model_render_state.draw.light.z;
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
}
