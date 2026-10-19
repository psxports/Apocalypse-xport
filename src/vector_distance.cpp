#include <stdlib.h>

#include "game_diagnostics.h"

#include "vector_distance.h"
#include "vector3.h"

uint32 MeasurePositionDistance(const uint32 *origin, const uint32 *target)
{
    uint32 left, right, axis, value;
    Vector3 delta;
    sint32 x, y, z;
    FUNCTION_MARKER(0x8006696C, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        left = origin[axis];
        right = target[axis];
        delta.words[axis] = left - right;
    }
    for (axis = 0; axis < 3; axis += 1)
    {
        if ((sint32)delta.words[axis] < 0)
            delta.words[axis] = 0 - delta.words[axis];
    }
    x = (sint32)delta.words[0];
    y = (sint32)delta.words[1];
    z = (sint32)delta.words[2];
    if (x < y)
    {
        if (y < z)
            value = (uint32)(x >> 2) + (uint32)(y >> 1) + (uint32)z;
        else if (z < x)
            value = (uint32)(z >> 2) + (uint32)(x >> 1) + (uint32)y;
        else
            value = (uint32)(x >> 2) + (uint32)(z >> 1) + (uint32)y;
    }
    else
    {
        if (x < z)
            value = (uint32)(y >> 2) + (uint32)(x >> 1) + (uint32)z;
        else if (z < y)
            value = (uint32)(z >> 2) + (uint32)(y >> 1) + (uint32)x;
        else
            value = (uint32)(y >> 2) + (uint32)(z >> 1) + (uint32)x;
    }
    return (uint32)((sint32)value >> 12);
}

uint32 MeasureHorizontalDistance(const uint32 *origin, const uint32 *target)
{
    uint32 x, z, left, right;
    FUNCTION_MARKER(0x80066918, "SLUS_003.73");
    left = origin[0];
    right = target[0];
    x = (uint32)((sint32)(left - right) >> 12);
    x *= x;
    left = origin[2];
    right = target[2];
    z = (uint32)((sint32)(left - right) >> 12);
    z *= z;
    return LookupDistanceRoot(x + z);
}

// Share the original shifted input and GTE command sequence
static uint32 SquareShiftedVector(const uint32 *vector)
{
    uint32 squares[3], axis;
    Vector3 shifted;
    for (axis = 0; axis < 3; axis += 1)
        shifted.words[axis] = (uint32)((sint32)vector[axis] >> 12);
    for (axis = 0; axis < 3; axis += 1)
        xport_gte_write_data(9 + axis, shifted.words[axis]);
    xport_gte_execute(0xA00428);
    for (axis = 0; axis < 3; axis += 1)
        squares[axis] = xport_gte_read_data(25 + axis);
    return squares[0] + squares[1] + squares[2];
}

uint32 MeasureVectorLength(const uint32 *vector)
{
    FUNCTION_MARKER(0x8006BF04, "SLUS_003.73");
    return LookupDistanceRoot(SquareShiftedVector(vector));
}

uint32 MeasureSquaredVectorLength(const uint32 *vector)
{
    FUNCTION_MARKER(0x8006BF8C, "SLUS_003.73");
    return SquareShiftedVector(vector);
}

uint32 MeasureEuclideanDistance(const uint32 *origin, const uint32 *target)
{
    uint32 left, right, x, y, z;
    FUNCTION_MARKER(0x8006689C, "SLUS_003.73");
    left = origin[0];
    right = target[0];
    x = (uint32)((sint32)(left - right) >> 12);
    left = origin[1];
    right = target[1];
    y = (uint32)((sint32)(left - right) >> 12);
    right = target[2];
    left = origin[2];
    z = (uint32)((sint32)(left - right) >> 12);
    return LookupDistanceRoot(x * x + y * y + z * z);
}

static const uint16 distance_root_values[3072] = {
#include "distance_root_table.inc"
};

uint32 LookupDistanceRoot(uint32 value)
{
    FUNCTION_MARKER(0x80085B54, "SLUS_003.73");
    xport_gte_write_data(30, value);
    uint32 shift = xport_gte_read_data(31) & 254;
    if (!value)
        return 0;
    uint32 index = (sint32)value < 0 ? value >> 20 : (value << (shift & 31)) >> 20;
    if (index < 1024 || index >= 4096)
    {
        GameDiagnostic("Invalid normalized distance-root index\n");
        abort();
    }
    uint32 result = distance_root_values[index - 1024];
    return (sint32)value < 0 ? result : result >> ((shift >> 1) & 31);
}
