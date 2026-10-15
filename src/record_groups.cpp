#include "effect_lifecycle.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "record_groups.h"
#include "record_data.h"
#include "model_resources.h"

uint32 SetRecordGroupEnabled(uint32 group, uint32 enabled)
{
    uint32 cursor, count, index, record, kind, payload, object, flags, identifier, table;
    FUNCTION_MARKER(0x80064B24, "SLUS_003.73");
    cursor = GetRecordPayload(group);
    count = r_u16(cursor);
    cursor += 2;
    for (index = 0; index < count; ++index)
    {
        identifier = r_u16(cursor);
        table = r_u32(0x800FF624);
        record = r_u32(table + identifier * 4);
        kind = (uint32)(sint32)(sint16)r_u16(record);
        cursor += 2;
        if (kind == 2 || kind == 9)
        {
            payload = record + 4 + ((uint32)(sint32)(sint16)r_u16(record + 2) << 1);
            if (payload & 2)
                payload += 2;
            object = FindModelObject(r_u32(payload));
            if (object)
            {
                flags = r_u16(object);
                w_u16(object, enabled ? flags & 65534 : flags | 1);
            }
        }
    }
    return 0;
}

void SuspendObjectsByRecord(uint32 list, uint32 record)
{
    uint32 object;
    FUNCTION_MARKER(0x80064F08, "SLUS_003.73");
    object = r_u32(list);
    while (object)
    {
        if (r_u16(object + 214) == record)
            sub_80062B7C(object, list);
        object = r_u32(object + 28);
    }
    // TODO Original V0 on an empty list is unspecified; current group callers discard it
}

void ResumeObjectsByRecord(uint32 object, uint32 record)
{
    FUNCTION_MARKER(0x80064F70, "SLUS_003.73");
    while (object)
    {
        if (r_u16(object + 214) == record)
            sub_80062BF0(object);
        object = r_u32(object + 28);
    }
    // TODO Original V0 on an empty list is unspecified; current group callers discard it
}

uint32 SetObjectRecordGroupMode(uint32 group, uint32 mode)
{
    uint32 cursor = group + 2, count, index, record, kind;
    FUNCTION_MARKER(0x80064FC8, "SLUS_003.73");
    count = r_u16(group);
    for (index = 0; index < count; ++index)
    {
        record = r_u16(cursor);
        kind = (uint32)(sint32)(sint16)r_u16(r_u32(r_u32(0x800FF624) + record * 4));
        if (kind == 1 || kind == 7)
        {
            if (mode == 5)
            {
                SuspendObjectsByRecord(0x800FF4E8, record);
                SuspendObjectsByRecord(0x800FF5DC, record);
            }
            else
            {
                ResumeObjectsByRecord(r_u32(0x800FF4E8), record);
                ResumeObjectsByRecord(r_u32(0x800FF5DC), record);
            }
        }
        cursor += 2;
    }
    return 0;
}

extern "C" uint32 sub_80064B24(uint32 group, uint32 enabled)
{
    FUNCTION_MARKER(0x80064B24, "SLUS_003.73");
    return SetRecordGroupEnabled(group, enabled);
}
extern "C" void sub_80064F08(uint32 list, uint32 record)
{
    FUNCTION_MARKER(0x80064F08, "SLUS_003.73");
    SuspendObjectsByRecord(list, record);
}
extern "C" void sub_80064F70(uint32 object, uint32 record)
{
    FUNCTION_MARKER(0x80064F70, "SLUS_003.73");
    ResumeObjectsByRecord(object, record);
}
extern "C" uint32 sub_80064FC8(uint32 group, uint32 mode)
{
    FUNCTION_MARKER(0x80064FC8, "SLUS_003.73");
    return SetObjectRecordGroupMode(group, mode);
}

void ApplyObjectListRecordAction(uint32 identifier, uint32 object, uint32 mode)
{
    uint32 table, target, receiver, damage;
    sint32 adjustment;
    FUNCTION_MARKER(0x80064A68, "SLUS_003.73");
    while (object)
    {
        if (r_u16(object + 214) == identifier)
        {
            if (!mode)
            {
                table = r_u32(object + 68);
                adjustment = (sint16)r_u16(table + 16);
                target = r_u32(table + 20);
                receiver = object + (uint32)adjustment;
                apocalypse_object_virtual20(target, receiver);
            }
            else if (mode == 1)
            {
                table = r_u32(object + 68);
                damage = (uint32)(sint32)(sint16)r_u16(object + 218);
                adjustment = (sint16)r_u16(table + 48);
                target = r_u32(table + 52);
                receiver = object + (uint32)adjustment;
                apocalypse_object_virtual52(target, receiver, damage, 0x800A71CC, 0);
            }
        }
        object = r_u32(object + 28);
    }
}

uint32 ApplyRecordGroupAction(uint32 group, uint32 mode)
{
    uint32 list, count, index, identifier, table, descriptor, pointer, object, head, flags;
    sint32 type;
    FUNCTION_MARKER(0x80064C1C, "SLUS_003.73");
    list = GetRecordPayload(group);
    count = r_u16(list);
    list += 2;
    for (index = 0; index < count; ++index)
    {
        identifier = r_u16(list);
        table = r_u32(0x800FF624);
        descriptor = r_u32(table + identifier * 4);
        type = (sint16)r_u16(descriptor);
        list += 2;
        if (type == 1)
        {
            head = r_u32(0x800FF4E8);
            ApplyObjectListRecordAction(identifier, head, mode);
            head = r_u32(0x800FF5DC);
            ApplyObjectListRecordAction(identifier, head, mode);
        }
        else if (type == 2 || type == 9)
        {
            pointer = descriptor + 2;
            pointer += ((uint32)(sint32)(sint16)r_u16(pointer) << 1) + 2;
            if (pointer & 2)
                pointer += 2;
            object = FindModelObject(r_u32(pointer));
            if (object)
            {
                if (mode == 1)
                    sub_8001E740(object, 0, 65535);
                else
                {
                    flags = r_u16(object);
                    w_u16(object, flags | 1);
                }
            }
        }
        else if (type == 10)
            DestroyEffectsByIdentifier(identifier);
        else if (type == 500 || type == 501)
        {
            object = r_u32(0x800FF434);
            while (object && (r_u8(object + 67) != 2 || r_u16(object + 10) != identifier))
                object = r_u32(object + 4);
            if (object)
                sub_80032ED8(object);
        }
    }
    return 0;
}

extern "C" void sub_80064A68(uint32 identifier, uint32 object, uint32 mode)
{
    FUNCTION_MARKER(0x80064A68, "SLUS_003.73");
    ApplyObjectListRecordAction(identifier, object, mode);
}
extern "C" uint32 sub_80064C1C(uint32 group, uint32 mode)
{
    FUNCTION_MARKER(0x80064C1C, "SLUS_003.73");
    return ApplyRecordGroupAction(group, mode);
}
