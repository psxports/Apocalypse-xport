#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
uint32 sub_8008E93C(uint32 serial, uint32 attribute, uint32 value);
uint32 sub_8008F05C(uint32 serial, uint32 left, uint32 right);
#include <stdlib.h>
#include <stdio.h>
}
#include "audio_session.h"
#include "sequence_transport.h"
#include "audio_voice.h"
#include "resource_io.h"
#include "game_heap.h"
#include "vector_distance.h"
#include "native_object_storage.h"
#include "psx_spu.h"
#include <string.h>

uint32 InitializeAudioOutput()
{
    FUNCTION_MARKER(0x800698FC, "SLUS_003.73");
    sub_8008E10C();
    sub_8008E93C(0, 0, 1);
    sub_8008E93C(0, 1, 1);
    return sub_8008F05C(0, 127, 127);
}

uint32 InitializeAudioSession(uint32 resource)
{
    uint32 bank;
    FUNCTION_MARKER(0x8006994C, "SLUS_003.73");
    SetSoundReverbMode(0);
    bank = LoadSoundBank((const char *)psx_addr(resource, 1), 0x800FF6D0);
    w_u32(0x800FF69C, bank);
    sub_8008F16C(0x80100688, 1, 1);
    // Preserve original SDK callback identities in the guest callback table
    w_u32(0x801044F0, 0x8008CDFC);
    w_u32(0x801044F4, 0x8008CEDC);
    w_u32(0x801044F8, 0x8008CB1C);
    w_u32(0x80104500, 0x8008CBCC);
    w_u32(0x8010450C, 0x8008C8CC);
    w_u32(0x80104510, 0x8008C99C);
    w_u32(0x801044FC, 0);
    w_u32(0x80104504, 0);
    w_u32(0x80104514, 0);
    w_u32(0x80104518, 0x8008CA6C);
    w_u32(0x8010451C, 0);
    w_u32(0x80104520, 0);
    w_u32(0x80104524, 0);
    w_u32(0x80104528, 0);
    w_u32(0x8010452C, 0);
    w_u32(0x80104530, 0);
    w_u32(0x80104508, 0);
    for (uint32 address = 0x80104534; address <= 0x80104580; address += 4)
        w_u32(address, 0);
    SsSetTickMode(4096);
    sub_800936AC(24);
    sub_8008EC4C();
    w_u32(0x800FF6B4, 0);
    w_u32(0x800FF6B8, 1);
    return 1;
}

uint32 ReleaseAudioSession()
{
    uint32 result, buffer, bank, body;
    FUNCTION_MARKER(0x80069A94, "SLUS_003.73");
    result = r_u32(0x800FF6AC);
    if ((sint32)result >= 0)
    {
        sub_8008DF18((uint32)(sint32)(sint16)r_u16(0x800FF6AC));
        result = 0xFFFFFFFF;
        w_u32(0x800FF6AC, result);
    }
    buffer = r_u32(0x800FF6A8);
    if (buffer)
    {
        result = sub_8006BC20(buffer);
        w_u32(0x800FF6A8, 0);
    }
    bank = r_u32(0x800FF6A4);
    if ((sint32)bank >= 0)
    {
        body = r_u32(0x800FF6D8);
        ReleaseSoundBank((sint16)bank, body);
        result = 0xFFFFFFFF;
        w_u32(0x800FF6A4, result);
        w_u32(0x800FF6D8, 0);
    }
    return result;
}

extern "C" uint32 sub_800698FC(void)
{
    FUNCTION_MARKER(0x800698FC, "SLUS_003.73");
    return InitializeAudioOutput();
}
extern "C" uint32 sub_8006994C(uint32 resource)
{
    FUNCTION_MARKER(0x8006994C, "SLUS_003.73");
    return InitializeAudioSession(resource);
}
extern "C" uint32 sub_80069A94(void)
{
    FUNCTION_MARKER(0x80069A94, "SLUS_003.73");
    return ReleaseAudioSession();
}

