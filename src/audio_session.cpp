#include <stdlib.h>
#include <string.h>

#include "game_settings.h"
#include "camera.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "audio_session.h"
#include "game_data_io.h"
#include "sequence_transport.h"
#include "audio_voice.h"
#include "global.h"
#include "resource_io.h"
#include "game_heap.h"
#include "psx_spu.h"

static void ReleaseSoundBank(sint16 bank);
static sint16 LoadSoundBank(const char *name);

#if !defined(XPORT_NATIVE_VAB) || !XPORT_NATIVE_VAB
    #error Native VAB resources require XPORT_NATIVE_VAB in the shared SDK
#endif
#if !defined(XPORT_NATIVE_SEQUENCE) || !XPORT_NATIVE_SEQUENCE
    #error Native SEQ resources require XPORT_NATIVE_SEQUENCE in the shared sequencer
#endif

SoundBankHandles sound_banks = {-1, -1, -1};

static uint8 *sequence_data;

void ResetSoundBankHandles()
{
    SoundBankHandles initial = {-1, -1, -1};
    sound_banks = initial;
}

struct SoundDefinition
{
    uint32 cfg;
    uint8 note;
};

enum
{
    SoundDefinitionCount = 256
};

static const SoundDefinition sound_definitions[SoundDefinitionCount] = {{0x00000001, 10}, {0x00000002, 20}, {0x00000004, 30}, {0x00000008, 40}, {0x00000010, 50}, {0x00000020, 60}, {0x00000040, 70}, {0x00000080, 80}, {0x00010001, 10}, {0x80010002, 30}, {0x00010004, 40}, {0x80010008, 50}, {0x00010010, 60}, {0x00010020, 70}, {0x00010040, 80}, {0x00020001, 10}, {0x00020002, 20}, {0x00020004, 30}, {0x80020008, 40}, {0x00020010, 70}, {0x00020020, 80}, {0x00030001, 10}, {0x00030002, 30}, {0x00030004, 40}, {0x00030008, 50}, {0x00030010, 60}, {0x00030020, 70}, {0x00040001, 10}, {0x00040002, 20}, {0x00040004, 30}, {0x80040008, 40}, {0x00040010, 50}, {0x00040020, 60}, {0x00040040, 70}, {0x00040080, 80}, {0x40000001, 10}, {0x40000002, 20}, {0x40000004, 30}, {0x40000008, 40}, {0xC0000010, 50}, {0x40000020, 60}, {0x40000040, 70}, {0x40000080, 80}, {0x40010001, 20}, {0x40010002, 30}, {0x40010004, 40}, {0x40010008, 50},
                                                                        {0x40040001, 10}, {0xC0040002, 40}, {0xC0050001, 10}, {0x40050002, 20}, {0x40050004, 30}, {0x40000001, 10}, {0x40000002, 20}, {0x40000004, 30}, {0x40000008, 40}, {0x40000010, 50}, {0x40000020, 60}, {0x40000040, 70}, {0x40000080, 80}, {0xC0010001, 10}, {0xC0010002, 20}, {0xC0010004, 30}, {0x40010008, 40}, {0x40010010, 50}, {0x40010020, 60}, {0x40010040, 70}, {0x40010080, 80}, {0x40020001, 10}, {0xC0020002, 20}, {0x40020008, 30}, {0x40020008, 40}, {0x40020010, 50}, {0x40020020, 60}, {0x40020040, 70}, {0x40020080, 80}, {0xC0030001, 15}, {0x40030002, 20}, {0x40000001, 10}, {0x40000002, 20}, {0x40000004, 30}, {0xC0000008, 40}, {0xC0000010, 50}, {0x40000020, 60}, {0xC0030001, 30}, {0x40030002, 40}, {0x40030004, 50}, {0xC0030008, 60}, {0x40030010, 70}, {0xC0040002, 20}, {0x40000001, 10}, {0x40000002, 20}, {0x40000004, 30}, {0x40000008, 40},
                                                                        {0x40000010, 50}, {0xC0000020, 60}, {0x40000040, 70}, {0x40000080, 80}, {0x40010001, 10}, {0x40010002, 20}, {0xC0000001, 10}, {0xC0000008, 40}, {0xC0000010, 50}, {0x40000020, 60}, {0x40000040, 70}, {0x40000001, 10}, {0xC0000002, 20}, {0x40000004, 30}, {0x40000020, 60}, {0x40000040, 70}, {0x40010004, 30}, {0x40010010, 50}, {0x40010020, 60}, {0x40010040, 70}, {0x40000001, 10}, {0xC0000002, 20}, {0x40000004, 30}, {0x40000008, 40}, {0xC0000010, 50}, {0x40000020, 60}, {0x40000001, 10}, {0x40000002, 20}, {0x40000004, 30}, {0x40000010, 40}, {0x40000020, 50}, {0x40000001, 10}, {0x40000002, 20}, {0xC0000004, 30}, {0x40000008, 40}, {0x40000010, 50}, {0x40000020, 60}, {0x40000040, 70}, {0x40000001, 10}, {0x40000002, 20}, {0xC0000004, 30}, {0x40000008, 40}, {0xC0000010, 50}, {0x40000020, 60}, {0x40000040, 70}, {0x40000080, 80}, {0x40010001, 10}};

