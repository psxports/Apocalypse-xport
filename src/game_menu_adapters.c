#include "draft_first_adapters.h"
#include "draft_first_signatures.h"
#include <string.h>

static uint32 menu_word(const void *input)
{
    uint32 value;
    memcpy(&value, input, sizeof(value));
    return value;
}

static void menu_store(void *output, uint32 value)
{
    memcpy(output, &value, sizeof(value));
}

uint32 xport_draft_host_sub_80015614_p34(uint32 object, uint32 label, void *x, void *y)
{
    uint32 index = 0u;
    uint32 row = object;
    uint32 result;
    menu_store(x, r_u32(object + 12u));
    menu_store(y, r_u32(object + 16u));
    result = r_u8(object + 10u);
    if (result != 0u)
    {
        do
        {
            if (r_u8(row + 37u) != 0u)
            {
                menu_store(y, menu_word(y) + r_u8(row + 36u));
                result = sub_80067724(label, r_u32(row + 24u));
                if (result != 0u)
                    return result;
                menu_store(y, menu_word(y) + r_u32(object + 20u));
            }
            ++index;
            result = index < r_u8(object + 10u);
            row += 28u;
        } while (result != 0u);
    }
    return result;
}

uint32 xport_draft_host_sub_80017364_p1234(void *confirm, void *cancel, void *any, void *start)
{
    static const uint32 events[7] = {0x800EC109u, 0x800EC119u, 0x800EC1E9u, 0x800EC139u, 0x800EC149u, 0x800EC159u, 0x800EC169u};
    uint32 value = r_u8(0x800EC1D9u);
    uint32 index;
    menu_store(start, value);
    if (value != 0u)
        w_u8(0x800EC1D9u, 0u);
    value = r_u8(0x800EC129u) != 0u || menu_word(start) != 0u;
    menu_store(confirm, value);
    if (value != 0u)
        w_u8(0x800EC129u, 0u);
    value = r_u8(0x800EC0F9u);
    menu_store(cancel, value);
    if (value != 0u)
        w_u8(0x800EC0F9u, 0u);
    menu_store(any, menu_word(cancel) | menu_word(confirm));
    for (index = 0u; index < 7u; ++index)
    {
        if (r_u8(events[index]) != 0u)
        {
            w_u8(events[index], 0u);
            menu_store(any, 1u);
        }
    }
    value = menu_word(confirm);
    if (value != 0u)
        menu_store(cancel, 0u);
    return value;
}
