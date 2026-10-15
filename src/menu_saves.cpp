#include "menu_saves.h"
extern "C"
{
#include "draft_first_signatures.h"
}

#include "player_save_state.h"

uint32 EnterSaveSelectionMenu(void)
{
    uint32 menu, text, index;
    FUNCTION_MARKER(0x80017760, "SLUS_003.73");
    menu = r_u32(0x800FF024);
    text = r_u32(0x800A5594);
    index = sub_800153F8(menu, text);
    menu = r_u32(0x800FF024);
    sub_80015374(menu, index & 255);
    w_u32(0x800FF080, 9);
    return 9;
}

uint32 PopulateSaveSelectionMenu(uint32 menu)
{
    uint32 slot, record = 0x800A4B3C, cursor = 0x800A493C, entry, checksum, row, channel;
    FUNCTION_MARKER(0x8001779C, "SLUS_003.73");
    for (slot = 0; slot < 8; ++slot)
    {
        if (r_u32(cursor + 512))
        {
            entry = FindLevelEntry(0x800A4B40 + slot * 136);
            checksum = CalculatePlayerSaveChecksum((const uint8 *)psx_addr(record, 136));
            if (r_u32(cursor + 512) == checksum && entry)
                sub_80015474(menu, r_u32(entry));
            else
            {
                sub_80015474(menu, r_u32(0x800A55CC));
                row = menu + r_u8(menu + 10) * 28 - 4;
                w_u8(row + 14, 255);
                w_u8(row + 15, 0);
                w_u8(row + 16, 0);
                w_u8(row + 17, 255);
                w_u8(row + 18, 0);
                w_u8(row + 19, 0);
            }
        }
        else
        {
            sub_80015474(menu, r_u32(0x800A5660));
            row = menu + r_u8(menu + 10) * 28 - 4;
            for (channel = 14; channel < 20; ++channel)
                w_u8(row + channel, 255);
        }
        record += 136;
        cursor += 136;
    }
    return 0;
}

extern "C" uint32 sub_80017760(void)
{
    FUNCTION_MARKER(0x80017760, "SLUS_003.73");
    return EnterSaveSelectionMenu();
}
extern "C" uint32 sub_8001779C(uint32 menu)
{
    FUNCTION_MARKER(0x8001779C, "SLUS_003.73");
    return PopulateSaveSelectionMenu(menu);
}
