#include <string.h>

#include "model_matrix.h"
#include "vector3.h"
#include "geometry_commands.h"

void InitializeModelIdentityMatrix(MATRIX &matrix)
{
    FUNCTION_MARKER(0x800854D8, "SLUS_003.73");
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            matrix.m[row][col] = row == col ? ONE : 0;
}

void TransformModelMatrix(const MATRIX &input, MATRIX &output)
{
    FUNCTION_MARKER(0x800854F4, "SLUS_003.73");
    uint32 rotation[5];
    Vector3 column[3];
    rotation[0] = ((uint16)input.m[0][0] | ((uint32)(uint16)input.m[0][1] << 16));
    rotation[1] = ((uint16)input.m[0][2] | ((uint32)(uint16)input.m[1][0] << 16));
    rotation[3] = ((uint16)input.m[2][0] | ((uint32)(uint16)input.m[2][1] << 16));
    xport_gte_write_data(9, rotation[0]);
    xport_gte_write_data(10, rotation[1] >> 16);
    xport_gte_write_data(11, rotation[3]);
    xport_gte_execute(0x49E012);
    rotation[2] = ((uint16)input.m[1][1] | ((uint32)(uint16)input.m[1][2] << 16));
    column[0].words[0] = xport_gte_read_data(25);
    column[0].words[1] = xport_gte_read_data(26);
    column[0].words[2] = xport_gte_read_data(27);
    xport_gte_write_data(9, rotation[0] >> 16);
    xport_gte_write_data(10, rotation[2]);
    xport_gte_write_data(11, rotation[3] >> 16);
    xport_gte_execute(0x49E012);
    rotation[4] = ((uint16)input.m[2][2]);
    column[1].words[0] = xport_gte_read_data(25);
    column[1].words[1] = xport_gte_read_data(26);
    column[1].words[2] = xport_gte_read_data(27);
    xport_gte_write_data(9, rotation[1]);
    xport_gte_write_data(10, rotation[2] >> 16);
    xport_gte_write_data(11, rotation[4]);
    xport_gte_execute(0x49E012);
    output.m[0][0] = column[0].words[0];
    output.m[0][1] = column[1].words[0];
    output.m[2][0] = column[0].words[2];
    output.m[2][1] = column[1].words[2];
    column[2].words[0] = xport_gte_read_data(25);
    column[2].words[1] = xport_gte_read_data(26);
    column[2].words[2] = xport_gte_read_data(27);
    output.m[0][2] = column[2].words[0];
    output.m[1][0] = column[0].words[1];
    output.m[1][1] = column[1].words[1];
    output.m[1][2] = column[2].words[1];
    output.m[2][2] = column[2].words[2];
}

void BuildModelRotationMatrix(const uint16 angles[3], MATRIX &matrix)
{
    FUNCTION_MARKER(0x800858FC, "SLUS_003.73");
    uint32 angle_xy = angles[0] | ((uint32)angles[1] << 16);
    uint32 angle_z, trig_x, trig_z, trig_y, c_x, control3, ir_z, ir_x, m11, m10, saved0, saved3;
    MATRIX saved_rotation;
    ReadRotMatrix(&saved_rotation);
    saved0 = ((uint16)saved_rotation.m[0][0] | ((uint32)(uint16)saved_rotation.m[0][1] << 16));
    saved3 = ((uint16)saved_rotation.m[2][0] | ((uint32)(uint16)saved_rotation.m[2][1] << 16));
    xport_gte_write_data(1, 0);
    angle_z = angles[2];
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
    matrix.m[0][0] = (sint16)((uint16)ir_x);
    matrix.m[2][0] = (sint16)((uint16)ir_z);
    matrix.m[1][0] = (sint16)((uint16)m10);
    xport_gte_write_data(0, c_x);
    ir_x = xport_gte_read_data(9);
    ir_z = xport_gte_read_data(11);
    xport_gte_execute(0x486012);
    matrix.m[0][1] = (sint16)((uint16)ir_x);
    matrix.m[2][1] = (sint16)((uint16)ir_z);
    matrix.m[1][2] = (sint16)((uint16)(0 - trig_x));
    matrix.m[1][1] = (sint16)((uint16)m11);
    xport_gte_write_control(0, saved0);
    xport_gte_write_control(3, saved3);
    ir_x = xport_gte_read_data(9);
    ir_z = xport_gte_read_data(11);
    matrix.m[0][2] = (sint16)((uint16)ir_x);
    matrix.m[2][2] = (sint16)((uint16)ir_z);
}

