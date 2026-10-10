#include "draft_first_signatures.h"
#include "xport.h"
#include "psx_gpu.h"
#include "game_gpu_present.h"
#include "psx_press.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint32 xport_gpu_graph_type_address = 0x800FCEACu;
const uint32 xport_spu_register_pointer_address = 0x800FD23Cu;
const uint32 xport_cd_ready_callback_address = 0x800FDA28u;
const uint32 xport_cd_sync_callback_address = 0x800FDA24u;
const uint32 xport_cd_status_address = 0x800FDA34u;
const uint32 xport_cd_setloc_table_address = 0x800FD99Cu;

uint32 apocalypse_reset_callbacks(void)
{
    uint32 dispatch;
    if (r_u16(0x800F71D4u) != 0u)
        return 0u;
    ResetCallbackPSX();
    w_u16(r_u32(0x800F8264u), 0u);
    w_u16(r_u32(0x800F8260u), r_u16(r_u32(0x800F8264u)));
    w_u32(r_u32(0x800F8268u), 0x33333333u);
    memset(psx_addr(0x800F71D4u, 1050u * 4u), 0, 1050u * 4u);
    /* Native event callbacks replace the SDK exception execution context */
    w_u32(0x800F7210u, 0x800F81ECu);
    w_u16(0x800F71D4u, 1u);
    w_u32(r_u32(0x800F82A0u), 0x107u);
    w_u32(0x800F829Cu, 0u);
    memset(psx_addr(0x800F827Cu, 32u), 0, 32u);
    w_u32(0x800F71D8u, 0x80086CC4u);
    ChangeClearPAD(0u);
    ChangeClearRCnt(3u, 0u);
    memset(psx_addr(0x800F82B0u, 32u), 0, 32u);
    w_u32(r_u32(0x800F82ACu), 0u);
    w_u32(0x800F71E4u, 0x80086DD8u);
    w_u16(0x800F7204u, 9u);
    w_u16(r_u32(0x800F8264u), 9u);
    dispatch = r_u32(0x800F825Cu);
    w_u32(dispatch + 20u, 0x80086D30u);
    w_u32(dispatch + 4u, 0x80086F58u);
    xport_bios_exit_critical();
    return 0x800F71D4u;
}

uint32 sub_8003B948(void)
{
    FUNCTION_MARKER(0x8003B948u, "SLUS_003.73");
    w_u32(0x800A6844u, 0x00000000u);
    w_u32(0x800A6848u, 0x00008000u);
    w_u32(0x800A684Cu, 0x00000000u);
    return 0x800A6844u;
}

