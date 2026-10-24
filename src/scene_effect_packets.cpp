#include <stdlib.h>
#include <string.h>
#include "texture_resources.h"
#include "game_diagnostics.h"
#include "psx.h"
#include "model_render.h"
#include "scene_lifecycle.h"
#include "model_render_view.h"
#include "scene_effect_packets.h"
#include "effect_object.h"
#include "line_effect.h"
#include "renderable_effect.h"
#include "primitive_particle.h"
#include "opaque_fragment.h"
#include "textured_fragment.h"
#include "textured_triangle.h"
#include "effect_lifecycle.h"
#include "sprite.h"
#include "endpoint_sprite.h"
#include "sprite_trail.h"
#include "textured_quad.h"
#include "ring_effect.h"
#include "point_segments.h"
#include "segment_effect.h"
#include "model_projection.h"
#include "global.h"
#include "hd.h"
#include "model_bank.h"
#include "model_attachment_data.h"
#include "game_tick.h"

static void CaptureHdEffectAttachment(HD_ATTACHMENT &attachment, const EffectObject &source)
{
    const ModelInstance *model = source.hd_attachment_owner.GetObject();
    if (!model || !model->hd_history || (source.hd_attachment_birth_only && source.hd_attachment_tick != game_clock.tick))
        return;
    const ModelAttachmentTable *table = ModelBank::Get(model->bank).attachments;
    if (!table || !table->count)
        return;
    const ModelAttachmentPoint &point = table->points[0];
    attachment.owner = model->hd_history;
    attachment.part = point.bone;
    attachment.point.vx = point.x;
    attachment.point.vy = point.y;
    attachment.point.vz = point.z;
    attachment.reference.vx = source.hd_attachment_origin.x / 4096.0f;
    attachment.reference.vy = source.hd_attachment_origin.y / 4096.0f;
    attachment.reference.vz = source.hd_attachment_origin.z / 4096.0f;
    if (source.hd_attachment_birth_only)
    {
        if (!source.hd_release_ready)
        {
            VECTORF socket;
            if (!hd_model_history_point(model->hd_history, point.bone, &attachment.point, &socket))
            {
                attachment.owner = 0;
                return;
            }
            source.hd_release_offset.vx = socket.vx - attachment.reference.vx;
            source.hd_release_offset.vy = socket.vy - attachment.reference.vy;
            source.hd_release_offset.vz = socket.vz - attachment.reference.vz;
            source.hd_release_ready = true;
        }
        attachment.reference.vx += source.hd_release_offset.vx;
        attachment.reference.vy += source.hd_release_offset.vy;
        attachment.reference.vz += source.hd_release_offset.vz;
    }
}

static uint32 CaptureHdEffectFlags(uint32 color)
{
    return ((color & 0x1000000) ? HD_RAW_TEXTURE : 0) | ((color & 0x2000000) ? HD_SEMITRANSPARENT : 0);
}

static void CaptureHdEffectPoint(HD_EFFECT_VERTEX &vertex, const Vector3 &pos, uint32 color, float scale)
{
    hd_effect_point(&vertex.pos, pos.x * scale, pos.y * scale, pos.z * scale);
    vertex.color = color;
}

static void CaptureHdEffectTexture(HD_EFFECT &effect, const TextureResource &tex)
{
    effect.page = tex.tpage;
    effect.clut = tex.clut;
    effect.flags |= HD_TEXTURED | HD_FILTERED | tex.HdTexelFlags(tex.clut);
    effect.texture_region = tex.region;
    for (uint32 corner = 0; corner < 4; ++corner)
    {
        effect.vertices[corner].u = tex.uv[corner].u;
        effect.vertices[corner].v = tex.uv[corner].v;
    }
}

static void CaptureHdLinePoint(VECTORF &point, const Vector3 &source)
{
    point.vx = source.x / 4096.0f;
    point.vy = source.y / 4096.0f;
    point.vz = source.z / 4096.0f;
}