void ScaleModelRotationMatrix(const uint16 factors[3], MATRIX &matrix)
{
    FUNCTION_MARKER(0x80085A08, "SLUS_003.73");
    uint32 scale, x, y, z, prev_x, prev_y, prev_z;

    scale = (uint32)(sint32)(sint16)factors[0];
    x = (uint32)(sint32)(sint16)matrix.m[0][0];
    y = (uint32)(sint32)(sint16)matrix.m[1][0];
    z = (uint32)(sint32)(sint16)matrix.m[2][0];
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    ExecuteGeometryCommand(0x198003D);

    scale = (uint32)(sint32)(sint16)factors[1];
    x = (uint32)(sint32)(sint16)matrix.m[0][1];
    y = (uint32)(sint32)(sint16)matrix.m[1][1];
    z = (uint32)(sint32)(sint16)matrix.m[2][1];
    prev_x = xport_gte_read_data(9);
    prev_y = xport_gte_read_data(10);
    prev_z = xport_gte_read_data(11);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    matrix.m[0][0] = (sint16)((uint16)prev_x);
    matrix.m[1][0] = (sint16)((uint16)prev_y);
    ExecuteGeometryCommand(0x198003D);
    matrix.m[2][0] = (sint16)((uint16)prev_z);

    scale = (uint32)(sint32)(sint16)factors[2];
    x = (uint32)(sint32)(sint16)matrix.m[0][2];
    y = (uint32)(sint32)(sint16)matrix.m[1][2];
    z = (uint32)(sint32)(sint16)matrix.m[2][2];
    prev_x = xport_gte_read_data(9);
    prev_y = xport_gte_read_data(10);
    prev_z = xport_gte_read_data(11);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    matrix.m[0][1] = (sint16)((uint16)prev_x);
    matrix.m[1][1] = (sint16)((uint16)prev_y);
    ExecuteGeometryCommand(0x198003D);
    matrix.m[2][1] = (sint16)((uint16)prev_z);

    prev_x = xport_gte_read_data(9);
    prev_y = xport_gte_read_data(10);
    prev_z = xport_gte_read_data(11);
    matrix.m[0][2] = (sint16)((uint16)prev_x);
    matrix.m[1][2] = (sint16)((uint16)prev_y);
    matrix.m[2][2] = (sint16)((uint16)prev_z);
}

void TransposeRotationMatrix(MATRIX &matrix)
{
    FUNCTION_MARKER(0x80085ACC, "SLUS_003.73");
    sint16 m01 = matrix.m[0][1];
    sint16 m02 = matrix.m[0][2];
    sint16 m10 = matrix.m[1][0];
    sint16 m12 = matrix.m[1][2];
    sint16 m20 = matrix.m[2][0];
    sint16 m21 = matrix.m[2][1];
    matrix.m[1][0] = m01;
    matrix.m[2][0] = m02;
    matrix.m[0][1] = m10;
    matrix.m[2][1] = m12;
    matrix.m[0][2] = m20;
    matrix.m[1][2] = m21;
}

void TransposeRotationMatrix(const MATRIX &input, MATRIX &output)
{
    FUNCTION_MARKER(0x80085B04, "SLUS_003.73");
    MATRIX rotation;
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            rotation.m[row][col] = input.m[row][col];
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            output.m[col][row] = rotation.m[row][col];
}

void ClearModelRotationMatrix(MATRIX &output)
{
    FUNCTION_MARKER(0x80085BE8, "SLUS_003.73");
    memset(output.m, 0, sizeof(output.m));
}

void CopyModelRotationMatrix(const MATRIX &input, MATRIX &output)
{
    FUNCTION_MARKER(0x80085C04, "SLUS_003.73");
    memmove(output.m, input.m, sizeof(output.m));
}

void TransposeModelMatrix(const MATRIX &input, MATRIX &output)
{
    FUNCTION_MARKER(0x80085C34, "SLUS_003.73");
    MATRIX rotation;
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            rotation.m[row][col] = input.m[row][col];
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            output.m[row][col] = rotation.m[col][row];
}
