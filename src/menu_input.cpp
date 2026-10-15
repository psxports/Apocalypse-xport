#include "menu_input.h"
#include "native_object_storage.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>

// Native output adapters preserve guest instrumentation and unaligned host words
static uint32 ReadMenuWord(const uint32 *word)
{
    uint32 address = GuestMemoryAddress(word);
    if (address)
        return r_u32(address);
    uint32 value;
    memcpy(&value, word, sizeof(value));
    return value;
}

static void WriteMenuWord(uint32 *word, uint32 value)
{
    uint32 address = GuestMemoryAddress(word);
    if (address)
        w_u32(address, value);
    else
        memcpy(word, &value, sizeof(value));
}

uint32 ReadMenuInput(uint32 *confirm, uint32 *cancel, uint32 *any, uint32 *start)
{
    FUNCTION_MARKER(0x80017364, "SLUS_003.73");
    static const uint32 events[7] = {0x800EC109, 0x800EC119, 0x800EC1E9, 0x800EC139, 0x800EC149, 0x800EC159, 0x800EC169};
    uint32 value = r_u8(0x800EC1D9);
    WriteMenuWord(start, value);
    if (value)
        w_u8(0x800EC1D9, 0);
    value = 0;
    if (r_u8(0x800EC129) || ReadMenuWord(start))
        value = 1;
    WriteMenuWord(confirm, value);
    if (value)
        w_u8(0x800EC129, 0);
    value = r_u8(0x800EC0F9);
    WriteMenuWord(cancel, value);
    if (value)
        w_u8(0x800EC0F9, 0);
    uint32 cancel_value = ReadMenuWord(cancel);
    uint32 confirm_value = ReadMenuWord(confirm);
    WriteMenuWord(any, cancel_value | confirm_value);
    for (uint32 index = 0; index < 7; ++index)
    {
        if (r_u8(events[index]))
        {
            w_u8(events[index], 0);
            WriteMenuWord(any, 1);
        }
    }
    value = ReadMenuWord(confirm);
    if (value)
        WriteMenuWord(cancel, 0);
    return value;
}

extern "C" uint32 sub_80017364(uint32 confirm, uint32 cancel, uint32 any, uint32 start)
{
    FUNCTION_MARKER(0x80017364, "SLUS_003.73");
    return ReadMenuInput((uint32 *)psx_addr(confirm, 4), (uint32 *)psx_addr(cancel, 4), (uint32 *)psx_addr(any, 4), (uint32 *)psx_addr(start, 4));
}

extern "C" uint32 xport_draft_host_sub_80017364_p1234(void *confirm, void *cancel, void *any, void *start)
{
    FUNCTION_MARKER(0x80017364, "SLUS_003.73");
    return ReadMenuInput((uint32 *)confirm, (uint32 *)cancel, (uint32 *)any, (uint32 *)start);
}
