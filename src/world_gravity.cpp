#include <string.h>

#include "world_gravity.h"
#include "global.h"

WorldGravity world_gravity = {0};

uint32 WorldGravity::SetVector(const Vector3 *vector)
{
    FUNCTION_MARKER(0x8003AA20, "SLUS_003.73");
    world_gravity.vector = *vector;
    uint32 mag = world_gravity.vector.Len();
    world_gravity.magnitude = mag;
    uint32 divisor = -(sint64)mag;
    world_gravity.opposed.Divide(world_gravity.vector, divisor);
    world_gravity.tenth = (uint32)((sint32)world_gravity.magnitude / 10);
    return world_gravity.tenth;
}

void WorldGravity::Reset()
{
    FUNCTION_MARKER(0x8003B948, "SLUS_003.73");
    world_gravity.vector.x = 0;
    world_gravity.vector.y = 0x8000;
    world_gravity.vector.z = 0;
}
