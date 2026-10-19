#include <string.h>
#include <stdint.h>

#include "geometry_commands.h"
#include "game_diagnostics.h"
#include "angle_math.h"
#include "vector_distance.h"
#include "vector_arithmetic.h"

sint16 ShortestCameraAngleDelta(sint16 source_angle, sint16 target_angle)
{
    sint32 delta;
    FUNCTION_MARKER(0x80077274, "SLUS_003.73");
    delta = (target_angle & 4095) - (source_angle & 4095);
    if (delta > 2048)
        delta -= 4096;
    else if (delta < -2048)
        delta += 4096;
    return delta;
}

uint32 SnapSmallAngles(uint16 *angles)
{
    uint32 axis, value, small = 0;
    FUNCTION_MARKER(0x8006C64C, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        value = angles[axis];
        small = ((value + 1) & 65535) < 3;
        if (small)
            angles[axis] = 0;
    }
    // Original return describes the final angle only
    return small;
}

uint32 NormalizeAngles(uint16 *angles)
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x8006C624, "SLUS_003.73");
    x = angles[0];
    z = angles[2];
    angles[0] = x & 4095;
    y = angles[1];
    angles[2] = z & 4095;
    y &= 4095;
    angles[1] = y;
    return y;
}

uint16 *AddAngleVector(uint16 *angles, const uint16 *source)
{
    uint32 axis, left, right;
    FUNCTION_MARKER(0x8006C730, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        left = angles[axis];
        right = source[axis];
        angles[axis] = left + right;
    }
    return angles;
}

uint16 *SubtractAngleVector(uint16 *angles, const uint16 *source)
{
    uint32 axis, left, right;
    FUNCTION_MARKER(0x8006C774, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        left = angles[axis];
        right = source[axis];
        angles[axis] = left - right;
    }
    return angles;
}

uint16 *DivideAngleVector(uint16 *angles, const uint32 &divisor)
{
    uint32 axis, denominator, quotient;
    sint32 numerator;
    FUNCTION_MARKER(0x8006C808, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        numerator = (sint16)angles[axis];
        denominator = divisor;
        if (!denominator)
            quotient = numerator < 0 ? 1 : 0xFFFFFFFF;
        else
            quotient = (uint32)(numerator / (sint32)denominator);
        angles[axis] = quotient;
    }
    return angles;
}

uint16 *SubtractAngleVector(uint16 *output, const uint16 *left, const uint16 *right)
{
    uint32 axis, a, b;
    ShortVector3 difference;
    FUNCTION_MARKER(0x8006CB28, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        a = left[axis];
        b = right[axis];
        difference.words[axis] = a - b;
    }
    output[0] = difference.words[0];
    output[1] = difference.words[1];
    output[2] = difference.words[2];
    return output;
}

uint16 *DampAngles(uint16 *angles, const uint8 *shifts)
{
    uint32 axis, amount, value, delta;
    sint32 signed_value, next_signed = 0;
    FUNCTION_MARKER(0x8006C8E8, "SLUS_003.73");
    signed_value = (sint16)angles[0];
    for (axis = 0; axis < 3; axis += 1)
    {
        amount = shifts[axis];
        value = angles[axis];
        delta = value - (uint32)(signed_value >> (amount & 31));
        if (axis < 2)
            next_signed = (sint16)angles[axis + 1];
        angles[axis] = delta;
        signed_value = next_signed;
    }
    return angles;
}

static void WriteDirectionWord(uint32 *output, uint32 axis, uint32 value)
{
    memcpy((uint8 *)output + axis * 4, &value, 4);
}

uint32 BuildDirectionVector(uint32 *output, uint32 scale, const uint16 *angles)
{
    uint32 pitch, yaw, product, value, table_value;
    FUNCTION_MARKER(0x800667CC, "SLUS_003.73");
    pitch = angles[0] & 4095;
    table_value = (uint32)rcos(pitch);
    product = scale * table_value;
    yaw = angles[1] & 4095;
    table_value = (uint32)rsin(yaw);
    value = 0 - (uint32)((sint32)product >> 12) * table_value;
    WriteDirectionWord(output, 0, value);
    pitch = angles[0] & 4095;
    table_value = (uint32)rsin(pitch);
    value = scale * table_value;
    WriteDirectionWord(output, 1, value);
    pitch = angles[0] & 4095;
    table_value = (uint32)rcos(pitch);
    product = scale * table_value;
    yaw = angles[1] & 4095;
    table_value = (uint32)rcos(yaw);
    value = 0 - (uint32)((sint32)product >> 12) * table_value;
    WriteDirectionWord(output, 2, value);
    return value;
}

uint32 *OffsetPositionByAngles(uint32 *position, const uint16 *angles)
{
    Vector3 direction;
    FUNCTION_MARKER(0x80022B54, "SLUS_003.73");
    BuildDirectionVector(direction.words, 100, angles);
    return SubtractVectors(position, direction.words);
}

// Read the selected lanes of an original partial-word load

uint32 CopyAngleVector(uint16 *output, const uint16 *source)
{
    uint32 word;
    uint16 tail;
    memcpy(&word, source, 4);
    memcpy(&tail, (const uint8 *)source + 4, 2);
    memcpy(output, &word, 4);
    memcpy((uint8 *)output + 4, &tail, 2);
    return word;
}

