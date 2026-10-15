extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "world_gravity.h"
#include "vector_distance.h"
#include "vector_arithmetic.h"
#include "native_object_storage.h"
#include <string.h>

uint32 SetGravityVector(const uint32 *vector)
{
    uint32 input = GuestMemoryAddress(vector), values[3], normalized[3];
    uint32 magnitude, divisor, result;
    FUNCTION_MARKER(0x8003AA20, "SLUS_003.73");
    if (input)
    {
        values[0] = r_u32(input);
        values[1] = r_u32(input + 4);
        values[2] = r_u32(input + 8);
    }
    else
    {
        memcpy(&values[0], vector, 4);
        memcpy(&values[1], (const uint8 *)vector + 4, 4);
        memcpy(&values[2], (const uint8 *)vector + 8, 4);
    }
    w_u32(0x800A6844, values[0]);
    w_u32(0x800A6848, values[1]);
    w_u32(0x800A684C, values[2]);
    magnitude = MeasureVectorLength((const uint32 *)psx_addr(0x800A6844, 12));
    w_u32(0x800FF480, magnitude);
    divisor = 0 - magnitude;
    DivideVector(normalized, (const uint32 *)psx_addr(0x800A6844, 12), &divisor);
    magnitude = r_u32(0x800FF480);
    w_u32(0x800A6850, normalized[0]);
    w_u32(0x800A6854, normalized[1]);
    w_u32(0x800A6858, normalized[2]);
    // Original signed magic multiply implements division by10 toward zero
    result = (uint32)((sint32)magnitude / 10);
    w_u32(0x800FFB94, result);
    return result;
}

extern "C" uint32 sub_8003AA20(uint32 vector)
{
    FUNCTION_MARKER(0x8003AA20, "SLUS_003.73");
    return SetGravityVector((const uint32 *)psx_addr(vector, 12));
}

extern "C" uint32 xport_draft_host_sub_8003AA20_p1(const void *vector)
{
    FUNCTION_MARKER(0x8003AA20, "SLUS_003.73");
    return SetGravityVector((const uint32 *)vector);
}
