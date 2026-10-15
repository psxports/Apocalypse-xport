#include "game_sdk_bindings.h"
#include "debug_finish_repro.h"

#if defined(_DEBUG)
#include "menu_saves.h"
#include "menu_window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool finish_repro_loaded;
static bool finish_repro_applied;

static FILE *OpenFinishReproFile(const char *name)
{
    const char *directory = getenv("APOCALYPSE_DEBUG_FINISH_REPRO");
    char path[2048];
    if (!directory || !directory[0])
        return 0;
    if (strlen(directory) + strlen(name) + 2 >= sizeof(path))
        abort();
    sprintf(path, "%s/%s", directory, name);
    FILE *file = xport_fopen(path, "rb");
    if (!file)
    {
        GameDiagnostic("Cannot open finish-menu reproduction: %s\n", path);
        abort();
    }
    return file;
}

void DebugLoadFinishPlayerSave(uint32 record)
{
    if (finish_repro_loaded || r_u32(0x800FF818))
        return;
    FILE *file = OpenFinishReproFile("player-save.bin");
    if (!file)
        return;
    if (fread(psx_addr(record, 136), 1, 136, file) != 136)
        abort();
    fclose(file);
    finish_repro_loaded = true;
}

static uint32 ReadFinishReproWord(const uint8 *memory, uint32 address)
{
    uint32 value;
    memcpy(&value, memory + (address & 0x1FFFFF), 4);
    return value;
}

void DebugRestoreFinishMenu()
{
    if (!finish_repro_loaded || finish_repro_applied || r_u32(0x800FF818))
        return;
    FILE *file = OpenFinishReproFile("game.ram");
    uint8 *memory = (uint8 *)malloc(0x200000);
    if (!memory || fread(memory, 1, 0x200000, file) != 0x200000)
        abort();
    fclose(file);
    uint32 saved_player = ReadFinishReproWord(memory, 0x800FF5A0);
    uint32 saved_camera = ReadFinishReproWord(memory, 0x800FF904);
    uint32 player = r_u32(0x800FF5A0);
    uint32 camera = r_u32(0x800FF904);
    for (uint32 axis = 4; axis <= 12; axis += 4)
    {
        w_u32(player + axis, ReadFinishReproWord(memory, saved_player + axis));
        w_u32(camera + axis, ReadFinishReproWord(memory, saved_camera + axis));
    }
    static const uint32 globals[4] = {0x800FF308, 0x800FF30C, 0x800FF378, 0x800FF874};
    for (uint32 index = 0; index < 4; ++index)
        w_u32(globals[index], ReadFinishReproWord(memory, globals[index]));
    CompleteLevel(ReadFinishReproWord(memory, 0x800FF040));
    EnterSaveSelectionMenu();
    static const uint32 menu_globals[5] = {0x800FF034, 0x800FF044, 0x800FFB54, 0x800FFB58, 0x800FF758};
    for (uint32 index = 0; index < 5; ++index)
        w_u32(menu_globals[index], ReadFinishReproWord(memory, menu_globals[index]));
    uint32 saved_menu = ReadFinishReproWord(memory, 0x800FF024);
    MenuWindow::Attach(r_u32(0x800FF024)).ResetSelection(memory[(saved_menu + 6) & 0x1FFFFF]);
    finish_repro_applied = true;
    free(memory);
    GameDiagnostic("Finish-menu reproduction restored\n");
}
#endif
