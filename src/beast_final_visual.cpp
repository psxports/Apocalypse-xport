#include "resource_script.h"
#include "visual_object.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "beast_final_visual.h"
#include "game_object_allocation.h"

// Native constructor extraction of the inline42360 block43918..43984
BeastFinalVisual::BeastFinalVisual(const Vector3 *pos) : VisualObject()
{
    uint8 bank;
    uint16 model_idx, flags;
    AddToList(visual_lists.actors);
    SetModelBank(GetRecModelBankName());
    bank = GetRecModelBankSlot();
    model_idx = ModelBank::FindResourceIdx(0x4E5B6ED6, bank);
    flags = model.flags;
    model.model_idx = model_idx;
    model.flags = flags & 65533;
    model.pos = *pos;
}

uint32 BeastFinalVisual::Update()
{
    FUNCTION_MARKER(0x800449EC, "SLUS_003.73");
    return 0;
}

uint32 BeastFinalVisual::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x800449F4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}
