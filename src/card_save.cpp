#include "game_sdk_bindings.h"
#include "card_save.h"
#include "game_string.h"
#include "scene_lifecycle.h"
extern "C"
{
#include "draft_first_signatures.h"
}
#include <stdio.h>
#include <stdlib.h>

// TODO Bind the missing BIOS B0 file services without inventing card results
static sint32 OpenCardFile(const char *name, uint32 mode)
{
    GameDiagnostic("Missing BIOS B0/32 open: name=%s mode=%u\n", name, mode);
    abort();
    return -1;
}

static sint32 ReadCardFile(sint32 descriptor, uint32 buffer, uint32 size)
{
    GameDiagnostic("Missing BIOS B0/34 read: fd=%d buffer=%08X size=%u\n", descriptor, buffer, size);
    abort();
    return -1;
}

static sint32 CloseCardFile(sint32 descriptor)
{
    GameDiagnostic("Missing BIOS B0/36 close: fd=%d\n", descriptor);
    abort();
    return -1;
}

#if !defined(LOCKSTEP_DEBUG)
static const sint32 NativeSaveBytes = 8 * 136;

static sint32 ProbeNativeSaveStorage()
{
    uint8 slots[NativeSaveBytes];
    sint32 size = xport_load(slots, NativeSaveBytes);
    return size == 0 || size == NativeSaveBytes ? 1 : -1;
}
#endif

uint32 InitializeCardSaveHeader(void)
{
    FUNCTION_MARKER(0x800102C0, "SLUS_003.73");
    w_u8(0x800A493C, 83);
    w_u8(0x800A493D, 67);
    w_u8(0x800A493E, 17);
    w_u8(0x800A493F, 1);
    for (sint32 index = 63; index >= 0; --index)
        w_u8(0x800A4940 + (uint32)index, 0);
    // Reuse the existing native string copier for the disjoint header title
    CopyGameString((const char *)psx_addr(0x800A02C4, 1), (char *)psx_addr(0x800A4940, 1));
    for (sint32 index = 27; index >= 0; --index)
        w_u8(0x800A4980 + (uint32)index, 0);
    for (uint32 index = 0; index < 32; ++index)
        w_u8(0x800A499C + index, r_u8(0x800A0224 + index));
    for (uint32 index = 0; index < 128; ++index)
    {
        uint32 value = r_u8(0x800A0244 + index);
        w_u8(0x800A49BC + index, value);
        if (r_u8(0x800A493E) >= 18)
            w_u8(0x800A4A3C + index, value);
        if (r_u8(0x800A493E) >= 19)
            w_u8(0x800A4ABC + index, value);
    }
    return 0;
}

uint32 QueryCardSaveFile(void)
{
    FUNCTION_MARKER(0x80010530, "SLUS_003.73");
#if !defined(LOCKSTEP_DEBUG)
    InitializeCardSaveHeader();
    sint32 size = xport_load(psx_addr(0x800A4B3C, NativeSaveBytes), NativeSaveBytes);
    if (size == NativeSaveBytes)
        return 0;
    for (uint32 index = 0; index < 272; ++index)
        w_u32(0x800A4B3C + index * 4, 0);
    if (size != 0)
        w_u32(0x800FEEC4, (uint32)-1);
    return 1;
#else
    InitializeCardSaveHeader();
    for (uint32 attempt = 0; attempt < 16; ++attempt)
    {
        sint32 descriptor = OpenCardFile((const char *)psx_addr(0x800A4924, 1), 1);
        if (descriptor == -1)
        {
            for (uint32 index = 0; index < 272; ++index)
                w_u32(0x800A4B3C + index * 4, 0);
        }
        else
        {
            DrainCardEvents();
            ReadCardFile(descriptor, 0x800A493C, 1728);
            CloseCardFile(descriptor);
            return 0;
        }
        WaitForGameFrames(1);
        UpdateCardStatus(0, 0);
    }
    return 1;
#endif
}

uint32 DrainCardEvents(void)
{
    FUNCTION_MARKER(0x80010938, "SLUS_003.73");
    TestEvent(r_u32(0x800FEED4));
    TestEvent(r_u32(0x800FEED8));
    TestEvent(r_u32(0x800FEEDC));
    return TestEvent(r_u32(0x800FEEE0));
}





