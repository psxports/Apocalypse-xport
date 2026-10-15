extern "C"
{
#include "game_sound_native.h"
#include "draft_first_signatures.h"
}

extern "C" uint32 apocalypse_sound_pitch(uint32 note, uint32 fine, uint32 center, uint32 shift)
{
    sint32 adjustment = (sint16)((shift & 0xFF) + fine);
    sint32 semitone = (sint32)(note + (uint32)(adjustment / 128) - (center & 0xFF));
    sint32 fraction = adjustment % 128;
    sint32 octave, remainder;
    uint32 product, rounded, distance;
    if (fraction < 0)
    {
        fraction += 128;
        --semitone;
        semitone += (sint16)fraction / 128;
    }
    semitone = (sint16)semitone;
    octave = semitone / 12 - 2;
    remainder = semitone % 12;
    if (remainder < 0)
    {
        remainder += 12;
        --octave;
    }
    product = (uint32)r_u16(0x800FD09C + (uint32)(2 * (sint16)remainder)) * (uint32)r_u16(0x800FD0B4 + (uint32)(2 * (sint16)fraction));
    rounded = (uint32)((sint32)product >> 16);
    octave = (sint16)octave;
    if (octave >= 0)
        return 0x3FFF;
    /* MIPS variable shifts mask their distance to five bits */
    distance = (uint32)-octave;
    // Unsigned left shift permits bit 31
    rounded += 1u << ((distance - 1) & 31);
    return (rounded >> (distance & 31)) & 0xFFFF;
}

extern "C" sint32 apocalypse_sound_vag_address(uint32 bank, uint32 vag)
{
    sint32 index;
    uint32 table, offset;
    if (apocalypse_sound_select_bank((uint32)(sint32)(sint16)bank, 0) == -1)
        return -1;
    index = ((sint32)(sint16)vag - 1) / 2;
    table = r_u32(0x8010560C);
    offset = (vag & 1) ? 12 : 14;
    return (sint32)((uint32)r_u16(table + ((uint32)index << 4) + offset) << 3);
}
