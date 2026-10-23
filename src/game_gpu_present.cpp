#include <stdlib.h>

#include "scene_lifecycle.h"

extern "C"
{
#include "game_gpu_present.h"
#include "game_diagnostics.h"
#include "psx_gpu.h"
}

uint32 SubmitActiveGpuFrame(void)
{
    FUNCTION_MARKER(0x800681FC, "SLUS_003.73");
    ResetGameGraphics(1);
    if (!scene_render.published)
    {
        // TODO Recover the original FF66C publisher before enabling VBlank submission
        GameDiagnostic(("Scene VBlank submission has no published frame\n"));
        abort();
    }
    PutDrawEnv(&scene_render.published->draw);
    PutDispEnv(&scene_render.published->disp);
    DrawOTag(scene_render.published->ot + 4095);
    return 0;
}
