#include "game_sdk_bindings.h"
extern "C"
{
#include "game_cd_startup.h"
#include "draft_first_adapters.h"
#include "psx_spu.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_data_io.h"

extern "C" sint32 CdRead(sint32 count, uint32 *destination, sint32 mode);
extern "C" sint32 CdReadSync(sint32 mode, uint8 *result);

static void FailCdStartup(const char *operation)
{
    GameDiagnostic("TODO CD startup: %s\n", operation);
    abort();
}

extern "C" uint32 sub_80097D14(uint32 mode)
{
    FUNCTION_MARKER(0x80097D14, "SLUS_003.73");
    uint32 previous = r_u32(0x800FDA30);
    w_u32(0x800FDA30, mode);
    return previous;
}

extern "C" uint32 sub_80097B4C(void)
{
    FUNCTION_MARKER(0x80097B4C, "SLUS_003.73");
    sint32 attempt;
    for (attempt = 4; attempt != -1; --attempt)
        if (CdInit() == 1)
        {
            w_u32(0x800FDA24, 0x80097BD0);
            w_u32(0x800FDA28, 0x80097BF8);
            w_u32(0x800FDD4C, 0x80097C20);
            w_u32(0x800FDD80, 0);
            return 1;
        }
    GameDiagnostic("%s", (const char *)psx_addr(0x800A43D4, 1));
    return 0;
}

extern "C" uint32 xport_draft_host_sub_8009835C_p1(const void *location)
{
    FUNCTION_MARKER(0x8009835C, "SLUS_003.73");
    const uint8 *position = (const uint8 *)location;
    uint32 x = position[0], y = position[1], z = position[2];
    return 75 * (60 * (10 * (x >> 4) + (x & 15)) + 10 * (y >> 4) + (y & 15)) + 10 * (z >> 4) + (z & 15) - 150;
}

extern "C" uint32 xport_draft_host_sub_80099B3C_p1(void *result, uint32 name)
{
    FUNCTION_MARKER(0x80099B3C, "SLUS_003.73");
#if defined(LOCKSTEP_DEBUG)
    return cd_search_file_native(result, (const char *)psx_addr(name, 1)) != 0;
#else
    return FindDataCdFile((CdlFILE *)result, (const char *)psx_addr(name, 1));
#endif
}

extern "C" uint32 sub_80099B3C(uint32 result, uint32 name)
{
    FUNCTION_MARKER(0x80099B3C, "SLUS_003.73");
    return xport_draft_host_sub_80099B3C_p1(psx_addr(result, sizeof(CdlFILE)), name);
}

static uint32 ControlCdStartup(uint32 command, const void *parameter, void *output)
{
    const void *original_parameter = parameter;
#if !defined(LOCKSTEP_DEBUG)
    CdlLOC location;
    parameter = PrepareDataCdLocation(command, parameter, &location);
#endif
    uint32 success = (uint32)CdControlB((uint8)command, (uint8 *)parameter, (uint8 *)output);
    if (success != 0 && (uint8)command == 2 && parameter != 0)
        memcpy(psx_addr(0x800FDA40, 3), original_parameter, 3);
    if (success != 0)
        w_u8(0x800FDA45, command);
    return success;
}

extern "C" uint32 xport_draft_host_sub_80097DF8_p2(uint32 command, const void *parameter, uint32 result)
{
    FUNCTION_MARKER(0x80097DF8, "SLUS_003.73");
    return ControlCdStartup(command, parameter, result ? psx_addr(result, 8) : 0);
}

extern "C" uint32 xport_draft_host_sub_80097DF8_p3(uint32 command, uint32 parameter, void *result)
{
    FUNCTION_MARKER(0x80097DF8, "SLUS_003.73");
    return ControlCdStartup(command, parameter ? psx_addr(parameter, 1) : 0, result);
}

extern "C" uint32 sub_80097F34(uint32 command, uint32 parameter)
{
    FUNCTION_MARKER(0x80097F34, "SLUS_003.73");
    return xport_draft_host_sub_80097F34_p2(command, parameter ? psx_addr(parameter, 1) : 0);
}

extern "C" uint32 xport_draft_host_sub_80097F34_p2(uint32 command, void *parameter)
{
    FUNCTION_MARKER(0x80097F34, "SLUS_003.73");
#if !defined(LOCKSTEP_DEBUG)
    CdlLOC location;
    parameter = (void *)PrepareDataCdLocation(command, parameter, &location);
#endif
    uint32 success = (uint32)CdControlF((uint8)command, (uint8 *)parameter);
    if (success != 0)
        w_u8(0x800FDA45, command);
    return success;
}

extern "C" uint32 sub_80097C6C(void)
{
    FUNCTION_MARKER(0x80097C6C, "SLUS_003.73");
    return r_u8(0x800FDA45);
}

extern "C" uint32 sub_80097D90(uint32 mode, uint32 result)
{
    FUNCTION_MARKER(0x80097D90, "SLUS_003.73");
    uint8 *response = result ? (uint8 *)psx_addr(result, 8) : 0;
    uint32 completion = (uint32)CD_sync((sint32)mode, response);
#if !defined(LOCKSTEP_DEBUG)
    if (completion == 2 && r_u8(0x800FDA45) == 17)
        RebaseDataCdResponse(response);
#endif
    return completion;
}

extern "C" uint32 sub_80097DF8(uint32 command, uint32 parameter, uint32 result)
{
    FUNCTION_MARKER(0x80097DF8, "SLUS_003.73");
    return xport_draft_host_sub_80097DF8_p2(command, parameter ? psx_addr(parameter, 1) : 0, result);
}

extern "C" void xport_draft_host_sub_80098258_p2(uint32 sector, void *location)
{
    FUNCTION_MARKER(0x80098258, "SLUS_003.73");
    uint8 *position = (uint8 *)location;
    sint32 value = (sint32)(sector + 150);
    sint32 frame = value % 75;
    sint32 second = value / 75 % 60;
    sint32 minute = value / 75 / 60;
    position[2] = (uint8)((uint32)(frame / 10) * 16 + (uint32)(frame % 10));
    position[1] = (uint8)((uint32)(second / 10) * 16 + (uint32)(second % 10));
    position[0] = (uint8)((uint32)(minute / 10) * 16 + (uint32)(minute % 10));
}

extern "C" uint32 sub_8009AA6C(uint32 count, uint32 destination, uint32 mode)
{
    FUNCTION_MARKER(0x8009AA6C, "SLUS_003.73");
    uint32 words = (mode & 0x30) == 0 ? 512 : ((mode & 0x30) == 32 ? 585 : 582);
    uint32 bytes = words * 4, previous_sync, previous_ready, completed, location;
    sint32 success;
    uint8 response;
    if ((mode & 0x30) == 0x10 || (mode & 0x30) == 0x30)
        FailCdStartup("9AA6C unsupported 2328-byte transfer mode");
    if ((r_u32(0x800FDD80) & 1) != 0)
        FailCdStartup("9AA6C alternate guest data callback mode");
    w_u32(0x800FDD5C, mode | 0x20);
    w_u32(0x800FDD60, words);
    w_u32(0x800FDD54, destination);
    w_u32(0x800FDD50, count);
    previous_sync = r_u32(0x800FDA24);
    previous_ready = r_u32(0x800FDA28);
    w_u32(0x800FDD74, previous_sync);
    w_u32(0x800FDD78, previous_ready);
    w_u32(0x800FDA24, 0);
    w_u32(0x800FDA28, 0);
    w_u32(0x800FDD6C, (uint32)VSync(-1));
    location = sub_8009835C(0x800FDA40);
    w_u32(0x800FDD70, location);
    if (count == 0 || count > 0x7FFFFFFF || count > 0xFFFFFFFF / bytes)
        success = 0;
    else
        success = CdRead((sint32)count, (uint32 *)psx_addr(destination, (size_t)count * bytes), (sint32)mode);
    completed = (uint32)CdReadSync(1, &response);
    w_u32(0x800FDD64, success != 0 && completed == 0 ? 0 : 0xFFFFFFFF);
    w_u32(0x800FDD58, success != 0 ? destination + count * bytes : destination);
    if (success != 0)
        w_u32(0x800FDD70, location + count);
    w_u32(0x800FDD68, (uint32)VSync(-1));
    w_u32(0x800FDA24, previous_sync);
    w_u32(0x800FDA28, previous_ready);
    if (r_u32(0x800FDD4C) == 0x80097C20)
        DeliverEvent(0xF0000003, 0x40);
    else if (r_u32(0x800FDD4C) != 0)
        FailCdStartup("Dispatch custom guest CD read completion callback");
    return success != 0;
}

extern "C" uint32 xport_draft_host_sub_8009AB6C_p2(uint32 mode, void *result)
{
    FUNCTION_MARKER(0x8009AB6C, "SLUS_003.73");
    sint32 completed = CdReadSync((sint32)mode, (uint8 *)result);
    w_u32(0x800FDD64, (uint32)completed);
    return (uint32)completed;
}

extern "C" uint32 sub_8009AB6C(uint32 mode, uint32 result)
{
    FUNCTION_MARKER(0x8009AB6C, "SLUS_003.73");
    return xport_draft_host_sub_8009AB6C_p2(mode, result ? psx_addr(result, 8) : 0);
}

static uint32 SignalCdReadCompletion(void)
{
    FUNCTION_MARKER(0x80097C20, "SLUS_003.73");
    DeliverEvent(0xF0000003, 0x40);
    // DeliverEvent has no native result carrier
    return 0;
}

static uint32 FailSelectedCdCallback(uint32 owner, uint32 slot, uint32 target, uint32 argument0, uint32 argument1)
{
    if (target == 0x80097C20)
        return SignalCdReadCompletion();
    GameDiagnostic("Missing selected callback owner=%08X slot=%08X target=%08X args=%08X,%08X\n", owner, slot, target, argument0, argument1);
    abort();
}

extern "C" uint32 sub_8009A4BC(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x8009A4BC, "SLUS_003.73");
    uint8 location[16];
    uint32 result, reason;
    w_u32(0x800FDD84, a2);
    if ((a1 & 255) != 1)
        w_u32(0x800FDD64, 0xFFFFFFFF);
    else if ((sint32)r_u32(0x800FDD64) > 0)
    {
        if (r_u32(0x800FDD60) == 512)
        {
            if ((r_u32(0x800FDD80) & 1) != 0)
            {
                sub_80098214(0);
                xport_draft_host_sub_800981F4_p1(location, 3);
                sub_80098238(0);
                sub_80098214(0x8009A71C);
            }
            else
            {
                xport_draft_host_sub_800981D4_p1(location, 3);
            }
            if (xport_draft_host_sub_8009835C_p1(location) != r_u32(0x800FDD70))
            {
                sub_8008BF34(0x800A4844);
                w_u32(0x800FDD64, 0xFFFFFFFF);
            }
        }
        if ((r_u32(0x800FDD80) & 1) != 0)
            sub_800981F4(r_u32(0x800FDD58), r_u32(0x800FDD60));
        else
        {
            sub_800981D4(r_u32(0x800FDD58), r_u32(0x800FDD60));
            w_u32(0x800FDD58, r_u32(0x800FDD58) + (r_u32(0x800FDD60) << 2));
            w_u32(0x800FDD64, r_u32(0x800FDD64) - 1);
            w_u32(0x800FDD70, r_u32(0x800FDD70) + 1);
        }
    }
    w_u32(0x800FDD68, sub_8008632C(0xFFFFFFFF));
    if ((sint32)r_u32(0x800FDD64) < 0)
        sub_8009A7E8(1);
    result = sub_8008632C(0xFFFFFFFF);
    if ((sint32)(r_u32(0x800FDD6C) + 1200) < (sint32)result)
        w_u32(0x800FDD64, 0xFFFFFFFF);
    if (r_u32(0x800FDD64) != 0)
    {
        result = sub_8008632C(0xFFFFFFFF);
        if ((sint32)(r_u32(0x800FDD6C) + 1200) >= (sint32)result)
            return result;
    }
    sub_80097DD0(r_u32(0x800FDD74));
    sub_80097DE4(r_u32(0x800FDD78));
    if ((r_u32(0x800FDD80) & 1) != 0)
        sub_80098214(r_u32(0x800FDD7C));
    result = sub_80097F34(9, 0);
    if (r_u32(0x800FDD4C) != 0)
    {
        reason = r_u32(0x800FDD64) == 0 ? 2 : 5;
        return FailSelectedCdCallback(0x8009A4BC, 0x800FDD4C, r_u32(0x800FDD4C), reason, a2);
    }
    return result;
}

