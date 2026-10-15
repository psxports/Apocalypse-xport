#ifndef MODEL_LIGHTING_H
#define MODEL_LIGHTING_H

extern "C"
{
#include "psx.h"
}

void ReadRendererNormal(uint32 address, uint32 normal[3]);
void LoadRendererVertexLighting(uint32 color_offset, uint32 color_table, uint32 normal_address);
void LoadRendererScratchLighting(uint32 scratch);

#endif
