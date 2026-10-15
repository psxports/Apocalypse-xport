extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "enemy_list.h"
#include "visual_object.h"
#include "vector_distance.h"
#include "record_factory.h"
#include "native_object_storage.h"
#include <string.h>

uint32 FinishSelectedEnemies()
{
    uint32 object, next, kind, relative, table, receiver, target;
    uint32 result = 0x800A0000;
    FUNCTION_MARKER(0x8005C338, "SLUS_003.73");
    object = r_u32(0x800FF4E8);
    while (object)
    {
        kind = r_u16(object + 58);
        next = r_u32(object + 28);
        relative = kind - 200;
        if (relative < 11)
        {
            switch (kind)
            {
                case 200: case 201: case 202: case 204: case 205:
                case 207: case 208: case 209: case 210:
                    table = r_u32(object + 68);
                    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
                    target = r_u32(table + 20);
                    result = xport_draft_guest_call1(target, receiver);
                    break;
                default:
                    result = 0x8005C3A4;
                    break;
            }
        }
        else
            result = relative << 2;
        object = next;
    }
    // Empty and skipped paths retain the original V0 carrier
    return result;
}

extern "C" uint32 sub_8005C338(void)
{
    FUNCTION_MARKER(0x8005C338, "SLUS_003.73");
    return FinishSelectedEnemies();
}

void ActivateObjectsByIndex(uint32 object, uint32 index)
{
    uint32 identifier, flags;
    FUNCTION_MARKER(0x80064E10, "SLUS_003.73");
    while (object)
    {
        identifier = r_u16(object + 214);
        if (identifier == index)
        {
            flags = r_u16(object + 76);
            w_u16(object + 76, flags | 1);
        }
        object = r_u32(object + 28);
    }
}

extern "C" void sub_80064E10(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x80064E10, "SLUS_003.73");
    ActivateObjectsByIndex(object, index);
}

uint32 ActivateEnemyIndices(const uint16 *indices)
{
    uint32 guest, count, index, identifier, table, record, kind, head;
    uint16 half;
    FUNCTION_MARKER(0x80064E50, "SLUS_003.73");
    guest = GuestMemoryAddress(indices);
    if (guest)
        count = r_u16(guest);
    else
    {
        memcpy(&half, indices, 2);
        count = half;
    }
    for (index = 0; index < count; ++index)
    {
        if (guest)
            identifier = r_u16(guest + 2 + index * 2);
        else
        {
            memcpy(&half, (const uint8 *)indices + 2 + index * 2, 2);
            identifier = half;
        }
        table = r_u32(0x800FF624);
        record = r_u32(table + identifier * 4);
        kind = (uint32)(sint32)(sint16)r_u16(record);
        if (kind == 1 || kind == 7)
        {
            head = r_u32(0x800FF4E8);
            ActivateObjectsByIndex(head, identifier);
            head = r_u32(0x800FF5DC);
            ActivateObjectsByIndex(head, identifier);
        }
    }
    return 0;
}

extern "C" uint32 sub_80064E50(uint32 indices)
{
    FUNCTION_MARKER(0x80064E50, "SLUS_003.73");
    return ActivateEnemyIndices((const uint16 *)psx_addr(indices, 2));
}

uint32 SpawnEnemyIndices(const uint16 *indices)
{
    uint32 guest, count, index, identifier;
    uint16 half;
    FUNCTION_MARKER(0x80064A08, "SLUS_003.73");
    guest = GuestMemoryAddress(indices);
    if (guest)
        count = r_u16(guest);
    else
    {
        memcpy(&half, indices, 2);
        count = half;
    }
    for (index = 0; index < count; ++index)
    {
        if (guest)
            identifier = r_u16(guest + 2 + index * 2);
        else
        {
            memcpy(&half, (const uint8 *)indices + 2 + index * 2, 2);
            identifier = half;
        }
        SpawnRecord(identifier);
    }
    return 0;
}

extern "C" uint32 sub_80064A08(uint32 indices)
{
    FUNCTION_MARKER(0x80064A08, "SLUS_003.73");
    return SpawnEnemyIndices((const uint16 *)psx_addr(indices, 2));
}

void ClearObjectActivationFlags(uint32 object)
{
    uint32 next, flags;
    FUNCTION_MARKER(0x80063738, "SLUS_003.73");
    while (object)
    {
        next = r_u32(object + 28);
        flags = r_u16(object + 76);
        w_u16(object + 76, flags & 1);
        object = next;
    }
}

void RestoreNearbySuspendedObjects()
{
    uint32 object, next, flags, player, distance, limit;
    FUNCTION_MARKER(0x800638A4, "SLUS_003.73");
    object = r_u32(0x800FF5E0);
    while (object)
    {
        next = r_u32(object + 28);
        flags = r_u16(object + 78);
        if (flags & 2)
        {
            player = r_u32(0x800FF5A0);
            distance = MeasurePositionDistance((const uint32 *)psx_addr(player + 4, 12), (const uint32 *)psx_addr(object + 4, 12));
            limit = r_u32(0x800FF5EC);
            if (limit >= distance)
                VisualObject::Attach(object).VisualObject::RestoreList();
        }
        object = next;
    }
}

extern "C" void sub_80063738(uint32 object)
{
    FUNCTION_MARKER(0x80063738, "SLUS_003.73");
    ClearObjectActivationFlags(object);
}
extern "C" void sub_800638A4(void)
{
    FUNCTION_MARKER(0x800638A4, "SLUS_003.73");
    RestoreNearbySuspendedObjects();
}
