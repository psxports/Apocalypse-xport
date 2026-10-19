#include <new>

#include "record_scripts.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "visual_registry.h"
#include "game_object_allocation.h"
#include "model_resources.h"
#include "angle_math.h"

static uint16 registry_count;

void EnableVisualRegistryEntry(uint32 identifier)
{
    FUNCTION_MARKER(0x80076B78, "SLUS_003.73");
    ModelInstance *instance = visual_lists.registry;
    identifier &= 65535;
    while (instance)
    {
        VisualRegistryEntry *entry = static_cast<VisualRegistryEntry *>(instance->owner);
        if (entry->identifier == identifier)
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
        VisualRegistryEntry *entry = static_cast<VisualRegistryEntry *>(instance->owner);
        if (entry->identifier == identifier)
        {
            instance->flags |= 1;
            return;
        }
        instance = instance->next;
    }
}

VisualRegistryEntry::VisualRegistryEntry(uint32 resource) : VisualObject()
{
    uint32 flags, state, model;
    FUNCTION_MARKER(0x80076C10, "SLUS_003.73");
    SetModelBank(GetRecordModelBankName());
    model = this->model.bank;
    this->model.model_idx = FindModelResourceIndex(resource, model);
    flags = this->model.flags;
    state = this->lifecycle_flags;
    this->model.flags = flags & 65533;
    this->lifecycle_flags = state & 65529;
    AddToList(visual_lists.registry);
    identifier = registry_count;
    ++registry_count;
}

uint32 VisualRegistryEntry::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80076CA8, "SLUS_003.73");
    RemoveFromList(visual_lists.registry);
    --registry_count;
    result = VisualObject::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(this);
    return result;
}

uint32 VisualRegistryEntry::Update()
{
    FUNCTION_MARKER(0x80076D1C, "SLUS_003.73");
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    return 0;
}
