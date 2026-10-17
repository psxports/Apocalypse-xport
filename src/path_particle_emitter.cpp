#include "record_data.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "path_particle_emitter.h"
#include "effect_sprite.h"
#include "expanding_ring.h"
#include "native_object_storage.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include <new>

PathParticleEmitter::PathParticleEmitter(uint32 identifier) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 record, first, second, table, slot, item, kind, payload = 0, value;
    FUNCTION_MARKER(0x8001F8AC, "SLUS_003.73");
    w_u32(object + 68, 0x800A10F4);
    record = GetRecordPayload(identifier);
    first = r_u16(record + 2);
    table = r_u32(0x800FF624);
    item = r_u32(table + first * 4);
    kind = (uint32)(sint32)(sint16)r_u16(item);
    second = r_u16(record + 4);
    if (kind == 10)
    {
        slot = table + second * 4;
        item = r_u32(slot);
        kind = (uint32)(sint32)(sint16)r_u16(item);
        if (kind == 11)
        {
            w_u32(object + 72, first);
            payload = r_u32(slot);
        }
    }
    table = r_u32(0x800FF624);
    slot = table + first * 4;
    item = r_u32(slot);
    kind = (uint32)(sint32)(sint16)r_u16(item);
    if (kind == 11)
    {
        item = r_u32(table + second * 4);
        kind = (uint32)(sint32)(sint16)r_u16(item);
        payload += 2;
        if (kind == 10)
        {
            w_u32(object + 72, second);
            payload = r_u32(slot) + 2;
        }
    }
    else
        payload += 2;
    w_u32(object + 84, identifier);
    // TODO Malformed record pairs retain the original low-address payload read
    value = r_u16(payload);
    w_u16(object + 80, value);
    value = r_u16(payload + 2);
    w_u8(object + 67, 7);
    w_u16(object + 82, value);
}

uint32 SpawnPathParticleEmitter(uint32 identifier)
{
    uint32 object = r_u32(0x800FF434), value;
    FUNCTION_MARKER(0x8001F6A0, "SLUS_003.73");
    while (object)
    {
        value = r_u8(object + 67);
        if (value == 7)
        {
            value = r_u32(object + 84);
            if (value == identifier)
                return value;
        }
        object = r_u32(object + 4);
    }
    object = sub_80032DC0(88);
    if (object)
    {
        void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PathParticleEmitter));
        new (dram_addr) PathParticleEmitter(identifier);
    }
    return object;
}
PathParticleEmitter &PathParticleEmitter::Attach(uint32 object)
{
    return *static_cast<PathParticleEmitter *>(FindNativeObjectStorage(object));
}

uint32 PathParticleEmitter::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8001F9B8, "SLUS_003.73");
    w_u32(object + 68, 0x800A10F4);
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 PathParticleEmitter::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 previous, current, factor, record, colors[3], endpoint, channel, product, frequency, child, result;
    uint32 origin[3], target[3], difference[3], shifted[3], scaled[3], fractional[3], offset[3], point[3], velocity[3];
    uint32 shift12 = 12, shift8 = 8;
    FUNCTION_MARKER(0x8001FA0C, "SLUS_003.73");
    previous = r_u32(object + 84);
    current = r_u32(object + 72);
    for (;;)
    {
        factor = RandomRange(256);
        record = ReadRecordPosition(origin, previous);
        colors[0] = r_u16(record + 6);
        colors[1] = r_u16(record + 8);
        colors[2] = r_u16(record + 10);
        record = ReadRecordPosition(target, current);
        for (channel = 0; channel < 3; ++channel)
        {
            endpoint = r_u16(record + 6 + channel * 2);
            product = (endpoint - colors[channel]) * factor;
            colors[channel] += (uint32)((sint32)product >> 8);
        }
        SubtractVectors(difference, target, origin);
        ShiftVectorRight(shifted, difference, &shift12);
        MultiplyVector(scaled, shifted, &factor);
        ShiftVectorRight(fractional, scaled, &shift8);
        ShiftVectorLeft(offset, fractional, &shift12);
        AddVectors(point, origin, offset);
        target[0] = point[0];
        target[1] = point[1];
        target[2] = point[2];
        frequency = r_u16(object + 80);
        if (frequency && !RandomRange(frequency))
        {
            result = RandomRange(8);
            velocity[0] = 0;
            velocity[1] = ((uint32)0 - result) << 12;
            velocity[2] = 0;
            child = AllocateEffectStorage(112);
            if (child)
            {
                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MovingSprite));
                new (dram_addr) MovingSprite(target, velocity, (const char *)psx_addr(0x800FF1FC, 1), 1000, (uint32)-60, 5);
            }
            // TODO The original sets colors even when smoke allocation fails
            sub_800332FC(child, colors[0] & 255, colors[1] & 255, colors[2] & 255);
        }
        frequency = r_u16(object + 82);
        if (frequency && !RandomRange(frequency))
        {
            child = AllocateEffectStorage(116);
            if (child)
            {
                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExpandingRing));
                new (dram_addr) ExpandingRing((const sint32 *)target, 150, colors[0] & 255, colors[1] & 255, colors[2] & 255, 20, 8);
            }
        }
        record = GetRecordPayload(current);
        result = r_u16(record);
        previous = current;
        if (!result)
            return result;
        current = r_u16(record + 2);
    }
}




extern "C" uint32 sub_8001F9B8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001F9B8, "SLUS_003.73");
    return PathParticleEmitter::Attach(object).PathParticleEmitter::Destroy(reason);
}
extern "C" uint32 sub_8001FA0C(uint32 object)
{
    FUNCTION_MARKER(0x8001FA0C, "SLUS_003.73");
    return PathParticleEmitter::Attach(object).PathParticleEmitter::Update();
}