uint32 ReleaseSoundBank(sint16 bank, uint32 header)
{
    FUNCTION_MARKER(0x80069CF8, "SLUS_003.73");
    SsVabClose(bank);
    if ((uint16)bank < 16 && r_u8(0x80105640 + bank) == 1)
    {
        w_u8(0x80105640 + bank, 0);
        w_u16(0x80105698, r_u16(0x80105698) - 1);
    }
    return sub_8006BC20(header);
}

// Native VAB storage and the translated voice selector share these original tables
static void PublishSoundBank(uint32 header, sint16 bank)
{
    sint32 body = spu_native_vab_address(bank);
    uint32 programs = header + 32, limit = 64, actual = 0, index, tones, sizes, accumulated = 0;
    uint32 samples = r_u8(header + 22);
    if (bank < 0 || bank >= 16 || body < 0)
    {
        GameDiagnostic("Cannot publish native sound bank %d\n", (int)bank);
        abort();
    }
    if ((r_u32(header) & 255) == 112 && (sint32)r_u32(header + 4) >= 5)
        limit = 128;
    tones = programs + limit * 16;
    sizes = tones + ((uint32)r_u16(header + 18) << 9);
    w_u32(0x80105580 + bank * 4, header);
    w_u32(0x80105540 + bank * 4, programs);
    w_u32(0x801055C8 + bank * 4, tones);
    w_u16(0x8010560A, limit);
    for (index = 0; index < limit; ++index)
    {
        uint32 entry = programs + index * 16;
        w_u32(entry + 8, actual);
        if (r_u8(entry))
            ++actual;
    }
    for (index = 0; index <= samples; ++index)
    {
        uint32 size = r_u16(sizes + index * 2);
        size <<= (sint32)r_u32(header + 4) >= 5 ? 3 : 2;
        accumulated += size;
        w_u16(programs + (index / 2) * 16 + ((index & 1) ? 14 : 12), ((uint32)body + accumulated) >> 3);
    }
    w_u32(0x801056A0 + bank * 4, (uint32)body);
    w_u32(0x80105658 + bank * 4, accumulated);
    w_u16(0x80105698, r_u16(0x80105698) + 1);
    w_u8(0x80105640 + bank, 1);
}

uint32 LoadSoundBank(const char *name, uint32 header_output)
{
    uint32 file_size, header, header_bytes, total_bytes;
    sint16 bank;
    FUNCTION_MARKER(0x80069D2C, "SLUS_003.73");
    file_size = LookupResource(name);
    header = sub_8006B864(file_size + 64, 1, 1);
    BeginResourceRead(header);
    WaitForResourceRead();
    header_bytes = (uint32)r_u16(header + 18) << 9;
    total_bytes = r_u32(header + 12);
    header_bytes += 0xA20;
    total_bytes = header_bytes + ((total_bytes - header_bytes + 63) & 0xFFFFFFC0);
    w_u32(header + 12, total_bytes);
    bank = SsVabOpenHead((uint8 *)psx_addr(header, header_bytes), -1);
    SsVabTransBody((uint8 *)psx_addr(header + header_bytes, total_bytes - header_bytes), bank);
    SsVabTransCompleted(1);
    PublishSoundBank(header, bank);
    ResizeHeapBlock(header, header_bytes);
    w_u32(header_output, header);
    return (uint32)(sint32)bank;
}

extern "C" uint32 sub_80069CF8(uint32 bank, uint32 header)
{
    FUNCTION_MARKER(0x80069CF8, "SLUS_003.73");
    return ReleaseSoundBank((sint16)bank, header);
}
extern "C" uint32 sub_80069D2C(uint32 name, uint32 header_output)
{
    FUNCTION_MARKER(0x80069D2C, "SLUS_003.73");
    return LoadSoundBank((const char *)psx_addr(name, 1), header_output);
}
extern "C" uint32 apocalypse_load_sound_bank(const char *name, uint32 header_output) { return LoadSoundBank(name, header_output); }
extern "C" uint32 xport_draft_host_sub_80069D2C_p1(void *name, uint32 header_output)
{
    FUNCTION_MARKER(0x80069D2C, "SLUS_003.73");
    return LoadSoundBank((const char *)name, header_output);
}

