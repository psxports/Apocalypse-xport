#include <new>

#include "game_heap.h"
#include "game_settings.h"
#include "game_diagnostics.h"
#include "pickup_factory.h"
#include "pickup.h"
#include "game_object_allocation.h"

Pickup *SpawnPickup(uint32 resource, const uint32 *position, uint32 flags, uint32 lifetime, const uint32 *velocity)
{
    uint32 difficulty;
    GameHeapBlock *object;
    FUNCTION_MARKER(0x80020EF8, "SLUS_003.73");
    difficulty = game_settings.difficulty;
    resource &= 65535;
    if (difficulty == 2)
    {
        if (resource == 14)
            return 0;
        if (resource == 15)
            resource = 14;
        else if (resource == 16)
            resource = 15;
    }
    else if (!difficulty && resource == 14)
        resource = 15;
    object = AllocateZeroedObject(344);
    if (!object)
        return 0;
    return new (object) Pickup(resource, position, velocity, flags, lifetime);
}
