extern "C"
{
#include "draft_first_signatures.h"
}
#include "game_string.h"
#include "native_object_storage.h"

static uint32 ReadNameByte(const char *text, uint32 address, uint32 offset)
{
    return address ? r_u8(address + offset) : (uint8)text[offset];
}

uint32 NamesEqualIgnoringCase(const char *left, const char *right)
{
    FUNCTION_MARKER(0x80067724, "SLUS_003.73");
    if (!left)
        return right ? 0 : 1;
    if (!right)
        return 0;
    uint32 left_address = GuestMemoryAddress(left);
    uint32 right_address = GuestMemoryAddress(right);
    uint32 offset = 0;
    for (;;)
    {
        uint32 first = ReadNameByte(left, left_address, offset);
        uint32 second = ReadNameByte(right, right_address, offset);
        if (first - 65 < 26)
            first += 32;
        if (((second - 65) & 255) < 26)
            second += 32;
        first &= 255;
        second &= 255;
        if (first != second)
            return 0;
        if (!first)
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
    uint32 from = GuestMemoryAddress(source);
    uint32 to = GuestMemoryAddress(destination);
    uint32 offset = 0, value;
    do
    {
        value = ReadNameByte(source, from, offset);
        if (to)
            w_u8(to + offset, value);
        else
            destination[offset] = (char)value;
        value = ReadNameByte(source, from, offset);
        ++offset;
    } while (value);
    return value;
}

extern "C" uint32 sub_80067808(uint32 source, uint32 destination)
{
    FUNCTION_MARKER(0x80067808, "SLUS_003.73");
    return CopyGameString((const char *)psx_addr(source, 1), (char *)psx_addr(destination, 1));
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