PositionedSoundEvent positioned_sound_event;

void ResetSoundEvents()
{
    PositionedSoundEvent initial = {false, {0, 0, 0}};
    positioned_sound_event = initial;
}

AudioPlaybackState audio_playback = {{0}, {0}, -1, false, false, true, 0, 0, false};

void ResetAudioPlayback()
{
    AudioPlaybackState initial = {{0}, {0}, -1, false, false, true, 0, 0, false};
    audio_playback = initial;
}

uint32 InitializeAudioOutput()
{
    FUNCTION_MARKER(0x800698FC, "SLUS_003.73");
    SsInit();
    ResetMusicSequencer();
    SsSetSerialAttr(0, 0, 1);
    SsSetSerialAttr(0, 1, 1);
    SsSetSerialVol(0, 127, 127);
    return 0;
}

uint32 InitializeAudioSession(const char *resource)
{
    FUNCTION_MARKER(0x8006994C, "SLUS_003.73");
    SetSoundReverbMode(0);
    sound_banks.common = LoadSoundBank(resource);
    InitializeSequenceTracks(1, 1);
    InitializeSequenceCallbacks();
    InitializeMusicSequenceTimer();
    SetMusicSequenceTickMode(4096);
    SsSetReservedVoice(24);
    StartMusicSequencer();
    audio_playback.resume_pending = false;
    audio_playback.start_pending = true;
    return 1;
}

uint32 ReleaseAudioSession()
{
    sint16 result;
    sint16 bank;
    FUNCTION_MARKER(0x80069A94, "SLUS_003.73");
    result = audio_playback.sequence;
    if (result >= 0)
    {
        StopAndResetSequence(audio_playback.sequence);
        result = -1;
        audio_playback.sequence = -1;
    }
    free(sequence_data);
    sequence_data = 0;
    bank = sound_banks.music;
    if (bank >= 0)
    {
        ReleaseSoundBank(bank);
        result = -1;
        sound_banks.music = -1;
    }
    return result;
}

static void ReleaseSoundBank(sint16 bank)
{
    FUNCTION_MARKER(0x80069CF8, "SLUS_003.73");
    SsVabClose(bank);
}

static sint16 LoadSoundBank(const char *name)
{
    FUNCTION_MARKER(0x80069D2C, "SLUS_003.73");
    uint32 file_size = LookupResource(name);
    // The decoded resource directory bounds rounded reads below 2 GiB
    uint32 capacity = file_size + 64;
    uint8 *image = (uint8 *)malloc(capacity);
    if (!image)
    {
        GameDiagnostic(("Cannot allocate sound bank %s\n", name));
        abort();
    }
    BeginResourceRead(image);
    WaitForResourceRead();
    if (file_size < 32)
    {
        GameDiagnostic(("Truncated sound bank %s\n", name));
        abort();
    }
    VabHdr *header = (VabHdr *)image;
    uint32 header_bytes = (header->ps << 9) + 0xA20;
    uint32 total_bytes = header->fsize;
    if (header_bytes > file_size || total_bytes < header_bytes || total_bytes > file_size)
    {
        GameDiagnostic(("Invalid sound bank spans %s\n", name));
        abort();
    }
    total_bytes = header_bytes + ((total_bytes - header_bytes + 63) & 0xFFFFFFC0);
    header->fsize = total_bytes;
    sint16 bank = SsVabOpenHead(image, -1);
    SpuVabNativeView view;
    if (!spu_vab_native_view(bank, &view) || view.body_bytes > capacity - header_bytes)
    {
        GameDiagnostic(("Cannot open native sound bank %s\n", name));
        abort();
    }
    SsVabTransBody(image + header_bytes, bank);
    SsVabTransCompleted(1);
    // The SDK owns complete metadata and SPU RAM after the synchronous transfer
    free(image);
    return bank;
}

