extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "psx_spu.h"

uint32 RequestCdStreamRefresh(void)
{
    FUNCTION_MARKER(0x8002EF58, "SLUS_003.73");
    w_u32(0x800FF254, 1);
    return 1;
}

void SetCdAudioEnabled(uint32 enabled)
{
    FUNCTION_MARKER(0x8002FB18, "SLUS_003.73");
    w_u32(0x800FF2B4, enabled);
}

uint32 SeekCdAudioEntry(uint32 index)
{
    uint32 result, entry, sector;
    uint8 mode = 200;
    CdlLOC location;
    FUNCTION_MARKER(0x8002FBE4, "SLUS_003.73");
    result = r_u32(0x800FF2B4);
    if (!result)
        return result;
    xport_draft_host_sub_80097DF8_p2(14, &mode, 0x800FF2D8);
    entry = r_u32(0x800A5F80 + index * 4);
    sector = r_u32(0x800FF2AC) + (entry << 4);
    CdIntToPos((sint32)sector, &location);
    return xport_draft_host_sub_80097DF8_p2(22, &location, 0x800FF2D8);
}

extern "C" uint32 sub_8002EF58(void)
{
    FUNCTION_MARKER(0x8002EF58, "SLUS_003.73");
    return RequestCdStreamRefresh();
}
extern "C" void sub_8002FB18(uint32 enabled)
{
    FUNCTION_MARKER(0x8002FB18, "SLUS_003.73");
    SetCdAudioEnabled(enabled);
}
extern "C" uint32 sub_8002FBE4(uint32 index)
{
    FUNCTION_MARKER(0x8002FBE4, "SLUS_003.73");
    return SeekCdAudioEntry(index);
}
