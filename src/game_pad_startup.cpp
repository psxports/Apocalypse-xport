extern "C"
{
#include "game_pad_startup.h"
#include "psx.h"
}
#include <string.h>

static void ResetPadOwner(uint32 owner)
{
    uint32 index;
    if (r_u8(owner + 73) == 0)
        return;
    if (r_u8(owner + 232) == 8)
    {
        uint32 child = r_u32(owner + 12);
        for (index = 0; index < 4; ++index, child += 240)
            ResetPadOwner(child);
    }
    w_u8(owner + 73, 0);
    w_u8(owner + 70, 0);
    w_u16(owner + 230, 0);
    w_u32(owner + 20, 0);
    w_u32(owner + 24, 0);
    w_u8(owner + 227, 0);
    w_u8(owner + 228, 0);
    w_u16(owner + 230, 0);
    w_u8(owner + 233, 0);
    w_u8(owner + 234, 0);
    w_u32(owner, 0);
    w_u32(owner + 4, 0);
    w_u32(owner + 8, 0);
    memset(psx_addr(owner + 93, 6), 255, 6);
}

// Native callback discards the original cleanup return carrier
static uint32 ResetPadOwnerCallback(uint32 owner)
{
    ResetPadOwner(owner);
    return 0;
}

extern "C" uint32 apocalypse_pad_init_mtap(uint32 first_packet, uint32 second_packet)
{
    uint32 port, index;
    w_u32(0x800FEE2C, 0);
    w_u32(0x800FEE40, 1);
    w_u32(0x800FEE0C, 0x8009F090);
    w_u32(0x800FEE10, 0x8009F87C);
    w_u32(0x800FEE14, 0x8009F240);
    w_u32(0x800FEDF4, 0x8009EA80);
    w_u32(0x800FEDF8, 0x8009E9BC);
    w_u32(0x800FEDFC, 0x8009EBD4);
    w_u32(0x800FEE00, 0x8009ECE4);
    w_u32(0x800FEE04, 0x8009EF60);
    w_u32(0x800FEE08, 0x8009EFDC);
    w_u32(0x800FEE18, 0x8009EB90);
    w_u32(0x800FEE28, 0x80107D88);
    memset(psx_addr(0x80107D88, 480), 0, 480);
    memset(psx_addr(0x80107F68, 1920), 0, 1920);
    w_u32(0x80107DB8, first_packet);
    w_u32(0x80107EA8, second_packet);
    for (port = 0; port < 2; ++port)
    {
        uint32 owner = 0x80107D88 + 240 * port;
        uint32 children = 0x80107F68 + 960 * port;
        uint32 packet = r_u32(owner + 48);
        uint32 receive = 0x80107CF8 + 35 * port;
        uint32 send = 0x80107D40 + 35 * port;
        w_u32(owner + 12, children);
        w_u32(owner + 16, owner);
        w_u8(packet, 255);
        w_u8(packet + 1, 0);
        w_u32(owner + 60, receive);
        w_u32(owner + 64, send);
        memset(psx_addr(owner + 93, 6), 255, 6);
        for (index = 0; index < 4; ++index)
        {
            uint32 child = children + 240 * index;
            uint32 child_packet = packet + 2 + 8 * index;
            w_u32(child + 16, owner);
            w_u32(child + 48, child_packet);
            w_u8(child + 55, 255);
            w_u8(child + 53, 0);
            w_u8(child + 52, 0);
            w_u8(child_packet, 255);
            w_u8(child_packet + 1, 0);
            w_u32(child + 60, receive + 2 + 8 * index);
            w_u32(child + 64, send + 3 + 8 * index);
            memset(psx_addr(child + 93, 6), 255, 6);
        }
    }
    sub_8009CD74();
    w_u32(0x800FEE2C, 1);
    PadInit(0);
    PadInitDirect((uint8 *)psx_addr(first_packet, 35), (uint8 *)psx_addr(second_packet, 35));
    static const PsxPadCallbackBinding callbacks[] = {{0x8009E9BC, ResetPadOwnerCallback}};
    psx_pad_bind_callbacks(callbacks, 1);
    psx_pad_bind_owner_packets(0x80107D88, 0x80107E78, 0x8009E9BC, 0x801086E8, 0x800FEE40, 0x800FEE3C);
    psx_bios_bind_tap_interrupt(0x80107CC8);
    return 1;
}

extern "C" uint32 sub_8009C7CC(void)
{
    FUNCTION_MARKER(0x8009C7CC, "SLUS_003.73");
    uint32 mask;
    w_u32(0x800FEE2C, 0);
    xport_bios_enter_critical();
    SysDeqIntRP(2, 0x80107CC8);
    SysEnqIntRP(2, 0x80107CC8);
    mask = r_u32(0x800FEE54);
    w_u32(mask, 0xFFFFFFFE);
    w_u32(mask + 4, r_u32(mask + 4) | 1);
    ChangeClearRCnt(3, 0);
    xport_bios_exit_critical();
    ResetPadOwner(r_u32(0x800FEE28));
    ResetPadOwner(r_u32(0x800FEE28) + 240);
    w_u32(0x80107CDC, 0);
    w_u32(0x80107CD8, 0);
    w_u32(0x800FEE2C, 1);
    PadStartCom();
    return 1;
}
