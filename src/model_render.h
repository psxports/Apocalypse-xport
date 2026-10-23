#ifndef MODEL_RENDER_H
#define MODEL_RENDER_H
extern "C"
{
#include "psx.h"
}
#include "model_render_view.h"
#include "model_geometry.h"
#include "model_instance.h"
#include "vector3.h"

struct ModelViewport
{
    uint16 right;
    uint16 bottom;
    uint16 left;
    uint16 top;
    uint16 near_depth;
    uint16 far_depth;
    uint16 center_x;
    uint16 center_y;
    uint16 distance;
};

extern ModelViewport model_viewport;

struct ModelDrawContext
{
    uint32 mode;
    uint32 fog_start;
    uint32 fog_shift;
    uint32 clut;
    uint32 tpage;
    Vector3 light;
    uint32 near_bias;
    uint32 far_bias;
};

struct ModelWaterPlane
{
    ShortVector3 normal;
    sint16 distance;
};

struct ModelRenderState
{
    bool translated;
    uint32 pass_count;
    uint32 previous_frame;
    uint32 effect_frame;
    uint32 effect_pass;
    bool part_hidden;
    bool water_clip_enabled;
    uint32 frame;
    bool use_scratch;
    ModelViewport *viewport;
    uint32 *ordering_table;
    ModelWaterPlane water_plane;
    uint32 reflection_clut;
    uint32 reflection_tpage;
    uint32 reflection_uv;
    uint32 scratch_cache_idx;
    uint32 cache_index;
    uint32 generated_vertex_idx;
    bool generated_cursor_valid;
    uint32 invalid_generated_cursor;
    ModelDrawContext draw;
    sint16 depth_bias[2];
};

extern ModelRenderState model_render_state;
void ResetModelRenderState();

void EmitScratchModelPackets(const ModelPolygon *desc, uint32 count);

uint32 AdvanceModelRenderCounter();
void LoadModelLineClipMatrices();
#endif