static bool CaptureHdSceneEffects(bool logic)
{
    const ModelProjectionConfig &cfg = GetModelProjConfig();
    uint16 page = EffectObject::GetDrawMode().code[0] & 0x1FF;
    for (EffectObject *node = effect_lists.rings; node; node = node->next)
    {
        const RingEffect *source = static_cast<const RingEffect *>(node);
        uint32 count = source->point_count * (source->row_count + 1);
        HD_RING_POINT *points = hd_effect_ring_points(count);
        if (!points)
            return false;
        for (uint32 index = 0; index < count; ++index)
        {
            const RingPoint &point = index < source->point_count ? source->ring_points[index] : source->row_points[index - source->point_count];
            points[index].distance = point.distance;
            points[index].color = point.color;
        }
        HD_RING ring = {0};
        CaptureHdEffectAttachment(ring.attachment, *source);
        CaptureHdLinePoint(ring.center, source->pos);
        ring.points = points;
        ring.count = source->point_count;
        ring.rows = source->row_count;
        ring.mask = source->seg_mask;
        ring.color = source->center_color;
        ring.angle = RAD12 * (source->angle & 4095);
        ring.step = RAD12 * (sint16)source->angle_step;
        ring.page = page;
        ring.hide_center = source->hide_center;
        if (!(logic ? hd_ring_tick(&source->hd_history, &ring) : hd_ring_capture(&source->hd_history, &ring)))
            return false;
    }
    for (EffectObject *node = effect_lists.segments; node; node = node->next)
    {
        const SegmentEffect *segments = static_cast<const SegmentEffect *>(node);
        HD_LINE_SAMPLE *lines = hd_line_samples(segments->point_count);
        if (segments->point_count && !lines)
            return false;
        for (sint32 index = 0; index < segments->point_count; ++index)
        {
            HD_LINE_SAMPLE &line = lines[index];
            line = HD_LINE_SAMPLE();
            line.page = page;
            CaptureHdLinePoint(line.points[0], index ? segments->points[index - 1].pos : segments->origin);
            CaptureHdLinePoint(line.points[1], segments->points[index].pos);
            line.colors[0] = index ? segments->points[index - 1].rgbc.rgbc : segments->rgbc.rgbc;
            line.colors[1] = segments->points[index].rgbc.rgbc;
            line.flags = CaptureHdEffectFlags(line.colors[0]);
            line.clip = 1;
            line.forced = segments->forced_depth;
            line.near_depth = cfg.near_depth;
            line.far_depth = cfg.max_distance;
        }
        if (!(logic ? hd_line_chain_tick(&segments->hd_history, lines, segments->point_count) : hd_line_chain_capture(&segments->hd_history, lines, segments->point_count)))
            return false;
    }
    for (EffectObject *node = effect_lists.points; node; node = node->next)
    {
        const PointSegments *points = static_cast<const PointSegments *>(node);
        HD_LINE_SAMPLE *lines = hd_line_samples(points->point_count);
        if (points->point_count && !lines)
            return false;
        for (sint32 index = 0; index < points->point_count; ++index)
        {
            uint32 color = points->points[index].rgbc.rgbc;
            HD_LINE_SAMPLE &line = lines[index];
            line = HD_LINE_SAMPLE();
            line.page = page;
            CaptureHdLinePoint(line.points[0], index ? points->points[index - 1].pos : points->origin);
            CaptureHdLinePoint(line.points[1], points->points[index].pos);
            line.colors[0] = line.colors[1] = color;
            line.flags = CaptureHdEffectFlags(color);
            line.clip = 1;
            line.forced = points->forced_depth;
            line.near_depth = cfg.near_depth;
            line.far_depth = cfg.max_distance;
        }
        if (!(logic ? hd_line_chain_tick(&points->hd_history, lines, points->point_count) : hd_line_chain_capture(&points->hd_history, lines, points->point_count)))
            return false;
    }
    for (EffectObject *node = effect_lists.sprs; node; node = node->next)
    {
        const Sprite *sprite = static_cast<const Sprite *>(node);
        const TextureFrame &frame = sprite->GetTexFrame();
        const TextureResource &tex = *frame.tex;
        HD_UI_IMAGE image = {0};
        VECTORF world = {sprite->pos.x / 4096.0f, sprite->pos.y / 4096.0f, sprite->pos.z / 4096.0f};
        image.page = (tex.tpage & ~0x60) | sprite->blend_mode;
        image.clut = sprite->clut ? sprite->clut : tex.clut;
        image.region = tex.region;
        image.width = frame.w;
        image.height = frame.h;
        image.offset_x = frame.ox;
        image.offset_y = frame.oy;
        for (uint32 corner = 0; corner < 4; ++corner)
            image.uv[corner] = tex.uv[corner].uv;
        HD_SPRITE_SAMPLE sample = {0};
        CaptureHdEffectAttachment(sample.attachment, *sprite);
        sample.pos = world;
        sample.pos.vx += sprite->hd_release_offset.vx;
        sample.pos.vy += sprite->hd_release_offset.vy;
        sample.pos.vz += sprite->hd_release_offset.vz;
        sample.image = image;
        sample.color = sprite->color;
        sample.flags = CaptureHdEffectFlags(sprite->color) | HD_FILTERED | tex.HdTexelFlags(image.clut);
        sample.size = (sint16)sprite->size;
        sample.scale_x = sprite->scale[0] / 4096.0f;
        sample.scale_y = sprite->scale[1] / 4096.0f;
        sample.angle = RAD12 * (sprite->angle & 4095);
        sample.flip = sprite->flags & 1;
        sample.bias = sprite->depth_bias;
        sample.forced = sprite->forced_depth;
        sample.near_depth = (logic ? model_viewport.distance : model_render_state.viewport->distance) / 2.0f;
        sample.far_depth = cfg.max_distance;
        sample.large = sprite->flags & 8;
        if (!(logic ? hd_sprite_tick(&sprite->hd_history, &sample) : hd_sprite_capture(&sprite->hd_history, &sample)))
            return false;
    }
    for (EffectObject *node = effect_lists.opaque_fragments; node; node = node->next)
    {
        const OpaqueFragment *fragment = static_cast<const OpaqueFragment *>(node);
        HD_POLYGON_SAMPLE sample = {0};
        HD_EFFECT &effect = sample.effect;
        sample.present = 1;
        effect.count = 3;
        effect.space = HD_EFFECT_WORLD;
        effect.page = page;
        effect.flags = HD_SEMITRANSPARENT;
        for (uint32 corner = 0; corner < 3; ++corner)
        {
            CaptureHdLinePoint(effect.vertices[corner].pos, fragment->vertices[corner]);
            effect.vertices[corner].color = fragment->draw_color.rgbc;
        }
        effect.depth.points[0] = effect.vertices[2].pos;
        effect.depth.count = 1;
        effect.depth.rule = HD_DEPTH_MIN;
        if (!(logic ? hd_polygon_chain_tick(&fragment->hd_history, &sample, 1) : hd_polygon_chain_capture(&fragment->hd_history, &sample, 1)))
            return false;
    }
    for (EffectObject *node = effect_lists.lines; node; node = node->next)
    {
        const LineEffect *source = static_cast<const LineEffect *>(node);
        HD_LINE_SAMPLE line = {0};
        line.page = page;
        line.flags = CaptureHdEffectFlags(source->color[0]);
        for (uint32 corner = 0; corner < 2; ++corner)
        {
            line.points[corner].vx = source->endpoint[corner].x / 4096.0f;
            line.points[corner].vy = source->endpoint[corner].y / 4096.0f;
            line.points[corner].vz = source->endpoint[corner].z / 4096.0f;
            line.colors[corner] = source->color[corner];
        }
        line.near_depth = cfg.near_depth;
        line.far_depth = cfg.max_distance;
        if (!(logic ? hd_line_tick(&source->hd_history, &line) : hd_line_capture(&source->hd_history, &line)))
            return false;
    }
    for (EffectObject *node = effect_lists.quads; node; node = node->next)
    {
        const TexturedQuad *source = static_cast<const TexturedQuad *>(node);
        if (!source->GetTex())
        {
            hd_polygon_chain_reset(source->hd_history);
            continue;
        }
        HD_POLYGON_SAMPLE sample = {0};
        HD_EFFECT &effect = sample.effect;
        sample.present = 1;
        sample.bias = source->depth_bias;
        effect.count = 4;
        effect.space = HD_EFFECT_WORLD;
        effect.flags = CaptureHdEffectFlags(source->color);
        CaptureHdEffectTexture(effect, *source->GetTex());
        if (source->flags & 4)
            effect.page = (effect.page & ~0x60) | 0x40;
        for (uint32 corner = 0; corner < 4; ++corner)
        {
            CaptureHdLinePoint(effect.vertices[corner].pos, corner ? source->corners[corner - 1] : source->pos);
            effect.vertices[corner].color = source->color;
            effect.depth.points[corner] = effect.vertices[corner].pos;
        }
        effect.depth.count = 4;
        effect.depth.rule = HD_DEPTH_MAX;
        if (!(logic ? hd_polygon_chain_tick(&source->hd_history, &sample, 1) : hd_polygon_chain_capture(&source->hd_history, &sample, 1)))
            return false;
    }
    for (EffectObject *node = effect_lists.textured_triangles; node; node = node->next)
    {
        static const uint8 faces[4][3] = {{0, 1, 2}, {0, 3, 1}, {1, 3, 2}, {2, 3, 0}};
        const TexturedTriangle *source = static_cast<const TexturedTriangle *>(node);
        VECTORF points[4];
        HD_POLYGON_SAMPLE samples[4] = {{0}};
        for (uint32 corner = 0; corner < 4; ++corner)
        {
            points[corner].vx = (float)source->transformed[corner].x;
            points[corner].vy = (float)source->transformed[corner].y;
            points[corner].vz = (float)source->transformed[corner].z;
        }
        for (uint32 face = 0; face < 4; ++face)
        {
            HD_EFFECT &effect = samples[face].effect;
            samples[face].present = 1;
            uint32 color = face ? source->colors[face - 1].rgbc : source->base_color.rgbc;
            const uint16 uv[3] = {(uint16)source->uv_clut, (uint16)source->uv_tpage, source->uv_tail};
            effect.count = 3;
            effect.space = HD_EFFECT_WORLD;
            effect.flags = HD_TEXTURED | CaptureHdEffectFlags(color);
            effect.clut = source->uv_clut >> 16;
            effect.page = source->uv_tpage >> 16;
            effect.window = source->texture_window;
            effect.texture_region = source->texture_region;
            effect.flags |= HD_FILTERED;
            effect.cull_positive = 1;
            for (uint32 corner = 0; corner < 3; ++corner)
            {
                HD_EFFECT_VERTEX &vertex = effect.vertices[corner];
                vertex.pos = points[faces[face][corner]];
                vertex.color = color;
                vertex.u = uv[corner] & 255;
                vertex.v = uv[corner] >> 8;
            }
            memcpy(effect.depth.points, points, sizeof(points));
            effect.depth.count = 4;
            effect.depth.rule = HD_DEPTH_MAX;
        }
        if (!(logic ? hd_polygon_chain_tick(&source->hd_history, samples, 4) : hd_polygon_chain_capture(&source->hd_history, samples, 4)))
            return false;
    }
    for (EffectObject *node = effect_lists.particles; node; node = node->next)
    {
        const PrimitiveParticle *source = static_cast<const PrimitiveParticle *>(node);
        const TILE &tile = source->primitive;
        HD_SPRITE_SAMPLE sample = {0};
        sample.pos.vx = source->pos.x / 4096.0f;
        sample.pos.vy = source->pos.y / 4096.0f;
        sample.pos.vz = source->pos.z / 4096.0f;
        sample.flags = CaptureHdEffectFlags(tile.rgbc.rgbc);
        sample.image.page = page;
        sample.color = tile.rgbc.rgbc;
        bool single = (tile.w & 15) == 1;
        sample.scale_x = single ? 1 : tile.w;
        sample.scale_y = single ? 1 : tile.h;
        sample.pixels = 1;
        sample.bias = source->depth_bias;
        sample.near_depth = cfg.near_depth;
        sample.far_depth = cfg.max_distance;
        if (!(logic ? hd_sprite_tick(&source->hd_history, &sample) : hd_sprite_capture(&source->hd_history, &sample)))
            return false;
    }
    HD_STRIP strip = {0};
    for (EffectObject *node = effect_lists.endpoint_sprs; node; node = node->next)
    {
        const EndpointSprite *source = static_cast<const EndpointSprite *>(node);
        if (source->hd_trail)
        {
            if (!source->hd_trail->CaptureHd(cfg.near_depth, cfg.max_distance, logic))
                return false;
            continue;
        }
        if (logic)
            continue;
        const TextureFrame &frame = source->GetTexFrame();
        HD_EFFECT effect = {0};
        effect.flags = CaptureHdEffectFlags(source->color);
        CaptureHdEffectTexture(effect, *frame.tex);
        for (uint32 corner = 0; corner < 4; ++corner)
            effect.vertices[corner].color = source->color;
        for (uint32 end = 0; end < 2; ++end)
            CaptureHdEffectPoint(effect.vertices[end], source->endpoint[end], source->color, 1.0f / 4096);
        if (!hd_effect_strip(&strip, &effect, (sint16)source->size, frame.w, source->flags, source->depth_bias, cfg.near_depth, cfg.max_distance))
            return false;
    }
    return true;
}

bool CaptureHdEffectLogic()
{
    for (EffectObject *node = effect_lists.renderables; node; node = node->next)
        if (node->IsAlive() && !static_cast<RenderableEffect *>(node)->CaptureHdTick())
            return false;
    return CaptureHdSceneEffects(true);
}

// Ribbon joins retain full signed32 coordinates until GPU packet publication
struct RibbonScreenPoint
{
    sint32 x, y;
};

struct RibbonEdge
{
    RibbonScreenPoint p[2];
};

static uint32 DivideEffectUnsigned(uint32 numerator, uint32 denominator)
{
    return denominator == 0 ? 0xFFFFFFFF : numerator / denominator;
}

