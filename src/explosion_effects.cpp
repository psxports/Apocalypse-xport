extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "explosion_effects.h"
#include "game_random.h"
#include "screen_flash.h"
#include "camera.h"
#include "effect_sprite.h"
#include "exp_act_burst.h"
#include "ring_burst_effect.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

// Native adapter preserves the known queue result across empty lists
static uint32 MarkExplosionList(uint32 object, uint32 result)
{
    FUNCTION_MARKER(0x80067338, "SLUS_003.73");
    while (object)
    {
        result = r_u16(object + 78) & 64;
        if (!result)
        {
            result = r_u16(object + 76) | 4;
            w_u16(object + 76, result);
        }
        object = r_u32(object + 28);
    }
    return result;
}

uint32 MarkExplosionObjects(uint32 object)
{
    // TODO Empty standalone calls have no defined original return input
    return MarkExplosionList(object, 0);
}

void ResetExplosionQueue()
{
    FUNCTION_MARKER(0x8006737C, "SLUS_003.73");
    w_u32(0x800FF648, 0);
}

uint32 QueueExplosion(const uint32 *position, uint32 mode, uint32 radius, uint32 strength, uint32 priority)
{
    uint32 count, append, index, destination, words[3], result, address, axis;
    FUNCTION_MARKER(0x80067388, "SLUS_003.73");
    count = r_u32(0x800FF648);
    append = (uint32)((sint32)count < 3);
    if (append)
    {
        index = count;
        result = 0x800A7294;
    }
    else
    {
        for (index = 0; index < 3; ++index)
            if ((sint32)r_u32(0x800A7294 + index * 4) < (sint32)priority)
                break;
        if (index == 3)
            return 0;
        result = 0x800A7288 + index * 4;
    }
    address = GuestMemoryAddress(position);
    for (axis = 0; axis < 3; ++axis)
    {
        if (address)
            words[axis] = r_u32(address + axis * 4);
        else
            memcpy(&words[axis], (const uint8 *)position + axis * 4, 4);
    }
    destination = 0x800A724C + index * 12;
    w_u32(destination, words[0]);
    w_u32(destination + 4, words[1]);
    w_u32(destination + 8, words[2]);
    if (append)
    {
        count = r_u32(0x800FF648);
        index = count;
    }
    w_u32(0x800A7270 + index * 4, mode);
    w_u32(0x800A727C + index * 4, radius);
    w_u32(0x800A7288 + index * 4, strength);
    w_u32(0x800A7294 + index * 4, priority);
    if (append)
        w_u32(0x800FF648, count + 1);
    result = MarkExplosionList(r_u32(0x800FF4E8), result);
    return MarkExplosionList(r_u32(0x800FF5DC), result);
}

extern "C" uint32 sub_80067338(uint32 object)
{
    FUNCTION_MARKER(0x80067338, "SLUS_003.73");
    return MarkExplosionObjects(object);
}

extern "C" void sub_8006737C(void)
{
    FUNCTION_MARKER(0x8006737C, "SLUS_003.73");
    ResetExplosionQueue();
}

extern "C" uint32 sub_80067388(uint32 position, uint32 mode, uint32 radius, uint32 strength, uint32 priority)
{
    FUNCTION_MARKER(0x80067388, "SLUS_003.73");
    return QueueExplosion((const uint32 *)psx_addr(position, 12), mode, radius, strength, priority);
}

extern "C" uint32 xport_draft_host_sub_80067388_p1(const void *position, uint32 mode, uint32 radius, uint32 strength, uint32 priority)
{
    FUNCTION_MARKER(0x80067388, "SLUS_003.73");
    return QueueExplosion((const uint32 *)position, mode, radius, strength, priority);
}

uint32 SpawnExplosionParticles(const uint32 *position, uint32 count, uint32 resource, uint32 size, uint32 spread)
{
    uint32 result, index = 0, child;
    FUNCTION_MARKER(0x8001E4C8, "SLUS_003.73");
    result = r_u32(0x800FF738);
    if (result)
        return result;
    size &= 65535;
    while ((sint32)index < (sint32)count)
    {
        child = sub_80032DC0(112);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ParticleSprite));
            new (dram_addr) ParticleSprite(position, 128, 128, 128, resource, size, 1, spread, 0x3000, 30);
        }
        ++index;
        result = (uint32)((sint32)index < (sint32)count);
    }
    return result;
}

