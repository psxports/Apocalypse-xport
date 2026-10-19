#include <stdlib.h>

#include "geometry_commands.h"
#include "model_render.h"
#include "scene_lifecycle.h"
#include "model_render_view.h"
#include "scene_effect_packets.h"
#include "effect_object.h"
#include "opaque_fragment.h"
#include "textured_fragment.h"
#include "effect_lifecycle.h"
#include "effect_sprite.h"
#include "textured_quad.h"
#include "ring_effect.h"
#include "point_segments.h"
#include "effect_segments.h"
#include "model_projection.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "game_diagnostics.h"

// Ribbon joins retain full signed32 coordinates until GPU packet publication
struct RibbonScreenPoint
{
    sint32 x, y;
};

struct RibbonEdge
{
    RibbonScreenPoint p[2];
};

// Native ABI omits incidental MIPS return carriers
// Human-approved FF44C fix keeps the packet mask at FF000000
static uint32 DivideEffectUnsigned(uint32 numerator, uint32 denominator)
{
    return denominator == 0 ? 0xFFFFFFFF : numerator / denominator;
}

static sint32 DivideEffectSigned(uint32 numerator, uint32 denominator)
{
    sint32 dividend = (sint32)numerator;
    sint32 divisor = (sint32)denominator;
    if (divisor == 0)
        return dividend < 0 ? 1 : -1;
    if (dividend == (sint32)0x80000000 && divisor == -1)
        return dividend;
    return dividend / divisor;
}

// Native extraction of the original three halfword vertex stores
static void LoadEffectTriangleVertex(const Vector3 &position, const Vector3 &view, uint32 first_register)
{
    uint32 x = (uint32)(position.x >> 12) - (uint32)(uint16)view.components[0];
    uint32 y = (uint32)(position.y >> 12) - (uint32)(uint16)view.components[1];
    uint32 z = (uint32)(position.z >> 12) - (uint32)(uint16)view.components[2];
    uint32 packed = (x & 0xFFFF) | ((y & 0xFFFF) << 16);
    xport_gte_write_data(first_register, packed);
    // VZ consumes only its signed low halfword in the existing GTE backend
    xport_gte_write_data(first_register + 1, (uint32)(sint32)(sint16)z);
}

// Native extraction preserves both bias reads around the packet tag store
static uint32 LinkBiasedEffectPacket(uint8 *packet, const EffectObject &object, uint32 depth)
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
    uint32 result = (head & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
    *bucket = result;
    return result;
}

