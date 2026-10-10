#include "game_sound_native.h"
#include <stdio.h>
#include <stdlib.h>

/* Unverified project translations of the observed LIBSND sequence controls */
static uint32 controls_record(uint32 table, uint32 displacement)
{
    uint32 record = r_u32(table) + displacement;
    if (!r_u32(table) || (record & 3u) || (record & 0x1FFFFFFFu) > PSX_DRAM_SIZE - 176u)
    {
        fprintf(stderr, "Missing native sequence record initialization at %08X\n", table);
        abort();
    }
    return record;
}

static uint32 controls_signed_high(uint32 value, uint32 multiplier)
{
    return (uint32)(((sint64)(sint32)value * (sint64)(sint32)multiplier) >> 32);
}

static uint32 controls_signed_ratio(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 sum = controls_signed_high(value, multiplier) + value;
    return (uint32)((sint32)sum >> shift) - (uint32)((sint32)value >> 31);
}

static uint32 controls_unsigned_ratio(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 high = (uint32)(((uint64)value * multiplier) >> 32);
    return (high + ((value - high) >> 1)) >> shift;
}

static uint32 sequence_voice_volume(uint32 sequence, uint32 left, uint32 right)
{
    uint32 record = controls_record(0x80104590u + (sequence & 255u) * 4u,
                                  ((sequence & 0xFF00u) >> 8) * 176u);
    uint32 voice = 0u;
    w_u16(record + 88u, (uint16)left);
    left = r_u16(record + 88u) < 127u;
    w_u16(record + 90u, (uint16)right);
    if (!left) w_u16(record + 88u, 127u);
    if (r_u16(record + 90u) >= 127u) w_u16(record + 90u, 127u);
    if ((sint8)r_u8(0x8010561Cu) > 0)
    {
        do
        {
            uint32 index = (uint32)(sint32)(sint16)voice;
            uint32 settings = index * 54u, value, amplitude, tone, program, pan, left_level, right_level;
            if (!(r_u32(0x800FD01Cu) & (1u << (index & 31u)))
                && (sint16)r_u16(0x80104E38u + settings) == (sint16)sequence
                && (sint16)r_u16(0x80104E40u + settings) == (sint8)r_u8(record + 38u))
            {
                (void)apocalypse_sound_select_bank((uint32)(sint32)(sint16)r_u16(0x80104E40u + settings),
                                                   (uint32)(sint32)(sint16)r_u16(0x80104E3Au + settings));
                value = (uint32)(sint32)(sint16)r_u16(0x80104E34u + settings);
                amplitude = (uint32)(sint32)(sint16)r_u16(0x80104E30u + settings)
                          * (uint32)(sint32)(sint16)r_u16(record + 96u + value * 2u);
                amplitude = controls_signed_ratio(amplitude, 0x81020409u, 6u);
                amplitude = ((amplitude << 14) - amplitude) * r_u8(r_u32(0x80105614u) + 24u);
                amplitude = controls_signed_ratio(amplitude, 0x82061029u, 13u);
                value = (uint32)(sint32)(sint16)r_u16(0x80104E3Cu + settings);
                amplitude *= r_u8(r_u32(0x8010560Cu) + (value << 4) + 1u);
                program = (uint32)(sint32)(sint16)r_u16(0x80104E3Au + settings);
                tone = (uint32)(sint32)(sint16)r_u16(0x80104E3Eu + settings);
                tone = ((program << 4) + tone) * 32u + r_u32(0x80105618u);
                amplitude *= r_u8(tone + 2u);
                amplitude = controls_unsigned_ratio(amplitude, 0x040C2051u, 13u);
                left_level = amplitude * r_u16(record + 88u);
                right_level = amplitude * r_u16(record + 90u);
                left_level = controls_unsigned_ratio(left_level, 0x02040811u, 6u);
                right_level = controls_unsigned_ratio(right_level, 0x02040811u, 6u);
                pan = r_u8(tone + 3u);
                if (pan < 64u) right_level = controls_unsigned_ratio(right_level * pan, 0x04104105u, 5u);
                else left_level = controls_unsigned_ratio(left_level * (127u - pan), 0x04104105u, 5u);
                value = (uint32)(sint32)(sint16)r_u16(0x80104E3Cu + index * 54u);
                pan = r_u8(r_u32(0x8010560Cu) + (value << 4) + 4u);
                if (pan < 64u)
                {
                    value = (right_level & 0xFFFFu) * pan;
                    right_level = (controls_signed_high(value, 0x82082083u) + value) >> 5;
                }
                else left_level = controls_signed_ratio((left_level & 0xFFFFu) * (127u - pan), 0x82082083u, 5u);
                pan = r_u8(0x80104E32u + index * 54u);
                if (pan < 64u)
                {
                    value = (right_level & 0xFFFFu) * pan;
                    right_level = (controls_signed_high(value, 0x82082083u) + value) >> 5;
                }
                else left_level = controls_signed_ratio((left_level & 0xFFFFu) * (127u - pan), 0x82082083u, 5u);
                if ((sint16)r_u16(0x80105608u) == 1)
                {
                    if ((left_level & 0xFFFFu) < (right_level & 0xFFFFu)) left_level = right_level;
                    else right_level = left_level;
                }
                value = (left_level & 0xFFFFu) * (left_level & 0xFFFFu);
                left_level = controls_signed_ratio(value, 0x80020009u, 13u);
                value = (right_level & 0xFFFFu) * (right_level & 0xFFFFu);
                right_level = controls_signed_ratio(value, 0x80020009u, 13u);
                w_u16(0x80105358u + index * 16u, (uint16)left_level);
                w_u16(0x8010535Au + index * 16u, (uint16)right_level);
                w_u8(0x80105338u + index, r_u8(0x80105338u + index) | 3u);
            }
            ++voice;
        } while ((sint16)voice < (sint8)r_u8(0x8010561Cu));
    }
    return (uint32)(sint32)(sint16)sequence;
}

