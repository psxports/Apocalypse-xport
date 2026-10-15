extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "cd_voice_event.h"
#include "audio_session.h"
#include "psx_spu.h"
#include "game_data_io.h"
#include <stdlib.h>

uint32 UpdateCdVoiceBalance(const uint32 *position)
{
    FUNCTION_MARKER(0x8002FB34, "SLUS_003.73");
    uint32 balance = CalculateSoundBalance(position, 1024, 16384);
    sint32 gain = (sint16)r_u16(0x800ECC7C);
    uint32 left_product = (balance & 4095) * (uint32)gain;
    uint32 right_product = ((balance >> 16) & 4095) * (uint32)gain;
    uint32 left = left_product >> 12;
    uint32 right = right_product >> 12;
    if (left >= 256)
        left = 255;
    if (right >= 256)
        right = 255;
    CdlATV volume;
    volume.val0 = (uint8)left;
    volume.val1 = (uint8)right;
    volume.val3 = 0;
    volume.val2 = 0;
    return (uint32)CdMix(&volume);
}

uint32 PlayCdVoiceEvent(uint32 kind, uint32 variant, const uint32 *position, uint32 mode)
{
    FUNCTION_MARKER(0x8002FC64, "SLUS_003.73");
    uint32 result = r_u32(0x800FF700);
    if (result)
        return result;
    uint32 scene = r_u32(0x800FF79C);
    result = r_u8(0x800EADC5 + 16 * scene);
    if (result)
        return result;
    result = r_u32(0x800FF250);
    if (result)
        return result;
    result = r_u32(0x800FF2BC);
    if (!result)
        return result;
    result = r_u32(0x800FF2B0);
    if (result)
        return result;
    // The original selected-voice helper does not read the forwarded mode
    (void)mode;
    SelectCdVoice(kind, variant);
    UpdateCdVoiceBalance(position);
    result = r_u32(0x800FF64C);
    w_u32(0x800FF2C4, kind);
    w_u32(0x800FF2C8, variant);
    w_u32(0x800FF2CC, result);
    return result;
}

extern "C" uint32 sub_8002FB34(uint32 position)
{
    FUNCTION_MARKER(0x8002FB34, "SLUS_003.73");
    return UpdateCdVoiceBalance((const uint32 *)psx_addr(position, 12));
}

extern "C" uint32 sub_8002FC64(uint32 kind, uint32 variant, uint32 position, uint32 mode)
{
    FUNCTION_MARKER(0x8002FC64, "SLUS_003.73");
    return PlayCdVoiceEvent(kind, variant, (const uint32 *)psx_addr(position, 12), mode);
}

uint32 SelectCdVoice(uint32 kind, uint32 variant)
{
    FUNCTION_MARKER(0x8002FD2C, "SLUS_003.73");
    uint32 result = (sint32)kind < 22;
    if (!result)
        return result;
    uint32 entry = 0x800A5FD8 + (kind << 6) + (variant << 2);
    if (r_u32(entry) == 0xFFFFFFFF)
        return 0xFFFFFFFF;
    result = r_u32(0x800FF700);
    if (result)
        return result;
    uint32 scene = r_u32(0x800FF79C);
    result = r_u8(0x800EADC5 + (scene << 4));
    if (result)
        return result;
    result = r_u32(0x800FF250);
    if (result)
        return result;
    result = r_u32(0x800FF2B0);
    if (result)
        return result;
    result = r_u32(0x800FF2BC);
    if (!result)
        return result;
    result = r_u32(0x800FF2C0);
    if (!result)
    {
        PrepareCdVoice();
        result = r_u32(0x800FF2C0);
        if (!result)
            return result;
    }
    uint8 filter[2];
    filter[0] = 1;
    filter[1] = (uint8)variant;
    xport_draft_host_sub_80097DF8_p2(13, filter, 0x800FF2D8);
    uint32 group = r_u32(0x800A5F80 + (kind << 2));
    uint32 base = r_u32(0x800FF2AC);
    uint32 sector = base + (group << 4);
    uint32 offset = r_u32(entry);
    w_u32(0x800FF2D4, sector);
    w_u32(0x800FF2D0, sector + (offset << 4));
    CdlLOC location;
    CdIntToPos((sint32)sector, &location);
    uint32 gain = r_u8(0x800ECC7C);
    CdlATV volume;
    volume.val2 = 0;
    volume.val3 = 0;
    volume.val0 = (uint8)gain;
    volume.val1 = (uint8)gain;
    CdMix(&volume);
    sub_80097F34(12, 0);
    xport_draft_host_sub_80097DF8_p2(27, &location, 0x800FF2D8);
    if (r_u8(0x800FF2D8))
    {
        w_u32(0x800FF2B8, 1);
        w_u32(0x800FF2BC, 0);
    }
    return 1;
}

extern "C" uint32 sub_8002FD2C(uint32 kind, uint32 variant)
{
    FUNCTION_MARKER(0x8002FD2C, "SLUS_003.73");
    return SelectCdVoice(kind, variant);
}

uint32 PrepareCdVoice(void)
{
    FUNCTION_MARKER(0x8002FAD0, "SLUS_003.73");
    uint8 parameter = 0xC8;
    xport_draft_host_sub_80097DF8_p2(14, &parameter, 0x800FF2D8);
    if (r_u8(0x800FF2D8))
        w_u32(0x800FF2C0, 1);
    return 1;
}

extern "C" uint32 sub_8002FAD0(void)
{
    FUNCTION_MARKER(0x8002FAD0, "SLUS_003.73");
    return PrepareCdVoice();
}

uint32 LocateXaVoiceFile()
{
    FUNCTION_MARKER(0x8002FBB4, "SLUS_003.73");
    CdlFILE file;
    // TODO Original consumes the position even if file lookup fails
#if defined(LOCKSTEP_DEBUG)
    CdSearchFile(&file, (const char *)psx_addr(0x800A1998, 1));
#else
    if (!FindDataCdFile(&file, (const char *)psx_addr(0x800A1998, 1)))
        abort();
#endif
    uint32 sector = (uint32)CdPosToInt(&file.pos);
    w_u32(0x800FF2AC, sector);
    return sector;
}

extern "C" uint32 sub_8002FBB4(void)
{
    FUNCTION_MARKER(0x8002FBB4, "SLUS_003.73");
    return LocateXaVoiceFile();
}
