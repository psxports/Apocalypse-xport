extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "effect_segments.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

// Native input bridge for the original word-pointer loads
static uint32 ReadSegmentEndpointWord(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

SegmentEffect::SegmentEffect(uint32 count) : EffectObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 storage, index, destination, x, y, z;
    FUNCTION_MARKER(0x800322F4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B38);
    storage = AllocateHeapBlock(count * 16, 0, 1);
    w_u32(object + 76, storage);
    for (index = 0; (sint32)index < (sint32)count; ++index)
    {
        w_u32(r_u32(object + 76) + index * 16 + 12, 0x50FFFFFF);
        destination = r_u32(object + 76) + index * 16;
        x = r_u32(0x800A71CC);
        y = r_u32(0x800A71D0);
        z = r_u32(0x800A71D4);
        w_u32(destination, x);
        w_u32(destination + 4, y);
        w_u32(destination + 8, z);
    }
    w_u32(object + 72, count);
    AddToList((uint32 *)psx_addr(0x800FF460, 4));
    w_u8(object + 92, 255);
    w_u8(object + 93, 255);
    w_u8(object + 94, 255);
    w_u8(object + 95, 80);
}

uint32 SegmentEffect::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 storage, result;
    FUNCTION_MARKER(0x800323CC, "SLUS_003.73");
    storage = r_u32(object + 76);
    w_u32(object + 68, 0x800A1B38);
    ReleaseHeapBlock(storage);
    RemoveFromList((uint32 *)psx_addr(0x800FF460, 4));
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 SegmentEffect::Enable()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index, destination, result;
    FUNCTION_MARKER(0x8003243C, "SLUS_003.73");
    for (index = 0; (sint32)index < (sint32)r_u32(object + 72); ++index)
    {
        destination = r_u32(object + 76) + index * 16 + 15;
        result = r_u8(destination) | 2;
        w_u8(destination, result);
    }
    result = r_u8(object + 95) | 2;
    w_u8(object + 95, result);
    return result;
}

uint32 SegmentEffect::SetEndpoints(const uint32 *origin, const uint32 *target)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], difference[3], step[3];
    uint32 x, y, z, result, index, destination;
    FUNCTION_MARKER(0x800324D4, "SLUS_003.73");
    position[0] = ReadSegmentEndpointWord(origin, 0);
    position[1] = ReadSegmentEndpointWord(origin, 1);
    position[2] = ReadSegmentEndpointWord(origin, 2);
    SubtractVectors(difference, target, origin);
    DivideVector(step, difference, (const uint32 *)psx_addr(object + 72, 4));
    x = ReadSegmentEndpointWord(origin, 0);
    y = ReadSegmentEndpointWord(origin, 1);
    z = ReadSegmentEndpointWord(origin, 2);
    w_u32(object + 80, x);
    w_u32(object + 84, y);
    w_u32(object + 88, z);
    result = r_u32(object + 72);
    index = 0;
    if ((sint32)result > 0)
    {
        do
        {
            AddVectors(position, step);
            destination = r_u32(object + 76) + index * 16;
            w_u32(destination, position[0]);
            w_u32(destination + 4, position[1]);
            w_u32(destination + 8, position[2]);
            result = r_u32(object + 72);
            ++index;
            result = (sint32)index < (sint32)result;
        } while (result);
    }
    return result;
}

extern "C" uint32 sub_800322F4(uint32 object, uint32 count)
{
    FUNCTION_MARKER(0x800322F4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(SegmentEffect));
    new (dram_addr) SegmentEffect(count);
    return object;
}

extern "C" uint32 sub_800323CC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800323CC, "SLUS_003.73");
    return SegmentEffect::Attach(object).SegmentEffect::Destroy(reason);
}

extern "C" uint32 sub_8003243C(uint32 object)
{
    FUNCTION_MARKER(0x8003243C, "SLUS_003.73");
    return SegmentEffect::Attach(object).SegmentEffect::Enable();
}

extern "C" uint32 sub_800324D4(uint32 object, uint32 origin, uint32 target)
{
    FUNCTION_MARKER(0x800324D4, "SLUS_003.73");
    return SegmentEffect::Attach(object).SegmentEffect::SetEndpoints((const uint32 *)psx_addr(origin, 12), (const uint32 *)psx_addr(target, 12));
}

SegmentEffect &SegmentEffect::Attach(uint32 object)
{
    return *static_cast<SegmentEffect *>(FindNativeObjectStorage(object));
}
