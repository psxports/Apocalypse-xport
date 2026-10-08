#include "game_cd_startup.h"
#include "draft_first_adapters.h"
#include "psx_spu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern sint32 CdRead(sint32 count, uint32 *destination, sint32 mode);
extern sint32 CdReadSync(sint32 mode, uint8 *result);

static void cd_startup_todo(const char *operation)
{
    fprintf(stderr, "TODO CD startup: %s\n", operation);
    abort();
}

uint32 sub_80097D14(uint32 mode)
{
    uint32 previous = r_u32(0x800FDA30u);
    w_u32(0x800FDA30u, mode);
    return previous;
}

uint32 sub_80097B4C(void)
{
    sint32 attempt;
    for (attempt = 4; attempt != -1; --attempt)
        if (CdInit() == 1)
        {
            w_u32(0x800FDA24u, 0x80097BD0u);
            w_u32(0x800FDA28u, 0x80097BF8u);
            w_u32(0x800FDD4Cu, 0x80097C20u);
            w_u32(0x800FDD80u, 0u);
            return 1u;
        }
    fprintf(stderr, "%s", (const char *)psx_addr(0x800A43D4u, 1u));
    return 0u;
}

uint32 xport_draft_host_sub_8009835C_p1(const void *location)
{
    const uint8 *position = (const uint8 *)location;
    uint32 x = position[0], y = position[1], z = position[2];
    return 75u * (60u * (10u * (x >> 4) + (x & 15u)) + 10u * (y >> 4) + (y & 15u)) + 10u * (z >> 4) + (z & 15u) - 150u;
}

uint32 xport_draft_host_sub_80099B3C_p1(void *result, uint32 name)
{
    return cd_search_file_native(result, (const char *)psx_addr(name, 1u)) != NULL;
}

uint32 sub_80099B3C(uint32 result, uint32 name)
{
    return CdSearchFile(result, name);
}

static uint32 cd_startup_control(uint32 command, const void *parameter, void *output)
{
    uint32 success = (uint32)CdControlB((uint8)command, (uint8 *)parameter, (uint8 *)output);
    if (success != 0u && (uint8)command == 2u && parameter != NULL)
        memcpy(psx_addr(0x800FDA40u, 3u), parameter, 3u);
    if (success != 0u)
        w_u8(0x800FDA45u, command);
    return success;
}

uint32 xport_draft_host_sub_80097DF8_p2(uint32 command, const void *parameter, uint32 result)
{
    return cd_startup_control(command, parameter, result ? psx_addr(result, 8u) : NULL);
}

uint32 xport_draft_host_sub_80097DF8_p3(uint32 command, uint32 parameter, void *result)
{
    return cd_startup_control(command, parameter ? psx_addr(parameter, 1u) : NULL, result);
}

uint32 sub_80097F34(uint32 command, uint32 parameter)
{
    return xport_draft_host_sub_80097F34_p2(command, parameter ? psx_addr(parameter, 1u) : NULL);
}

uint32 xport_draft_host_sub_80097F34_p2(uint32 command, void *parameter)
{
    uint32 success = (uint32)CdControlF((uint8)command, (uint8 *)parameter);
    if (success != 0u)
        w_u8(0x800FDA45u, command);
    return success;
}

uint32 sub_80097C6C(void)
{
    return r_u8(0x800FDA45u);
}

uint32 sub_80097D90(uint32 mode, uint32 result)
{
    return (uint32)CD_sync((sint32)mode, result ? (uint8 *)psx_addr(result, 8u) : NULL);
}

uint32 sub_80097DF8(uint32 command, uint32 parameter, uint32 result)
{
    return xport_draft_host_sub_80097DF8_p2(command, parameter ? psx_addr(parameter, 1u) : NULL, result);
}

void xport_draft_host_sub_80098258_p2(uint32 sector, void *location)
{
    uint8 *position = (uint8 *)location;
    sint32 value = (sint32)(sector + 150u);
    sint32 frame = value % 75;
    sint32 second = value / 75 % 60;
    sint32 minute = value / 75 / 60;
    position[2] = (uint8)((uint32)(frame / 10) * 16u + (uint32)(frame % 10));
    position[1] = (uint8)((uint32)(second / 10) * 16u + (uint32)(second % 10));
    position[0] = (uint8)((uint32)(minute / 10) * 16u + (uint32)(minute % 10));
}

uint32 sub_8009AA6C(uint32 count, uint32 destination, uint32 mode)
{
    uint32 words = (mode & 0x30u) == 0u ? 512u : ((mode & 0x30u) == 32u ? 585u : 582u);
    uint32 bytes = words * 4u, previous_sync, previous_ready, completed, location;
    sint32 success;
    uint8 response;
    if ((mode & 0x30u) == 0x10u || (mode & 0x30u) == 0x30u)
        cd_startup_todo("9AA6C unsupported 2328-byte transfer mode");
    if ((r_u32(0x800FDD80u) & 1u) != 0u)
        cd_startup_todo("9AA6C alternate guest data callback mode");
    w_u32(0x800FDD5Cu, mode | 0x20u);
    w_u32(0x800FDD60u, words);
    w_u32(0x800FDD54u, destination);
    w_u32(0x800FDD50u, count);
    previous_sync = r_u32(0x800FDA24u); previous_ready = r_u32(0x800FDA28u);
    w_u32(0x800FDD74u, previous_sync); w_u32(0x800FDD78u, previous_ready);
    w_u32(0x800FDA24u, 0u); w_u32(0x800FDA28u, 0u);
    w_u32(0x800FDD6Cu, (uint32)VSync(-1));
    location = sub_8009835C(0x800FDA40u);
    w_u32(0x800FDD70u, location);
    if (count == 0u || count > 0x7FFFFFFFu || count > 0xFFFFFFFFu / bytes)
        success = 0;
    else
        success = CdRead((sint32)count, (uint32 *)psx_addr(destination, (size_t)count * bytes), (sint32)mode);
    completed = (uint32)CdReadSync(1, &response);
    w_u32(0x800FDD64u, success != 0 && completed == 0u ? 0u : 0xFFFFFFFFu);
    w_u32(0x800FDD58u, success != 0 ? destination + count * bytes : destination);
    if (success != 0) w_u32(0x800FDD70u, location + count);
    w_u32(0x800FDD68u, (uint32)VSync(-1));
    w_u32(0x800FDA24u, previous_sync); w_u32(0x800FDA28u, previous_ready);
    if (r_u32(0x800FDD4Cu) == 0x80097C20u)
        DeliverEvent(0xF0000003u, 0x40u);
    else if (r_u32(0x800FDD4Cu) != 0u)
        cd_startup_todo("Dispatch custom guest CD read completion callback");
    return success != 0u;
}

uint32 xport_draft_host_sub_8009AB6C_p2(uint32 mode, void *result)
{
    sint32 completed = CdReadSync((sint32)mode, (uint8 *)result);
    w_u32(0x800FDD64u, (uint32)completed);
    return (uint32)completed;
}

uint32 sub_8009AB6C(uint32 mode, uint32 result)
{
    return xport_draft_host_sub_8009AB6C_p2(mode, result ? psx_addr(result, 8u) : NULL);
}
