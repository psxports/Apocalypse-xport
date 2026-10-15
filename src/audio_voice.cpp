extern "C"
{
#include "draft_first_signatures.h"
#include "game_sound_native.h"
}
#include "audio_voice.h"
#include "audio_session.h"

uint32 PlaySoundVoices(uint32 configuration, uint8 note, sint16 left, sint16 right, uint32 pitch_variation)
{
    uint32 program = (configuration >> 16) & 127, remaining = configuration;
    uint32 voice_count = 1, voices, limit, scan, next_tone = 0, pitch_address = 0x80100738;
    uint32 random_offset = pitch_variation, pitch;
    sint32 bank, shifted_note, vag;
    sint16 settings[9] = {0};
    sint16 registers[8] = {0};
    uint8 tone_attributes[32] = {0};
    FUNCTION_MARKER(0x8006A4C4, "SLUS_003.73");
    // TODO Review unused local fields still initialized by the prior native adapter contract
    if (pitch_variation)
        random_offset = sub_80066570(pitch_variation << 1) - pitch_variation;
    if ((uint16)remaining >= 2)
    {
        voice_count = 0;
        for (uint32 tone = 0; tone < 16; ++tone)
            voice_count += (remaining >> tone) & 1;
    }
    sub_80094544();
    voices = sub_8009426C(voice_count & 255, configuration >> 31 ? 127 : 0);
    if ((sint32)voices > 0)
    {
        bank = (sint16)r_u16(configuration & 0x40000000 ? 0x800FF6A0 : 0x800FF69C);
        // Original GTE leading-zero result determines the highest allocated voice
        limit = 0;
        scan = voices;
        while (scan)
        {
            ++limit;
            scan >>= 1;
        }
        settings[1] = (sint16)bank;
        settings[5] = (sint16)program;
        settings[6] = (sint16)apocalypse_sound_actual_program((uint32)bank, program);
        shifted_note = (sint32)(((uint32)note + random_offset) << 16);
        registers[0] = left;
        registers[1] = right;
        registers[3] = 0;
        for (uint32 voice = 0; voice < limit && (uint16)remaining; ++voice)
        {
            if ((voices >> voice) & 1)
            {
                for (uint32 tone = next_tone; tone < 16; ++tone)
                {
                    if ((remaining >> tone) & 1)
                    {
                        next_tone = tone + 1;
                        apocalypse_sound_tone_attributes((uint32)bank, program, tone, tone_attributes);
                        vag = (sint16)xport_load_le16(tone_attributes + 22);
                        settings[0] = (sint16)vag;
                        pitch = apocalypse_sound_pitch((uint32)(shifted_note >> 16), 0, tone_attributes[4], tone_attributes[5]);
                        settings[2] = (sint16)pitch;
                        w_u16(pitch_address, pitch);
                        settings[4] = (sint16)tone;
                        settings[3] = tone_attributes[4];
                        apocalypse_sound_voice_settings(voice, settings);
                        registers[2] = settings[2];
                        registers[4] = (sint16)(apocalypse_sound_vag_address((uint32)bank, (uint32)vag) >> 3);
                        registers[5] = (sint16)xport_load_le16(tone_attributes + 16);
                        registers[6] = (sint16)xport_load_le16(tone_attributes + 18);
                        SsQueueRegisters((sint32)voice, (SndRegisterAttr *)registers);
                        remaining &= ~(1 << tone);
                        break;
                    }
                }
            }
            pitch_address += 2;
        }
        apocalypse_sound_queue_key_on(voices);
        apocalypse_sound_queue_reverb(voices, voices);
    }
    sub_80094570();
    return voices;
}

extern "C" uint32 sub_8006A4C4(uint32 configuration, uint32 note, uint32 left, uint32 right, uint32 pitch_variation)
{
    FUNCTION_MARKER(0x8006A4C4, "SLUS_003.73");
    return PlaySoundVoices(configuration, (uint8)note, (sint16)left, (sint16)right, pitch_variation);
}

static void QueueSoundVoiceOff(sint16 voice)
{
    uint32 index = (uint16)voice, low, high, offset, pending_low, pending_high;
    if (r_u32(0x80104584) == 1)
        return;
    w_u32(0x80104584, 1);
    if (index < 24)
    {
        w_u16(0x80105638, index);
        index = r_u16(0x80105638);
        offset = index * 54;
        low = index < 16 ? 1 << index : 0;
        high = index >= 16 ? 1 << (index - 16) : 0;
        w_u8(0x80104E45 + offset, 0);
        low |= r_u16(0x80105520);
        high |= r_u16(0x80105522);
        w_u16(0x80104E2C + offset, 0);
        w_u16(0x80104E28 + offset, 0);
        pending_low = r_u16(0x80104E18);
        w_u16(0x80105520, low);
        w_u16(0x80104E18, pending_low & ~low);
        pending_high = r_u16(0x80104E1A);
        w_u16(0x80105522, high);
        w_u16(0x80104E1A, pending_high & ~high);
    }
    w_u32(0x80104584, 0);
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

extern "C" void sub_8006A294(uint32 mask)
{
    FUNCTION_MARKER(0x8006A294, "SLUS_003.73");
    StopSoundVoices(mask);
}

void UpdatePositionedSoundVoices(uint32 voices, const uint32 *origin, sint32 pitch_delta)
{
    uint32 balance, gain, left, right, base_pitch, pitch, cap, difference, product;
    SndRegisterAttr attributes = {0};
    FUNCTION_MARKER(0x80069FAC, "SLUS_003.73");
    if (!voices)
        return;
    balance = CalculateSoundBalance(origin, 256, 12000);
    gain = (uint32)(sint32)(sint16)r_u16(0x800ECC7A);
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
                base_pitch = r_u16(0x80100738 + voice * 2);
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

extern "C" void sub_80069FAC(uint32 voices, uint32 position, uint32 delta)
{
    FUNCTION_MARKER(0x80069FAC, "SLUS_003.73");
    UpdatePositionedSoundVoices(voices, (const uint32 *)psx_addr(position, 12), (sint32)delta);
}