static uint32 BuildAudioBankFilename(char (&filename)[24], const char *basename)
{
    uint32 idx = 0;
    while (basename[idx])
    {
        if (idx >= sizeof(filename) - 5)
        {
            GameDiagnostic(("Audio resource basename exceeds the local buffer\n"));
            abort();
        }
        filename[idx] = basename[idx];
        ++idx;
    }
    memcpy(filename + idx, ".VAB", 5);
    return idx;
}

sint16 LoadLevelSoundBank(const char *basename)
{
    char filename[24];
    FUNCTION_MARKER(0x80069B0C, "SLUS_003.73");
    BuildAudioBankFilename(filename, basename);
    sint16 bank = LoadSoundBank(filename);
    sound_banks.level = bank;
    return bank;
}

void ReleaseLevelSoundBank()
{
    FUNCTION_MARKER(0x80069B8C, "SLUS_003.73");
    sint16 bank = sound_banks.level;
    if (bank >= 0)
    {
        ReleaseSoundBank(bank);
        sound_banks.level = -1;
    }
}

uint32 LoadAudioSequence(const char *basename)
{
    char filename[24];
    uint32 file_size, data_size, result;
    FUNCTION_MARKER(0x80069BC4, "SLUS_003.73");
    uint32 idx = BuildAudioBankFilename(filename, basename);
    sint16 bank = LoadSoundBank(filename);
    sound_banks.music = bank;
    filename[idx + 1] = 'S';
    filename[idx + 2] = 'E';
    filename[idx + 3] = 'Q';
    data_size = GetResourceFileSize(filename);
    file_size = LookupResource(filename);
    if (sequence_data)
    {
        GameDiagnostic(("SEQ data must be released before loading another sequence\n"));
        abort();
    }
    sequence_data = (uint8 *)malloc(file_size);
    if (!sequence_data)
    {
        GameDiagnostic(("Cannot allocate SEQ data %s\n", filename));
        abort();
    }
    BeginResourceRead(sequence_data);
    WaitForResourceRead();
    bank = sound_banks.music;
    audio_playback.sequence = OpenAudioSequence(sequence_data, data_size, bank);
    if (data_size < 13)
    {
        GameDiagnostic(("Truncated SEQ tempo %s\n", filename));
        abort();
    }
    result = sequence_data[12] << 16;
    audio_playback.tempo = sequence_data[10] | (sequence_data[11] << 8) | result;
    return result;
}

uint32 PlaySoundEffect(sint32 sound_id, sint16 volume, uint32 pitch_variation)
{
    sint32 product;
    sint16 scaled;
    FUNCTION_MARKER(0x80069DF0, "SLUS_003.73");
    if (sound_id < 0)
        return 0;
    product = volume * game_settings.effects_volume;
    // The original word shift discards the upper three ID bits
    uint32 idx = sound_id & 0x1FFFFFFF;
    if (idx >= SoundDefinitionCount)
    {
        GameDiagnostic(("Invalid sound definition %d\n", sound_id));
        abort();
    }
    const SoundDefinition &sound = sound_definitions[idx];
    scaled = product >> 14;
    return PlaySoundVoices(sound.cfg, sound.note, scaled, scaled, pitch_variation);
}

uint32 PlayScaledSound(sint32 sound_id, sint16 left, sint16 right, uint32 pitch_variation)
{
    sint32 left_product, right_product;
    sint16 scale;
    FUNCTION_MARKER(0x80069E68, "SLUS_003.73");
    if (sound_id < 0)
        return 0;
    scale = game_settings.effects_volume;
    left_product = left * scale;
    right_product = right * scale;
    // The original word shift discards the upper three ID bits
    uint32 idx = sound_id & 0x1FFFFFFF;
    if (idx >= SoundDefinitionCount)
    {
        GameDiagnostic(("Invalid sound definition %d\n", sound_id));
        abort();
    }
    const SoundDefinition &sound = sound_definitions[idx];
    return PlaySoundVoices(sound.cfg, sound.note, left_product >> 14, right_product >> 14, pitch_variation);
}

