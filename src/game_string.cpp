extern "C"
{
#include "draft_first_signatures.h"
}
#include "game_string.h"



uint32 NamesEqualIgnoringCase(const char *left, const char *right)
{
    FUNCTION_MARKER(0x80067724, "SLUS_003.73");
    if (!left)
        return right ? 0 : 1;
    if (!right)
        return 0;
    uint32 offset = 0;
    for (;;)
    {
        uint32 left_byte = (uint8)left[offset];
        uint32 right_byte = (uint8)right[offset];
        if (left_byte - 65 < 26)
            left_byte += 32;
        if (((right_byte - 65) & 255) < 26)
            right_byte += 32;
        left_byte &= 255;
        right_byte &= 255;
        if (left_byte != right_byte)
            return 0;
        if (!left_byte)
            return 1;
        ++offset;
    }
}

extern "C" uint32 sub_80067724(uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x80067724, "SLUS_003.73");
    return NamesEqualIgnoringCase(left ? (const char *)psx_addr(left, 1) : 0, right ? (const char *)psx_addr(right, 1) : 0);
}

uint32 CopyGameString(const char *source, char *destination)
{
    FUNCTION_MARKER(0x80067808, "SLUS_003.73");
    uint32 offset = 0, value;
    do
    {
        destination[offset] = source[offset];
        value = (uint8)source[offset];
        ++offset;
    } while (value);
    return value;
}



extern "C" uint32 xport_draft_host_sub_80067808_p1(void *source, uint32 destination)
{
    FUNCTION_MARKER(0x80067808, "SLUS_003.73");
    return CopyGameString((const char *)source, (char *)psx_addr(destination, 1));
}

extern "C" uint32 xport_draft_host_sub_80067808_p2(uint32 source, void *destination)
{
    FUNCTION_MARKER(0x80067808, "SLUS_003.73");
    return CopyGameString((const char *)psx_addr(source, 1), (char *)destination);
}
