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

uint32 ApplyRegionToObjects(const uint32 *lower, const uint32 *upper, uint32 object, uint32 trigger)
{
    uint32 next, count = 0, x, y, z, table, receiver, target;
    FUNCTION_MARKER(0x80067ABC, "SLUS_003.73");
    while (object)
    {
        next = r_u32(object + 28);
        if (!sub_80062F48(object))
        {
            x = r_u32(object + 4);
            y = r_u32(object + 8);
            z = r_u32(object + 12);
            if ((sint32)x >= (sint32)ReadRegionWord(lower, 0) && (sint32)ReadRegionWord(upper, 0) >= (sint32)x && (sint32)z >= (sint32)ReadRegionWord(lower, 2) && (sint32)ReadRegionWord(upper, 2) >= (sint32)z && (sint32)y >= (sint32)ReadRegionWord(lower, 1) && (sint32)ReadRegionWord(upper, 1) >= (sint32)y)
            {
                table = r_u32(object + 68);
                if (trigger)
                {
                    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
                    target = r_u32(table + 20);
                    (void)DispatchObjectFinish(target, receiver);
                }
                else
                {
                    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 8);
                    target = r_u32(table + 12);
                    apocalypse_object_cleanup(target, receiver, 3);
                }
                ++count;
            }
        }
        object = next;
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
    count = ApplyRegionToObjects(lower, upper, r_u32(0x800FF4E8), 1);
    count += ApplyRegionToObjects(lower, upper, r_u32(0x800FF5DC), 1);
    count += ApplyRegionToObjects(lower, upper, r_u32(0x800FF204), 1);
    count += ApplyRegionToObjects(lower, upper, r_u32(0x800FF5E0), 0);
    return count + RemoveRegionEffects(lower, upper);
}

extern "C" uint32 apocalypse_clear_region_native(const uint32 *lower, const uint32 *upper)
{
    return ClearRegion(lower, upper);
}

extern "C" uint32 sub_80067ABC(uint32 lower, uint32 upper, uint32 object, uint32 trigger)
{
    FUNCTION_MARKER(0x80067ABC, "SLUS_003.73");
    return ApplyRegionToObjects((const uint32 *)psx_addr(lower, 12), (const uint32 *)psx_addr(upper, 12), object, trigger);
}

extern "C" uint32 sub_8001F7A0(uint32 lower, uint32 upper)
{
    FUNCTION_MARKER(0x8001F7A0, "SLUS_003.73");
    return RemoveRegionEffects((const uint32 *)psx_addr(lower, 12), (const uint32 *)psx_addr(upper, 12));
}

extern "C" uint32 sub_80067C20(uint32 lower, uint32 upper)
{
    FUNCTION_MARKER(0x80067C20, "SLUS_003.73");
    return ClearRegion((const uint32 *)psx_addr(lower, 12), (const uint32 *)psx_addr(upper, 12));
}

uint32 SetRegionObjectsEnabled(const uint32 *lower, const uint32 *upper, uint32 enabled)
{
    uint32 object, resource, position[3], order[3] = {0, 2, 1}, index, axis, result = 0x800F0000;
    FUNCTION_MARKER(0x80067CCC, "SLUS_003.73");
    object = r_u32(0x800FF794);
    while (object)
    {
        resource = 0x800EAEF8 + r_u8(object + 27) * 64;
        result = r_u8(resource + 10);
        if (result)
        {
            position[0] = r_u32(object + 4);
            position[1] = r_u32(object + 8);
            position[2] = r_u32(object + 12);
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
                result = r_u16(object);
                result = enabled ? result & 65534 : result | 1;
                w_u16(object, result);
            }
        }
        object = r_u32(object + 28);
    }
    return result;
}

extern "C" uint32 apocalypse_region_enabled_native(const uint32 *lower, const uint32 *upper, uint32 enabled)
{
    return SetRegionObjectsEnabled(lower, upper, enabled);
}

extern "C" uint32 sub_80067CCC(uint32 lower, uint32 upper, uint32 enabled)
{
    FUNCTION_MARKER(0x80067CCC, "SLUS_003.73");
    return SetRegionObjectsEnabled((const uint32 *)psx_addr(lower, 12), (const uint32 *)psx_addr(upper, 12), enabled);
}
