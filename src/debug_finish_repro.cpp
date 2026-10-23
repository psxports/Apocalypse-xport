#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_recording.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "debug_finish_repro.h"

#if defined(_DEBUG)

    #include "menu_window.h"
    #include "pause_menu.h"
    #include "player_save_state.h"
    #include "game_panel.h"

static bool finish_repro_loaded;
static bool finish_repro_applied;

void ResetDebugFinishRepro()
{
    finish_repro_loaded = false;
    finish_repro_applied = false;
}

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
        GameDiagnostic(("Cannot open finish-menu reproduction: %s\n", path));
        abort();
    }
    return file;
}

void DebugLoadFinishPlayerSave(PlayerSaveRecord &rec)
{
    if (finish_repro_loaded || input_recording.GetMode() != InputRecording::Live)
        return;
    FILE *file = OpenFinishReproFile("player-save.bin");
    if (!file)
        return;
    uint8 data[PlayerSaveSize];
    if (xport_fread(data, 1, PlayerSaveSize, file) != PlayerSaveSize)
        abort();
    fclose(file);
    DecodePlayerSave(rec, data, sizeof(data));
    finish_repro_loaded = true;
}

struct FinishReproContext
{
    Vector3 player_pos;
    Vector3 camera_pos;
    uint32 enemy_total;
    uint32 enemy_kills;
    uint32 area_kind;
    uint32 lives;
    uint32 completion_val;
    uint32 visible;
    uint32 blink_ticks;
    uint32 target_percent;
    uint32 disp_percent;
    uint32 lives_timer;
    uint32 selection;
};

struct FinishReproFile
{
    char magic[4];
    uint32 version;
    FinishReproContext context;
};

typedef char FinishReproFileSize[sizeof(FinishReproFile) == 76 ? 1 : -1];

static FinishReproContext ReadFinishReproContext()
{
    FinishReproFile data;
    uint8 extra;
    FILE *file = OpenFinishReproFile("finish-context.bin");
    if (!file)
        abort();
    bool valid = xport_fread(&data, 1, sizeof(data), file) == sizeof(data);
    valid = valid && xport_fread(&extra, 1, 1, file) == 0 && !ferror(file);
    fclose(file);
    if (!valid || memcmp(data.magic, "AFIN", 4) || data.version != 1 || data.context.selection > 255)
    {
        GameDiagnostic(("Invalid native finish test context\n"));
        abort();
    }
    return data.context;
}

void DebugRestoreFinishMenu()
{
    if (!finish_repro_loaded || finish_repro_applied || input_recording.GetMode() != InputRecording::Live)
        return;
    const char *phase = getenv("APOCALYPSE_DEBUG_FINISH_PHASE");
    bool results = phase && !strcmp(phase, "results");
    if (phase && !results && strcmp(phase, "save"))
    {
        GameDiagnostic(("Invalid finish reproduction phase\n"));
        abort();
    }
    FinishReproContext context = ReadFinishReproContext();
    ModelInstance *player = visual_lists.players;
    ModelInstance *camera = visual_lists.cameras;
    if (!player || !camera)
    {
        GameDiagnostic(("Missing native finish test player/camera\n"));
        abort();
    }
    player->pos = context.player_pos;
    camera->pos = context.camera_pos;
    game_progress.level.enemy_total = context.enemy_total;
    game_progress.level.enemy_kills = context.enemy_kills;
    level_environment.area_kind = context.area_kind;
    game_progress.lives = context.lives;
    CompleteLevel(context.completion_val);
    if (!results)
    {
        MenuWindow::EnterSaveSelection();
        panel_text.visible = context.visible;
        game_progress.completion.blink_ticks = context.blink_ticks;
        game_progress.completion.target_percent = context.target_percent;
        game_progress.completion.disp_percent = context.disp_percent;
        panel_text.lives_timer = context.lives_timer;
        if (!menu_runtime.confirm)
        {
            GameDiagnostic(("Missing native finish test confirmation menu\n"));
            abort();
        }
        menu_runtime.confirm->ResetSelection(context.selection);
    }
    finish_repro_applied = true;
    GameDiagnostic(("Finish-menu reproduction restored\n"));
}
#endif
