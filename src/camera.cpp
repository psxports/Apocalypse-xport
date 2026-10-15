#include "game_sdk_bindings.h"
#include "quaternion_math.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>
#include "camera.h"
#include "game_heap.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

// Native extraction shared by the two original tracking methods

#include "angle_math.h"
#include "vector_arithmetic.h"
#include "object_collision_events.h"

// Native output adapter for the original quaternion matrix helper

// Native vector adapter preserves the original two GTE commands

uint32 sub_80077274(sint16 first, sint16 second);

// Native helper retains the original shake multiply, bias, SLL4 and SRA16

// Native extraction of repeated manual-camera key handling

// Native extraction of the original countdown and delta blocks

#include "camera.h"

// Original global transitions have no instance receiver

// Native adapter preserves the defined caller result when a list is empty

static uint32 DivideCameraWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 ReadCameraPositionWord(uint32 guest, const void *native, uint32 axis)
{
    uint32 value;
    if (!native)
        return r_u32(guest + axis * 4);
    memcpy(&value, (const uint8 *)native + axis * 4, 4);
    return value;
}

Camera &Camera::Attach(uint32 object)
{
    return *static_cast<Camera *>(FindNativeObjectStorage(object));
}

static void StoreTrackingAngles(uint32 destination, const sint16 angles[4])
{
    w_u32(destination, (uint32)(uint16)angles[0] | ((uint32)(uint16)angles[1] << 16));
    w_u16(destination + 4, (uint16)angles[2]);
}

static void TurnTrackingCamera(uint32 camera, sint32 delta, sint32 distance, int fine_turn)
{
    uint32 adjustment, maximum, value;
    if (distance >= 1001)
    {
        if (delta < (fine_turn ? -8 : -256))
        {
            if (delta < -256)
            {
                if (delta < -512)
                {
                    adjustment = (0 - (uint32)delta) - 512;
                    maximum = r_u32(camera + 572) << 1;
                    if ((sint32)maximum < (sint32)adjustment)
                        adjustment = maximum;
                    w_u16(camera + 494, r_u16(camera + 494) - adjustment);
                }
                value = r_u16(camera + 494);
                adjustment = r_u16(camera + 572);
                w_u16(camera + 494, value - adjustment);
            }
            else
                w_u16(camera + 494, r_u16(camera + 494) - 9);
        }
        if (delta >= (fine_turn ? 9 : 257))
        {
            if (delta >= 257)
            {
                if (delta >= 513)
                {
                    adjustment = (uint32)delta - 512;
                    maximum = r_u32(camera + 572) << 1;
                    if ((sint32)maximum < (sint32)adjustment)
                        adjustment = maximum;
                    w_u16(camera + 494, r_u16(camera + 494) + adjustment);
                }
                value = r_u16(camera + 494);
                adjustment = r_u16(camera + 572);
                w_u16(camera + 494, value + adjustment);
            }
            else
                w_u16(camera + 494, r_u16(camera + 494) + 9);
        }
        value = (uint32)(sint32)(sint16)r_u16(camera + 494);
        if ((sint32)value < 0)
        {
            do
            {
                value += 4096;
                w_u16(camera + 494, value);
            } while ((sint16)value < 0);
        }
    }
    w_u16(camera + 494, r_u16(camera + 494) & 4095);
}

static void BuildCameraQuaternionMatrix(uint32 quaternion, MATRIX *matrix)
{
    uint32 w = r_u32(quaternion + 12), x = r_u32(quaternion);
    uint32 wx = w * x, y = r_u32(quaternion + 4);
    uint32 wy = w * y, z = r_u32(quaternion + 8);
    uint32 wz = w * z, xx = x * x, xy = x * y, yy = y * y, zz = z * z;
    uint32 xz = x * z, yz;
    uint32 xx_shift, xy_shift, yy_shift, zz_shift, wx_shift, wy_shift, wz_shift, xz_shift, yz_shift;
    matrix->t[0] = 0;
    matrix->t[1] = 0;
    matrix->t[2] = 0;
    wz_shift = (uint32)((sint32)wz >> 11);
    yy_shift = (uint32)((sint32)yy >> 11);
    zz_shift = (uint32)((sint32)zz >> 11);
    xy_shift = (uint32)((sint32)xy >> 11);
    matrix->m[0][0] = (sint16)(4096 - (yy_shift + zz_shift));
    matrix->m[0][1] = (sint16)(xy_shift + wz_shift);
    matrix->m[1][0] = (sint16)(xy_shift - wz_shift);
    wy_shift = (uint32)((sint32)wy >> 11);
    xz_shift = (uint32)((sint32)xz >> 11);
    xx_shift = (uint32)((sint32)xx >> 11);
    matrix->m[1][1] = (sint16)(4096 - (xx_shift + zz_shift));
    wx_shift = (uint32)((sint32)wx >> 11);
    matrix->m[0][2] = (sint16)(xz_shift - wy_shift);
    matrix->m[2][0] = (sint16)(xz_shift + wy_shift);
    matrix->m[2][2] = (sint16)(4096 - (xx_shift + yy_shift));
    yz = y * z;
    yz_shift = (uint32)((sint32)yz >> 11);
    matrix->m[1][2] = (sint16)(yz_shift + wx_shift);
    matrix->m[2][1] = (sint16)(yz_shift - wx_shift);
}

static void RotateCameraVector(MATRIX *matrix, const uint32 input[3], uint32 output[3])
{
    uint32 high[3], low[3], high_mac[3], low_mac[3], packed[5];
    uint32 axis, value, magnitude;
    for (axis = 0; axis < 4; ++axis)
    {
        uint32 first = axis * 2, second = first + 1;
        packed[axis] = (uint16)matrix->m[first / 3][first % 3] | ((uint32)(uint16)matrix->m[second / 3][second % 3] << 16);
    }
    packed[4] = (uint16)matrix->m[2][2];
    for (axis = 0; axis < 5; ++axis)
        xport_draft_gte_control_write(axis, packed[axis]);
    for (axis = 0; axis < 3; ++axis)
    {
        value = input[axis];
        if ((sint32)value < 0)
        {
            magnitude = 0 - value;
            high[axis] = 0 - (uint32)((sint32)magnitude >> 15);
            low[axis] = 0 - (magnitude & 0x7FFF);
        }
        else
        {
            high[axis] = (uint32)((sint32)value >> 15);
            low[axis] = value & 0x7FFF;
        }
    }
    for (axis = 0; axis < 3; ++axis)
        xport_draft_gte_data_write(9 + axis, high[axis]);
    xport_draft_gte_execute(0x41E012);
    for (axis = 0; axis < 3; ++axis)
        high_mac[axis] = xport_draft_gte_data_read(25 + axis);
    for (axis = 0; axis < 3; ++axis)
        xport_draft_gte_data_write(9 + axis, low[axis]);
    xport_draft_gte_execute(0x49E012);
    for (axis = 0; axis < 3; ++axis)
        low_mac[axis] = xport_draft_gte_data_read(25 + axis);
    for (axis = 0; axis < 3; ++axis)
        output[axis] = low_mac[axis] + (high_mac[axis] << 3);
}

static uint32 ReadCameraCollisionAngle(uint32 record[35], sint16 strength, uint32 reverse)
{
    uint32 numerator, ratio, product;
    sint32 divisor = (sint32)record[17];
    if (!divisor)
        record[17] = 1;
    divisor = (sint32)record[17];
    numerator = (record[17] - record[16]) << 12;
    if (numerator == 0x80000000 && divisor == -1)
        ratio = numerator;
    else
        ratio = (uint32)((sint32)numerator / divisor);
    product = ratio * ratio;
    product = (uint32)(sint32)strength * (uint32)((sint32)product >> 12);
    if (reverse)
        return (uint32)(sint32)(sint16)(0 - (uint32)((sint32)product >> 12));
    return (uint32)((sint32)(product << 4) >> 16);
}

static void ApplyCameraCollisionQuaternion(uint32 object, const uint32 q[4])
{
    uint32 w = r_u32(object + 456), x = r_u32(object + 444);
    uint32 y = r_u32(object + 448), z = r_u32(object + 452);
    uint32 out_x = w * q[0] + x * q[3] + y * q[2] - z * q[1];
    uint32 out_y = w * q[1] + y * q[3] + z * q[0] - x * q[2];
    uint32 out_z = w * q[2] + z * q[3] + x * q[1] - y * q[0];
    uint32 out_w = w * q[3] - x * q[0] - y * q[1] - z * q[2];
    w_u32(object + 444, (uint32)((sint32)out_x >> 12));
    w_u32(object + 448, (uint32)((sint32)out_y >> 12));
    w_u32(object + 452, (uint32)((sint32)out_z >> 12));
    w_u32(object + 456, (uint32)((sint32)out_w >> 12));
}

