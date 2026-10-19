#include "pause_menu.h"
#include "card_save.h"
#include "level_catalogue.h"
#include "game_string.h"
#include "menu_saves.h"
#include "menu_window.h"
#include "game_diagnostics.h"

#include "player_save_state.h"

uint32 EnterSaveSelectionMenu(void)
{
    MenuWindow *menu;
    uint32 index;
    const char *text;
    FUNCTION_MARKER(0x80017760, "SLUS_003.73");
    menu = menu_runtime.confirm;
    text = MenuText::yes;
    index = menu->FindItem(text);
    menu = menu_runtime.confirm;
    menu->ResetSelection(index & 255);
    menu_runtime.screen = MENU_SCREEN_SAVE_CONFIRM;
    return MENU_SCREEN_SAVE_CONFIRM;
}

uint32 PopulateSaveSelectionMenu(MenuWindow *menu)
{
    uint32 slot, checksum, channel;
    MenuItem *row;
    const LevelEntry *entry;
    FUNCTION_MARKER(0x8001779C, "SLUS_003.73");
    for (slot = 0; slot < CardSaveSlotCount; ++slot)
    {
        const PlayerSaveRecord *record = card_save_image.slots + slot;
        if (record->checksum)
        {
            entry = FindLevelEntry(record->progress.level);
            checksum = CalculatePlayerSaveChecksum(*record);
            if (record->checksum == checksum && entry)
                menu->AddItem(entry->title);
            else
            {
                menu->AddItem(MenuText::read_error);
                row = menu->items + menu->count - 1;
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
            menu->AddItem(MenuText::empty_slot);
            row = menu->items + menu->count - 1;
            for (channel = 14; channel < 20; ++channel)
                row->color[(channel - 14) / 3][(channel - 14) % 3] = 255;
        }
    }
    return 0;
}