uint32 LoadLevelSoundBank(uint32 basename)
{
    char filename[24];
    uint32 index = 0, character, bank;
    FUNCTION_MARKER(0x80069B0C, "SLUS_003.73");
    // TODO Original basename must fit the local filename buffer
    for (;;)
    {
        character = r_u8(basename);
        ++basename;
        if (!character)
            break;
        filename[index++] = (char)character;
    }
    filename[index++] = '.';
    filename[index++] = 'V';
    filename[index++] = 'A';
    filename[index++] = 'B';
    filename[index] = 0;
    bank = LoadSoundBank(filename, 0x800FF6D4);
    w_u32(0x800FF6A0, bank);
    return bank;
}

void ReleaseLevelSoundBank()
{
    uint32 bank, header;
    FUNCTION_MARKER(0x80069B8C, "SLUS_003.73");
    bank = r_u32(0x800FF6A0);
    if ((sint32)bank >= 0)
    {
        header = r_u32(0x800FF6D4);
        ReleaseSoundBank((sint16)bank, header);
        w_u32(0x800FF6A0, 0xFFFFFFFF);
        w_u32(0x800FF6D4, 0);
    }
}

uint32 LoadAudioSequence(uint32 basename)
{
    char filename[24];
    uint32 index = 0, character, bank, file_size, sequence, result, tempo;
    FUNCTION_MARKER(0x80069BC4, "SLUS_003.73");
    for (;;)
    {
        character = r_u8(basename);
        if (!character)
            break;
        // TODO Original filenames must fit the addressed local buffer
        if (index >= sizeof(filename) - 5)
        {
            GameDiagnostic("Audio sequence filename exceeds the local buffer\n");
            abort();
        }
        filename[index++] = (char)character;
        ++basename;
    }
    filename[index] = '.';
    filename[index + 1] = 'V';
    filename[index + 2] = 'A';
    filename[index + 3] = 'B';
    filename[index + 4] = 0;
    bank = LoadSoundBank(filename, 0x800FF6D8);
    w_u32(0x800FF6A4, bank);
    filename[index + 1] = 'S';
    filename[index + 2] = 'E';
    filename[index + 3] = 'Q';
    file_size = LookupResource(filename);
    sequence = sub_8006B864(file_size, 1, 1);
    w_u32(0x800FF6A8, sequence);
    BeginResourceRead(sequence);
    WaitForResourceRead();
    sequence = r_u32(0x800FF6A8);
    bank = (uint32)(sint32)(sint16)r_u16(0x800FF6A4);
    result = (uint32)(sint32)(sint16)apocalypse_sequence_open(sequence, bank);
    sequence = r_u32(0x800FF6A8);
    w_u32(0x800FF6AC, result);
    tempo = r_u8(sequence + 10);
    w_u32(0x800FF6C8, tempo);
    tempo += (uint32)r_u8(sequence + 11) << 8;
    w_u32(0x800FF6C8, tempo);
    result = (uint32)r_u8(sequence + 12) << 16;
    w_u32(0x800FF6C8, tempo + result);
    return result;
}

extern "C" uint32 sub_80069B0C(uint32 basename)
{
    FUNCTION_MARKER(0x80069B0C, "SLUS_003.73");
    return LoadLevelSoundBank(basename);
}
extern "C" void sub_80069B8C(void)
{
    FUNCTION_MARKER(0x80069B8C, "SLUS_003.73");
    ReleaseLevelSoundBank();
}
extern "C" uint32 sub_80069BC4(uint32 basename)
{
    FUNCTION_MARKER(0x80069BC4, "SLUS_003.73");
    return LoadAudioSequence(basename);
}

