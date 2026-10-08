#include "draft_first_signatures.h"

uint32 sub_8009C65C(uint32 destination)
{
    uint32 source = 0x800FDFDCu;
    uint32 output = destination;
    uint32 distance = 0u;
    uint32 index;
    do
    {
        uint32 code = r_u8(source++);
        if (code >= 0xF0u)
        {
            distance = 0u;
            if (code != 0xF0u)
                distance = ((code << 8) | r_u8(source++)) - 61695u;
        }
        else
        {
            uint32 remaining = code + 1u;
            do
            {
                uint8 value = distance != 0u
                    ? r_u8(output - distance) : r_u8(source++);
                w_u8(output++, value);
            } while (--remaining != 0u);
        }
    } while (distance != 0xF00u);
    output = destination + 8u;
    for (index = 4u; index <= 0x87FFu; ++index)
    {
        w_u16(output, r_u16(output) ^ r_u16(output - 8u));
        output += 2u;
    }
    return 1u;
}