uint32 PollCardEvents(void)
{
    FUNCTION_MARKER(0x8001081C, "SLUS_003.73");
    uint32 event = TestEvent(r_u32(0x800FEED4)) == 1 ? 1 : 0;
    if (TestEvent(r_u32(0x800FEED8)) == 1)
        event = 2;
    if (TestEvent(r_u32(0x800FEEDC)) == 1)
        event = 3;
    if (TestEvent(r_u32(0x800FEEE0)) == 1)
        event = 4;
    uint32 ticks = r_u32(0x800FEEF4);
    w_u32(0x800FEEF4, ticks + 1);
    return (sint32)ticks < 120 ? event : 2;
}

uint32 WaitForCardHardwareEvent(void)
{
    FUNCTION_MARKER(0x80010980, "SLUS_003.73");
    // Original wait has no timeout or frame-service call
    for (;;)
    {
        if (TestEvent(r_u32(0x800FEEE4)) == 1)
            return 1;
        if (TestEvent(r_u32(0x800FEEE8)) == 1)
            return 2;
        if (TestEvent(r_u32(0x800FEEEC)) == 1)
            return 3;
        if (TestEvent(r_u32(0x800FEEF0)) == 1)
            return 4;
    }
}

uint32 DrainCardHardwareEvents(void)
{
    FUNCTION_MARKER(0x800109F0, "SLUS_003.73");
    TestEvent(r_u32(0x800FEEE4));
    TestEvent(r_u32(0x800FEEE8));
    TestEvent(r_u32(0x800FEEEC));
    return TestEvent(r_u32(0x800FEEF0));
}

uint32 UpdateCardStatus(uint32 port, uint32 slot)
{
    FUNCTION_MARKER(0x80010AC8, "SLUS_003.73");
#if !defined(LOCKSTEP_DEBUG)
    sint32 status = port == 0 && slot == 0 ? ProbeNativeSaveStorage() : -1;
    w_u32(0x800FEED0, 0);
    w_u32(0x800FEEC8, status == 1 ? 1 : 0);
    w_u32(0x800FEECC, (uint32)status);
    w_u32(0x800FEEC4, (uint32)status);
    return (uint32)status;
#else
    uint32 state = r_u32(0x800FEED0);
    uint32 channel = (port << 4) + slot;
    sint32 event;
    switch (state)
    {
        case 0:
            _card_info((sint32)channel);
            w_u32(0x800FEED0, 1);
            w_u32(0x800FEEF4, 0);
            w_u32(0x800FEEC4, 0);
            break;
        case 1:
            event = (sint32)PollCardEvents();
            if (!event)
                break;
            if (event == 1)
            {
                uint32 loaded = r_u32(0x800FEEC8);
                w_u32(0x800FEECC, 1);
                w_u32(0x800FEED0, loaded == 1 ? 4 : 2);
                break;
            }
            if (event == 4)
            {
                w_u32(0x800FEECC, 2);
                DrainCardHardwareEvents();
                // Native adapter for the reviewed new-card/write BIOS wrapper
                _new_card();
                psx_bios_card_write_guest(channel, 63, 0);
                WaitForCardHardwareEvent();
                w_u32(0x800FEED0, 2);
                w_u32(0x800FEEC8, 0);
                break;
            }
            w_u32(0x800FEECC, event == 3 ? (uint32)-1 : (uint32)-3);
            w_u32(0x800FEED0, 4);
            w_u32(0x800FEEC8, 0);
            break;
        case 2:
            DrainCardEvents();
            psx_bios_card_load_guest(channel);
            w_u32(0x800FEED0, 3);
            w_u32(0x800FEEF4, 0);
            break;
        case 3:
            event = (sint32)PollCardEvents();
            if (!event)
                break;
            w_u32(0x800FEED0, 4);
            if (event == 1)
            {
                w_u32(0x800FEEC8, 1);
                break;
            }
            w_u32(0x800FEECC, event == 3 ? (uint32)-1 : event == 4 ? (uint32)-2 : (uint32)-3);
            w_u32(0x800FEEC8, 0);
            break;
        case 4:
        {
            uint32 result = r_u32(0x800FEECC);
            w_u32(0x800FEED0, 0);
            w_u32(0x800FEEC4, result);
            break;
        }
        default:
            break;
    }
    return r_u32(0x800FEEC4);
#endif
}






