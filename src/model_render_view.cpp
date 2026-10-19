#include "model_render_view.h"

static ModelRenderView model_render_view;
static ModelRenderView *configured_model_render_view;

ModelRenderView &GetModelRenderView()
{
    return model_render_view;
}

ModelRenderView &GetConfiguredModelRenderView()
{
    return *configured_model_render_view;
}

void SetConfiguredModelRenderView(ModelRenderView &view)
{
    configured_model_render_view = &view;
}

MATRIX *SetModelRenderViewTranslation(ModelRenderView &view)
{
    VECTOR pos;
    pos.vx = (sint32)view.pos.words[0];
    pos.vy = (sint32)view.pos.words[1];
    pos.vz = (sint32)view.pos.words[2];
    pos.pad = 0;
    return TransMatrix(&view.rotation, &pos);
}

uint32 ReadModelRenderMatrixWord(const MATRIX &matrix, uint32 index)
{
    // Callers select one of the five GTE rotation-control words
    if (index == 4)
    {
        // The final SDK word includes two ABI alignment bytes
        return xport_load_le32((const uint8 *)&matrix + 8 * sizeof(sint16));
    }
    uint32 component = index * 2;
    uint32 low = (uint16)matrix.m[component / 3][component % 3];
    ++component;
    return low | ((uint32)(uint16)matrix.m[component / 3][component % 3] << 16);
}
