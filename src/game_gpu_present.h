#ifndef GAME_GPU_PRESENT_H
#define GAME_GPU_PRESENT_H
#include "psx.h"
#ifdef __cplusplus
extern "C"
{
#endif
    uint32 SubmitActiveGpuFrame();
    void SetDisplayMask(sint32 enabled);
    sint32 PresentGpuFrame(void);
    sint32 ResetGameGraphics(sint32 mode);
#ifdef __cplusplus
}
#endif
#endif
