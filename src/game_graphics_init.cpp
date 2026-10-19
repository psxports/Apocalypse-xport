extern "C"
{
#include "psx.h"
#include "game_gpu_present.h"
}

extern "C" sint32 ResetGameGraphics(sint32 mode)
{
    FUNCTION_MARKER(0x80087F7C, "SLUS_003.73");
    uint32 kind = (uint32)mode & 7;
    if (kind != 0 && kind != 3 && kind != 5)
        return ResetGraph(mode);
    ResetCallback();
    sint32 result = ResetGraph(mode);
    return result < 0 ? result : GetGraphType();
}
