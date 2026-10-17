#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "ribbon_segments.h"
#include "game_heap.h"
#include "model_resources.h"
#include "model_projection.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <stdlib.h>

RibbonSegments::RibbonSegments(uint32 segments, uint32 mode) : RenderableEffect(), tex(0)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer, count, index = 0, offset = 0;
    FUNCTION_MARKER(0x80026A44, "SLUS_003.73");
    w_u32(object + 68, 0x800A1598);
    w_u32(object + 100, segments);
    buffer = AllocateHeapBlock(segments * 36, 0, 1);
    count = r_u32(object + 100);
    w_u32(object + 104, buffer);
    if ((sint32)count > 0)
    {
        do
        {
            buffer = r_u32(object + 104) + offset;
            ++index;
            w_u16(buffer + 18, 0);
            w_u16(buffer + 16, 0);
            count = r_u32(object + 100);
            offset += 36;
        } while ((sint32)index < (sint32)count);
    }
    w_u8(object + 72, mode);
    w_u32(object + 92, 8);
}

RibbonSegments &RibbonSegments::Attach(uint32 object)
{
    return *static_cast<RibbonSegments *>(FindNativeObjectStorage(object));
}

uint32 RibbonSegments::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer, result;
    FUNCTION_MARKER(0x80026AF4, "SLUS_003.73");
    buffer = r_u32(object + 104);
    w_u32(object + 68, 0x800A1598);
    ReleaseHeapBlock(buffer);
    result = RenderableEffect::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

extern "C" uint32 sub_80026AF4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80026AF4, "SLUS_003.73");
    return RibbonSegments::Attach(object).RibbonSegments::Destroy(reason);
}

void RibbonSegments::SetMaterial(uint32 name)
{
    FUNCTION_MARKER(0x80026B54, "SLUS_003.73");
    tex = &GetTextureResource(name);
}

uint32 RibbonSegments::SetColors(uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, index = 0, offset = 0;
    FUNCTION_MARKER(0x80026B80, "SLUS_003.73");
    tail_red &= 255;
    tail_green &= 255;
    result = r_u32(object + 100);
    tail_blue &= 255;
    if ((sint32)result <= 0)
        return result;
    do
    {
        w_u8(r_u32(object + 104) + offset + 12, red);
        w_u8(r_u32(object + 104) + offset + 13, green);
        w_u8(r_u32(object + 104) + offset + 14, blue);
        w_u8(r_u32(object + 104) + offset + 20, tail_red);
        w_u8(r_u32(object + 104) + offset + 21, tail_green);
        ++index;
        w_u8(r_u32(object + 104) + offset + 22, tail_blue);
        result = (sint32)index < (sint32)r_u32(object + 100);
        offset += 36;
    } while (result);
    return result;
}

uint32 RibbonSegments::SetSize(uint32 width, uint32 height)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, index = 0, offset = 0;
    FUNCTION_MARKER(0x80026C18, "SLUS_003.73");
    result = r_u32(object + 100);
    if ((sint32)result <= 0)
        return result;
    do
    {
        w_u16(r_u32(object + 104) + offset + 16, width);
        ++index;
        w_u16(r_u32(object + 104) + offset + 18, height);
        result = (sint32)index < (sint32)r_u32(object + 100);
        offset += 36;
    } while (result);
    return result;
}









static uint32 ReadRibbonNormal(const sint32 *output)
{
    uint32 address = GuestMemoryAddress(output);
    return address ? r_u32(address) : (uint32)*output;
}

static void WriteRibbonNormal(sint32 *output, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u32(address, value);
    else
        *output = (sint32)value;
}

uint32 CalculateRibbonNormal(uint32 first, uint32 second, sint32 *output_x, sint32 *output_y, uint32 minimum)
{
    uint32 horizontal, vertical, norm, result;
    FUNCTION_MARKER(0x8002626C, "SLUS_003.73");
    horizontal = (uint32)(sint32)(sint16)second - (uint32)(sint32)(sint16)first;
    horizontal = (uint32)((sint32)(horizontal * 320) / 512);
    vertical = (uint32)((sint32)second >> 16) - (uint32)((sint32)first >> 16);
    WriteRibbonNormal(output_x, vertical);
    WriteRibbonNormal(output_y, 0 - horizontal);
    if ((sint32)horizontal < 0)
        horizontal = 0 - horizontal;
    if ((sint32)vertical < 0)
        vertical = 0 - vertical;
    norm = vertical < horizontal ? horizontal + vertical / 2 : vertical + horizontal / 2;
    result = (sint32)norm < (sint32)minimum;
    if (result)
    {
        WriteRibbonNormal(output_y, 0);
        WriteRibbonNormal(output_x, 0);
        return result;
    }
    // A minimum of zero can reach the original DIV by zero
    result = ReadRibbonNormal(output_x) << 6;
    result = norm ? (uint32)((sint32)result / (sint32)norm) : ((sint32)result < 0 ? 1 : (uint32)-1);
    WriteRibbonNormal(output_x, result);
    result = ReadRibbonNormal(output_y) << 6;
    result = norm ? (uint32)((sint32)result / (sint32)norm) : ((sint32)result < 0 ? 1 : (uint32)-1);
    WriteRibbonNormal(output_y, result);
    result = (uint32)((sint32)(ReadRibbonNormal(output_x) << 9) / 320);
    WriteRibbonNormal(output_x, result);
    return result;
}