// Native extraction retains the second forced-depth byte read after linking
static uint32 LinkForcedEffectPacket(uint8 *packet, const Sprite &sprite, uint32 depth)
{
    uint32 *ordering_table = scene_render.cur->ot;
    P_TAG *header = (P_TAG *)packet;
    uint32 *bucket = ordering_table + depth;
    uint32 head = *bucket;
    header->addr = head;
    depth = sprite.forced_depth;
    bucket = ordering_table + depth;
    head = *bucket;
    uint32 result = (head & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
    *bucket = result;
    return result;
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
    uint32 tag = GetEffectDrawMode().tag;
    uint32 code = GetEffectDrawMode().code[0];
    DR_TPAGE *state = (DR_TPAGE *)packet;
    state->tag = tag;
    state->code[0] = code;
    uint32 head = *bucket;
    state->tag = (tag & 0xFF000000) | (head & 0xFFFFFF);
    head = *bucket;
    *bucket = (head & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
}

void BuildSceneEffectPackets(void)
{
    Vector3 line_start, line_end;
    ProjectedWorldLine line_projection;
    ModelBorrow model_borrow;
    uint32 trig_angle;
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    FUNCTION_MARKER(0x8003736C, "SLUS_003.73");
    sint16 ring_vertex_x;
    uint32 ring_state_head;
    uint32 rotation_words[5];
    uint32 packed_vertex_xy;
    Vector3 ring_view_pos, view_pos, sprite_view_pos;
    EffectObject *render_node;
    sint32 result;
    EffectObject *ring_node;
    uint8 *state_packet;
    uint8 *state_end;
    uint32 state_code;
    uint32 near_fade;
    uint32 scaled_view_x;
    uint32 center_x_word;
    uint32 scaled_view_y;
    uint32 center_y_word;
    sint32 center_x;
    sint32 center_y;

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
    sint32 quad_tag;
    EffectObject *segment_node;
    EffectPoint *segment_point;
    EffectObject *point_node;
    const EffectPoint *point;
    EffectObject *sprite_node;
    uint8 *sprite_packet;
    uint8 *sprite_packet_end;
    uint32 sprite_color;
    uint32 proj_size;
    const TextureResource *sprite_tex;
    uint32 uv_word1;
    uint32 uv_word2;
    uint16 uv_tail;
    uint32 blend_mode;
    uint32 sprite_angle;
    uint32 scale_x;
    const TextureFrame *sprite_frame;
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
    EffectObject *fragment_node;
    Vector3 ring_relative_pos, ring_world_pos;
    uint32 world_shift;
    uint16 projected_x_word;
    uint16 projected_y_word;
    sint32 sprite_depth;
    uint32 scr_center;
    sint32 viewport_near_depth;
    sint32 ray_count;
    sint32 angle_step;
    sint32 ray_angle;
    const RingPoint *ring_ray;
    uint16 center_scr_x;
    uint16 center_scr_y;
    sint32 s_scale;
    sint32 c_scale;
    bool seg_enabled;
    uint32 ring_row_stride;
    const RingPoint *ring_row;
    rotation_words[0] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 0);
    rotation_words[1] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 1);
    xport_gte_write_control(0, rotation_words[0]);
    xport_gte_write_control(1, rotation_words[1]);
    rotation_words[2] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 2);
    rotation_words[3] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 3);
    rotation_words[4] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 4);
    xport_gte_write_control(2, rotation_words[2]);
    xport_gte_write_control(3, rotation_words[3]);
    xport_gte_write_control(4, rotation_words[4]);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    view_pos.components[0] = GetModelRenderView().pos.words[0];
    view_pos.components[1] = GetModelRenderView().pos.words[1];
    view_pos.components[2] = GetModelRenderView().pos.words[2];
    const ModelViewport *view_pointer = model_render_state.viewport;
    result = view_pointer->distance;
    render_node = effect_lists.renderables;
    viewport_near_depth = (uint32)result >> 1;
    for (; render_node; render_node = render_node->next)
    {
        if (render_node->IsAlive())
            result = static_cast<RenderableEffect *>(render_node)->Render();
    }

    for (ring_node = effect_lists.rings;; ring_node = ring_node->next)
    {
        if (!ring_node)
        {
            for (segment_node = effect_lists.segments;; segment_node = segment_node->next)
            {
                if (!segment_node)
                {
                    for (point_node = effect_lists.points; point_node; point_node = point_node->next)
                    {
                        const PointSegments *points_effect = static_cast<PointSegments *>(point_node);
                        line_start.x = points_effect->origin.words[0];
                        line_start.y = points_effect->origin.words[1];
                        line_start.z = points_effect->origin.words[2];
                        point = points_effect->points;
                        for (sint32 point_idx = 0; point_idx < (sint32)points_effect->point_count; ++point_idx)
                        {
                            uint32 endpoint_x = point->pos.words[0];
                            uint32 color = point->rgbc.rgbc;
                            line_end.x = endpoint_x;
                            line_end.y = point->pos.words[1];
                            line_end.z = point->pos.words[2];
                            sint32 depth = ProjectAndClipWorldLine(line_start, line_end, line_projection);
                            if (depth >= 0)
                            {
                                uint8 *state_packet = scene_render.cursor;
                                uint8 *packet_limit = scene_render.limit;
                                const uint32 packet_size = sizeof(DR_TPAGE) + sizeof(LINE_F2);
                                if (packet_limit - state_packet < (sint32)packet_size)
                                    return;
                                uint8 *line_packet = state_packet + sizeof(DR_TPAGE);
                                scene_render.cursor = state_packet + packet_size;
                                LINE_F2 *line = (LINE_F2 *)line_packet;
                                line->tag = 0;
                                setLineF2(line);
                                line->rgbc.rgbc = color;
                                uint32 forced_depth = points_effect->forced_depth;
                                uint32 *bucket = scene_render.cur->ot + (forced_depth ? forced_depth : depth);
                                line->p[0].xy = line_projection.screen[0];
                                line->p[1].xy = line_projection.screen[1];
                                LinkEffectPacket(line_packet, bucket);
                                if (color & 0x2000000)
                                    LinkEffectStatePacket(state_packet, bucket);
                            }
                            ++point;
                            line_start = line_end;
                        }
                    }

                    for (sprite_node = effect_lists.sprites;; sprite_node = sprite_node->next)
                    {
                        if (!sprite_node)
                            break;
                        const Sprite *sprite = static_cast<Sprite *>(sprite_node);
                        sprite_view_pos.x = ((uint32)((((sint32)(sprite->pos.words[0])) >> 12)) - (uint32)(view_pos.components[0]));
                        sprite_view_pos.y = ((uint32)((((sint32)(sprite->pos.words[1])) >> 12)) - (uint32)(view_pos.components[1]));
                        sprite_view_pos.z = ((uint32)((((sint32)(sprite->pos.words[2])) >> 12)) - (uint32)(view_pos.components[2]));
                        packed_vertex_xy = (((uint16)(sprite_view_pos.x)) | ((uint32)(((uint16)(sprite_view_pos.y))) << (uint32)(16)));
                        xport_gte_write_data(0, packed_vertex_xy);
                        xport_gte_write_data(1, sprite_view_pos.z);
                        ExecuteGeometryCommand(0x180001);
                        sprite_packet = scene_render.cursor;
                        uint8 *packet_limit = scene_render.limit;
                        const uint32 packet_size = sizeof(POLY_FT4);
                        if (packet_limit - sprite_packet < (sint32)packet_size)
                            return;
                        sprite_packet_end = sprite_packet + packet_size;
                        POLY_FT4 *poly = (POLY_FT4 *)sprite_packet;
                        poly->tag = 0;
                        setPolyFT4(poly);
                        sprite_color = sprite->color;
                        scene_render.cursor = sprite_packet_end;
                        poly->rgbc.rgbc = sprite_color;
                        sprite_depth = xport_gte_read_data(27);
                        result = (sprite_depth < viewport_near_depth);
                        if (sprite_depth >= viewport_near_depth && (result = (sint32)proj_cfg.max_distance < sprite_depth, !result) && ((proj_size = DivideEffectUnsigned((uint32)(sint32)(sint16)sprite->size << 7, (uint32)sprite_depth)) < 0x201 || (sprite->flags & 8) != 0))
                        {
                            scr_center = xport_gte_read_data(14);
                            sprite_tex = sprite->GetTextureFrame().tex;
                            uint32 palette_override = sprite->clut;
                            uint32 first_uv_word = PackTextureUv(*sprite_tex, 0);
                            uv_word1 = PackTextureUv(*sprite_tex, 1);
                            uv_word2 = PackTextureUv(*sprite_tex, 2);
                            uv_tail = sprite_tex->uv[3].uv;
                            if (palette_override)
                            {
                                uint32 packed_uv0 = (first_uv_word & 0xFFFF) | (palette_override << 16);
                                poly->p[0].uv.uv = packed_uv0;
                                poly->p[0].attr = packed_uv0 >> 16;
                            }
                            else
                            {
                                poly->p[0].uv.uv = first_uv_word;
                                poly->p[0].attr = first_uv_word >> 16;
                            }
                            blend_mode = sprite->blend_mode;
                            // The packed third UV word retains the fourth UV pair in the upper half
                            poly->p[2].uv.uv = uv_word2;
                            poly->p[2].attr = uv_word2 >> 16;
                            poly->p[3].uv.uv = uv_tail;
                            uint32 packed_uv1 = (uv_word1 & 0xFF9FFFFF) | (blend_mode << 16);
                            poly->p[1].uv.uv = packed_uv1;
                            poly->p[1].attr = packed_uv1 >> 16;
                            sprite_angle = sprite->angle;
                            if (proj_size >= 0x100 || sprite_angle)
                            {
                                // Preserve the original interleaved UV reads and decrements
                                uint32 first_uv = poly->p[1].uv.u;
                                uint32 second_uv = poly->p[2].uv.v;
                                poly->p[1].uv.u = first_uv - 1;
                                uint32 third_uv = poly->p[3].uv.u;
                                poly->p[2].uv.v = second_uv - 1;
                                uint32 fourth_uv = poly->p[3].uv.v;
                                poly->p[3].uv.u = third_uv - 1;
                                poly->p[3].uv.v = fourth_uv - 1;
                            }
                            scale_x = proj_size * sprite->scale[0];
                            scale_y = proj_size * sprite->scale[1];
                            sprite_frame = &sprite->GetTextureFrame();
                            uint32 sprite_flags = sprite->flags;
                            frame_w = sprite_frame->w;
                            frame_h = sprite_frame->h;
                            frame_oy = sprite_frame->oy;
                            flip_x = (sprite_flags & 1) != 0;
                            frame_ox = sprite_frame->ox;
                            uint32 left_idx = flip_x ? 1 : 0;
                            uint32 right_idx = 1 - left_idx;
                            if (sprite_angle)
                            {
                                trig_angle = sprite_angle & 0xFFF;
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
                            forced_depth = sprite->forced_depth;
                            if (forced_depth)
                                result = LinkForcedEffectPacket(sprite_packet, *sprite, forced_depth);
                            else
                                result = LinkBiasedEffectPacket(sprite_packet, *sprite, sprite_depth);
                        }
                        else
                        {
                            scene_render.cursor = sprite_packet;
                        }
                    }

                    for (fragment_node = effect_lists.opaque_fragments; fragment_node; fragment_node = fragment_node->next)
                    {
                        const OpaqueFragment *fragment = static_cast<const OpaqueFragment *>(fragment_node);
                        LoadEffectTriangleVertex(fragment->vertices[0], view_pos, 0);
                        LoadEffectTriangleVertex(fragment->vertices[1], view_pos, 2);
                        LoadEffectTriangleVertex(fragment->vertices[2], view_pos, 4);
                        ExecuteGeometryCommand(0x280030);
                        uint8 *poly_packet = scene_render.cursor;
                        uint8 *packet_limit = scene_render.limit;
                        const uint32 packet_size = sizeof(POLY_F3) + sizeof(DR_TPAGE);
                        if (packet_limit - poly_packet < (sint32)packet_size)
                            return;
                        uint8 *state_packet = poly_packet + sizeof(POLY_F3);
                        POLY_F3 *poly = (POLY_F3 *)poly_packet;
                        DR_TPAGE *state = (DR_TPAGE *)state_packet;
                        poly->tag = 0;
                        setPolyF3(poly);
                        scene_render.cursor = poly_packet + packet_size;
                        setSemiTrans(poly, 1);
                        poly->rgbc.r = fragment->draw_color.r;
                        poly->rgbc.g = fragment->draw_color.g;
                        poly->rgbc.b = fragment->draw_color.b;
                        uint32 state_tag = GetEffectDrawMode().tag;
                        uint32 state_code = GetEffectDrawMode().code[0];
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
                        ShiftVectorRight(world_pos.words, effect->endpoint[0].words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        uint32 packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        sint32 depth0 = xport_gte_read_data(27);
                        if (depth0 < (sint32)proj_cfg.near_depth || depth0 > (sint32)proj_cfg.max_distance)
                            continue;
                        uint32 scr0 = xport_gte_read_data(14);
                        ShiftVectorRight(world_pos.words, effect->endpoint[1].words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        sint32 depth1 = xport_gte_read_data(27);
                        if (depth1 < (sint32)proj_cfg.near_depth || depth1 > (sint32)proj_cfg.max_distance)
                            continue;
                        uint32 scr1 = xport_gte_read_data(14);
                        uint8 *line_packet = scene_render.cursor;
                        uint8 *packet_limit = scene_render.limit;
                        const uint32 packet_size = sizeof(LINE_G2);
                        if (packet_limit - line_packet < (sint32)packet_size)
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
                        if (!quad->GetTexture())
                            continue;
                        const uint32 world_shift = 12;
                        const uint32 packet_size = sizeof(POLY_FT4);
                        uint32 packed_xy;
                        Vector3 world_pos, relative_pos;
                        uint8 *poly_packet, *packet_limit;
                        uint32 uv_words[3];
                        uint16 uv_tail;
                        sint32 depth[4], max_depth;
                        POLY_FT4 *poly;
                        const TextureResource *tex;
                        ShiftVectorRight(world_pos.words, quad->pos.words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        poly_packet = scene_render.cursor;
                        packet_limit = scene_render.limit;
                        tex = quad->GetTexture();
                        if (packet_limit - poly_packet < (sint32)packet_size)
                            return;
                        scene_render.cursor = poly_packet + packet_size;
                        depth[0] = xport_gte_read_data(27);
                        if (depth[0] < (sint32)proj_cfg.near_depth || depth[0] > (sint32)proj_cfg.max_distance)
                            goto RejectTexturedQuad;
                        poly = (POLY_FT4 *)poly_packet;
                        poly->p[0].xy.xy = xport_gte_read_data(14);
                        ShiftVectorRight(world_pos.words, quad->corners[0].words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        depth[1] = xport_gte_read_data(27);
                        if (depth[1] < (sint32)proj_cfg.near_depth || depth[1] > (sint32)proj_cfg.max_distance)
                            goto RejectTexturedQuad;
                        poly->p[1].xy.xy = xport_gte_read_data(14);
                        ShiftVectorRight(world_pos.words, quad->corners[1].words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
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
                        ShiftVectorRight(world_pos.words, quad->corners[2].words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        uv_words[0] = PackTextureUv(*tex, 0);
                        uv_words[1] = PackTextureUv(*tex, 1);
                        uv_words[2] = PackTextureUv(*tex, 2);
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

                    for (EffectObject *triangle_node = effect_lists.textured_triangles; triangle_node; triangle_node = triangle_node->next)
                    {
                        const TexturedTriangle *triangle = static_cast<const TexturedTriangle *>(triangle_node);
                        const uint32 packet_size = sizeof(POLY_FT3);
                        const uint32 max_packet_bytes = 2 * sizeof(DR_TWIN) + 4 * sizeof(POLY_FT3);
                        const uint8 face_vertices[4][3] = {{0, 1, 2}, {0, 3, 1}, {1, 3, 2}, {2, 3, 0}};
                        uint8 *packet_cursor = scene_render.cursor;
                        uint8 *packet_limit = scene_render.limit;
                        uint32 scr[4], packed_xy;
                        Vector3 relative_pos;
                        uint32 *bucket;
                        uint8 *window_packet[2], *poly_cursor;
                        sint32 depth[4], max_depth = -1;
                        if (packet_limit - packet_cursor < (sint32)max_packet_bytes)
                            return;
                        relative_pos.words[0] = (uint32)triangle->transformed[0].x - (uint32)view_pos.components[0];
                        relative_pos.words[1] = (uint32)triangle->transformed[0].y - (uint32)view_pos.components[1];
                        relative_pos.words[2] = (uint32)triangle->transformed[0].z - (uint32)view_pos.components[2];
                        packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        depth[0] = xport_gte_read_data(27);
                        scr[0] = xport_gte_read_data(14);
                        for (uint32 idx = 1; idx < 4; ++idx)
                        {
                            const Vector3 &pos = triangle->transformed[idx];
                            relative_pos.words[0] = (uint32)pos.x - (uint32)view_pos.components[0];
                            relative_pos.words[1] = (uint32)pos.y - (uint32)view_pos.components[1];
                            relative_pos.words[2] = (uint32)pos.z - (uint32)view_pos.components[2];
                            packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                            xport_gte_write_data(0, packed_xy);
                            xport_gte_write_data(1, relative_pos.words[2]);
                            ExecuteGeometryCommand(0x180001);
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
                            ExecuteGeometryCommand(0x1400006);
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
                        const TILE &primitive = particle->primitive;
                        const uint32 packed_size = (uint16)primitive.w | ((uint32)(uint16)primitive.h << 16);
                        const uint32 world_shift = 12;
                        Vector3 world_pos, relative_pos;
                        ShiftVectorRight(world_pos.words, particle->pos.words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        uint32 packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        uint8 *packet = scene_render.cursor;
                        uint8 *packet_limit = scene_render.limit;
                        XY *scr_pos;
                        if ((packed_size & 15) == 1)
                        {
                            const uint32 packet_size = sizeof(TILE_1);
                            if (packet_limit - packet < (sint32)packet_size)
                                return;
                            TILE_1 *tile = (TILE_1 *)packet;
                            uint32 tag = primitive.tag;
                            scene_render.cursor = packet + packet_size;
                            // Copy the initialized template including its command and service bits
                            tile->tag = tag;
                            tile->rgbc.rgbc = primitive.rgbc.rgbc;
                            scr_pos = &tile->p[0];
                        }
                        else
                        {
                            const uint32 packet_size = sizeof(TILE);
                            if (packet_limit - packet < (sint32)packet_size)
                                return;
                            TILE *tile = (TILE *)packet;
                            uint32 tag = primitive.tag;
                            scene_render.cursor = packet + packet_size;
                            tile->tag = tag;
                            tile->rgbc.rgbc = primitive.rgbc.rgbc;
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
                    for (EffectObject *ribbon_node = effect_lists.endpoint_sprites; ribbon_node; ribbon_node = ribbon_node->next)
                    {
                        const EndpointSprite *ribbon = static_cast<EndpointSprite *>(ribbon_node);
                        bool new_start = (ribbon->flags & 0x10) || force_start;
                        const uint32 world_shift = 12;
                        const uint32 packet_size = sizeof(POLY_FT4);
                        Vector3 world_pos, relative_pos;
                        uint32 packed_xy, start_width;
                        RibbonScreenPoint start_pos;
                        if (new_start)
                        {
                            ShiftVectorRight(world_pos.words, ribbon->endpoint[0].words, world_shift);
                            SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                            packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                            xport_gte_write_data(0, packed_xy);
                            xport_gte_write_data(1, relative_pos.words[2]);
                            ExecuteGeometryCommand(0x180001);
                        }
                        else
                            start_pos = prev_center;
                        const TextureFrame &frame = ribbon->GetTextureFrame();
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
                            start_width = DivideEffectUnsigned((uint32)(sint32)(sint16)ribbon->size * frame_width, depth) >> 1;
                            if (start_width < 2)
                                start_width = 2;
                        }
                        ShiftVectorRight(world_pos.words, ribbon->endpoint[1].words, world_shift);
                        SubtractVectors(relative_pos.words, world_pos.words, view_pos.words);
                        packed_xy = (relative_pos.words[0] & 0xFFFF) | ((relative_pos.words[1] & 0xFFFF) << 16);
                        xport_gte_write_data(0, packed_xy);
                        xport_gte_write_data(1, relative_pos.words[2]);
                        ExecuteGeometryCommand(0x180001);
                        uint8 *packet = scene_render.cursor;
                        uint8 *packet_limit = scene_render.limit;
                        if (packet_limit - packet < (sint32)packet_size)
                            return;
                        POLY_FT4 *poly = (POLY_FT4 *)packet;
                        poly->tag = 0;
                        setPolyFT4(poly);
                        uint32 color = ribbon->color;
                        scene_render.cursor = packet + packet_size;
                        poly->rgbc.rgbc = color;
                        uint32 uv0 = PackTextureUv(*tex, 0);
                        uint32 uv1 = PackTextureUv(*tex, 1);
                        uint32 uv2 = PackTextureUv(*tex, 2);
                        uint16 uv_tail = tex->uv[3].uv;
                        poly->p[0].uv.uv = uv0;
                        poly->p[0].attr = uv0 >> 16;
                        poly->p[1].uv.uv = uv1;
                        poly->p[1].attr = uv1 >> 16;
                        uint32 first_uv = poly->p[1].uv.u;
                        // Preserve the fourth UV pair in the third packed UV word
                        poly->p[2].uv.uv = uv2;
                        poly->p[2].attr = uv2 >> 16;
                        uint32 second_uv = poly->p[2].uv.v;
                        poly->p[3].uv.uv = uv_tail;
                        poly->p[1].uv.u = first_uv - 1;
                        uint32 third_uv = poly->p[3].uv.u;
                        poly->p[2].uv.v = second_uv - 1;
                        uint32 fourth_uv = poly->p[3].uv.v - 1;
                        poly->p[3].uv.u = third_uv - 1;
                        poly->p[3].uv.v = fourth_uv;
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
                        uint32 width_product = (uint32)(sint32)(sint16)ribbon->size * frame_width;
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
                            sint32 offset_x = DivideEffectSigned((uint32)dy * start_width, len);
                            sint32 offset_y = DivideEffectSigned((uint32)dx * start_width, len);
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
                            sint32 offset_x = DivideEffectSigned(end_product, len);
                            sint32 offset_y = DivideEffectSigned((uint32)dx * end_width, len);
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
                        if (((flags & 2) != 0))
                        {
                            if (((flags & 1) != 0))
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
                        else if (((flags & 1) != 0))
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
                    return;
                }
                const SegmentEffect *segments = static_cast<SegmentEffect *>(segment_node);
                uint32 origin_x = segments->origin.words[0];
                uint32 prev_color = segments->rgbc.rgbc;
                line_start.x = origin_x;
                line_start.y = segments->origin.words[1];
                line_start.z = segments->origin.words[2];
                segment_point = segments->points;
                for (sint32 seg_idx = 0; seg_idx < (sint32)segments->point_count; ++seg_idx)
                {
                    uint32 endpoint_x = segment_point->pos.words[0];
                    uint32 endpoint_color = segment_point->rgbc.rgbc;
                    line_end.x = endpoint_x;
                    line_end.y = segment_point->pos.words[1];
                    line_end.z = segment_point->pos.words[2];
                    sint32 depth = ProjectAndClipWorldLine(line_start, line_end, line_projection);
                    if (depth >= 0)
                    {
                        uint8 *state_packet = scene_render.cursor;
                        uint8 *packet_limit = scene_render.limit;
                        const uint32 packet_size = sizeof(DR_TPAGE) + sizeof(LINE_G2);
                        if (packet_limit - state_packet < (sint32)packet_size)
                            return;
                        uint8 *line_packet = state_packet + sizeof(DR_TPAGE);
                        scene_render.cursor = state_packet + packet_size;
                        LINE_G2 *line = (LINE_G2 *)line_packet;
                        line->tag = 0;
                        setLineG2(line);
                        line->p[0].rgbc.rgbc = prev_color;
                        line->p[1].rgbc.rgbc = endpoint_color;
                        uint32 forced_depth = segments->forced_depth;
                        uint32 *bucket = scene_render.cur->ot + (forced_depth ? forced_depth : depth);
                        line->p[0].xy.xy = line_projection.screen[0];
                        line->p[1].xy.xy = line_projection.screen[1];
                        LinkEffectPacket(line_packet, bucket);
                        bool semi_trans = (prev_color & 0x2000000) != 0;
                        prev_color = endpoint_color;
                        if (semi_trans)
                            LinkEffectStatePacket(state_packet, bucket);
                    }
                    prev_color = endpoint_color;
                    ++segment_point;
                    line_start = line_end;
                }
            }
        }
        const RingEffect *ring = static_cast<RingEffect *>(ring_node);
        XY *prev_ray, *cur_ray, *write_pos, *cur_row, *prev_row, *swap_ray;
        const uint32 state_size = sizeof(DR_TPAGE);
        const uint32 tri_size = sizeof(POLY_G3);
        const uint32 quad_size = sizeof(POLY_G4);
        world_shift = 12;
        ShiftVectorRight(ring_world_pos.words, ring->pos.words, world_shift);
        SubtractVectors(ring_relative_pos.words, ring_world_pos.words, view_pos.words);
        packed_vertex_xy = (ring_relative_pos.words[0] & 0xFFFF) | ((ring_relative_pos.words[1] & 0xFFFF) << 16);
        xport_gte_write_data(0, packed_vertex_xy);
        xport_gte_write_data(1, ring_relative_pos.words[2]);
        ExecuteGeometryCommand(0x180001);
        state_packet = scene_render.cursor;
        uint8 *state_limit = scene_render.limit;
        result = (state_limit - state_packet < (sint32)state_size);
        if ((state_limit - state_packet < (sint32)state_size))
            break;
        state_end = state_packet + state_size;
        uint32 ring_state = GetEffectDrawMode().tag;
        state_code = GetEffectDrawMode().code[0];
        DR_TPAGE *state = (DR_TPAGE *)state_packet;
        state->tag = ring_state;
        state->code[0] = state_code;
        ray_angle = ((sint16)(ring->angle));
        ring_ray = ring->ring_points;
        ray_count = ring->point_count;
        scene_render.cursor = state_end;
        angle_step = ((sint16)(ring->angle_step));
        ring_view_pos.x = xport_gte_read_data(25);
        ring_view_pos.y = xport_gte_read_data(26);
        ring_view_pos.z = xport_gte_read_data(27);
        result = (ring_view_pos.z < 700);
        if (ring_view_pos.z >= 0 && ring_view_pos.z <= 20000)
        {
            near_fade = 0;
            if ((ring_view_pos.z < 700))
                near_fade = 255 * ring_view_pos.z / 700 + 1;
            if ((ring_view_pos.z >= viewport_near_depth))
            {
                uint32 projected_pair = xport_gte_read_data(14);
                projected_x_word = (uint16)projected_pair;
                projected_y_word = (uint16)(projected_pair >> 16);
                center_scr_x = projected_x_word;
                center_scr_y = projected_y_word;
            }
            else
            {
                const ModelViewport *ring_view = model_render_state.viewport;
                uint32 ring_height = ring_view->distance;
                scaled_view_x = ring_height * (uint32)ring_view_pos.x;
                sint32 projection_x = DivideEffectSigned((uint32)scaled_view_x, (uint32)ring_view_pos.z);
                center_x_word = (uint32)ring_view->center_x + (uint32)projection_x;
                center_scr_x = (uint16)center_x_word;
                if ((sint16)center_scr_x >= 1024)
                    center_scr_x = 1023;
                else if ((sint16)center_scr_x < -1024)
                    center_scr_x = (uint16)-1024;
                ring_view = model_render_state.viewport;
                ring_height = ring_view->distance;
                scaled_view_y = ring_height * (uint32)ring_view_pos.y;
                sint32 projection_y = DivideEffectSigned((uint32)scaled_view_y, (uint32)ring_view_pos.z);
                center_y_word = (uint32)ring_view->center_y + (uint32)projection_y;
                center_scr_y = (uint16)center_y_word;
                if ((sint16)center_scr_y >= 1024)
                    center_scr_y = 1023;
                else if ((sint16)center_scr_y < -1024)
                    center_scr_y = (uint16)-1024;
            }
            result = ((uint32)(center_scr_x) << (uint32)(16));
            center_x = ((sint16)(center_scr_x));
            if (((((sint16)(center_scr_x)) != -1024) && (center_scr_x != 1023)))
            {
                result = ((uint32)(center_scr_y) << (uint32)(16));
                center_y = ((sint16)(center_scr_y));
                if (((((sint16)(center_scr_y)) != -1024) && (center_scr_y != 1023)))
                {
                    prev_ray = ring->proj_rows[0];
                    trig_angle = ray_angle & 0xFFF;
                    cur_ray = ring->proj_rows[1];

                    ring_ray_end = ring_ray + ray_count;
                    sint32 ring_sine = rsin(trig_angle);
                    ot_bucket = scene_render.cur->ot + (((uint32)ring_view_pos.z & 0x3FFC) >> 2);
                    seed_radius_proj = DivideEffectUnsigned(400 * ring_ray_end[-1].distance, (uint32)ring_view_pos.z);
                    s_scale = (400 * ring_sine) / 256;
                    c_scale = rcos(trig_angle);
                    seed_x = center_x + (sint32)((uint32)seed_radius_proj * (uint32)s_scale) / 4096;
                    seed_y = center_y - (sint32)((uint32)seed_radius_proj * (uint32)c_scale) / 4096;
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
                    seed_radius = ((sint32)(ring_ray_end[-1].distance));
                    while (1)
                    {
                        ray_idx = 0;
                        if ((seed_row_idx >= ring->row_count))
                            break;
                        seed_radius = (sint32)((uint32)seed_radius + ring_row->distance);
                        seed_row_proj = DivideEffectSigned(400 * (uint32)seed_radius, (uint32)ring_view_pos.z);
                        seed_row_x = center_x + (sint32)((uint32)seed_row_proj * (uint32)s_scale) / 4096;
                        seed_row_y = center_y - (sint32)((uint32)seed_row_proj * (uint32)c_scale) / 4096;
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

                    ray_angle = (sint32)((uint32)ray_angle + (uint32)angle_step);
                    ring_color = ring_ray;
                    ring_row_stride = ray_count;
                    while ((ray_idx < ray_count))
                    {
                        seg_enabled = ((ring->seg_mask >> ((uint32)ray_idx & 31)) & 1);
                        if ((ray_idx == ((sint32)((uint32)(ray_count) - (uint32)(1)))))
                            ray_angle = ((sint16)(ring->angle));
                        trig_angle = ray_angle & 0xFFF;
                        sint32 ray_sine = rsin(trig_angle);
                        ray_radius_proj = DivideEffectUnsigned(400 * ring_ray->distance, (uint32)ring_view_pos.z);
                        s_scale = (400 * ray_sine) / 256;
                        c_scale = rcos(trig_angle);
                        ray_x = center_x + (sint32)((uint32)ray_radius_proj * (uint32)s_scale) / 4096;
                        ray_y = center_y - (sint32)((uint32)ray_radius_proj * (uint32)c_scale) / 4096;
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
                        if ((!(ring->hide_center) && seg_enabled))
                        {
                            tri_packet = scene_render.cursor;
                            uint8 *tri_limit = scene_render.limit;
                            result = 100663296;
                            if (tri_limit - tri_packet < (sint32)tri_size)
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
                            poly0->p[0].xy.x = center_scr_x;
                            poly0->p[0].xy.y = center_scr_y;
                            poly0->p[1].xy.x = prev_ray[0].x;
                            poly0->p[1].xy.y = prev_ray[0].y;
                            poly0->p[2].xy.x = cur_ray[0].x;
                            poly0->p[2].xy.y = cur_ray[0].y;
                            (void)LinkEffectPacket(tri_packet, ot_bucket);
                            row_idx = 0;
                        }
                        cur_row = cur_ray;
                        prev_row = prev_ray;
                        row_radius = ((sint32)(ring_ray->distance));
                        ring_row = ring->row_count ? ring->row_points + ray_idx : 0;
                        while ((row_idx < ring->row_count))
                        {
                            row_radius = (sint32)((uint32)row_radius + ring_row->distance);
                            row_radius_proj = DivideEffectSigned(400 * (uint32)row_radius, (uint32)ring_view_pos.z);
                            row_x = center_x + (sint32)((uint32)row_radius_proj * (uint32)s_scale) / 4096;
                            row_y = center_y - (sint32)((uint32)row_radius_proj * (uint32)c_scale) / 4096;
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
                                result = 0x8000000;
                                if (quad_limit - quad_packet < (sint32)quad_size)
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
                                poly1->tag = ((quad_tag & 0xFF000000) | (*ot_bucket & 0xFFFFFF));
                                *ot_bucket = ((*ot_bucket & 0xFF000000) | gpu_packet_link((uintptr_t)quad_packet));
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
                        ray_angle = (sint32)((uint32)ray_angle + (uint32)angle_step);
                        ++ring_ray;
                    }

                    ring_state_head = *ot_bucket;
                    state->tag = (state->tag & 0xFF000000) | (ring_state_head & 0xFFFFFF);
                    result = ((*ot_bucket & 0xFF000000) | gpu_packet_link((uintptr_t)state_packet));
                    *ot_bucket = result;
                }
            }
        }
    }

    return;
}
