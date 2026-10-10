#include "game_sound_native.h"
#include <string.h>

sint32 apocalypse_sound_select_bank(uint32 bank, uint32 program)
{
    sint32 signed_bank = (sint16)bank;
    sint32 signed_program = (sint16)program;
    uint32 offset, header, programs, tones;
    FUNCTION_MARKER(0x800935ECu, "SLUS_003.73");
    if ((uint16)bank >= 16u)
        return -1;
    if (r_u8(0x80105640u + (uint32)signed_bank) != 1u)
        return -1;
    if (signed_program >= (sint16)r_u16(0x8010560Au))
        return -1;
    offset = (uint32)signed_bank * 4u;
    header = r_u32(0x80105580u + offset);
    programs = r_u32(0x80105540u + offset);
    tones = r_u32(0x801055C8u + offset);
    w_u8(0x80105621u, (uint8)bank);
    w_u8(0x80105626u, (uint8)program);
    w_u32(0x80105618u, tones);
    w_u32(0x80105614u, header);
    w_u32(0x8010560Cu, programs);
    w_u8(0x80105627u, r_u8(programs + (uint32)signed_program * 16u + 8u));
    return 0;
}

sint32 apocalypse_sound_actual_program(uint32 bank, uint32 program)
{
    sint32 signed_program = (sint16)program;
    uint32 programs;
    FUNCTION_MARKER(0x800940D4u, "SLUS_003.73");
    if ((uint16)bank >= 17u || signed_program < 0)
        return -1;
    if ((sint16)r_u16(0x8010560Au) < signed_program)
        return -1;
    programs = r_u32(0x80105540u + (uint32)(sint16)bank * 4u);
    return (sint16)r_u16(programs + (uint32)signed_program * 16u + 8u);
}

sint32 apocalypse_sound_tone_attributes(uint32 bank, uint32 program, uint32 tone, void *output)
{
    static const uint8 byte_offsets[14] = {0, 1, 2, 3, 4, 5, 7, 6, 8, 9, 10, 11, 12, 13};
    uint8 *destination = (uint8 *)output;
    uint32 index, displacement, source;
    uint16 value;
    FUNCTION_MARKER(0x8008F9FCu, "SLUS_003.73");
    if (r_u8(0x80105640u + (uint32)(sint16)bank) != 1u)
        return -1;
    (void)apocalypse_sound_select_bank((uint32)(sint16)bank, (uint32)(sint16)program);
    displacement = tone + (uint32)(sint8)r_u8(0x80105627u) * 16u;
    displacement = (uint32)((sint32)(displacement << 16) >> 11);
    for (index = 0; index < 14u; ++index)
    {
        uint32 offset = byte_offsets[index];
        source = r_u32(0x80105618u) + displacement;
        destination[offset] = r_u8(source + offset);
    }
    source = r_u32(0x80105618u) + displacement;
    for (index = 16u; index < 24u; index += 2u)
    {
        value = r_u16(source + index);
        memcpy(destination + index, &value, sizeof(value));
    }
    return 0;
}

uint32 apocalypse_sound_voice_settings(uint32 voice, const void *settings)
{
    const uint8 *source = (const uint8 *)settings;
    uint32 offset = voice * 54u;
    uint16 value;
    FUNCTION_MARKER(0x80094134u, "SLUS_003.73");
    memcpy(&value, source, sizeof(value));
    w_u16(0x80104E28u + offset, value);
    memcpy(&value, source + 4u, sizeof(value));
    w_u16(0x80104E2Cu + offset, value);
    memcpy(&value, source + 14u, sizeof(value));
    w_u16(0x80104E30u + offset, value);
    w_u8(0x80104E32u + offset, source[16]);
    memcpy(&value, source + 6u, sizeof(value));
    w_u16(0x80104E38u + offset, 33u);
    w_u16(0x80104E36u + offset, value);
    memcpy(&value, source + 12u, sizeof(value));
    w_u16(0x80104E3Au + offset, value);
    memcpy(&value, source + 10u, sizeof(value));
    w_u16(0x80104E3Cu + offset, value);
    memcpy(&value, source + 8u, sizeof(value));
    w_u16(0x80104E3Eu + offset, value);
    memcpy(&value, source + 2u, sizeof(value));
    w_u16(0x80104E40u + offset, value);
    return offset;
}
