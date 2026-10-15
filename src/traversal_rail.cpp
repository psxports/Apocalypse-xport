extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "traversal_rail.h"
#include "effect_segments.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include <new>
#include <string.h>

// Native input bridge preserves sequential original record-word loads

// Native extraction of the repeated signed brightness block

// Native extractions of the original6CAC8/6CB28 vector blocks

#include "traversal_rail.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"

static uint32 ForwardDot(uint32 record, const uint32 delta[3]);
static void StoreOutput(uint32 guest, uint32 *native, const uint32 values[3]);
static uint32 ReadOutput(uint32 guest, const uint32 *native, uint32 index);

static uint32 ReadPathRecordWord(const uint32 *data, uint32 axis)
{
    uint32 address = GuestMemoryAddress(data), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)data + axis * 4, 4);
    return value;
}

static uint32 GetTraversalRailBrightness(uint32 distance)
{
    uint32 result;
    if ((sint32)distance < 1025)
        result = 255;
    else if ((sint32)distance < 4096)
        result = 255 - (255 * (distance - 1024)) / 3072;
    else
        result = 0;
    return (sint32)result < 32 ? 32 : result;
}

static void AddFlexibleTraversalRailNeighbors(uint16 output[3], uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x8006CAC8, "SLUS_003.73");
    uint32 axis, first, second;
    for (axis = 0; axis < 3; ++axis)
    {
        first = r_u16(left + axis * 2);
        second = r_u16(right + axis * 2);
        output[axis] = (uint16)(first + second);
    }
}

static void SubtractFlexibleTraversalRailVector(uint16 output[3], const uint16 *native_left, uint32 guest_left, uint32 guest_right)
{
    uint32 axis, first, second;
    for (axis = 0; axis < 3; ++axis)
    {
        first = native_left ? native_left[axis] : r_u16(guest_left + axis * 2);
        second = r_u16(guest_right + axis * 2);
        output[axis] = (uint16)(first - second);
    }
}

static void WriteFlexibleTraversalRailVector(uint32 destination, const uint16 vector[3])
{
    uint32 value = (uint32)vector[0] | ((uint32)vector[1] << 16);
    uint32 lane = (destination + 3) & 3;
    w_masked_u32((destination + 3) & ~3, value >> (8 * (3 - lane)), (1 << (lane + 1)) - 1);
    lane = destination & 3;
    w_masked_u32(destination & ~3, value << (8 * lane), (15 << lane) & 15);
    w_u16(destination + 4, vector[2]);
}

TraversalRail &TraversalRail::Attach(uint32 object)
{
    return *static_cast<TraversalRail *>(FindNativeObjectStorage(object));
}

FlexibleTraversalRail &FlexibleTraversalRail::Attach(uint32 object)
{
    return *static_cast<FlexibleTraversalRail *>(FindNativeObjectStorage(object));
}

static uint32 ForwardDot(uint32 record, const uint32 delta[3])
{
    uint32 x, z;
    x = (uint32)((sint32)delta[0] >> 12) * r_u32(record + 308);
    z = (uint32)((sint32)delta[2] >> 12) * r_u32(record + 316);
    return x + z;
}

static void StoreOutput(uint32 guest, uint32 *native, const uint32 values[3])
{
    if (native)
        memcpy(native, values, 12);
    else
    {
        w_u32(guest, values[0]);
        w_u32(guest + 4, values[1]);
        w_u32(guest + 8, values[2]);
    }
}

static uint32 ReadOutput(uint32 guest, const uint32 *native, uint32 index)
{
    uint32 value;
    if (!native)
        return r_u32(guest + 4 * index);
    memcpy(&value, (const unsigned char *)native + 4 * index, 4);
    return value;
}