static sint16 ReadCameraOscillation(uint32 object, uint32 amplitude_offset, uint32 frequency_offset)
{
    uint32 frequency = (uint32)(sint32)(sint16)r_u16(object + frequency_offset);
    uint32 time = r_u32(0x800FF2F0);
    uint32 phase = frequency * time;
    uint32 amplitude = (uint32)(sint32)(sint16)r_u16(object + amplitude_offset);
    uint32 sine = (uint32)(sint32)(sint16)r_u16(0x800F863C + (phase & 4095) * 4);
    sint32 product = (sint32)(amplitude * sine);
    if (product < 0)
        product = (sint32)((uint32)product + 4095);
    return (sint16)((sint32)((uint32)product << 4) >> 16);
}

static void AdjustCameraDebugValue(uint32 button, uint32 address, uint32 subtract)
{
    if (r_u8(button))
    {
        uint32 step = r_u8(0x800EC364) ? 32 : 8;
        uint32 value = r_u32(address);
        w_u32(address, subtract ? value - step : value + step);
    }
}

static void AdvanceCameraGlobalTransition(uint32 count_address, uint32 value_address, uint32 delta_address)
{
    uint32 count = r_u32(count_address);
    if (count)
    {
        --count;
        uint32 value = r_u32(value_address);
        uint32 delta = r_u32(delta_address);
        w_u32(count_address, count);
        w_u32(value_address, value + delta);
    }
}

static uint32 MarkShakeList(uint32 list, uint32 result)
{
    while (list)
    {
        result = r_u16(list + 76) | 2;
        w_u16(list + 76, result);
        list = r_u32(list + 28);
    }
    return result;
}

uint32 ClearCameraPresetFlags()
{
    FUNCTION_MARKER(0x80076D44, "SLUS_003.73");
    uint32 cursor = 0x801028B7;
    for (sint32 index = 15; index >= 0; --index)
    {
        w_u8(cursor, 0);
        --cursor;
    }
    return cursor;
}

Camera::Camera(const VisualObject &target) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 block, index, source, x, y, z, count, flags;
    FUNCTION_MARKER(0x800772B8, "SLUS_003.73");
    w_u32(object + 68, 0x800A3DA4);
    uint32 target_address = NativeObjectGuestAddress(&target);
    w_u32(object + 296, target_address);
    w_u32(object + 336, target_address);
    w_u32(object + 372, 256);
    w_u32(object + 384, 0);
    w_u16(object + 412, 0);
    w_u16(object + 414, 0);
    for (block = object + 428; block < object + 492; block += 16)
    {
        for (index = 0; index < 3; ++index)
            w_u32(block + index * 4, 0);
        w_u32(block + 12, 4096);
    }
    source = r_u32(object + 296);
    w_u32(object + 536, (uint32)-1);
    w_u32(object + 540, (uint32)-1);
    w_u16(object + 546, 784);
    w_u16(object + 544, 256);
    w_u32(object + 568, 0);
    w_u32(object + 572, 15);
    w_u32(object + 576, 0);
    x = r_u32(source + 4); y = r_u32(source + 8); z = r_u32(source + 12);
    w_u32(object + 308, x); w_u32(object + 312, y); w_u32(object + 316, z);
    w_u32(object + 332, 0);
    source = r_u32(object + 336);
    x = r_u32(source + 4); y = r_u32(source + 8); z = r_u32(source + 12);
    w_u32(object + 344, x); w_u32(object + 348, y); w_u32(object + 352, z);
    w_u16(object + 58, 99);
    w_u32(object + 368, 0);
    w_u16(object, 1);
    PrepareView();
    AddToList((uint32 *)psx_addr(0x800FF904, 4));
    w_u32(object + 564, 7);
    count = r_u32(0x800FF900);
    flags = r_u16(object + 78);
    w_u16(object + 212, 0);
    w_u16(object + 78, flags & 0xFFFD);
    w_u32(0x800FF900, count + 1);
}

uint32 Camera::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8007741C, "SLUS_003.73");
    w_u32(NativeObjectGuestAddress(this) + 68, 0x800A3DA4);
    RemoveFromList((uint32 *)psx_addr(0x800FF904, 4));
    w_u32(0x800FF900, r_u32(0x800FF900) - 1);
    result = VisualObject::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(NativeObjectGuestAddress(this));
    return result;
}

void Camera::SetColDistances(uint32 value)
{
    uint32 camera = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80077490, "SLUS_003.73");
    w_u32(camera + 536, value);
    w_u32(camera + 540, value);
}

void Camera::SetLateralColDistance(uint32 value)
{
    FUNCTION_MARKER(0x8007749C, "SLUS_003.73");
    w_u32(NativeObjectGuestAddress(this) + 536, value);
}

void Camera::SetRearColDistance(uint32 value)
{
    FUNCTION_MARKER(0x800774A4, "SLUS_003.73");
    w_u32(NativeObjectGuestAddress(this) + 540, value);
}

void Camera::SetColYawStrength(uint32 value)
{
    uint32 camera = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800774AC, "SLUS_003.73");
    w_u16(camera + 544, value);
}

void Camera::SetColPitchStrength(uint32 value)
{
    FUNCTION_MARKER(0x800774B4, "SLUS_003.73");
    w_u16(NativeObjectGuestAddress(this) + 546, value);
}

extern "C" uint32 sub_800774BC(uint32, uint32 list)
{
    FUNCTION_MARKER(0x800774BC, "SLUS_003.73");
    // TODO Empty standalone calls have no defined original return input
    return MarkShakeList(list, 0);
}

uint32 Camera::ApplyExplosionShake(uint32 kind)
{
    const uint32 camera = NativeObjectGuestAddress(this);
    uint32 result = kind == 1 ? 1 : 2;
    uint32 preset, byte0, byte1, byte2;
    FUNCTION_MARKER(0x800774EC, "SLUS_003.73");
    if (kind <= 2)
    {
        preset = kind == 0 ? 0x800FFCF8 : kind == 1 ? 0x800FFCF0 : 0x800FFCE8;
        CopyAngleVector((uint16 *)psx_addr(camera + 548, 6), (const uint16 *)psx_addr(preset, 6));
        byte0 = r_u8(0x800FFCE0);
        byte1 = r_u8(0x800FFCE1);
        byte2 = r_u8(0x800FFCE2);
        w_u8(camera + 554, byte0);
        w_u8(camera + 555, byte1);
        w_u8(camera + 556, byte2);
        CopyAngleVector((uint16 *)psx_addr(camera + 558, 6), (const uint16 *)psx_addr(0x800FFCD8, 6));
    }
    result = MarkShakeList(r_u32(0x800FF4E8), result);
    return MarkShakeList(r_u32(0x800FF5DC), result);
}

uint32 Camera::SetModePreset(uint32 mode)
{
    uint32 camera = NativeObjectGuestAddress(this);
    uint32 result = 20;
    FUNCTION_MARKER(0x80077624, "SLUS_003.73");
    mode &= 65535;
    if (!mode)
    {
        w_u32(camera + 564, 20);
        w_u32(camera + 572, 0);
    }
    else if (mode == 1 || mode == 2)
    {
        w_u32(camera + 564, mode == 1 ? 18 : 20);
        result = 16;
        w_u32(camera + 572, result);
    }
    return result;
}

uint32 Camera::PrepareView()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, offset_x, offset_y, offset_z;
    FUNCTION_MARKER(0x80077680, "SLUS_003.73");
    x = r_u32(object + 4);
    offset_x = r_u32(0x800ED510);
    w_u32(0x800ED520, (uint32)((sint32)(x + offset_x) >> 12));
    y = r_u32(object + 8);
    offset_y = r_u32(0x800ED514);
    w_u32(0x800ED524, (uint32)((sint32)(y + offset_y) >> 12));
    z = r_u32(object + 12);
    offset_z = r_u32(0x800ED518);
    w_u32(0x800ED528, (uint32)((sint32)(z + offset_z) >> 12));
    w_u32(0x800ED530, (uint32)((sint32)(r_u32(object + 344) + offset_x) >> 12));
    w_u32(0x800ED534, (uint32)((sint32)(r_u32(object + 348) + offset_y) >> 12));
    w_u32(0x800ED538, (uint32)((sint32)(r_u32(object + 352) + offset_z) >> 12));
    ConvertQuaternionToMatrix((const uint32 *)psx_addr(object + 428, 16), (MATRIX *)psx_addr(0x800ED550, 32));
    return sub_800878AC(0x800ED550, 0x800ED520);
}

