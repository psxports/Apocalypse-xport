#ifndef MODEL_PROJECTION_H
#define MODEL_PROJECTION_H
extern "C"
{
#include "psx.h"
}
#include "model_geometry.h"
#include "vector3.h"

static const uint32 model_projection_vertex_count = 1000;
static const uint32 model_scratch_vertex_count = 128;

struct ProjectedWorldLine
{
    XY scr[2];
    sint32 depth[2];
};

struct ModelProjectedVertex
{
    // Projection variants embed clip flags in the packed screen word
    uint32 scr;
    // Low halfword is depth, high halfword holds clip and cache flags
    uint32 depth_flags;
    // Packed pre-projection GTE IR1 and IR2 components
    uint32 xy;
};

void ResetModelProjVertices();
ModelProjectedVertex &GetModelProjVertex(uint32 idx);

struct ModelScratchVertex
{
    // Packed screen coordinates include the original clipping bits
    uint32 scr;
    sint16 depth;
};

void ResetScratchProjVertices();
ModelScratchVertex &GetScratchProjVertex(uint32 idx);

struct ModelProjectionConfig
{
    uint32 far_depth;
    uint32 fog_start;
    uint32 fog_shift;
    uint32 max_distance;

    struct ScreenParameters
    {
        uint32 x, y, depth;
    } scr;

    uint32 near_depth;
};

const ModelProjectionConfig &GetModelProjConfig();
void UpdateModelWaterClipFlags(uint32 count);
uint32 ProjectModelScratchVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectAndClipModelVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectModelClippedVertices(const SVECTOR *vertices, uint32 count);
uint32 ProjectModelReferenceVertices(const SVECTOR *input, uint32 count);
void LoadProjIr(const ShortVector3 &vector);
void StoreProjIr(ShortVector3 &vector);
void CrossProjIr(const ShortVector3 &vector);
void LoadModelRotationAndTransformIr(const MATRIX &matrix);
uint32 MeasureProjIrLen();
uint32 NormalizeProjIr();
void GenerateModelReflectionCoords(const MATRIX &matrix, const Vector3 *pos, const ModelGeometry &model);
uint32 ProjectOffsetScratchVertices(const SVECTOR *vertices, uint32 count, const Vector3 *pos);
sint32 ProjectWorldPoint(const Vector3 *pos, XY *scr);
uint32 ProjectOffsetModelVertices(const SVECTOR *vertices, uint32 count, const Vector3 *translation);
sint32 ProjectAndClipWorldLine(const Vector3 &start, const Vector3 &end, ProjectedWorldLine &proj);
void ScaleProjIr(uint32 scale);
uint32 ConfigureProjDepth(uint32 limit, uint32 base, uint32 extent);
void SetProjScrParameters(uint32 x, uint32 y, uint32 depth);
#endif