TraversalRail::TraversalRail(const uint32 *data, uint32 index) : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 value, cursor;
    FUNCTION_MARKER(0x80018E74, "SLUS_003.73");
    w_u32(object + 68, 0x800A07C0);
    value = ReadPathRecordWord(data, 0);
    w_u32(object + 4, value << 12);
    value = ReadPathRecordWord(data, 1);
    w_u32(object + 8, value << 12);
    value = ReadPathRecordWord(data, 2);
    w_u16(object + 214, index);
    w_u32(object + 12, value << 12);
    cursor = sub_80066088(index);
    sub_8006613C(object + 296, r_u16(cursor + 2));
    Initialize();
}

uint32 TraversalRail::Initialize()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, secondary_flags, left, right, x, z, angle, upper, value, length, effect;
    sint32 count;
    uint32 direction[3], normal[3], difference[3];
    FUNCTION_MARKER(0x80018F90, "SLUS_003.73");
    flags = r_u16(object);
    secondary_flags = r_u16(object + 78);
    w_u16(object, flags | 1);
    w_u16(object + 78, secondary_flags & 0xFFEF);
    sub_80062A38(object, 0x800FF4E8);
    left = r_u32(object + 304);
    right = r_u32(object + 12);
    x = r_u32(object + 296);
    z = r_u32(object + 4);
    angle = 1024 - (uint32)ratan2((sint32)(left - right), (sint32)(x - z));
    left = r_u32(object + 296);
    right = r_u32(object + 4);
    w_u16(object + 320, angle & 4095);
    direction[0] = (uint32)((sint32)(left - right) >> 12);
    left = r_u32(object + 300);
    right = r_u32(object + 8);
    direction[1] = (uint32)((sint32)(left - right) >> 12);
    left = r_u32(object + 304);
    right = r_u32(object + 12);
    direction[2] = (uint32)((sint32)(left - right) >> 12);
    xport_draft_host_sub_800872BC_p12(direction, normal);
    w_u32(object + 308, normal[0]);
    w_u32(object + 312, normal[1]);
    w_u32(object + 316, normal[2]);
    left = r_u32(object + 4);
    right = r_u32(object + 296);
    upper = right;
    if ((sint32)left < (sint32)right)
        w_u32(object + 324, left);
    else
    {
        upper = r_u32(object + 4);
        w_u32(object + 324, right);
    }
    w_u32(object + 328, upper);
    left = r_u32(object + 12);
    right = r_u32(object + 304);
    upper = right;
    if ((sint32)left < (sint32)right)
        w_u32(object + 332, left);
    else
    {
        upper = r_u32(object + 12);
        w_u32(object + 332, right);
    }
    w_u32(object + 336, upper);
    value = r_u32(object + 324);
    w_u32(object + 324, value - 0x100000);
    left = r_u32(object + 328);
    right = r_u32(object + 336);
    w_u32(object + 328, left + 0x100000);
    value = r_u32(object + 332);
    w_u32(object + 336, right + 0x100000);
    w_u32(object + 332, value - 0x100000);
    xport_draft_host_sub_8006C3AC_p1(difference, object + 296, object + 4);
    length = xport_draft_host_sub_8006BF04_p1(difference);
    w_u32(object + 340, length);
    count = (sint32)length / 512;
    if (count < 3)
        count = 3;
    effect = sub_80032DC0(100);
    if (effect)
    {
        void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(SegmentEffect));
        new (dram_addr) SegmentEffect((uint32)count);
    }
    w_u32(object + 344, effect);
    SegmentEffect::Attach(effect).SetEndpoints((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 296, 12));
    w_u8(r_u32(object + 344) + 66, 1);
    SegmentEffect::Attach(r_u32(object + 344)).Enable();
    w_u16(object + 58, 400);
    return 400;
}

