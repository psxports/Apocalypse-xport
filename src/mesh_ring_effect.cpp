#include <new>
#include <string.h>
#include <stdlib.h>

#include "scene_lifecycle.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "mesh_ring_effect.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "global.h"
#include "ring_effect.h"
#include "effect_object.h"
#include "renderable_effect.h"
#include "texture_resources.h"
#include "hd.h"

struct MeshRingEffect::Projection
{
    XY scr[3];
    bool valid[3];
    sint16 depth;
};

sint32 MeshRingEffect::ReadCameraOffset(sint32 pos, uint32 axis)
{
    return (pos >> 12) - (uint32)GetModelRenderView().pos.comp[axis];
}

static uint32 LinkPacket(void *packet, uint32 *ot_entry)
{
    P_TAG *header = (P_TAG *)packet;
    header->addr = *ot_entry;
    uint32 result = (*ot_entry & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
    *ot_entry = result;
    return result;
}

MeshRingEffect::MeshRingEffect(sint32 count, uint32 resource) : RenderableEffect(), radius(0), expansion(0), u_scroll(0), v_scroll(0), seg_mask(0xFFFFFFFF), forced_depth(0), seg_count(count), segs(0), proj(0), tex(0), hd_history(0)
{
    FUNCTION_MARKER(0x80028FB8, "SLUS_003.73");
    angles.x = angles.y = angles.z = 0;
    if (count <= 0 || count > 0xFFFFFFFF / 140 || count > (size_t)-1 / sizeof(*segs) || count > (size_t)-1 / sizeof(*proj))
    {
        GameDiagnostic(("Invalid mesh ring segment count %d\n", count));
        abort();
    }
    segs = (Segment *)malloc(count * sizeof(*segs));
    proj = (Projection *)malloc(count * sizeof(*proj));
    if (!segs || !proj)
    {
        GameDiagnostic(("Mesh ring geometry allocation failed\n"));
        abort();
    }
    tex = &TextureResource::Get(resource);
    for (sint32 i = 0; i < seg_count; ++i)
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
    SetTexOffsets(0, 0, 2);
}

uint32 MeshRingEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800290F4, "SLUS_003.73");
    free(proj);
    free(segs);
    proj = 0;
    segs = 0;
    uint32 result = RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseStorage() : result;
}

uint32 MeshRingEffect::SetGeometry(const Vector3 *src_pos, uint32 radius, uint32 expansion)
{
    FUNCTION_MARKER(0x80029198, "SLUS_003.73");
    pos = *src_pos;
    this->expansion = expansion;
    this->radius = radius;
    ShortVector3 angle = angles;
    sint32 inc = ONE / seg_count;
    for (sint32 i = 0; i < seg_count; ++i)
    {
        Vector3 dir, radius_offset, radial_step;
        dir.SetDir(1, angle);
        radius_offset.Multiply(dir, this->radius);
        Segment *seg = segs + i;
        seg->v[0].Add(pos, radius_offset);
        radial_step.Multiply(dir, this->expansion);
        seg->v[1].Add(seg->v[0], radial_step);
        seg->v[2].Add(seg->v[1], radial_step);
        angle.y = (uint16)angle.y + inc;
    }
    return 0;
}

