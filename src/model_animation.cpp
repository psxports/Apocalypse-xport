#include "game_sdk_bindings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "model_animation.h"
#include "angle_math.h"
#include "game_heap.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 DivideAnimationWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 RemainderAnimationWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return numerator;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return 0;
    return (uint32)((sint32)numerator % (sint32)denominator);
}

uint32 InitializeModelAnimation(uint32 object, uint32 record)
{
    uint32 count, model, header, vertices, storage, result, row, candidate, base, left, right;
    FUNCTION_MARKER(0x8007CAC8, "SLUS_003.73");
    count = r_u16(record + 2);
    model = r_u8(object + 27);
    w_u32(object + 364, record + 4);
    header = r_u32(0x800EAEF8 + model * 64 + 16);
    vertices = r_u32(header - 4);
    storage = AllocateHeapBlock(vertices * 24, 0, 1);
    w_u32(object + 356, storage);
    result = AllocateHeapBlock(count * 12, 0, 1);
    w_u32(object + 360, result);
    for (row = 0; row < count; ++row)
    {
        storage = r_u32(object + 360) + row * 12;
        w_u16(storage + 10, 0);
        w_u16(storage + 8, 0);
        w_u16(storage + 6, 0);
        w_u16(storage + 4, 0);
        w_u16(storage + 2, 0);
        w_u16(storage, 0);
        result = row + 1 < count;
    }
    for (row = 0; row < count; ++row)
    {
        for (candidate = 0; candidate < count; ++candidate)
        {
            base = r_u32(object + 364);
            left = r_u16(base + row * 12 + 2);
            right = r_u16(base + candidate * 12);
            if (left == right)
            {
                w_u16(base + row * 12 + 10, candidate);
                break;
            }
        }
        if (candidate == count)
        {
            base = r_u32(object + 364);
            w_u16(base + row * 12 + 10, 65535);
        }
        result = row + 1 < count;
    }
    return result;
}

uint32 UpdateModelAnimationData(uint32 object)
{
    uint32 model, entry, animation, data, interval, header, vertices;
    FUNCTION_MARKER(0x8007CECC, "SLUS_003.73");
    model = r_u8(object + 27);
    entry = 0x800EAEF8 + model * 64;
    animation = r_u8(object + 26);
    data = r_u32(entry + 24);
    interval = r_u16(data + animation * 8 + 10);
    if (!interval)
        return interval;
    header = r_u32(entry + 16);
    vertices = r_u32(header - 4);
    return InterpolateModelAnimation(vertices * 4, interval + 1, data, object, 0, vertices);
}

uint32 InterpolateModelAnimation(uint32 components, uint32 interval, uint32 table, uint32 object, uint32 index_base, uint32 vertices)
{
    uint32 frame, lower, upper, entry, offset, limit, data, factor = 0;
    uint32 source0, source1, output, result, index, axis;
    uint16 difference[4];
    uint32 initial[3], interpolated[3];
    FUNCTION_MARKER(0x8007CF38, "SLUS_003.73");
    frame = r_u8(object + 24);
    lower = frame - RemainderAnimationWord(frame, interval);
    entry = table + r_u8(object + 26) * 8;
    upper = lower + interval;
    offset = r_u32(entry + 4);
    limit = r_u16(entry + 8);
    data = table + offset;
    if ((sint32)upper < (sint32)limit)
        factor = DivideAnimationWord((frame - lower) << 12, upper - lower);
    else if (r_u8(object + 296) == 1)
    {
        upper = 0;
        factor = DivideAnimationWord((frame - lower) << 12, limit - lower);
    }
    else
    {
        lower -= interval;
        if ((sint32)lower < 0)
            lower = 0;
        upper -= interval;
        if (upper)
            factor = DivideAnimationWord((frame - lower) << 12, upper - lower);
    }
    output = 0x800ED760 + index_base * 24;
    source0 = data + DivideAnimationWord(vertices * lower, interval) * 24 + index_base * 24;
    result = data + DivideAnimationWord(vertices * upper, interval) * 24;
    source1 = result + index_base * 24;
    for (index = 0; (sint32)index < (sint32)components; ++index)
    {
        SubtractAngleVector(difference, (const uint16 *)psx_addr(source1, 6), (const uint16 *)psx_addr(source0, 6));
        for (axis = 0; axis < 3; ++axis)
            initial[axis] = (uint32)(sint32)(sint16)r_u16(source0 + axis * 2);
        for (axis = 0; axis < 3; ++axis)
            xport_draft_gte_data_write(25 + axis, initial[axis]);
        for (axis = 0; axis < 3; ++axis)
            xport_draft_gte_data_write(9 + axis, (uint32)(sint32)(sint16)difference[axis]);
        xport_draft_gte_data_write(8, factor);
        xport_draft_gte_execute(0x1A8003E);
        for (axis = 0; axis < 3; ++axis)
            interpolated[axis] = xport_draft_gte_data_read(25 + axis);
        for (axis = 0; axis < 3; ++axis)
            w_u16(output + axis * 2, interpolated[axis]);
        source0 += 6;
        source1 += 6;
        output += 6;
        result = (sint32)(index + 1) < (sint32)components;
    }
    return result;
}

