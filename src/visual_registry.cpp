#include "visual_object.h"
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
    FUNCTION_MARKER(0x80076B78, "SLUS_003.73");
    ModelInstance *instance = visual_lists.registry;
    identifier &= 65535;
    while (instance)
    {
        uint32 object = NativeObjectGuestAddress(instance->owner);
        if (r_u16(object + 296) == identifier)
        {
            instance->flags &= 65534;
            return;
        }
        instance = instance->next;
    }
}

void DisableVisualRegistryEntry(uint32 identifier)
{
    FUNCTION_MARKER(0x80076BC4, "SLUS_003.73");
    ModelInstance *instance = visual_lists.registry;
    identifier &= 65535;
    while (instance)
    {
        uint32 object = NativeObjectGuestAddress(instance->owner);
        if (r_u16(object + 296) == identifier)
        {
            instance->flags |= 1;
            return;
        }
        instance = instance->next;
    }
}

VisualRegistryEntry::VisualRegistryEntry(uint32 resource) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, state, identifier, model;
    FUNCTION_MARKER(0x80076C10, "SLUS_003.73");
    model = r_u32(0x800FF62C);
    w_u32(object + 68, 0x800A3D24);
    SetModelBank((const char *)psx_addr(model, 1));
    model = this->model.bank;
    this->model.model_idx = FindModelResourceIndex(resource, model);
    flags = this->model.flags;
    state = this->lifecycle_flags;
    this->model.flags = flags & 65533;
    this->lifecycle_flags = state & 65529;
    AddToList(visual_lists.registry);
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
    RemoveFromList(visual_lists.registry);
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
    AddAngleVector((uint16 *)this->model.angles, (const uint16 *)motion.angular_velocity);
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
