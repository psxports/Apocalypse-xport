#include "line_effect.h"
#include "effect_lifecycle.h"
#include "object_initializers.h"
#include "hd.h"

LineEffect::LineEffect() : EffectObject(), hd_history(0)
{
    FUNCTION_MARKER(0x80032824, "SLUS_003.73");
    color[0] = 0x50808080;
    color[1] = 0x55808080;
    endpoint[0] = zero_vector;
    endpoint[1] = zero_vector;
    AddToList(effect_lists.lines);
}

LineEffect::~LineEffect()
{
    hd_line_free(hd_history);
}

uint32 LineEffect::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80032880, "SLUS_003.73");
    RemoveFromList(effect_lists.lines);
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}
