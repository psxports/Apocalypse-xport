#ifndef MODEL_SHADOW_H
#define MODEL_SHADOW_H
extern "C"
{
#include "psx.h"
}
struct TextureResource;
struct ModelGeometry;
uint32 RenderModelShadow(const ModelGeometry &model, const void *matrix, uint32 rgb, uint32 scale, uint32 depth_bias, const TextureResource &texture);
#endif
