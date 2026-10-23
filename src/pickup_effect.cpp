#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "pickup_effect.h"
#include "ribbon_segments.h"
#include "beam_segments.h"
#include "ring_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "global.h"

uint32 PickupEffect::SetPos(const Vector3 *)
{
    FUNCTION_MARKER(0x8003200C, "SLUS_003.73");
    return 0;
}

uint32 PickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80032014, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}
