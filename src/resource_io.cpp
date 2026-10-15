#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "game_startup_adapters.h"
}
#include "resource_io.h"
#include "game_data_io.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

void SetResourceReadOption(uint32 option)
{
    FUNCTION_MARKER(0x8006AED8, "SLUS_003.73");
    // TODO Recover the option meanings from all resource-reader branches
    w_u32(0x800FF70C, option);
}

uint32 InitializeResourceDirectory()
{
    uint32 result;
#if defined(LOCKSTEP_DEBUG)
    uint8 file[24];
    uint32 sectors;
#endif
    FUNCTION_MARKER(0x8006AEE4, "SLUS_003.73");
    w_u32(0x800FF700, 0);
    sub_80097D14(0);
#if !defined(LOCKSTEP_DEBUG)
    if (!sub_80097B4C())
        abort();
    result = LoadDataResourceDirectory();
    w_u32(0x800FFC38, result);
    w_u32(0x800FF710, 0);
    return result;
#else
    while (!sub_80097B4C())
    {
        GameDiagnostic("%s", (const char *)psx_addr(0x800A3A60, 1));
        for (uint32 frame = 0; frame < 100; ++frame)
            VSync(0);
    }
    while (!xport_draft_host_sub_80099B3C_p1(file, 0x800A3A70))
    {
        while (!sub_80097B4C())
            for (uint32 frame = 0; frame < 100; ++frame)
                VSync(0);
        for (uint32 frame = 0; frame < 100; ++frame)
            VSync(0);
    }
    sectors = (xport_load_le32(file + 4) + 2047) >> 11;
    xport_draft_host_sub_80097DF8_p2(2, file, 0);
    sub_8009AA6C(sectors, 0x801007A8, 128);
    while ((sint32)sub_8009AB6C(1, 0) > 0)
        VSync(0);
    while (!xport_draft_host_sub_80099B3C_p1(file, 0x800A3A7C))
        for (uint32 frame = 0; frame < 100; ++frame)
            VSync(0);
    result = xport_draft_host_sub_8009835C_p1(file);
    w_u32(0x800FFC38, result);
    w_u32(0x800FF710, 0);
    return result;
#endif
}

extern "C" void sub_8006AED8(uint32 option)
{
    FUNCTION_MARKER(0x8006AED8, "SLUS_003.73");
    SetResourceReadOption(option);
}
extern "C" uint32 sub_8006AEE4(void)
{
    FUNCTION_MARKER(0x8006AEE4, "SLUS_003.73");
    return InitializeResourceDirectory();
}

static uint8 ReadResourceNameByte(const char *name, uint32 mapped, uint32 index)
{
    if (mapped)
        return r_u8(mapped + index);
    return (uint8)name[index];
}

static uint8 FoldResourceNameByte(uint8 value)
{
    if (value >= 'A' && value <= 'Z')
        value = (uint8)(value + 32);
    return value;
}

uint32 LookupResource(const char *name)
{
    uint32 mapped, table = 0x801007A8, destination, input, copy, left, right, mismatch;
    uint32 byte_count, rounded, blocks, result;
    uint8 character;
    sint16 rectangle[4];
    FUNCTION_MARKER(0x8006B04C, "SLUS_003.73");
    mapped = GuestMemoryAddress(name);
    w_u32(0x800FF6FC, 0);
    copy = 0;
    destination = 0x800FF714;
    // TODO Original resource-name copy has no destination length bound
    do
    {
        character = ReadResourceNameByte(name, mapped, copy++);
        w_u8(destination++, character);
    } while (character);
    input = 0;
    while (ReadResourceNameByte(name, mapped, input))
    {
        mismatch = 0;
        do
        {
            left = ReadResourceNameByte(name, mapped, input);
            ++input;
            right = r_u8(table);
            ++table;
            left = FoldResourceNameByte((uint8)left);
            right = FoldResourceNameByte((uint8)right);
            if (left != right)
            {
                mismatch = 1;
                break;
            }
        } while (ReadResourceNameByte(name, mapped, input));
        if (mismatch)
        {
            input = 0;
            do
            {
                character = r_u8(table);
                ++table;
            } while (character);
        }
        else if (!r_u8(table))
            continue;
        else
        {
            input = 0;
            ++table;
            do
            {
                character = r_u8(table);
                ++table;
            } while (character);
        }
        table = ((table + 3) & 0xFFFFFFFC) + 8;
        if (r_u8(table) == 255)
        {
            left = r_u32(0x800FF718);
            right = r_u32(0x800FF71C);
            xport_store_le32((uint8 *)rectangle, left);
            xport_store_le32((uint8 *)rectangle + 4, right);
            xport_draft_host_sub_8008847C_p1(rectangle, 0, 0, 255);
            rectangle[1] = (sint16)((uint16)rectangle[1] + 256);
            xport_draft_host_sub_8008847C_p1(rectangle, 0, 0, 255);
        }
    }
    table = (table + 4) & 0xFFFFFFFC;
    w_u32(0x800FFC48, r_u32(table));
    byte_count = r_u32(table + 4);
    rounded = byte_count + 2047;
    if ((sint32)rounded < 0)
        rounded = byte_count + 4094;
    blocks = (uint32)((sint32)rounded >> 11);
    result = blocks << 11;
    w_u32(0x800FFC44, blocks);
    w_u32(0x800FFC2C, result);
    w_u32(0x800FF708, 1);
    return result;
}

extern "C" uint32 apocalypse_resource_lookup(const char *name) { return LookupResource(name); }
extern "C" uint32 sub_8006B04C(uint32 name)
{
    FUNCTION_MARKER(0x8006B04C, "SLUS_003.73");
    return LookupResource((const char *)psx_addr(name, 1));
}

uint32 BeginResourceRead(uint32 destination)
{
    FUNCTION_MARKER(0x8006B234, "SLUS_003.73");
    return game_startup_read_begin(0, destination);
}

uint32 ProcessResourceRead()
{
    FUNCTION_MARKER(0x8006B2A8, "SLUS_003.73");
    // Preserve the current native buffer transfer adapter contract
    return game_startup_read_step();
}

uint32 WaitForResourceRead()
{
    uint32 busy;
    FUNCTION_MARKER(0x8006B44C, "SLUS_003.73");
    busy = r_u32(0x800FF700);
    while (busy)
    {
        ProcessResourceRead();
        busy = r_u32(0x800FF700);
    }
    return busy;
}

extern "C" sint32 sub_8006B234(uint32 destination)
{
    FUNCTION_MARKER(0x8006B234, "SLUS_003.73");
    return (sint32)BeginResourceRead(destination);
}
extern "C" uint32 sub_8006B2A8(void)
{
    FUNCTION_MARKER(0x8006B2A8, "SLUS_003.73");
    return ProcessResourceRead();
}
extern "C" sint32 sub_8006B44C(void)
{
    FUNCTION_MARKER(0x8006B44C, "SLUS_003.73");
    return (sint32)WaitForResourceRead();
}