uint32 TraversalRail::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, receiver, target;
    FUNCTION_MARKER(0x8001919C, "SLUS_003.73");
    w_u32(object + 68, 0x800A07C0);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    child = r_u32(object + 344);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 AdvanceMovementConstraint(uint32 record, uint32 position, uint32 scale, uint32 guest_output, uint32 *native_output)
{
    uint32 scaled[3], vector[3], delta[3], distance, index, left, right;
    FUNCTION_MARKER(0x8001922C, "SLUS_003.73");
    MultiplyScalarVector(scaled, &scale, (const uint32 *)psx_addr(record + 308, 12));
    AddVectors(vector, (const uint32 *)psx_addr(position, 12), scaled);
    StoreOutput(guest_output, native_output, vector);
    // The original writes the candidate even when the later test fails
    for (index = 0; index < 3; ++index)
    {
        left = ReadOutput(guest_output, native_output, index);
        right = r_u32(record + 4 + 4 * index);
        delta[index] = left - right;
    }
    distance = MeasureVectorLength(delta);
    if ((sint32)distance >= (sint32)r_u32(record + 340))
        return 0;
    return (sint32)ForwardDot(record, delta) >= 0;
}

uint32 ProjectMovementConstraint(uint32 record, uint32 position, uint32 guest_output, uint32 *native_output)
{
    uint32 delta[3], scaled[3], vector[3], distance;
    FUNCTION_MARKER(0x80019318, "SLUS_003.73");
    SubtractVectors(delta, (const uint32 *)psx_addr(position, 12), (const uint32 *)psx_addr(record + 4, 12));
    distance = MeasureVectorLength(delta);
    if ((sint32)distance >= (sint32)r_u32(record + 340))
        return 0;
    if ((sint32)ForwardDot(record, delta) < 0)
        return 0;
    MultiplyVector(scaled, (const uint32 *)psx_addr(record + 308, 12), &distance);
    AddVectors(vector, (const uint32 *)psx_addr(record + 4, 12), scaled);
    StoreOutput(guest_output, native_output, vector);
    return 1;
}

uint32 TraversalRail::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 camera, distance, color, effect, storage, index, offset, player, lower, coordinate, upper, result;
    uint32 collision[4];
    FUNCTION_MARKER(0x800193F8, "SLUS_003.73");
    camera = r_u32(0x800FF904);
    distance = sub_8006696C(object + 4, camera + 4);
    color = GetTraversalRailBrightness(distance);
    effect = r_u32(object + 344);
    w_u8(effect + 94, color);
    w_u8(effect + 93, color);
    w_u8(effect + 92, color);
    effect = r_u32(object + 344);
    index = 0;
    if ((sint32)r_u32(effect + 72) > 0)
    {
        offset = 0;
        do
        {
            storage = r_u32(effect + 76);
            camera = r_u32(0x800FF904);
            distance = sub_8006696C(storage + offset, camera + 4);
            color = GetTraversalRailBrightness(distance);
            storage = r_u32(r_u32(object + 344) + 76) + offset;
            w_u8(storage + 14, color);
            w_u8(storage + 13, color);
            w_u8(storage + 12, color);
            effect = r_u32(object + 344);
            ++index;
            result = (sint32)index < (sint32)r_u32(effect + 72);
            offset += 16;
        } while (result);
    }
    player = r_u32(0x800FF5A0);
    lower = r_u32(object + 324);
    coordinate = r_u32(player + 4);
    result = (sint32)lower < (sint32)coordinate;
    if (!result)
        return result;
    upper = r_u32(object + 328);
    result = (sint32)coordinate < (sint32)upper;
    if (!result)
        return result;
    coordinate = r_u32(player + 12);
    lower = r_u32(object + 332);
    result = (sint32)lower < (sint32)coordinate;
    if (!result)
        return result;
    upper = r_u32(object + 336);
    result = (sint32)coordinate < (sint32)upper;
    if (!result)
        return result;
    result = r_u32(player + 508);
    if (result)
        return result;
    result = xport_draft_host_sub_8007C398_p3(object + 4, object + 296, collision, player, 0);
    player = r_u32(0x800FF5A0);
    if (result == player)
        w_u32(result + 512, object);
    return result;
}

