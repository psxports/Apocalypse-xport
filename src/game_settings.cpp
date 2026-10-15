extern "C"
{
#include "draft_first_signatures.h"
}
#include "game_settings.h"
#include "native_object_storage.h"
#include <string.h>

static void StoreSettingsWord(uint8 *record, uint32 mapped, uint32 offset, uint32 value)
{
    if (mapped)
        w_u32(mapped + offset, value);
    else
        memcpy(record + offset, &value, 4);
}

static void StoreSettingsByte(uint8 *record, uint32 mapped, uint32 offset, uint8 value)
{
    if (mapped)
        w_u8(mapped + offset, value);
    else
        record[offset] = value;
}

static uint32 ReadSettingsWord(const uint8 *record, uint32 mapped, uint32 offset)
{
    uint32 value;
    if (mapped)
        return r_u32(mapped + offset);
    memcpy(&value, record + offset, 4);
    return value;
}

static uint16 ReadSettingsHalf(const uint8 *record, uint32 mapped, uint32 offset)
{
    uint16 value;
    if (mapped)
        return r_u16(mapped + offset);
    memcpy(&value, record + offset, 2);
    return value;
}

static uint8 ReadSettingsByte(const uint8 *record, uint32 mapped, uint32 offset)
{
    return mapped ? r_u8(mapped + offset) : record[offset];
}

static const uint32 settings_byte_globals[8] =
{
    0x800EC47A, 0x800EC474, 0x800EC476, 0x800EC478,
    0x800EC49A, 0x800EC494, 0x800EC496, 0x800EC498
};

uint32 SaveGameSettings(uint8 *record)
{
    uint32 mapped, value, index;
    FUNCTION_MARKER(0x80015F7C, "SLUS_003.73");
    mapped = GuestMemoryAddress(record);
    value = r_u32(0x800FF384);
    StoreSettingsWord(record, mapped, 0, value);
    value = (uint32)(sint32)(sint16)r_u16(0x800ECC7A);
    StoreSettingsWord(record, mapped, 4, value);
    value = (uint32)(sint32)(sint16)r_u16(0x800ECC78);
    StoreSettingsWord(record, mapped, 8, value);
    value = (uint32)(sint32)(sint16)r_u16(0x800ECC7C);
    StoreSettingsWord(record, mapped, 12, value);
    for (index = 0; index < 8; ++index)
    {
        value = r_u8(settings_byte_globals[index]);
        StoreSettingsByte(record, mapped, 16 + index, (uint8)value);
    }
    return value;
}

uint32 RestoreGameSettings(const uint8 *record)
{
    uint32 mapped, flags, value, height, x, y, width, index;
    uint32 clip_x, clip_y, clip_width, clip_height;
    FUNCTION_MARKER(0x80016014, "SLUS_003.73");
    mapped = GuestMemoryAddress(record);
    flags = ReadSettingsWord(record, mapped, 0);
    value = ReadSettingsHalf(record, mapped, 4);
    w_u16(0x800ECC7A, value);
    value = ReadSettingsHalf(record, mapped, 8);
    w_u16(0x800ECC78, value);
    value = ReadSettingsHalf(record, mapped, 12);
    w_u32(0x800FF384, flags);
    w_u16(0x800ECC7C, value);
    height = ReadSettingsByte(record, mapped, 16);
    w_u16(0x800EC47A, height);
    x = ReadSettingsByte(record, mapped, 17);
    w_u16(0x800EC474, x);
    y = ReadSettingsByte(record, mapped, 18);
    w_u16(0x800EC476, y);
    width = ReadSettingsByte(record, mapped, 19);
    w_u16(0x800EC478, width);
    for (index = 4; index < 8; ++index)
    {
        value = ReadSettingsByte(record, mapped, 16 + index);
        w_u16(settings_byte_globals[index], value);
    }
    sub_8006FDFC(0x800EC0F8, x, y, width, height);
    clip_x = (uint32)(sint32)(sint16)r_u16(0x800EC494);
    clip_y = (uint32)(sint32)(sint16)r_u16(0x800EC496);
    clip_width = (uint32)(sint32)(sint16)r_u16(0x800EC498);
    clip_height = (uint32)(sint32)(sint16)r_u16(0x800EC49A);
    return sub_8006FE14(0x800EC0F8, 3, 2, 1, 0, clip_x, clip_y, clip_width, clip_height);
}

extern "C" uint32 sub_80015F7C(uint32 record)
{
    FUNCTION_MARKER(0x80015F7C, "SLUS_003.73");
    return SaveGameSettings((uint8 *)psx_addr(record, 24));
}
extern "C" uint32 sub_80016014(uint32 record)
{
    FUNCTION_MARKER(0x80016014, "SLUS_003.73");
    return RestoreGameSettings((const uint8 *)psx_addr(record, 24));
}