uint32 sub_800622E0(void)
{
    FUNCTION_MARKER(0x800622E0u, "SLUS_003.73");
    w_u32(0x800FFF80u, 0x00000000u);
    w_u32(0x800FFF84u, 0xFFE70000u);
    w_u32(0x800FFF88u, 0x00000000u);
    w_u32(0x800FFF70u, 0x00000000u);
    w_u32(0x800FFF74u, 0xFFF9C000u);
    w_u32(0x800FFF78u, 0x00000000u);
    w_u32(0x800FFF60u, 0x00000000u);
    w_u32(0x800FFF64u, 0xFFF60000u);
    w_u32(0x800FFF68u, 0xFFC4A000u);
    w_u32(0x800FFF50u, 0x00000000u);
    w_u32(0x800FFF54u, 0xFFF9C000u);
    w_u32(0x800FFF58u, 0x00000000u);
    w_u32(0x800FFF40u, 0x00000000u);
    w_u32(0x800FFF44u, 0xFFF6A000u);
    w_u32(0x800FFF48u, 0x0028A000u);
    w_u32(0x800FFF30u, 0x00000000u);
    w_u32(0x800FFF34u, 0xFFF9C000u);
    w_u32(0x800FFF38u, 0x00000000u);
    w_u32(0x800FFF20u, 0x00000000u);
    w_u32(0x800FFF24u, 0xFFBB4000u);
    w_u32(0x800FFF28u, 0x00514000u);
    w_u32(0x800FFF10u, 0x00000000u);
    w_u32(0x800FFF14u, 0xFFF9C000u);
    w_u32(0x800FFF18u, 0xFFCE0000u);
    w_u32(0x800FFF00u, 0x00000000u);
    w_u32(0x800FFF04u, 0xFF736000u);
    w_u32(0x800FFF08u, 0x00190000u);
    w_u32(0x800FFEF0u, 0x00000000u);
    w_u32(0x800FFEF4u, 0xFFF9C000u);
    w_u32(0x800FFEF8u, 0xFFED4000u);
    w_u32(0x800FFEE0u, 0x00000000u);
    w_u32(0x800FFEE4u, 0xFFF1F000u);
    w_u32(0x800FFEE8u, 0x00320000u);
    w_u32(0x800FFED0u, 0x00000000u);
    w_u32(0x800FFED4u, 0xFFE7A000u);
    w_u32(0x800FFED8u, 0x00320000u);
    w_u32(0x800FFEC0u, 0x00000000u);
    w_u32(0x800FFEC4u, 0x00064000u);
    w_u32(0x800FFEC8u, 0xF92A0000u);
    w_u32(0x800FFEB0u, 0x00000000u);
    w_u32(0x800FFEB4u, 0xFFF9C000u);
    w_u32(0x800FFEB8u, 0x00000000u);
    w_u16(0x800FFC10u, 0x00000032u);
    w_u16(0x800FFC08u, 0x0000001Eu);
    w_u8(0x800FFBF4u, 0x00000003u);
    w_u8(0x800FFBF6u, 0x00000003u);
    w_u16(0x800FFBECu, 0x00000258u);
    w_u16(0x800FFBF0u, 0x00000258u);
    w_u32(0x800FFEA0u, 0x00000000u);
    w_u16(0x800FFC0Cu, 0x00000019u);
    w_u16(0x800FFC0Eu, 0x00000000u);
    w_u16(0x800FFC04u, 0x00000000u);
    w_u16(0x800FFC06u, 0x00000000u);
    w_u16(0x800FFBFCu, 0x00000000u);
    w_u16(0x800FFBFEu, 0x00000000u);
    w_u16(0x800FFC00u, 0x00000019u);
    w_u8(0x800FFBF5u, 0x00000000u);
    w_u16(0x800FFBEEu, 0x00000000u);
    w_u32(0x800FFEA4u, 0xFFF92000u);
    w_u32(0x800FFEA8u, 0x00000000u);
    return 0x800FFEA0u;
}

uint32 sub_80063928(void)
{
    FUNCTION_MARKER(0x80063928u, "SLUS_003.73");
    w_u32(0x800A71CCu, 0x00000000u);
    w_u32(0x800A71D0u, 0x00000000u);
    w_u32(0x800A71D4u, 0x00000000u);
    w_u16(0x800FF5E4u, 0x00000000u);
    w_u16(0x800FF5E6u, 0x00000000u);
    w_u16(0x800FF5E8u, 0x00000000u);
    return 0x800A71CCu;
}

uint32 apocalypse_crt_initialize(void)
{
    if (r_u32(0x800F7130u) != 0u)
        return 1u;
    w_u32(0x800F7130u, 1u);
    /* Empty constructors and finite delay loops have no guest data effects */
    sub_8003B948();
    sub_800622E0();
    sub_80063928();
    sub_8007A5FC();
    return 1u;
}

extern uint32 apocalypse_music_callback(uint32 address);

uint32 apocalypse_dispatch_music_target(uint32 address)
{
    if (address == 0x8008CFFCu || address == 0x8008EC6Cu || address == 0x8008ECB8u)
        return apocalypse_music_callback(address);
    if (address == 0x80066458u)
        return sub_80066458();
    fprintf(stderr, "Unimplemented music callback: %08X\n", address);
    abort();
}

static sint32 dispatch_guest_callback(void *context, uint32 address)
{
    if (address == 0x80066458u)
    {
        mdec_psyq_pump(33868800u / FIELD_RATE);
        sub_80066458();
        return 1;
    }
    if (address == 0x8008CFFCu || address == 0x8008EC6Cu || address == 0x8008ECB8u)
    {
        apocalypse_music_callback(address);
        return 1;
    }
    fprintf(stderr, "Unimplemented guest callback: %08X\n", address);
    abort();
}

void apocalypse_bind_callbacks(void)
{
    psx_bios_bind_guest_callback_service(dispatch_guest_callback, NULL);
}