// Native helpers retain the original inline packet and coordinate operations
static uint32 PackRibbonOffset(uint32 center, uint32 x, uint32 y)
{
    uint32 center_x = (uint32)(sint32)(sint16)center;
    uint32 center_y = (uint32)((sint32)center >> 16);
    return ((center_y + y) << 16) | ((center_x + x) & 65535);
}

static void MakeRibbonEdges(uint32 center, uint32 normal_x, uint32 normal_y, uint32 width, uint32 height, uint32 edges[4])
{
    uint32 x = (uint32)((sint32)(normal_x * width) >> 6);
    uint32 y = (uint32)((sint32)(normal_y * width) >> 6);
    edges[0] = PackRibbonOffset(center, x, y);
    edges[1] = PackRibbonOffset(center, 0 - x, 0 - y);
    x = (uint32)((sint32)(normal_x * height) >> 6);
    y = (uint32)((sint32)(normal_y * height) >> 6);
    edges[2] = PackRibbonOffset(center, x, y);
    edges[3] = PackRibbonOffset(center, 0 - x, 0 - y);
}

static uint32 LinkRibbonPacket(uint32 packet, uint32 ordering_table, uint32 tag, uint32 previous)
{
    uint32 next;
    w_u32(packet, (tag & 0xFF000000) | (previous & 0xFFFFFF));
    next = r_u32(ordering_table);
    next = (next & 0xFF000000) | (packet & 0xFFFFFF);
    w_u32(ordering_table, next);
    return next;
}

static uint32 InsertRibbonPacket(uint32 packet, uint32 ordering_table)
{
    uint32 tag = r_u32(packet);
    uint32 previous = r_u32(ordering_table);
    return LinkRibbonPacket(packet, ordering_table, tag, previous);
}