FlexibleTraversalRail::FlexibleTraversalRail(const uint32 *data, uint32 index)
    : TraversalRail(data, index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, offset, item;
    FUNCTION_MARKER(0x800196D4, "SLUS_003.73");
    w_u32(object + 68, 0x800A0780);
    // Original register-only delay loop has no guest memory or call effects
    count = r_u32(r_u32(object + 344) + 72) - 1;
    offset = 0;
    for (item = 0; (sint32)item < (sint32)count; ++item)
    {
        w_u16(object + 356 + offset, 0);
        w_u16(object + 358 + offset, 0);
        w_u16(object + 360 + offset, 0);
        w_u16(object + 500 + offset, 0);
        w_u16(object + 502 + offset, 0);
        w_u16(object + 504 + offset, 0);
        offset += 6;
    }
    w_u32(object + 352, 0);
    w_u16(object + 58, 401);
}

uint32 FlexibleTraversalRail::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80019788, "SLUS_003.73");
    w_u32(object + 68, 0x800A0780);
    result = TraversalRail::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 FlexibleTraversalRail::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 camera, distance, color, effect, storage, index, offset, player, lower, coordinate, upper, result, value;
    uint32 collision[4], count, bank, current, previous, following, pointer, axis, x, y, z;
    uint32 difference[3], step[3], position[3];
    uint16 neighbor[3], delta[3], adjusted[3];
    FUNCTION_MARKER(0x800198A4, "SLUS_003.73");
    camera = r_u32(0x800FF904);
    distance = sub_8006696C(object + 4, camera + 4);
    color = GetTraversalRailBrightness(distance);
    effect = r_u32(object + 344);
    w_u8(effect + 94, color);
    w_u8(effect + 93, color);
    w_u8(effect + 92, color);
    effect = r_u32(object + 344);
    index = 0;
    if ((sint32)r_u32(effect + 72) > 0)
    {
        offset = 0;
        do
        {
            storage = r_u32(effect + 76);
            camera = r_u32(0x800FF904);
            distance = sub_8006696C(storage + offset, camera + 4);
            color = GetTraversalRailBrightness(distance);
            storage = r_u32(r_u32(object + 344) + 76) + offset;
            w_u8(storage + 14, color);
            w_u8(storage + 13, color);
            w_u8(storage + 12, color);
            effect = r_u32(object + 344);
            ++index;
            result = (sint32)index < (sint32)r_u32(effect + 72);
            offset += 16;
        } while (result);
    }

    value = r_u32(object + 348);
    if (value)
    {
        bank = r_u32(object + 352);
        w_u32(object + 348, value - 1);
        current = object + 356 + bank * 144;
        pointer = object + 356 + (bank ^ 1) * 144;
        following = pointer + 6;
        previous = pointer - 6;
        count = r_u32(r_u32(object + 344) + 72);
        for (index = 0; (sint32)index < (sint32)(count - 1); ++index)
        {
            if (index == 0)
                SubtractFlexibleTraversalRailVector(delta, 0, following, current);
            else if (index == count - 2)
                SubtractFlexibleTraversalRailVector(delta, 0, previous, current);
            else
            {
                AddFlexibleTraversalRailNeighbors(neighbor, previous, following);
                SubtractFlexibleTraversalRailVector(delta, neighbor, 0, current);
            }
            for (axis = 0; axis < 3; ++axis)
                adjusted[axis] = (uint16)(delta[axis] - (uint16)((sint16)delta[axis] >> 3));
            WriteFlexibleTraversalRailVector(current, adjusted);
            current += 6;
            following += 6;
            previous += 6;
        }
        xport_draft_host_sub_8006C3AC_p1(difference, object + 296, object + 4);
        xport_draft_host_sub_8006C4EC_p123(step, difference, &count);
        position[0] = r_u32(object + 4);
        position[1] = r_u32(object + 8);
        position[2] = r_u32(object + 12);
        effect = r_u32(object + 344);
        x = r_u32(object + 4);
        y = r_u32(object + 8);
        z = r_u32(object + 12);
        w_u32(effect + 80, x);
        w_u32(effect + 84, y);
        w_u32(effect + 88, z);
        offset = 0;
        for (index = 0; (sint32)index < (sint32)(count - 1); ++index)
        {
            for (axis = 0; axis < 3; ++axis)
                position[axis] += step[axis];
            for (axis = 0; axis < 3; ++axis)
            {
                bank = r_u32(object + 352);
                value = (uint32)(sint32)(sint16)r_u16(object + 356 + bank * 144 + offset + axis * 2);
                position[axis] += value << 12;
            }
            storage = r_u32(r_u32(object + 344) + 76) + index * 16;
            w_u32(storage, position[0]);
            w_u32(storage + 4, position[1]);
            w_u32(storage + 8, position[2]);
            offset += 6;
        }
        storage = r_u32(r_u32(object + 344) + 76) + count * 16;
        x = r_u32(object + 296);
        y = r_u32(object + 300);
        z = r_u32(object + 304);
        w_u32(storage - 16, x);
        w_u32(storage - 12, y);
        w_u32(storage - 8, z);
        bank = r_u32(object + 352);
        w_u32(object + 352, bank ^ 1);
    }
    player = r_u32(0x800FF5A0);
    lower = r_u32(object + 324);
    coordinate = r_u32(player + 4);
    result = (sint32)lower < (sint32)coordinate;
    if (!result)
        return result;
    upper = r_u32(object + 328);
    result = (sint32)coordinate < (sint32)upper;
    if (!result)
        return result;
    coordinate = r_u32(player + 12);
    lower = r_u32(object + 332);
    result = (sint32)lower < (sint32)coordinate;
    if (!result)
        return result;
    upper = r_u32(object + 336);
    result = (sint32)coordinate < (sint32)upper;
    if (!result)
        return result;
    result = r_u32(player + 508);
    if (result)
        return result;
    result = xport_draft_host_sub_8007C398_p3(object + 4, object + 296, collision, player, 0);
    player = r_u32(0x800FF5A0);
    if (result == player)
        w_u32(result + 512, object);
    return result;
}

