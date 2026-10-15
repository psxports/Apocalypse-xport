#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "visual_object.h"
#include "vector_arithmetic.h"
#include "animation.h"
#include "vector_distance.h"
#include "activation_record.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

// Original list-level operation has a list-head receiver, not an object receiver

// List traversal is a free function, not an object method

static void CopyObjectPosition(uint32 destination, uint32 source)
{
    uint32 x = r_u32(source);
    uint32 y = r_u32(source + 4);
    uint32 z = r_u32(source + 8);
    w_u32(destination, x);
    w_u32(destination + 4, y);
    w_u32(destination + 8, z);
}

void FlagObjectNearReference(uint32 object)
{
    FUNCTION_MARKER(0x80029CC4, "SLUS_003.73");
    uint32 reference = r_u32(0x800FF21C);
    if (!reference)
        return;
    uint32 distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(reference + 24, 12));
    reference = r_u32(0x800FF21C);
    uint32 radius = r_u32(reference + 168);
    if ((sint32)radius < (sint32)distance)
        return;
    uint32 flags = r_u16(object + 78);
    w_u16(object + 78, flags | 256);
}

void SmoothObjectPositions(uint32 object)
{
    FUNCTION_MARKER(0x80030168, "SLUS_003.73");
    uint32 current[3], previous_quarter[3], current_quarter[3], weighted[3], output[3];
    uint32 shift = 2, weight = 3, axis;
    uint32 special_head = r_u32(0x800FF5A0);
    if (object == special_head)
    {
        for (axis = 0; axis < 3; ++axis)
            current[axis] = r_u32(object + 184 + axis * 4);
        ShiftVectorRight(previous_quarter, (const uint32 *)psx_addr(object + 588, 12), &shift);
        ShiftVectorRight(current_quarter, current, &shift);
        MultiplyScalarVector(weighted, &weight, current_quarter);
        AddVectors(output, previous_quarter, weighted);
        for (axis = 0; axis < 3; ++axis)
            w_u32(object + 184 + axis * 4, output[axis]);
        for (axis = 0; axis < 3; ++axis)
            w_u32(object + 588 + axis * 4, current[axis]);
    }
    while (object)
    {
        for (axis = 0; axis < 3; ++axis)
            current[axis] = r_u32(object + 4 + axis * 4);
        ShiftVectorRight(previous_quarter, (const uint32 *)psx_addr(object + 240, 12), &shift);
        ShiftVectorRight(current_quarter, current, &shift);
        MultiplyScalarVector(weighted, &weight, current_quarter);
        AddVectors(output, previous_quarter, weighted);
        for (axis = 0; axis < 3; ++axis)
            w_u32(object + 4 + axis * 4, output[axis]);
        for (axis = 0; axis < 3; ++axis)
            w_u32(object + 240 + axis * 4, current[axis]);
        object = r_u32(object + 28);
    }
}

uint32 SaveObjectPositions(uint32 object)
{
    FUNCTION_MARKER(0x8003032C, "SLUS_003.73");
    uint32 special_head = r_u32(0x800FF5A0);
    if (object == special_head)
        CopyObjectPosition(object + 588, object + 184);
    while (object)
    {
        CopyObjectPosition(object + 240, object + 4);
        object = r_u32(object + 28);
    }
    return special_head;
}

uint32 RestoreObjectPositions(uint32 object)
{
    FUNCTION_MARKER(0x80030390, "SLUS_003.73");
    uint32 special_head = r_u32(0x800FF5A0);
    if (object == special_head)
        CopyObjectPosition(object + 184, object + 588);
    while (object)
    {
        CopyObjectPosition(object + 4, object + 240);
        object = r_u32(object + 28);
    }
    return special_head;
}

void UpdateVisualObjects(uint32 *head)
{
    uint32 list = GuestMemoryAddress(head);
    if (!list)
    {
        // TODO Bind native list heads to the original update traversal
        GameDiagnostic("TODO Native visual update list head\n");
        abort();
    }
    FUNCTION_MARKER(0x80063770, "SLUS_003.73");
    uint32 object = r_u32(list);
    uint32 next = object ? r_u32(object + 28) : 0;
    while (object)
    {
        VisualObject::Attach(object).AdvanceColor();
        uint32 flags = r_u16(object + 78);
        if (flags & 64)
        {
            if (flags & 128)
            {
                uint32 table = r_u32(object + 68);
                sint32 adjustment = (sint16)r_u16(table + 8);
                uint32 target = r_u32(table + 12);
                apocalypse_object_cleanup(target, object + (uint32)adjustment, 3);
            }
            else
                w_u16(object + 78, flags | 128);
        }
        else
        {
            uint32 player = r_u32(0x800FF5A0);
            uint32 distance = MeasurePositionDistance((const uint32 *)psx_addr(player + 4, 12), (const uint32 *)psx_addr(object + 4, 12));
            if (!DeactivateDistantObject(object, distance))
            {
                flags = r_u16(object + 78);
                w_u32(object + 220, distance);
                if ((flags & 2) && r_u32(0x800FF5EC) < distance)
                    VisualObject::Attach(object).MoveToTemporaryList(head);
                else
                {
                    UpdateAnimationPose((uint8 *)psx_addr(object, 1));
                    VisualObject::Attach(object).UpdateShadow();
                }
            }
        }
        object = next;
        if (!next)
            return;
        next = r_u32(next + 28);
    }
}

extern "C" void sub_80063770(uint32 list)
{
    FUNCTION_MARKER(0x80063770, "SLUS_003.73");
    UpdateVisualObjects((uint32 *)psx_addr(list, 4));
}

extern "C" uint32 sub_80030168(uint32 object)
{
    FUNCTION_MARKER(0x80030168, "SLUS_003.73");
    SmoothObjectPositions(object);
    // Native ABI omits the unused original local-pointer return carrier
    return 0;
}

extern "C" uint32 sub_8003032C(uint32 object)
{
    FUNCTION_MARKER(0x8003032C, "SLUS_003.73");
    return SaveObjectPositions(object);
}

extern "C" uint32 sub_80030390(uint32 object)
{
    FUNCTION_MARKER(0x80030390, "SLUS_003.73");
    return RestoreObjectPositions(object);
}

extern "C" void sub_80029CC4(uint32 object)
{
    FUNCTION_MARKER(0x80029CC4, "SLUS_003.73");
    FlagObjectNearReference(object);
}