// Native extraction of the original three halfword vertex stores
static void LoadEffectTriangleVertex(const Vector3 &pos, const Vector3 &view, uint32 register_base)
{
    uint32 x = (uint32)(pos.x >> 12) - (uint32)(uint16)view.comp[0];
    uint32 y = (uint32)(pos.y >> 12) - (uint32)(uint16)view.comp[1];
    uint32 z = (uint32)(pos.z >> 12) - (uint32)(uint16)view.comp[2];
    uint32 packed = (x & 0xFFFF) | ((y & 0xFFFF) << 16);
    xport_gte_write_data(register_base, packed);
    // VZ consumes only its signed low halfword in the existing GTE backend
    xport_gte_write_data(register_base + 1, (sint16)z);
}

// Native extraction preserves both bias reads around the packet tag store
static void LinkBiasedEffectPacket(uint8 *packet, const EffectObject &object, uint32 depth)
{
    uint32 bias = object.depth_bias;
    uint32 *ordering_table = scene_render.cur->ot;
    P_TAG *header = (P_TAG *)packet;
    uint32 *bucket = ordering_table + (((depth - bias) & 0x3FFC) >> 2);
    uint32 head = *bucket;
    header->addr = head;
    bias = object.depth_bias;
    bucket = ordering_table + (((depth - bias) & 0x3FFC) >> 2);
    head = *bucket;
    *bucket = (head & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
}

// Native extraction retains the second forced-depth byte read after linking
static void LinkForcedEffectPacket(uint8 *packet, const Sprite &spr, uint32 depth)
{
    uint32 *ordering_table = scene_render.cur->ot;
    P_TAG *header = (P_TAG *)packet;
    uint32 *bucket = ordering_table + depth;
    uint32 head = *bucket;
    header->addr = head;
    depth = spr.forced_depth;
    bucket = ordering_table + depth;
    head = *bucket;
    *bucket = (head & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
}

// Native extraction keeps the two OT head loads around the packet tag store
static uint32 LinkEffectPacket(uint8 *packet, uint32 *bucket)
{
    P_TAG *header = (P_TAG *)packet;
    uint32 head = *bucket;
    header->addr = head;
    head = *bucket;
    uint32 result = (head & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
    *bucket = result;
    return result;
}

// Native extraction caches both state words before their packet stores
static void LinkEffectStatePacket(uint8 *packet, uint32 *bucket)
{
    uint32 tag = EffectObject::GetDrawMode().tag;
    uint32 code = EffectObject::GetDrawMode().code[0];
    DR_TPAGE *state = (DR_TPAGE *)packet;
    state->tag = tag;
    state->code[0] = code;
    uint32 head = *bucket;
    state->tag = (tag & 0xFF000000) | (head & 0xFFFFFF);
    head = *bucket;
    *bucket = (head & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
}

void BuildSceneEffectPackets()
{
    ModelBorrow model_borrow;
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    FUNCTION_MARKER(0x8003736C, "SLUS_003.73");
    uint32 rotation_words[5];
    Vector3 view_pos;
    EffectObject *render_node;
    sint32 viewport_near_depth;
    rotation_words[0] = PackModelRotationWord(GetModelRenderView().projection, 0);
    rotation_words[1] = PackModelRotationWord(GetModelRenderView().projection, 1);
    xport_gte_write_control(0, rotation_words[0]);
    xport_gte_write_control(1, rotation_words[1]);
    rotation_words[2] = PackModelRotationWord(GetModelRenderView().projection, 2);
    rotation_words[3] = PackModelRotationWord(GetModelRenderView().projection, 3);
    rotation_words[4] = PackModelRotationWord(GetModelRenderView().projection, 4);
    xport_gte_write_control(2, rotation_words[2]);
    xport_gte_write_control(3, rotation_words[3]);
    xport_gte_write_control(4, rotation_words[4]);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    view_pos.x = GetModelRenderView().pos.x;
    view_pos.y = GetModelRenderView().pos.y;
    view_pos.z = GetModelRenderView().pos.z;
    viewport_near_depth = model_render_state.viewport->distance >> 1;
    render_node = effect_lists.renderables;
    for (; render_node; render_node = render_node->next)
    {
        if (render_node->IsAlive())
            static_cast<RenderableEffect *>(render_node)->Render();
    }

    if (hd_ui_collecting())
    {
        if (!CaptureHdSceneEffects(false))
        {
            GameDiagnostic(("Cannot capture HD scene effects\n"));
            abort();
        }
        return;
    }

    for (EffectObject *ring_node = effect_lists.rings; ring_node; ring_node = ring_node->next)
    {
        sint16 ring_vertex_x;
        uint32 ring_state_head;
        uint8 *state_packet;
        uint8 *state_end;
        uint32 state_code;
        uint32 near_fade;
        const RingPoint *ring_ray_end;
        uint32 *ot_bucket;
        uint32 seed_radius_proj;
        sint32 seed_x;
        sint32 seed_y;
        uint32 seed_row_idx;
        sint32 seed_radius;
        sint32 ray_idx;
        sint32 seed_row_proj;
        sint32 seed_row_x;
        sint32 seed_row_y;
        const RingPoint *ring_color;
        uint32 ray_radius_proj;
        sint32 ray_x;
        sint32 ray_y;
        uint32 row_idx;
        uint8 *tri_packet;
        uint8 *tri_end;
        uint32 center_color;
        sint32 row_radius;
        sint32 row_radius_proj;
        sint32 row_x;
        sint32 row_y;
        uint8 *quad_packet;
        uint8 *quad_end;
        uint32 row_color;
        uint32 prev_row_color;
        uint32 quad_tag;
        sint32 ray_count;
        sint32 angle_step;
        sint32 ray_angle;
        const RingPoint *ring_ray;
        sint32 s_scale;
        sint32 c_scale;
        bool seg_enabled;
        uint32 ring_row_stride;
        const RingPoint *ring_row;
        uint32 trig_angle;
        uint32 packed_vertex_xy;
        Vector3 ring_view_pos, ring_relative_pos, ring_world_pos;
        XY center_scr;
        const uint32 world_shift = 12;
        const RingEffect *ring = static_cast<RingEffect *>(ring_node);
        XY *prev_ray, *cur_ray, *write_pos, *cur_row, *prev_row, *swap_ray;
        const sint32 state_size = sizeof(DR_TPAGE);
        const sint32 tri_size = sizeof(POLY_G3);
        const sint32 quad_size = sizeof(POLY_G4);
        hd_ring_reset(ring->hd_history);
        ring_world_pos.ShiftRight(ring->pos, world_shift);
        ring_relative_pos.Subtract(ring_world_pos, view_pos);
        packed_vertex_xy = ((uint32)ring_relative_pos.x & 0xFFFF) | (((uint32)ring_relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_vertex_xy);
        xport_gte_write_data(1, (uint32)ring_relative_pos.z);
        xport_gte_execute(0x180001);
        state_packet = scene_render.cursor;
        uint8 *state_limit = scene_render.limit;
        if (state_limit - state_packet < state_size)
            return;
        state_end = state_packet + state_size;
        uint32 ring_state = EffectObject::GetDrawMode().tag;
        state_code = EffectObject::GetDrawMode().code[0];
        DR_TPAGE *state = (DR_TPAGE *)state_packet;
        state->tag = ring_state;
        state->code[0] = state_code;
        ray_angle = (sint16)ring->angle;
        ring_ray = ring->ring_points;
        ray_count = ring->point_count;
        scene_render.cursor = state_end;
        angle_step = (sint16)ring->angle_step;
        ring_view_pos.x = xport_gte_read_data(25);
        ring_view_pos.y = xport_gte_read_data(26);
        ring_view_pos.z = xport_gte_read_data(27);
        if (ring_view_pos.z >= 0 && ring_view_pos.z <= 20000)
        {
            near_fade = 0;
            if (ring_view_pos.z < 700)
                near_fade = 255 * ring_view_pos.z / 700 + 1;
            if (ring_view_pos.z >= viewport_near_depth)
            {
                center_scr.xy = xport_gte_read_data(14);
            }
            else
            {
                const ModelViewport *ring_view = model_render_state.viewport;
                uint32 proj_distance = ring_view->distance;
                uint32 scaled_view_x = proj_distance * (uint32)ring_view_pos.x;
                sint32 proj_x = DivideSignedWord(scaled_view_x, ring_view_pos.z);
                uint32 center_x_word = (uint32)ring_view->center_x + proj_x;
                center_scr.x = center_x_word;
                if (center_scr.x >= 1024)
                    center_scr.x = 1023;
                else if (center_scr.x < -1024)
                    center_scr.x = -1024;
                ring_view = model_render_state.viewport;
                proj_distance = ring_view->distance;
                uint32 scaled_view_y = proj_distance * (uint32)ring_view_pos.y;
                sint32 proj_y = DivideSignedWord(scaled_view_y, ring_view_pos.z);
                uint32 center_y_word = (uint32)ring_view->center_y + proj_y;
                center_scr.y = center_y_word;
                if (center_scr.y >= 1024)
                    center_scr.y = 1023;
                else if (center_scr.y < -1024)
                    center_scr.y = -1024;
            }
            if (center_scr.x != -1024 && center_scr.x != 1023)
            {
                if (center_scr.y != -1024 && center_scr.y != 1023)
                {
                    prev_ray = ring->proj_rows[0];
                    trig_angle = ray_angle & 0xFFF;
                    cur_ray = ring->proj_rows[1];

                    ring_ray_end = ring_ray + ray_count;
                    sint32 ring_sine = rsin(trig_angle);
                    ot_bucket = scene_render.cur->ot + (((uint32)ring_view_pos.z & 0x3FFC) >> 2);
                    seed_radius_proj = DivideEffectUnsigned(400 * ring_ray_end[-1].distance, ring_view_pos.z);
                    s_scale = (400 * ring_sine) / 256;
                    c_scale = rcos(trig_angle);
                    seed_x = center_scr.x + (sint32)((uint32)seed_radius_proj * (uint32)s_scale) / 4096;
                    seed_y = center_scr.y - (sint32)((uint32)seed_radius_proj * (uint32)c_scale) / 4096;
                    if (seed_x > 612)
                        seed_x = 612;
                    if (seed_x < -100)
                        seed_x = -100;
                    if (seed_y > 340)
                        seed_y = 340;
                    if (seed_y < -100)
                        seed_y = -100;
                    seed_row_idx = 0;
                    write_pos = prev_ray;
                    prev_ray[0].x = seed_x;
                    prev_ray[0].y = seed_y;
                    ring_row = ring->row_count ? ring->row_points + ray_count - 1 : 0;
                    seed_radius = (sint32)(ring_ray_end[-1].distance);
                    while (1)
                    {
                        ray_idx = 0;
                        if (seed_row_idx >= ring->row_count)
                            break;
                        seed_radius = (uint32)seed_radius + ring_row->distance;
                        seed_row_proj = DivideSignedWord(400 * (uint32)seed_radius, ring_view_pos.z);
                        seed_row_x = center_scr.x + (sint32)((uint32)seed_row_proj * (uint32)s_scale) / 4096;
                        seed_row_y = center_scr.y - (sint32)((uint32)seed_row_proj * (uint32)c_scale) / 4096;
                        if (seed_row_x > 612)
                            seed_row_x = 612;
                        if (seed_row_x < -100)
                            seed_row_x = -100;
                        if (seed_row_y > 340)
                            seed_row_y = 340;
                        if (seed_row_y < -100)
                            seed_row_y = -100;
                        ++seed_row_idx;
                        if (seed_row_idx < ring->row_count)
                            ring_row += ray_count;
                        write_pos[1].x = seed_row_x;
                        ++write_pos;
                        write_pos[0].y = seed_row_y;
                    }

                    ray_angle = (uint32)ray_angle + angle_step;
                    ring_color = ring_ray;
                    ring_row_stride = ray_count;
                    while (ray_idx < ray_count)
                    {
                        seg_enabled = (ring->seg_mask >> (ray_idx & 31)) & 1;
                        if (ray_idx == ray_count - 1)
                            ray_angle = (sint16)ring->angle;
                        trig_angle = ray_angle & 0xFFF;
                        sint32 ray_sine = rsin(trig_angle);
                        ray_radius_proj = DivideEffectUnsigned(400 * ring_ray->distance, ring_view_pos.z);
                        s_scale = (400 * ray_sine) / 256;
                        c_scale = rcos(trig_angle);
                        ray_x = center_scr.x + (sint32)((uint32)ray_radius_proj * (uint32)s_scale) / 4096;
                        ray_y = center_scr.y - (sint32)((uint32)ray_radius_proj * (uint32)c_scale) / 4096;
                        if (ray_x > 612)
                            ray_x = 612;
                        if (ray_x < -100)
                            ray_x = -100;
                        if (ray_y > 340)
                            ray_y = 340;
                        if (ray_y < -100)
                            ray_y = -100;
                        cur_ray[0].x = ray_x;
                        cur_ray[0].y = ray_y;
                        row_idx = 0;
                        if (!ring->hide_center && seg_enabled)
                        {
                            tri_packet = scene_render.cursor;
                            uint8 *tri_limit = scene_render.limit;
                            if (tri_limit - tri_packet < tri_size)
                                return;
                            tri_end = tri_packet + tri_size;
                            POLY_G3 *poly0 = (POLY_G3 *)tri_packet;
                            poly0->tag = 0;
                            setPolyG3(poly0);
                            center_color = ring->center_color;
                            scene_render.cursor = tri_end;
                            poly0->p[0].rgbc.rgbc = center_color;
                            poly0->p[1].rgbc.rgbc = ring_color->color;
                            poly0->p[2].rgbc.rgbc = ring_color->color;
                            if (near_fade)
                            {
                                for (uint32 vertex_idx = 0; vertex_idx < 3; ++vertex_idx)
                                {
                                    POINT_C *vertex = poly0->p + vertex_idx;
                                    vertex->rgbc.r = (vertex->rgbc.r * near_fade) >> 8;
                                    vertex->rgbc.g = (vertex->rgbc.g * near_fade) >> 8;
                                    vertex->rgbc.b = (vertex->rgbc.b * near_fade) >> 8;
                                }
                            }
                            poly0->p[0].xy.x = center_scr.x;
                            poly0->p[0].xy.y = center_scr.y;
                            poly0->p[1].xy.x = prev_ray[0].x;
                            poly0->p[1].xy.y = prev_ray[0].y;
                            poly0->p[2].xy.x = cur_ray[0].x;
                            poly0->p[2].xy.y = cur_ray[0].y;
                            LinkEffectPacket(tri_packet, ot_bucket);
                            row_idx = 0;
                        }
                        cur_row = cur_ray;
                        prev_row = prev_ray;
                        row_radius = (sint32)ring_ray->distance;
                        ring_row = ring->row_count ? ring->row_points + ray_idx : 0;
                        while (row_idx < ring->row_count)
                        {
                            row_radius = (sint32)((uint32)row_radius + ring_row->distance);
                            row_radius_proj = DivideSignedWord(400 * (uint32)row_radius, ring_view_pos.z);
                            row_x = center_scr.x + (sint32)((uint32)row_radius_proj * (uint32)s_scale) / 4096;
                            row_y = center_scr.y - (sint32)((uint32)row_radius_proj * (uint32)c_scale) / 4096;
                            if (row_x > 612)
                                row_x = 612;
                            if (row_x < -100)
                                row_x = -100;
                            if (row_y > 340)
                                row_y = 340;
                            if (row_y < -100)
                                row_y = -100;
                            cur_row[1].x = row_x;
                            cur_row[1].y = row_y;
                            if (seg_enabled)
                            {
                                quad_packet = scene_render.cursor;
                                uint8 *quad_limit = scene_render.limit;
                                if (quad_limit - quad_packet < quad_size)
                                    return;
                                quad_end = quad_packet + quad_size;
                                POLY_G4 *poly1 = (POLY_G4 *)quad_packet;
                                poly1->tag = 0;
                                setPolyG4(poly1);

                                row_color = ring_row->color;
                                scene_render.cursor = quad_end;
                                poly1->p[0].rgbc.rgbc = row_color;
                                poly1->p[1].rgbc.rgbc = row_color;
                                if (row_idx)
                                {
                                    prev_row_color = (ring_row - ring_row_stride)->color;
                                    poly1->p[2].rgbc.rgbc = prev_row_color;
                                }
                                else
                                {
                                    poly1->p[2].rgbc.rgbc = ring_color->color;
                                    prev_row_color = ring_color->color;
                                }
                                poly1->p[3].rgbc.rgbc = prev_row_color;
                                if (near_fade)
                                {
                                    for (uint32 vertex_idx = 0; vertex_idx < 4; ++vertex_idx)
                                    {
                                        POINT_C *vertex = poly1->p + vertex_idx;
                                        vertex->rgbc.r = (vertex->rgbc.r * near_fade) >> 8;
                                        vertex->rgbc.g = (vertex->rgbc.g * near_fade) >> 8;
                                        vertex->rgbc.b = (vertex->rgbc.b * near_fade) >> 8;
                                    }
                                }
                                ring_vertex_x = prev_row[0].x;
                                quad_tag = poly1->tag;
                                poly1->p[2].xy.x = ring_vertex_x;
                                poly1->p[2].xy.y = prev_row[0].y;
                                poly1->p[3].xy.x = cur_row[0].x;
                                poly1->p[3].xy.y = cur_row[0].y;
                                poly1->p[0].xy.x = prev_row[1].x;
                                poly1->p[0].xy.y = prev_row[1].y;
                                poly1->p[1].xy.x = cur_row[1].x;
                                poly1->p[1].xy.y = cur_row[1].y;
                                poly1->tag = (quad_tag & 0xFF000000) | (*ot_bucket & 0xFFFFFF);
                                *ot_bucket = (*ot_bucket & 0xFF000000) | gpu_packet_link((uintptr_t)quad_packet);
                            }
                            ++cur_row;
                            ++prev_row;
                            ++row_idx;
                            if (row_idx < ring->row_count)
                                ring_row += ring_row_stride;
                        }

                        ++ring_color;
                        swap_ray = prev_ray;
                        ++ray_idx;
                        prev_ray = cur_ray;
                        cur_ray = swap_ray;
                        ray_angle = (uint32)ray_angle + angle_step;
                        ++ring_ray;
                    }

                    ring_state_head = *ot_bucket;
                    state->tag = (state->tag & 0xFF000000) | (ring_state_head & 0xFFFFFF);
                    *ot_bucket = (*ot_bucket & 0xFF000000) | gpu_packet_link((uintptr_t)state_packet);
                }
            }
        }
    }

    for (EffectObject *seg_node = effect_lists.segments; seg_node; seg_node = seg_node->next)
    {
        Vector3 line_start, line_end;
        ProjectedWorldLine line_proj;
        const EffectPoint *seg_point;
        const SegmentEffect *segs = static_cast<SegmentEffect *>(seg_node);
        uint32 ox = segs->origin.x;
        uint32 prev_color = segs->rgbc.rgbc;
        line_start.x = ox;
        line_start.y = (uint32)segs->origin.y;
        line_start.z = (uint32)segs->origin.z;
        seg_point = segs->points;
        for (sint32 seg_idx = 0; seg_idx < segs->point_count; ++seg_idx)
        {
            uint32 endpoint_x = seg_point->pos.x;
            uint32 endpoint_color = seg_point->rgbc.rgbc;
            line_end.x = endpoint_x;
            line_end.y = (uint32)seg_point->pos.y;
            line_end.z = (uint32)seg_point->pos.z;
            sint32 depth = ProjectAndClipWorldLine(line_start, line_end, line_proj);
            if (depth >= 0)
            {
                uint8 *state_packet = scene_render.cursor;
                uint8 *packet_limit = scene_render.limit;
                const sint32 packet_size = sizeof(DR_TPAGE) + sizeof(LINE_G2);
                if (packet_limit - state_packet < packet_size)
                    return;
                uint8 *line_packet = state_packet + sizeof(DR_TPAGE);
                scene_render.cursor = state_packet + packet_size;
                LINE_G2 *line = (LINE_G2 *)line_packet;
                line->tag = 0;
                setLineG2(line);
                line->p[0].rgbc.rgbc = prev_color;
                line->p[1].rgbc.rgbc = endpoint_color;
                uint32 forced_depth = segs->forced_depth;
                uint32 *bucket = scene_render.cur->ot + (forced_depth ? forced_depth : depth);
                line->p[0].xy = line_proj.scr[0];
                line->p[1].xy = line_proj.scr[1];
                LinkEffectPacket(line_packet, bucket);
                bool semi_trans = (prev_color & 0x2000000) != 0;
                prev_color = endpoint_color;
                if (semi_trans)
                    LinkEffectStatePacket(state_packet, bucket);
            }
            prev_color = endpoint_color;
            ++seg_point;
            line_start = line_end;
        }
    }

    for (EffectObject *point_node = effect_lists.points; point_node; point_node = point_node->next)
    {
        Vector3 line_start, line_end;
        ProjectedWorldLine line_proj;
        const EffectPoint *point;
        const PointSegments *points_effect = static_cast<PointSegments *>(point_node);
        line_start.x = (uint32)points_effect->origin.x;
        line_start.y = (uint32)points_effect->origin.y;
        line_start.z = (uint32)points_effect->origin.z;
        point = points_effect->points;
        for (sint32 point_idx = 0; point_idx < points_effect->point_count; ++point_idx)
        {
            uint32 endpoint_x = point->pos.x;
            uint32 color = point->rgbc.rgbc;
            line_end.x = endpoint_x;
            line_end.y = (uint32)point->pos.y;
            line_end.z = (uint32)point->pos.z;
            sint32 depth = ProjectAndClipWorldLine(line_start, line_end, line_proj);
            if (depth >= 0)
            {
                uint8 *state_packet = scene_render.cursor;
                uint8 *packet_limit = scene_render.limit;
                const sint32 packet_size = sizeof(DR_TPAGE) + sizeof(LINE_F2);
                if (packet_limit - state_packet < packet_size)
                    return;
                uint8 *line_packet = state_packet + sizeof(DR_TPAGE);
                scene_render.cursor = state_packet + packet_size;
                LINE_F2 *line = (LINE_F2 *)line_packet;
                line->tag = 0;
                setLineF2(line);
                line->rgbc.rgbc = color;
                uint32 forced_depth = points_effect->forced_depth;
                uint32 *bucket = scene_render.cur->ot + (forced_depth ? forced_depth : depth);
                line->p[0] = line_proj.scr[0];
                line->p[1] = line_proj.scr[1];
                LinkEffectPacket(line_packet, bucket);
                if (color & 0x2000000)
                    LinkEffectStatePacket(state_packet, bucket);
            }
            ++point;
            line_start = line_end;
        }
    }

    for (EffectObject *spr_node = effect_lists.sprs; spr_node; spr_node = spr_node->next)
    {
        uint8 *spr_packet;
        uint8 *spr_packet_end;
        uint32 spr_color;
        uint32 proj_size;
        const TextureResource *spr_tex;
        uint32 uv_word1;
        uint32 uv_word2;
        uint16 uv_tail;
        uint32 blend_mode;
        uint32 spr_angle;
        uint32 scale_x;
        const TextureFrame *spr_frame;
        sint32 frame_w;
        sint32 frame_h;
        sint32 frame_oy;
        bool flip_x;
        sint32 frame_ox;
        uint32 scale_y;
        uint32 pixel_w;
        uint32 pixel_h_word;
        uint32 scr_origin;
        uint32 scr_right;
        sint32 s;
        sint32 ox_s;
        sint32 oy_s;
        sint32 c;
        sint32 ox_c;
        sint32 oy_c;
        sint32 w_c;
        sint32 right_s;
        sint32 bottom_s;
        uint32 size_x;
        sint32 right_c;
        sint32 bottom_c;
        uint32 size_y;
        uint32 scaled_x0;
        uint32 scaled_x1;
        uint32 scaled_x2;
        uint32 scaled_x3;
        uint32 forced_depth;
        sint32 spr_depth;
        uint32 scr_center;
        uint32 trig_angle;
        uint32 packed_vertex_xy;
        Vector3 spr_view_pos;
        const Sprite *spr = static_cast<Sprite *>(spr_node);
        spr_view_pos.x = (uint32)(spr->pos.x >> 12) - (uint32)view_pos.x;
        spr_view_pos.y = (uint32)(spr->pos.y >> 12) - (uint32)view_pos.y;
        spr_view_pos.z = (uint32)(spr->pos.z >> 12) - (uint32)view_pos.z;
        packed_vertex_xy = (uint16)spr_view_pos.x | ((uint32)(uint16)spr_view_pos.y << 16);
        xport_gte_write_data(0, packed_vertex_xy);
        xport_gte_write_data(1, spr_view_pos.z);
        xport_gte_execute(0x180001);
        spr_packet = scene_render.cursor;
        uint8 *packet_limit = scene_render.limit;
        const sint32 packet_size = sizeof(POLY_FT4);
        if (packet_limit - spr_packet < packet_size)
            return;
        spr_packet_end = spr_packet + packet_size;
        POLY_FT4 *poly = (POLY_FT4 *)spr_packet;
        poly->tag = 0;
        setPolyFT4(poly);
        spr_color = spr->color;
        scene_render.cursor = spr_packet_end;
        poly->rgbc.rgbc = spr_color;
        spr_depth = xport_gte_read_data(27);
        if (spr_depth >= viewport_near_depth && spr_depth <= (sint32)proj_cfg.max_distance && ((proj_size = DivideEffectUnsigned((uint32)(sint16)spr->size << 7, (uint32)spr_depth)) < 0x201 || (spr->flags & 8) != 0))
        {
            scr_center = xport_gte_read_data(14);
            spr_tex = spr->GetTexFrame().tex;
            uint32 pal_override = spr->clut;
            uint32 uv_word0 = spr_tex->PackUv(0);
            uv_word1 = spr_tex->PackUv(1);
            uv_word2 = spr_tex->PackUv(2);
            uv_tail = spr_tex->uv[3].uv;
            if (pal_override)
            {
                uint32 packed_uv0 = (uv_word0 & 0xFFFF) | (pal_override << 16);
                poly->p[0].uv.uv = packed_uv0;
                poly->p[0].attr = packed_uv0 >> 16;
            }
            else
            {
                poly->p[0].uv.uv = uv_word0;
                poly->p[0].attr = uv_word0 >> 16;
            }
            blend_mode = spr->blend_mode;
            // The packed third UV word retains the fourth UV pair in the upper half
            poly->p[2].uv.uv = uv_word2;
            poly->p[2].attr = uv_word2 >> 16;
            poly->p[3].uv.uv = uv_tail;
            uint32 packed_uv1 = (uv_word1 & 0xFF9FFFFF) | (blend_mode << 16);
            poly->p[1].uv.uv = packed_uv1;
            poly->p[1].attr = packed_uv1 >> 16;
            spr_angle = spr->angle;
            if (proj_size >= 0x100 || spr_angle)
            {
                // Preserve the original interleaved UV reads and decrements
                uint32 u1 = poly->p[1].uv.u;
                uint32 v2 = poly->p[2].uv.v;
                poly->p[1].uv.u = u1 - 1;
                uint32 u3 = poly->p[3].uv.u;
                poly->p[2].uv.v = v2 - 1;
                uint32 v3 = poly->p[3].uv.v;
                poly->p[3].uv.u = u3 - 1;
                poly->p[3].uv.v = v3 - 1;
            }
            scale_x = proj_size * spr->scale[0];
            scale_y = proj_size * spr->scale[1];
            spr_frame = &spr->GetTexFrame();
            uint32 spr_flags = spr->flags;
            frame_w = spr_frame->w;
            frame_h = spr_frame->h;
            frame_oy = spr_frame->oy;
            flip_x = (spr_flags & 1) != 0;
            frame_ox = spr_frame->ox;
            uint32 left_idx = flip_x ? 1 : 0;
            uint32 right_idx = 1 - left_idx;
            if (spr_angle)
            {
                trig_angle = spr_angle & 0xFFF;
                s = (sint16)rsin(trig_angle);
                // Byte frame dimensions and signed16 trig values fit signed32 products
                ox_s = frame_ox * s;
                oy_s = frame_oy * s;
                c = (sint16)rcos(trig_angle);
                ox_c = frame_ox * c;
                oy_c = frame_oy * c;
                w_c = frame_w * c;
                right_s = ox_s + frame_w * s;
                bottom_s = oy_s + frame_h * s;
                size_x = scale_x >> 12;
                right_c = ox_c + w_c;
                bottom_c = oy_c + frame_h * c;
                size_y = scale_y >> 12;
                // Unsigned scale products and shifts preserve packed coordinate carries
                scaled_x0 = size_x * (ox_c - oy_s);
                scaled_x1 = size_x * (right_c - oy_s);
                scaled_x2 = size_x * (ox_c - bottom_s);
                scaled_x3 = size_x * (right_c - bottom_s);
                poly->p[left_idx].xy.xy = scr_center + (scaled_x0 >> 19) + (((size_y * (ox_s + oy_c)) >> 19) << 16);
                poly->p[right_idx].xy.xy = scr_center + (scaled_x1 >> 19) + (((size_y * (right_s + oy_c)) >> 19) << 16);
                poly->p[2 + left_idx].xy.xy = scr_center + (scaled_x2 >> 19) + (((size_y * (ox_s + bottom_c)) >> 19) << 16);
                poly->p[2 + right_idx].xy.xy = scr_center + (scaled_x3 >> 19) + (((size_y * (right_s + bottom_c)) >> 19) << 16);
            }
            else
            {
                pixel_w = (proj_size * frame_w) >> 7;
                pixel_h_word = ((proj_size * frame_h) >> 7) << 16;
                scr_origin = scr_center + (((proj_size * frame_oy) >> 7) << 16) + (sint16)((sint32)(proj_size * frame_ox) >> 7);
                if (pixel_w < 5)
                    pixel_w = 2;
                if (pixel_h_word <= 0x4FFFF)
                    pixel_h_word = 0x10000;
                scr_right = scr_origin + pixel_w;
                poly->p[left_idx].xy.xy = scr_origin;
                poly->p[right_idx].xy.xy = scr_right;
                poly->p[2 + left_idx].xy.xy = scr_origin + pixel_h_word;
                poly->p[2 + right_idx].xy.xy = scr_right + pixel_h_word;
            }
            forced_depth = spr->forced_depth;
            if (forced_depth)
                LinkForcedEffectPacket(spr_packet, *spr, forced_depth);
            else
                LinkBiasedEffectPacket(spr_packet, *spr, spr_depth);
        }
        else
        {
            scene_render.cursor = spr_packet;
        }
    }

    for (EffectObject *fragment_node = effect_lists.opaque_fragments; fragment_node; fragment_node = fragment_node->next)
    {
        const OpaqueFragment *fragment = static_cast<const OpaqueFragment *>(fragment_node);
        hd_polygon_chain_reset(fragment->hd_history);
        LoadEffectTriangleVertex(fragment->vertices[0], view_pos, 0);
        LoadEffectTriangleVertex(fragment->vertices[1], view_pos, 2);
        LoadEffectTriangleVertex(fragment->vertices[2], view_pos, 4);
        xport_gte_execute(0x280030);
        uint8 *poly_packet = scene_render.cursor;
        uint8 *packet_limit = scene_render.limit;
        const sint32 packet_size = sizeof(POLY_F3) + sizeof(DR_TPAGE);
        if (packet_limit - poly_packet < packet_size)
            return;
        uint8 *state_packet = poly_packet + sizeof(POLY_F3);
        POLY_F3 *poly = (POLY_F3 *)poly_packet;
        DR_TPAGE *state = (DR_TPAGE *)state_packet;
        poly->tag = 0;
        setPolyF3(poly);
        scene_render.cursor = poly_packet + packet_size;
        setSemiTrans(poly, 1);
        setRGB0(poly, fragment->draw_color.r, fragment->draw_color.g, fragment->draw_color.b);
        uint32 state_tag = EffectObject::GetDrawMode().tag;
        uint32 state_code = EffectObject::GetDrawMode().code[0];
        state->tag = state_tag;
        state->code[0] = state_code;
        poly->p[0].xy = xport_gte_read_data(12);
        poly->p[1].xy = xport_gte_read_data(13);
        poly->p[2].xy = xport_gte_read_data(14);
        uint32 depth = xport_gte_read_data(27);
        uint32 *bucket = scene_render.cur->ot + ((depth & 0x3FFC) >> 2);
        uint32 linked_head = LinkEffectPacket(poly_packet, bucket);
        state->tag = (state->tag & 0xFF000000) | (linked_head & 0xFFFFFF);
        *bucket = (*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)state_packet);
    }

    for (EffectObject *line_node = effect_lists.lines; line_node; line_node = line_node->next)
    {
        const LineEffect *effect = static_cast<LineEffect *>(line_node);
        const uint32 world_shift = 12;
        Vector3 world_pos, relative_pos;
        world_pos.ShiftRight(effect->endpoint[0], world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        uint32 packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        sint32 depth0 = xport_gte_read_data(27);
        if (depth0 < (sint32)proj_cfg.near_depth || depth0 > (sint32)proj_cfg.max_distance)
            continue;
        uint32 scr0 = xport_gte_read_data(14);
        world_pos.ShiftRight(effect->endpoint[1], world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        sint32 depth1 = xport_gte_read_data(27);
        if (depth1 < (sint32)proj_cfg.near_depth || depth1 > (sint32)proj_cfg.max_distance)
            continue;
        uint32 scr1 = xport_gte_read_data(14);
        uint8 *line_packet = scene_render.cursor;
        uint8 *packet_limit = scene_render.limit;
        const sint32 packet_size = sizeof(LINE_G2);
        if (packet_limit - line_packet < packet_size)
            return;
        LINE_G2 *line = (LINE_G2 *)line_packet;
        line->tag = 0;
        setLineG2(line);
        uint32 color = effect->color[0];
        scene_render.cursor = line_packet + packet_size;
        line->p[0].rgbc.rgbc = color;
        line->p[1].rgbc.rgbc = effect->color[1];
        line->p[0].xy.xy = scr0;
        line->p[1].xy.xy = scr1;
        if (depth1 < depth0)
            depth0 = depth1;
        uint32 *bucket = scene_render.cur->ot + ((depth0 & 0x3FFC) >> 2);
        LinkEffectPacket(line_packet, bucket);
    }

    for (EffectObject *quad_node = effect_lists.quads; quad_node; quad_node = quad_node->next)
    {
        const TexturedQuad *quad = static_cast<TexturedQuad *>(quad_node);
        hd_polygon_chain_reset(quad->hd_history);
        if (!quad->GetTex())
            continue;
        const uint32 world_shift = 12;
        const sint32 packet_size = sizeof(POLY_FT4);
        uint32 packed_xy;
        Vector3 world_pos, relative_pos;
        uint8 *poly_packet, *packet_limit;
        uint32 uv_words[3];
        uint16 uv_tail;
        sint32 depth[4], max_depth;
        POLY_FT4 *poly;
        const TextureResource *tex;
        world_pos.ShiftRight(quad->pos, world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        poly_packet = scene_render.cursor;
        packet_limit = scene_render.limit;
        tex = quad->GetTex();
        if (packet_limit - poly_packet < packet_size)
            return;
        scene_render.cursor = poly_packet + packet_size;
        depth[0] = xport_gte_read_data(27);
        if (depth[0] < (sint32)proj_cfg.near_depth || depth[0] > (sint32)proj_cfg.max_distance)
            goto RejectTexturedQuad;
        poly = (POLY_FT4 *)poly_packet;
        poly->p[0].xy.xy = xport_gte_read_data(14);
        world_pos.ShiftRight(quad->corners[0], world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        depth[1] = xport_gte_read_data(27);
        if (depth[1] < (sint32)proj_cfg.near_depth || depth[1] > (sint32)proj_cfg.max_distance)
            goto RejectTexturedQuad;
        poly->p[1].xy.xy = xport_gte_read_data(14);
        world_pos.ShiftRight(quad->corners[1], world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        poly->tag = 0;
        setPolyFT4(poly);
        poly->rgbc.rgbc = quad->color;
        max_depth = depth[0];
        if (max_depth < depth[1])
            max_depth = depth[1];
        depth[2] = xport_gte_read_data(27);
        if (depth[2] < (sint32)proj_cfg.near_depth || depth[2] > (sint32)proj_cfg.max_distance)
            goto RejectTexturedQuad;
        poly->p[2].xy.xy = xport_gte_read_data(14);
        world_pos.ShiftRight(quad->corners[2], world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        uv_words[0] = tex->PackUv(0);
        uv_words[1] = tex->PackUv(1);
        uv_words[2] = tex->PackUv(2);
        uv_tail = tex->uv[3].uv;
        poly->p[0].uv.uv = uv_words[0];
        poly->p[0].attr = uv_words[0] >> 16;
        poly->p[1].uv.uv = uv_words[1];
        poly->p[1].attr = uv_words[1] >> 16;
        // Preserve the fourth UV pair carried by this packed word
        poly->p[2].uv.uv = uv_words[2];
        poly->p[2].attr = uv_words[2] >> 16;
        poly->p[3].uv.uv = uv_tail;
        if (quad->flags & 4)
            poly->p[1].attr = (poly->p[1].attr & 0xFF9F) | 0x40;
        if (max_depth < depth[2])
            max_depth = depth[2];
        depth[3] = xport_gte_read_data(27);
        if (depth[3] < (sint32)proj_cfg.near_depth || depth[3] > (sint32)proj_cfg.max_distance)
            goto RejectTexturedQuad;
        poly->p[3].xy.xy = xport_gte_read_data(14);
        if (max_depth < depth[3])
            max_depth = depth[3];
        LinkBiasedEffectPacket(poly_packet, *quad, max_depth);
        continue;
    RejectTexturedQuad:
        scene_render.cursor = poly_packet;
    }

    // Preserve the approved FF44C packet mask correction
    for (EffectObject *triangle_node = effect_lists.textured_triangles; triangle_node; triangle_node = triangle_node->next)
    {
        const TexturedTriangle *triangle = static_cast<const TexturedTriangle *>(triangle_node);
        hd_polygon_chain_reset(triangle->hd_history);
        const sint32 packet_size = sizeof(POLY_FT3);
        const sint32 max_packet_bytes = 2 * sizeof(DR_TWIN) + 4 * sizeof(POLY_FT3);
        const uint8 face_vertices[4][3] = {{0, 1, 2}, {0, 3, 1}, {1, 3, 2}, {2, 3, 0}};
        uint8 *packet_cursor = scene_render.cursor;
        uint8 *packet_limit = scene_render.limit;
        uint32 scr[4], packed_xy;
        Vector3 relative_pos;
        uint32 *bucket;
        uint8 *window_packet[2], *poly_cursor;
        sint32 depth[4], max_depth = -1;
        if (packet_limit - packet_cursor < max_packet_bytes)
            return;
        relative_pos.x = (uint32)triangle->transformed[0].x - (uint32)view_pos.comp[0];
        relative_pos.y = (uint32)triangle->transformed[0].y - (uint32)view_pos.comp[1];
        relative_pos.z = (uint32)triangle->transformed[0].z - (uint32)view_pos.comp[2];
        packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        depth[0] = xport_gte_read_data(27);
        scr[0] = xport_gte_read_data(14);
        for (uint32 idx = 1; idx < 4; ++idx)
        {
            const Vector3 &pos = triangle->transformed[idx];
            relative_pos.x = (uint32)pos.x - (uint32)view_pos.comp[0];
            relative_pos.y = (uint32)pos.y - (uint32)view_pos.comp[1];
            relative_pos.z = (uint32)pos.z - (uint32)view_pos.comp[2];
            packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
            xport_gte_write_data(0, packed_xy);
            xport_gte_write_data(1, (uint32)relative_pos.z);
            xport_gte_execute(0x180001);
            // The original pipeline projects the next vertex before rejecting the previous one
            if (depth[idx - 1] < (sint32)proj_cfg.near_depth || depth[idx - 1] > (sint32)proj_cfg.max_distance)
                goto NextTexturedTriangle;
            if (max_depth < depth[idx - 1])
                max_depth = depth[idx - 1];
            depth[idx] = xport_gte_read_data(27);
            if (idx < 3)
                scr[idx] = xport_gte_read_data(14);
        }
        if (depth[3] < (sint32)proj_cfg.near_depth || depth[3] > (sint32)proj_cfg.max_distance)
            goto NextTexturedTriangle;
        if (max_depth < depth[3])
            max_depth = depth[3];
        scr[3] = xport_gte_read_data(14);
        bucket = scene_render.cur->ot + ((max_depth & 0x3FFC) >> 2);
        if (triangle->texture_window)
        {
            window_packet[0] = scene_render.cursor;
            window_packet[1] = window_packet[0] + sizeof(DR_TWIN);
            scene_render.cursor = window_packet[0] + 2 * sizeof(DR_TWIN);
            DR_TWIN *reset_window = (DR_TWIN *)window_packet[0];
            DR_TWIN *apply_window = (DR_TWIN *)window_packet[1];
            PSX_RECT zero_window = {0, 0, 0, 0};
            reset_window->tag = 0;
            setTexWindow(reset_window, &zero_window);
            apply_window->tag = 0;
            setTexWindow(apply_window, &zero_window);
            apply_window->code[0] = triangle->texture_window;
            if (triangle->texture_window)
                LinkEffectPacket(window_packet[0], bucket);
        }
        poly_cursor = scene_render.cursor;
        for (uint32 face = 0; face < 4; ++face)
        {
            const uint8 *vertices = face_vertices[face];
            xport_gte_write_data(12, scr[vertices[0]]);
            xport_gte_write_data(14, scr[vertices[2]]);
            xport_gte_write_data(13, scr[vertices[1]]);
            xport_gte_execute(0x1400006);
            sint32 signed_area = xport_gte_read_data(24);
            if (signed_area <= 0)
            {
                POLY_FT3 *poly = (POLY_FT3 *)poly_cursor;
                poly->tag = 0;
                setPolyFT3(poly);
                poly->rgbc.rgbc = (face ? triangle->colors[face - 1].rgbc : triangle->base_color.rgbc) | 0x24000000;
                uint32 uv_clut = triangle->uv_clut;
                uint32 tag = poly->tag;
                poly->p[0].uv.uv = uv_clut;
                poly->p[0].attr = uv_clut >> 16;
                uint32 packed_uv2 = triangle->uv_tpage;
                poly->p[1].uv.uv = packed_uv2;
                poly->p[1].attr = packed_uv2 >> 16;
                poly->p[2].uv.uv = triangle->uv_tail;
                poly->p[0].xy.xy = scr[vertices[0]];
                poly->p[1].xy.xy = scr[vertices[1]];
                poly->p[2].xy.xy = scr[vertices[2]];
                poly->tag = (tag & 0xFF000000) | (*bucket & 0xFFFFFF);
                uint32 address = gpu_packet_link((uintptr_t)poly_cursor);
                poly_cursor += packet_size;
                *bucket = (*bucket & 0xFF000000) | address;
            }
        }
        scene_render.cursor = poly_cursor;
        if (triangle->texture_window)
            LinkEffectPacket(window_packet[1], bucket);
    NextTexturedTriangle:;
    }

    for (EffectObject *particle_node = effect_lists.particles; particle_node; particle_node = particle_node->next)
    {
        const PrimitiveParticle *particle = static_cast<PrimitiveParticle *>(particle_node);
        const TILE &prim = particle->primitive;
        const uint32 packed_size = (uint16)prim.w | ((uint32)(uint16)prim.h << 16);
        const uint32 world_shift = 12;
        Vector3 world_pos, relative_pos;
        world_pos.ShiftRight(particle->pos, world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        uint32 packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        uint8 *packet = scene_render.cursor;
        uint8 *packet_limit = scene_render.limit;
        XY *scr_pos;
        if ((packed_size & 15) == 1)
        {
            const sint32 packet_size = sizeof(TILE_1);
            if (packet_limit - packet < packet_size)
                return;
            TILE_1 *tile = (TILE_1 *)packet;
            uint32 tag = prim.tag;
            scene_render.cursor = packet + packet_size;
            // Copy the initialized template including its command and service bits
            tile->tag = tag;
            tile->rgbc.rgbc = prim.rgbc.rgbc;
            scr_pos = &tile->p[0];
        }
        else
        {
            const sint32 packet_size = sizeof(TILE);
            if (packet_limit - packet < packet_size)
                return;
            TILE *tile = (TILE *)packet;
            uint32 tag = prim.tag;
            scene_render.cursor = packet + packet_size;
            tile->tag = tag;
            tile->rgbc.rgbc = prim.rgbc.rgbc;
            tile->w = packed_size;
            tile->h = packed_size >> 16;
            scr_pos = &tile->p[0];
        }
        sint32 depth = xport_gte_read_data(27);
        if (depth < (sint32)proj_cfg.near_depth || depth > (sint32)proj_cfg.max_distance)
        {
            scene_render.cursor = packet;
            continue;
        }
        scr_pos->xy = xport_gte_read_data(14);
        LinkBiasedEffectPacket(packet, *particle, depth);
    }

    RibbonEdge saved_edge = {{{0, 0}, {0, 0}}};
    RibbonEdge prev_edge = {{{0, 0}, {0, 0}}};
    RibbonScreenPoint prev_center = {0, 0};
    bool force_start = false;
    for (EffectObject *ribbon_node = effect_lists.endpoint_sprs; ribbon_node; ribbon_node = ribbon_node->next)
    {
        const EndpointSprite *ribbon = static_cast<EndpointSprite *>(ribbon_node);
        bool new_start = (ribbon->flags & 0x10) || force_start;
        const uint32 world_shift = 12;
        const sint32 packet_size = sizeof(POLY_FT4);
        Vector3 world_pos, relative_pos;
        uint32 packed_xy, start_width;
        RibbonScreenPoint start_pos;
        if (new_start)
        {
            world_pos.ShiftRight(ribbon->endpoint[0], world_shift);
            relative_pos.Subtract(world_pos, view_pos);
            packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
            xport_gte_write_data(0, packed_xy);
            xport_gte_write_data(1, (uint32)relative_pos.z);
            xport_gte_execute(0x180001);
        }
        else
            start_pos = prev_center;
        const TextureFrame &frame = ribbon->GetTexFrame();
        const TextureResource *tex = frame.tex;
        uint32 frame_width = frame.w;
        if (new_start)
        {
            sint32 depth = xport_gte_read_data(27);
            force_start = true;
            if (depth < (sint32)proj_cfg.near_depth || depth > (sint32)proj_cfg.max_distance)
                continue;
            XY scr;
            scr.xy = xport_gte_read_data(14);
            start_pos.x = scr.x;
            start_pos.y = scr.y;
            start_width = DivideEffectUnsigned((uint32)(sint16)ribbon->size * frame_width, depth) >> 1;
            if (start_width < 2)
                start_width = 2;
        }
        world_pos.ShiftRight(ribbon->endpoint[1], world_shift);
        relative_pos.Subtract(world_pos, view_pos);
        packed_xy = ((uint32)relative_pos.x & 0xFFFF) | (((uint32)relative_pos.y & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, (uint32)relative_pos.z);
        xport_gte_execute(0x180001);
        uint8 *packet = scene_render.cursor;
        uint8 *packet_limit = scene_render.limit;
        if (packet_limit - packet < packet_size)
            return;
        POLY_FT4 *poly = (POLY_FT4 *)packet;
        poly->tag = 0;
        setPolyFT4(poly);
        uint32 color = ribbon->color;
        scene_render.cursor = packet + packet_size;
        poly->rgbc.rgbc = color;
        uint32 uv0 = tex->PackUv(0);
        uint32 uv1 = tex->PackUv(1);
        uint32 uv2 = tex->PackUv(2);
        uint16 uv_tail = tex->uv[3].uv;
        poly->p[0].uv.uv = uv0;
        poly->p[0].attr = uv0 >> 16;
        poly->p[1].uv.uv = uv1;
        poly->p[1].attr = uv1 >> 16;
        uint32 u1 = poly->p[1].uv.u;
        // Preserve the fourth UV pair in the third packed UV word
        poly->p[2].uv.uv = uv2;
        poly->p[2].attr = uv2 >> 16;
        uint32 v2 = poly->p[2].uv.v;
        poly->p[3].uv.uv = uv_tail;
        poly->p[1].uv.u = u1 - 1;
        uint32 u3 = poly->p[3].uv.u;
        poly->p[2].uv.v = v2 - 1;
        uint32 v3 = poly->p[3].uv.v - 1;
        poly->p[3].uv.u = u3 - 1;
        poly->p[3].uv.v = v3;
        sint32 depth = xport_gte_read_data(27);
        if (depth < (sint32)proj_cfg.near_depth || depth > (sint32)proj_cfg.max_distance)
        {
            scene_render.cursor = packet;
            force_start = true;
            continue;
        }
        XY scr;
        scr.xy = xport_gte_read_data(14);
        RibbonScreenPoint end_pos = {scr.x, scr.y};
        uint32 width_product = (uint32)(sint16)ribbon->size * frame_width;
        prev_center.x = end_pos.x;
        uint32 end_width = DivideEffectUnsigned(width_product, depth) >> 1;
        prev_center.y = end_pos.y;
        if (end_width < 2)
            end_width = 2;
        sint32 dx = end_pos.x - start_pos.x;
        sint32 dy = end_pos.y - start_pos.y;
        uint32 len = LookupDistanceRoot((uint32)dx * (uint32)dx + (uint32)dy * (uint32)dy);
        if (!len)
        {
            scene_render.cursor = packet;
            force_start = true;
            continue;
        }
        RibbonEdge start_edge, end_edge;
        if (new_start)
        {
            sint32 offset_x = DivideSignedWord((uint32)dy * start_width, len);
            sint32 offset_y = DivideSignedWord((uint32)dx * start_width, len);
            start_edge.p[0].x = (uint32)start_pos.x + (uint32)offset_x;
            start_edge.p[1].x = (uint32)start_pos.x - (uint32)offset_x;
            start_edge.p[1].y = (uint32)start_pos.y + (uint32)offset_y;
            start_edge.p[0].y = (uint32)start_pos.y - (uint32)offset_y;
        }
        else
            start_edge = prev_edge;
        uint32 join_flags = ribbon->flags;
        bool close = (join_flags & 0x20) != 0;
        if (join_flags & 0x10)
            saved_edge = start_edge;
        uint32 end_product = (uint32)dy * end_width;
        if (close)
        {
            end_edge = saved_edge;
            prev_edge.p[0].x = saved_edge.p[0].x;
        }
        else
        {
            sint32 offset_x = DivideSignedWord(end_product, len);
            sint32 offset_y = DivideSignedWord((uint32)dx * end_width, len);
            end_edge.p[0].x = (uint32)end_pos.x + (uint32)offset_x;
            end_edge.p[1].x = (uint32)end_pos.x - (uint32)offset_x;
            end_edge.p[0].y = (uint32)end_pos.y - (uint32)offset_y;
            end_edge.p[1].y = (uint32)end_pos.y + (uint32)offset_y;
            prev_edge.p[0].x = end_edge.p[0].x;
        }
        prev_edge.p[0].y = end_edge.p[0].y;
        prev_edge.p[1].x = end_edge.p[1].x;
        uint32 flags = ribbon->flags;
        prev_edge.p[1].y = end_edge.p[1].y;
        if ((flags & 2) != 0)
        {
            if ((flags & 1) != 0)
            {
                poly->p[0].xy.x = end_edge.p[0].x;
                poly->p[1].xy.x = end_edge.p[1].x;
                poly->p[0].xy.y = end_edge.p[0].y;
                poly->p[1].xy.y = end_edge.p[1].y;
                poly->p[2].xy.x = start_edge.p[0].x;
                poly->p[3].xy.x = start_edge.p[1].x;
                poly->p[3].xy.y = start_edge.p[1].y;
                poly->p[2].xy.y = start_edge.p[0].y;
            }
            else
            {
                poly->p[0].xy.x = end_edge.p[1].x;
                poly->p[1].xy.x = end_edge.p[0].x;
                poly->p[0].xy.y = end_edge.p[1].y;
                poly->p[1].xy.y = end_edge.p[0].y;
                poly->p[2].xy.x = start_edge.p[1].x;
                poly->p[3].xy.x = start_edge.p[0].x;
                poly->p[2].xy.y = start_edge.p[1].y;
                poly->p[3].xy.y = start_edge.p[0].y;
            }
        }
        else if ((flags & 1) != 0)
        {
            poly->p[0].xy.x = start_edge.p[1].x;
            poly->p[1].xy.x = end_edge.p[1].x;
            poly->p[0].xy.y = start_edge.p[1].y;
            poly->p[1].xy.y = end_edge.p[1].y;
            poly->p[2].xy.x = start_edge.p[0].x;
            poly->p[3].xy.x = end_edge.p[0].x;
            poly->p[3].xy.y = end_edge.p[0].y;
            poly->p[2].xy.y = start_edge.p[0].y;
        }
        else
        {
            poly->p[0].xy.x = start_edge.p[0].x;
            poly->p[1].xy.x = end_edge.p[0].x;
            poly->p[1].xy.y = end_edge.p[0].y;
            poly->p[2].xy.x = start_edge.p[1].x;
            poly->p[3].xy.x = end_edge.p[1].x;
            poly->p[2].xy.y = start_edge.p[1].y;
            poly->p[3].xy.y = end_edge.p[1].y;
            poly->p[0].xy.y = start_edge.p[0].y;
        }
        LinkBiasedEffectPacket(packet, *ribbon, depth);
        force_start = false;
    }
}
