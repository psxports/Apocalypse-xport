#ifndef GAME_MENU_HELPERS_H
#define GAME_MENU_HELPERS_H
#include "psx.h"
uint32 sub_800151FC(void);
uint32 sub_800154E0(uint32 object, uint32 label);
uint32 sub_80011860(uint32 button, uint32 selected, uint32 other1, uint32 other2, uint32 other3);
uint32 sub_80067808(uint32 source, uint32 destination);
void apocalypse_menu_destroy(uint32 object);
#endif