uint32 Camera::ResolveCollisions()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80077748, "SLUS_003.73");
    MATRIX matrix;
    uint32 previous[3], input[3], transformed[3], lateral[3], opposite[3], forward[3];
    uint32 target[3], difference[3], collision[35];
    uint32 q[4], rotated[4], pitch_rotation[4], yaw_rotation[4], combined[4], old_q[4], desired_q[4];
    sint16 first_angles[4], second_angles[4];
    sint32 distance;
    uint32 result, angle, index, target_object, radius;
    uint32 paused = r_u32(0x800FF008);
    uint32 target_initialized = 0;
    previous[0] = r_u32(object + 4);
    previous[1] = r_u32(object + 8);
    previous[2] = r_u32(object + 12);
    if (!paused)
    {
        radius = r_u32(0x800FF8FC);
        for (index = 0; index < 4; ++index)
            q[index] = r_u32(object + 428 + 4 * index);
        for (index = 0; index < 4; ++index)
            w_u32(object + 460 + 4 * index, q[index]);
        input[0] = 0;
        input[1] = 0;
        input[2] = 0 - (radius << 12);
        BuildCameraQuaternionMatrix(object + 444, &matrix);
        RotateCameraVector(&matrix, input, transformed);
        distance = (sint32)r_u32(object + 540);
        // Authorized original-bug fix uses the default distance for invalid negatives
        if (distance < -1)
            distance = -1;
        if (distance == -1)
        {
            forward[0] = transformed[0];
            forward[1] = 0;
            forward[2] = transformed[2];
        }
        else if (distance > 0)
        {
            input[0] = 0;
            input[1] = 0;
            input[2] = 0 - (r_u32(object + 540) << 12);
            RotateCameraVector(&matrix, input, forward);
        }
        distance = (sint32)r_u32(object + 536);
        // Authorized original-bug fix uses the default distance for invalid negatives
        if (distance < -1)
            distance = -1;
        if (distance == -1 || distance > 0)
        {
            input[0] = 0 - ((distance == -1 ? r_u32(0x800FF8FC) : r_u32(object + 536)) << 12);
            input[1] = 0;
            input[2] = 0;
            RotateCameraVector(&matrix, input, lateral);
            opposite[0] = 0 - lateral[0];
            opposite[1] = 0 - lateral[1];
            opposite[2] = 0 - lateral[2];
        }
        target[0] = r_u32(object + 308) + transformed[0];
        target[1] = r_u32(object + 312) + transformed[1];
        target[2] = r_u32(object + 316) + transformed[2];
        target_initialized = 1;
        w_u32(0x801028C8, target[0]);
        w_u32(0x801028CC, target[1]);
        w_u32(0x801028D0, target[2]);
        SubtractVectors(difference, target, previous);
        if (r_u32(object + 536))
        {
            for (index = 0; index < 2; ++index)
            {
                uint32 *offset = index ? opposite : lateral;
                collision[0] = r_u32(object + 308);
                collision[1] = r_u32(object + 312);
                collision[2] = r_u32(object + 316);
                collision[3] = r_u32(object + 308) + offset[0];
                collision[4] = r_u32(object + 312);
                collision[5] = r_u32(object + 316) + offset[2];
                InitializeCollisionRay((uint8 *)collision);
                ((uint8 *)collision)[136] = 0;
                w_u32(0x800FF96C, 1);
                QueryCollisionRay((uint8 *)collision, 1);
                w_u32(0x800FF96C, 0);
                if (collision[26])
                {
                    angle = ReadCameraCollisionAngle(collision, (sint16)r_u16(object + 544), index);
                    BuildYawQuaternion(q, angle);
                    ApplyCameraCollisionQuaternion(object, q);
                }
            }
        }
        if (r_u32(object + 540))
        {
            collision[0] = r_u32(object + 308);
            collision[1] = r_u32(object + 312);
            collision[2] = r_u32(object + 316);
            collision[3] = r_u32(object + 308) + (forward[0] << 1);
            collision[4] = r_u32(object + 312);
            collision[5] = r_u32(object + 316) + (forward[2] << 1);
            InitializeCollisionRay((uint8 *)collision);
            ((uint8 *)collision)[136] = 0;
            w_u32(0x800FF96C, 1);
            QueryCollisionRay((uint8 *)collision, 1);
            w_u32(0x800FF96C, 0);
            if (collision[26])
            {
                angle = ReadCameraCollisionAngle(collision, (sint16)r_u16(object + 546), 0);
                BuildPitchQuaternion(q, angle);
                MultiplyQuaternions(rotated, (const uint32 *)psx_addr(object + 444, 16), q);
                for (index = 0; index < 4; ++index)
                    w_u32(object + 444 + 4 * index, rotated[index]);
            }
        }
    }
    InterpolateQuaternion((uint32 *)psx_addr(object + 460, 16), (const uint32 *)psx_addr(object + 444, 16), 1023, (uint32 *)psx_addr(object + 428, 16));
    input[0] = 0;
    input[1] = 0;
    input[2] = 0 - (r_u32(0x800FF8FC) << 12);
    BuildCameraQuaternionMatrix(object + 428, &matrix);
    RotateCameraVector(&matrix, input, transformed);
    w_u32(object + 4, r_u32(object + 308) + transformed[0]);
    w_u32(object + 8, r_u32(object + 312) + transformed[1]);
    w_u32(object + 12, r_u32(object + 316) + transformed[2]);
    SubtractVectors(difference, (const uint32 *)psx_addr(object + 4, 12), previous);
    result = r_u32(0x800FF008);
    w_u32(object + 104, difference[0]);
    w_u32(object + 108, difference[1]);
    w_u32(object + 112, difference[2]);
    if (!result)
    {
        target_object = r_u32(object + 296);
        result = r_u32(object + 336);
        if (target_object != result)
        {
            if (!target_initialized)
            {
                GameDiagnostic("TODO Camera target is uninitialized after a live pause-state change\n");
                abort();
            }
            CalculateDirectionAngles((uint16 *)first_angles, (const uint32 *)psx_addr(object + 308, 12), target);
            CalculateDirectionAngles((uint16 *)second_angles, (const uint32 *)psx_addr(r_u32(0x800FF4EC) + 4, 12), target);
            angle = sub_80077274(first_angles[1], second_angles[1]);
            desired_q[0] = desired_q[1] = desired_q[2] = 0;
            desired_q[3] = 4096;
            for (index = 0; index < 4; ++index)
                q[index] = r_u32(object + 428 + 4 * index);
            for (index = 0; index < 4; ++index)
                w_u32(object + 476 + 4 * index, q[index]);
            CalculateDirectionAngles((uint16 *)first_angles, (const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 308, 12));
            CalculateDirectionAngles((uint16 *)second_angles, (const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(r_u32(0x800FF4EC) + 4, 12));
            BuildYawQuaternion(yaw_rotation, (uint32)(sint32)(sint16)(4096 - angle));
            MultiplyQuaternions(combined, yaw_rotation, (const uint32 *)psx_addr(object + 428, 16));
            BuildPitchQuaternion(pitch_rotation, ((uint32)(uint16)second_angles[0] - (uint32)(uint16)first_angles[0]) & 0xFFF);
            MultiplyQuaternions(rotated, combined, pitch_rotation);
            for (index = 0; index < 4; ++index)
                desired_q[index] = rotated[index];
            for (index = 0; index < 4; ++index)
                old_q[index] = r_u32(object + 428 + 4 * index);
            return InterpolateQuaternion(old_q, desired_q, 1023, (uint32 *)psx_addr(object + 428, 16));
        }
    }
    return result;
}

