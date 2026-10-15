#include "game_sdk_bindings.h"
extern "C"
{
#include "psx.h"
#include "psx_gpu.h"
#include "game_gpu_present.h"
#include "draft_first_adapters.h"
extern "C" uint32 apocalypse_reset_callbacks(void);
}
#include <stdlib.h>
#include <string.h>

extern "C" sint32 apocalypse_reset_graph(sint32 mode)
{
    uint32 kind = (uint32)mode & 7;
    uint32 graph_type;
    sint32 result;
    if (kind != 0 && kind != 3 && kind != 5)
        return ResetGraph(mode);
    memset(psx_addr(0x800FCEAC, 128), 0, 128);
    apocalypse_reset_callbacks();
    result = ResetGraph(mode);
    graph_type = (uint8)GetGraphType();
    w_u8(0x800FCEAC, graph_type);
    w_u8(0x800FCEAD, 1);
    w_u16(0x800FCEB0, r_u16(0x800FCF2C + 4 * graph_type));
    w_u16(0x800FCEB2, r_u16(0x800FCF38 + 4 * graph_type));
    memset(psx_addr(0x800FCEBC, 92), 255, 92);
    memset(psx_addr(0x800FCF18, 20), 255, 20);
    return result < 0 ? result : (sint32)graph_type;
}

static void UpdateDrawClip(void)
{
    uint32 slot = (uint32)(sint32)(sint16)r_u16(0x80104414) * 2;
    w_u16(0x80104396, r_u16(0x8010440E));
    w_u16(0x80104394, r_u16(0x8010440C));
    w_u16(0x80104390, r_u16(0x80104408) + r_u16(0x80104378 + slot));
    w_u16(0x80104392, r_u16(0x8010440A) + r_u16(0x8010437C + slot));
    PutDrawEnv((DRAWENV *)psx_addr(0x80104390, sizeof(DRAWENV)));
}

static void UpdateDrawOffset(void)
{
    if (r_u16(0x80104416) != 0)
    {
        uint32 slot = (uint32)(sint32)(sint16)r_u16(0x80104414) * 2;
        w_u16(0x80104406, 0);
        w_u16(0x80104404, 0);
        w_u16(0x80104398, r_u16(0x80104388) + r_u16(0x80104378 + slot));
        w_u16(0x8010439A, r_u16(0x8010438A) + r_u16(0x8010437C + slot));
        PutDrawEnv((DRAWENV *)psx_addr(0x80104390, sizeof(DRAWENV)));
    }
    else
    {
        uint32 slot = r_u16(0x80104414) != 0 ? 0 : 2;
        sint32 x = (sint16)r_u16(0x80104388) + (sint16)r_u16(0x80104378 + slot);
        sint32 y = (sint16)r_u16(0x8010438A) + (sint16)r_u16(0x8010437C + slot);
        SetGeomOffset(x, y);
        w_u16(0x80104404, (uint32)x);
        w_u16(0x80104406, (uint32)y);
    }
}