extern "C" uint32 sub_8007CAC8(uint32 object, uint32 record)
{
    FUNCTION_MARKER(0x8007CAC8, "SLUS_003.73");
    return InitializeModelAnimation(object, record);
}

extern "C" uint32 sub_8007CECC(uint32 object)
{
    FUNCTION_MARKER(0x8007CECC, "SLUS_003.73");
    return UpdateModelAnimationData(object);
}

extern "C" uint32 sub_8007CF38(uint32 components, uint32 interval, uint32 table, uint32 object, uint32 index_base, uint32 vertices)
{
    FUNCTION_MARKER(0x8007CF38, "SLUS_003.73");
    return InterpolateModelAnimation(components, interval, table, object, index_base, vertices);
}

uint32 SetModelBoneTable(const uint8 *object, const uint16 *record)
{
    FUNCTION_MARKER(0x8007CA50, "SLUS_003.73");
    uint32 record_address = GuestMemoryAddress(record);
    // TODO Persistent model records must remain in mapped game memory
    if (!record_address)
    {
        GameDiagnostic("TODO Unmapped persistent model bone table\n");
        abort();
    }
    uint32 object_address = GuestMemoryAddress(object);
    uint32 model = object_address ? r_u8(object_address + 27) : object[27];
    uint32 entry = 0x800EAEF8 + model * 64;
    w_u32(entry + 28, record_address + 4);
    return entry;
}

extern "C" uint32 sub_8007CA50(uint32 object, uint32 record)
{
    FUNCTION_MARKER(0x8007CA50, "SLUS_003.73");
    return SetModelBoneTable((const uint8 *)psx_addr(object, 28), (const uint16 *)psx_addr(record, 4));
}

// Resource-index owner has no recovered instance receiver
uint32 UpdateModelColorAnimation(uint32 model_index)
{
    uint32 model, current, end, step = 1, old_tick, tick, color_output;
    FUNCTION_MARKER(0x80080398, "SLUS_003.73");
    if (model_index == 0xFFFFFFFF)
        return 0xFFFFFFFF;
    model = 0x800EAEF8 + (model_index << 6);
    current = r_u32(model + 40);
    if (!current)
        return 0x800EAEF8;
    color_output = r_u32(model + 32);
    tick = r_u32(0x800FFB40);
    w_u32(0x800FFB04, color_output);
    end = current + r_u32(current - 4);
    old_tick = r_u32(0x800FFAB4);
    if (tick >= old_tick)
        step = tick - old_tick;
    while (current < end)
    {
        uint32 keys = current + 4;
        uint32 elapsed = r_u8(current + 3);
        uint32 key = r_u8(current + 2);
        uint32 count = r_u8(current + 1);
        uint32 active = keys + (key << 2);
        uint32 duration = r_u8(active + 3);
        uint32 next, base[3], delta[3], axis, scale, color_index;
        elapsed += step;
        if ((sint32)elapsed >= (sint32)duration)
        {
            // TODO: Original zero-duration cycles can fail to terminate
            do
            {
                elapsed -= r_u8(active + 3);
                active += 4;
                ++key;
                if (key == count)
                {
                    active = keys;
                    key = 0;
                }
            } while ((sint32)elapsed >= (sint32)r_u8(active + 3));
        }
        w_u8(current + 2, key);
        active = keys + (key << 2);
        w_u8(current + 3, elapsed);
        for (axis = 0; axis < 3; ++axis)
            base[axis] = (uint32)r_u8(active + axis) << 4;
        next = key + 1;
        if (next == count)
            next = 0;
        next = keys + (next << 2);
        for (axis = 0; axis < 3; ++axis)
            delta[axis] = ((uint32)r_u8(next + axis) << 4) - base[axis];
        xport_gte_write_data(6, 0);
        scale = DivideAnimationWord(elapsed << 12, r_u8(active + 3));
        xport_gte_write_data(8, scale);
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_data(25 + axis, base[axis]);
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_data(9 + axis, delta[axis]);
        xport_draft_gte_execute(0x1A8003E);
        color_index = r_u8(current);
        color_output = r_u32(0x800FFB04);
        w_u32(color_output + (color_index << 2), xport_gte_read_data(22));
        current += (count << 2) + 4;
    }
    return 0;
}

extern "C" uint32 sub_80080398(uint32 model_index)
{
    FUNCTION_MARKER(0x80080398, "SLUS_003.73");
    return UpdateModelColorAnimation(model_index);
}