extern "C" uint32 sub_800981D4(uint32 destination, uint32 count)
{
    FUNCTION_MARKER(0x800981D4, "SLUS_003.73");
    /* TODO Bind existing SDK destination/count API during integration */
    return sub_8009986C(destination, count) == 0;
}

extern "C" uint32 sub_800981B4(uint32 volume)
{
    FUNCTION_MARKER(0x800981B4, "SLUS_003.73");
    sub_8009928C(volume);
    return 1;
}

extern "C" void sub_8009AD7C(void)
{
    FUNCTION_MARKER(0x8009AD7C, "SLUS_003.73");
    uint32 retained_result;
    sub_80085E6C();
    if (r_u32(0x800FDA48) == 1)
    {
        sub_8009C788(0);
        sub_8009C760(0);
    }
    else
    {
        sub_80098214(0);
        sub_80097DE4(0);
    }
    w_u8(r_u32(0x800FDD9C), 0);
    retained_result = r_u32(0x800FDDA8);
    w_u8(retained_result, 0);
    sub_80085E7C(retained_result);
}

extern "C" uint32 sub_8009835C(uint32 a1)
{
    FUNCTION_MARKER(0x8009835C, "SLUS_003.73");
    uint32 x = r_u8(a1), y = r_u8(a1 + 1), z = r_u8(a1 + 2);
    return 75 * (60 * (10 * (x >> 4) + (x & 15)) + 10 * (y >> 4) + (y & 15)) + 10 * (z >> 4) + (z & 15) - 150;
}

