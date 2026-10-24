#include "short_vector3.h"
#include "vector3.h"
#include "global.h"

uint32 ShortVector3::SetDir(const Vector3 &origin, const Vector3 &target)
{
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    sint32 dx = (uint32)target.x - origin.x;
    sint32 dy = (uint32)target.y - origin.y;
    sint32 dz = (uint32)target.z - origin.z;
    dx >>= 12;
    dy >>= 12;
    dz >>= 12;
    sint32 angle;
    if (dz > 0)
        angle = 2048 - catan(DivideSignedWord(dx * ONE, -dz));
    else if (dz < 0)
        angle = catan(DivideSignedWord(dx * ONE, dz));
    else
        angle = dx > 0 ? -1024 : 1024;
    y = angle;
    uint32 horizontal_x = dx;
    uint32 horizontal_z = dz;
    uint32 distance = LookupDistanceRoot(horizontal_x * horizontal_x + horizontal_z * horizontal_z);
    if (!distance)
        angle = dy > 0 ? 1024 : -1024;
    else
    {
        uint32 magnitude = dy > 0 ? dy : -dy;
        sint32 slope = magnitude * ONE / distance;
        angle = dy > 0 ? catan(slope) : -catan(slope);
    }
    x = angle;
    z = 0;
    x &= 4095;
    y &= 4095;
    return distance;
}

uint32 ShortVector3::NormalizeAngles()
{
    FUNCTION_MARKER(0x8006C624, "SLUS_003.73");
    x = (uint16)x & (4095);
    z = (uint16)z & (4095);
    y = (uint16)y & (4095);
    return (uint16)y;
}

bool ShortVector3::SnapAngles()
{
    FUNCTION_MARKER(0x8006C64C, "SLUS_003.73");
    bool small = false;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint32 val = comp[axis];
        small = val >= -1 && val <= 1;
        if (small)
            comp[axis] = 0;
    }
    // Original return describes the final angle only
    return small;
}

