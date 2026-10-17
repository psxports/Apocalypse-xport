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
    uint32 result = 0x800A0000;
    FUNCTION_MARKER(0x8005C338, "SLUS_003.73");
    ModelInstance *instance = visual_lists.actors;
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        uint32 kind = object.kind;
        ModelInstance *next = instance->next;
        uint32 relative = kind - 200;
        if (relative < 11)
        {
            switch (kind)
            {
                case 200: case 201: case 202: case 204: case 205:
                case 207: case 208: case 209: case 210:
                    result = object.Finish();
                    break;
                default:
                    result = 0x8005C3A4;
                    break;
            }
        }
        else
            result = relative << 2;
        instance = next;
    }
    return result;
}



void ActivateObjectsByIndex(ModelInstance *instance, uint32 index)
{
    FUNCTION_MARKER(0x80064E10, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.record_idx == index)
            object.activation_flags |= 1;
        instance = instance->next;
    }
}



uint32 ActivateEnemyIndices(const uint16 *indices)
{
    uint32 guest, count, index, identifier, table, record, kind;
    ModelInstance *head;
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
            head = visual_lists.actors;
            ActivateObjectsByIndex(head, identifier);
            head = visual_lists.solids;
            ActivateObjectsByIndex(head, identifier);
        }
    }
    return 0;
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

void ClearObjectActivationFlags(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80063738, "SLUS_003.73");
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        object.activation_flags &= 1;
        instance = next;
    }
}

void RestoreNearbySuspendedObjects()
{
    FUNCTION_MARKER(0x800638A4, "SLUS_003.73");
    ModelInstance *instance = visual_lists.suspended;
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        uint32 flags = object.lifecycle_flags;
        if (flags & 2)
        {
            uint32 distance = MeasurePositionDistance(visual_lists.players->pos, instance->pos);
            if (r_u32(0x800FF5EC) >= distance)
                object.RestoreList();
        }
        instance = next;
    }
}



