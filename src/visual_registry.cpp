extern "C"
{
#include "draft_first_signatures.h"
}
#include "visual_registry.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "model_resources.h"
#include "angle_math.h"
#include <new>

void EnableVisualRegistryEntry(uint32 identifier)
{
    uint32 object, flags;
    FUNCTION_MARKER(0x80076B78, "SLUS_003.73");
    object = r_u32(0x800FF8A0);
    identifier &= 65535;
    while (object)
    {
        if (r_u16(object + 296) == identifier)
        {
            flags = r_u16(object) & 65534;
            w_u16(object, flags);
            return;
        }
        object = r_u32(object + 28);
    }
    // TODO Original V0 on an empty list is unspecified; the current script caller discards it
}

void DisableVisualRegistryEntry(uint32 identifier)
{
    uint32 object = r_u32(0x800FF8A0), flags;
    FUNCTION_MARKER(0x80076BC4, "SLUS_003.73");
    identifier &= 65535;
    while (object)
    {
        if (r_u16(object + 296) == identifier)
        {
            flags = r_u16(object) | 1;
            w_u16(object, flags);
            return;
        }
        object = r_u32(object + 28);
    }
    // TODO Original V0 on an empty list is unspecified; the script caller discards it
}

VisualRegistryEntry::VisualRegistryEntry(uint32 resource) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, state, identifier, model;
    FUNCTION_MARKER(0x80076C10, "SLUS_003.73");
    model = r_u32(0x800FF62C);
    w_u32(object + 68, 0x800A3D24);
    SetModelBank((const char *)psx_addr(model, 1));
    model = r_u8(object + 27);
    w_u16(object + 22, FindModelResourceIndex(resource, model));
    flags = r_u16(object);
    state = r_u16(object + 78);
    w_u16(object, flags & 65533);
    w_u16(object + 78, state & 65529);
    AddToList((uint32 *)psx_addr(0x800FF8A0, 4));
    identifier = r_u16(0x800FF8A4);
    w_u16(object + 296, identifier);
    w_u16(0x800FF8A4, identifier + 1);
}

uint32 VisualRegistryEntry::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80076CA8, "SLUS_003.73");
    w_u32(object + 68, 0x800A3D24);
    RemoveFromList((uint32 *)psx_addr(0x800FF8A0, 4));
    w_u16(0x800FF8A4, r_u16(0x800FF8A4) - 1);
    result = VisualObject::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 VisualRegistryEntry::Update()
{
    FUNCTION_MARKER(0x80076D1C, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 132, 6));
    return object + 16;
}

VisualRegistryEntry &VisualRegistryEntry::Attach(uint32 object)
{
    return *static_cast<VisualRegistryEntry *>(FindNativeObjectStorage(object));
}

extern "C" void sub_80076B78(uint32 identifier)
{
    FUNCTION_MARKER(0x80076B78, "SLUS_003.73");
    EnableVisualRegistryEntry(identifier);
}
extern "C" void sub_80076BC4(uint32 identifier)
{
    FUNCTION_MARKER(0x80076BC4, "SLUS_003.73");
    DisableVisualRegistryEntry(identifier);
}
extern "C" uint32 sub_80076C10(uint32 object, uint32 resource)
{
    FUNCTION_MARKER(0x80076C10, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(VisualRegistryEntry));
    new (storage) VisualRegistryEntry(resource);
    return object;
}
extern "C" uint32 sub_80076CA8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80076CA8, "SLUS_003.73");
    return VisualRegistryEntry::Attach(object).VisualRegistryEntry::Destroy(reason);
}
extern "C" uint32 sub_80076D1C(uint32 object)
{
    FUNCTION_MARKER(0x80076D1C, "SLUS_003.73");
    return VisualRegistryEntry::Attach(object).VisualRegistryEntry::Update();
}