uint32 Camera::SetTargetTransition(const void *position, uint32 frames)
{
    uint32 guest_position = GuestMemoryAddress(position);
    const void *native_position = guest_position ? 0 : position;
    uint32 camera = NativeObjectGuestAddress(this);
    uint32 object, x, y, z, delta[3], axis, divisor;
    FUNCTION_MARKER(0x80078178, "SLUS_003.73");
    if (r_u32(camera + 332))
    {
        object = r_u32(camera + 296);
        if (object && !sub_80062F48(object))
        {
            object = r_u32(camera + 296);
            x = r_u32(object + 4);
            y = r_u32(object + 8);
            z = r_u32(object + 12);
            w_u32(camera + 308, x);
            w_u32(camera + 312, y);
            w_u32(camera + 316, z);
        }
    }
    w_u32(camera + 332, frames);
    for (axis = 0; axis < 3; ++axis)
    {
        x = ReadCameraPositionWord(guest_position, native_position, axis);
        y = r_u32(camera + 308 + axis * 4);
        delta[axis] = x - y;
    }
    for (axis = 0; axis < 3; ++axis)
        w_u32(camera + 320 + axis * 4, delta[axis]);
    divisor = r_u32(camera + 332);
    for (axis = 0; axis < 3; ++axis)
    {
        x = r_u32(camera + 320 + axis * 4);
        w_u32(camera + 320 + axis * 4, DivideCameraWord(x, divisor));
    }
    return camera + 320;
}

void Camera::ClearTargetTransition()
{
    uint32 camera = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8007823C, "SLUS_003.73");
    w_u32(camera + 332, 0);
}

uint32 Camera::ClampTargetToSegment()
{
    uint32 camera = NativeObjectGuestAddress(this);
    uint32 segment[3], delta[3], axis, x, y, z, partial, dot, squared, denominator, factor, result;
    FUNCTION_MARKER(0x80078310, "SLUS_003.73");
    xport_draft_host_sub_8006C3AC_p1(segment, camera + 400, camera + 388);
    xport_draft_host_sub_8006C3AC_p1(delta, camera + 308, camera + 388);
    for (axis = 0; axis < 3; ++axis)
    {
        segment[axis] = (uint32)((sint32)segment[axis] >> 6);
        delta[axis] = (uint32)((sint32)delta[axis] >> 6);
    }
    partial = segment[0] * delta[0] + segment[1] * delta[1];
    dot = partial + segment[2] * delta[2];
    if ((sint32)dot <= 0)
    {
        x = r_u32(camera + 388); y = r_u32(camera + 392); z = r_u32(camera + 396);
        w_u32(camera + 308, x); w_u32(camera + 312, y); w_u32(camera + 316, z);
        return partial;
    }
    squared = segment[0] * segment[0] + segment[1] * segment[1] + segment[2] * segment[2];
    if ((sint32)squared < (sint32)dot)
    {
        x = r_u32(camera + 400); y = r_u32(camera + 404); z = r_u32(camera + 408);
        w_u32(camera + 308, x); w_u32(camera + 312, y); w_u32(camera + 316, z);
        return 1;
    }
    denominator = (uint32)((sint32)squared >> 12);
    if (denominator)
        dot = DivideCameraWord(dot, denominator);
    factor = (uint32)((sint32)dot >> 6);
    result = r_u32(camera + 388) + segment[0] * factor;
    w_u32(camera + 308, result);
    result = r_u32(camera + 392) + segment[1] * factor;
    w_u32(camera + 312, result);
    result = r_u32(camera + 396) + segment[2] * factor;
    w_u32(camera + 316, result);
    return result;
}

uint32 Camera::SetYawTransition(uint32 a2, uint32 a3)
{
    const uint32 a1 = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8007851C, "SLUS_003.73");
    a3 &= 65535;
    sint32 result;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    result = (a2 & 0xFFF);
    if (a3)
    {
        v4 = ((sint16)(r_u16(((uint32)((a1 + 494))))));
        v5 = result;
        w_u32(0x800FFD1C, result);
        if ((v4 >= result))
        {
            result = (result < v4);
            if ((v5 < v4))
            {
                v8 = ((sint16)(r_u16(((uint32)((a1 + 494))))));
                w_u32(0x800FFD18, a3);
                if (((v8 - v5) < 2049))
                    v7 = (v5 - v8);
                else
                    v7 = ((v5 + 4096) - v8);
                goto LABEL_4;
            }
            w_u32(0x800FFD14, 0);
            w_u32(0x800FFD18, 0);
        }
        else
        {
            w_u32(0x800FFD18, a3);
            v6 = (result - v4);
            v7 = ((result - v4) - 4096);
            if ((v6 >= 2049))
            {
            LABEL_4:
                result = (v7 / (sint32)a3);

                w_u32(0x800FFD14, result);
                return result;
            }
            result = (v6 / (sint32)a3);
            w_u32(0x800FFD14, result);
        }
    }
    else
    {
        w_u16(((uint32)((a1 + 494))), result);
        w_u32(0x800FFD18, 0);
    }
    return result;
}

uint32 SetCameraOrbitDistanceTransition(uint32 target, uint32 frames)
{
    uint32 value = target & 65535;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x800785D8, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - r_u32(0x800FF908)) / (sint32)count);
        w_u32(0x800FF90C, count);
        w_u32(0x800FFD0C, result);
    }
    else
    {
        result = value;
        w_u32(0x800FF908, value);
        w_u32(0x800FF90C, 0);
    }
    return result;
}

uint32 SetCameraOrbitHeightTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x80078618, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - r_u32(0x800FF910)) / (sint32)count);
        w_u32(0x800FF914, count);
        w_u32(0x800FFD10, result);
    }
    else
    {
        result = value;
        w_u32(0x800FF910, value);
        w_u32(0x800FF914, 0);
    }
    return result;
}

uint32 SetCameraTargetLateralOffsetTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x8007865C, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - r_u32(0x800FF918)) / (sint32)count);
        w_u32(0x800FF91C, count);
        w_u32(0x800FFD20, result);
    }
    else
    {
        result = value;
        w_u32(0x800FF918, value);
        w_u32(0x800FF91C, 0);
    }
    return result;
}

uint32 SetCameraTargetHeightOffsetTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x800786A0, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - r_u32(0x800FF920)) / (sint32)count);
        w_u32(0x800FF924, count);
        w_u32(0x800FFD24, result);
    }
    else
    {
        result = value;
        w_u32(0x800FF920, value);
        w_u32(0x800FF924, 0);
    }
    return result;
}

uint32 SetCameraTargetForwardOffsetTransition(uint32 target, uint32 frames)
{
    uint32 value = (uint32)(sint32)(sint16)target;
    uint32 count = frames & 65535, result;
    FUNCTION_MARKER(0x800786E4, "SLUS_003.73");
    if (count)
    {
        result = (uint32)((sint32)(value - r_u32(0x800FF928)) / (sint32)count);
        w_u32(0x800FF92C, count);
        w_u32(0x800FFD28, result);
    }
    else
    {
        result = value;
        w_u32(0x800FF928, value);
        w_u32(0x800FF92C, 0);
    }
    return result;
}

uint32 Camera::SetProjDistanceTransition(uint32 target, uint32 frames)
{
    uint32 camera = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80078728, "SLUS_003.73");
    w_u16(camera + 376, frames);
    frames &= 65535;
    result = target & 65535;
    if (frames)
    {
        result -= r_u32(camera + 372);
        result = DivideCameraWord(result, r_u16(camera + 376));
        w_u32(camera + 380, result);
    }
    else
        w_u32(camera + 372, result);
    return result;
}

uint32 Camera::GetProjDistance()
{
    FUNCTION_MARKER(0x80078760, "SLUS_003.73");
    return r_u32(NativeObjectGuestAddress(this) + 372);
}

