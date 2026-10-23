#include <stdlib.h>

#include "game_diagnostics.h"
#include "game_settings.h"
#include "global.h"

extern "C"
{
#include "psx_spu.h"
}
#include "audio_voice.h"
#include "audio_session.h"

static uint16 voice_base_pitch[24];

void ResetSoundVoicePitches()
{
    for (uint32 voice = 0; voice < 24; ++voice)
        voice_base_pitch[voice] = 0;
}

uint32 PlaySoundVoices(uint32 cfg, uint8 note, sint16 left, sint16 right, uint32 pitch_variation)
{
    uint8 program = (cfg >> 16) & 127;
    uint16 remaining = cfg;
    uint8 voice_count = 1;
    uint32 limit, scan, next_tone = 0;
    uint32 random_offset = pitch_variation;
    uint16 pitch;
    sint16 bank, vag, effective_note;
    sint32 voices;
    SndVoiceStats settings = {0};
    SndRegisterAttr registers = {0};
    VagAtr tone_attributes = {0};
    FUNCTION_MARKER(0x8006A4C4, "SLUS_003.73");
    // Voice metadata consumes zero volume and pan while register writes carry output gains
    if (pitch_variation)
        random_offset = RandomRange(pitch_variation << 1) - pitch_variation;
    if (remaining >= 2)
    {
        voice_count = 0;
        for (uint32 tone = 0; tone < 16; ++tone)
            voice_count += (remaining >> tone) & 1;
    }
    if (spu_vm_state()->events.voice_count > 24)
    {
        GameDiagnostic(("Invalid sound voice capacity\n"));
        abort();
    }
    SsBlockVoiceAllocation();
    voices = SsAllocateVoices(voice_count, cfg >> 31 ? 127 : 0);
    if (voices > 0)
    {
        bank = cfg & 0x40000000 ? sound_banks.level : sound_banks.common;
        // Original GTE leading-zero result determines the highest allocated voice
        limit = 0;
        scan = voices;
        while (scan)
        {
            ++limit;
            scan >>= 1;
        }
        if (limit > 24)
        {
            GameDiagnostic(("Sound allocator returned voices outside the 24-voice range\n"));
            abort();
        }
        settings.vabId = bank;
        settings.prog_num = program;
        settings.prog_actual = SsGetActualProgFromProg(bank, program);
        effective_note = note + random_offset;
        registers.volume.left = left;
        registers.volume.right = right;
        registers.mask = 0;
        for (uint32 voice = 0; voice < limit && remaining; ++voice)
        {
            if ((voices >> voice) & 1)
            {
                for (uint32 tone = next_tone; tone < 16; ++tone)
                {
                    if ((remaining >> tone) & 1)
                    {
                        next_tone = tone + 1;
                        SsUtGetVagAtr(bank, program, tone, &tone_attributes);
                        vag = tone_attributes.vag;
                        settings.vagId = vag;
                        pitch = SsPitchFromNote(effective_note, 0, tone_attributes.center, tone_attributes.shift);
                        settings.pitch = pitch;
                        voice_base_pitch[voice] = pitch;
                        settings.tone = tone;
                        settings.note = tone_attributes.center;
                        SsSetVoiceSettings(voice, &settings);
                        registers.pitch = settings.pitch;
                        registers.addr = SsUtGetVagAddr(bank, vag) >> 3;
                        registers.adsr1 = tone_attributes.adsr1;
                        registers.adsr2 = tone_attributes.adsr2;
                        SsQueueRegisters(voice, &registers);
                        remaining &= ~(1 << tone);
                        break;
                    }
                }
            }
        }
        SpuVoiceEventState *events = &spu_vm_state()->events;
        uint8 count = events->voice_count;
        if (count > 24)
        {
            GameDiagnostic(("Invalid sound event voice capacity\n"));
            abort();
        }
        for (uint32 voice = 0; voice < count; ++voice)
        {
            uint32 bit = 1 << voice;
            if (voices & bit)
            {
                uint32 half = voice >= 16 ? 1 : 0;
                uint16 half_bit = 1 << (voice >= 16 ? voice - 16 : voice);
                events->key_on[half] |= half_bit;
                events->key_off[half] &= ~half_bit;
            }
        }
        for (uint32 voice = 0; voice < 24; ++voice)
        {
            uint32 bit = 1 << voice;
            if (voices & bit)
            {
                uint16 *mask = events->reverb + (voice >= 16 ? 1 : 0);
                uint32 half_bit = 1 << (voice >= 16 ? voice - 16 : voice);
                *mask |= half_bit;
            }
        }
    }
    SsUnBlockVoiceAllocation();
    return voices;
}

static void QueueSoundVoiceOff(sint16 voice)
{
    if (spu_sequence_scheduler.processing == 1)
        return;
    spu_sequence_scheduler.processing = 1;
    if (voice >= 0 && voice < 24)
    {
        spu_vm_state()->note.voice = voice;
        spu_sequence_vm_key_off_now();
    }
    spu_sequence_scheduler.processing = 0;
}

void StopSoundVoices(uint32 mask)
{
    FUNCTION_MARKER(0x8006A294, "SLUS_003.73");
    if (!mask)
        return;
    for (uint32 voice = 0; voice < 24; ++voice)
        if (mask & (1 << voice))
            QueueSoundVoiceOff(voice);
}

void UpdatePositionedSoundVoices(uint32 voices, const Vector3 *origin, sint32 pitch_delta)
{
    uint32 balance;
    sint32 left, right, base_pitch, pitch, cap, difference, product;
    sint16 gain;
    SndRegisterAttr attributes = {0};
    FUNCTION_MARKER(0x80069FAC, "SLUS_003.73");
    if (!voices)
        return;
    balance = CalculateSoundBalance(origin, 256, 12000);
    gain = game_settings.effects_volume;
    left = ((balance & 4095) * gain) >> 12;
    right = (((balance >> 16) & 4095) * gain) >> 12;
    if (balance & 0x80000000)
        left = -left;
    attributes.volume.left = left;
    attributes.volume.right = right;
    attributes.mask = pitch_delta ? 35 : 3;
    for (uint32 voice = 0; voice < 24; ++voice)
    {
        if (voices & (1 << voice))
        {
            if (pitch_delta)
            {
                base_pitch = voice_base_pitch[voice];
                product = (uint32)base_pitch * pitch_delta;
                // Q13 correction truncates toward zero after the low-word product
                pitch = base_pitch - product / 8192;
                if (base_pitch < pitch)
                {
                    difference = pitch - base_pitch;
                    cap = base_pitch + (base_pitch >> 1);
                    if (cap < difference)
                        pitch = cap;
                }
                attributes.pitch = pitch;
            }
            SsQueueRegisters(voice, &attributes);
        }
    }
}
