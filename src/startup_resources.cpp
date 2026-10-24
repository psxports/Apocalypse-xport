#include <string.h>
#include <stdlib.h>
#include <new>

#include "pause_menu.h"
#include "card_save.h"
#include "player_save_state.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_tuning.h"
#include "palette_resource.h"
#include "model_projection.h"

extern "C"
{
#include "game_diagnostics.h"
#include "game_gpu_present.h"
}
#include "startup_resources.h"
#include "loading_screen.h"
#include "scene_lifecycle.h"
#include "global.h"
#include "game_pad_input.h"
#include "movie_lifecycle.h"
#include "audio_session.h"
#include "cd_voice_event.h"
#include "model_bank.h"
#include "hud_text.h"
#include "game_panel.h"
#include "effect_lifecycle.h"
#include "camera.h"
#include "resource_io.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "menu_window.h"
#include "menu_lifecycle.h"
#include "effect_object.h"

struct StartupDisplay
{
    DRAWENV draw;
    DISPENV disp;
};

static StartupDisplay startup_display;

void ResetStartupDisp()
{
    memset(&startup_display, 0, sizeof(startup_display));
}

static void InitializeMenuResources()
{
    MenuWindow *menu;
    FUNCTION_MARKER(0x8001610C, "SLUS_003.73");
    InitializeLevelLoadEstimates();
    player_saves.sel = GetDefaultPlayerSave();
    player_saves.cur = GetDefaultPlayerSave();
    menu = new MenuWindow(256, 0, 0, 320, 256, 26);
    MenuWindow *prev_confirm = menu_runtime.confirm;
    menu_runtime.confirm = menu;
    if (prev_confirm)
        DestroyMenuObject(prev_confirm);
    menu->AddItem(MenuText::no);
    menu_runtime.confirm->AddItem(MenuText::yes);
    menu_runtime.confirm->spacing = 30;
}

uint32 InitializeGameRuntime()
{
    DRAWENV *startup_draw = &startup_display.draw;
    DISPENV *startup_disp = &startup_display.disp;
    PSX_RECT rect = {0, 0, 1024, 512};

#if !defined(AP_WEB)
    uint32 start_tick;
#endif
    uint32 resource;
    uint32 result;
    FUNCTION_MARKER(0x800713E8, "SLUS_003.73");
    InitializeGameHeaps();
    ResetCallback();
    SetDispMask(0);
    GamePadInput::Setup();
    InitGeom();
    ResetGameGraphics(0);
    SetGraphDebug(0);
    ResetGameGraphics(0);
    startup_draw->ofs[0] = 0;
    startup_draw->ofs[1] = 0;
    memset(&startup_draw->tw, 0, sizeof(startup_draw->tw));
    startup_draw->tpage = 0;
    startup_draw->dtd = 0;
    startup_draw->dfe = 0;
    startup_draw->isbg = 0;
    PutDrawEnv(startup_draw);
    startup_disp->disp.x = 0;
    startup_disp->disp.y = 0;
    startup_disp->disp.w = 512;
    startup_disp->disp.h = 240;
    memset(&startup_disp->screen, 0, sizeof(startup_disp->screen));
    if (GetVideoMode() == 1)
    {
        startup_disp->screen.y = 24;
        startup_disp->pad0 = 1;
    }
    startup_disp->isinter = 0;
    startup_disp->isrgb24 = 0;
    PutDispEnv(startup_disp);
    InitGeom();
    SetFarColor(0, 0, 0);
    SetGeomOffset(0, 0);
    startup_draw->clip.x = 0;
    startup_draw->clip.y = 0;
    startup_draw->clip.w = 512;
    startup_draw->clip.h = 240;
    PutDrawEnv(startup_draw);
    SetGeomOffset(0, 0);
    PutDrawEnv(startup_draw);
    SetGeomOffset(0, 240);
    InitializeSceneDisp();
    VSyncCallback(SceneVBlankCallback);
    ClearImage2(&rect, 0, 0, 0);
    DrawSync(0);
    SetDispMask(1);
    SetAuxiliaryRandomSeed(0x9F6AD);
    ::InitializeResourceDirectory();
    ModelBank::Initialize();

    InitializeMenuResources();
    InitializeMovieResourceTable();
    LocateXaVoiceFile();
    InitializeModelVramRects();
    InitializeAudioOutput();
    ResetHudTextQueue();
    PaletteResource::Initialize();
    SetProjScrParameters(0x100, 0x78, 0x100);
    ConfigureProjDepth(10, 0x1770, 0x800);
    PutDispEnv(&scene_render.buffers[1].disp);
#if !defined(AP_WEB)
    PlayMovie(3);
    if (game_pads[0].buttons[14].down == 0)
        LoadStartupImage(true, "legal.rle", 240);
    start_tick = game_clock.vblank_frame;
#endif
    SetResourceReadOption(0x7D0);
    ReloadGameCfg();
    InitializeAudioSession("sfx.vab");
    resource = ModelBank::QueueLoad("procfire", false);
    ModelBank::Get(resource).protected_bank = true;
    resource = ModelBank::QueueLoad("bits", false);
    ModelBank::Get(resource).protected_bank = true;
    resource = ModelBank::QueueLoad("items", false);
    ModelBank::Get(resource).protected_bank = true;
    items_bank = resource;
    resource = ModelBank::QueueLoad("bruce", false);
    ModelBank::Get(resource).protected_bank = true;
    resource = ModelBank::QueueLoad("plus_one", false);
    ModelBank::Get(resource).protected_bank = true;
    ModelBank::WaitForLoads();
    SetResourceReadOption(4);
#if !defined(AP_WEB)
    if (game_clock.vblank_frame - start_tick < 0x12C && (game_pads[0].buttons[4].down == 0 || game_pads[0].buttons[6].down == 0))
    {
        do
        {
            if (game_pads[0].buttons[14].down != 0)
                break;
            if (game_pads[0].buttons[3].down != 0)
                break;
            GamePadInput::Poll();
            if (game_clock.vblank_frame - start_tick >= 0x12C)
                break;
        } while (game_pads[0].buttons[4].down == 0 || game_pads[0].buttons[6].down == 0);
    }
#endif
    game_progress.lives = GetGameTuning().initial_lives;
    LoadPanelResource();
    InitializeHudFonts();
    EffectObject::InitializeResources();
    LoadHudTextGlowResource();
    result = InitializeHudTextGlowCurve();
    game_session.exit_reason = 0;
    return result;
}