extern "C" void apocalypse_gs_init_graph(uint32 width, uint32 height, uint32 attributes, uint32 dither, uint32 rgb24)
{
    uint32 address;
    uint32 word;
    width &= 0xFFFF;
    height &= 0xFFFF;
    attributes &= 0xFFFF;
    dither &= 0xFFFF;
    rgb24 &= 0xFFFF;
    apocalypse_reset_graph(((attributes >> 4) & 3) == 3 ? 3 : 0);
    for (address = 0x80104398; address <= 0x801043A4; address += 2)
        w_u16(address, 0);
    w_u8(0x801043A6, dither);
    w_u8(0x801043A7, 0);
    w_u8(0x801043A8, 0);
    PutDrawEnv((DRAWENV *)psx_addr(0x80104390, sizeof(DRAWENV)));
    for (address = 0x801043F0; address <= 0x801043FE; address += 2)
        w_u16(address, 0);
    w_u16(0x801043F4, width);
    w_u16(0x801043F6, height);
    if (r_u32(0x800FCE4C) == 1)
    {
        w_u16(0x801043FA, 24);
        w_u8(0x80104402, 1);
    }
    w_u8(0x80104400, attributes & 1);
    w_u16(0x80104416, attributes & 4);
    w_u8(0x80104401, rgb24);
    PutDispEnv((DISPENV *)psx_addr(0x801043F0, sizeof(DISPENV)));
    InitGeom();
    SetBackColor(0, 0, 0);
    SetGeomOffset(0, 0);
    w_u16(0x80104406, 0);
    w_u16(0x80104404, 0);
    w_u16(0x80104414, 0);
    w_u32(0x80104418, width);
    w_u32(0x8010441C, height);
    if (width == 0)
        abort();
    for (address = 0x80104490; address <= 0x801044AC; address += 4)
        w_u32(address, 0);
    w_u16(0x80104490, 4096);
    w_u16(0x80104498, 4096);
    w_u16(0x801044A0, 4096);
    for (address = 0; address < 32; address += 4)
    {
        word = r_u32(0x80104490 + address);
        w_u32(0x801044D0 + address, word);
        w_u32(0x80104430 + address, word);
    }
    w_u16(0x80104430, 0);
    w_u16(0x80104438, 0);
    w_u16(0x80104440, 0);
    for (address = 0; address < 32; address += 4)
        w_u32(0x80104450 + address, r_u32(0x80104430 + address));
    for (address = 0x80104380; address <= 0x8010438A; address += 2)
        w_u16(address, 0);
    w_u16(0x8010440A, 0);
    w_u16(0x80104408, 0);
    w_u16(0x801044D8, (height << 14) / width / 3);
    w_u8(0x8010435B, 3);
    w_u8(0x8010435F, 2);
    w_u8(0x8010436B, 3);
    w_u8(0x8010436F, 2);
    w_u32(0x80104410, 1);
    w_u16(0x8010440C, width);
    w_u16(0x8010440E, height);
    UpdateDrawClip();
    UpdateDrawOffset();
}

extern "C" void apocalypse_gs_def_disp_buff(uint32 x0, uint32 y0, uint32 x1, uint32 y1)
{
    w_u16(0x80104378, x0);
    w_u16(0x8010437A, x1);
    w_u16(0x8010437C, y0);
    w_u16(0x8010437E, y1);
    w_u16(0x80104380, r_u16(0x80104416) != 0 ? 0 : x0);
    w_u16(0x80104382, r_u16(0x80104416) != 0 ? 0 : x1);
    w_u16(0x80104384, r_u16(0x80104416) != 0 ? 0 : y0);
    w_u16(0x80104386, r_u16(0x80104416) != 0 ? 0 : y1);
    UpdateDrawClip();
    UpdateDrawOffset();
}

extern "C" sint32 apocalypse_clear_image2(PSX_RECT *rectangle, uint8 red, uint8 green, uint8 blue)
{
    const uint32 packet_address = 0x80102A08;
    uint32 *packet = (uint32 *)psx_addr(packet_address, 56);
    GpuRasterState state;
    sint32 width_limit = (sint16)r_u16(0x800FCEB0) - 1;
    sint32 height_limit = (sint16)r_u16(0x800FCEB2) - 1;
    uint32 color = red | ((uint32)green << 8) | ((uint32)blue << 16);
    uint32 xy, size, mode;
    if (!rectangle)
        abort();
    rectangle->w = rectangle->w < 0 ? 0 : rectangle->w > width_limit ? (sint16)width_limit : rectangle->w;
    rectangle->h = rectangle->h < 0 ? 0 : rectangle->h > height_limit ? (sint16)height_limit : rectangle->h;
    xy = (uint16)rectangle->x | ((uint32)(uint16)rectangle->y << 16);
    size = (uint16)rectangle->w | ((uint32)(uint16)rectangle->h << 16);
    gpu_export_raster(&state);
    mode = 0xE1000400 | ((state.draw_mode | state.draw_flags) & 0x7FF);
    if (((rectangle->x | rectangle->w) & 63) != 0)
    {
        packet[0] = 0x08000000 | ((packet_address + 40) & 0xFFFFFF);
        packet[1] = 0xE3000000;
        packet[2] = 0xE4FFFFFF;
        packet[3] = 0xE5000000;
        packet[4] = 0xE6000000;
        packet[5] = mode;
        packet[6] = 0x60000000 | color;
        packet[7] = xy;
        packet[8] = size;
        packet[10] = 0x03FFFFFF;
        packet[11] = 0xE3000000 | state.area[0] | (state.area[1] << 10);
        packet[12] = 0xE4000000 | state.area[2] | (state.area[3] << 10);
        packet[13] = 0xE5000000 | ((uint32)state.offset[0] & 0x7FF) | (((uint32)state.offset[1] & 0x7FF) << 11);
    }
    else
    {
        packet[0] = 0x05FFFFFF;
        packet[1] = 0xE6000000;
        packet[2] = mode;
        packet[3] = 0x02000000 | color;
        packet[4] = xy;
        packet[5] = size;
    }
    DrawOTag(packet);
    return 0;
}

