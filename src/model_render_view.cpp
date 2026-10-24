#include <string.h>
#include <stdlib.h>

#include "model_render_view.h"
#include "model_instance.h"
#include "psx.h"
#include "hd.h"
#include "global.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_lighting.h"
#include "resource_script.h"
#include "character.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "model_render.h"
#include "model_matrix.h"
#include "model_subdivision.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "model_clipping.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "texture_resources.h"
#include "model_render_view_internal.h"

static ModelRenderView model_render_view;
static ModelRenderView *configured_model_render_view;

ModelRenderView &GetModelRenderView()
{
    return model_render_view;
}

ModelRenderView &GetConfiguredModelRenderView()
{
    return *configured_model_render_view;
}

void SetConfiguredModelRenderView(ModelRenderView &view)
{
    configured_model_render_view = &view;
}

MATRIX *ModelRenderView::SetTranslation()
{
    VECTOR pos;
    pos.vx = this->pos.x;
    pos.vy = this->pos.y;
    pos.vz = this->pos.z;
    pos.pad = 0;
    return TransMatrix(&rotation, &pos);
}

uint32 PackModelRotationWord(const MATRIX &matrix, uint32 idx)
{
    // Callers select one of the five GTE rotation-control words
    if (idx == 4)
        return (uint16)matrix.m[2][2];
    uint32 component = idx * 2;
    uint32 low = (uint16)matrix.m[component / 3][component % 3];
    ++component;
    return low | ((uint32)(uint16)matrix.m[component / 3][component % 3] << 16);
}

ModelClipPlane model_clip_planes[8];
MATRIX model_clip_matrix[2];

static uint32 PackClipPlaneHalves(sint16 low, sint16 high)
{
    return (uint16)low | ((uint32)(uint16)high << 16);
}

static void SetModelDrawArea(const ModelViewport &bounds, uint32 *ordering_table)
{
    PSX_RECT rect;
    FUNCTION_MARKER(0x8007E57C, "SLUS_003.73");
    rect.x = bounds.left;
    rect.y = bounds.top + scene_render.cur->draw.clip.y;
    rect.w = bounds.right - bounds.left;
    rect.h = bounds.bottom - bounds.top;
    DR_AREA *draw_area = (DR_AREA *)scene_render.cursor;
    SetDrawArea(draw_area, &rect);
    setaddr(draw_area, getaddr(ordering_table + 4095));
    setaddr(ordering_table + 4095, gpu_packet_link((uintptr_t)draw_area));
    scene_render.cursor = (uint8 *)(draw_area + 1);
}

static void RotateRenderViewNormal(const MATRIX &matrix, const ModelClipPlane &input, sint16 output[3])
{
    uint32 words[5], axis;
    ShortVector3 result;
    for (axis = 0; axis < 5; ++axis)
        words[axis] = PackModelRotationWord(matrix, axis);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_write_data(0, PackClipPlaneHalves(input.n.x, input.n.y));
    xport_gte_write_data(1, PackClipPlaneHalves(input.n.z, input.offset));
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; ++axis)
        result[axis] = xport_gte_read_data(9 + axis);
    for (axis = 0; axis < 3; ++axis)
        output[axis] = result[axis];
}

static void ConfigureModelClipPlanes(const ModelRenderView &camera, const ModelViewport &bounds)
{
    uint32 len, axis;
    uint32 depth = bounds.distance;
    sint32 difference;
    model_clip_planes[0].n.x = 0;
    model_clip_planes[0].n.y = 0;
    model_clip_planes[0].n.z = -ONE;
    model_clip_planes[0].offset = bounds.far_depth;
    model_clip_planes[1].n.x = 0;
    model_clip_planes[1].n.y = 0;
    model_clip_planes[1].n.z = ONE;
    model_clip_planes[1].offset = -bounds.near_depth;

    difference = bounds.center_y - bounds.top;
    len = LookupDistanceRoot((uint32)difference * difference + depth * depth);
    model_clip_planes[2].n.x = 0;
    model_clip_planes[2].n.y = DivideSignedWord(bounds.distance * ONE, len);
    model_clip_planes[2].n.z = DivideSignedWord(difference * ONE, len);
    model_clip_planes[2].offset = 0;

    difference = bounds.bottom - bounds.center_y;
    len = LookupDistanceRoot((uint32)difference * difference + depth * depth);
    model_clip_planes[3].n.x = 0;
    model_clip_planes[3].n.y = DivideSignedWord(-bounds.distance * ONE, len);
    model_clip_planes[3].n.z = DivideSignedWord(difference * ONE, len);
    model_clip_planes[3].offset = 0;

    difference = bounds.center_x - bounds.left;
    len = LookupDistanceRoot((uint32)difference * 25 * difference + depth * (depth << 6));
    model_clip_planes[4].n.y = 0;
    model_clip_planes[4].n.x = DivideSignedWord(bounds.distance << 15, len);
    model_clip_planes[4].n.z = DivideSignedWord(difference * 5 * ONE, len);
    model_clip_planes[4].offset = 0;

    difference = bounds.right - bounds.center_x;
    len = LookupDistanceRoot((uint32)difference * 25 * difference + depth * (depth << 6));
    model_clip_planes[5].n.y = 0;
    model_clip_planes[5].n.x = DivideSignedWord(-(bounds.distance << 15), len);
    model_clip_planes[5].n.z = DivideSignedWord(difference * 5 * ONE, len);
    model_clip_planes[5].offset = 0;

    difference = bounds.center_x - bounds.left;
    len = LookupDistanceRoot((uint32)difference * difference + depth * depth);
    model_clip_planes[6].n.y = 0;
    model_clip_planes[6].n.x = DivideSignedWord(bounds.distance * ONE, len);
    model_clip_planes[6].n.z = DivideSignedWord(difference * ONE, len);
    model_clip_planes[6].offset = 0;

    difference = bounds.right - bounds.center_x;
    len = LookupDistanceRoot((uint32)difference * difference + depth * depth);
    model_clip_planes[7].n.y = 0;
    model_clip_planes[7].n.x = DivideSignedWord(-bounds.distance * ONE, len);
    model_clip_planes[7].n.z = DivideSignedWord(difference * ONE, len);
    model_clip_planes[7].offset = 0;
    for (axis = 0; axis < 3; ++axis)
    {
        RotateRenderViewNormal(camera.rotation, model_clip_planes[axis], model_clip_matrix[0].m[axis]);
        RotateRenderViewNormal(camera.rotation, model_clip_planes[axis + 3], model_clip_matrix[1].m[axis]);
    }
}