uint32 PlayPositionedSound(sint32 sound_id, const Vector3 *src, uint32 pitch_variation)
{
    uint32 packed;
    sint32 left_gain, right_gain;
    sint16 scale;
    FUNCTION_MARKER(0x80069EF4, "SLUS_003.73");
    if (sound_id < 0)
        return 0;
    packed = CalculateSoundBalance(src, 256, 12000);
    scale = game_settings.effects_volume;
    left_gain = ((packed & 4095) * scale) >> 12;
    right_gain = (((packed >> 16) & 4095) * scale) >> 12;
    if (packed & 0x80000000)
        left_gain = -left_gain;
    // The original word shift discards the upper three ID bits
    uint32 idx = sound_id & 0x1FFFFFFF;
    if (idx >= SoundDefinitionCount)
    {
        GameDiagnostic(("Invalid sound definition %d\n", sound_id));
        abort();
    }
    const SoundDefinition &sound = sound_definitions[idx];
    return PlaySoundVoices(sound.cfg, sound.note, left_gain, right_gain, pitch_variation);
}

uint32 CalculateSoundBalance(const Vector3 *origin, sint32 minimum, sint32 maximum)
{
    Camera *listener;
    uint32 numerator, weight, angle, flag = 0, delta;
    Vector3 pos;
    uint32 product, low, high;
    sint32 distance, orientation;
    FUNCTION_MARKER(0x80067E9C, "SLUS_003.73");
    listener = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    if (!listener)
        return 0;
    pos = listener->model.pos;
    distance = origin->Distance(pos);
    if (minimum >= distance)
        return 0x0FFF0FFF;
    if (distance >= maximum)
        return 0;
    delta = (uint32)maximum - distance;
    numerator = (delta << 12) - delta;
    listener = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    orientation = listener->model.angles.x - 1024;
    weight = DivideSignedWord(numerator, maximum);
    if (orientation < 0)
        orientation = -orientation;
    if (orientation < 64)
        return weight | (weight << 16);
    angle = ratan2((uint32)origin->z - pos.z, (uint32)origin->x - pos.x);
    listener = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    delta = (uint16)listener->angles.y;
    angle = (1024 - angle - (delta - 2048)) & 4095;
    if (angle - 1025 < 2047)
    {
        flag = 0x80000000;
        weight -= (sint32)weight >> 4;
    }
    product = weight * rsin(angle);
    product = (sint32)product >> 12;
    if (angle < 2048)
    {
        high = weight;
        low = weight - product;
    }
    else
    {
        low = weight;
        high = weight + product;
    }
    return low | (high << 16) | flag;
}

uint32 SetSoundReverbMode(uint8 mode)
{
    uint32 result;
    FUNCTION_MARKER(0x8006A334, "SLUS_003.73");
    result = SsUtSetReverbType(mode);
    if (mode)
    {
        SpuSetReverb(1);
        result = 240;
        audio_playback.reverb_ticks = result;
    }
    else
        audio_playback.reverb_ticks = 0;
    return result;
}

uint32 EnableSoundReverb()
{
    FUNCTION_MARKER(0x8006A384, "SLUS_003.73");
    return SpuSetReverb(1);
}

uint32 DisableSoundReverb()
{
    FUNCTION_MARKER(0x8006A3A4, "SLUS_003.73");
    return SetSoundReverbMode(0);
}

uint32 MarkMusicRestorePending()
{
    FUNCTION_MARKER(0x8006A0EC, "SLUS_003.73");
    audio_playback.restore_pending = true;
    return 1;
}

uint32 ResetMusicPlaybackState()
{
    FUNCTION_MARKER(0x8006A0FC, "SLUS_003.73");
    audio_playback.suspended = false;
    audio_playback.resume_pending = false;
    audio_playback.start_pending = true;
    return 1;
}

