#include "model_lighting.h"
extern "C"
{
#include "draft_first_signatures.h"
}

void ReadRendererNormal(uint32 address, uint32 normal[3])
{
    uint32 packed, last;
    FUNCTION_MARKER(0x800821B8, "SLUS_003.73");
    packed = r_u32(address);
    last = r_u32(address + 4);
    normal[0] = packed;
    normal[1] = (uint32)((sint32)packed >> 16);
    normal[2] = last;
}

void LoadRendererVertexLighting(uint32 color_offset, uint32 color_table, uint32 normal_address)
{
    uint32 color, packed, last, y;
    FUNCTION_MARKER(0x80082224, "SLUS_003.73");
    color = r_u32(color_table + (color_offset & 0x3FC));
    packed = r_u32(normal_address);
    last = r_u32(normal_address + 4);
    y = (uint32)((sint32)packed >> 16);
    xport_gte_write_data(6, color);
    xport_gte_write_data(9, packed);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, last);
}

extern "C" void sub_800821B8(uint32 address, uint32 normal[3])
{
    FUNCTION_MARKER(0x800821B8, "SLUS_003.73");
    ReadRendererNormal(address, normal);
}
extern "C" void sub_80082224(uint32 color_offset, uint32 color_table, uint32 normal_address)
{
    FUNCTION_MARKER(0x80082224, "SLUS_003.73");
    LoadRendererVertexLighting(color_offset, color_table, normal_address);
}

void LoadRendererScratchLighting(uint32 scratch)
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x80082254, "SLUS_003.73");
    x = r_u32(scratch + 0x1A4);
    y = r_u32(scratch + 0x1B4);
    z = r_u32(scratch + 0x1C4);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
}

extern "C" void sub_80082254(uint32 scratch)
{
    FUNCTION_MARKER(0x80082254, "SLUS_003.73");
    LoadRendererScratchLighting(scratch);
}
