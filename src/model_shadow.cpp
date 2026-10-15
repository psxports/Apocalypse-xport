extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "model_shadow.h"
#include "model_projection.h"
#include <string.h>

static uint32 ReadShadowMatrixWord(const void *matrix, uint32 offset)
{
    uint32 value;
    memcpy(&value, (const unsigned char *)matrix + offset, 4);
    return value;
}

static uint32 ReadShadowMatrixHalf(const void *matrix, uint32 offset)
{
    uint16 value;
    memcpy(&value, (const unsigned char *)matrix + offset, 2);
    return value;
}

static uint32 DivideShadowValue(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void LoadShadowVector(const uint16 vector[3])
{
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, vector[axis]);
}

static void StoreShadowVector(uint16 vector[3])
{
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        vector[axis] = (uint16)xport_gte_read_data(9 + axis);
}

static void RotateShadowVector(const uint32 matrix[5])
{
    uint32 axis;
    xport_gte_read_data(9);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, matrix[axis]);
    xport_gte_execute(0x49E012);
}

static void CrossShadowVector(const uint16 vector[3])
{
    xport_gte_read_data(9);
    xport_gte_write_control(0, vector[0]);
    xport_gte_write_control(2, vector[1]);
    xport_gte_write_control(4, vector[2]);
    xport_gte_execute(0x178000C);
}

static void LoadShadowCorner(uint32 vertex, const uint16 center[3], const uint16 first[3], const uint16 second[3], uint32 add_first, uint32 add_second)
{
    uint32 values[3], axis;
    for (axis = 0; axis < 3; ++axis)
    {
        values[axis] = center[axis];
        values[axis] = add_first ? values[axis] + first[axis] : values[axis] - first[axis];
        values[axis] = add_second ? values[axis] + second[axis] : values[axis] - second[axis];
        values[axis] &= 65535;
    }
    xport_gte_write_data(vertex * 2, values[0] | (values[1] << 16));
    xport_gte_write_data(vertex * 2 + 1, values[2]);
}

