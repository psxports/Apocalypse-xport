#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "pickup_effect.h"
#include "pulse_laser_pickup_effect.h"
#include "ribbon_segments.h"
#include "beam_segments.h"
#include "ring_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "global.h"

PulseLaserPickupEffect::PulseLaserPickupEffect() : PickupEffect(), ring()
{
    FUNCTION_MARKER(0x80031E10, "SLUS_003.73");
}

uint32 PulseLaserPickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80031E44, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    PickupEffect::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 PulseLaserPickupEffect::SetPos(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80031ECC, "SLUS_003.73");
    if (ring)
        ring->SetPos(pos);
    else
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(104);
        if (!allocation)
        {
            GameDiagnostic(("Pulse laser pickup ring allocation failed\n"));
            abort();
        }
        ring = new (allocation) RingEffect(4, 2);
        ring->SetCenterColor(255, 255, 255);
        ring->SetRingColors(0, 130, 0);
        ring->SetRowColors(0, 0, 40, 0);
        ring->SetRowColors(1, 0, 0, 0);
        ring->SetOddRadii(21);
        ring->SetEvenRadii(12);
        ring->SetRowExtent(0, 8);
        ring->SetRowExtent(1, 8);
        ring->SetPos(pos);
        ring->angle = RandomRange(1024);
        ring->persistent = true;
    }
    uint32 angle = ring->angle + 150;
    ring->angle = angle;
    return angle;
}

PulseLaserPickupEffect::~PulseLaserPickupEffect()
{
    if (ring)
        ring->Destroy(3);
}
