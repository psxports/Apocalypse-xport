extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "beam_trigger.h"
#include "effect_lifecycle.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "object_collision_events.h"
#include "record_factory.h"
#include "audio_session.h"
#include "game_random.h"
#include "point_segments.h"
#include "native_object_storage.h"
#include <new>

uint32 BeamTrigger::InitializeEffect()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 effect, storage, x, y, z, value;
    FUNCTION_MARKER(0x8001A478, "SLUS_003.73");
    effect = AllocateEffectStorage(96);
    if (effect)
    {
        void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(PointSegments));
        new (dram_addr) PointSegments(1);
    }
    w_u32(object + 320, effect);
    x = r_u32(object + 4);
    y = r_u32(object + 8);
    z = r_u32(object + 12);
    w_u32(effect + 84, x);
    w_u32(effect + 88, y);
    w_u32(effect + 92, z);
    w_u8(r_u32(object + 320) + 66, 1);
    PointSegments::Attach(r_u32(object + 320)).EnableSegments();
    effect = r_u32(object + 320);
    storage = r_u32(effect + 80);
    x = r_u32(object + 308);
    y = r_u32(object + 312);
    z = r_u32(object + 316);
    w_u32(storage, x);
    w_u32(storage + 4, y);
    w_u32(storage + 8, z);
    storage = r_u32(r_u32(object + 320) + 80);
    value = r_u8(object + 296);
    w_u8(storage + 12, value);
    storage = r_u32(r_u32(object + 320) + 80);
    value = r_u8(object + 297);
    w_u8(storage + 13, value);
    storage = r_u32(r_u32(object + 320) + 80);
    value = r_u8(object + 298);
    w_u8(storage + 14, value);
    return value;
}

BeamTrigger::BeamTrigger(const uint8 *, uint32 index) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, secondary_flags, cursor, reference, records, record, value;
    FUNCTION_MARKER(0x8001A2E8, "SLUS_003.73");
    w_u32(object + 68, 0x800A0700);
    flags = r_u16(object);
    secondary_flags = r_u16(object + 78);
    w_u16(object, flags | 1);
    w_u16(object + 78, secondary_flags & 0xFFEF);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    index &= 65535;
    w_u8(object + 299, 0);
    ReadRecordPosition((uint32 *)psx_addr(object + 4, 12), index);
    cursor = GetRecordPayload(index);
    reference = r_u16(cursor + 2);
    records = r_u32(0x800FF624);
    record = r_u32(records + reference * 4);
    value = r_u16(record);
    if ((sint16)value == 3)
    {
        value = r_u16(cursor + 4);
        w_u16(object + 306, value);
        reference = r_u16(cursor + 2);
    }
    else
    {
        w_u16(object + 306, reference);
        reference = r_u16(cursor + 4);
    }
    ReadRecordPosition((uint32 *)psx_addr(object + 308, 12), reference);
    w_u8(object + 296, 32);
    w_u8(object + 297, 0);
    w_u8(object + 298, 0);
    w_u16(object + 304, 0);
    w_u32(object + 300, 1);
    InitializeEffect();
}

uint32 BeamTrigger::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, receiver, target;
    FUNCTION_MARKER(0x8001A3E8, "SLUS_003.73");
    w_u32(object + 68, 0x800A0700);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    child = r_u32(object + 320);
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

uint32 BeamTrigger::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 effect, player, storage, active, value, flags, result, timer, random, table, receiver, target, axis;
    FUNCTION_MARKER(0x8001A550, "SLUS_003.73");
    effect = r_u32(object + 320);
    player = r_u32(0x800FF5A0);
    storage = r_u32(effect + 80);
    active = 0;
    if (player && TraceSegmentCollision((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 308, 12), player))
    {
        value = r_u8(object + 299);
        active = 1;
        if (!value)
        {
            PlaySoundEffect(41, 8192, 0);
            SpawnRecord(r_u16(object + 306));
            w_u8(object + 299, 1);
        }
        for (axis = 0; axis < 3; ++axis)
        {
            value = r_u8(object + 296 + axis);
            w_u8(storage + 12 + axis, value ? 255 : 0);
        }
        w_u16(object + 304, 0);
    }
    flags = r_u16(object + 76);
    result = flags & 0xFFFE;
    if (flags & 1)
    {
        w_u16(object + 76, result);
        result = 1;
        w_u8(object + 299, 1);
    }
    if (active)
        return result;
    if (r_u8(object + 299))
    {
        timer = r_u16(object + 304) + 1;
        w_u16(object + 304, timer);
        if ((timer & 65535) >= 61)
        {
            table = r_u32(object + 68);
            receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
            target = r_u32(table + 20);
            return xport_draft_guest_call1(target, receiver);
        }
        if (!RandomRange(2))
        {
            for (axis = 0; axis < 3; ++axis)
            {
                result = r_u8(object + 296 + axis) >> 2;
                w_u8(storage + 12 + axis, result);
            }
            return result;
        }
    }
    for (axis = 0; axis < 3; ++axis)
    {
        value = r_u8(object + 296 + axis);
        random = RandomRange(value >> 1);
        value = r_u8(object + 296 + axis);
        result = random - (value >> 2);
        w_u8(storage + 12 + axis, value + result);
    }
    return result;
}

extern "C" uint32 sub_8001A2E8(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8001A2E8, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(BeamTrigger));
    // Original data input is unused; do not dereference or validate it
    new (storage) BeamTrigger(0, index);
    return object;
}

extern "C" uint32 sub_8001A3E8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001A3E8, "SLUS_003.73");
    return BeamTrigger::Attach(object).BeamTrigger::Destroy(reason);
}

extern "C" uint32 sub_8001A478(uint32 object)
{
    FUNCTION_MARKER(0x8001A478, "SLUS_003.73");
    return BeamTrigger::Attach(object).InitializeEffect();
}

extern "C" uint32 sub_8001A550(uint32 object)
{
    FUNCTION_MARKER(0x8001A550, "SLUS_003.73");
    return BeamTrigger::Attach(object).BeamTrigger::Update();
}

BeamTrigger &BeamTrigger::Attach(uint32 object)
{
    return *static_cast<BeamTrigger *>(FindNativeObjectStorage(object));
}
