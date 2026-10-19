#include <stdlib.h>

#include "game_diagnostics.h"
#include "game_settings.h"
#include "game_random.h"
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

uint32 PlaySoundVoices(uint32 configuration, uint8 note, sint16 left, sint16 right, uint32 pitch_variation)
{
    uint32 program = (configuration >> 16) & 127, remaining = configuration;
    uint32 voice_count = 1, voices, limit, scan, next_tone = 0;
    uint32 random_offset = pitch_variation, pitch;
    sint32 bank, shifted_note, vag;
    SndVoiceStats settings = {0};
    SndRegisterAttr registers = {0};
    VagAtr tone_attributes = {0};
    FUNCTION_MARKER(0x8006A4C4, "SLUS_003.73");
    // Voice metadata consumes zero volume and pan while register writes carry output gains
    if (pitch_variation)
        random_offset = RandomRange(pitch_variation << 1) - pitch_variation;
    if ((uint16)remaining >= 2)
    {
        voice_count = 0;
        for (uint32 tone = 0; tone < 16; ++tone)
            voice_count += (remaining >> tone) & 1;
    }
    SsBlockVoiceAllocation();
    voices = SsAllocateVoices(voice_count & 255, configuration >> 31 ? 127 : 0);
    if ((sint32)voices > 0)
    {
        bank = configuration & 0x40000000 ? sound_banks.level : sound_banks.common;
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
            // TODO Establish the invalid allocator-mask contract
            GameDiagnostic("Sound allocator returned voices outside the 24-voice range\n");
            abort();
        }
        settings.vabId = (sint16)bank;
        settings.prog_num = (sint16)program;
        settings.prog_actual = (sint16)SsGetActualProgFromProg((uint32)bank, program);
        shifted_note = (sint32)(((uint32)note + random_offset) << 16);
        registers.volume.left = left;
        registers.volume.right = right;
        registers.mask = 0;
        for (uint32 voice = 0; voice < limit && (uint16)remaining; ++voice)
        {
            if ((voices >> voice) & 1)
            {
                for (uint32 tone = next_tone; tone < 16; ++tone)
                {
                    if ((remaining >> tone) & 1)
                    {
                        next_tone = tone + 1;
                        SsUtGetVagAtr((uint32)bank, program, tone, &tone_attributes);
                        vag = tone_attributes.vag;
                        settings.vagId = (sint16)vag;
                        pitch = SsPitchFromNote(shifted_note >> 16, 0, tone_attributes.center, tone_attributes.shift);
                        settings.pitch = pitch;
                        voice_base_pitch[voice] = (uint16)pitch;
                        settings.tone = (sint16)tone;
                        settings.note = tone_attributes.center;
                        SsSetVoiceSettings(voice, &settings);
                        registers.pitch = settings.pitch;
                        registers.addr = (sint16)(SsUtGetVagAddr((uint32)bank, (uint32)vag) >> 3);
                        registers.adsr1 = (sint16)tone_attributes.adsr1;
                        registers.adsr2 = (sint16)tone_attributes.adsr2;
                        SsQueueRegisters((sint32)voice, &registers);
                        remaining &= ~(1 << tone);
                        break;
                    }
                }
            }
        }
        SpuVoiceEventState *events = &spu_vm_state()->events;
        sint32 count = (sint8)events->voice_count;
        for (sint32 voice = 0; voice < count; ++voice)
        {
            uint32 bit = 1u << ((uint32)voice & 31);
            if (voices & bit)
            {
                uint32 half = voice >= 16 ? 1 : 0;
                uint32 half_bit = 1u << ((uint32)(voice >= 16 ? voice - 16 : voice) & 31);
                events->key_on[half] |= (uint16)half_bit;
                events->key_off[half] &= (uint16)~half_bit;
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
    uint32 index = (uint16)voice;
    if (spu_sequence_scheduler.processing == 1)
        return;
    spu_sequence_scheduler.processing = 1;
    if (index < 24)
    {
        spu_vm_state()->note.voice = index;
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
            QueueSoundVoiceOff((sint16)voice);
}

void UpdatePositionedSoundVoices(uint32 voices, const uint32 *origin, sint32 pitch_delta)
{
    uint32 balance, gain, left, right, base_pitch, pitch, cap, difference, product;
    SndRegisterAttr attributes = {0};
    FUNCTION_MARKER(0x80069FAC, "SLUS_003.73");
    if (!voices)
        return;
    balance = CalculateSoundBalance(origin, 256, 12000);
    gain = (uint32)(sint32)(sint16)game_settings.effects_volume;
    left = ((balance & 4095) * gain) >> 12;
    right = (((balance >> 16) & 4095) * gain) >> 12;
    if ((sint32)balance < 0)
        left = 0 - left;
    attributes.volume.left = (sint16)left;
    attributes.volume.right = (sint16)right;
    attributes.mask = pitch_delta ? 35 : 3;
    for (uint32 voice = 0; voice < 24; ++voice)
    {
        if (voices & (1 << voice))
        {
            if (pitch_delta)
            {
                base_pitch = voice_base_pitch[voice];
                product = base_pitch * (uint32)pitch_delta;
                if ((sint32)product < 0)
                    product += 8191;
                product = (uint32)((sint32)product >> 13);
                pitch = base_pitch - product;
                if ((sint32)base_pitch < (sint32)pitch)
                {
                    difference = pitch - base_pitch;
                    cap = base_pitch + (base_pitch >> 1);
                    if ((sint32)cap < (sint32)difference)
                        pitch = cap;
                }
                attributes.pitch = (sint16)pitch;
            }
            SsQueueRegisters((sint32)voice, &attributes);
        }
    }
}
