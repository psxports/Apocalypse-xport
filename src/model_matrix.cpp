#include <string.h>

#include "model_matrix.h"
#include "model_instance.h"
#include "vector3.h"
#include "psx.h"

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
    rotation[0] = (uint16)input.m[0][0] | ((uint32)(uint16)input.m[0][1] << 16);
    rotation[1] = (uint16)input.m[0][2] | ((uint32)(uint16)input.m[1][0] << 16);
    rotation[3] = (uint16)input.m[2][0] | ((uint32)(uint16)input.m[2][1] << 16);
    xport_gte_write_data(9, rotation[0]);
    xport_gte_write_data(10, rotation[1] >> 16);
    xport_gte_write_data(11, rotation[3]);
    xport_gte_execute(0x49E012);
    rotation[2] = (uint16)input.m[1][1] | ((uint32)(uint16)input.m[1][2] << 16);
    column[0].x = xport_gte_read_data(25);
    column[0].y = xport_gte_read_data(26);
    column[0].z = xport_gte_read_data(27);
    xport_gte_write_data(9, rotation[0] >> 16);
    xport_gte_write_data(10, rotation[2]);
    xport_gte_write_data(11, rotation[3] >> 16);
    xport_gte_execute(0x49E012);
    rotation[4] = (uint16)input.m[2][2];
    column[1].x = xport_gte_read_data(25);
    column[1].y = xport_gte_read_data(26);
    column[1].z = xport_gte_read_data(27);
    xport_gte_write_data(9, rotation[1]);
    xport_gte_write_data(10, rotation[2] >> 16);
    xport_gte_write_data(11, rotation[4]);
    xport_gte_execute(0x49E012);
    output.m[0][0] = (uint32)column[0].x;
    output.m[0][1] = (uint32)column[1].x;
    output.m[2][0] = (uint32)column[0].z;
    output.m[2][1] = (uint32)column[1].z;
    column[2].x = xport_gte_read_data(25);
    column[2].y = xport_gte_read_data(26);
    column[2].z = xport_gte_read_data(27);
    output.m[0][2] = (uint32)column[2].x;
    output.m[1][0] = (uint32)column[0].y;
    output.m[1][1] = (uint32)column[1].y;
    output.m[1][2] = (uint32)column[2].y;
    output.m[2][2] = (uint32)column[2].z;
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
    memcpy(rotation.m, input.m, sizeof(rotation.m));
    TransposeMatrix(&rotation, &output);
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
    TransposeRotationMatrix(input, output);
}
