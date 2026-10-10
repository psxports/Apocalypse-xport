#ifndef GAME_GPU_PRESENT_H
#define GAME_GPU_PRESENT_H
#include "psx.h"
void apocalypse_set_disp_mask(sint32 enabled);
sint32 apocalypse_gpu_present(void);
sint32 apocalypse_reset_graph(sint32 mode);
#endif
