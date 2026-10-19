#include "game_diagnostics.h"
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
