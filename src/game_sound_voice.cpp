#include "game_sdk_bindings.h"
extern "C"
{
#include "game_sound_native.h"
}
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" sint32 apocalypse_sound_select_bank(uint32 bank, uint32 program)
{
    sint32 signed_bank = (sint16)bank;
    sint32 signed_program = (sint16)program;
    uint32 offset, header, programs, tones;
    FUNCTION_MARKER(0x800935EC, "SLUS_003.73");
    if ((uint16)bank >= 16)
        return -1;
    if (r_u8(0x80105640 + (uint32)signed_bank) != 1)
        return -1;
    if (signed_program >= (sint16)r_u16(0x8010560A))
        return -1;
    offset = (uint32)signed_bank * 4;
    header = r_u32(0x80105580 + offset);
    programs = r_u32(0x80105540 + offset);
    tones = r_u32(0x801055C8 + offset);
    w_u8(0x80105621, (uint8)bank);
    w_u8(0x80105626, (uint8)program);
    w_u32(0x80105618, tones);
    w_u32(0x80105614, header);
    w_u32(0x8010560C, programs);
    w_u8(0x80105627, r_u8(programs + (uint32)signed_program * 16 + 8));
    return 0;
}

extern "C" sint32 apocalypse_sound_actual_program(uint32 bank, uint32 program)
{
    sint32 signed_program = (sint16)program;
    uint32 programs;
    FUNCTION_MARKER(0x800940D4, "SLUS_003.73");
    if ((uint16)bank >= 17 || signed_program < 0)
        return -1;
    if ((sint16)r_u16(0x8010560A) < signed_program)
        return -1;
    programs = r_u32(0x80105540 + (uint32)(sint16)bank * 4);
    return (sint16)r_u16(programs + (uint32)signed_program * 16 + 8);
}

extern "C" sint32 apocalypse_sound_tone_attributes(uint32 bank, uint32 program, uint32 tone, void *output)
{
    static const uint8 byte_offsets[14] = {0, 1, 2, 3, 4, 5, 7, 6, 8, 9, 10, 11, 12, 13};
    uint8 *destination = (uint8 *)output;
    uint32 index, displacement, source;
    uint16 value;
    FUNCTION_MARKER(0x8008F9FC, "SLUS_003.73");
    if (r_u8(0x80105640 + (uint32)(sint16)bank) != 1)
        return -1;
    (void)apocalypse_sound_select_bank((uint32)(sint16)bank, (uint32)(sint16)program);
    displacement = tone + (uint32)(sint8)r_u8(0x80105627) * 16;
    displacement = (uint32)((sint32)(displacement << 16) >> 11);
    for (index = 0; index < 14; ++index)
    {
        uint32 offset = byte_offsets[index];
        source = r_u32(0x80105618) + displacement;
        destination[offset] = r_u8(source + offset);
    }
    source = r_u32(0x80105618) + displacement;
    for (index = 16; index < 24; index += 2)
    {
        value = r_u16(source + index);
        memcpy(destination + index, &value, sizeof(value));
    }
    return 0;
}

extern "C" uint32 apocalypse_sound_voice_settings(uint32 voice, const void *settings)
{
    const uint8 *source = (const uint8 *)settings;
    uint32 offset = voice * 54;
    uint16 value;
    FUNCTION_MARKER(0x80094134, "SLUS_003.73");
    memcpy(&value, source, sizeof(value));
    w_u16(0x80104E28 + offset, value);
    memcpy(&value, source + 4, sizeof(value));
    w_u16(0x80104E2C + offset, value);
    memcpy(&value, source + 14, sizeof(value));
    w_u16(0x80104E30 + offset, value);
    w_u8(0x80104E32 + offset, source[16]);
    memcpy(&value, source + 6, sizeof(value));
    w_u16(0x80104E38 + offset, 33);
    w_u16(0x80104E36 + offset, value);
    memcpy(&value, source + 12, sizeof(value));
    w_u16(0x80104E3A + offset, value);
    memcpy(&value, source + 10, sizeof(value));
    w_u16(0x80104E3C + offset, value);
    memcpy(&value, source + 8, sizeof(value));
    w_u16(0x80104E3E + offset, value);
    memcpy(&value, source + 2, sizeof(value));
    w_u16(0x80104E40 + offset, value);
    return offset;
}

static uint32 SetSpuVoiceMask(uint32 mode, uint32 mask, uint32 low_index, uint32 high_index)
{
    FUNCTION_MARKER(0x80095A0C, "SLUS_003.73");
    uint32 queued = r_u32(0x800FD228) & 1;
    uint32 base = queued ? 0x80105668 : r_u32(0x800FD23C);
    uint32 high = r_u16(base + high_index * 2), low = r_u16(base + low_index * 2);
    uint32 result = low | ((high & 255) << 16);
    if (mode != 0 && mode != 1 && mode != 8)
        return result & 0xFFFFFF;
    // Re-read the original queue selection before publishing the mask
    queued = r_u32(0x800FD228) & 1;
    base = queued ? 0x80105668 : r_u32(0x800FD23C);
    if (mode == 8)
    {
        w_u16(base + low_index * 2, mask);
        w_u16(base + high_index * 2, (mask >> 16) & 255);
        result = mask;
    }
    else
    {
        low = r_u16(base + low_index * 2);
        w_u16(base + low_index * 2, mode ? low | mask : low & ~mask);
        high = r_u16(base + high_index * 2);
        uint32 high_mask = (mask >> 16) & 255;
        w_u16(base + high_index * 2, mode ? high | high_mask : high & ~high_mask);
        result = mode ? result | (mask & 0xFFFFFF) : result & ~(mask & 0xFFFFFF);
    }
    if (queued)
        w_u32(0x800FD1F4, r_u32(0x800FD1F4) | (1u << (((sint32)(low_index - 198) >> 1) & 31)));
    return result & 0xFFFFFF;
}