// TODO Bind the remaining file/directory services without fake card success
static sint32 WriteCardFile(sint32 descriptor, uint32 buffer, uint32 size)
{
    GameDiagnostic("Missing BIOS B0/35 write: fd=%d buffer=%08X size=%u\n", descriptor, buffer, size);
    abort();
    return -1;
}

static uint32 FindFirstCardFile(const char *name, uint32 entry)
{
    GameDiagnostic("Missing card directory adapter 80085EEC: name=%s entry=%08X\n", name, entry);
    abort();
    return 0;
}

static uint32 FindNextCardFile(uint32 entry)
{
    GameDiagnostic("Missing BIOS B0/43 nextfile: entry=%08X\n", entry);
    abort();
    return 0;
}

static uint32 FormatCardFilesystem(const char *name)
{
    GameDiagnostic("Missing BIOS B0/41 format: name=%s\n", name);
    abort();
    return 0;
}

uint32 WriteCardSaveFile(void)
{
    FUNCTION_MARKER(0x800103E4, "SLUS_003.73");
#if !defined(LOCKSTEP_DEBUG)
    sint32 saved = xport_save(psx_addr(0x800A4B3C, NativeSaveBytes), NativeSaveBytes);
    w_u32(0x800FEEC4, saved == 1 ? 1 : (uint32)-1);
    return saved == 1 ? 0 : 1;
#else
    sint32 retries = 200;
    w_u32(0x800FEED0, 0);
    w_u32(0x800FEEC8, 0);
    for (;;)
    {
        VSync(0);
        UpdateCardStatus(0, 0);
        uint32 loaded = r_u32(0x800FEEC8);
        --retries;
        if (loaded == 1)
            break;
        if (retries < 0)
        {
            w_u32(0x800FEECC, 0);
            w_u32(0x800FEEC4, 0);
            return 1;
        }
    }
    w_u32(0x800FEECC, 0);
    w_u32(0x800FEEC4, 0);
    InitializeCardSaveHeader();
    for (uint32 attempt = 0; attempt < 16; ++attempt)
    {
        uint32 status = r_u32(0x800FEEC4);
        if (status != 2 || r_u32(0x800FEECC) != status)
        {
            const char *name = (const char *)psx_addr(0x800A4924, 1);
            sint32 descriptor = OpenCardFile(name, 0x10200);
            if (descriptor != -1)
                CloseCardFile(descriptor);
            descriptor = OpenCardFile(name, 2);
            if (descriptor != -1)
            {
                DrainCardEvents();
                WriteCardFile(descriptor, 0x800A493C, 1728);
                CloseCardFile(descriptor);
                return 0;
            }
        }
        WaitForGameFrames(1);
        WaitForGameFrames(1);
        UpdateCardStatus(0, 0);
    }
    return 1;
#endif
}

uint32 GetFreeCardBlocks(uint32 port, uint32 slot)
{
    FUNCTION_MARKER(0x80010A38, "SLUS_003.73");
#if !defined(LOCKSTEP_DEBUG)
    // Native slots share one host file and require no emulated card directory
    return port == 0 && slot == 0 && ProbeNativeSaveStorage() == 1 ? 1 : 0;
#else
    char name[32];
    // Native C formatting adapter preserves the original signed decimal arguments
    snprintf(name, sizeof(name), (const char *)psx_addr(0x800A020C, 1), (sint32)port, (sint32)slot);
    uint32 total = 0, entry = 0x800A4FFC;
    if (FindFirstCardFile(name, entry) == entry)
    {
        do
        {
            uint32 size = r_u32(entry + 24);
            entry += 40;
            total += size;
        } while (FindNextCardFile(entry) == entry);
    }
    return 15 - (uint32)((sint32)total / 8192);
#endif
}

uint32 FormatCard(uint32 port, uint32 slot)
{
    FUNCTION_MARKER(0x80010CBC, "SLUS_003.73");
    // Native storage avoids incidental original stack spills for wider decimal IDs
    char name[32];
    snprintf(name, sizeof(name), (const char *)psx_addr(0x800A0218, 1), (sint32)port, (sint32)slot);
    DrainCardHardwareEvents();
    uint32 formatted = FormatCardFilesystem(name);
    uint32 event = WaitForCardHardwareEvent();
    if (event != 1)
        event = event == 3 ? (uint32)-1 : (uint32)-3;
    if (!formatted && event == 1)
        return (uint32)-3;
    return event;
}




