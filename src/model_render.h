#include "model_render_view.h"
#ifndef MODEL_RENDER_H
#define MODEL_RENDER_H
extern "C"
{
#include "psx.h"
}
#include "model_geometry.h"
#include "model_instance.h"
void BuildModelPolygonPackets(const ModelPolygon *descriptor, uint32 count);
void EmitScratchModelPackets(const ModelPolygon *descriptor, uint32 count);
void RenderModelObjectList(ModelInstance *object);
void RenderTranslatedClippedModel(const ModelGeometry &geometry);
void RenderUntranslatedClippedModel(const ModelGeometry &geometry);
void RenderProjectedModel(const ModelGeometry &model);
void RenderScratchModel(const ModelGeometry &geometry);
void RenderVisibleModelObjectList(uint32 object);
uint32 RenderPosedModelObject(ModelInstance &instance);
void CopyModelRotationMatrix(const uint32 *input, uint32 *output);
void ClearModelRotationMatrix(uint32 *output);
void InitializeModelIdentityMatrix(uint32 *matrix);
void TransposeModelMatrix(const uint32 *input, uint32 *output);
uint32 AdvanceModelRenderCounter();
uint32 CullModelObjectList(ModelInstance *object);
void RenderTranslatedModel(const ModelGeometry &geometry);
void RenderOffsetModel(const ModelGeometry &model, const sint32 vector[3]);
void RenderOffsetScratchModel(const ModelGeometry &model, const sint32 position[3]);
void UpdateModelTextureCoordinates(uint32 bank_index);
uint32 SetModelDrawArea(uint32 bounds, uint32 ordering_table);
void LoadModelLineClipMatrices();
uint32 ConfigureModelRenderView(ModelRenderView &camera, uint32 bounds, uint32 ordering_table);
#endif