static uint32 SetNoiseVoice(uint32 mode, uint32 mask)
{
    FUNCTION_MARKER(0x800959DC, "SLUS_003.73");
    return SetSpuVoiceMask(mode, mask, 202, 203);
}

extern "C" uint32 sub_8009426C(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x8009426C, "SLUS_003.73");
    sint32 v4;
    sint32 v5;
    unsigned short v6;
    unsigned short v7;
    sint32 v8;
    sint8 v9;
    sint8 v10;
    unsigned char v11;
    sint32 v12;
    uint32 v13;
    sint32 v14;
    uint32 v15;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    uint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    v4 = 0;
    v5 = 0;
    if (!(((uint8)(a1))))
        goto LABEL_25;
    v6 = -1;
    do
    {
        v7 = (uint8)a2;
        v8 = 0;
        v9 = 99;
        v10 = 99;
        v11 = 0;
        if (!r_u8(0x8010561C))
            goto LABEL_18;
        v12 = 0;
        while (1)
        {
            if ((((1u << ((uint32)v12 & 31)) & v5) != 0))
                goto LABEL_17;
            v13 = (0x80104E28 + ((27 * v12)) * 2);
            if ((!(r_u8((((uint32)(v13)) + (29) * 1))) && !r_u16((v13 + (3) * 2))))
                break;
            v14 = (sint16)r_u16(v13 + 26);
            if ((v14 >= v7))
            {
                if ((v14 == v7))
                {
                    v15 = ((unsigned short)(r_u16((v13 + (3) * 2))));
                    if ((v15 >= v6))
                    {
                        if (((v15 != v6) || (v8 >= (sint16)r_u16(v13 + 2))))
                            goto LABEL_17;
                        v8 = ((unsigned short)(r_u16((v13 + (1) * 2))));
                    }
                    else
                    {
                        v8 = ((unsigned short)(r_u16((v13 + (1) * 2))));
                        v6 = r_u16((v13 + (3) * 2));
                    }
                    v9 = v11;
                }
            }
            else
            {
                v7 = r_u16((v13 + (13) * 2));
                v6 = r_u16((v13 + (3) * 2));
                v8 = ((unsigned short)(r_u16((v13 + (1) * 2))));
                v9 = v11;
            }
        LABEL_17:
            v12 = ++v11;

            if ((v11 >= ((uint32)(((unsigned char)(r_u8(0x8010561C)))))))
                goto LABEL_18;
        }

        v10 = v11;
    LABEL_18:
        if ((v10 == 99))
        {
            if ((v9 == 99))
                return -1;
            v17 = (1u << ((uint32)v9 & 31));
        }
        else
        {
            v17 = (1u << ((uint32)v10 & 31));
        }

        v5 |= v17;
        ++v4;
        v6 = -1;
    } while ((((unsigned char)(v4)) < ((uint32)(((unsigned char)(a1))))));
LABEL_25:
    v18 = a1;

    v19 = 0;
    if (r_u8(0x8010561C))
    {
        v20 = 0;
        do
        {
            v21 = (1u << ((uint32)v20 & 31));
            if ((((1u << ((uint32)v20 & 31)) & v5) != 0))
            {
                --v18;
                v22 = (0x80104E28 + ((27 * v20)) * 2);
                v23 = r_u8((((uint32)(v22)) + (29) * 1));
                w_u16((v22 + (1) * 2), ((unsigned char)(v18)));
                w_u16(v22 + 26, (uint8)a2);
                w_u16((v22 + (15) * 2), 0);
                w_u16((v22 + (21) * 2), 0);
                if ((v23 == 2))
                    SetNoiseVoice(0, (uint32)v21);
                w_u8((((uint32)(v22)) + (29) * 1), 1);
            }
            else
            {
                w_u16((0x80104E28 + (((27 * v20) + 1)) * 2), (r_u16((0x80104E28 + (((27 * v20) + 1)) * 2)) + (((unsigned char)(a1)))));
            }
            v20 = ((unsigned char)(++v19));
        } while ((((unsigned char)(v19)) < ((uint32)(((unsigned char)(r_u8(0x8010561C)))))));
    }
    v24 = 0;
    v25 = 0;
    do
    {
        ++v24;
        w_u32((0x801054E0 + (v25) * 4), (r_u32((0x801054E0 + (v25) * 4)) & (~v5)));
        v25 = ((unsigned char)(v24));
    } while ((((unsigned char)(v24)) < 0x10));
    return v5;
}