uint32 PlaySoundEffect(sint32 sound_id, sint16 volume, uint32 pitch_variation)
{
    uint32 product, descriptor, configuration;
    sint16 scaled;
    uint8 note;
    FUNCTION_MARKER(0x80069DF0, "SLUS_003.73");
    if (sound_id < 0)
        return 0;
    product = (uint32)(sint32)volume * (uint32)(sint32)(sint16)r_u16(0x800ECC7A);
    descriptor = 0x800E53A8 + (uint32)sound_id * 8;
    note = r_u8(descriptor + 4);
    configuration = r_u32(descriptor);
    scaled = (sint16)((sint32)(product << 2) >> 16);
    return PlaySoundVoices(configuration, note, scaled, scaled, pitch_variation);
}

extern "C" uint32 sub_80069DF0(uint32 sound_id, uint32 volume, uint32 pitch_variation)
{
    FUNCTION_MARKER(0x80069DF0, "SLUS_003.73");
    return PlaySoundEffect((sint32)sound_id, (sint16)volume, pitch_variation);
}

uint32 PlayScaledSound(sint32 sound_id, sint16 left, sint16 right, uint32 pitch_variation)
{
    uint32 scale, left_product, right_product, program, note, entry, left_volume, right_volume;
    FUNCTION_MARKER(0x80069E68, "SLUS_003.73");
    if (sound_id < 0)
        return 0;
    scale = (uint32)(sint32)(sint16)r_u16(0x800ECC7A);
    left_product = (uint32)(sint32)left * scale;
    right_product = (uint32)(sint32)right * scale;
    entry = 0x800E53A8 + ((uint32)sound_id << 3);
    note = r_u8(0x800E53A8 + (((uint32)sound_id << 3) | 4));
    left_volume = (uint32)((sint32)(left_product << 2) >> 16);
    program = r_u32(entry);
    right_volume = (uint32)((sint32)(right_product << 2) >> 16);
    return PlaySoundVoices(program, (uint8)note, left_volume, right_volume, pitch_variation);
}

uint32 PlayPositionedSound(sint32 sound_id, const uint32 *source, uint32 pitch_variation)
{
    uint32 packed, scale, left, right, note, program, entry;
    FUNCTION_MARKER(0x80069EF4, "SLUS_003.73");
    if (sound_id < 0)
        return 0;
    packed = CalculateSoundBalance(source, 256, 12000);
    scale = (uint32)(sint32)(sint16)r_u16(0x800ECC7A);
    left = ((packed & 4095) * scale) >> 12;
    right = (((packed >> 16) & 4095) * scale) >> 12;
    if ((sint32)packed < 0)
        left = 0 - left;
    entry = 0x800E53A8 + ((uint32)sound_id << 3);
    note = r_u8(0x800E53A8 + (((uint32)sound_id << 3) | 4));
    left = (uint32)(sint32)(sint16)left;
    program = r_u32(entry);
    right = (uint32)(sint32)(sint16)right;
    return PlaySoundVoices(program, (uint8)note, left, right, pitch_variation);
}

extern "C" uint32 sub_80069E68(uint32 sound_id, uint32 left, uint32 right, uint32 pitch_variation)
{
    FUNCTION_MARKER(0x80069E68, "SLUS_003.73");
    return PlayScaledSound((sint32)sound_id, (sint16)left, (sint16)right, pitch_variation);
}
extern "C" uint32 sub_80069EF4(uint32 sound_id, uint32 source, uint32 pitch_variation)
{
    FUNCTION_MARKER(0x80069EF4, "SLUS_003.73");
    return PlayPositionedSound((sint32)sound_id, (const uint32 *)psx_addr(source, 12), pitch_variation);
}

