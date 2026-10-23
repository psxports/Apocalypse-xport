#include "effect_controller.h"
#include "effect_lifecycle.h"

EffectController::EffectController() : EffectObject()
{
    FUNCTION_MARKER(0x800330F4, "SLUS_003.73");
    AddToList(effect_lists.controllers);
}

uint32 EffectController::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80033138, "SLUS_003.73");
    RemoveFromList(effect_lists.controllers);
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}
