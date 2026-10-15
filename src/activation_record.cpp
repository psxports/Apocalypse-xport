extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "activation_record.h"
#include "game_heap.h"
#include "record_data.h"
#include "record_factory.h"
#include "vector_distance.h"
#include "native_object_storage.h"
#include <new>

ActivationRecord::ActivationRecord(uint32 index, uint32 state) : ActivationRecordBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 head;
    FUNCTION_MARKER(0x8006FBB0, "SLUS_003.73");
    head = r_u32(0x800FF7DC);
    w_u16(object + 4, index);
    w_u32(object, 0x800A3ACC);
    w_u32(0x800FF7DC, object);
    w_u16(object + 6, state);
    w_u32(object + 20, head);
    ReadRecordPosition((uint32 *)psx_addr(object + 8, 12), index & 65535);
}

ActivationRecord &ActivationRecord::Attach(uint32 object)
{
    return *static_cast<ActivationRecord *>(FindNativeObjectStorage(object));
}

uint32 ActivationRecord::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 previous = 0, current, next;
    FUNCTION_MARKER(0x8006FC00, "SLUS_003.73");
    current = r_u32(0x800FF7DC);
    w_u32(object, 0x800A3ACC);
    // TODO Original assumes the receiver belongs to this list
    while (current != object)
    {
        previous = current;
        current = r_u32(previous + 20);
    }
    next = r_u32(object + 20);
    if (previous)
        w_u32(previous + 20, next);
    else
        w_u32(0x800FF7DC, next);
    w_u32(object, 0x800A3ADC);
    if (reason & 1)
        return ReleaseHeapBlock(object);
    return 0;
}

uint32 ClearActivationRecords()
{
    uint32 object, table, receiver, target;
    FUNCTION_MARKER(0x8006FB04, "SLUS_003.73");
    object = r_u32(0x800FF7DC);
    while (object)
    {
        object = r_u32(0x800FF7DC);
        table = r_u32(object);
        receiver = object + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
        object = r_u32(0x800FF7DC);
    }
    return object;
}

extern "C" uint32 sub_8006FBB0(uint32 object, uint32 index, uint32 state)
{
    FUNCTION_MARKER(0x8006FBB0, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ActivationRecord));
    new (dram_addr) ActivationRecord(index, state);
    return object;
}

extern "C" uint32 sub_8006FC00(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8006FC00, "SLUS_003.73");
    return ActivationRecord::Attach(object).ActivationRecord::Destroy(reason);
}
extern "C" uint32 sub_8006FB04(void)
{
    FUNCTION_MARKER(0x8006FB04, "SLUS_003.73");
    return ClearActivationRecords();
}

void DeleteActivationRecords()
{
    uint32 node, table, adjustment, target, receiver, next;
    FUNCTION_MARKER(0x8006FB60, "SLUS_003.73");
    node = r_u32(0x800FF7DC);
    while (node)
    {
        table = r_u32(node);
        adjustment = (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        receiver = node + adjustment;
        next = r_u32(node + 20);
        apocalypse_object_cleanup(target, receiver, 3);
        node = next;
    }
}

// TODO Establish the original owning class before making this an instance method
uint32 DeactivateDistantObject(uint32 object, uint32 distance)
{
    uint32 allocation, index, state, table, adjustment, target;
    FUNCTION_MARKER(0x8006FC84, "SLUS_003.73");
    if (!(r_u16(object + 78) & 4))
        return 0;
    if ((sint16)r_u16(object + 218) < 0)
        return 0;
    if (distance < 9000)
        return 0;
    allocation = AllocateZeroedHeapBlock(24);
    if (allocation)
    {
        index = r_u16(object + 214);
        state = (uint32)(sint32)(sint16)r_u16(object + 218);
        void *dram_addr = AllocateNativeObjectStorage(allocation, sizeof(ActivationRecord));
        new (dram_addr) ActivationRecord(index, state);
    }
    table = r_u32(object + 68);
    adjustment = (uint32)(sint32)(sint16)r_u16(table + 16);
    target = r_u32(table + 20);
    apocalypse_object_virtual20(target, object + adjustment);
    return 1;
}

extern "C" void sub_8006FB60(void)
{
    FUNCTION_MARKER(0x8006FB60, "SLUS_003.73");
    DeleteActivationRecords();
}
extern "C" uint32 sub_8006FC84(uint32 object, uint32 distance)
{
    FUNCTION_MARKER(0x8006FC84, "SLUS_003.73");
    return DeactivateDistantObject(object, distance);
}

void RestoreNearbyObjects()
{
    uint32 node, player, next, object, state, saved_state, flags;
    uint32 table, adjustment, target, receiver;
    FUNCTION_MARKER(0x8006FD18, "SLUS_003.73");
    node = r_u32(0x800FF7DC);
    while (node)
    {
        player = r_u32(0x800FF5A0);
        next = r_u32(node + 20);
        if (MeasurePositionDistance((const uint32 *)psx_addr(player + 4, 12), (const uint32 *)psx_addr(node + 8, 12)) < 9000)
        {
            object = CreateObjectFromRecord(r_u16(node + 4));
            if (object)
            {
                state = r_u16(node + 6);
                saved_state = r_u16(node + 6);
                if ((sint16)state >= 0)
                    w_u16(object + 218, saved_state);
                flags = r_u16(object + 78);
                w_u16(object + 78, flags | 4);
                table = r_u32(node);
                adjustment = (uint32)(sint32)(sint16)r_u16(table + 8);
                target = r_u32(table + 12);
                receiver = node + adjustment;
                apocalypse_object_cleanup(target, receiver, 3);
            }
        }
        node = next;
    }
}

extern "C" void sub_8006FD18(void)
{
    FUNCTION_MARKER(0x8006FD18, "SLUS_003.73");
    RestoreNearbyObjects();
}

ActivationRecordBase &ActivationRecordBase::Attach(uint32 object)
{
    return *static_cast<ActivationRecordBase *>(FindNativeObjectStorage(object));
}

uint32 ActivationRecordBase::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8006FDC8, "SLUS_003.73");
    w_u32(object, 0x800A3ADC);
    if (reason & 1)
        return ReleaseHeapBlock(object);
    return 0x800A3ADC;
}

extern "C" uint32 sub_8006FDC8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8006FDC8, "SLUS_003.73");
    return ActivationRecordBase::Attach(object).ActivationRecordBase::Destroy(reason);
}