uint32 ModelRenderView::Configure(ModelViewport &bounds, uint32 *ordering_table)
{
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    const ModelRenderView *owner;
    uint32 axis, result;
    uint32 animation_bank, next_tick;
    sint32 water_y;
    uint16 view_y;
    FUNCTION_MARKER(0x8007E63C, "SLUS_003.73");
    ConfigureModelClipPlanes(*this, bounds);
    SetGeomScreen(bounds.distance);
    SetGeomOffset(bounds.center_x, bounds.center_y);
    SetModelDrawArea(bounds, ordering_table);
    SetConfiguredModelRenderView(*this);
    model_render_state.viewport = &bounds;
    model_render_state.ordering_table = ordering_table;
    bounds.near_depth = proj_cfg.near_depth;
    bounds.far_depth = proj_cfg.far_depth;
    TransposeRotationMatrix(rotation, inverse);
    for (axis = 0; axis < 3; ++axis)
    {
        sint32 scaled = 8 * inverse.m[0][axis];
        projection.m[1][axis] = inverse.m[1][axis];
        projection.m[2][axis] = inverse.m[2][axis];
        projection.m[0][axis] = scaled / 5;
    }
    const TextureResource &tex = TextureResource::Get(0xCCE0187C);
    animation_bank = ModelBank::GetActive(0);
    model_render_state.reflection_clut = (uint32)tex.clut << 16;
    model_render_state.reflection_tpage = (uint32)tex.tpage << 16;
    next_tick = game_clock.active_frame;
    model_render_state.previous_frame = model_render_state.frame;
    model_render_state.frame = next_tick;
    model_render_state.reflection_uv = tex.uv[0].uv;
    model_render_state.reflection_region = tex.region;
    ModelBank::UpdateColors(animation_bank);
    ModelBank::UpdateColors(ModelBank::GetActive(1));
    ModelBank::UpdateColors(GetRecModelBankSlot());
    ModelBank::UpdateTexCoords(GetRecModelBankSlot());
    owner = &GetConfiguredModelRenderView();
    model_render_state.water_plane.normal.x = -owner->rotation.m[1][0];
    model_render_state.water_plane.normal.y = -owner->rotation.m[1][1];
    model_render_state.water_plane.normal.z = -owner->rotation.m[1][2];
    water_y = level_environment.water_y;
    view_y = owner->pos.y;
    result = (water_y >> 12) - view_y;
    model_render_state.water_plane.distance = result;
    return result;
}

void LoadModelLineClipMatrices()
{
    const ModelClipPlane *p = model_clip_planes;
    xport_gte_write_control(8, PackClipPlaneHalves(p[0].n.x, p[0].n.y));
    xport_gte_write_control(9, PackClipPlaneHalves(p[0].n.z, p[1].n.x));
    xport_gte_write_control(10, PackClipPlaneHalves(p[1].n.y, p[1].n.z));
    xport_gte_write_control(11, PackClipPlaneHalves(p[2].n.x, p[2].n.y));
    xport_gte_write_control(12, (uint16)p[2].n.z);
    xport_gte_write_control(13, p[0].offset);
    xport_gte_write_control(14, p[1].offset);
    xport_gte_write_control(15, 0);
    xport_gte_write_control(16, PackClipPlaneHalves(p[3].n.x, p[3].n.y));
    xport_gte_write_control(17, PackClipPlaneHalves(p[3].n.z, p[6].n.x));
    xport_gte_write_control(18, PackClipPlaneHalves(p[6].n.y, p[6].n.z));
    xport_gte_write_control(19, PackClipPlaneHalves(p[7].n.x, p[7].n.y));
    xport_gte_write_control(20, (uint16)p[7].n.z);
}

void ModelRenderView::CaptureHd(HD_CAMERA &camera, const ModelViewport &bounds) const
{
    MATRIXF world;
    angles.BuildRotationMatrixf(world);
    IdentityMatrixf(&camera.view);
    TransposeMatrixf(&world, &camera.view);
    VECTORF negative = {-(float)pos.x, -(float)pos.y, -(float)pos.z};
    VECTORF translated;
    ApplyMatrixf(&camera.view, &negative, &translated);
    TransMatrixf(&camera.view, &translated);
    hd_projection(&camera, bounds.distance, bounds.right - bounds.left, bounds.bottom - bounds.top, bounds.center_x - bounds.left, bounds.center_y - bounds.top, bounds.near_depth, bounds.far_depth);
}