extern "C" uint32 xport_draft_host_sub_80069EF4_p2(uint32 sound, const void *position, uint32 parameter)
{
    FUNCTION_MARKER(0x80069EF4, "SLUS_003.73");
    return PlayPositionedSound((sint32)sound, (const uint32 *)position, parameter);
}

static uint32 ReadSoundPositionWord(const uint32 *origin, uint32 mapped, uint32 axis)
{
    uint32 value;
    if (mapped)
        return r_u32(mapped + axis * 4);
    memcpy(&value, (const uint8 *)origin + axis * 4, 4);
    return value;
}

uint32 CalculateSoundBalance(const uint32 *origin, sint32 minimum, sint32 maximum)
{
    uint32 listener, position[3], numerator, weight, angle, flag = 0, delta, magnitude;
    uint32 mapped, origin_z, origin_x, product, low, high;
    sint32 distance;
    FUNCTION_MARKER(0x80067E9C, "SLUS_003.73");
    listener = r_u32(0x800FF904);
    if (!listener)
        return 0;
    position[0] = r_u32(listener + 4);
    position[1] = r_u32(listener + 8);
    position[2] = r_u32(listener + 12);
    distance = (sint32)MeasurePositionDistance(origin, position);
    if (minimum >= distance)
        return 0x0FFF0FFF;
    if (distance >= maximum)
        return 0;
    delta = (uint32)maximum - (uint32)distance;
    numerator = (delta << 12) - delta;
    listener = r_u32(0x800FF904);
    delta = (uint32)(sint32)(sint16)r_u16(listener + 16);
    if (!maximum)
        weight = (sint32)numerator < 0 ? 1 : 0xFFFFFFFF;
    else if (numerator == 0x80000000 && maximum == -1)
        weight = numerator;
    else
        weight = (uint32)((sint32)numerator / maximum);
    delta -= 1024;
    magnitude = (delta ^ (uint32)((sint32)delta >> 31)) - (uint32)((sint32)delta >> 31);
    if ((sint32)magnitude < 64)
        return weight | (weight << 16);
    mapped = GuestMemoryAddress(origin);
    origin_z = ReadSoundPositionWord(origin, mapped, 2);
    origin_x = ReadSoundPositionWord(origin, mapped, 0);
    angle = (uint32)ratan2((sint32)(origin_z - position[2]), (sint32)(origin_x - position[0]));
    listener = r_u32(0x800FF904);
    delta = r_u16(listener + 494);
    angle = (1024 - angle - (delta - 2048)) & 4095;
    if (angle - 1025 < 2047)
    {
        flag = 0x80000000;
        weight -= (uint32)((sint32)weight >> 4);
    }
    product = weight * (uint32)(sint32)(sint16)r_u16(0x800F863C + angle * 4);
    product = (uint32)((sint32)product >> 12);
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

extern "C" uint32 apocalypse_sound_balance_native(const uint32 origin[3], uint32 minimum, uint32 maximum) { return CalculateSoundBalance(origin, (sint32)minimum, (sint32)maximum); }
extern "C" uint32 sub_80067E9C(uint32 origin, uint32 minimum, uint32 maximum)
{
    FUNCTION_MARKER(0x80067E9C, "SLUS_003.73");
    return CalculateSoundBalance((const uint32 *)psx_addr(origin, 12), (sint32)minimum, (sint32)maximum);
}

uint32 SetSoundReverbMode(uint8 mode)
{
    uint32 result;
    FUNCTION_MARKER(0x8006A334, "SLUS_003.73");
    result = sub_8009013C(mode);
    if (mode)
    {
        sub_800901EC();
        result = 240;
        w_u32(0x800FF6CC, result);
    }
    else
        w_u32(0x800FF6CC, 0);
    return result;
}

uint32 EnableSoundReverb()
{
    FUNCTION_MARKER(0x8006A384, "SLUS_003.73");
    return sub_800901EC();
}

uint32 DisableSoundReverb()
{
    FUNCTION_MARKER(0x8006A3A4, "SLUS_003.73");
    return SetSoundReverbMode(0);
}

extern "C" uint32 sub_8006A334(uint32 mode)
{
    FUNCTION_MARKER(0x8006A334, "SLUS_003.73");
    return SetSoundReverbMode((uint8)mode);
}
extern "C" uint32 sub_8006A384(void)
{
    FUNCTION_MARKER(0x8006A384, "SLUS_003.73");
    return EnableSoundReverb();
}
extern "C" uint32 sub_8006A3A4(void)
{
    FUNCTION_MARKER(0x8006A3A4, "SLUS_003.73");
    return DisableSoundReverb();
}

uint32 MarkMusicRestorePending()
{
    FUNCTION_MARKER(0x8006A0EC, "SLUS_003.73");
    w_u32(0x800FFC20, 1);
    return 1;
}

uint32 ResetMusicPlaybackState()
{
    FUNCTION_MARKER(0x8006A0FC, "SLUS_003.73");
    w_u32(0x800FF6B0, 0);
    w_u32(0x800FF6B4, 0);
    w_u32(0x800FF6B8, 1);
    return 1;
}

uint32 StopMusicPlayback()
{
    FUNCTION_MARKER(0x8006A3F0, "SLUS_003.73");
    if (r_u32(0x800FF6AC) != 0xFFFFFFFF)
    {
        StopSequenceImmediately((uint32)(sint32)(sint16)r_u16(0x800FF6AC), 0);
    }
    w_u32(0x800FF6B8, 1);
    w_u32(0x800FF6B4, 0);
    return 1;
}

extern "C" uint32 sub_8006A3F0(void)
{
    FUNCTION_MARKER(0x8006A3F0, "SLUS_003.73");
    return StopMusicPlayback();
}

uint32 RestoreMusicPlayback()
{
    uint32 sequence, volume, result;
    FUNCTION_MARKER(0x8006A428, "SLUS_003.73");
    if (r_u32(0x800FF6B4))
    {
        sequence = (uint32)(sint32)(sint16)r_u16(0x800FF6AC);
        apocalypse_sequence_replay(sequence);
    }
    else if (r_u32(0x800FF6B8))
    {
        sequence = (uint32)(sint32)(sint16)r_u16(0x800FF6AC);
        apocalypse_sequence_play(sequence, 1, 0);
    }
    volume = (uint32)(sint32)(sint16)r_u16(0x800ECC78);
    sequence = (uint32)(sint32)(sint16)r_u16(0x800FF6AC);
    result = apocalypse_sequence_volume(sequence, volume, volume);
    w_u32(0x800FF6B4, 0);
    w_u32(0x800FF6B8, 0);
    return result;
}

uint32 SetSoundReverbDepth(uint8 left, uint8 right)
{
    FUNCTION_MARKER(0x8006A4A0, "SLUS_003.73");
    return sub_800900AC(left, right);
}

extern "C" uint32 sub_8006A0EC(void)
{
    FUNCTION_MARKER(0x8006A0EC, "SLUS_003.73");
    return MarkMusicRestorePending();
}
extern "C" uint32 sub_8006A0FC(void)
{
    FUNCTION_MARKER(0x8006A0FC, "SLUS_003.73");
    return ResetMusicPlaybackState();
}
extern "C" uint32 sub_8006A428(void)
{
    FUNCTION_MARKER(0x8006A428, "SLUS_003.73");
    return RestoreMusicPlayback();
}
extern "C" uint32 sub_8006A4A0(uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x8006A4A0, "SLUS_003.73");
    return SetSoundReverbDepth((uint8)left, (uint8)right);
}

