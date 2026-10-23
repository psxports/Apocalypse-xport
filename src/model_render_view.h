#ifndef MODEL_RENDER_VIEW_H
#define MODEL_RENDER_VIEW_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

struct ModelViewport;

struct ModelRenderView
{
    Vector3 pos;
    Vector3 target_pos;
    ShortVector3 angles;
    MATRIX rotation;
    MATRIX inverse;
    MATRIX projection;

public:
    uint32 Configure(ModelViewport &bounds, uint32 *ordering_table);
    MATRIX *SetTranslation();
};

ModelRenderView &GetModelRenderView();
ModelRenderView &GetConfiguredModelRenderView();
void SetConfiguredModelRenderView(ModelRenderView &view);
uint32 PackModelRotationWord(const MATRIX &matrix, uint32 idx);
#endif