ShortVector3 &ShortVector3::AddAngles(const ShortVector3 &src)
{
    FUNCTION_MARKER(0x8006C730, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        comp[axis] = (uint16)comp[axis] + ((uint16)src.comp[axis]);
    return *this;
}

ShortVector3 &ShortVector3::SubtractAngles(const ShortVector3 &src)
{
    FUNCTION_MARKER(0x8006C774, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        comp[axis] = (uint16)comp[axis] - ((uint16)src.comp[axis]);
    return *this;
}

ShortVector3 &ShortVector3::DivideAngles(const uint32 &divisor)
{
    FUNCTION_MARKER(0x8006C808, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        comp[axis] = DivideSignedWord(comp[axis], divisor);
    return *this;
}

ShortVector3 &ShortVector3::DampAngles(const uint8 *shifts)
{
    FUNCTION_MARKER(0x8006C8E8, "SLUS_003.73");
    uint32 axis, amount, val, delta;
    sint32 signed_val, next_signed = 0;
    signed_val = x;
    for (axis = 0; axis < 3; axis += 1)
    {
        amount = shifts[axis];
        val = (uint16)comp[axis];
        delta = val - (signed_val >> (amount & 31));
        if (axis < 2)
            next_signed = comp[axis + 1];
        comp[axis] = delta;
        signed_val = next_signed;
    }
    return *this;
}

ShortVector3 &ShortVector3::AddAngles(const ShortVector3 &left, const ShortVector3 &right)
{
    FUNCTION_MARKER(0x8006CAC8, "SLUS_003.73");
    ShortVector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
        result.comp[axis] = (uint16)left.comp[axis] + (uint16)right.comp[axis];
    *this = result;
    return *this;
}

ShortVector3 &ShortVector3::SubtractAngles(const ShortVector3 &left, const ShortVector3 &right)
{
    FUNCTION_MARKER(0x8006CB28, "SLUS_003.73");
    ShortVector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
        result.comp[axis] = (uint16)left.comp[axis] - (uint16)right.comp[axis];
    *this = result;
    return *this;
}

ShortVector3 &ShortVector3::Scale(const uint32 &scalar, const ShortVector3 &vector)
{
    FUNCTION_MARKER(0x8006CBF8, "SLUS_003.73");
    ShortVector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
        result.comp[axis] = vector.comp[axis] * scalar;
    *this = result;
    return *this;
}

ShortVector3 &ShortVector3::ShiftRight(const ShortVector3 &vector, const uint32 &shift)
{
    FUNCTION_MARKER(0x8006CCE0, "SLUS_003.73");
    ShortVector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
        result.comp[axis] = vector.comp[axis] >> (shift & 31);
    *this = result;
    return *this;
}

void ShortVector3::BuildRotationMatrix(MATRIX &matrix) const
{
    FUNCTION_MARKER(0x800858FC, "SLUS_003.73");
    uint32 angle_xy = (uint16)x | ((uint32)(uint16)y << 16);
    uint32 angle_z, trig_x, trig_z, trig_y, c_x, control3, ir_z, ir_x, m11, m10, saved0, saved3;
    MATRIX saved_rotation;
    ReadRotMatrix(&saved_rotation);
    saved0 = (uint16)saved_rotation.m[0][0] | ((uint32)(uint16)saved_rotation.m[0][1] << 16);
    saved3 = (uint16)saved_rotation.m[2][0] | ((uint32)(uint16)saved_rotation.m[2][1] << 16);
    xport_gte_write_data(1, 0);
    angle_z = (uint16)z;
    trig_x = (uint16)rsin(angle_xy) | ((uint32)(uint16)rcos(angle_xy) << 16);
    trig_z = (uint16)rsin(angle_z) | ((uint32)(uint16)rcos(angle_z) << 16);
    xport_gte_write_control(0, trig_x);
    c_x = trig_x >> 16;
    xport_gte_write_control(3, c_x);
    xport_gte_write_data(0, trig_z & 0xFFFF);
    xport_gte_execute(0x486012);
    trig_y = (uint16)rsin(angle_xy >> 16) | ((uint32)(uint16)rcos(angle_xy >> 16) << 16);
    xport_gte_write_data(0, trig_z >> 16);
    control3 = (trig_y >> 16) - (trig_y << 16);
    ir_x = xport_gte_read_data(9);
    m10 = xport_gte_read_data(11);
    xport_gte_execute(0x486012);
    xport_gte_write_control(0, trig_y);
    xport_gte_write_control(3, control3);
    ir_x = ((ir_x ^ trig_z) & 0xFFFF) ^ trig_z;
    xport_gte_write_data(0, ir_x);
    m11 = xport_gte_read_data(11);
    ir_x = xport_gte_read_data(9);
    xport_gte_execute(0x486012);
    ir_x = (ir_x & 0xFFFF) - (trig_z << 16);
    xport_gte_write_data(0, ir_x);
    ir_x = xport_gte_read_data(9);
    ir_z = xport_gte_read_data(11);
    xport_gte_execute(0x486012);
    matrix.m[0][0] = ir_x;
    matrix.m[2][0] = ir_z;
    matrix.m[1][0] = m10;
    xport_gte_write_data(0, c_x);
    ir_x = xport_gte_read_data(9);
    ir_z = xport_gte_read_data(11);
    xport_gte_execute(0x486012);
    matrix.m[0][1] = ir_x;
    matrix.m[2][1] = ir_z;
    matrix.m[1][2] = -(sint64)trig_x;
    matrix.m[1][1] = m11;
    xport_gte_write_control(0, saved0);
    xport_gte_write_control(3, saved3);
    ir_x = xport_gte_read_data(9);
    ir_z = xport_gte_read_data(11);
    matrix.m[0][2] = ir_x;
    matrix.m[2][2] = ir_z;
}

void ShortVector3::BuildRotationMatrixf(MATRIXF &matrix) const
{
    VECTORF angles;
    angles.vx = (x & 4095) * RAD12;
    angles.vy = (y & 4095) * RAD12;
    angles.vz = (z & 4095) * RAD12;
    IdentityMatrixf(&matrix);
    RotMatrixYXZf(&angles, &matrix);
}