static uint32 ReadSavedVoiceVolumes(sint16 voice, uint32 left_output, uint32 right_output)
{
    SpuVoiceAttr attributes = {0};
    uint32 left, right;
    if ((uint16)voice >= 24)
        return 0xFFFFFFFF;
    attributes.voice = (uint32)1 << voice;
    SpuGetVoiceAttr(&attributes);
    left = (uint16)attributes.volume.left;
    right = (uint16)attributes.volume.right;
    if (left >= 0x4000)
        left -= 0x8000;
    w_u16(left_output, left);
    if (right >= 0x4000)
        right -= 0x8000;
    w_u16(right_output, right);
    return 0;
}

uint32 SuspendAudioPlayback()
{
    uint32 pending, left_output = 0x800E5BA8, right_output = 0x800E5BD8;
    sint16 attributes[8] = {0};
    FUNCTION_MARKER(0x8006A114, "SLUS_003.73");
    pending = r_u32(0x800FF6B0);
    if (pending)
        return pending;
    w_u32(0x800FF6B0, 1);
    attributes[1] = 0;
    attributes[0] = 0;
    attributes[3] = 3;
    for (uint32 voice = 0; voice < 24; ++voice)
    {
        ReadSavedVoiceVolumes((sint16)voice, left_output, right_output);
        SsQueueRegisters((sint32)voice, (SndRegisterAttr *)attributes);
        right_output += 2;
        left_output += 2;
    }
    if (r_u32(0x800FF6B4))
        w_u32(0x800FFC20, 1);
    else
    {
        sub_8006A3C4();
        w_u32(0x800FFC20, 0);
    }
    return sub_80097F34(11, 0);
}

