#include <new>
#include <string.h>
#include <stdlib.h>

#include "scene_lifecycle.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "mesh_ring_effect.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"

struct MeshRingEffect::Projection
{
    uint32 scr[3];
    bool valid[3];
    sint16 depth;
};

MeshRingEffect::MeshRingEffect(uint32 count, uint32 resource) : RenderableEffect(), radius(0), expansion(0), u_scroll(0), v_scroll(0), seg_mask(0xFFFFFFFF), forced_depth(0), seg_count(count), segs(0), proj(0), tex(0)
{
    FUNCTION_MARKER(0x80028FB8, "SLUS_003.73");
    angles.words[0] = angles.words[1] = angles.words[2] = 0;
    if (!count || count > 0x7FFFFFFF || count > 0xFFFFFFFF / 140 || count > (size_t)-1 / sizeof(*segs) || count > (size_t)-1 / sizeof(*proj))
    {
        GameDiagnostic("Invalid mesh ring segment count %u\n", count);
        abort();
    }
    segs = (Segment *)malloc((size_t)count * sizeof(*segs));
    proj = (Projection *)malloc((size_t)count * sizeof(*proj));
    if (!segs || !proj)
    {
        GameDiagnostic("Mesh ring geometry allocation failed\n");
        abort();
    }
    tex = &GetTextureResource(resource);
    for (uint32 i = 0; i < seg_count; ++i)
    {
        // OT links and unused packet payload are replaced before publication
        memset(segs[i].poly, 0, sizeof(segs[i].poly));
        for (uint32 p = 0; p < 2; ++p)
        {
            POLY_GT4 *poly = &segs[i].poly[p];
            setPolyGT4(poly);
            setSemiTrans(poly, 1);
            poly->p[0].attr = tex->clut;
            poly->p[1].attr = tex->tpage;
        }
    }
    SetColors(128, 128, 128);
    SetTextureOffsets(0, 0, 2);
}

MeshRingEffect::~MeshRingEffect()
{
    free(proj);
    free(segs);
}

uint32 MeshRingEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800290F4, "SLUS_003.73");
    free(proj);
    free(segs);
    proj = 0;
    segs = 0;
    uint32 result = RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : result;
}

void MeshRingEffect::Update()
{
    // Original 0x8003A144 has no body or defined return carrier
    return;
}

uint32 MeshRingEffect::SetTextureOffsets(uint32 u, uint32 v, uint32 repeats)
{
    FUNCTION_MARKER(0x80029324, "SLUS_003.73");
    u_scroll = u;
    v_scroll = v;
    u = tex->uv[0].u + (u & 63);
    v = tex->uv[0].v + (v & 63);
    sint32 numerator = repeats << 6;
    uint32 step = numerator / (sint32)seg_count;
    for (uint32 i = 0; i < seg_count; ++i)
    {
        POLY_GT4 *poly = segs[i].poly;
        uint32 u0 = u;
        uint32 next_u = u + step;
        u = next_u & 63;
        setUV4(poly, u0, v, next_u, v, u0, v + 32, next_u, v + 32);
        setUV4(poly + 1, u0, v + 32, next_u, v + 32, u0, v + 64, next_u, v + 64);
    }
    return 0;
}

uint32 MeshRingEffect::SetColors(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800293D8, "SLUS_003.73");
    if ((sint32)red < 0)
        red = 0;
    if ((sint32)green < 0)
        green = 0;
    if ((sint32)blue < 0)
        blue = 0;
    for (uint32 i = 0; i < seg_count; ++i)
    {
        POLY_GT4 *poly = segs[i].poly;
        setRGB0(poly, 0, 0, 0);
        setRGB1(poly, 0, 0, 0);
        setRGB2(poly, red, green, blue);
        setRGB3(poly, red, green, blue);
        setRGB0(poly + 1, red, green, blue);
        setRGB1(poly + 1, red, green, blue);
        setRGB2(poly + 1, 0, 0, 0);
        setRGB3(poly + 1, 0, 0, 0);
    }
    return 0;
}

uint32 MeshRingEffect::SetGeometry(const uint32 *position, uint32 radius, uint32 expansion)
{
    FUNCTION_MARKER(0x80029198, "SLUS_003.73");
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    this->expansion = expansion;
    this->radius = radius;
    ShortVector3 angle = angles;
    uint32 inc = 4096 / (sint32)seg_count;
    for (uint32 i = 0; i < seg_count; ++i)
    {
        Vector3 dir, radius_offset, inner_pos, radial_step, middle_pos, outer_pos;
        BuildDirectionVector(dir.words, 1, angle.words);
        MultiplyVector(radius_offset.words, dir.words, this->radius);
        AddVectors(inner_pos.words, pos.words, radius_offset.words);
        Segment *segment = segs + i;
        segment->v[0] = inner_pos;
        MultiplyVector(radial_step.words, dir.words, this->expansion);
        AddVectors(middle_pos.words, segment->v[0].words, radial_step.words);
        segment->v[1] = middle_pos;
        AddVectors(outer_pos.words, segment->v[1].words, radial_step.words);
        segment->v[2] = outer_pos;
        angle.words[1] += inc;
    }
    return 0;
}

static sint32 ReadMeshRingCameraOffset(uint32 position, uint32 axis)
{
    uint32 camera = GetModelRenderView().pos.words[axis];
    return (sint32)((uint32)((sint32)position >> 12) - camera);
}

