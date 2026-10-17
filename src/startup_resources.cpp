#include "palette_resources.h"
#include "model_projection.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "game_gpu_present.h"
uint32 apocalypse_reset_callbacks(void);
void apocalypse_gs_init_graph(uint32 width, uint32 height, uint32 attributes, uint32 dither, uint32 rgb24);
void apocalypse_gs_def_disp_buff(uint32 x0, uint32 y0, uint32 x1, uint32 y1);
sint32 apocalypse_clear_image2(PSX_RECT *rectangle, uint8 red, uint8 green, uint8 blue);
uint32 _bu_init(void);
uint32 sub_800882F8(uint32 mode);
}
#include "startup_resources.h"
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
#include "native_object_storage.h"
#include "menu_window.h"
#include <new>

uint32 InitializeGameRuntime()
{
    uint32 heap_begin;
    uint32 heap_split;
    uint32 first_word;
    uint32 second_word;

    union
    {
        uint32 words[2];
        PSX_RECT value;
    } rectangle;

    uint32 start_tick;
    uint32 resource;
    uint32 first_count;
    uint32 second_count;
    uint32 result;
    FUNCTION_MARKER(0x800713E8, "SLUS_003.73");
    w_u32(0x800FFAC0, 0x800EE750);
    w_u32(0x800FFAC4, 0x800F0690);
    heap_begin = r_u32(0x80108740);
    w_u32(0x800E6C14, 0x801FE000);
    w_u32(0x800E6C08, heap_begin);
    heap_split = heap_begin + 0x43800;
    w_u32(0x800E6C0C, heap_split);
    w_u32(0x800E6C10, heap_split);
    InitializeGameHeaps();
    apocalypse_reset_callbacks();
    InitializeMemoryCardEvents(1);
    apocalypse_set_disp_mask(0);
    SetupGamePadInput();
    InitGeom();
    apocalypse_reset_graph(0);
    SetGraphDebug(0);
    apocalypse_gs_init_graph(0x200, 0xF0, 0, 0, 0);
    apocalypse_gs_def_disp_buff(0, 0, 0, 0xF0);
    InitializeSceneDisplay();
    VSyncCallbackPSX(0x80066458);
    first_word = r_u32(0x800FF84C);
    second_word = r_u32(0x800FF850);
    rectangle.words[0] = first_word;
    rectangle.words[1] = second_word;
    apocalypse_clear_image2(&rectangle.value, 0, 0, 0);
    DrawSync(0);
    apocalypse_set_disp_mask(1);
    SetAuxiliaryRandomSeed(0x9F6AD);
    ::InitializeResourceDirectory();
    InitializeModelResources();
    w_u32(0x800FEF00, 20);
    InitializeMenuResources();
    InitializeMovieResourceTable();
    LocateXaVoiceFile();
    InitializeModelVramRectangles();
    InitializeAudioOutput();
    ResetHudTextQueue();
    ::InitializePaletteResources();
    SetProjectionScreenParameters(0x100, 0x78, 0x100);
    ConfigureProjectionDepth(10, 0x1770, 0x800);
    PutDispEnv((DISPENV *)psx_addr(0x800C636C, sizeof(DISPENV)));
    PlayMovie(3);
    if (r_u8(0x800EC1D8) == 0)
        LoadStartupImage(1, "legal.rle", 240);
    start_tick = r_u32(0x800FF64C);
    sub_8006AED8(0x7D0);
    ::ReloadGamePadConfiguration();
    InitializeAudioSession("sfx.vab");
    resource = QueueModelBankLoad("procfire", 0);
    GetModelBank(resource).protected_bank = 1;
    resource = QueueModelBankLoad("bits", 0);
    GetModelBank(resource).protected_bank = 1;
    resource = QueueModelBankLoad("items", 0);
    GetModelBank(resource).protected_bank = 1;
    w_u8(0x800FF645, resource);
    resource = QueueModelBankLoad("bruce", 0);
    GetModelBank(resource).protected_bank = 1;
    resource = QueueModelBankLoad("plus_one", 0);
    GetModelBank(resource).protected_bank = 1;
    WaitForModelBankLoads();
    sub_8006AED8(4);
    if (r_u32(0x800FF64C) - start_tick < 0x12C && (r_u8(0x800EC138) == 0 || r_u8(0x800EC158) == 0))
    {
        do
        {
            if (r_u8(0x800EC1D8) != 0)
                break;
            if (r_u8(0x800EC128) != 0)
                break;
            sub_80070748();
            if (r_u32(0x800FF64C) - start_tick >= 0x12C)
                break;
        } while (r_u8(0x800EC138) == 0 || r_u8(0x800EC158) == 0);
    }
    first_count = (uint32)(sint32)(sint16)r_u16(0x800EC522);
    second_count = (uint32)(sint32)(sint16)r_u16(0x800EC524);
    w_u32(0x800FF874, first_count);
    w_u32(0x800FF878, second_count);
    LoadPanelResource();
    InitializeHudFonts();
    InitializeEffectResources();
    LoadHudTextGlowResource();
    result = InitializeHudTextGlowCurve();
    w_u32(0x800FF2EC, 0);
    return result;
}

