#include "renderable_effect.h"
#include "effect_lifecycle.h"

RenderableEffect::RenderableEffect() : EffectObject()
{
    FUNCTION_MARKER(0x8003304C, "SLUS_003.73");
    AddToList(effect_lists.renderables);
}

uint32 RenderableEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80033090, "SLUS_003.73");
    RemoveFromList(effect_lists.renderables);
    uint32 result = EffectObject::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

uint32 RenderableEffect::Render()
{
    FUNCTION_MARKER(0x8003A13C, "SLUS_003.73");
    // Original no-op has no defined return carrier
    return 0;
}

bool RenderableEffect::CaptureHdTick()
{
    return true;
}