extern "C" uint32 sub_800983DC(void)
{
    FUNCTION_MARKER(0x800983DC, "SLUS_003.73");
    uint8 response[8];
    uint32 code, count = 0, error = 0, i, result, destination;
    uint32 interrupt;
    w_u8(r_u32(0x800FDCEC), 1);
    interrupt = r_u32(0x800FDCF8);
    code = r_u8(interrupt) & 7;
    if (!code)
        return 0;
    while (code != (r_u8(interrupt) & 7))
        code = r_u8(interrupt) & 7;
    while (count < 8 && (r_u8(r_u32(0x800FDCEC)) & 0x20))
        response[count++] = r_u8(r_u32(0x800FDCF0));
    for (i = count; i < 8; ++i)
        response[i] = 0;
    w_u8(r_u32(0x800FDCEC), 1);
    w_u8(r_u32(0x800FDCF8), 7);
    w_u8(r_u32(0x800FDCF4), 7);
    if (code != 3 || r_u32(0x800FDBEC + (uint32)r_u8(0x800FDA45) * 4))
    {
        if (!(r_u32(0x800FDA34) & 0x10) && (response[0] & 0x10))
            w_u32(0x800FDA3C, r_u32(0x800FDA3C) + 1);
        error = response[0] & 0x1D;
        w_u32(0x800FDA34, response[0]);
        w_u32(0x800FDA38, response[1]);
    }
    if (code == 5 && (sint32)r_u32(0x800FDA30) > 0)
    {
        GameDiagnostic("%s", (const char *)psx_addr(0x800A456C, 1));
        if ((sint32)r_u32(0x800FDA30) > 0)
            GameDiagnostic((const char *)psx_addr(0x800A4578, 1), (const char *)psx_addr(r_u32(0x800FDA4C + (uint32)r_u8(0x800FDA45) * 4), 1), r_u32(0x800FDA34), r_u32(0x800FDA38));
    }
    switch (code)
    {
        case 1:
            if (error && count == 1)
                error = 0;
            w_u8(0x800FDD05, error ? 5 : 1);
            for (i = 0; i < 8; ++i)
                w_u8(0x80105860 + i, response[i]);
            w_u8(r_u32(0x800FDCEC), 0);
            w_u8(r_u32(0x800FDCF8), 0);
            return 4;
        case 2:
            w_u8(0x800FDD04, error ? 5 : 2);
            destination = 0x80105858;
            result = 2;
            break;
        case 3:
            if (error)
            {
                w_u8(0x800FDD04, 5);
                result = 2;
            }
            else if (r_u32(0x800FDAEC + (uint32)r_u8(0x800FDA45) * 4))
            {
                w_u8(0x800FDD04, 3);
                result = 1;
            }
            else
            {
                w_u8(0x800FDD04, 2);
                result = 2;
            }
            destination = 0x80105858;
            break;
        case 4:
            w_u8(0x800FDD06, 4);
            w_u8(0x800FDD05, r_u8(0x800FDD06));
            for (i = 0; i < 8; ++i)
                w_u8(0x80105868 + i, response[i]);
            destination = 0x80105860;
            result = 4;
            break;
        case 5:
            w_u8(0x800FDD05, 5);
            w_u8(0x800FDD04, r_u8(0x800FDD05));
            for (i = 0; i < 8; ++i)
                w_u8(0x80105858 + i, response[i]);
            destination = 0x80105860;
            result = 6;
            break;
        default:
            sub_8008BF34(0x800A4594);
            sub_8008BF9C(0x800A45A8, code);
            return 0;
    }
    for (i = 0; i < 8; ++i)
        w_u8(destination + i, response[i]);
    return result;
}

extern "C" uint32 sub_8009C760(uint32 callback)
{
    uint32 previous = r_u32(0x80107CBC);
    FUNCTION_MARKER(0x8009C760, "SLUS_003.73");
    w_u32(0x80107CBC, callback);
    return previous;
}

uint32 CompleteCdReadDma(void)
{
    FUNCTION_MARKER(0x8009A71C, "SLUS_003.73");
    w_u32(0x800FDD58, r_u32(0x800FDD58) + r_u32(0x800FDD60) * 4);
    w_u32(0x800FDD64, r_u32(0x800FDD64) - 1);
    w_u32(0x800FDD70, r_u32(0x800FDD70) + 1);
    uint32 result = r_u32(0x800FDD64);
    if (!result)
    {
        sub_80097DD0(r_u32(0x800FDD74));
        sub_80097DE4(r_u32(0x800FDD78));
        if (r_u32(0x800FDD80) & 1)
            sub_80098214(r_u32(0x800FDD7C));
        sub_80097F34(9, 0);
        result = r_u32(0x800FDD4C);
        if (result)
            return FailSelectedCdCallback(0x8009A71C, 0x800FDD4C, result, 2, r_u32(0x800FDD84));
    }
    return result;
}
