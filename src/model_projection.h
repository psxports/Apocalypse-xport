#ifndef MODEL_PROJECTION_H
#define MODEL_PROJECTION_H
extern "C"
{
#include "psx.h"
}
#include "model_geometry.h"
#include "vector3.h"

static const uint32 MODEL_PROJECTION_VERTEX_COUNT = 1000;
static const uint32 MODEL_SCRATCH_VERTEX_COUNT = 128;

struct ProjectedWorldLine
{
    uint32 screen[2];
    uint32 depth[2];
};

struct ModelProjectedVertex
{
    uint32 screen;
    uint32 depth_flags;
    uint32 xy;
};

void ResetModelProjectionVertices();
ModelProjectedVertex &GetModelProjectionVertex(uint32 index);

struct ModelScratchVertex
{
    uint32 screen;
    sint16 depth;
};

void ResetScratchProjectionVertices();
ModelScratchVertex &GetScratchProjectionVertex(uint32 index);

struct ModelLightingValue;

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
void UpdateModelWaterClipFlags(uint32 count);
uint32 ProjectModelScratchVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectAndClipModelVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectModelClippedVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectModelReferenceVertices(const SVECTOR *input, uint32 count);
void LoadModelRotationAndTransformIr(const MATRIX &matrix);
void LoadPackedProjectionIr(const ModelLightingValue &value);
uint32 MeasureProjectionIrLength();
uint32 NormalizeProjectionIr();
void GenerateModelNormalColors(const SVECTOR *vertices, uint32 count);
void RenderModelProjectionEffect(const ModelGeometry &model, const MATRIX &matrix, const SVECTOR &vector, const uint32 *position, const MATRIX &direction, uint32 color);
void GenerateModelReflectionCoordinates(const MATRIX &matrix, const uint32 *position, const ModelGeometry &model);
uint32 ProjectOffsetScratchVertices(const SVECTOR *vertices, uint32 count, const sint32 position[3]);
uint32 ProjectWorldPoint(const uint32 *position, uint32 *screen);
uint32 ProjectOffsetModelVertices(const SVECTOR *vertices, uint32 count, const sint32 translation[3]);
uint32 ProjectAndClipWorldLine(const Vector3 &start, const Vector3 &end, ProjectedWorldLine &projected);
void ScaleProjectionIr(uint32 scale);
void AccumulateProjectionVector(const SVECTOR &vector, uint32 scale);
uint32 ConfigureProjectionDepth(uint32 limit, uint32 base, uint32 extent);
void SetProjectionScreenParameters(uint32 x, uint32 y, uint32 depth);
#endif
