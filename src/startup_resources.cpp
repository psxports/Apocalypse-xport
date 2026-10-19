#include <string.h>
#include <stdlib.h>
#include <new>

#include "pause_menu.h"
#include "card_save.h"
#include "player_save_state.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_tuning.h"
#include "palette_resources.h"
#include "model_projection.h"
extern "C"
{
#include "game_diagnostics.h"
#include "game_gpu_present.h"
#include "psx_gpu.h"
}
#include "startup_resources.h"
static void InitializeMenuResources();
#include "loading_screen.h"
#include "scene_lifecycle.h"
#include "game_random.h"
#include "game_pad_poll.h"
#include "movie_lifecycle.h"
#include "audio_session.h"
#include "cd_voice_event.h"
#include "model_resources.h"
#include "hud_text.h"
#include "game_panel.h"
#include "effect_lifecycle.h"
#include "camera.h"
#include "resource_io.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "menu_window.h"
#include "menu_lifecycle.h"

struct StartupDisplay
{
    uint32 clear_packet[14];
    DRAWENV draw;
    DISPENV display;
};

static StartupDisplay startup_display;

void ResetStartupDisplay()
{
    memset(&startup_display, 0, sizeof(startup_display));
}

static void ClearStartupVram(PSX_RECT *rectangle, uint8 red, uint8 green, uint8 blue)
{
    FUNCTION_MARKER(0x8008850C, "SLUS_003.73");
    uint32 *packet = startup_display.clear_packet;
    GpuRasterState state;
    sint32 width_limit = GPU_VRAM_WIDTH - 1;
    sint32 height_limit = GPU_VRAM_HEIGHT - 1;
    uint32 color = red | ((uint32)green << 8) | ((uint32)blue << 16);
    uint32 xy, size, mode;
    if (!rectangle || !gpu_register_packet_range(packet, sizeof(startup_display.clear_packet)))
        abort();
    rectangle->w = rectangle->w < 0 ? 0 : rectangle->w > width_limit ? (sint16)width_limit : rectangle->w;
    rectangle->h = rectangle->h < 0 ? 0 : rectangle->h > height_limit ? (sint16)height_limit : rectangle->h;
    xy = (uint16)rectangle->x | ((uint32)(uint16)rectangle->y << 16);
    size = (uint16)rectangle->w | ((uint32)(uint16)rectangle->h << 16);
    gpu_export_raster(&state);
    mode = 0xE1000400 | ((state.draw_mode | state.draw_flags) & 0x7FF);
    if (((rectangle->x | rectangle->w) & 63) != 0)
    {
        packet[0] = 0x08000000 | gpu_packet_link((uintptr_t)(packet + 10));
        packet[1] = 0xE3000000;
        packet[2] = 0xE4FFFFFF;
        packet[3] = 0xE5000000;
        packet[4] = 0xE6000000;
        packet[5] = mode;
        packet[6] = 0x60000000 | color;
        packet[7] = xy;
        packet[8] = size;
        packet[10] = 0x03FFFFFF;
        packet[11] = 0xE3000000 | state.area[0] | (state.area[1] << 10);
        packet[12] = 0xE4000000 | state.area[2] | (state.area[3] << 10);
        packet[13] = 0xE5000000 | ((uint32)state.offset[0] & 0x7FF) | (((uint32)state.offset[1] & 0x7FF) << 11);
    }
    else
    {
        packet[0] = 0x05FFFFFF;
        packet[1] = 0xE6000000;
        packet[2] = mode;
        packet[3] = 0x02000000 | color;
        packet[4] = xy;
        packet[5] = size;
    }
    DrawOTag(packet);
}

uint32 InitializeGameRuntime()
{
    DRAWENV *startup_draw = &startup_display.draw;
    DISPENV *startup_disp = &startup_display.display;
    PSX_RECT rectangle = {0, 0, 1024, 512};

    uint32 start_tick;
    uint32 resource;
    uint32 lives_count;
    uint32 result;
    FUNCTION_MARKER(0x800713E8, "SLUS_003.73");
    InitializeGameHeaps();
    ResetCallback();
    InitializeCardStorage();
    SetDisplayMask(0);
    SetupGamePadInput();
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
    InitializeSceneDisplay();
    VSyncCallback(SceneVBlankCallback);
    ClearStartupVram(&rectangle, 0, 0, 0);
    DrawSync(0);
    SetDisplayMask(1);
    SetAuxiliaryRandomSeed(0x9F6AD);
    ::InitializeResourceDirectory();
    InitializeModelResources();

    InitializeMenuResources();
    InitializeMovieResourceTable();
    LocateXaVoiceFile();
    InitializeModelVramRectangles();
    InitializeAudioOutput();
    ResetHudTextQueue();
    ::InitializePaletteResources();
    SetProjectionScreenParameters(0x100, 0x78, 0x100);
    ConfigureProjectionDepth(10, 0x1770, 0x800);
    PutDispEnv(&scene_render.buffers[1].disp);
    PlayMovie(3);
    if (game_pads[0].buttons[14].down == 0)
        LoadStartupImage(1, "legal.rle", 240);
    start_tick = game_clock.vblank_frame;
    SetResourceReadOption(0x7D0);
    ReloadGameConfiguration();
    InitializeAudioSession("sfx.vab");
    resource = QueueModelBankLoad("procfire", 0);
    GetModelBank(resource).protected_bank = 1;
    resource = QueueModelBankLoad("bits", 0);
    GetModelBank(resource).protected_bank = 1;
    resource = QueueModelBankLoad("items", 0);
    GetModelBank(resource).protected_bank = 1;
    items_bank = resource;
    resource = QueueModelBankLoad("bruce", 0);
    GetModelBank(resource).protected_bank = 1;
    resource = QueueModelBankLoad("plus_one", 0);
    GetModelBank(resource).protected_bank = 1;
    WaitForModelBankLoads();
    SetResourceReadOption(4);
    if (game_clock.vblank_frame - start_tick < 0x12C && (game_pads[0].buttons[4].down == 0 || game_pads[0].buttons[6].down == 0))
    {
        do
        {
            if (game_pads[0].buttons[14].down != 0)
                break;
            if (game_pads[0].buttons[3].down != 0)
                break;
            PollPadPackets();
            if (game_clock.vblank_frame - start_tick >= 0x12C)
                break;
        } while (game_pads[0].buttons[4].down == 0 || game_pads[0].buttons[6].down == 0);
    }
    lives_count = (uint32)GetGameTuning().initial_lives;
    game_progress.lives = lives_count;
    LoadPanelResource();
    InitializeHudFonts();
    InitializeEffectResources();
    LoadHudTextGlowResource();
    result = InitializeHudTextGlowCurve();
    game_session.exit_reason = 0;
    return result;
}

static void InitializeMenuResources()
{
    MenuWindow *menu;
    const char *label;
    FUNCTION_MARKER(0x8001610C, "SLUS_003.73");
    InitializeLevelLoadEstimates();
    player_saves.selected = GetDefaultPlayerSave();
    player_saves.current = GetDefaultPlayerSave();
    menu = new MenuWindow(256, 0, 0, 320, 256, 26);
    label = MenuText::no;
    MenuWindow *prev_confirm = menu_runtime.confirm;
    menu_runtime.confirm = menu;
    if (prev_confirm)
        DestroyMenuObject(prev_confirm);
    menu->AddItem(label);
    menu = menu_runtime.confirm;
    label = MenuText::yes;
    menu->AddItem(label);
    menu = menu_runtime.confirm;
    menu->spacing = 30;
}
