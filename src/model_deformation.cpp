extern "C" {
#include "draft_first_signatures.h"
}
#include "model_deformation.h"
#include "game_random.h"

// TODO Establish receiver ownership before classifying this as an instance method
uint32 ApplyRandomModelOffset(uint32 object, uint32 range)
{
    uint32 model, index, bank, half_range, x, y, z, destination;
    FUNCTION_MARKER(0x800197DC, "SLUS_003.73");
    model = r_u32(object + 344);
    index = RandomRange(r_u32(model + 72) - 1);
    bank = r_u32(object + 352);
    half_range = (uint32)((sint32)range >> 1);
    x = RandomRange(range) - half_range;
    y = RandomRange(range) - half_range;
    z = RandomRange(range) - half_range;
    destination = object + 144 * bank + 356 + 6 * index;
    w_u16(destination + 4, z);
    w_u16(destination, x);
    w_u16(destination + 2, y);
    w_u32(object + 348, 64);
    return 64;
}


