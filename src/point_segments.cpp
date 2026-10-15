extern "C"
{
#include "draft_first_signatures.h"
}
#include "point_segments.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include <new>

PointSegments::PointSegments(uint32 count) : EffectObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 storage, index = 0, destination, x, y, z;
    FUNCTION_MARKER(0x80032068, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B50);
    storage = AllocateHeapBlock(count << 4, 0, 1);
    w_u32(object + 80, storage);
    while ((sint32)index < (sint32)count)
    {
        w_u32(r_u32(object + 80) + (index << 4) + 12, 0x40FFFFFF);
        destination = r_u32(object + 80) + (index << 4);
        ++index;
        x = r_u32(0x800A71CC);
        y = r_u32(0x800A71D0);
        z = r_u32(0x800A71D4);
        w_u32(destination, x);
        w_u32(destination + 4, y);
        w_u32(destination + 8, z);
    }
    w_u32(object + 76, count);
    AddToList((uint32 *)psx_addr(0x800FF45C, 4));
}

uint32 PointSegments::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 storage, result;
    FUNCTION_MARKER(0x80032128, "SLUS_003.73");
    storage = r_u32(object + 80);
    w_u32(object + 68, 0x800A1B50);
    ReleaseHeapBlock(storage);
    RemoveFromList((uint32 *)psx_addr(0x800FF45C, 4));
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 PointSegments::EnableSegments()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, destination, flags;
    FUNCTION_MARKER(0x80032198, "SLUS_003.73");
    while ((sint32)index < (sint32)r_u32(object + 76))
    {
        destination = r_u32(object + 80) + (index << 4);
        flags = r_u8(destination + 15);
        ++index;
        w_u8(destination + 15, flags | 2);
    }
    return 0;
}

PointSegments &PointSegments::Attach(uint32 object)
{
    return *static_cast<PointSegments *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80032068(uint32 object, uint32 count)
{
    FUNCTION_MARKER(0x80032068, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PointSegments));
    new (dram_addr) PointSegments(count);
    return object;
}

extern "C" uint32 sub_80032128(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80032128, "SLUS_003.73");
    return PointSegments::Attach(object).PointSegments::Destroy(reason);
}

extern "C" uint32 sub_80032198(uint32 object)
{
    FUNCTION_MARKER(0x80032198, "SLUS_003.73");
    return PointSegments::Attach(object).EnableSegments();
}
