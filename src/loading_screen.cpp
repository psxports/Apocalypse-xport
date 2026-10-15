#include "loading_screen.h"
#include "game_string.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
uint32 sub_80088B28(uint32 environment);
}

// Native rectangle view preserves the two ordered original word loads
static void ReadLoadingRectangle(uint32 rectangle[2], uint32 address)
{
    rectangle[0] = r_u32(address);
    rectangle[1] = r_u32(address + 4);
}

uint32 PrepareLevelLoadingScreen(const char *name, uint32 image)
{
    FUNCTION_MARKER(0x80016354, "SLUS_003.73");
    uint32 rectangle[2];
    sub_80088B28(0x800C636C);
    ReadLoadingRectangle(rectangle, 0x800FF05C);
    xport_draft_host_sub_8008847C_p1((const sint16 *)rectangle, 0, 0, 0);
    sub_8001024C(0, image, 240);
    ReadLoadingRectangle(rectangle, 0x800FF064);
    xport_draft_host_sub_8008847C_p1((const sint16 *)rectangle, 100, 0, 0);
    ReadLoadingRectangle(rectangle, 0x800FF06C);
    xport_draft_host_sub_8008847C_p1((const sint16 *)rectangle, 0, 0, 0);
    uint32 cursor = r_u32(0x800FF004);
    w_u32(0x800FF050, 1);
    w_u32(0x800FF058, 0);
    uint32 result;
    for (;;)
    {
        result = r_u8(cursor);
        if (!result)
            break;
        if (NamesEqualIgnoringCase((const char *)psx_addr(cursor, 1), name))
        {
            if (r_u8(cursor))
            {
                ++cursor;
                uint32 value;
                do
                {
                    value = r_u8(cursor);
                    ++cursor;
                } while (value);
                --cursor;
            }
            uint32 words = (cursor + 3) & 0xFFFFFFFC;
            result = r_u32(words);
            while (result != 0xFFFFFFFF)
            {
                uint32 total = r_u32(0x800FF058);
                uint32 size = r_u32(words);
                words += 4;
                w_u32(0x800FF058, total + size);
                result = r_u32(words);
            }
            break;
        }
        uint32 value;
        do
        {
            value = r_u8(cursor);
            ++cursor;
        } while (value);
        --cursor;
        uint32 words = (cursor + 3) & 0xFFFFFFFC;
        value = r_u32(words);
        if (value != 0xFFFFFFFF)
        {
            words += 4;
            do
            {
                value = r_u32(words);
                words += 4;
            } while (value != 0xFFFFFFFF);
            words -= 4;
        }
        cursor = words + 4;
    }
    w_u32(0x800FF054, 0);
    return result;
}

extern "C" uint32 sub_80016354(uint32 name, uint32 image)
{
    FUNCTION_MARKER(0x80016354, "SLUS_003.73");
    return PrepareLevelLoadingScreen((const char *)psx_addr(name, 1), image);
}