uint32 Camera::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, x, y, z, count, target, value, yaw, timer, table, first, second, radius;
    uint32 saved[7], matrices[5][4], matrix[4], shake_word, shake_tail, magnitude, rate;
    uint32 mode_button, record_button, toggle_button, modifier, basic_button, held;
    sint32 angle, vertical, oscillation_count;
    FUNCTION_MARKER(0x8007876C, "SLUS_003.73");
    result = r_u32(0x800FF008);
    if (result)
        return result;
    x = r_u32(object + 4);
    y = r_u32(object + 8);
    z = r_u32(object + 12);
    w_u32(0x801028C8, x);
    w_u32(0x801028CC, y);
    w_u32(0x801028D0, z);
    value = r_u32(object + 576);
    count = r_u32(object + 332);
    w_u32(object + 576, value + 1);
    if (count)
    {
        sub_8006C0B8(object + 308, object + 320);
        count = r_u32(object + 332);
        w_u32(object + 332, count - 1);
    }
    else
    {
        target = r_u32(object + 296);
        if (target && !sub_80062F48(target))
        {
            target = r_u32(object + 296);
            x = r_u32(target + 4);
            y = r_u32(target + 8);
            z = r_u32(target + 12);
            w_u32(object + 308, x);
            w_u32(object + 312, y);
            w_u32(object + 316, z);
        }
    }
    count = r_u32(object + 368);
    if (count)
    {
        sub_8006C0B8(object + 344, object + 356);
        count = r_u32(object + 368);
        w_u32(object + 368, count - 1);
    }
    else
    {
        target = r_u32(object + 336);
        x = r_u32(target + 4);
        y = r_u32(target + 8);
        z = r_u32(target + 12);
        w_u32(object + 344, x);
        w_u32(object + 348, y);
        w_u32(object + 352, z);
    }
    if (r_u16(object + 376))
    {
        value = r_u32(object + 372);
        x = r_u32(object + 380);
        count = r_u16(object + 376);
        w_u32(object + 372, value + x);
        w_u16(object + 376, count - 1);
    }
    if (r_u32(object + 384))
        ClampTargetToSegment();
    if (r_u8(0x800EC1E8))
    {
        if (r_u32(0x800FF944))
            w_u32(0x800FF940, 1);
        w_u32(0x800FF944, 0);
    }
    else
        w_u32(0x800FF944, 1);
    mode_button = r_u8(0x800EC2D5);
    if (mode_button)
    {
        record_button = r_u8(0x800EC2E5);
        if (record_button)
        {
            toggle_button = r_u8(0x800EC2D4);
            if (toggle_button)
            {
                modifier = r_u8(0x800EC2E4);
                basic_button = r_u8(0x800EC0F8);
                held = r_u8(0x800EC1E8);
                if (held & modifier & basic_button)
                {
                    value = r_u32(0x800FF934);
                    w_u8(0x800EC2D5, 0);
                    w_u8(0x800EC2E5, 0);
                    value ^= 1;
                    w_u32(0x800FF934, value);
                    w_u32(0x800FF300, value);
                    if (value)
                    {
                        saved[0] = r_u32(0x800FF918);
                        saved[1] = r_u32(0x800FF920);
                        saved[2] = r_u32(0x800FF928);
                        saved[3] = r_u32(0x800FF908);
                        saved[4] = r_u32(0x800FF910);
                        saved[5] = (uint32)(sint32)(sint16)r_u16(object + 494);
                        saved[6] = r_u32(object + 372);
                        w_u32(0x800FFCA0, saved[0]);
                        w_u32(0x800FFCA4, saved[1]);
                        w_u32(0x800FFCA8, saved[2]);
                        w_u32(0x800FFCAC, saved[3]);
                        w_u32(0x800FFCB0, saved[4]);
                        w_u32(0x800FFCB4, saved[5]);
                        w_u32(0x800FF93C, saved[6]);
                    }
                    else
                        goto AdvanceTransitions;
                }
            }
        }
    }
    if (r_u32(0x800FF934))
    {
        if (r_u8(0x800EC354))
        {
            saved[0] = r_u32(0x800FFCA0);
            saved[1] = r_u32(0x800FFCA4);
            saved[2] = r_u32(0x800FFCA8);
            saved[3] = r_u32(0x800FFCAC);
            saved[4] = r_u32(0x800FFCB0);
            saved[5] = r_u16(0x800FFCB4);
            saved[6] = r_u32(0x800FF93C);
            w_u32(0x800FF918, saved[0]);
            w_u32(0x800FF920, saved[1]);
            w_u32(0x800FF928, saved[2]);
            w_u32(0x800FF908, saved[3]);
            w_u32(0x800FF910, saved[4]);
            w_u16(object + 494, saved[5]);
            w_u32(object + 372, saved[6]);
        }
        if (r_u8(0x800EC108))
            w_u32(object + 372, r_u32(object + 372) - 1);
        if (r_u8(0x800EC118))
            w_u32(object + 372, r_u32(object + 372) + 1);
        if (r_u8(0x800EC0F8))
            w_u32(object + 372, 256);
        AdjustCameraDebugValue(0x800EC2F4, 0x800FF918, 1);
        AdjustCameraDebugValue(0x800EC304, 0x800FF918, 0);
        AdjustCameraDebugValue(0x800EC314, 0x800FF928, 0);
        AdjustCameraDebugValue(0x800EC324, 0x800FF928, 1);
        AdjustCameraDebugValue(0x800EC2B4, 0x800FF920, 1);
        AdjustCameraDebugValue(0x800EC2C4, 0x800FF920, 0);
        AdjustCameraDebugValue(0x800EC2D4, 0x800FF910, 1);
        AdjustCameraDebugValue(0x800EC2E4, 0x800FF910, 0);
        AdjustCameraDebugValue(0x800EC274, 0x800FF908, 1);
        AdjustCameraDebugValue(0x800EC2A4, 0x800FF908, 0);
        if (r_u8(0x800EC284))
        {
            value = r_u8(0x800EC364);
            yaw = (uint32)(sint32)(sint16)r_u16(object + 494);
            w_u16(object + 494, (yaw + (value ? 32 : 8)) & 4095);
        }
        if (r_u8(0x800EC294))
        {
            value = r_u8(0x800EC364);
            yaw = (uint32)(sint32)(sint16)r_u16(object + 494);
            w_u16(object + 494, (yaw - (value ? 32 : 8)) & 4095);
        }
    }
