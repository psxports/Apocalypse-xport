#ifndef MODEL_RENDER_VIEW_INTERNAL_H
#define MODEL_RENDER_VIEW_INTERNAL_H

#include "model_render_view.h"

struct ModelClipPlane
{
    ShortVector3 n;
    sint16 offset;
};

// One view-owned clip set is consumed by visible-list traversal
extern ModelClipPlane model_clip_planes[8];
extern MATRIX model_clip_matrix[2];

#endif
