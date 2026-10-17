#include "model_resources.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "region_actions.h"
#include "visual_object.h"
#include "native_object_storage.h"
#include <string.h>

static uint32 ReadRegionWord(const uint32 *input, uint32 axis)
{
    uint32 guest = GuestMemoryAddress(input), value;
    if (guest)
        return r_u32(guest + axis * 4);
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

uint32 ApplyRegionToObjects(const uint32 *lower, const uint32 *upper, ModelInstance *instance, uint32 trigger)
{
    uint32 count = 0;
    FUNCTION_MARKER(0x80067ABC, "SLUS_003.73");
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (!object.IsFinished())
        {
            uint32 x = instance->pos[0];
            uint32 y = instance->pos[1];
            uint32 z = instance->pos[2];
            if ((sint32)x >= (sint32)ReadRegionWord(lower, 0) && (sint32)ReadRegionWord(upper, 0) >= (sint32)x && (sint32)z >= (sint32)ReadRegionWord(lower, 2) && (sint32)ReadRegionWord(upper, 2) >= (sint32)z && (sint32)y >= (sint32)ReadRegionWord(lower, 1) && (sint32)ReadRegionWord(upper, 1) >= (sint32)y)
            {
                if (trigger)
                    object.Finish();
                else
                    object.Destroy(3);
                ++count;
            }
        }
        instance = next;
    }
    return count;
}

uint32 RemoveRegionEffects(const uint32 *lower, const uint32 *upper)
{
    uint32 object, kind, next, axis, value, table, receiver, target, count = 0;
    FUNCTION_MARKER(0x8001F7A0, "SLUS_003.73");
    object = r_u32(0x800FF434);
    while (object)
    {
        kind = r_u8(object + 67);
        next = r_u32(object + 4);
        if (kind == 7)
        {
            for (axis = 0; axis < 3; ++axis)
            {
                value = r_u32(object + 24 + axis * 4);
                if ((sint32)value < (sint32)ReadRegionWord(lower, axis))
                    break;
                if ((sint32)ReadRegionWord(upper, axis) < (sint32)value)
                    break;
            }
            if (axis == 3)
            {
                table = r_u32(object + 68);
                ++count;
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 8);
                target = r_u32(table + 12);
                apocalypse_object_cleanup(target, receiver, 3);
            }
        }
        object = next;
    }
    return count;
}

uint32 ClearRegion(const uint32 *lower, const uint32 *upper)
{
    uint32 count;
    FUNCTION_MARKER(0x80067C20, "SLUS_003.73");
    count = ApplyRegionToObjects(lower, upper, visual_lists.actors, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.solids, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.pickups, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.suspended, 0);
    return count + RemoveRegionEffects(lower, upper);
}

extern "C" uint32 apocalypse_clear_region_native(const uint32 *lower, const uint32 *upper)
{
    return ClearRegion(lower, upper);
}







uint32 SetRegionObjectsEnabled(const uint32 *lower, const uint32 *upper, uint32 enabled)
{
    ModelInstance *object;
    uint32 position[3], order[3] = {0, 2, 1}, index, axis, result = 0x800F0000;
    FUNCTION_MARKER(0x80067CCC, "SLUS_003.73");
    object = GetModelSceneObjects();
    while (object)
    {
        result = GetModelBank(object->bank).loaded;
        if (result)
        {
            position[0] = object->pos[0];
            position[1] = object->pos[1];
            position[2] = object->pos[2];
            for (index = 0; index < 3; ++index)
            {
                axis = order[index];
                result = (sint32)position[axis] < (sint32)ReadRegionWord(lower, axis);
                if (result)
                    break;
                result = (sint32)ReadRegionWord(upper, axis) < (sint32)position[axis];
                if (result)
                    break;
            }
            if (index == 3)
            {
                result = object->flags;
                result = enabled ? result & 65534 : result | 1;
                object->flags = result;
            }
        }
        object = object->next;
    }
    return result;
}

extern "C" uint32 apocalypse_region_enabled_native(const uint32 *lower, const uint32 *upper, uint32 enabled)
{
    return SetRegionObjectsEnabled(lower, upper, enabled);
}


