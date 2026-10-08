#include "game_pad_startup.h"
#include "psx.h"
#include <string.h>

static void reset_pad_owner(uint32 owner)
{
    uint32 index;
    if (r_u8(owner + 73u) == 0u)
        return;
    if (r_u8(owner + 232u) == 8u)
        for (index = 0u; index < 4u; ++index)
            reset_pad_owner(r_u32(owner + 12u) + 240u * index);
    w_u8(owner + 73u, 0u);
    w_u8(owner + 70u, 0u);
    w_u16(owner + 230u, 0u);
    w_u32(owner + 20u, 0u);
    w_u32(owner + 24u, 0u);
    w_u8(owner + 227u, 0u);
    w_u8(owner + 228u, 0u);
    w_u16(owner + 230u, 0u);
    w_u8(owner + 233u, 0u);
    w_u8(owner + 234u, 0u);
    w_u32(owner, 0u);
    w_u32(owner + 4u, 0u);
    w_u32(owner + 8u, 0u);
    memset(psx_addr(owner + 93u, 6u), 255, 6u);
}

uint32 apocalypse_pad_init_mtap(uint32 first_packet, uint32 second_packet)
{
    uint32 port, index;
    w_u32(0x800FEE2Cu, 0u);
    w_u32(0x800FEE40u, 1u);
    w_u32(0x800FEE0Cu, 0x8009F090u);
    w_u32(0x800FEE10u, 0x8009F87Cu);
    w_u32(0x800FEE14u, 0x8009F240u);
    w_u32(0x800FEDF4u, 0x8009EA80u);
    w_u32(0x800FEDF8u, 0x8009E9BCu);
    w_u32(0x800FEDFCu, 0x8009EBD4u);
    w_u32(0x800FEE00u, 0x8009ECE4u);
    w_u32(0x800FEE04u, 0x8009EF60u);
    w_u32(0x800FEE08u, 0x8009EFDCu);
    w_u32(0x800FEE18u, 0x8009EB90u);
    w_u32(0x800FEE28u, 0x80107D88u);
    memset(psx_addr(0x80107D88u, 480u), 0, 480u);
    memset(psx_addr(0x80107F68u, 1920u), 0, 1920u);
    w_u32(0x80107DB8u, first_packet);
    w_u32(0x80107EA8u, second_packet);
    for (port = 0u; port < 2u; ++port)
    {
        uint32 owner = 0x80107D88u + 240u * port;
        uint32 children = 0x80107F68u + 960u * port;
        uint32 packet = r_u32(owner + 48u);
        uint32 receive = 0x80107CF8u + 35u * port;
        uint32 send = 0x80107D40u + 35u * port;
        w_u32(owner + 12u, children);
        w_u32(owner + 16u, owner);
        w_u8(packet, 255u);
        w_u8(packet + 1u, 0u);
        w_u32(owner + 60u, receive);
        w_u32(owner + 64u, send);
        memset(psx_addr(owner + 93u, 6u), 255, 6u);
        for (index = 0u; index < 4u; ++index)
        {
            uint32 child = children + 240u * index;
            uint32 child_packet = packet + 2u + 8u * index;
            w_u32(child + 16u, owner);
            w_u32(child + 48u, child_packet);
            w_u8(child + 55u, 255u);
            w_u8(child + 53u, 0u);
            w_u8(child + 52u, 0u);
            w_u8(child_packet, 255u);
            w_u8(child_packet + 1u, 0u);
            w_u32(child + 60u, receive + 2u + 8u * index);
            w_u32(child + 64u, send + 3u + 8u * index);
            memset(psx_addr(child + 93u, 6u), 255, 6u);
        }
    }
    sub_8009CD74();
    w_u32(0x800FEE2Cu, 1u);
    PadInit(0);
    PadInitDirect((uint8 *)psx_addr(first_packet, 35u), (uint8 *)psx_addr(second_packet, 35u));
    psx_pad_bind_owner_packets(0x80107D88u, 0x80107E78u);
    psx_bios_bind_tap_interrupt(0x80107CC8u);
    return 1u;
}

uint32 sub_8009C7CC(void)
{
    uint32 mask;
    w_u32(0x800FEE2Cu, 0u);
    xport_bios_enter_critical();
    SysDeqIntRP(2u, 0x80107CC8u);
    SysEnqIntRP(2u, 0x80107CC8u);
    mask = r_u32(0x800FEE54u);
    w_u32(mask, 0xFFFFFFFEu);
    w_u32(mask + 4u, r_u32(mask + 4u) | 1u);
    ChangeClearRCnt(3u, 0u);
    xport_bios_exit_critical();
    reset_pad_owner(r_u32(0x800FEE28u));
    reset_pad_owner(r_u32(0x800FEE28u) + 240u);
    w_u32(0x80107CDCu, 0u);
    w_u32(0x80107CD8u, 0u);
    w_u32(0x800FEE2Cu, 1u);
    PadStartCom();
    return 1u;
}