uint32 apocalypse_sequence_play(uint32 sequence, uint32 mode, uint32 loops)
{
    uint32 table, record, value0, value1, value2, flags_record, result = 1u;
    sequence = (uint32)(sint32)(sint16)sequence;
    mode = (uint32)(sint32)(sint8)mode;
    loops = (uint32)(sint32)(sint16)loops;
    table = 0x80104590u + sequence * 4u;
    record = controls_record(table, 0u);
    value0 = r_u32(record + 4u); value1 = r_u32(record + 4u); value2 = r_u32(record + 4u);
    w_u32(record, value0); w_u32(record + 8u, value1); w_u32(record + 12u, value2);
    flags_record = controls_record(table, 0u);
    w_u32(flags_record + 152u, r_u32(flags_record + 152u) & ~0x200u);
    flags_record = controls_record(table, 0u);
    w_u32(flags_record + 152u, r_u32(flags_record + 152u) & ~4u);
    w_u8(record + 32u, (uint8)loops);
    if (mode == 1u)
    {
        uint32 left, right;
        flags_record = controls_record(table, 0u);
        w_u32(flags_record + 152u, r_u32(flags_record + 152u) | 1u);
        left = r_u16(record + 88u); w_u8(record + 20u, (uint8)mode);
        right = r_u16(record + 90u); w_u8(record + 33u, 0u);
        return sequence_voice_volume(sequence, left, right);
    }
    if (!mode)
    {
        flags_record = controls_record(table, 0u);
        result = r_u32(flags_record + 152u) | 2u;
        w_u32(flags_record + 152u, result);
    }
    return result;
}

uint32 apocalypse_sequence_replay(uint32 sequence)
{
    uint32 table, record, flags, result;
    sequence = (uint32)(sint32)(sint16)sequence;
    table = 0x80104590u + sequence * 4u;
    record = controls_record(table, 0u); flags = r_u32(record + 152u);
    result = flags & 0x100u;
    if (!(flags & 0x204u))
    {
        result = 0xFFFFFFFDu;
        if (!(flags & 0x100u))
        {
            w_u32(record + 152u, flags & ~2u);
            record = controls_record(table, 0u);
            w_u32(record + 152u, r_u32(record + 152u) | 8u);
            record = controls_record(table, 0u);
            result = r_u32(record + 152u) | 1u;
            w_u32(record + 152u, result);
        }
    }
    return result;
}

uint32 apocalypse_sequence_volume(uint32 sequence, uint32 left, uint32 right)
{
    uint32 record;
    sequence = (uint32)(sint32)(sint16)sequence;
    record = controls_record(0x80104590u + sequence * 4u, 0u);
    if (r_u32(record + 152u) == 1u)
        return sequence_voice_volume(sequence, left & 0xFFFFu, right & 0xFFFFu);
    w_u16(record + 88u, (uint16)left); w_u16(record + 90u, (uint16)right);
    return 1u;
}