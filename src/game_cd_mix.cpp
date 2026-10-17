extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>

extern "C" uint32 xport_draft_host_sub_800981B4_p1(const void *volume)
{
    FUNCTION_MARKER(0x800981B4, "SLUS_003.73");
    CdlATV native_volume;
    memcpy(&native_volume, volume, sizeof(native_volume));
    CdMix(&native_volume);
    return 1;
}

extern "C" uint32 sub_8009928C(uint32 volume)
{
    FUNCTION_MARKER(0x8009928C, "SLUS_003.73");
    uint8 channels[4];
    uint32 index;
    for (index = 0; index < 4; ++index)
        channels[index] = r_u8(volume + index);
    xport_draft_host_sub_800981B4_p1(channels);
    return 0;
}
