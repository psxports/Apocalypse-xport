#ifndef MODEL_RENDER_H
#define MODEL_RENDER_H
extern "C"
{
#include "psx.h"
}
void BuildModelPolygonPackets(uint32 descriptor, uint32 count);
void RenderModelObjectList(uint32 object);
void RenderVisibleModelObjectList(uint32 object);
uint32 RenderPosedModelObject(uint32 object);
void CopyModelRotationMatrix(const uint32 *input, uint32 *output);
void ClearModelRotationMatrix(uint32 *output);
void InitializeModelIdentityMatrix(uint32 *matrix);
void TransposeModelMatrix(const uint32 *input, uint32 *output);
uint32 AdvanceModelRenderCounter();
uint32 CullModelObjectList(uint32 object);
void RenderTranslatedModel(uint32 geometry);
void RenderOffsetModel(uint32 model, const sint32 vector[3]);
void RenderOffsetScratchModel(uint32 model, const sint32 position[3]);
uint32 UpdateModelTextureCoordinates(uint32 bank_index);
uint32 InitializeModelUvWaveTable();
uint32 SetModelDrawArea(uint32 bounds, uint32 ordering_table);
uint32 ConfigureModelRenderView(uint32 camera, uint32 bounds, uint32 ordering_table);
#endif