AdvanceTransitions:
    AdvanceCameraGlobalTransition(0x800FF90C, 0x800FF908, 0x800FFD0C);
    AdvanceCameraGlobalTransition(0x800FF914, 0x800FF910, 0x800FFD10);
    timer = r_u32(0x800FFD18);
    if (timer)
    {
        --timer;
        w_u32(0x800FFD18, timer);
        if (timer)
        {
            yaw = r_u16(object + 494);
            value = r_u16(0x800FFD14);
            yaw = (yaw + value) & 4095;
        }
        else
            yaw = r_u16(0x800FFD1C);
        w_u16(object + 494, yaw);
    }
    AdvanceCameraGlobalTransition(0x800FF91C, 0x800FF918, 0x800FFD20);
    AdvanceCameraGlobalTransition(0x800FF924, 0x800FF920, 0x800FFD24);
    AdvanceCameraGlobalTransition(0x800FF92C, 0x800FF928, 0x800FFD28);
    value = r_u32(0x800FF908);
    first = value * value;
    value = r_u32(0x800FF910);
    second = value * value;
    radius = SquareRoot0(first + second);
    x = r_u32(0x800FF910);
    y = r_u32(0x800FF908);
    w_u32(0x800FF8FC, radius);
    angle = ratan2(x, y);
    value = r_u32(0x800FF920);
    w_u32(0x800ED514, value << 12);
    yaw = r_u16(object + 494);
    x = r_u32(0x800FF918);
    table = 0x800F863C + (yaw & 4095) * 4;
    first = x * (uint32)(sint32)(sint16)r_u16(table + 2);
    z = r_u32(0x800FF928);
    second = z * (uint32)(sint32)(sint16)r_u16(table);
    w_u32(0x800ED510, 0 - first - second);
    yaw = r_u16(object + 494);
    table = 0x800F863C + (yaw & 4095) * 4;
    first = x * (uint32)(sint32)(sint16)r_u16(table);
    second = z * (uint32)(sint32)(sint16)r_u16(table + 2);
    w_u32(0x800FF8F8, 0 - (uint32)angle);
    w_u32(0x800ED518, first - second);
    sub_80066B8C(object + 16, object + 4, object + 344);
    target = r_u32(object + 296);
    if (r_u16(target + 570) && (sint16)r_u16(object + 414) && !r_u32(object + 332))
    {
        vertical = (sint32)r_u32(target + 108);
        value = r_u32(0x800FF94C);
        oscillation_count = (sint32)(vertical < 0 ? value + 1 : value - 1);
        w_u32(0x800FF94C, (uint32)oscillation_count);
        target = r_u32(object + 296);
        value = r_u32(target + 108);
        magnitude = (uint32)(sint32)(sint16)r_u16(object + 414);
        first = (uint32)((sint32)(0 - value) >> 12) * magnitude;
        value = r_u32(0x800FF948);
        count = r_u32(0x800FF94C);
        value += first;
        w_u32(0x800FF948, value);
        if ((sint32)count > 0)
            w_u32(object + 312, r_u32(object + 312) + value);
    }
    else
    {
        w_u32(0x800FF948, 0);
        w_u32(0x800FF94C, 0);
    }
    magnitude = (uint32)(sint32)(sint16)r_u16(object + 412);
    if (magnitude)
    {
        target = r_u32(object + 296);
        if ((sint32)r_u32(target + 108) < 0)
            w_u32(0x800FF8F8, r_u32(0x800FF8F8) - magnitude);
        target = r_u32(object + 296);
        if ((sint32)r_u32(target + 108) > 0)
        {
            magnitude = r_u16(object + 412);
            value = r_u32(0x800FF8F8);
            magnitude = (uint32)((sint32)(magnitude << 16) >> 17);
            w_u32(0x800FF8F8, value - magnitude);
        }
        w_u32(0x800FF8F8, r_u32(0x800FF8F8) & 4095);
    }
    switch (r_u32(object + 564))
    {
        case 2:
        case 3:
        case 7:
        case 15:
            w_u32(0x800FF930, 0);
            BuildView();
            break;
        case 18:
        case 19:
            w_u32(0x800FF930, 1);
            UpdatePlayerRelativeTracking();
            break;
        case 20:
            w_u32(0x800FF930, 1);
            UpdateFixedOriginTracking();
            break;
    }
    ResolveCollisions();
    sub_80066B8C(object + 16, object + 4, object + 344);
    value = r_u32(object + 548);
    if (value || (sint16)r_u16(object + 552))
    {
        BuildYawQuaternion(matrices[0], (uint32)(sint32)ReadCameraOscillation(object, 550, 560));
        MultiplyQuaternions(matrices[1], matrices[0], (const uint32 *)psx_addr(object + 428, 16));
        BuildPitchQuaternion(matrices[2], (uint32)(sint32)ReadCameraOscillation(object, 548, 558));
        MultiplyQuaternions(matrices[3], matrices[1], matrices[2]);
        BuildRollQuaternion(matrices[4], (uint32)(sint32)ReadCameraOscillation(object, 552, 562));
        MultiplyQuaternions(matrix, matrices[3], matrices[4]);
        w_u32(object + 428, matrix[0]);
        w_u32(object + 432, matrix[1]);
        w_u32(object + 436, matrix[2]);
        w_u32(object + 440, matrix[3]);
        // Object storage is aligned for the original paired LWL/LWR reads
        shake_word = r_u32(object + 548);
        shake_word = r_u32(object + 548);
        shake_tail = r_u16(object + 552);
        vertical = (sint16)r_u16(object + 548);
        magnitude = r_u16(object + 548);
        rate = r_u8(object + 554);
        w_u16(object + 548, vertical < 0 ? magnitude + rate : magnitude - rate);
        vertical = (sint16)r_u16(object + 550);
        magnitude = r_u16(object + 550);
        rate = r_u8(object + 555);
        w_u16(object + 550, vertical < 0 ? magnitude + rate : magnitude - rate);
        vertical = (sint16)r_u16(object + 552);
        magnitude = r_u16(object + 552);
        rate = r_u8(object + 556);
        w_u16(object + 552, vertical < 0 ? magnitude + rate : magnitude - rate);
        if ((shake_word ^ r_u16(object + 548)) & 0x8000)
            w_u16(object + 548, 0);
        if (((shake_word >> 16) ^ r_u16(object + 550)) & 0x8000)
            w_u16(object + 550, 0);
        if ((shake_tail ^ r_u16(object + 552)) & 0x8000)
            w_u16(object + 552, 0);
    }
    PrepareView();
    result = r_u32(0x800FF930);
    if (result)
    {
        x = r_u32(object + 476);
        y = r_u32(object + 480);
        z = r_u32(object + 484);
        value = r_u32(object + 488);
        w_u32(object + 428, x);
        w_u32(object + 432, y);
        w_u32(object + 436, z);
        w_u32(object + 440, value);
    }
    return result;
}

uint32 Camera::BuildView()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800793C8, "SLUS_003.73");
    uint32 offset[3] = {0, 0, r_u32(0x800FF8FC) << 12};
    uint32 up[3] = {0, 0x1F4000, 0}, translated[3], forward[3], normal_up[3];
    uint32 rotation = r_u32(object + 492);
    rotation = r_u32(object + 492);
    uint32 roll = r_u16(object + 496);
    uint32 steps[4], pass, step, i, x, y, z, sine, cosine, table, sum;
    sint32 divisor;
    sint16 matrix[9];
    steps[0] = r_u32(0x800FF8F8) & 0xFFF;
    steps[1] = (rotation >> 16) & 0xFFF;
    steps[2] = rotation & 0xFFF;
    steps[3] = roll & 0xFFF;
    for (pass = 0; pass < 2; ++pass)
    {
        uint32 *vector = pass ? up : offset;
        if (pass)
            steps[0] = r_u32(0x800FF8F8) & 4095;
        for (step = 0; step < 4; ++step)
        {
            table = 0x800F863C + steps[step] * 4;
            sine = (uint32)(sint32)(sint16)r_u16(table);
            cosine = (uint32)(sint32)(sint16)r_u16(table + 2);
            x = (uint32)((sint32)vector[0] >> 12);
            y = (uint32)((sint32)vector[1] >> 12);
            z = (uint32)((sint32)vector[2] >> 12);
            if (step == 1)
            {
                vector[0] = x * cosine + z * sine;
                vector[2] = z * cosine - x * sine;
            }
            else if (step == 3)
            {
                vector[0] = x * cosine - y * sine;
                vector[1] = y * cosine + x * sine;
            }
            else
            {
                vector[1] = y * cosine - z * sine;
                vector[2] = z * cosine + y * sine;
            }
            if (pass)
            {
                w_u32(object + 416, vector[0]);
                w_u32(object + 420, vector[1]);
                w_u32(object + 424, vector[2]);
            }
        }
    }
    xport_draft_host_sub_8006C34C_p12(translated, offset, object + 308);
    for (i = 0; i < 3; ++i)
        w_u32(object + 512 + i * 4, translated[i]);
    x = r_u32(object + 308);
    y = r_u32(object + 312);
    z = r_u32(object + 316);
    w_u32(object + 524, x);
    w_u32(object + 528, y);
    w_u32(object + 532, z);
    sum = 0;
    for (i = 0; i < 3; ++i)
    {
        forward[i] = (uint32)((sint32)r_u32(object + 524 + i * 4) >> 12) - (uint32)((sint32)r_u32(object + 512 + i * 4) >> 12);
        sum += forward[i] * forward[i];
    }
    divisor = (sint32)SquareRoot0(sum);
    if (!divisor)
        divisor = 1;
    for (i = 0; i < 3; ++i)
    {
        uint32 value = forward[i] << 12;
        forward[i] = value == 0x80000000 && divisor == -1 ? value : (uint32)((sint32)value / divisor);
    }
    sum = 0;
    for (i = 0; i < 3; ++i)
    {
        normal_up[i] = (uint32)((sint32)r_u32(object + 416 + i * 4) >> 12);
        sum += normal_up[i] * normal_up[i];
    }
    divisor = (sint32)SquareRoot0(sum);
    if (!divisor)
        divisor = 1;
    for (i = 0; i < 3; ++i)
    {
        uint32 value = normal_up[i] << 12;
        normal_up[i] = value == 0x80000000 && divisor == -1 ? value : (uint32)((sint32)value / divisor);
    }
    xport_draft_gte_control_write(0, normal_up[0]);
    xport_draft_gte_control_write(2, normal_up[1]);
    xport_draft_gte_control_write(4, normal_up[2]);
    xport_draft_gte_data_write(11, forward[2]);
    xport_draft_gte_data_write(9, forward[0]);
    xport_draft_gte_data_write(10, forward[1]);
    xport_draft_gte_execute(0x170000C);
    x = xport_draft_gte_data_read(25);
    y = xport_draft_gte_data_read(26);
    z = xport_draft_gte_data_read(27);
    matrix[0] = (sint16)((sint32)x >> 12);
    matrix[3] = (sint16)((sint32)y >> 12);
    matrix[6] = (sint16)((sint32)z >> 12);
    matrix[1] = (sint16)normal_up[0];
    matrix[4] = (sint16)normal_up[1];
    matrix[7] = (sint16)normal_up[2];
    matrix[2] = (sint16)forward[0];
    matrix[5] = (sint16)forward[1];
    matrix[8] = (sint16)forward[2];
    return ConvertMatrixToQuaternion(matrix, (uint32 *)psx_addr(object + 444, 16));
}

