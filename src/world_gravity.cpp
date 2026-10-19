#include <string.h>

#include "world_gravity.h"
#include "vector_distance.h"
#include "vector_arithmetic.h"

WorldGravity world_gravity = {0};

void ResetGravityVector()
{
    FUNCTION_MARKER(0x8003B948, "SLUS_003.73");
    world_gravity.vector.words[0] = 0;
    world_gravity.vector.words[1] = 0x8000;
    world_gravity.vector.words[2] = 0;
}

uint32 SetGravityVector(const uint32 *vector)
{
    FUNCTION_MARKER(0x8003AA20, "SLUS_003.73");
    Vector3 values;
    memcpy(values.words, vector, sizeof(values.words));
    world_gravity.vector = values;
    uint32 magnitude = MeasureVectorLength(world_gravity.vector.words);
    world_gravity.magnitude = magnitude;
    uint32 divisor = 0 - magnitude;
    DivideVector(world_gravity.opposed.words, world_gravity.vector.words, divisor);
    world_gravity.tenth = (uint32)((sint32)world_gravity.magnitude / 10);
    return world_gravity.tenth;
}