static uint32 LinkMeshRingRenderPacket(uint8 *packet, uint32 *ordering_entry)
{
    P_TAG *header = (P_TAG *)packet;
    uint32 link = *ordering_entry;
    header->addr = link;
    link = *ordering_entry;
    uint32 result = (link & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
    *ordering_entry = result;
    return result;
}

uint32 MeshRingEffect::Render()
{
    FUNCTION_MARKER(0x8002948C, "SLUS_003.73");
    for (uint32 i = 0; i < seg_count; ++i)
    {
        Projection *p = proj + i;
        p->depth = 0;
        for (uint32 j = 0; j < 3; ++j)
        {
            const uint32 *v = segs[i].v[2 - j].words;
            sint32 x = ReadMeshRingCameraOffset(v[0], 0);
            sint32 y = ReadMeshRingCameraOffset(v[1], 1);
            sint32 z = ReadMeshRingCameraOffset(v[2], 2);
            xport_gte_write_data(0, (uint32)(uint16)x | ((uint32)(uint16)y << 16));
            xport_gte_write_data(1, z);
            xport_gte_execute(0x180001);
            p->scr[j] = xport_gte_read_data(14);
            sint32 depth = xport_gte_read_data(27);
            sint32 scr = p->scr[j];
            if (scr == 67044351 || scr == 67107840 || scr == -67107841 || scr == -67044352 || depth < -20000 || depth >= 20001)
                p->valid[j] = false;
            else
            {
                p->valid[j] = true;
                if (!j || depth < p->depth)
                    p->depth = depth;
            }
        }
    }
    PSX_RECT tex_window = {(sint16)tex->uv[0].u, (sint16)tex->uv[0].v, 64, 64};
    PSX_RECT no_window = {0, 0, 0, 0};
    uint32 result = 0;
    for (uint32 i = 1; i <= seg_count; ++i)
    {
        const Projection *prev = proj + i - 1;
        const Projection *next = proj + (i == seg_count ? 0 : i);
        sint32 depth = next->depth < prev->depth ? next->depth : prev->depth;
        if ((forced_depth || depth) && ((seg_mask >> (i & 31)) & 1))
        {
            uint32 ot_offset = forced_depth ? 4 * forced_depth : ((uint32)depth - depth_bias) & 0x3FFC;
            uint32 *ot_entry = scene_render.cur->ot + (ot_offset >> 2);
            uint8 *window_packet = scene_render.cursor;
            uint8 *limit = scene_render.limit;
            result = limit - window_packet < (sint32)(2 * sizeof(DR_TWIN) + sizeof(DR_TPAGE));
            if (result)
                return result;
            uint8 *reset_packet = window_packet + sizeof(DR_TWIN);
            uint8 *mode_packet = reset_packet + sizeof(DR_TWIN);
            uint8 *next_packet = mode_packet + sizeof(DR_TPAGE);
            scene_render.cursor = next_packet;
            DR_TWIN *reset_window = (DR_TWIN *)reset_packet;
            DR_TWIN *apply_window = (DR_TWIN *)window_packet;
            DR_TPAGE *restore_mode = (DR_TPAGE *)mode_packet;
            setTexWindow(reset_window, &no_window);
            setTexWindow(apply_window, &tex_window);
            uint8 *poly_packet[2];
            poly_packet[0] = 0;
            uint32 state_tag = GetEffectDrawMode().tag;
            uint32 state_code = GetEffectDrawMode().code[0];
            poly_packet[1] = 0;
            restore_mode->tag = state_tag;
            restore_mode->code[0] = state_code;
            LinkMeshRingRenderPacket(reset_packet, ot_entry);
            if ((prev->valid[0] && prev->valid[1]) && next->valid[0] && next->valid[1])
            {
                poly_packet[0] = scene_render.cursor;
                uint8 *limit = scene_render.limit;
                result = limit - poly_packet[0] < (sint32)sizeof(POLY_GT4);
                if (result)
                    return result;
                uint8 *next_packet = poly_packet[0] + sizeof(POLY_GT4);
                scene_render.cursor = next_packet;
                POLY_GT4 *poly = (POLY_GT4 *)poly_packet[0];
                // Original packet templates use the unchanged geometry base
                *poly = segs[0].poly[0];
                poly->p[0].xy.xy = prev->scr[0];
                poly->p[1].xy.xy = next->scr[0];
                poly->p[2].xy.xy = prev->scr[1];
                poly->p[3].xy.xy = next->scr[1];
            }
            if ((prev->valid[2] && prev->valid[1]) && next->valid[2] && next->valid[1])
            {
                poly_packet[1] = scene_render.cursor;
                uint8 *limit = scene_render.limit;
                result = limit - poly_packet[1] < (sint32)sizeof(POLY_GT4);
                if (result)
                    return result;
                uint8 *next_packet = poly_packet[1] + sizeof(POLY_GT4);
                scene_render.cursor = next_packet;
                POLY_GT4 *poly = (POLY_GT4 *)poly_packet[1];
                *poly = segs[0].poly[1];
                poly->p[0].xy.xy = prev->scr[1];
                poly->p[1].xy.xy = next->scr[1];
                poly->p[2].xy.xy = prev->scr[2];
                poly->p[3].xy.xy = next->scr[2];
            }
            if (poly_packet[0])
            {
                LinkMeshRingRenderPacket(poly_packet[0], ot_entry);
            }
            if (poly_packet[1])
            {
                LinkMeshRingRenderPacket(poly_packet[1], ot_entry);
            }
            uint32 head_link = LinkMeshRingRenderPacket(window_packet, ot_entry);
            restore_mode->tag = (restore_mode->tag & 0xFF000000) | (head_link & 0xFFFFFF);
            *ot_entry = (*ot_entry & 0xFF000000) | gpu_packet_link((uintptr_t)mode_packet);
        }
    }
    return 1;
}