uint32 Camera::UpdatePlayerRelativeTracking()
{
    const uint32 camera = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80079DC8, "SLUS_003.73");
    uint32 target, original_target, origin, history, initialized, distance = 0, target_distance;
    uint32 angle, table_index, product, magnitude, scale, previous, value;
    int distance_initialized = 0;
    sint32 delta;
    sint16 direction[4], target_angles[4], turn[4], position_angles[4], final_target_angles[4];
    original_target = r_u32(0x800FF4EC);
    if (!original_target)
    {
        w_u32(camera + 336, r_u32(camera + 296));
        return BuildView();
    }
    target = r_u32(0x800FF4EC);
    w_u32(camera + 336, original_target);
    w_u32(0x800ED510, 0);
    w_u32(0x800ED514, (uint32)-368640);
    w_u32(0x800ED518, 0);
    if (target)
    {
        distance = sub_8006696C(camera + 308, target + 4);
        distance_initialized = 1;
    }
    origin = camera + 4;
    xport_draft_host_sub_80066B8C_p1(direction, camera + 308, origin);
    xport_draft_host_sub_80066B8C_p1(target_angles, r_u32(0x800FF4EC) + 4, origin);
    history = 0x800FFCB8;
    initialized = 0x800FF954;
    if (!r_u32(initialized))
    {
        w_u32(initialized, 1);
        StoreTrackingAngles(history, target_angles);
    }
    delta = (sint16)sub_80077274((sint16)r_u16(history + 2), target_angles[1]);
    target = r_u32(0x800FF4EC);
    angle = r_u16(history + 2);
    target_angles[1] = (sint16)((angle + (uint32)(delta >> 1) + 4096) & 4095);
    target_distance = sub_8006696C(target + 4, origin);
    target = r_u32(0x800FF4EC);
    if (target)
    {
        xport_draft_host_sub_80066B8C_p1(turn, camera + 308, target + 4);
        delta = (sint16)sub_80077274((sint16)r_u16(camera + 494), turn[1]);
        if (!distance_initialized)
        {
            // TODO The original reads an uninitialized distance if its live target changes here
            GameDiagnostic("Uninitialized tracking-camera distance at %08X\n", 0x80079DC8);
            abort();
        }
        TurnTrackingCamera(camera, delta, (sint32)distance, 1);
    }
    xport_draft_host_sub_80066B8C_p1(position_angles, camera + 308, origin);
    xport_draft_host_sub_80066B8C_p1(final_target_angles, r_u32(0x800FF4EC) + 4, origin);
    table_index = ((uint32)(uint16)position_angles[1] - (uint32)(uint16)final_target_angles[1]) & 4095;
    angle = r_u16(camera + 494);
    table_index += ((uint32)(uint16)target_angles[1] - angle) & 4095;
    table_index &= 4095;
    value = (uint32)(sint32)(sint16)r_u16(0x800F863C + table_index * 4);
    product = value * target_distance;
    magnitude = (uint32)((sint32)product >> 12);
    value = (uint32)((sint32)product >> 31);
    magnitude = (magnitude ^ value) - value;
    scale = (uint32)((sint32)(magnitude * 6553) >> 12);
    if (!r_u32(0x800FF958))
    {
        w_u32(0x800FF958, 1);
        w_u32(0x800FFCC0, scale);
    }
    previous = r_u32(0x800FFCC0);
    value = (uint32)((sint32)(scale + previous) >> 1);
    w_u32(0x800FF8FC, value);
    if ((sint32)value < 1700)
        w_u32(0x800FF8FC, 1700);
    value = r_u32(0x800FF8FC);
    if ((sint32)value >= 3001)
        w_u32(0x800FF8FC, 3000);
    w_u32(0x800FF8F8, 130);
    w_u32(0x800FFCC0, scale);
    StoreTrackingAngles(history, target_angles);
    return BuildView();
}

uint32 Camera::UpdateFixedOriginTracking()
{
    const uint32 camera = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8007A1A8, "SLUS_003.73");
    uint32 target, original_target, origin, history, initialized, distance = 0, target_distance;
    uint32 angle, table_index, product, magnitude, scale, previous, value;
    int distance_initialized = 0;
    sint32 delta;
    sint16 direction[4], target_angles[4], turn[4], position_angles[4], final_target_angles[4];
    original_target = r_u32(0x800FF4EC);
    if (!original_target)
    {
        w_u32(camera + 336, r_u32(camera + 296));
        return BuildView();
    }
    target = r_u32(0x800FF4EC);
    w_u32(camera + 336, original_target);
    w_u32(0x800ED510, 0);
    w_u32(0x800ED514, (uint32)-368640);
    w_u32(0x800ED518, 0);
    if (target)
    {
        distance = sub_8006696C(camera + 308, target + 4);
        distance_initialized = 1;
    }
    origin = 0x801028C8;
    xport_draft_host_sub_80066B8C_p1(direction, camera + 308, origin);
    xport_draft_host_sub_80066B8C_p1(target_angles, r_u32(0x800FF4EC) + 4, origin);
    history = 0x800FFCC8;
    initialized = 0x800FF95C;
    if (!r_u32(initialized))
    {
        w_u32(initialized, 1);
        StoreTrackingAngles(history, target_angles);
    }
    delta = (sint16)sub_80077274((sint16)r_u16(history + 2), target_angles[1]);
    target = r_u32(0x800FF4EC);
    angle = r_u16(history + 2);
    target_angles[1] = (sint16)((angle + (uint32)(delta >> 1) + 4096) & 4095);
    target_distance = sub_8006696C(target + 4, origin);
    target = r_u32(0x800FF4EC);
    if (target)
    {
        xport_draft_host_sub_80066B8C_p1(turn, camera + 308, target + 4);
        delta = (sint16)sub_80077274((sint16)r_u16(camera + 494), turn[1]);
        if (!distance_initialized)
        {
            // TODO The original reads an uninitialized distance if its live target changes here
            GameDiagnostic("Uninitialized tracking-camera distance at %08X\n", 0x8007A1A8);
            abort();
        }
        TurnTrackingCamera(camera, delta, (sint32)distance, 0);
    }
    xport_draft_host_sub_80066B8C_p1(position_angles, camera + 308, origin);
    xport_draft_host_sub_80066B8C_p1(final_target_angles, r_u32(0x800FF4EC) + 4, origin);
    if (!r_u32(0x800FF960))
    {
        previous = r_u32(0x800FF8FC);
        w_u32(0x800FF960, 1);
        w_u32(0x800FFCD0, previous);
    }
    previous = r_u32(0x800FFCD0);
    w_u32(0x800FF8F8, 100);
    StoreTrackingAngles(history, target_angles);
    value = (uint32)((sint32)(previous + 1700) >> 1);
    w_u32(0x800FF8FC, value);
    w_u32(0x800FFCD0, value);
    return BuildView();
}

