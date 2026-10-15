extern "C"
{
#include "game_startup_adapters.h"
#include "draft_first_adapters.h"
#include "psx_spu.h"
}
#include <string.h>
#include "game_data_io.h"

extern "C" sint32 CdRead(sint32 count, uint32 *destination, sint32 mode);

static uint8 *native_destination;
static sint32 native_read_error;

extern "C" uint32 game_startup_read_begin(void *host_destination, uint32 guest_destination)
{
    sint32 sector;
    sub_8002F130();
    native_destination = (uint8 *)host_destination;
    native_read_error = 0;
    if (host_destination == 0)
        w_u32(0x800FFC40, guest_destination);
    sector = (sint32)((uint32)((sint32)r_u32(0x800FFC48) / 2048) + r_u32(0x800FFC38));
    CdIntToPos(sector, (CdlLOC *)psx_addr(0x800FFC3C, sizeof(CdlLOC)));
    w_u32(0x800FF700, 1);
    w_u32(0x800FF2B8, 0);
    w_u32(0x800FF2BC, 1);
    w_u32(0x800FF2C0, 0);
    w_u32(0x800FF6FC, 0);
    return 1;
}

extern "C" uint32 game_startup_read_step(void)
{
    uint32 state = r_u32(0x800FF700), error = 0;
    if (state == 1)
    {
        w_u32(0x800FF6FC, 0);
#if defined(LOCKSTEP_DEBUG)
        if (CdControl(2, (uint8 *)psx_addr(0x800FFC3C, 3), 0))
#else
        if (CdControl(9, 0, 0))
#endif
            w_u32(0x800FF700, 2);
        else
            error = 1;
    }
    else if (state == 2)
    {
        uint32 count = r_u32(0x800FFC44);
        uint8 *destination = native_destination;
        native_read_error = count == 0 ? -1 : 0;
        if (destination == 0 && count != 0)
            destination = (uint8 *)psx_addr(r_u32(0x800FFC40), count * 2048);
#if defined(LOCKSTEP_DEBUG)
        // Stop streaming and restore the resource position immediately before reading
        if (native_read_error == 0 && (!CdControl(9, 0, 0) || !CdControl(2, (uint8 *)psx_addr(0x800FFC3C, 3), 0)))
            native_read_error = -1;
        if (native_read_error == 0 && !CdRead((sint32)count, (uint32 *)destination, 0))
            native_read_error = -1;
#else
        if (native_read_error == 0 && !ReadDataResource(destination, r_u32(0x800FFC48), count * 2048))
            native_read_error = -1;
#endif
        if (native_read_error == 0)
            w_u32(0x800FF700, 3);
        else
        {
            error = 1;
            w_u32(0x800FF700, 1);
        }
    }
    else if (state == 3)
    {
        if (r_u32(0x800FF6FC) == r_u32(0x800FFC2C))
        {
            w_u32(0x800FF700, 0);
            w_u32(0x800FF708, 0);
            native_destination = 0;
        }
        else if (native_read_error >= 0)
            w_u32(0x800FF6FC, r_u32(0x800FFC2C));
        else
        {
            uint32 failures = r_u32(0x800FF710) + 1;
            w_u32(0x800FF710, failures);
            error = 1;
            if ((sint32)failures >= 5)
            {
                w_u32(0x800FF710, 0);
                w_u32(0x800FF700, 1);
            }
        }
    }
    if (error != 0)
    {
        PSX_RECT rectangle;
        memcpy(&rectangle, psx_addr(0x800FF718, sizeof(rectangle)), sizeof(rectangle));
        ClearImage(&rectangle, 255, 0, 0);
        rectangle.y = (sint16)((uint16)rectangle.y + 256);
        ClearImage(&rectangle, 255, 0, 0);
    }
    return sub_80016560();
}

extern "C" uint32 xport_draft_host_sub_8006B234_p1(void *destination)
{
    FUNCTION_MARKER(0x8006B234, "SLUS_003.73");
    return game_startup_read_begin(destination, 0);
}

extern "C" uint32 xport_draft_host_sub_800885A4_p1(const sint16 *rectangle, uint32 pixels)
{
    FUNCTION_MARKER(0x800885A4, "SLUS_003.73");
    PSX_RECT native_rectangle;
    uint32 bytes;
    memcpy(&native_rectangle, rectangle, sizeof(native_rectangle));
    bytes = (uint32)(uint16)native_rectangle.w * (uint32)(uint16)native_rectangle.h * 2;
    return (uint32)LoadImagePSX(&native_rectangle, (uint32 *)psx_addr(pixels, bytes));
}