uint32 UpdateAngleAcceleration(ShortVector3 cur, ShortVector3 &velocity, ShortVector3 &acceleration, ShortVector3 target, uint32 scale)
{
    sint16 dx = (sint16)((uint16)target.x - (uint16)cur.x);
    sint16 dy = (sint16)((uint16)target.y - (uint16)cur.y);
    uint32 result;
    FUNCTION_MARKER(0x80066CF0, "SLUS_003.73");
    if (dx < -2048)
        dx = (sint16)((uint16)dx + 4096);
    if (dx > 2048)
        dx = (sint16)((uint16)dx - 4096);
    if (dy < -2048)
        dy = (sint16)((uint16)dy + 4096);
    if (dy > 2048)
        dy = (sint16)((uint16)dy - 4096);
    if (!dx && !dy)
    {
        velocity.words[0] = 0;
        velocity.words[1] = 0;
        return 0;
    }
    result = (uint32)((sint32)(scale * (uint32)(sint32)dx) >> 8);
    acceleration.words[0] = result;
    result = (uint32)((sint32)(scale * (uint32)(sint32)dy) >> 8);
    acceleration.words[1] = result;
    return result;
}

static uint32 ReadDirectionPosition(const uint32 *position, uint32 axis)
{
    uint32 value;
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

static uint32 DivideDirectionSigned(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 CalculateDirectionAngles(uint16 *angles, const uint32 *origin, const uint32 *target)
{
    uint32 target_x, origin_x, target_y, origin_y, origin_z, target_z, distance, angle, pitch, yaw;
    sint32 x, y, z;
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    target_x = ReadDirectionPosition(target, 0);
    origin_x = ReadDirectionPosition(origin, 0);
    target_y = ReadDirectionPosition(target, 1);
    x = (sint32)(target_x - origin_x) >> 12;
    origin_y = ReadDirectionPosition(origin, 1);
    origin_z = ReadDirectionPosition(origin, 2);
    target_z = ReadDirectionPosition(target, 2);
    z = (sint32)(target_z - origin_z) >> 12;
    y = (sint32)(target_y - origin_y) >> 12;
    if (z > 0)
        angle = 2048 - CalculateAngleFromSlope((sint32)DivideDirectionSigned((uint32)x << 12, 0 - (uint32)z));
    else if (z < 0)
        angle = CalculateAngleFromSlope((sint32)DivideDirectionSigned((uint32)x << 12, (uint32)z));
    else
        angle = x > 0 ? (uint32)-1024 : 1024;
    angles[1] = angle;
    distance = LookupDistanceRoot((uint32)x * (uint32)x + (uint32)z * (uint32)z);
    if (!distance)
        angle = y > 0 ? 1024 : (uint32)-1024;
    else if (y > 0)
        angle = CalculateAngleFromSlope((sint32)(((uint32)y << 12) / distance));
    else
        angle = 0 - CalculateAngleFromSlope((sint32)(((0 - (uint32)y) << 12) / distance));
    angles[0] = angle;
    pitch = angles[0];
    yaw = angles[1];
    angles[2] = 0;
    angles[0] = pitch & 4095;
    angles[1] = yaw & 4095;
    return distance;
}

static uint32 ReadYawVectorWord(const uint32 *input, uint32 axis)
{
    uint32 value;
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

uint32 RotateVectorYaw(uint32 *output, const uint32 *input, uint32 angle)
{
    uint32 component, coefficient, left, right, value;
    FUNCTION_MARKER(0x800665CC, "SLUS_003.73");
    component = ReadYawVectorWord(input, 0);
    coefficient = (uint32)rcos(angle);
    left = (uint32)((sint32)component >> 3) * coefficient;
    component = ReadYawVectorWord(input, 2);
    coefficient = (uint32)rsin(angle);
    right = (uint32)((sint32)component >> 3) * coefficient;
    value = (uint32)((sint32)(left + right) >> 9);
    WriteDirectionWord(output, 0, value);
    value = ReadYawVectorWord(input, 1);
    WriteDirectionWord(output, 1, value);
    component = ReadYawVectorWord(input, 2);
    coefficient = (uint32)rcos(angle);
    left = (uint32)((sint32)component >> 3) * coefficient;
    component = ReadYawVectorWord(input, 0);
    coefficient = (uint32)rsin(angle);
    right = (uint32)((sint32)component >> 3) * coefficient;
    value = (uint32)((sint32)(left - right) >> 9);
    WriteDirectionWord(output, 2, value);
    return value;
}

void LoadFixedGteTranslation(const uint32 *position)
{
    FUNCTION_MARKER(0x80084504, "SLUS_003.73");
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    xport_gte_write_control(5, (uint32)((sint32)x >> 12));
    xport_gte_write_control(6, (uint32)((sint32)y >> 12));
    xport_gte_write_control(7, (uint32)((sint32)z >> 12));
}

uint32 InverseCosineAngle(uint32 value)
{
    FUNCTION_MARKER(0x80067930, "SLUS_003.73");
    uint32 angle = 512, negative = (sint32)value < 0;
    sint32 step = 256;
    if (negative)
        value = 0 - value;
    do
    {
        sint32 cosine = rcos(angle);
        if ((sint32)value < cosine)
            angle += (uint32)step;
        else if ((sint32)value == cosine)
            break;
        else
            angle -= (uint32)step;
        step >>= 1;
    } while (step);
    return negative ? 2048 - angle : angle;
}

sint32 CalculateAngleFromSlope(sint32 slope)
{
    return catan(slope);
}
