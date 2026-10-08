#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <string.h>

uint32 xport_draft_host_sub_800981B4_p1(const void *volume)
{
    CdlATV native_volume;
    memcpy(&native_volume, volume, sizeof(native_volume));
    CdMix(&native_volume);
    return 1u;
}

uint32 sub_8009928C(uint32 volume)
{
    uint8 channels[4];
    uint32 index;
    for (index = 0u; index < 4u; ++index)
        channels[index] = r_u8(volume + index);
    xport_draft_host_sub_800981B4_p1(channels);
    return 0u;
}
