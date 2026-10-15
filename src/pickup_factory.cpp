extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "pickup_factory.h"
#include "pickup.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include <new>

uint32 SpawnPickup(uint32 resource, const uint32 *position, uint32 flags, uint32 yaw, const uint32 *velocity)
{
    uint32 difficulty, object;
    FUNCTION_MARKER(0x80020EF8, "SLUS_003.73");
    difficulty = r_u32(0x800FF384);
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
        return object;
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Pickup));
    new (dram_addr) Pickup(resource, position, velocity, flags, yaw);
    return object;
}

extern "C" uint32 sub_80020EF8(uint32 resource, uint32 position, uint32 flags, uint32 yaw, uint32 velocity)
{
    FUNCTION_MARKER(0x80020EF8, "SLUS_003.73");
    return SpawnPickup(resource, (const uint32 *)psx_addr(position, 12), flags, yaw, flags & 8 ? 0 : (const uint32 *)psx_addr(velocity, 12));
}

extern "C" uint32 apocalypse_pickup_create_native(uint32 resource, const void *position, uint32 flags, uint32 yaw, uint32 velocity)
{
    return SpawnPickup(resource, (const uint32 *)position, flags, yaw, flags & 8 ? 0 : (const uint32 *)psx_addr(velocity, 12));
}
