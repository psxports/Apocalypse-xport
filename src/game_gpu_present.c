#include "game_gpu_present.h"
#include "psx_gpu.h"

void apocalypse_set_disp_mask(sint32 enabled)
{
    SetDispMask(enabled);
}

sint32 apocalypse_gpu_present(void)
{
    return gpu_present();
}
