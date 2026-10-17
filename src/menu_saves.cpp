#include "level_catalogue.h"
#include "game_string.h"
#include "menu_saves.h"
#include "menu_window.h"
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
    uint32 slot, record = 0x800A4B3C, cursor = 0x800A493C, checksum, channel;
    MenuItem *row;
    const LevelEntry *entry;
    FUNCTION_MARKER(0x8001779C, "SLUS_003.73");
    for (slot = 0; slot < 8; ++slot)
    {
        if (r_u32(cursor + 512))
        {
            entry = FindLevelEntry((const char *)psx_addr(0x800A4B40 + slot * 136, 1));
            checksum = CalculatePlayerSaveChecksum((const uint8 *)psx_addr(record, 136));
            if (r_u32(cursor + 512) == checksum && entry)
                MenuWindow::Attach(menu).AddItem(entry->title);
            else
            {
                sub_80015474(menu, r_u32(0x800A55CC));
                row = MenuWindow::Attach(menu).items + MenuWindow::Attach(menu).count - 1;
                row->color[0][0] = 255;
                row->color[0][1] = 0;
                row->color[0][2] = 0;
                row->color[1][0] = 255;
                row->color[1][1] = 0;
                row->color[1][2] = 0;
            }
        }
        else
        {
            sub_80015474(menu, r_u32(0x800A5660));
            row = MenuWindow::Attach(menu).items + MenuWindow::Attach(menu).count - 1;
            for (channel = 14; channel < 20; ++channel)
                row->color[(channel - 14) / 3][(channel - 14) % 3] = 255;
        }
        record += 136;
        cursor += 136;
    }
    return 0;
}



