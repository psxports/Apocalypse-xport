#ifndef MODEL_PROJECTION_H
#define MODEL_PROJECTION_H
extern "C"
{
#include "psx.h"
}
#include "model_geometry.h"
struct ModelProjectionConfig
{
    uint32 far_depth;
    uint32 fog_start;
    uint32 fog_shift;
    uint32 max_distance;
    uint32 scr[3];
    uint32 near_depth;
};

const ModelProjectionConfig &GetModelProjectionConfig();
uint32 UpdateProjectedVertexDepthFlags(uint32 count);
uint32 ProjectModelScratchVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectModelClippedVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectModelReferenceVertices(const SVECTOR *input, uint32 count);
void LoadPackedProjectionIr(uint32 geometry);
uint32 MeasureProjectionIrLength();
uint32 NormalizeProjectionIr();
void CrossProjectionIr(const uint32 *vector);
void TransformProjectionIr(const uint32 *matrix);
void GenerateModelNormalColors(const SVECTOR *vertices, uint32 count);
void RenderModelProjectionEffect(const ModelGeometry &model, const uint32 *matrix, uint32 vector, uint32 position, uint32 direction, uint32 color);
uint32 GenerateModelReflectionCoordinates(const uint32 *matrix, const uint32 *position, uint32 output, const ModelGeometry &model);
uint32 ProjectOffsetScratchVertices(const SVECTOR *vertices, uint32 count, const sint32 position[3]);
uint32 ProjectWorldPoint(const uint32 *position, uint32 *screen);
uint32 ProjectOffsetModelVertices(const SVECTOR *vertices, uint32 count, const sint32 translation[3]);
uint32 ProjectAndClipWorldLine(uint32 line);
void TransformPackedMatrixForProjection(uint32 input, uint32 output);
void ScaleProjectionIr(uint32 scale);
void AccumulateProjectionVector(uint32 vector, uint32 scale);
uint32 ConfigureProjectionDepth(uint32 limit, uint32 base, uint32 extent);
void SetProjectionScreenParameters(uint32 x, uint32 y, uint32 depth);
#endif