uint32 RibbonSegments::Render()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, current, next, current_colors, next_colors, index = 0, first = 1, phase;
    uint32 normal_x = 0, normal_y = 0, start_x, start_y, end_x, end_y;
    uint32 current_depth, next_depth, current_width, current_height, next_width, next_height;
    uint32 previous_edges[4], edges[4], packet, quad[4], ordering_table, depth, value, u, v, tick, width;
    sint32 computed_x, computed_y, wave;
    FUNCTION_MARKER(0x80026C68, "SLUS_003.73");
    result = r_u8(object + 80);
    if (result)
        return result;
    current = r_u32(object + 104);
    next = current + 36;
    w_u32(0x1F800000, r_u32(current));
    w_u32(0x1F800004, r_u32(current + 4));
    w_u32(0x1F800008, r_u32(current + 8));
    current_colors = current + 20;
    next_colors = next + 20;
    phase = r_u32(object + 88);
    for (;;)
    {
        result = (sint32)index < (sint32)(r_u32(object + 100) - 1);
        if (!result)
            return result;
        w_u32(0x1F80000C, r_u32(next));
        w_u32(0x1F800010, r_u32(next_colors - 16));
        w_u32(0x1F800014, r_u32(next_colors - 12));
        result = ProjectAndClipWorldLine(0x1F800000);
        if ((sint32)result < 0)
        {
            normal_x = 0;
            normal_y = 0;
            goto AdvanceSegment;
        }
        current_depth = r_u32(0x1F800028);
        current_width = current_height = 0;
        if (current_depth)
        {
            current_width = (uint32)((sint32)(400 * r_u16(current_colors - 4)) / (sint32)current_depth);
            current_height = (uint32)((sint32)(400 * r_u16(current_colors - 2)) / (sint32)current_depth);
        }
        next_depth = r_u32(0x1F80002C);
        next_width = next_height = 0;
        if (next_depth)
        {
            next_width = (uint32)((sint32)(400 * r_u16(next_colors - 4)) / (sint32)next_depth);
            next_height = (uint32)((sint32)(400 * r_u16(next_colors - 2)) / (sint32)next_depth);
        }
        CalculateRibbonNormal(r_u32(0x1F800018), r_u32(0x1F80001C), &computed_x, &computed_y, 3);
        if (normal_x | normal_y)
        {
            start_x = normal_x;
            start_y = normal_y;
            end_x = (uint32)computed_x;
            end_y = (uint32)computed_y;
            if (end_x | end_y)
            {
                normal_x = end_x;
                normal_y = end_y;
            }
            else
            {
                end_x = normal_x;
                end_y = normal_y;
            }
        }
        else
        {
            if (!(computed_x | computed_y))
            {
                first = 1;
                goto AdvanceSegment;
            }
            start_x = end_x = normal_x = (uint32)computed_x;
            start_y = end_y = normal_y = (uint32)computed_y;
        }
        if (first)
            MakeRibbonEdges(r_u32(0x1F800018), start_x, start_y, current_width, current_height, previous_edges);
        MakeRibbonEdges(r_u32(0x1F80001C), end_x, end_y, next_width, next_height, edges);
        if (r_u32(object + 96) && (sint32)index >= (sint32)r_u32(object + 100) / 2)
        {
            tick = r_u32(0x800FF2F0);
            wave = rsin((tick * 200 + 130) & 4095);
            edges[0] += (uint32)(wave / 2048) << 16;
            wave = rsin(((tick * 1000) & 0x3FF8) >> 2);
            value = (uint32)((wave * 3) / 4096);
            wave = rsin((tick * 150 + 100) & 4095);
            edges[1] += (value + (uint32)(wave / 2048)) << 16;
        }
        next_depth = r_u32(0x1F80002C);
        current_depth = r_u32(0x1F800028);
        depth = (sint32)current_depth < (sint32)next_depth ? current_depth : next_depth;
        ordering_table = r_u32(0x800FF660) + (depth & 0x3FFC) + 112;
        packet = r_u32(0x800FF668);
        quad[0] = packet;
        quad[1] = packet + 52;
        quad[2] = packet + 104;
        quad[3] = packet + 156;
        result = r_u32(0x800FF374) < packet + 240;
        if (result)
            return result;
        w_u32(0x800FF668, packet + 240);
        w_u8(packet + 223, 2);
        w_u32(packet + 228, 0);
        w_u32(packet + 224, 0xE2000000);
        if (!tex)
        {
            GameDiagnostic("Ribbon texture is not selected\n");
            abort();
        }
        u = tex->uv[0].u;
        v = tex->uv[0].v;
        w_u8(packet + 211, 2);
        w_u32(packet + 216, 0);
        w_u32(packet + 212, 0xE2000318 | ((v >> 3) << 15) | ((u >> 3) << 10));
        InsertRibbonPacket(packet + 220, ordering_table);
        w_u32(quad[3], 0x0C000000);
        w_u32(quad[2], 0x0C000000);
        w_u32(quad[1], 0x0C000000);
        w_u32(quad[0], 0x0C000000);
        value = tex->clut;
        w_u16(quad[3] + 14, value);
        w_u16(quad[2] + 14, value);
        w_u16(quad[1] + 14, value);
        w_u16(quad[0] + 14, value);
        value = tex->tpage;
        w_u16(quad[3] + 26, value);
        w_u16(quad[2] + 26, value);
        w_u16(quad[1] + 26, value);
        w_u16(quad[0] + 26, value);
        value = r_u32(object + 84) & 63;
        u = tex->uv[0].u + value;
        v = tex->uv[0].v + (phase & 63);
        // Original texture strips are written in quad order 2, 0, 1, 3
        w_u8(quad[2] + 12, u);
        w_u8(quad[2] + 36, u + 16);
        w_u8(quad[2] + 24, u);
        w_u8(quad[2] + 13, v);
        w_u8(quad[2] + 37, v);
        value = r_u8(object + 92);
        w_u8(quad[2] + 48, u + 16);
        w_u8(quad[2] + 25, value + v);
        w_u8(quad[2] + 49, r_u8(object + 92) + v);
        w_u8(quad[0] + 12, u + 16);
        w_u8(quad[0] + 13, v);
        w_u8(quad[0] + 36, u + 32);
        w_u8(quad[0] + 37, v);
        w_u8(quad[0] + 24, u + 16);
        w_u8(quad[0] + 25, r_u8(object + 92) + v);
        w_u8(quad[0] + 48, u + 32);
        w_u8(quad[0] + 49, r_u8(object + 92) + v);
        w_u8(quad[1] + 12, u + 48);
        w_u8(quad[1] + 13, v);
        w_u8(quad[1] + 36, u + 32);
        w_u8(quad[1] + 37, v);
        w_u8(quad[1] + 24, u + 48);
        value = r_u8(object + 92);
        w_u8(quad[1] + 48, u + 32);
        w_u8(quad[1] + 25, value + v);
        w_u8(quad[1] + 49, r_u8(object + 92) + v);
        w_u8(quad[3] + 12, u + 64);
        w_u8(quad[3] + 13, v);
        w_u8(quad[3] + 36, u + 48);
        w_u8(quad[3] + 37, v);
        w_u8(quad[3] + 24, u + 64);
        value = r_u8(object + 92);
        w_u8(quad[3] + 48, u + 48);
        w_u8(quad[3] + 25, value + v);
        w_u8(quad[3] + 49, r_u8(object + 92) + v);
        value = r_u32(next_colors);
        width = r_u32(object + 92);
        w_u32(quad[0] + 4, value);
        w_u32(quad[0] + 16, r_u32(current_colors));
        w_u32(quad[0] + 28, r_u32(next_colors - 8));
        value = r_u32(current_colors - 8);
        w_u32(quad[0] + 8, edges[0]);
        w_u32(quad[0] + 40, value);
        w_u32(quad[0] + 20, previous_edges[0]);
        w_u32(quad[0] + 32, r_u32(0x1F80001C));
        w_u32(quad[0] + 44, r_u32(0x1F800018));
        w_u32(quad[2] + 4, 0);
        w_u32(quad[2] + 16, 0);
        w_u32(quad[2] + 28, r_u32(next_colors));
        value = r_u32(current_colors);
        w_u32(quad[2] + 8, edges[2]);
        w_u32(quad[2] + 20, previous_edges[2]);
        w_u32(quad[2] + 32, edges[0]);
        w_u32(quad[2] + 44, previous_edges[0]);
        w_u32(quad[2] + 40, value);
        w_u32(quad[1] + 4, r_u32(next_colors));
        w_u32(quad[1] + 16, r_u32(current_colors));
        w_u32(quad[1] + 28, r_u32(next_colors - 8));
        value = r_u32(current_colors - 8);
        w_u32(quad[1] + 8, edges[1]);
        w_u32(quad[1] + 20, previous_edges[1]);
        w_u32(quad[1] + 40, value);
        w_u32(quad[1] + 32, r_u32(0x1F80001C));
        w_u32(quad[1] + 44, r_u32(0x1F800018));
        w_u32(quad[3] + 4, 0);
        w_u32(quad[3] + 16, 0);
        w_u32(quad[3] + 28, r_u32(next_colors));
        w_u32(quad[3] + 40, r_u32(current_colors));
        w_u32(quad[3] + 8, edges[3]);
        w_u32(quad[3] + 20, previous_edges[3]);
        w_u32(quad[3] + 32, edges[1]);
        w_u32(quad[3] + 44, previous_edges[1]);
        w_u8(quad[3] + 7, 62);
        w_u8(quad[2] + 7, 62);
        w_u8(quad[1] + 7, 62);
        value = r_u32(quad[0]);
        w_u8(quad[0] + 7, 62);
        tick = r_u32(ordering_table);
        tick = LinkRibbonPacket(quad[0], ordering_table, value, tick);
        value = r_u32(quad[2]);
        tick = LinkRibbonPacket(quad[2], ordering_table, value, tick);
        value = r_u32(quad[1]);
        tick = LinkRibbonPacket(quad[1], ordering_table, value, tick);
        value = r_u32(quad[3]);
        first = 0;
        tick = LinkRibbonPacket(quad[3], ordering_table, value, tick);
        phase -= width;
        previous_edges[0] = edges[0];
        previous_edges[2] = edges[2];
        previous_edges[1] = edges[1];
        value = r_u32(packet + 208);
        LinkRibbonPacket(packet + 208, ordering_table, value, tick);
        value = r_u32(0x800FF468);
        tick = r_u32(0x800FF46C);
        w_u32(packet + 232, value);
        w_u32(packet + 236, tick);
        InsertRibbonPacket(packet + 232, ordering_table);
        previous_edges[3] = edges[3];
AdvanceSegment:
        next_colors += 36;
        current_colors += 36;
        next += 36;
        value = r_u32(0x1F80000C);
        tick = r_u32(0x1F800010);
        ++index;
        depth = r_u32(0x1F800014);
        w_u32(0x1F800000, value);
        w_u32(0x1F800004, tick);
        w_u32(0x1F800008, depth);
    }
}

extern "C" uint32 sub_80026C68(uint32 object)
{
    FUNCTION_MARKER(0x80026C68, "SLUS_003.73");
    return RibbonSegments::Attach(object).RibbonSegments::Render();
}
