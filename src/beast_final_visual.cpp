extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "beast_final_visual.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include <string.h>

// Native constructor extraction of the inline42360 block43918..43984
BeastFinalVisual::BeastFinalVisual(const uint32 *position) : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 value, flags, coordinates[3], address;
    w_u32(object + 68, 0x800A2080);
    sub_80062A38(object, 0x800FF4E8);
    value = r_u32(0x800FF62C);
    sub_800626C8(object, value);
    value = r_u8(0x800FF644);
    value = sub_8006E080(0x4E5B6ED6, value);
    flags = r_u16(object);
    w_u16(object + 22, value);
    w_u16(object, flags & 65533);
    address = GuestMemoryAddress(position);
    if (address)
    {
        coordinates[0] = r_u32(address);
        coordinates[1] = r_u32(address + 4);
        coordinates[2] = r_u32(address + 8);
    }
    else
        memcpy(coordinates, position, 12);
    w_u32(object + 4, coordinates[0]);
    w_u32(object + 8, coordinates[1]);
    w_u32(object + 12, coordinates[2]);
}

uint32 BeastFinalVisual::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x800449F4, "SLUS_003.73");
    w_u32(object + 68, 0x800A2080);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

void BeastFinalVisual::Update()
{
    FUNCTION_MARKER(0x800449EC, "SLUS_003.73");
}

BeastFinalVisual &BeastFinalVisual::Attach(uint32 object)
{
    return *static_cast<BeastFinalVisual *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_800449F4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800449F4, "SLUS_003.73");
    return BeastFinalVisual::Attach(object).BeastFinalVisual::Destroy(reason);
}

extern "C" void sub_800449EC(uint32 object)
{
    FUNCTION_MARKER(0x800449EC, "SLUS_003.73");
    BeastFinalVisual::Attach(object).BeastFinalVisual::Update();
}
