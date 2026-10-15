extern "C"
{
#include "game_gpu_present.h"
#include "draft_first_signatures.h"
#include "psx_gpu.h"
}

void SetDisplayMask(sint32 enabled)
{
    SetDispMask(enabled);
}

sint32 PresentGpuFrame(void)
{
    return gpu_present();
}

uint32 SubmitActiveGpuFrame(void)
{
    FUNCTION_MARKER(0x800681FC, "SLUS_003.73");
    sub_80087F7C(1);
    sub_8008895C(r_u32(0x800FF66C));
    sub_80088B28(r_u32(0x800FF66C) + 92);
    return sub_800888EC(r_u32(0x800FF66C) + 16492);
}

extern "C" void apocalypse_set_disp_mask(sint32 enabled) { SetDisplayMask(enabled); }
extern "C" sint32 apocalypse_gpu_present(void) { return PresentGpuFrame(); }
extern "C" uint32 sub_800681FC(void)
{
    FUNCTION_MARKER(0x800681FC, "SLUS_003.73");
    return SubmitActiveGpuFrame();
}
