#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "xport.h"
#include "psx_gpu.h"
#include "game_gpu_present.h"
#include "psx_press.h"
}
#include "object_initializers.h"
#include "scene_lifecycle.h"
#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{
extern const uint32 xport_gpu_graph_type_address = 0x800FCEAC;
extern const uint32 xport_spu_register_pointer_address = 0x800FD23C;
extern const uint32 xport_cd_ready_callback_address = 0x800FDA28;
extern const uint32 xport_cd_sync_callback_address = 0x800FDA24;
extern const uint32 xport_cd_status_address = 0x800FDA34;
extern const uint32 xport_cd_setloc_table_address = 0x800FD99C;
}

extern "C" uint32 apocalypse_reset_callbacks(void)
{
    uint32 dispatch;
    if (r_u16(0x800F71D4) != 0)
        return 0;
    ResetCallbackPSX();
    w_u16(r_u32(0x800F8264), 0);
    w_u16(r_u32(0x800F8260), r_u16(r_u32(0x800F8264)));
    w_u32(r_u32(0x800F8268), 0x33333333);
    memset(psx_addr(0x800F71D4, 1050 * 4), 0, 1050 * 4);
    /* Native event callbacks replace the SDK exception execution context */
    w_u32(0x800F7210, 0x800F81EC);
    w_u16(0x800F71D4, 1);
    w_u32(r_u32(0x800F82A0), 0x107);
    w_u32(0x800F829C, 0);
    memset(psx_addr(0x800F827C, 32), 0, 32);
    w_u32(0x800F71D8, 0x80086CC4);
    ChangeClearPAD(0);
    ChangeClearRCnt(3, 0);
    memset(psx_addr(0x800F82B0, 32), 0, 32);
    w_u32(r_u32(0x800F82AC), 0);
    w_u32(0x800F71E4, 0x80086DD8);
    w_u16(0x800F7204, 9);
    w_u16(r_u32(0x800F8264), 9);
    dispatch = r_u32(0x800F825C);
    w_u32(dispatch + 20, 0x80086D30);
    w_u32(dispatch + 4, 0x80086F58);
    xport_bios_exit_critical();
    return 0x800F71D4;
}







extern "C" uint32 apocalypse_crt_initialize(void)
{
    if (r_u32(0x800F7130) != 0)
        return 1;
    w_u32(0x800F7130, 1);
    /* Empty constructors and finite delay loops have no guest data effects */
    InitializeObjectUpVector();
    InitializeObjectMotionConstants();
    InitializeObjectZeroVectors();
    ResetCameraPresetData();
    return 1;
}

extern "C" uint32 apocalypse_music_callback(uint32 address);

extern "C" uint32 apocalypse_dispatch_music_target(uint32 address)
{
    if (address == 0x8008CFFC || address == 0x8008EC6C || address == 0x8008ECB8)
        return apocalypse_music_callback(address);
    if (address == 0x80066458)
        return AdvanceSceneVBlank();
    GameDiagnostic("Unimplemented music callback: %08X\n", address);
    abort();
}

static sint32 DispatchGuestCallback(void *context, uint32 address)
{
    if (address == 0x80066458)
    {
        mdec_psyq_pump((uint32)33868800 / FIELD_RATE);
        AdvanceSceneVBlank();
        return 1;
    }
    if (address == 0x8008CFFC || address == 0x8008EC6C || address == 0x8008ECB8)
    {
        apocalypse_music_callback(address);
        return 1;
    }
    GameDiagnostic("Unimplemented guest callback: %08X\n", address);
    abort();
}

extern "C" void apocalypse_bind_callbacks(void)
{
    psx_bios_bind_guest_callback_service(DispatchGuestCallback, 0);
}

extern "C" uint32 sub_8008632C(uint32 mode)
{
    FUNCTION_MARKER(0x8008632C, "SLUS_003.73");
    return (uint32)VSync((sint32)mode);
}

extern "C" uint32 sub_8008655C(void)
{
    FUNCTION_MARKER(0x8008655C, "SLUS_003.73");
    return apocalypse_reset_callbacks();
}

extern "C" uint32 sub_800865EC(uint32 callback)
{
    FUNCTION_MARKER(0x800865EC, "SLUS_003.73");
    return VSyncCallbackPSX(callback);
}
