extern "C"
{
#include "psx.h"
#include "xport.h"
#include "psx_gpu.h"
#include "psx_spu.h"
#include "game_bios_bootstrap.h"
#include "game_pad_startup.h"
#include "game_gpu_present.h"
uint32 _copy_memcard_patch(void);
uint32 _patch_card(void);
uint32 _patch_card2(void);
void apocalypse_bind_callbacks(void);
uint32 apocalypse_reset_callbacks(void);
uint32 sub_80085E7C(uint32 retained_v0);
uint32 sub_8009FA6C(uint32 requested_mode);
uint32 sub_80085E6C(void);
uint32 sub_8009FF8C(uint32 mode);
uint32 sub_8009FAD8(void);
uint32 sub_8009FF9C(void);
}
#include "level_session.h"
#include "game_data_io.h"
#include <string.h>

extern "C" void xport_main(void)
{
    static uint8 image[PSX_DRAM_SIZE];
    size_t size;
    uint32 payload_size;
    const GpuPsyqStateBinding gpu_binding = {0x800FCF18, 0x800FCEBC, 0x800FCEAC, 0x800FCEAF, 0x800FCE4C, 0};
    if (!xport_file_read("../orig/SLUS_003.73", image, sizeof(image), &size) || size < 2048 || memcmp(image, "PS-X EXE", 8) != 0)
    {
        xport_message_error("Apocalypse", "Cannot load ../orig/SLUS_003.73");
        xport_set_exit_code(1);
        return;
    }
#if defined(LOCKSTEP_DEBUG)
    if (!cd_mount_cue("../iso/Apocalypse (USA).cue"))
#else
    if (!InitializeDataCd())
#endif
    {
        xport_message_error("Apocalypse", "Cannot initialize game DATA streams");
        xport_set_exit_code(1);
        return;
    }
    payload_size = (uint32)image[28] | ((uint32)image[29] << 8) | ((uint32)image[30] << 16) | ((uint32)image[31] << 24);
    if (payload_size > size - 2048 || payload_size > PSX_DRAM_SIZE - 0x10000)
    {
        xport_message_error("Apocalypse", "Invalid original executable payload");
        xport_set_exit_code(1);
        return;
    }
    xport_guest_copy(xport_guest_ref(0x80010000), xport_host_ref(image + 2048), payload_size);
    xport_guest_fill(0x800FFB50, 0, 0x80108740 - 0x800FFB50);
    w_u32(0x800F7134, 0x80108740);
    w_u32(0x800F7138, r_u32(0x800F7158) - 8 - r_u32(0x800F7154) - 0x108740);
    if (!gpu_bind_psyq_state(&gpu_binding))
    {
        xport_set_exit_code(1);
        return;
    }
    // Game frame producers do not provide complete frame-skip recovery scopes
    gpu_set_frame_skip(0);
    apocalypse_bind_card_bootstrap();
    psx_bios_init_user_heap(0x80108744, r_u32(0x800F7138));
    apocalypse_bind_callbacks();
    RunGameSession();
}

extern "C" uint32 sub_80085E7C(uint32 retained_v0)
{
    FUNCTION_MARKER(0x80085E7C, "SLUS_003.73");
    xport_bios_exit_critical();
    return retained_v0;
}

extern "C" uint32 sub_8009FA6C(uint32 requested_mode)
{
    uint32 mode = requested_mode;
    uint32 result;
    FUNCTION_MARKER(0x8009FA6C, "SLUS_003.73");
    ChangeClearPAD(0);
    sub_80085E6C();
    if (ReadInitPadFlag() == 0)
        mode = 0;
    sub_8009FF8C(mode);
    _copy_memcard_patch();
    _patch_card();
    result = _patch_card2();
    return sub_80085E7C(result);
}

extern "C" uint32 sub_80085E6C(void)
{
    FUNCTION_MARKER(0x80085E6C, "SLUS_003.73");
    return (uint32)xport_bios_enter_critical();
}

extern "C" uint32 sub_8009FF8C(uint32 mode)
{
    FUNCTION_MARKER(0x8009FF8C, "SLUS_003.73");
    return xport_bios_init_card(mode);
}

extern "C" uint32 sub_8009FAD8(void)
{
    uint32 result;
    FUNCTION_MARKER(0x8009FAD8, "SLUS_003.73");
    sub_80085E6C();
    sub_8009FF9C();
    result = ChangeClearPAD(0);
    return sub_80085E7C(result);
}

extern "C" uint32 sub_8009FF9C(void)
{
    FUNCTION_MARKER(0x8009FF9C, "SLUS_003.73");
    return xport_bios_start_card();
}