extern "C" void sub_800879EC(uint32 projection_distance)
{
    FUNCTION_MARKER(0x800879EC, "SLUS_003.73");
    xport_draft_gte_control_write(26, projection_distance);
}

extern "C" uint32 sub_80087F60(void)
{
    FUNCTION_MARKER(0x80087F60, "SLUS_003.73");
    return r_u32(0x800FCE4C);
}

extern "C" uint32 sub_8008AFFC(uint32 a1)
{
    FUNCTION_MARKER(0x8008AFFC, "SLUS_003.73");
    sint32 result;
    w_u8(((uint32)((a1 + 3))), 9);
    result = 44;
    w_u8(((uint32)((a1 + 7))), 44);
    return result;
}

extern "C" void sub_800879CC(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x800879CC, "SLUS_003.73");
    SetGeomOffset((sint32)a1, (sint32)a2);
}

extern "C" void sub_800878DC(uint32 matrix)
{
    FUNCTION_MARKER(0x800878DC, "SLUS_003.73");
    uint32 index;
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, r_u32(matrix + index * 4));
}

extern "C" void sub_8008798C(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8008798C, "SLUS_003.73");
    SetBackColor((sint32)red, (sint32)green, (sint32)blue);
}

extern "C" void sub_800879AC(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800879AC, "SLUS_003.73");
    SetFarColor((sint32)red, (sint32)green, (sint32)blue);
}

extern "C" void sub_8008793C(uint32 matrix)
{
    FUNCTION_MARKER(0x8008793C, "SLUS_003.73");
    uint32 words[5];
    uint32 index;
    for (index = 0; index < 5; ++index)
        words[index] = r_u32(matrix + index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(16 + index, words[index]);
}

extern "C" uint32 sub_80087E5C(uint32 destination, uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x80087E5C, "SLUS_003.73");
    SetDefDrawEnv((DRAWENV *)psx_addr(destination, sizeof(DRAWENV)), (sint16)x, (sint16)y, (sint16)width, (sint32)height);
    w_u8(destination + 23, (sint32)height < (r_u32(0x800FCE4C) != 0 ? 289 : 257));
    return destination;
}

extern "C" uint32 sub_80087F10(uint32 destination, uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x80087F10, "SLUS_003.73");
    SetDefDispEnv((DISPENV *)psx_addr(destination, sizeof(DISPENV)), (sint16)x, (sint16)y, (sint16)width, (sint16)height);
    return destination;
}

extern "C" uint32 sub_80087F7C(uint32 mode)
{
    FUNCTION_MARKER(0x80087F7C, "SLUS_003.73");
    return (uint32)apocalypse_reset_graph((sint32)mode);
}

extern "C" uint32 sub_800880F0(uint32 level)
{
    FUNCTION_MARKER(0x800880F0, "SLUS_003.73");
    return (uint32)SetGraphDebug((sint32)level);
}

extern "C" uint32 sub_800882F8(uint32 mode)
{
    FUNCTION_MARKER(0x800882F8, "SLUS_003.73");
    return (uint32)DrawSync((sint32)mode);
}

extern "C" uint32 sub_80088B28(uint32 environment)
{
    FUNCTION_MARKER(0x80088B28, "SLUS_003.73");
    PutDispEnv((DISPENV *)psx_addr(environment, sizeof(DISPENV)));
    return environment;
}

extern "C" uint32 sub_8008895C(uint32 environment)
{
    FUNCTION_MARKER(0x8008895C, "SLUS_003.73");
    uint32 index;
    if (r_u8(0x800FCEAE) >= 2)
        GameDiagnostic("PutDrawEnv(%08x)...\n", environment);
    PutDrawEnv((DRAWENV *)psx_addr(environment, sizeof(DRAWENV)));
    for (index = 0; index < 92; ++index)
        w_u8(0x800FCEBC + index, r_u8(environment + index));
    return environment;
}

extern "C" uint32 sub_800888EC(uint32 ordering_table)
{
    FUNCTION_MARKER(0x800888EC, "SLUS_003.73");
    if (r_u8(0x800FCEAE) >= 2)
        GameDiagnostic("DrawOTag(%08x)...\n", ordering_table);
    DrawOTag((uint32 *)psx_addr(ordering_table, 4));
    /* Native submission uses the original immediate queue return contract */
    return 0;
}