uint32 StopMusicSequence()
{
    FUNCTION_MARKER(0x8006A3C4, "SLUS_003.73");
    RequestSequenceStop(audio_playback.sequence, 0);
    audio_playback.resume_pending = true;
    audio_playback.start_pending = false;
    return 1;
}

uint32 StopMusicPlayback()
{
    FUNCTION_MARKER(0x8006A3F0, "SLUS_003.73");
    if (audio_playback.sequence != -1)
        StopSequenceImmediately(audio_playback.sequence, 0);
    audio_playback.start_pending = true;
    audio_playback.resume_pending = false;
    return 1;
}

uint32 RestoreMusicPlayback()
{
    sint16 sequence, volume;
    uint32 result;
    FUNCTION_MARKER(0x8006A428, "SLUS_003.73");
    if (audio_playback.resume_pending)
    {
        sequence = audio_playback.sequence;
        RequestSequenceReplay(sequence, 0);
    }
    else if (audio_playback.start_pending)
    {
        sequence = audio_playback.sequence;
        PlaySequence(sequence, 1, 0);
    }
    volume = game_settings.music_volume;
    sequence = audio_playback.sequence;
    result = SetSequenceVolumes(sequence, volume, volume);
    audio_playback.resume_pending = false;
    audio_playback.start_pending = false;
    return result;
}

uint32 SetSoundReverbDepth(uint8 left, uint8 right)
{
    FUNCTION_MARKER(0x8006A4A0, "SLUS_003.73");
    SsUtSetReverbDepth(left, right);
    return 0;
}

static void ReadSavedVoiceVolumes(sint16 voice, sint16 &left_output, sint16 &right_output)
{
    SpuVoiceAttr attributes = {0};
    sint32 left, right;
    if (voice < 0 || voice >= 24)
        return;
    attributes.voice = 1 << voice;
    SpuGetVoiceAttr(&attributes);
    left = (uint16)attributes.volume.left;
    right = (uint16)attributes.volume.right;
    if (left >= 0x4000)
        left -= 0x8000;
    left_output = left;
    if (right >= 0x4000)
        right -= 0x8000;
    right_output = right;
}

uint32 SuspendAudioPlayback()
{
    SndRegisterAttr attributes = {0};
    FUNCTION_MARKER(0x8006A114, "SLUS_003.73");
    if (audio_playback.suspended)
        return audio_playback.suspended;
    audio_playback.suspended = true;
    attributes.volume.right = 0;
    attributes.volume.left = 0;
    attributes.mask = 3;
    for (uint32 voice = 0; voice < 24; ++voice)
    {
        ReadSavedVoiceVolumes(voice, audio_playback.saved_left[voice], audio_playback.saved_right[voice]);
        SsQueueRegisters(voice, &attributes);
    }
    if (audio_playback.resume_pending)
        audio_playback.restore_pending = true;
    else
    {
        StopMusicSequence();
        audio_playback.restore_pending = false;
    }
    return FastControlDataCd(CdlMute, 0);
}

uint32 ResumeAudioPlayback()
{
    sint16 left, right;
    SndRegisterAttr attributes = {0};
    FUNCTION_MARKER(0x8006A1DC, "SLUS_003.73");
    if (!audio_playback.suspended)
        return audio_playback.suspended;
    audio_playback.suspended = false;
    attributes.mask = 3;
    for (uint32 voice = 0; voice < 24; ++voice)
    {
        left = audio_playback.saved_left[voice];
        right = audio_playback.saved_right[voice];
        if (left || right)
        {
            attributes.volume.left = left;
            attributes.volume.right = audio_playback.saved_right[voice];
            SsQueueRegisters(voice, &attributes);
        }
    }
    if (!audio_playback.restore_pending)
        RestoreMusicPlayback();
    return FastControlDataCd(CdlDemute, 0);
}

uint32 FlushPendingPositionedSound(void)
{
    FUNCTION_MARKER(0x80022860, "SLUS_003.73");
    uint32 result = positioned_sound_event.pending;
    if (result)
        result = PlayPositionedSound(3, &positioned_sound_event.pos, 0);
    positioned_sound_event.pending = false;
    return result;
}

uint32 SwitchMenuAudioSequence(const char *basename)
{
    FUNCTION_MARKER(0x80015F48, "SLUS_003.73");
    StopMusicPlayback();
    ReleaseAudioSession();
    return LoadAudioSequence(basename);
}
