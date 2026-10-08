#include "game_startup_adapters.h"
#include "draft_first_adapters.h"
#include "psx_spu.h"
#include <string.h>

/* Existing physical sector backend, declared locally for native destinations */
extern sint32 cd_read_sector_native(uint8 output[2352]);
static uint8 *native_destination;
static sint32 native_read_error;

uint32 game_startup_read_begin(void *host_destination, uint32 guest_destination)
{
    sint32 sector;
    sub_8002F130();
    native_destination = (uint8 *)host_destination;
    native_read_error = 0;
    if (host_destination == NULL)
        w_u32(0x800FFC40u, guest_destination);
    sector = (sint32)r_u32(0x800FFC48u) / 2048 + (sint32)r_u32(0x800FFC38u);
    CdIntToPos(sector, (CdlLOC *)psx_addr(0x800FFC3Cu, sizeof(CdlLOC)));
    w_u32(0x800FF700u, 1u);
    w_u32(0x800FF2B8u, 0u);
    w_u32(0x800FF2BCu, 1u);
    w_u32(0x800FF2C0u, 0u);
    w_u32(0x800FF6FCu, 0u);
    return 1u;
}

uint32 game_startup_read_step(void)
{
    uint32 state = r_u32(0x800FF700u), error = 0u;
    if (state == 1u)
    {
        w_u32(0x800FF6FCu, 0u);
        if (CdControl(2u, (uint8 *)psx_addr(0x800FFC3Cu, 3u), NULL))
            w_u32(0x800FF700u, 2u);
        else
            error = 1u;
    }
    else if (state == 2u)
    {
        uint8 sector[2352];
        uint32 count = r_u32(0x800FFC44u), index;
        uint8 *destination = native_destination;
        native_read_error = count == 0u ? -1 : 0;
        if (destination == NULL && count != 0u)
            destination = (uint8 *)psx_addr(r_u32(0x800FFC40u), count * 2048u);
        for (index = 0u; index < count && native_read_error == 0; ++index)
        {
            if (!cd_read_sector_native(sector))
                native_read_error = -1;
            else
                memcpy(destination + index * 2048u, sector + 24u, 2048u);
        }
        if (native_read_error == 0)
            w_u32(0x800FF700u, 3u);
        else
        {
            error = 1u;
            w_u32(0x800FF700u, 1u);
        }
    }
    else if (state == 3u)
    {
        if (r_u32(0x800FF6FCu) == r_u32(0x800FFC2Cu))
        {
            w_u32(0x800FF700u, 0u);
            w_u32(0x800FF708u, 0u);
            native_destination = NULL;
        }
        else if (native_read_error >= 0)
            w_u32(0x800FF6FCu, r_u32(0x800FFC2Cu));
        else
        {
            uint32 failures = r_u32(0x800FF710u) + 1u;
            w_u32(0x800FF710u, failures);
            error = 1u;
            if ((sint32)failures >= 5)
            {
                w_u32(0x800FF710u, 0u);
                w_u32(0x800FF700u, 1u);
            }
        }
    }
    if (error != 0u)
    {
        PSX_RECT rectangle;
        memcpy(&rectangle, psx_addr(0x800FF718u, sizeof(rectangle)), sizeof(rectangle));
        ClearImage(&rectangle, 255u, 0u, 0u);
        rectangle.y = (sint16)((uint16)rectangle.y + 256u);
        ClearImage(&rectangle, 255u, 0u, 0u);
    }
    return sub_80016560();
}

uint32 xport_draft_host_sub_8006B234_p1(void *destination)
{
    return game_startup_read_begin(destination, 0u);
}

uint32 xport_draft_host_sub_800885A4_p1(const sint16 *rectangle, uint32 pixels)
{
    PSX_RECT native_rectangle;
    uint32 bytes;
    memcpy(&native_rectangle, rectangle, sizeof(native_rectangle));
    bytes = (uint32)(uint16)native_rectangle.w * (uint32)(uint16)native_rectangle.h * 2u;
    return (uint32)LoadImagePSX(&native_rectangle, (uint32 *)psx_addr(pixels, bytes));
}
