#include "model_render.h"
#include "model_projection.h"
#include "scene_lifecycle.h"
#include "model_materials.h"
#include "model_geometry.h"
#include "game_diagnostics.h"

void EmitScratchModelPackets(const ModelPolygon *descriptor, uint32 count)
{
    uint32 *ordering, *destination;
    uint8 *packet, *limit, *next;
    uint32 indices0, indices1, screen[4];
    const ModelScratchVertex *points[4];
    uint32 flags, index, depth, value, fog, shift, color_indices, color, opcode, length;
    uint32 uv0, uv1, uv2, previous, extra;
    sint32 winding, current_depth;
    FUNCTION_MARKER(0x80083F38, "SLUS_003.73");
    if (!count)
        return;
    ordering = model_render_state.ordering_table;
    packet = scene_render.cursor;
    const ModelPalette *colors = GetCurrentModelPalette();
    limit = scene_render.limit - 100;
    do
    {
        indices0 = (((uint32)descriptor->v[0] << 3) | ((uint32)descriptor->v[1] << 19));
        indices1 = (((uint32)descriptor->v[2] << 3) | ((uint32)descriptor->v[3] << 19));
        points[0] = &GetScratchProjectionVertex((indices0 & 65535) >> 3);
        points[1] = &GetScratchProjectionVertex((indices0 >> 16) >> 3);
        points[2] = &GetScratchProjectionVertex((indices1 & 65535) >> 3);
        points[3] = &GetScratchProjectionVertex((indices1 >> 16) >> 3);
        screen[0] = points[0]->screen;
        screen[1] = points[1]->screen;
        screen[2] = points[2]->screen;
        xport_gte_write_data(12, screen[0]);
        xport_gte_write_data(13, screen[1]);
        xport_gte_write_data(14, screen[2]);
        xport_gte_execute(0x1400006);
        screen[3] = points[3]->screen;
        flags = (((uint32)descriptor->source_stride << 16) | descriptor->flags);
        if (!(screen[0] & screen[1] & screen[2] & screen[3] & 0x60006000))
        {
            winding = (sint32)xport_gte_read_data(24);
            xport_gte_write_data(15, screen[3]);
            xport_gte_execute(0x1400006);
            if (flags & 0xC0)
            {
                if (winding <= 0)
                    winding = (sint32)xport_gte_read_data(24);
                else
                    goto Visible;
                if (winding < 0)
                {
                Visible:
                    P_TAG *header = (P_TAG *)packet;
                    POINT_C *first_point = (POINT_C *)(packet + sizeof(uint32));
                    first_point->xy.xy = screen[0];
                    if (packet >= limit)
                    {
                        // The original exits through a neighboring private-frame epilogue
                        scene_render.cursor = packet;
                        return;
                    }
                    depth = (uint32)(sint32)points[0]->depth;
                    for (index = 1; index < 4; ++index)
                    {
                        value = (uint32)(sint32)points[index]->depth;
                        if ((sint32)(depth - value) < 0)
                            depth = value;
                    }
                    value = (uint32)(sint32)(sint16)(uint16)model_render_state.draw.fog_start;
                    shift = (uint32)(sint32)(sint16)(uint16)model_render_state.draw.fog_shift;
                    current_depth = (sint32)(depth - value);
                    fog = 0;
                    if (current_depth > 0)
                    {
                        if ((sint32)shift < 0)
                            fog = (uint32)(current_depth >> ((0 - shift) & 31));
                        else
                            fog = (uint32)current_depth << (shift & 31);
                    }
                    xport_gte_write_data(8, fog);
                    if (flags & 0x800)
                    {
                        color_indices = descriptor->color;
                        xport_gte_write_data(20, ReadModelPaletteColor(colors, ((color_indices << 2) & 0x3FC) >> 2));
                        xport_gte_write_data(21, ReadModelPaletteColor(colors, ((color_indices >> 6) & 0x3FC) >> 2));
                        xport_gte_write_data(22, ReadModelPaletteColor(colors, ((color_indices >> 14) & 0x3FC) >> 2));
                        xport_gte_execute(0xF8002A);
                        if (flags & 1)
                        {
                            POLY_GT4 *poly = (POLY_GT4 *)packet;
                            poly->p[1].xy.xy = screen[1];
                            poly->p[2].xy.xy = screen[2];
                            uv0 = PackModelPolygonUv(*descriptor, 0);
                            uv1 = PackModelPolygonUv(*descriptor, 1);
                            uv2 = PackModelPolygonUv(*descriptor, 2);
                            uv1 |= (flags & 0x180) << 14;
                            poly->p[0].uv.uv = uv0;
                            poly->p[0].attr = (uv0) >> 16;
                            poly->p[1].uv.uv = uv1;
                            poly->p[1].attr = (uv1) >> 16;
                            poly->p[2].uv.uv = uv2;
                            poly->p[2].attr = (uv2) >> 16;
                            xport_gte_write_data(6, ReadModelPaletteColor(colors, ((color_indices >> 22) & 0x3FC) >> 2));
                            color = xport_gte_read_data(20);
                            poly->p[1].rgbc.rgbc = xport_gte_read_data(21);
                            poly->p[2].rgbc.rgbc = xport_gte_read_data(22);
                            xport_gte_execute(0x780010);
                            if (flags & 16)
                            {
                                opcode = 0x34000000;
                                length = 0x09000000;
                            }
                            else
                            {
                                poly->p[3].xy.xy = screen[3];
                                poly->p[3].uv.uv = uv2 >> 16;
                                poly->p[3].attr = (uv2 >> 16) >> 16;
                                opcode = 0x3C000000;
                                poly->p[3].rgbc.rgbc = xport_gte_read_data(22);
                                length = 0x0C000000;
                            }
                        }
                        else
                        {
                            POLY_G4 *poly = (POLY_G4 *)packet;
                            poly->p[1].xy.xy = screen[1];
                            poly->p[2].xy.xy = screen[2];
                            xport_gte_write_data(6, ReadModelPaletteColor(colors, ((color_indices >> 22) & 0x3FC) >> 2));
                            color = xport_gte_read_data(20);
                            poly->p[1].rgbc.rgbc = xport_gte_read_data(21);
                            poly->p[2].rgbc.rgbc = xport_gte_read_data(22);
                            xport_gte_execute(0x780010);
                            if (flags & 16)
                            {
                                opcode = 0x30000000;
                                length = 0x06000000;
                            }
                            else
                            {
                                poly->p[3].xy.xy = screen[3];
                                opcode = 0x38000000;
                                poly->p[3].rgbc.rgbc = xport_gte_read_data(22);
                                length = 0x08000000;
                            }
                        }
                    }
                    else
                    {
                        xport_gte_write_data(6, descriptor->color);
                        xport_gte_execute(0x780010);
                        if (flags & 1)
                        {
                            uv0 = PackModelPolygonUv(*descriptor, 0);
                            uv1 = PackModelPolygonUv(*descriptor, 1);
                            uv2 = PackModelPolygonUv(*descriptor, 2);
                            POLY_FT4 *poly = (POLY_FT4 *)packet;
                            poly->p[1].xy.xy = screen[1];
                            poly->p[2].xy.xy = screen[2];
                            poly->p[0].uv.uv = uv0;
                            poly->p[0].attr = (uv0) >> 16;
                            poly->p[1].uv.uv = uv1 | ((flags & 0x180) << 14);
                            poly->p[1].attr = (uv1 | ((flags & 0x180) << 14)) >> 16;
                            poly->p[2].uv.uv = uv2;
                            poly->p[2].attr = (uv2) >> 16;
                            color = xport_gte_read_data(22);
                            if (flags & 16)
                            {
                                opcode = 0x24000000;
                                length = 0x07000000;
                            }
                            else
                            {
                                poly->p[3].xy.xy = screen[3];
                                poly->p[3].uv.uv = uv2 >> 16;
                                poly->p[3].attr = (uv2 >> 16) >> 16;
                                opcode = 0x2C000000;
                                length = 0x09000000;
                            }
                        }
                        else
                        {
                            POLY_F4 *poly = (POLY_F4 *)packet;
                            poly->p[1].xy = screen[1];
                            poly->p[2].xy = screen[2];
                            color = xport_gte_read_data(22);
                            if (flags & 16)
                            {
                                opcode = 0x20000000;
                                length = 0x04000000;
                            }
                            else
                            {
                                poly->p[3].xy = screen[3];
                                opcode = 0x28000000;
                                length = 0x05000000;
                            }
                        }
                    }
                    header->rgbc.rgbc = color | opcode | ((flags & 64) << 19);
                    if (flags & 0x6000)
                    {
                        extra = (uint32)(sint32)(sint16)(uint16)model_render_state.draw.near_bias;
                        if (!(flags & 0x2000))
                            extra = (uint32)(sint32)(sint16)(uint16)model_render_state.draw.far_bias;
                        depth += extra;
                    }
                    destination = ordering + ((depth < 0x4000 ? depth & 0xFFFC : 0x3FFC) >> 2);
                    previous = *destination;
                    if (flags & 32)
                    {
                        length += 0x03000000;
                        extra = GetModelPolygonWindow(*descriptor);
                        header->addr = previous;
                        header->len = (previous | length) >> 24;
                        next = packet + (length >> 22) + 4;
                        *destination = gpu_packet_link((uintptr_t)next);
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
                        *destination = gpu_packet_link((uintptr_t)packet);
                        header->addr = previous;
                        header->len = (previous | length) >> 24;
                        next = packet + (length >> 22) + 4;
                        if ((flags & 0x41) == 0x40)
                        {
                            DR_TPAGE *mode = (DR_TPAGE *)next;
                            mode->code[0] = ((flags & 0x180) >> 2) | 0xE1000200;
                            mode->tag = gpu_packet_link((uintptr_t)packet) | 0x01000000;
                            *destination = gpu_packet_link((uintptr_t)next);
                            next += 8;
                        }
                        packet = next;
                    }
                }
            }
        }
        descriptor = descriptor->next;
        --count;
    } while (count);
    scene_render.cursor = packet;
}
