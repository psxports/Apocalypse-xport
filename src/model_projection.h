#ifndef MODEL_PROJECTION_H
#define MODEL_PROJECTION_H
extern "C"
{
#include "psx.h"
}
uint32 UpdateProjectedVertexDepthFlags(uint32 count);
uint32 ProjectModelReferenceVertices(uint32 input, uint32 count);
void LoadPackedProjectionIr(uint32 geometry);
uint32 MeasureProjectionIrLength();
uint32 NormalizeProjectionIr();
void CrossProjectionIr(const uint32 *vector);
void TransformProjectionIr(const uint32 *matrix);
void GenerateModelNormalColors(uint32 vertices, uint32 count);
void RenderModelProjectionEffect(uint32 model, const uint32 *matrix, uint32 vector, uint32 position, uint32 direction, uint32 color);
uint32 GenerateModelReflectionCoordinates(const uint32 *matrix, const uint32 *position, uint32 output, uint32 model);
uint32 ProjectOffsetScratchVertices(uint32 vertices, uint32 count, const sint32 position[3]);
uint32 ProjectWorldPoint(const uint32 *position, uint32 *screen);
uint32 ProjectOffsetModelVertices(uint32 vertices, uint32 count, const sint32 translation[3]);
uint32 ProjectAndClipWorldLine(uint32 line);
void TransformPackedMatrixForProjection(uint32 input, uint32 output);
void ScaleProjectionIr(uint32 scale);
void AccumulateProjectionVector(uint32 vector, uint32 scale);
uint32 ConfigureProjectionDepth(uint32 limit, uint32 base, uint32 extent);
void SetProjectionScreenParameters(uint32 x, uint32 y, uint32 depth);
#endif