extern "C" uint32 sub_80018E74(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80018E74, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TraversalRail));
    new (dram_addr) TraversalRail((const uint32 *)psx_addr(data, 12), index);
    return object;
}

extern "C" uint32 sub_80018F90(uint32 object)
{
    FUNCTION_MARKER(0x80018F90, "SLUS_003.73");
    return TraversalRail::Attach(object).TraversalRail::Initialize();
}

extern "C" uint32 sub_8001919C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001919C, "SLUS_003.73");
    return TraversalRail::Attach(object).TraversalRail::Destroy(reason);
}

extern "C" uint32 sub_800193F8(uint32 object)
{
    FUNCTION_MARKER(0x800193F8, "SLUS_003.73");
    return TraversalRail::Attach(object).TraversalRail::Update();
}

extern "C" uint32 sub_800196D4(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x800196D4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FlexibleTraversalRail));
    new (dram_addr) FlexibleTraversalRail((const uint32 *)psx_addr(data, 12), index);
    return object;
}

extern "C" uint32 sub_80019788(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80019788, "SLUS_003.73");
    return FlexibleTraversalRail::Attach(object).FlexibleTraversalRail::Destroy(reason);
}

extern "C" uint32 sub_800198A4(uint32 object)
{
    FUNCTION_MARKER(0x800198A4, "SLUS_003.73");
    return FlexibleTraversalRail::Attach(object).FlexibleTraversalRail::Update();
}

extern "C" uint32 sub_8001922C(uint32 record, uint32 position, uint32 scale, uint32 output)
{
    FUNCTION_MARKER(0x8001922C, "SLUS_003.73");
    return AdvanceMovementConstraint(record, position, scale, output, 0);
}

extern "C" uint32 apocalypse_movement_1922C_native(uint32 record, uint32 position, uint32 scale, uint32 output[3])
{
    return AdvanceMovementConstraint(record, position, scale, 0, output);
}

extern "C" uint32 sub_80019318(uint32 record, uint32 position, uint32 output)
{
    FUNCTION_MARKER(0x80019318, "SLUS_003.73");
    return ProjectMovementConstraint(record, position, output, 0);
}

extern "C" uint32 apocalypse_movement_19318_native(uint32 record, uint32 position, uint32 output[3])
{
    return ProjectMovementConstraint(record, position, 0, output);
}
