extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "player_save_state.h"
#include "game_settings.h"
#include "player.h"
#include "native_object_storage.h"
#include <string.h>

static uint32 ReadPlayerSaveWord(const uint8 *record, uint32 mapped, uint32 offset)
{
    uint32 value;
    if (mapped)
        return r_u32(mapped + offset);
    memcpy(&value, record + offset, 4);
    return value;
}

static uint32 ReadPlayerSaveByte(const uint8 *record, uint32 mapped, uint32 offset)
{
    return mapped ? r_u8(mapped + offset) : record[offset];
}

uint32 FindLevelEntry(uint32 name)
{
    uint32 entry = 0x800A5254, candidate;
    FUNCTION_MARKER(0x80018948, "SLUS_003.73");
    candidate = r_u32(entry + 4);
    if (!r_u8(candidate))
        return 0;
    for (;;)
    {
        candidate = r_u32(entry + 4);
        if (sub_80067724(candidate, name))
            return entry;
        entry += 24;
        if ((sint32)entry >= (sint32)0x800A562C)
            return 0;
        candidate = r_u32(entry + 4);
        if (!r_u8(candidate))
            return 0;
    }
}

uint32 CaptureCurrentPlayerSave()
{
    uint32 source, destination, character, position[4], owner, object, index, result;
    uint32 owner_value, global_value, mask, ammo;
    FUNCTION_MARKER(0x800189EC, "SLUS_003.73");
    source = r_u32(0x800FF83C);
    destination = 0x800A53D8;
    // TODO Retain original unchecked name copies pending a defined overflow contract
    character = r_u8(source);
    while (character)
    {
        ++source;
        w_u8(destination, character);
        character = r_u8(source);
        ++destination;
    }
    w_u8(destination, 0);
    source = xport_draft_host_sub_8006613C_p1(position, r_u32(0x800FF620)) + 6;
    destination = 0x800A53E1;
    character = r_u8(source);
    while (character)
    {
        character = r_u8(source);
        ++source;
        w_u8(destination, character);
        character = r_u8(source);
        ++destination;
    }
    w_u8(destination, 0);
    owner = r_u32(0x800FF5A0);
    if (owner)
    {
        owner_value = (uint32)(sint32)(sint16)r_u16(owner + 218);
        w_u32(0x800A5438, owner_value);
        owner_value = r_u32(owner + 532);
        global_value = r_u32(0x800FF874);
        w_u8(0x800A5416, 0);
        w_u32(0x800A5440, global_value);
        w_u32(0x800A543C, owner_value);
        for (index = 0; index < 8; ++index)
        {
            object = r_u32(owner + 612 + index * 4);
            if (object)
            {
                mask = r_u8(0x800A5416);
                w_u8(0x800A5416, mask | (1 << index));
                object = r_u32(owner + 612 + index * 4);
                ammo = r_u32(object + 56);
                w_u32(0x800A5418 + index * 4, ammo);
            }
        }
        owner_value = r_u8(owner + 644);
        w_u8(0x800A5417, owner_value);
    }
    SaveGameSettings((uint8 *)psx_addr(0x800A5444, 24));
    result = CalculatePlayerSaveChecksum((const uint8 *)psx_addr(0x800A53D4, 136));
    w_u32(0x800A53D4, result);
    return result;
}

uint32 CalculatePlayerSaveChecksum(const uint8 *record)
{
    uint32 mapped, index, result = 0;
    FUNCTION_MARKER(0x80018B44, "SLUS_003.73");
    mapped = GuestMemoryAddress(record);
    for (index = 1; index < 34; ++index)
        result ^= ReadPlayerSaveWord(record, mapped, index * 4);
    return result | 1;
}

uint32 RestorePlayerInventory(uint8 *record)
{
    uint32 mapped, health, owner, index, mask, weapon, ammo, selected;
    FUNCTION_MARKER(0x80018B70, "SLUS_003.73");
    mapped = GuestMemoryAddress(record);
    health = ReadPlayerSaveWord(record, mapped, 104);
    owner = r_u32(0x800FF5A0);
    if ((sint32)health < 3)
    {
        health = 3;
        if (mapped)
            w_u32(mapped + 104, health);
        else
            memcpy(record + 104, &health, 4);
    }
    health = ReadPlayerSaveWord(record, mapped, 104);
    w_u32(owner + 532, health);
    for (index = 1; index < 8; ++index)
    {
        mask = ReadPlayerSaveByte(record, mapped, 66);
        if ((mask >> index) & 1)
        {
            Player::Attach(owner).ReplaceWeapon(index);
            weapon = r_u32(owner + 612 + index * 4);
            ammo = ReadPlayerSaveWord(record, mapped, 68 + index * 4);
            w_u32(weapon + 56, ammo);
        }
    }
    selected = ReadPlayerSaveByte(record, mapped, 67);
    w_u32(owner + 644, selected);
    return selected;
}

extern "C" uint32 sub_80018B44(uint32 record)
{
    FUNCTION_MARKER(0x80018B44, "SLUS_003.73");
    return CalculatePlayerSaveChecksum((const uint8 *)psx_addr(record, 136));
}

extern "C" uint32 sub_80018B70(uint32 record)
{
    FUNCTION_MARKER(0x80018B70, "SLUS_003.73");
    return RestorePlayerInventory((uint8 *)psx_addr(record, 136));
}

extern "C" uint32 sub_80018948(uint32 name)
{
    FUNCTION_MARKER(0x80018948, "SLUS_003.73");
    return FindLevelEntry(name);
}
extern "C" uint32 sub_800189EC(void)
{
    FUNCTION_MARKER(0x800189EC, "SLUS_003.73");
    return CaptureCurrentPlayerSave();
}
