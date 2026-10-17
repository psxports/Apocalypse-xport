extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "game_lighting.h"
#include "native_object_storage.h"
#include <string.h>

static uint16 ReadLightingHalf(const sint16 *vector, uint32 address, uint32 lane)
{
    uint16 value;
    if (address)
        return r_u16(address + lane * 2);
    memcpy(&value, (const uint8 *)vector + lane * 2, 2);
    return value;
}

sint16 *SetLightingDirections(const sint16 *first, const sint16 *second, const sint16 *third)
{
    uint32 first_address = GuestMemoryAddress(first), second_address = GuestMemoryAddress(second), third_address = GuestMemoryAddress(third);
    uint32 lane;
    uint16 value;
    FUNCTION_MARKER(0x8007C978, "SLUS_003.73");
    for (lane = 0; lane < 3; ++lane)
    {
        value = ReadLightingHalf(first, first_address, lane);
        w_u16(0x800EE6F0 + lane * 2, value);
    }
    for (lane = 0; lane < 3; ++lane)
    {
        value = ReadLightingHalf(second, second_address, lane);
        w_u16(0x800EE6F6 + lane * 2, value);
    }
    for (lane = 0; lane < 3; ++lane)
    {
        value = ReadLightingHalf(third, third_address, lane);
        w_u16(0x800EE6FC + lane * 2, value);
    }
    return (sint16 *)psx_addr(0x800EE6F0, 18);
}

sint16 *SetLightingColors(const sint16 *first, const sint16 *second, const sint16 *third)
{
    uint32 first_address = GuestMemoryAddress(first), second_address = GuestMemoryAddress(second), third_address = GuestMemoryAddress(third);
    uint32 lane;
    uint16 value;
    FUNCTION_MARKER(0x8007C9E4, "SLUS_003.73");
    for (lane = 0; lane < 3; ++lane)
    {
        value = ReadLightingHalf(first, first_address, lane);
        w_u16(0x800EE710 + lane * 6, value);
        value = ReadLightingHalf(second, second_address, lane);
        w_u16(0x800EE712 + lane * 6, value);
        value = ReadLightingHalf(third, third_address, lane);
        w_u16(0x800EE714 + lane * 6, value);
    }
    return (sint16 *)psx_addr(0x800EE710, 18);
}





extern "C" uint32 apocalypse_lighting_directions_native(uint32 first, uint32 second, const sint16 *third)
{
    SetLightingDirections((const sint16 *)psx_addr(first, 6), (const sint16 *)psx_addr(second, 6), third);
    return 0x800EE6F0;
}

extern "C" uint32 apocalypse_lighting_colors_native(uint32 first, const sint16 *second, const sint16 *third)
{
    SetLightingColors((const sint16 *)psx_addr(first, 6), second, third);
    return 0x800EE710;
}
