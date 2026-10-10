#include "psx.h"

/* Unverified native sound queue translation */
void apocalypse_sound_queue_registers(uint32 voice, const sint16 registers[8])
{
    uint32 base = 0x80105358u + 16u * voice;
    uint32 dirty_address = 0x80105338u + voice;
    uint16 flags = (uint16)registers[3];
    if ((flags & 1u) != 0u || flags == 0u)
    {
        w_u16(base, (uint16)registers[0] & 0x7FFFu);
        w_u8(dirty_address, r_u8(dirty_address) | 1u);
    }
    if ((flags & 2u) != 0u || flags == 0u)
    {
        w_u16(base + 2u, (uint16)registers[1] & 0x7FFFu);
        w_u8(dirty_address, r_u8(dirty_address) | 2u);
    }
    if ((flags & 4u) != 0u || flags == 0u)
    {
        w_u16(base + 8u, (uint16)registers[5]);
        w_u8(dirty_address, r_u8(dirty_address) | 0x10u);
    }
    if ((flags & 8u) != 0u || flags == 0u)
    {
        w_u16(base + 10u, (uint16)registers[6]);
        w_u8(dirty_address, r_u8(dirty_address) | 0x20u);
    }
    if ((flags & 0x20u) != 0u || flags == 0u)
    {
        w_u16(base + 4u, (uint16)registers[2]);
        w_u8(dirty_address, r_u8(dirty_address) | 4u);
    }
    if ((flags & 0x10u) != 0u || flags == 0u)
    {
        w_u16(base + 6u, (uint16)registers[4]);
        w_u8(dirty_address, r_u8(dirty_address) | 8u);
    }
}

void apocalypse_sound_queue_key_on(uint32 voice_mask)
{
    sint32 count = (sint8)r_u8(0x8010561Cu);
    sint32 voice;
    for (voice = 0; voice < count; ++voice)
    {
        uint32 bit = 1u << ((uint32)voice & 31u);
        if ((voice_mask & bit) != 0u)
        {
            uint32 half = voice >= 16 ? 2u : 0u;
            uint32 half_bit = 1u << ((uint32)(voice >= 16 ? voice - 16 : voice) & 31u);
            uint32 on = 0x80104E18u + half;
            uint32 off = 0x80105520u + half;
            w_u16(on, r_u16(on) | half_bit);
            w_u16(off, r_u16(off) & ~half_bit);
        }
    }
}

void apocalypse_sound_queue_reverb(uint32 changed_mask, uint32 reverb_mask)
{
    uint32 voice;
    for (voice = 0u; voice < 24u; ++voice)
    {
        uint32 bit = 1u << voice;
        if ((changed_mask & bit) != 0u)
        {
            uint32 address = voice >= 16u ? 0x80104E1Eu : 0x80104E1Cu;
            uint32 half_bit = 1u << (voice >= 16u ? voice - 16u : voice);
            uint16 value = r_u16(address);
            if ((reverb_mask & bit) != 0u)
                value = (uint16)(value | half_bit);
            else
                value = (uint16)(value & ~half_bit);
            w_u16(address, value);
        }
    }
}
