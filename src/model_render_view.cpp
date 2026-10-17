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
    pos.vx = (sint32)view.pos[0];
    pos.vy = (sint32)view.pos[1];
    pos.vz = (sint32)view.pos[2];
    pos.pad = 0;
    return TransMatrix(&view.rotation, &pos);
}

uint32 ReadModelRenderMatrixWord(const MATRIX &matrix, uint32 index)
{
    return xport_load_le32((const uint8 *)&matrix + index * 4);
}
