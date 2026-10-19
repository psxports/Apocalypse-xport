#ifndef MODEL_RENDER_VIEW_H
#define MODEL_RENDER_VIEW_H

#include "vector3.h"
extern "C"
{
#include "psx.h"
}

struct ModelRenderView
{
    Vector3 pos;
    ShortVector3 angles;
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