uint32 ResetCameraPresetData()
{
    FUNCTION_MARKER(0x8007A5FC, "SLUS_003.73");
    w_u32(0x801029B8, 0x00000000);
    w_u32(0x801029BC, 0xFFE70000);
    w_u32(0x801029C0, 0x00000000);
    w_u32(0x801029A8, 0x00000000);
    w_u32(0x801029AC, 0xFFF9C000);
    w_u32(0x801029B0, 0x00000000);
    w_u32(0x80102998, 0x00000000);
    w_u32(0x8010299C, 0xFFF60000);
    w_u32(0x801029A0, 0xFFC4A000);
    w_u32(0x80102988, 0x00000000);
    w_u32(0x8010298C, 0xFFF9C000);
    w_u32(0x80102990, 0x00000000);
    w_u32(0x80102978, 0x00000000);
    w_u32(0x8010297C, 0xFFF6A000);
    w_u32(0x80102980, 0x0028A000);
    w_u32(0x80102968, 0x00000000);
    w_u32(0x8010296C, 0xFFF9C000);
    w_u32(0x80102970, 0x00000000);
    w_u32(0x80102958, 0x00000000);
    w_u32(0x8010295C, 0xFFBB4000);
    w_u32(0x80102960, 0x00514000);
    w_u32(0x80102948, 0x00000000);
    w_u32(0x8010294C, 0xFFF9C000);
    w_u32(0x80102950, 0xFFCE0000);
    w_u32(0x80102938, 0x00000000);
    w_u32(0x8010293C, 0xFF736000);
    w_u32(0x80102940, 0x00190000);
    w_u32(0x80102928, 0x00000000);
    w_u32(0x8010292C, 0xFFF9C000);
    w_u32(0x80102930, 0xFFED4000);
    w_u32(0x80102918, 0x00000000);
    w_u32(0x8010291C, 0xFFF1F000);
    w_u32(0x80102920, 0x00320000);
    w_u32(0x80102908, 0x00000000);
    w_u32(0x8010290C, 0xFFE7A000);
    w_u32(0x80102910, 0x00320000);
    w_u32(0x801028F8, 0x00000000);
    w_u32(0x801028FC, 0x00064000);
    w_u32(0x80102900, 0xF92A0000);
    w_u32(0x801028E8, 0x00000000);
    w_u32(0x801028EC, 0xFFF9C000);
    w_u32(0x801028F0, 0x00000000);
    w_u16(0x800FFCFC, 0x0032);
    w_u16(0x800FFCF4, 0x001E);
    w_u8(0x800FFCE0, 0x03);
    w_u8(0x800FFCE2, 0x03);
    w_u16(0x800FFCD8, 0x0258);
    w_u16(0x800FFCDC, 0x0258);
    w_u32(0x801028D8, 0x00000000);
    w_u16(0x800FFCF8, 0x0019);
    w_u16(0x800FFCFA, 0x0000);
    w_u16(0x800FFCF0, 0x0000);
    w_u16(0x800FFCF2, 0x0000);
    w_u16(0x800FFCE8, 0x0000);
    w_u16(0x800FFCEA, 0x0000);
    w_u16(0x800FFCEC, 0x0019);
    w_u8(0x800FFCE1, 0x00);
    w_u16(0x800FFCDA, 0x0000);
    w_u32(0x801028DC, 0xFFF92000);
    w_u32(0x801028E0, 0x00000000);
    w_u32(0x800ED510, 0x00000000);
    w_u32(0x800ED514, 0xFFFA6000);
    w_u32(0x800ED518, 0x00000000);
    return 0x800ED510;
}

extern "C" void sub_8007823C(uint32 camera)
{
    FUNCTION_MARKER(0x8007823C, "SLUS_003.73");
    Camera::Attach(camera).ClearTargetTransition();
}

extern "C" void sub_80077490(uint32 camera, uint32 value)
{
    FUNCTION_MARKER(0x80077490, "SLUS_003.73");
    Camera::Attach(camera).SetColDistances(value);
}

extern "C" void sub_800774AC(uint32 camera, uint32 value)
{
    FUNCTION_MARKER(0x800774AC, "SLUS_003.73");
    Camera::Attach(camera).SetColYawStrength(value);
}

extern "C" uint32 sub_80077624(uint32 camera, uint32 mode)
{
    FUNCTION_MARKER(0x80077624, "SLUS_003.73");
    return Camera::Attach(camera).SetModePreset(mode);
}

extern "C" uint32 sub_80078178(uint32 camera, uint32 position, uint32 frames)
{
    FUNCTION_MARKER(0x80078178, "SLUS_003.73");
    return Camera::Attach(camera).SetTargetTransition(psx_addr(position, 12), frames);
}

extern "C" uint32 xport_draft_host_sub_80078178_p2(uint32 camera, const void *position, uint32 frames)
{
    FUNCTION_MARKER(0x80078178, "SLUS_003.73");
    return Camera::Attach(camera).SetTargetTransition(position, frames);
}

extern "C" uint32 sub_80078310(uint32 camera)
{
    FUNCTION_MARKER(0x80078310, "SLUS_003.73");
    return Camera::Attach(camera).ClampTargetToSegment();
}

extern "C" uint32 sub_80078728(uint32 camera, uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x80078728, "SLUS_003.73");
    return Camera::Attach(camera).SetProjDistanceTransition(target, frames);
}

extern "C" void sub_8007749C(uint32 object, uint32 value)
{
    FUNCTION_MARKER(0x8007749C, "SLUS_003.73");
    Camera::Attach(object).SetLateralColDistance(value);
}

extern "C" void sub_800774A4(uint32 object, uint32 value)
{
    FUNCTION_MARKER(0x800774A4, "SLUS_003.73");
    Camera::Attach(object).SetRearColDistance(value);
}

extern "C" void sub_800774B4(uint32 object, uint32 value)
{
    FUNCTION_MARKER(0x800774B4, "SLUS_003.73");
    Camera::Attach(object).SetColPitchStrength(value);
}

extern "C" uint32 sub_800772B8(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x800772B8, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(Camera));
    new (storage) Camera(VisualObject::Attach(target));
    return object;
}

extern "C" uint32 sub_8007741C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8007741C, "SLUS_003.73");
    return Camera::Attach(object).Camera::Destroy(reason);
}

extern "C" uint32 sub_80077680(uint32 object)
{
    FUNCTION_MARKER(0x80077680, "SLUS_003.73");
    return Camera::Attach(object).PrepareView();
}

extern "C" uint32 sub_80079DC8(uint32 camera)
{
    FUNCTION_MARKER(0x80079DC8, "SLUS_003.73");
    return Camera::Attach(camera).UpdatePlayerRelativeTracking();
}

extern "C" uint32 sub_8007A1A8(uint32 camera)
{
    FUNCTION_MARKER(0x8007A1A8, "SLUS_003.73");
    return Camera::Attach(camera).UpdateFixedOriginTracking();
}

extern "C" uint32 sub_8007A5FC(void)
{
    FUNCTION_MARKER(0x8007A5FC, "SLUS_003.73");
    return ResetCameraPresetData();
}

extern "C" uint32 sub_80076D44(void)
{
    FUNCTION_MARKER(0x80076D44, "SLUS_003.73");
    return ClearCameraPresetFlags();
}

extern "C" uint32 sub_80078760(uint32 object)
{
    FUNCTION_MARKER(0x80078760, "SLUS_003.73");
    return Camera::Attach(object).GetProjDistance();
}

extern "C" uint32 sub_80077748(uint32 object)
{
    FUNCTION_MARKER(0x80077748, "SLUS_003.73");
    return Camera::Attach(object).ResolveCollisions();
}

extern "C" uint32 sub_8007876C(uint32 object)
{
    FUNCTION_MARKER(0x8007876C, "SLUS_003.73");
    return Camera::Attach(object).Camera::Update();
}

extern "C" uint32 sub_800793C8(uint32 object)
{
    FUNCTION_MARKER(0x800793C8, "SLUS_003.73");
    return Camera::Attach(object).BuildView();
}

extern "C" uint32 sub_8007851C(uint32 object, uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x8007851C, "SLUS_003.73");
    return Camera::Attach(object).SetYawTransition(target, frames);
}

extern "C" uint32 sub_800785D8(uint32, uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x800785D8, "SLUS_003.73");
    return SetCameraOrbitDistanceTransition(target, frames);
}

extern "C" uint32 sub_80078618(uint32, uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x80078618, "SLUS_003.73");
    return SetCameraOrbitHeightTransition(target, frames);
}

extern "C" uint32 sub_8007865C(uint32, uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x8007865C, "SLUS_003.73");
    return SetCameraTargetLateralOffsetTransition(target, frames);
}

extern "C" uint32 sub_800786A0(uint32, uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x800786A0, "SLUS_003.73");
    return SetCameraTargetHeightOffsetTransition(target, frames);
}

extern "C" uint32 sub_800786E4(uint32, uint32 target, uint32 frames)
{
    FUNCTION_MARKER(0x800786E4, "SLUS_003.73");
    return SetCameraTargetForwardOffsetTransition(target, frames);
}

extern "C" uint32 apocalypse_explosion_camera_native(uint32 camera, uint32 kind)
{
    return Camera::Attach(camera).ApplyExplosionShake(kind);
}

extern "C" uint32 sub_800774EC(uint32 camera, uint32, uint32 kind)
{
    FUNCTION_MARKER(0x800774EC, "SLUS_003.73");
    return Camera::Attach(camera).ApplyExplosionShake(kind);
}