uint32 HandleExplosionProximity(const uint32 *position, uint32 kind)
{
    uint32 result, listener[3], child, sound;
    sint32 distance;
    FUNCTION_MARKER(0x8001E1B8, "SLUS_003.73");
    result = r_u32(0x800FF738);
    if (result)
        return result;
    listener[0] = r_u32(0x800ED520) << 12;
    listener[1] = r_u32(0x800ED524) << 12;
    listener[2] = r_u32(0x800ED528) << 12;
    distance = (sint32)apocalypse_position_distance_native(position, listener);
    if (kind != 0 && kind != 1)
        return 1;
    sound = (r_u32(0x800FF2F0) & 1) ? 1 : 2;
    xport_draft_host_sub_80069EF4_p2(sound, position, 0);
    child = sub_80032DC0(160);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExpActBurst));
        if (kind)
            new (dram_addr) ExpActBurst(position, 20, 200, 150, 10, 5000, 10, 0, 0, 1, 0, 7, 250, 5, 20, 65);
        else
            new (dram_addr) ExpActBurst(position, 10, 100, 70, 10, 3000, 10, 0, 0, 1, 0, 10, 250, 14, 10, 128);
    }
    if (kind && distance < 6000)
        SetScreenFlash(70, 30, 0, 20, 0, 0);
    result = (uint32)(distance < 2000);
    if (distance < 4000)
    {
        uint32 camera = r_u32(0x800FF904);
        uint32 mode = kind ? (distance < 2000 ? 0 : 1) : (distance < 2000 ? 1 : 2);
        return Camera::Attach(camera).ApplyExplosionShake(mode);
    }
    return result;
}

uint32 SpawnExplosion(const uint32 *position, uint32 large)
{
    uint32 result, count;
    if (large)
    {
        FUNCTION_MARKER(0x8001E598, "SLUS_003.73");
    }
    else
    {
        FUNCTION_MARKER(0x8001E650, "SLUS_003.73");
    }
    if (r_u32(0x800FF378) != 9)
        QueueExplosion(position, 1, 512, 32, 256);
    result = r_u32(0x800FF738);
    if (result)
        return result;
    count = RandomRange(20);
    SpawnExplosionParticles(position, count, 10, 750, 80);
    HandleExplosionProximity(position, large ? 1 : 0);
    return SpawnExplosionRing(position, large ? 200 : 70, 240, 200, 0, 5, 0, 100);
}

extern "C" uint32 sub_8001E4C8(uint32 position, uint32 count, uint32 resource, uint32 size, uint32 spread)
{
    FUNCTION_MARKER(0x8001E4C8, "SLUS_003.73");
    return SpawnExplosionParticles((const uint32 *)psx_addr(position, 12), count, resource, size, spread);
}

extern "C" uint32 xport_draft_host_sub_8001E4C8_p1(const void *position, uint32 count, uint32 resource, uint32 size, uint32 spread)
{
    FUNCTION_MARKER(0x8001E4C8, "SLUS_003.73");
    return SpawnExplosionParticles((const uint32 *)position, count, resource, size, spread);
}

extern "C" uint32 sub_8001E1B8(uint32 position, uint32 kind)
{
    FUNCTION_MARKER(0x8001E1B8, "SLUS_003.73");
    return HandleExplosionProximity((const uint32 *)psx_addr(position, 12), kind);
}

extern "C" uint32 apocalypse_explosion_proximity_native(const void *position, uint32 kind)
{
    return HandleExplosionProximity((const uint32 *)position, kind);
}

extern "C" uint32 sub_8001E650(uint32 position)
{
    FUNCTION_MARKER(0x8001E650, "SLUS_003.73");
    return SpawnExplosion((const uint32 *)psx_addr(position, 12), 0);
}

extern "C" uint32 sub_8001E598(uint32 position)
{
    FUNCTION_MARKER(0x8001E598, "SLUS_003.73");
    return SpawnExplosion((const uint32 *)psx_addr(position, 12), 1);
}

extern "C" uint32 apocalypse_explosion_native(const void *position, uint32 large)
{
    return SpawnExplosion((const uint32 *)position, large ? 1 : 0);
}