extern "C" uint32 sub_800713E8(void)
{
    FUNCTION_MARKER(0x800713E8, "SLUS_003.73");
    return InitializeGameRuntime();
}

uint32 InitializeMemoryCardEvents(uint32 requested_mode)
{
    uint32 result;
    FUNCTION_MARKER(0x80010610, "SLUS_003.73");
    sub_8009FA6C(requested_mode);
    sub_8009FAD8();
    _bu_init();
    sub_80085E6C();
    result = OpenEventGuest(0xF4000001, 0x4, 0x2000, 0);
    w_u32(0x800FEED4, result);
    result = OpenEventGuest(0xF4000001, 0x8000, 0x2000, 0);
    w_u32(0x800FEED8, result);
    result = OpenEventGuest(0xF4000001, 0x100, 0x2000, 0);
    w_u32(0x800FEEDC, result);
    result = OpenEventGuest(0xF4000001, 0x2000, 0x2000, 0);
    w_u32(0x800FEEE0, result);
    result = OpenEventGuest(0xF0000011, 0x4, 0x2000, 0);
    w_u32(0x800FEEE4, result);
    result = OpenEventGuest(0xF0000011, 0x8000, 0x2000, 0);
    w_u32(0x800FEEE8, result);
    result = OpenEventGuest(0xF0000011, 0x100, 0x2000, 0);
    w_u32(0x800FEEEC, result);
    result = OpenEventGuest(0xF0000011, 0x2000, 0x2000, 0);
    w_u32(0x800FEEF0, result);
    sub_80085E7C(result);
    result = EnableEvent(r_u32(0x800FEED4));
    result = EnableEvent(r_u32(0x800FEED8));
    result = EnableEvent(r_u32(0x800FEEDC));
    result = EnableEvent(r_u32(0x800FEEE0));
    result = EnableEvent(r_u32(0x800FEEE4));
    result = EnableEvent(r_u32(0x800FEEE8));
    result = EnableEvent(r_u32(0x800FEEEC));
    result = EnableEvent(r_u32(0x800FEEF0));
    w_u32(0x800FEED0, 0);
    w_u32(0x800FEEC8, 0);
    return result;
}

extern "C" uint32 sub_80010610(uint32 requested_mode)
{
    FUNCTION_MARKER(0x80010610, "SLUS_003.73");
    return InitializeMemoryCardEvents(requested_mode);
}


void InitializeMenuResources()
{
    uint32 base, offset, destination, words[4], menu, label;
    uint32 first, second;
    FUNCTION_MARKER(0x8001610C, "SLUS_003.73");
    InitializeLevelLoadEstimates();
    for (destination = 0; destination < 2; ++destination)
    {
        base = destination == 0 ? 0x800A545C : 0x800A53D4;
        for (offset = 0; offset < 128; offset += 16)
        {
            words[0] = r_u32(0x800A0514 + offset);
            words[1] = r_u32(0x800A0518 + offset);
            words[2] = r_u32(0x800A051C + offset);
            words[3] = r_u32(0x800A0520 + offset);
            w_u32(base + offset, words[0]);
            w_u32(base + offset + 4, words[1]);
            w_u32(base + offset + 8, words[2]);
            w_u32(base + offset + 12, words[3]);
        }
        first = r_u32(0x800A0594);
        second = r_u32(0x800A0598);
        w_u32(base + 128, first);
        w_u32(base + 132, second);
    }
    menu = AllocateZeroedObject(444);
    if (menu)
    {
        void *storage = AllocateNativeObjectStorage(menu, sizeof(MenuWindow));
        new (storage) MenuWindow(256, 0, 0, 320, 256, 26);
    }
    label = r_u32(0x800A5598);
    w_u32(0x800FF024, menu);
    MenuWindow::Attach(menu).AddItem((const char *)psx_addr(label, 1));
    menu = r_u32(0x800FF024);
    label = r_u32(0x800A5594);
    MenuWindow::Attach(menu).AddItem((const char *)psx_addr(label, 1));
    menu = r_u32(0x800FF024);
    MenuWindow::Attach(menu).spacing = 30;

}


