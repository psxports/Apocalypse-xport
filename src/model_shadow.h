#ifndef MODEL_SHADOW_H
#define MODEL_SHADOW_H
extern "C"
{
#include "psx.h"
}
uint32 RenderModelShadow(uint32 model, const void *matrix, uint32 rgb, uint32 scale, uint32 depth_bias, uint32 texture);
#endif
