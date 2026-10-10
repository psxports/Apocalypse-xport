#include "psx.h"
#include "psx_gpu.h"
#include "game_gpu_present.h"
#include <stdlib.h>
#include <string.h>

sint32 apocalypse_reset_graph(sint32 mode)
{
    uint32 kind = (uint32)mode & 7u;
    uint32 graph_type;
    sint32 result;
    extern uint32 apocalypse_reset_callbacks(void);
    if (kind != 0u && kind != 3u && kind != 5u)
        return ResetGraph(mode);
    memset(psx_addr(0x800FCEACu, 128u), 0, 128u);
    apocalypse_reset_callbacks();
    result = ResetGraph(mode);
    graph_type = (uint8)GetGraphType();
    w_u8(0x800FCEACu, graph_type);
    w_u8(0x800FCEADu, 1u);
    w_u16(0x800FCEB0u, r_u16(0x800FCF2Cu + 4u * graph_type));
    w_u16(0x800FCEB2u, r_u16(0x800FCF38u + 4u * graph_type));
    memset(psx_addr(0x800FCEBCu, 92u), 255, 92u);
    memset(psx_addr(0x800FCF18u, 20u), 255, 20u);
    return result < 0 ? result : (sint32)graph_type;
}

static void update_draw_clip(void)
{
    uint32 slot = (uint32)(sint32)(sint16)r_u16(0x80104414u) * 2u;
    w_u16(0x80104396u, r_u16(0x8010440Eu));
    w_u16(0x80104394u, r_u16(0x8010440Cu));
    w_u16(0x80104390u, r_u16(0x80104408u) + r_u16(0x80104378u + slot));
    w_u16(0x80104392u, r_u16(0x8010440Au) + r_u16(0x8010437Cu + slot));
    PutDrawEnv((DRAWENV *)psx_addr(0x80104390u, sizeof(DRAWENV)));
}

static void update_draw_offset(void)
{
    if (r_u16(0x80104416u) != 0u)
    {
        uint32 slot = (uint32)(sint32)(sint16)r_u16(0x80104414u) * 2u;
        w_u16(0x80104406u, 0u);
        w_u16(0x80104404u, 0u);
        w_u16(0x80104398u, r_u16(0x80104388u) + r_u16(0x80104378u + slot));
        w_u16(0x8010439Au, r_u16(0x8010438Au) + r_u16(0x8010437Cu + slot));
        PutDrawEnv((DRAWENV *)psx_addr(0x80104390u, sizeof(DRAWENV)));
    }
    else
    {
        uint32 slot = r_u16(0x80104414u) != 0u ? 0u : 2u;
        sint32 x = (sint16)r_u16(0x80104388u) + (sint16)r_u16(0x80104378u + slot);
        sint32 y = (sint16)r_u16(0x8010438Au) + (sint16)r_u16(0x8010437Cu + slot);
        SetGeomOffset(x, y);
        w_u16(0x80104404u, (uint32)x);
        w_u16(0x80104406u, (uint32)y);
    }
}

void apocalypse_gs_init_graph(uint32 width, uint32 height, uint32 attributes, uint32 dither, uint32 rgb24)
{
    uint32 address;
    uint32 word;
    width &= 0xFFFFu;
    height &= 0xFFFFu;
    attributes &= 0xFFFFu;
    dither &= 0xFFFFu;
    rgb24 &= 0xFFFFu;
    apocalypse_reset_graph(((attributes >> 4) & 3u) == 3u ? 3 : 0);
    for (address = 0x80104398u; address <= 0x801043A4u; address += 2u)
        w_u16(address, 0u);
    w_u8(0x801043A6u, dither);
    w_u8(0x801043A7u, 0u);
    w_u8(0x801043A8u, 0u);
    PutDrawEnv((DRAWENV *)psx_addr(0x80104390u, sizeof(DRAWENV)));
    for (address = 0x801043F0u; address <= 0x801043FEu; address += 2u)
        w_u16(address, 0u);
    w_u16(0x801043F4u, width);
    w_u16(0x801043F6u, height);
    if (r_u32(0x800FCE4Cu) == 1u)
    {
        w_u16(0x801043FAu, 24u);
        w_u8(0x80104402u, 1u);
    }
    w_u8(0x80104400u, attributes & 1u);
    w_u16(0x80104416u, attributes & 4u);
    w_u8(0x80104401u, rgb24);
    PutDispEnv((DISPENV *)psx_addr(0x801043F0u, sizeof(DISPENV)));
    InitGeom();
    SetBackColor(0, 0, 0);
    SetGeomOffset(0, 0);
    w_u16(0x80104406u, 0u);
    w_u16(0x80104404u, 0u);
    w_u16(0x80104414u, 0u);
    w_u32(0x80104418u, width);
    w_u32(0x8010441Cu, height);
    if (width == 0u)
        abort();
    for (address = 0x80104490u; address <= 0x801044ACu; address += 4u)
        w_u32(address, 0u);
    w_u16(0x80104490u, 4096u);
    w_u16(0x80104498u, 4096u);
    w_u16(0x801044A0u, 4096u);
    for (address = 0u; address < 32u; address += 4u)
    {
        word = r_u32(0x80104490u + address);
        w_u32(0x801044D0u + address, word);
        w_u32(0x80104430u + address, word);
    }
    w_u16(0x80104430u, 0u);
    w_u16(0x80104438u, 0u);
    w_u16(0x80104440u, 0u);
    for (address = 0u; address < 32u; address += 4u)
        w_u32(0x80104450u + address, r_u32(0x80104430u + address));
    for (address = 0x80104380u; address <= 0x8010438Au; address += 2u)
        w_u16(address, 0u);
    w_u16(0x8010440Au, 0u);
    w_u16(0x80104408u, 0u);
    w_u16(0x801044D8u, (height << 14) / width / 3u);
    w_u8(0x8010435Bu, 3u);
    w_u8(0x8010435Fu, 2u);
    w_u8(0x8010436Bu, 3u);
    w_u8(0x8010436Fu, 2u);
    w_u32(0x80104410u, 1u);
    w_u16(0x8010440Cu, width);
    w_u16(0x8010440Eu, height);
    update_draw_clip();
    update_draw_offset();
}