uint32 ResumeAudioPlayback()
{
    uint32 pending, left, right, left_input = 0x800E5BA8, right_input = 0x800E5BD8;
    sint16 attributes[8] = {0};
    FUNCTION_MARKER(0x8006A1DC, "SLUS_003.73");
    pending = r_u32(0x800FF6B0);
    if (!pending)
        return pending;
    w_u32(0x800FF6B0, 0);
    attributes[3] = 3;
    for (uint32 voice = 0; voice < 24; ++voice)
    {
        left = r_u16(left_input);
        right = r_u16(right_input);
        if (left | right)
        {
            attributes[0] = (sint16)left;
            attributes[1] = (sint16)r_u16(right_input);
            SsQueueRegisters((sint32)voice, (SndRegisterAttr *)attributes);
        }
        right_input += 2;
        left_input += 2;
    }
    if (!r_u32(0x800FFC20))
        RestoreMusicPlayback();
    return sub_80097F34(12, 0);
}

extern "C" uint32 sub_8006A114(void)
{
    FUNCTION_MARKER(0x8006A114, "SLUS_003.73");
    return SuspendAudioPlayback();
}
extern "C" uint32 sub_8006A1DC(void)
{
    FUNCTION_MARKER(0x8006A1DC, "SLUS_003.73");
    return ResumeAudioPlayback();
}

uint32 FlushPendingPositionedSound(void)
{
    FUNCTION_MARKER(0x80022860, "SLUS_003.73");
    uint32 result = r_u32(0x800FF214);
    if (result)
        result = PlayPositionedSound(3, (const uint32 *)psx_addr(0x800FFD48, 12), 0);
    w_u32(0x800FF214, 0);
    return result;
}

extern "C" uint32 sub_80022860(void)
{
    FUNCTION_MARKER(0x80022860, "SLUS_003.73");
    return FlushPendingPositionedSound();
}


uint32 SwitchMenuAudioSequence(uint32 basename)
{
    FUNCTION_MARKER(0x80015F48, "SLUS_003.73");
    StopMusicPlayback();
    ReleaseAudioSession();
    return LoadAudioSequence(basename);
}

extern "C" uint32 sub_80015F48(uint32 basename)
{
    FUNCTION_MARKER(0x80015F48, "SLUS_003.73");
    return SwitchMenuAudioSequence(basename);
}
