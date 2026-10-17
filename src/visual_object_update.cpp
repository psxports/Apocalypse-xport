#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "visual_object.h"
#include "vector_arithmetic.h"
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

void SmoothObjectPositions(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80030168, "SLUS_003.73");
    uint32 cur[3], prev_quarter[3], cur_quarter[3], weighted[3], output[3];
    uint32 shift = 2, weight = 3, axis;
    if (instance == visual_lists.players)
    {
        const uint32 object = NativeObjectGuestAddress(instance->owner);
        for (axis = 0; axis < 3; ++axis)
            cur[axis] = r_u32(object + 184 + axis * 4);
        ShiftVectorRight(prev_quarter, (const uint32 *)psx_addr(object + 588, 12), &shift);
        ShiftVectorRight(cur_quarter, cur, &shift);
        MultiplyScalarVector(weighted, &weight, cur_quarter);
        AddVectors(output, prev_quarter, weighted);
        for (axis = 0; axis < 3; ++axis)
            w_u32(object + 184 + axis * 4, output[axis]);
        for (axis = 0; axis < 3; ++axis)
            w_u32(object + 588 + axis * 4, cur[axis]);
    }
    while (instance)
    {
        for (axis = 0; axis < 3; ++axis)
            cur[axis] = instance->pos[axis];
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        ShiftVectorRight(prev_quarter, visual.pos_history.saved, &shift);
        ShiftVectorRight(cur_quarter, cur, &shift);
        MultiplyScalarVector(weighted, &weight, cur_quarter);
        AddVectors(output, prev_quarter, weighted);
        for (axis = 0; axis < 3; ++axis)
            instance->pos[axis] = output[axis];
        for (axis = 0; axis < 3; ++axis)
            visual.pos_history.saved[axis] = cur[axis];
        instance = instance->next;
    }
}

ModelInstance *SaveObjectPositions(ModelInstance *instance)
{
    FUNCTION_MARKER(0x8003032C, "SLUS_003.73");
    ModelInstance *special_head = visual_lists.players;
    if (instance == special_head)
    {
        const uint32 object = NativeObjectGuestAddress(instance->owner);
        CopyObjectPosition(object + 588, object + 184);
    }
    while (instance)
    {
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        for (uint32 axis = 0; axis < 3; ++axis)
            visual.pos_history.saved[axis] = instance->pos[axis];
        instance = instance->next;
    }
    return special_head;
}

ModelInstance *RestoreObjectPositions(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80030390, "SLUS_003.73");
    ModelInstance *special_head = visual_lists.players;
    if (instance == special_head)
    {
        const uint32 object = NativeObjectGuestAddress(instance->owner);
        CopyObjectPosition(object + 184, object + 588);
    }
    while (instance)
    {
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        for (uint32 axis = 0; axis < 3; ++axis)
            instance->pos[axis] = visual.pos_history.saved[axis];
        instance = instance->next;
    }
    return special_head;
}

void UpdateVisualObjects(ModelInstance *&head)
{
    FUNCTION_MARKER(0x80063770, "SLUS_003.73");
    ModelInstance *instance = head;
    ModelInstance *next = instance ? instance->next : 0;
    while (instance)
    {
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        const uint32 object = NativeObjectGuestAddress(&visual);
        visual.AdvanceColor();
        uint32 flags = visual.lifecycle_flags;
        if (flags & 64)
        {
            if (flags & 128)
                visual.Destroy(3);
            else
                visual.lifecycle_flags = flags | 128;
        }
        else
        {
            uint32 distance = MeasurePositionDistance(visual_lists.players->pos, instance->pos);
            if (!DeactivateDistantObject(object, distance))
            {
                flags = visual.lifecycle_flags;
                w_u32(object + 220, distance);
                if ((flags & 2) && r_u32(0x800FF5EC) < distance)
                    visual.MoveToTemporaryList(head);
                else
                {
                    visual.UpdateAnimationPose();
                    visual.UpdateShadow();
                }
            }
        }
        instance = next;
        if (!next)
            return;
        next = next->next;
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
    SmoothObjectPositions(object ? &VisualObject::Attach(object).model : 0);
    // Native ABI omits the unused original local-pointer return carrier
    return 0;
}






