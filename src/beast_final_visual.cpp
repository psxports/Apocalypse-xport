#include <string.h>

#include "record_scripts.h"
#include "visual_object.h"
#include "model_resources.h"
#include "game_diagnostics.h"
#include "beast_final_visual.h"
#include "game_object_allocation.h"

// Native constructor extraction of the inline42360 block43918..43984
BeastFinalVisual::BeastFinalVisual(const uint32 *position) : VisualObject()
{
    uint32 value, flags;
    Vector3 coordinates;
    AddToList(visual_lists.actors);
    SetModelBank(GetRecordModelBankName());
    value = GetRecordModelBankSlot();
    value = ::FindModelResourceIndex(0x4E5B6ED6, value);
    flags = this->model.flags;
    this->model.model_idx = value;
    this->model.flags = flags & 65533;
    memcpy(coordinates.words, position, sizeof(coordinates.words));
    this->model.pos = coordinates;
}

uint32 BeastFinalVisual::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x800449F4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}

uint32 BeastFinalVisual::Update()
{
    FUNCTION_MARKER(0x800449EC, "SLUS_003.73");
    return 0;
}
