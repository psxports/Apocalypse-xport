#include <stdlib.h>

#include "movie_lifecycle.h"
#include "game_diagnostics.h"
#include "game_tick.h"
#include "game_settings.h"
#include "resource_io.h"
#include "cd_voice_event.h"
#include "model_bank.h"
#include "audio_session.h"
#include "psx_spu.h"
#include "game_data_io.h"

static const uint32 voice_grps[22] = {0x00000000, 0x00000019, 0x00000034, 0x0000004A, 0x00000071, 0x00000086, 0x000000B1, 0x000000C0, 0x000000CF, 0x000000F8, 0x00000107, 0x00000116, 0x00000125, 0x00000156, 0x00000187, 0x000001C3, 0x000001DB, 0x0000021F, 0x00000257, 0x00000293, 0x000002BB, 0x000002FB};

static const uint32 voice_lengths[352] = {0x0000000D, 0x00000015, 0x0000000D, 0x0000000C, 0x00000008, 0x0000000B, 0x00000019, 0x0000000F, 0x0000000B, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000F, 0x0000001B, 0x0000001B, 0x0000001B, 0x0000001B, 0x0000001B, 0x0000001B, 0x0000001B, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000010, 0x00000007, 0x00000016, 0x00000015, 0x00000011, 0x0000000E, 0x00000012, 0x0000000C, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000027, 0x0000001D, 0x00000014, 0x0000000B, 0x00000011, 0x00000010, 0x00000010, 0x00000010, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000C, 0x0000000E, 0x00000011, 0x00000015, 0x00000015, 0x00000015, 0x00000015,
                                          0x00000015, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000F, 0x00000010, 0x00000012, 0x00000017, 0x0000000F, 0x0000000C, 0x0000002B, 0x0000002B, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000010, 0x00000015, 0x00000012, 0x0000000D, 0x00000029, 0x0000001E, 0x00000010, 0x00000017, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                                          0xFFFFFFFF, 0xFFFFFFFF, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000025, 0x00000029, 0x0000001E, 0x0000000B, 0x00000012, 0x00000012, 0x0000001D, 0x00000031, 0x00000016, 0x00000018, 0x00000018, 0x00000010, 0x0000000F, 0x0000000C, 0x0000001C, 0x0000001C, 0x00000015, 0x0000001A, 0x00000017, 0x00000013, 0x00000016,
                                          0x0000001D, 0x00000016, 0x00000006, 0x00000009, 0x00000016, 0x0000000B, 0x00000011, 0x0000000D, 0x00000008, 0x00000031, 0x0000000B, 0x0000001A, 0x00000008, 0x00000018, 0x00000021, 0x0000001A, 0x0000000F, 0x0000001B, 0x0000000C, 0x0000000A, 0x0000000E, 0x0000000B, 0x00000008, 0x00000008, 0x0000001B, 0x0000003C, 0x00000012, 0x00000007, 0x0000000E, 0x00000016, 0x00000016, 0x00000014, 0x00000014, 0x00000009, 0x00000009, 0x00000011, 0x00000005, 0x00000006, 0x00000014, 0x00000018, 0x0000000F, 0x0000000F, 0x0000000F, 0x0000001C, 0x0000001D, 0x0000001E, 0x00000028, 0x00000029, 0x00000044, 0x00000026, 0x0000000C, 0x00000012, 0x0000001B, 0x00000014, 0x00000018, 0x00000023, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000001B, 0x00000020, 0x00000028, 0x0000002B, 0x0000001E, 0x0000001A, 0x0000002A, 0x00000029, 0x00000017, 0x00000025, 0x00000022, 0x00000038,
                                          0x00000013, 0x00000018, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000016, 0x0000003C, 0x00000024, 0x00000018, 0x00000014, 0x0000001E, 0x00000024, 0x0000002E, 0x00000025, 0x00000016, 0x0000000C, 0x00000011, 0x00000013, 0x0000000D, 0x0000000D, 0xFFFFFFFF, 0x0000001B, 0x00000015, 0x0000000D, 0x0000000F, 0x00000017, 0x00000028, 0x00000014, 0x00000016, 0x00000019, 0x00000027, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000008, 0x00000012, 0x0000000C, 0x00000014, 0x0000000B, 0x00000007, 0x00000008, 0x00000012, 0x0000001B, 0x0000000C, 0x0000000D, 0x0000001D, 0x0000000E, 0x0000002E, 0x00000040, 0xFFFFFFFF, 0x00000013, 0x00000011, 0x00000017, 0x0000000B, 0x0000000F, 0x0000000D, 0x0000000A, 0x00000015, 0x00000008, 0x0000000B, 0x00000011, 0x0000000B, 0x0000000B, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

CdVoiceState cd_voice = {0, 0, true, false, true, false, 0, 0, 0, 0, 0, {0}};

void ResetCdVoiceState()
{
    CdVoiceState initial = {0, 0, true, false, true, false, 0, 0, 0, 0, 0, {0}};
    cd_voice = initial;
}

static uint32 PrepareCdVoice(void)
{
    FUNCTION_MARKER(0x8002FAD0, "SLUS_003.73");
    uint8 parameter = 0xC8;
    ControlDataCd(CdlSetmode, &parameter, cd_voice.response);
    if (cd_voice.response[0])
        cd_voice.prepared = true;
    return 1;
}

void SetCdAudioEnabled(bool enabled)
{
    FUNCTION_MARKER(0x8002FB18, "SLUS_003.73");
    cd_voice.enabled = enabled;
}

uint32 UpdateCdVoiceBalance(const Vector3 *pos)
{
    FUNCTION_MARKER(0x8002FB34, "SLUS_003.73");
    uint32 balance = CalculateSoundBalance(pos, 1024, 16384);
    sint16 gain = game_settings.voice_volume;
    uint32 left_product = (balance & 4095) * gain;
    uint32 right_product = ((balance >> 16) & 4095) * gain;
    uint32 left = left_product >> 12;
    uint32 right = right_product >> 12;
    if (left >= 256)
        left = 255;
    if (right >= 256)
        right = 255;
    CdlATV volume;
    volume.val0 = left;
    volume.val1 = right;
    volume.val3 = 0;
    volume.val2 = 0;
    return CdMix(&volume);
}

uint32 LocateXaVoiceFile()
{
    FUNCTION_MARKER(0x8002FBB4, "SLUS_003.73");
    CdlFILE file;
    if (!FindDataCdFile(&file, "\\APOC.XA;1"))
        abort();
    uint32 sector = CdPosToInt(&file.pos);
    cd_voice.base_sector = sector;
    return sector;
}

uint32 SeekCdAudioEntry(uint32 idx)
{
    uint32 result, entry, sector;
    uint8 mode = 200;
    CdlLOC location;
    FUNCTION_MARKER(0x8002FBE4, "SLUS_003.73");
    result = cd_voice.enabled;
    if (!result)
        return result;
    ControlDataCd(CdlSetmode, &mode, cd_voice.response);
    // The original word shift discards the upper two group bits
    uint32 grp = idx & 0x3FFFFFFF;
    if (grp >= 22)
    {
        GameDiagnostic(("Invalid CD audio group %u\n", idx));
        abort();
    }
    entry = voice_grps[grp];
    sector = cd_voice.base_sector + (entry << 4);
    CdIntToPos(sector, &location);
    return ControlDataCd(22, &location, cd_voice.response);
}

uint32 PlayCdVoiceEvent(uint32 kind, uint32 variant, const Vector3 *pos)
{
    FUNCTION_MARKER(0x8002FC64, "SLUS_003.73");
    uint32 result = GetResourceReadPhase();
    if (result)
        return result;
    result = ModelBank::GetLoadPending();
    if (result)
        return result;
    result = movie_playback.active;
    if (result)
        return result;
    result = cd_voice.available;
    if (!result)
        return result;
    result = cd_voice.cooldown;
    if (result)
        return result;
    SelectCdVoice(kind, variant);
    UpdateCdVoiceBalance(pos);
    result = game_clock.vblank_frame;
    cd_voice.kind = kind;
    cd_voice.variant = variant;
    cd_voice.last_frame = result;
    return result;
}

uint32 SelectCdVoice(sint32 kind, uint32 variant)
{
    FUNCTION_MARKER(0x8002FD2C, "SLUS_003.73");
    uint32 result = kind < 22;
    if (!result)
        return result;
    uint32 grp = kind & 0x3FFFFFFF;
    // Shipped channel 18 crosses a nominal 16-word row
    // Keep the original wrapping flat offset before narrowing the filter channel
    uint32 table_offset = (grp << 6) + (variant << 2);
    if (table_offset >= sizeof(voice_lengths))
    {
        GameDiagnostic(("Invalid CD voice group/variant %d/%u\n", kind, variant));
        abort();
    }
    if (voice_lengths[table_offset / 4] == 0xFFFFFFFF)
        return 0xFFFFFFFF;
    result = GetResourceReadPhase();
    if (result)
        return result;
    result = ModelBank::GetLoadPending();
    if (result)
        return result;
    result = movie_playback.active;
    if (result)
        return result;
    result = cd_voice.cooldown;
    if (result)
        return result;
    result = cd_voice.available;
    if (!result)
        return result;
    result = cd_voice.prepared;
    if (!result)
    {
        PrepareCdVoice();
        result = cd_voice.prepared;
        if (!result)
            return result;
    }
    uint8 filter[2];
    filter[0] = 1;
    filter[1] = variant;
    ControlDataCd(13, filter, cd_voice.response);
    // Validate the second table only when its original read is reached
    if (grp >= 22)
    {
        GameDiagnostic(("Invalid decoded CD voice group %d\n", kind));
        abort();
    }
    uint32 sector = cd_voice.base_sector + (voice_grps[grp] << 4);
    uint32 len = voice_lengths[table_offset / 4];
    cd_voice.cur_sector = sector;
    cd_voice.end_sector = sector + (len << 4);
    CdlLOC location;
    CdIntToPos(sector, &location);
    uint8 gain = game_settings.voice_volume;
    CdlATV volume;
    volume.val2 = 0;
    volume.val3 = 0;
    volume.val0 = gain;
    volume.val1 = gain;
    CdMix(&volume);
    FastControlDataCd(CdlDemute, 0);
    ControlDataCd(27, &location, cd_voice.response);
    if (cd_voice.response[0])
    {
        cd_voice.active = true;
        cd_voice.available = false;
    }
    return 1;
}
