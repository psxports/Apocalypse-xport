#include "model_render.h"
#include "model_projection.h"
#include "scene_lifecycle.h"
#include "model_materials.h"
#include "model_geometry.h"
#include "game_diagnostics.h"

void EmitScratchModelPackets(const ModelPolygon *desc, uint32 count)
{
    uint32 *ordering, *dst;
    uint8 *packet, *limit, *next;
    uint32 scr[4];
    const ModelScratchVertex *points[4];
    uint32 flags, idx, fog, color_indices, color, opcode, len;
    sint16 fog_start, shift;
    uint32 uv0, uv1, uv2, prev, extra;
    sint32 winding, depth, cur_depth;
    FUNCTION_MARKER(0x80083F38, "SLUS_003.73");
    if (!count)
        return;
    ordering = model_render_state.ordering_table;
    packet = scene_render.cursor;
    const ModelPalette *colors = GetCurModelPal();
    limit = scene_render.limit - 100;
    do
    {
        for (idx = 0; idx < 4; ++idx)
            points[idx] = &GetScratchProjVertex(desc->v[idx]);
        scr[0] = points[0]->scr;
        scr[1] = points[1]->scr;
        scr[2] = points[2]->scr;
        xport_gte_write_data(12, scr[0]);
        xport_gte_write_data(13, scr[1]);
        xport_gte_write_data(14, scr[2]);
        xport_gte_execute(0x1400006);
        scr[3] = points[3]->scr;
        flags = ((uint32)desc->src_stride << 16) | desc->flags;
        if (!(scr[0] & scr[1] & scr[2] & scr[3] & 0x60006000))
        {
            winding = xport_gte_read_data(24);
            xport_gte_write_data(15, scr[3]);
            xport_gte_execute(0x1400006);
            if (flags & 0xC0)
            {
                if (winding <= 0)
                    winding = xport_gte_read_data(24);
                else
                    goto Visible;
                if (winding < 0)
                {
                Visible:
                    P_TAG *header = (P_TAG *)packet;
                    POINT_C *vertex0 = (POINT_C *)(packet + sizeof(uint32));
                    vertex0->xy.xy = scr[0];
                    if (packet >= limit)
                    {
                        // The original exits through a neighboring private-frame epilogue
                        scene_render.cursor = packet;
                        return;
                    }
                    depth = points[0]->depth;
                    for (idx = 1; idx < 4; ++idx)
                    {
                        if (depth < points[idx]->depth)
                            depth = points[idx]->depth;
                    }
                    fog_start = model_render_state.draw.fog_start;
                    shift = model_render_state.draw.fog_shift;
                    cur_depth = depth - fog_start;
                    fog = 0;
                    if (cur_depth > 0)
                    {
                        if (shift < 0)
                            fog = cur_depth >> (-shift & 31);
                        else
                            fog = (uint32)cur_depth << (shift & 31);
                    }
                    xport_gte_write_data(8, fog);
                    if (flags & 0x800)
                    {
                        color_indices = desc->color;
                        xport_gte_write_data(20, ReadModelPalColor(colors, color_indices & 255));
                        xport_gte_write_data(21, ReadModelPalColor(colors, (color_indices >> 8) & 255));
                        xport_gte_write_data(22, ReadModelPalColor(colors, (color_indices >> 16) & 255));
                        xport_gte_execute(0xF8002A);
                        if (flags & 1)
                        {
                            POLY_GT4 *poly = (POLY_GT4 *)packet;
                            poly->p[1].xy.xy = scr[1];
                            poly->p[2].xy.xy = scr[2];
                            uv0 = PackModelPolygonUv(*desc, 0);
                            uv1 = PackModelPolygonUv(*desc, 1);
                            uv2 = PackModelPolygonUv(*desc, 2);
                            uv1 |= (flags & 0x180) << 14;
                            poly->p[0].uv.uv = uv0;
                            poly->p[0].attr = uv0 >> 16;
                            poly->p[1].uv.uv = uv1;
                            poly->p[1].attr = uv1 >> 16;
                            poly->p[2].uv.uv = uv2;
                            poly->p[2].attr = uv2 >> 16;
                            xport_gte_write_data(6, ReadModelPalColor(colors, color_indices >> 24));
                            color = xport_gte_read_data(20);
                            poly->p[1].rgbc.rgbc = xport_gte_read_data(21);
                            poly->p[2].rgbc.rgbc = xport_gte_read_data(22);
                            xport_gte_execute(0x780010);
                            if (flags & 16)
                            {
                                opcode = 0x34000000;
                                len = 0x09000000;
                            }
                            else
                            {
                                poly->p[3].xy.xy = scr[3];
                                poly->p[3].uv.uv = uv2 >> 16;
                                poly->p[3].attr = 0;
                                opcode = 0x3C000000;
                                poly->p[3].rgbc.rgbc = xport_gte_read_data(22);
                                len = 0x0C000000;
                            }
                        }
                        else
                        {
                            POLY_G4 *poly = (POLY_G4 *)packet;
                            poly->p[1].xy.xy = scr[1];
                            poly->p[2].xy.xy = scr[2];
                            xport_gte_write_data(6, ReadModelPalColor(colors, color_indices >> 24));
                            color = xport_gte_read_data(20);
                            poly->p[1].rgbc.rgbc = xport_gte_read_data(21);
                            poly->p[2].rgbc.rgbc = xport_gte_read_data(22);
                            xport_gte_execute(0x780010);
                            if (flags & 16)
                            {
                                opcode = 0x30000000;
                                len = 0x06000000;
                            }
                            else
                            {
                                poly->p[3].xy.xy = scr[3];
                                opcode = 0x38000000;
                                poly->p[3].rgbc.rgbc = xport_gte_read_data(22);
                                len = 0x08000000;
                            }
                        }
                    }
                    else
                    {
                        xport_gte_write_data(6, desc->color);
                        xport_gte_execute(0x780010);
                        if (flags & 1)
                        {
                            uv0 = PackModelPolygonUv(*desc, 0);
                            uv1 = PackModelPolygonUv(*desc, 1);
                            uv2 = PackModelPolygonUv(*desc, 2);
                            POLY_FT4 *poly = (POLY_FT4 *)packet;
                            poly->p[1].xy.xy = scr[1];
                            poly->p[2].xy.xy = scr[2];
                            poly->p[0].uv.uv = uv0;
                            poly->p[0].attr = uv0 >> 16;
                            poly->p[1].uv.uv = uv1 | ((flags & 0x180) << 14);
                            poly->p[1].attr = (uv1 | ((flags & 0x180) << 14)) >> 16;
                            poly->p[2].uv.uv = uv2;
                            poly->p[2].attr = uv2 >> 16;
                            color = xport_gte_read_data(22);
                            if (flags & 16)
                            {
                                opcode = 0x24000000;
                                len = 0x07000000;
                            }
                            else
                            {
                                poly->p[3].xy.xy = scr[3];
                                poly->p[3].uv.uv = uv2 >> 16;
                                poly->p[3].attr = 0;
                                opcode = 0x2C000000;
                                len = 0x09000000;
                            }
                        }
                        else
                        {
                            POLY_F4 *poly = (POLY_F4 *)packet;
                            poly->p[1].xy = scr[1];
                            poly->p[2].xy = scr[2];
                            color = xport_gte_read_data(22);
                            if (flags & 16)
                            {
                                opcode = 0x20000000;
                                len = 0x04000000;
                            }
                            else
                            {
                                poly->p[3].xy = scr[3];
                                opcode = 0x28000000;
                                len = 0x05000000;
                            }
                        }
                    }
                    header->rgbc.rgbc = color | opcode | ((flags & 64) << 19);
                    if (flags & 0x6000)
                    {
                        sint16 bias = model_render_state.draw.near_bias;
                        if (!(flags & 0x2000))
                            bias = model_render_state.draw.far_bias;
                        depth += bias;
                    }
                    dst = ordering + ((depth >= 0 && depth < 0x4000 ? depth & 0xFFFC : 0x3FFC) >> 2);
                    prev = *dst;
                    if (flags & 32)
                    {
                        len += 0x03000000;
                        extra = GetModelPolygonWindow(*desc);
                        header->addr = prev;
                        header->len = (prev | len) >> 24;
                        next = packet + (len >> 22) + 4;
                        *dst = gpu_packet_link((uintptr_t)next);
                        DR_TWIN *reset_window = (DR_TWIN *)(next - sizeof(DR_TWIN));
                        reset_window->tag = 0;
                        reset_window->code[0] = 0xE2000000;
                        reset_window->code[1] = 0;
                        DR_TWIN *apply_window = (DR_TWIN *)next;
                        apply_window->tag = gpu_packet_link((uintptr_t)packet) | 0x02000000;
                        apply_window->code[0] = extra;
                        apply_window->code[1] = 0;
                        packet = next + 12;
                    }
                    else
                    {
                        *dst = gpu_packet_link((uintptr_t)packet);
                        header->addr = prev;
                        header->len = (prev | len) >> 24;
                        next = packet + (len >> 22) + 4;
                        if ((flags & 0x41) == 0x40)
                        {
                            DR_TPAGE *mode = (DR_TPAGE *)next;
                            mode->code[0] = ((flags & 0x180) >> 2) | 0xE1000200;
                            mode->tag = gpu_packet_link((uintptr_t)packet) | 0x01000000;
                            *dst = gpu_packet_link((uintptr_t)next);
                            next += 8;
                        }
                        packet = next;
                    }
                }
            }
        }
        desc = desc->next;
        --count;
    } while (count);
    scene_render.cursor = packet;
}