uint32 MeshRingEffect::SetTexOffsets(uint32 u, uint32 v, uint32 repeats)
{
    FUNCTION_MARKER(0x80029324, "SLUS_003.73");
    u_scroll = u;
    v_scroll = v;
    u = tex->uv[0].u + (u & 63);
    v = tex->uv[0].v + (v & 63);
    sint32 numerator = repeats << 6;
    sint32 step = numerator / seg_count;
    for (sint32 i = 0; i < seg_count; ++i)
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

uint32 MeshRingEffect::SetColors(sint32 r, sint32 g, sint32 b)
{
    FUNCTION_MARKER(0x800293D8, "SLUS_003.73");
    if (r < 0)
        r = 0;
    if (g < 0)
        g = 0;
    if (b < 0)
        b = 0;
    for (sint32 i = 0; i < seg_count; ++i)
    {
        POLY_GT4 *poly = segs[i].poly;
        setRGB0(poly, 0, 0, 0);
        setRGB1(poly, 0, 0, 0);
        setRGB2(poly, r, g, b);
        setRGB3(poly, r, g, b);
        setRGB0(poly + 1, r, g, b);
        setRGB1(poly + 1, r, g, b);
        setRGB2(poly + 1, 0, 0, 0);
        setRGB3(poly + 1, 0, 0, 0);
    }
    return 0;
}

uint32 MeshRingEffect::CaptureHd(bool logic) const
{
    const uint32 window = 0x318 | ((tex->uv[0].u >> 3) << 10) | ((tex->uv[0].v >> 3) << 15);
    HD_POLYGON_SAMPLE *samples = hd_polygon_samples(seg_count * 2);
    if (!samples)
    {
        GameDiagnostic(("Cannot stage HD mesh ring\n"));
        abort();
    }
    for (sint32 index = 1; index <= seg_count; ++index)
    {
        const Segment &previous = segs[index - 1], &next = segs[index == seg_count ? 0 : index];
        VECTORF points[2][3];
        for (uint32 ray = 0; ray < 2; ++ray)
            for (uint32 corner = 0; corner < 3; ++corner)
            {
                const Vector3 &source = ray ? next.v[2 - corner] : previous.v[2 - corner];
                VECTORF &point = points[ray][corner];
                point.vx = source.x / 4096.0f;
                point.vy = source.y / 4096.0f;
                point.vz = source.z / 4096.0f;
            }
        for (uint32 layer = 0; layer < 2; ++layer)
        {
            // The original renderer uses the first segment's material templates
            const POLY_GT4 &material = segs[0].poly[layer];
            HD_POLYGON_SAMPLE &sample = samples[(index - 1) * 2 + layer];
            sample = HD_POLYGON_SAMPLE();
            HD_EFFECT &effect = sample.effect;
            sample.bias = depth_bias;
            sample.uv_period = 64;
            sample.present = (seg_mask >> (index & 31)) & 1;
            sample.reject_zero_depth = 1;
            effect.count = 4;
            effect.space = HD_EFFECT_WORLD;
            effect.forced_depth = forced_depth;
            effect.flags = HD_TEXTURED | HD_SEMITRANSPARENT;
            effect.clut = material.p[0].attr;
            effect.page = material.p[1].attr;
            effect.window = window;
            effect.flags |= HD_FILTERED;
            effect.texture_region = tex->region;
            for (uint32 corner = 0; corner < 4; ++corner)
            {
                HD_EFFECT_VERTEX &vertex = effect.vertices[corner];
                vertex.pos = points[corner & 1][layer + (corner >> 1)];
                vertex.color = material.p[corner].rgbc.rgbc;
                vertex.u = material.p[corner].uv.u;
                vertex.v = material.p[corner].uv.v;
            }
            memcpy(effect.depth.points, points, sizeof(points));
            effect.depth.count = 6;
            effect.depth.rule = HD_DEPTH_MIN;
        }
    }
    if (!(logic ? hd_polygon_chain_tick(&hd_history, samples, seg_count * 2) : hd_polygon_chain_capture(&hd_history, samples, seg_count * 2)))
    {
        GameDiagnostic(("Cannot capture HD mesh ring\n"));
        abort();
    }
    return 1;
}

bool MeshRingEffect::CaptureHdTick()
{
    CaptureHd(true);
    return true;
}

uint32 MeshRingEffect::Render()
{
    FUNCTION_MARKER(0x8002948C, "SLUS_003.73");
    if (hd_ui_collecting())
        return CaptureHd(false);
    hd_polygon_chain_reset(hd_history);
    for (sint32 i = 0; i < seg_count; ++i)
    {
        Projection *p = proj + i;
        p->depth = 0;
        for (uint32 j = 0; j < 3; ++j)
        {
            const Vector3 &v = segs[i].v[2 - j];
            XY input;
            input.x = MeshRingEffect::ReadCameraOffset(v.x, 0);
            input.y = MeshRingEffect::ReadCameraOffset(v.y, 1);
            sint32 z = MeshRingEffect::ReadCameraOffset(v.z, 2);
            xport_gte_write_data(0, input.xy);
            xport_gte_write_data(1, z);
            xport_gte_execute(0x180001);
            p->scr[j].xy = xport_gte_read_data(14);
            sint32 depth = xport_gte_read_data(27);
            sint32 scr = p->scr[j].xy;
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
    PSX_RECT tex_window = {tex->uv[0].u, tex->uv[0].v, 64, 64};
    PSX_RECT no_window = {0, 0, 0, 0};
    uint32 result = 0;
    const sint32 state_size = 2 * sizeof(DR_TWIN) + sizeof(DR_TPAGE);
    const sint32 poly_size = sizeof(POLY_GT4);
    for (sint32 i = 1; i <= seg_count; ++i)
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
            result = limit - window_packet < state_size;
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
            POLY_GT4 *poly[2] = {0, 0};
            uint32 state_tag = EffectObject::GetDrawMode().tag;
            uint32 state_code = EffectObject::GetDrawMode().code[0];
            restore_mode->tag = state_tag;
            restore_mode->code[0] = state_code;
            LinkPacket(reset_packet, ot_entry);
            if ((prev->valid[0] && prev->valid[1]) && next->valid[0] && next->valid[1])
            {
                poly[0] = (POLY_GT4 *)scene_render.cursor;
                uint8 *limit = scene_render.limit;
                result = limit - scene_render.cursor < poly_size;
                if (result)
                    return result;
                uint8 *next_packet = scene_render.cursor + sizeof(POLY_GT4);
                scene_render.cursor = next_packet;
                // Original packet templates use the unchanged geometry base
                *poly[0] = segs[0].poly[0];
                poly[0]->p[0].xy = prev->scr[0];
                poly[0]->p[1].xy = next->scr[0];
                poly[0]->p[2].xy = prev->scr[1];
                poly[0]->p[3].xy = next->scr[1];
            }
            if ((prev->valid[2] && prev->valid[1]) && next->valid[2] && next->valid[1])
            {
                poly[1] = (POLY_GT4 *)scene_render.cursor;
                uint8 *limit = scene_render.limit;
                result = limit - scene_render.cursor < poly_size;
                if (result)
                    return result;
                uint8 *next_packet = scene_render.cursor + sizeof(POLY_GT4);
                scene_render.cursor = next_packet;
                *poly[1] = segs[0].poly[1];
                poly[1]->p[0].xy = prev->scr[1];
                poly[1]->p[1].xy = next->scr[1];
                poly[1]->p[2].xy = prev->scr[2];
                poly[1]->p[3].xy = next->scr[2];
            }
            if (poly[0])
            {
                LinkPacket(poly[0], ot_entry);
            }
            if (poly[1])
            {
                LinkPacket(poly[1], ot_entry);
            }
            uint32 head_link = LinkPacket(window_packet, ot_entry);
            restore_mode->tag = (restore_mode->tag & 0xFF000000) | (head_link & 0xFFFFFF);
            *ot_entry = (*ot_entry & 0xFF000000) | gpu_packet_link((uintptr_t)mode_packet);
        }
    }
    return 1;
}

MeshRingEffect::~MeshRingEffect()
{
    hd_polygon_chain_free(hd_history);
    free(proj);
    free(segs);
}

void MeshRingEffect::Update()
{
    // Original 0x8003A144 has no body or defined return carrier
    return;
}
