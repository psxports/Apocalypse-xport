#ifndef MODEL_RENDER_VIEW_H
#define MODEL_RENDER_VIEW_H
extern "C"
{
#include "psx.h"
}

struct ModelRenderView
{
    uint32 pos[3];
    uint16 angles[4];
    MATRIX rotation;
    MATRIX inverse;
    MATRIX projection;
};

ModelRenderView &GetModelRenderView();
ModelRenderView &GetConfiguredModelRenderView();
void SetConfiguredModelRenderView(ModelRenderView &view);
MATRIX *SetModelRenderViewTranslation(ModelRenderView &view);
uint32 ReadModelRenderMatrixWord(const MATRIX &matrix, uint32 index);
#endif
