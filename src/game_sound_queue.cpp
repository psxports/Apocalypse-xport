extern "C"
{
#include "psx.h"
}

extern "C" void apocalypse_sound_queue_key_on(uint32 voice_mask)
{
    sint32 count = (sint8)r_u8(0x8010561C);
    sint32 voice;
    for (voice = 0; voice < count; ++voice)
    {
        // The signed voice count can reach shift bit 31
        uint32 bit = 1u << ((uint32)voice & 31);
        if ((voice_mask & bit) != 0)
        {
            uint32 half = voice >= 16 ? 2 : 0;
            uint32 half_bit = 1u << ((uint32)(voice >= 16 ? voice - 16 : voice) & 31);
            uint32 on = 0x80104E18 + half;
            uint32 off = 0x80105520 + half;
            w_u16(on, r_u16(on) | half_bit);
            w_u16(off, r_u16(off) & ~half_bit);
        }
    }
}

extern "C" void apocalypse_sound_queue_reverb(uint32 changed_mask, uint32 reverb_mask)
{
    uint32 voice;
    for (voice = 0; voice < 24; ++voice)
    {
        uint32 bit = 1 << voice;
        if ((changed_mask & bit) != 0)
        {
            uint32 address = voice >= 16 ? 0x80104E1E : 0x80104E1C;
            uint32 half_bit = 1 << (voice >= 16 ? voice - 16 : voice);
            uint16 value = r_u16(address);
            if ((reverb_mask & bit) != 0)
                value = (uint16)(value | half_bit);
            else
                value = (uint16)(value & ~half_bit);
            w_u16(address, value);
        }
    }
}