uint32 RenderModelShadow(uint32 model, const void *matrix, uint32 rgb, uint32 scale, uint32 depth_bias, uint32 texture)
{
    uint32 packed, minimum, maximum, product, axis, camera, rotation[5], transpose[5], stretched[5];
    uint32 extents[3], average, numerator, length, squares[3], factor, depth, packet, result, projected, bucket;
    uint16 center[3], relative[3], normalized[3], basis[3], first[3], second[3], selected[3];
    FUNCTION_MARKER(0x8007B2E4, "SLUS_003.73");
    for (axis = 0; axis < 3; ++axis)
    {
        packed = r_u32(model + 20 + axis * 4);
        minimum = packed & 65535;
        maximum = (uint32)((sint32)packed >> 16);
        product = (uint32)((sint32)(minimum - maximum) >> 1) * scale;
        extents[axis] = (uint32)((sint32)product >> 12);
        packed = r_u32(model + 20 + axis * 4);
        center[axis] = (uint16)((sint32)((packed & 65535) + (uint32)((sint32)packed >> 16)) >> 1);
    }
    LoadShadowVector(center);
    for (axis = 0; axis < 5; ++axis)
        rotation[axis] = ReadShadowMatrixWord(matrix, axis * 4);
    RotateShadowVector(rotation);
    StoreShadowVector(center);
    for (axis = 0; axis < 3; ++axis)
        center[axis] = (uint16)(center[axis] + ReadShadowMatrixHalf(matrix, 20 + axis * 4));
    camera = r_u32(0x800FFB0C);
    for (axis = 0; axis < 3; ++axis)
        center[axis] = (uint16)(center[axis] - r_u16(camera + 4 + axis * 4));
    LoadShadowVector(center);
    NormalizeProjectionIr();
    StoreShadowVector(relative);
    for (axis = 0; axis < 3; ++axis)
        extents[axis] = (uint32)(sint32)(sint16)extents[axis];
    average = (uint32)((sint32)(extents[0] + extents[1] + extents[2]) / 3);
    numerator = (uint32)((sint32)(average << 16) >> 4);
    for (axis = 0; axis < 4; ++axis)
        rotation[axis] = ReadShadowMatrixWord(matrix, axis * 4);
    rotation[4] = ReadShadowMatrixHalf(matrix, 16);
    transpose[0] = ((rotation[0] ^ rotation[1]) & 65535) ^ rotation[1];
    transpose[1] = ((rotation[3] ^ rotation[0]) & 65535) ^ rotation[0];
    transpose[2] = ((rotation[2] ^ rotation[3]) & 65535) ^ rotation[3];
    transpose[3] = ((rotation[1] ^ rotation[2]) & 65535) ^ rotation[2];
    transpose[4] = rotation[4];
    stretched[0] = DivideShadowValue(numerator, extents[0]) & 65535;
    stretched[1] = 0;
    stretched[2] = DivideShadowValue(numerator, extents[1]) & 65535;
    stretched[3] = 0;
    stretched[4] = DivideShadowValue(numerator, extents[2]) & 65535;
    LoadShadowVector(relative);
    RotateShadowVector(transpose);
    NormalizeProjectionIr();
    RotateShadowVector(stretched);
    StoreShadowVector(normalized);
    length = MeasureProjectionIrLength();
    for (axis = 0; axis < 3; ++axis)
        squares[axis] = xport_gte_read_data(25 + axis);
    selected[0] = selected[1] = selected[2] = 0;
    if ((sint32)squares[1] >= (sint32)squares[0] && (sint32)squares[2] >= (sint32)squares[0])
        selected[0] = 4096;
    else if ((sint32)squares[0] >= (sint32)squares[1] && (sint32)squares[2] >= (sint32)squares[1])
        selected[1] = 4096;
    else
        selected[2] = 4096;
    LoadShadowVector(normalized);
    CrossShadowVector(selected);
    StoreShadowVector(basis);
    NormalizeProjectionIr();
    factor = extents[0] * extents[1];
    average = (uint32)(sint32)(sint16)average;
    factor = DivideShadowValue(factor, average);
    factor *= extents[2];
    factor *= (uint32)(sint32)(sint16)length;
    factor = DivideShadowValue(factor, average);
    factor = (uint32)((sint32)factor >> 12);
    RotateShadowVector(stretched);
    RotateShadowVector(rotation);
    CrossShadowVector(relative);
    ScaleProjectionIr(factor);
    StoreShadowVector(first);
    LoadShadowVector(normalized);
    CrossShadowVector(basis);
    NormalizeProjectionIr();
    RotateShadowVector(stretched);
    RotateShadowVector(rotation);
    CrossShadowVector(relative);
    ScaleProjectionIr(factor);
    StoreShadowVector(second);
    camera = r_u32(0x800FFB0C);
    sub_800878DC(camera + 116);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    LoadShadowCorner(0, center, first, second, 0, 0);
    LoadShadowCorner(1, center, first, second, 1, 0);
    LoadShadowCorner(2, center, first, second, 0, 1);
    xport_gte_execute(0x280030);
    w_u32(r_u32(0x800FFAC0), xport_gte_read_data(12));
    w_u32(r_u32(0x800FFAC0) + 8, xport_gte_read_data(13));
    w_u32(r_u32(0x800FFAC0) + 16, xport_gte_read_data(14));
    LoadShadowCorner(0, center, first, second, 1, 1);
    xport_gte_execute(0x180001);
    w_u32(r_u32(0x800FFAC0) + 24, xport_gte_read_data(14));
    xport_gte_execute(0x168002E);
    depth = xport_gte_read_data(7) + depth_bias;
    if (depth >= 4096)
        depth = 4095;
    packet = r_u32(0x800FF668);
    result = r_u32(0x800FF374) < packet + 40;
    w_u32(0x800FF668, packet + 40);
    if (result)
    {
        w_u32(0x800FF668, packet);
        return result;
    }
    w_u32(packet + 4, rgb);
    sub_8008AFFC(packet);
    w_u8(packet + 7, r_u8(packet + 7) | 2);
    projected = r_u32(0x800FFAC0);
    for (axis = 0; axis < 4; ++axis)
    {
        w_u16(packet + 8 + axis * 8, r_u16(projected + axis * 8));
        w_u16(packet + 10 + axis * 8, r_u16(projected + 2 + axis * 8));
    }
    w_u8(packet + 12, r_u8(texture));
    w_u8(packet + 13, r_u8(texture + 1));
    w_u8(packet + 20, r_u8(texture + 4) - 1);
    w_u8(packet + 21, r_u8(texture + 5));
    w_u8(packet + 28, r_u8(texture + 8));
    w_u8(packet + 29, r_u8(texture + 9) - 1);
    w_u8(packet + 36, r_u8(texture + 10) - 1);
    w_u8(packet + 37, r_u8(texture + 11) - 1);
    w_u16(packet + 14, r_u16(texture + 2));
    product = r_u16(texture + 6);
    packed = r_u32(packet);
    w_u16(packet + 22, (product & 0xFF9F) | 32);
    bucket = r_u32(0x800FF660) + depth * 4 + 112;
    w_u32(packet, (packed & 0xFF000000) | (r_u32(bucket) & 0xFFFFFF));
    result = (r_u32(bucket) & 0xFF000000) | (packet & 0xFFFFFF);
    w_u32(bucket, result);
    return result;
}

extern "C" uint32 sub_8007B2E4(uint32 model, uint32 matrix, uint32 rgb, uint32 scale, uint32 depth_bias, uint32 texture)
{
    FUNCTION_MARKER(0x8007B2E4, "SLUS_003.73");
    return RenderModelShadow(model, psx_addr(matrix, 32), rgb, scale, depth_bias, texture);
}
extern "C" uint32 apocalypse_model_shadow_native(uint32 model, const void *matrix, uint32 rgb, uint32 scale, uint32 depth_bias, uint32 texture) { return RenderModelShadow(model, matrix, rgb, scale, depth_bias, texture); }