void apocalypse_gs_def_disp_buff(uint32 x0, uint32 y0, uint32 x1, uint32 y1)
{
    w_u16(0x80104378u, x0);
    w_u16(0x8010437Au, x1);
    w_u16(0x8010437Cu, y0);
    w_u16(0x8010437Eu, y1);
    w_u16(0x80104380u, r_u16(0x80104416u) != 0u ? 0u : x0);
    w_u16(0x80104382u, r_u16(0x80104416u) != 0u ? 0u : x1);
    w_u16(0x80104384u, r_u16(0x80104416u) != 0u ? 0u : y0);
    w_u16(0x80104386u, r_u16(0x80104416u) != 0u ? 0u : y1);
    update_draw_clip();
    update_draw_offset();
}

sint32 apocalypse_clear_image2(PSX_RECT *rectangle, uint8 red, uint8 green, uint8 blue)
{
    const uint32 packet_address = 0x80102A08u;
    uint32 *packet = (uint32 *)psx_addr(packet_address, 56u);
    GpuRasterState state;
    sint32 width_limit = (sint16)r_u16(0x800FCEB0u) - 1;
    sint32 height_limit = (sint16)r_u16(0x800FCEB2u) - 1;
    uint32 color = red | ((uint32)green << 8) | ((uint32)blue << 16);
    uint32 xy, size, mode;
    if (!rectangle)
        abort();
    rectangle->w = rectangle->w < 0 ? 0 : rectangle->w > width_limit ? (sint16)width_limit : rectangle->w;
    rectangle->h = rectangle->h < 0 ? 0 : rectangle->h > height_limit ? (sint16)height_limit : rectangle->h;
    xy = (uint16)rectangle->x | ((uint32)(uint16)rectangle->y << 16);
    size = (uint16)rectangle->w | ((uint32)(uint16)rectangle->h << 16);
    gpu_export_raster(&state);
    mode = 0xE1000400u | ((state.draw_mode | state.draw_flags) & 0x7FFu);
    if (((rectangle->x | rectangle->w) & 63) != 0)
    {
        packet[0] = 0x08000000u | ((packet_address + 40u) & 0xFFFFFFu);
        packet[1] = 0xE3000000u;
        packet[2] = 0xE4FFFFFFu;
        packet[3] = 0xE5000000u;
        packet[4] = 0xE6000000u;
        packet[5] = mode;
        packet[6] = 0x60000000u | color;
        packet[7] = xy;
        packet[8] = size;
        packet[10] = 0x03FFFFFFu;
        packet[11] = 0xE3000000u | state.area[0] | (state.area[1] << 10);
        packet[12] = 0xE4000000u | state.area[2] | (state.area[3] << 10);
        packet[13] = 0xE5000000u | ((uint32)state.offset[0] & 0x7FFu) | (((uint32)state.offset[1] & 0x7FFu) << 11);
    }
    else
    {
        packet[0] = 0x05FFFFFFu;
        packet[1] = 0xE6000000u;
        packet[2] = mode;
        packet[3] = 0x02000000u | color;
        packet[4] = xy;
        packet[5] = size;
    }
    DrawOTag(packet);
    return 0;
}
