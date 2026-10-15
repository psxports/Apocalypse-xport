extern "C"
{
#include "draft_first_signatures.h"
}
#include "game_random.h"

uint32 SetRandomSeed(uint32 seed)
{
    FUNCTION_MARKER(0x8006654C, "SLUS_003.73");
    w_u32(0x800FFC14, seed);
    w_u32(0x800FFC18, 314159265);
    w_u32(0x800FFC1C, 178453311);
    return 178453311;
}

uint32 RandomRange(uint32 range)
{
    uint32 state, multiplier, increment, next, product;
    FUNCTION_MARKER(0x80066570, "SLUS_003.73");
    state = r_u32(0x800FFC14);
    multiplier = r_u32(0x800FFC18);
    increment = r_u32(0x800FFC1C);
    next = state * multiplier + increment;
    product = (next & 65535) * range;
    multiplier = (multiplier ^ next) + (uint32)((sint32)next >> 4);
    increment += 0xEFEFEFF0;
    w_u32(0x800FFC14, next);
    increment += (uint32)((sint32)next >> 3);
    w_u32(0x800FFC18, multiplier);
    w_u32(0x800FFC1C, increment);
    // Original result shifts the wrapped product arithmetically
    return (uint32)((sint32)product >> 16);
}

extern "C" uint32 sub_8006654C(uint32 seed)
{
    FUNCTION_MARKER(0x8006654C, "SLUS_003.73");
    return SetRandomSeed(seed);
}

extern "C" uint32 sub_80066570(uint32 range)
{
    FUNCTION_MARKER(0x80066570, "SLUS_003.73");
    return RandomRange(range);
}

uint32 SetAuxiliaryRandomSeed(uint32 seed)
{
    FUNCTION_MARKER(0x800771F4, "SLUS_003.73");
    w_u32(0x800FFD00, seed);
    w_u32(0x800FFD04, 314159265);
    w_u32(0x800FFD08, 178453311);
    return 178453311;
}

extern "C" uint32 sub_800771F4(uint32 seed)
{
    FUNCTION_MARKER(0x800771F4, "SLUS_003.73");
    return SetAuxiliaryRandomSeed(seed);
}
