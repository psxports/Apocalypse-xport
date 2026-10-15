extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_oscillating.h"
#include "ring_effect.h"
#include "object_damage.h"
#include "object_collision_events.h"
#include "native_object_storage.h"
#include "vector_arithmetic.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "ring_burst_effect.h"
#include "angle_math.h"
#include <new>

static uint32 DivideOscillatingScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

OscillatingProjectile::OscillatingProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2) : EffectController()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 child, x, y, z, green, red, blue, green_tail, red_tail, blue_tail, duration, velocity, coordinate, hit, candidate, distance;
    FUNCTION_MARKER(0x8001C158, "SLUS_003.73");
    red1 &= 255;
    green1 &= 255;
    blue1 &= 255;
    red2 &= 255;
    green2 &= 255;
    blue2 &= 255;
    w_u32(object + 68, 0x800A109C);
    w_u8(object + 67, 1);
    w_u8(object + 104, (red1 + red2) >> 1);
    w_u8(object + 105, (green1 + green2) >> 1);
    w_u8(object + 106, (blue1 + blue2) >> 1);
    child = AllocateEffectStorage(124);
    if (child)
    {
        green = r_u8(object + 105);
        red = r_u8(object + 104);
        blue = r_u8(object + 106);
        red_tail = r_u8(object + 104);
        green_tail = r_u8(object + 105);
        blue_tail = r_u8(object + 106);
        duration = amplitude << 1;
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
        new (dram_addr) RingBurstEffect(position, 4, red, green, blue, 2, red_tail, green_tail, blue_tail, 20, duration, 0, 0, duration, 0, duration, 0, 0, 0);
    }
    x = native_position ? native_position[0] : r_u32(guest_position);
    y = native_position ? native_position[1] : r_u32(guest_position + 4);
    z = native_position ? native_position[2] : r_u32(guest_position + 8);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    BuildDirectionVector((uint32 *)psx_addr(object + 36, 12), speed, (const uint16 *)angles);
    w_u32(object + 72, damage);
    w_u32(object + 84, amplitude);
    w_u32(object + 80, lifetime);
    w_u32(object + 88, 370);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect(2, 1);
    }
    w_u32(object + 76, child);
    RingEffect::Attach(child).SetCenterColor(red1, green1, blue1);
    RingEffect::Attach(r_u32(object + 76)).SetRingColors(red2, green2, blue2);
    RingEffect::Attach(r_u32(object + 76)).SetRowColors(0, 0, 0, 0);
    RingEffect::Attach(r_u32(object + 76)).SetOddRadii(amplitude);
    RingEffect::Attach(r_u32(object + 76)).SetEvenRadii(amplitude);
    RingEffect::Attach(r_u32(object + 76)).SetRowExtent(0, (uint32)((sint32)(42 * amplitude) / 16));
    RingEffect::Attach(r_u32(object + 76)).SetPosition((const uint32 *)psx_addr(object + 24, 12));
    w_u8(r_u32(object + 76) + 66, 1);
    w_u16(object + 10, r_u16(object + 80) + 26);
    w_u32(0x800ED638, r_u32(object + 24));
    w_u32(0x800ED63C, r_u32(object + 28));
    w_u32(0x800ED640, r_u32(object + 32));
    duration = r_u16(object + 10);
    velocity = r_u32(object + 36);
    coordinate = r_u32(object + 24);
    w_u32(0x800ED644, coordinate + duration * velocity);
    duration = r_u16(object + 10);
    velocity = r_u32(object + 40);
    coordinate = r_u32(object + 28);
    w_u32(0x800ED648, coordinate + duration * velocity);
    duration = r_u16(object + 10);
    velocity = r_u32(object + 44);
    coordinate = r_u32(object + 32);
    w_u32(0x800ED64C, coordinate + duration * velocity);
    sub_8007BB24(0x800ED638);
    w_u32(0x800FF974, 1);
    sub_8007DD04(0x800ED638, 0);
    hit = r_u32(0x800ED6A0);
    w_u32(0x800FF974, 0);
    if (hit)
    {
        candidate = r_u32(0x800FF794);
        while (candidate && candidate != hit)
            candidate = r_u32(candidate + 28);
        w_u32(object + 96, r_u32(0x800ED6A0));
        w_u32(object + 100, r_u32(0x800ED6B8));
        distance = r_u32(0x800ED678);
        w_u16(object + 10, DivideOscillatingScalar(distance, speed));
    }
    if (r_u16(object + 10) >= 151)
        w_u16(object + 10, 150);
}

uint32 OscillatingProjectile::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, result;
    sint32 adjustment;
    FUNCTION_MARKER(0x8001C51C, "SLUS_003.73");
    child = r_u32(object + 76);
    w_u32(object + 68, 0x800A109C);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    EffectController::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 OscillatingProjectile::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 position = object + 24, velocity = object + 36;
    uint32 old_position[3], hit_record[4], hit_vector[4];
    uint32 child, base, phase, increment, hit, trigger, result, damage;
    uint32 red1, green1, blue1, red2, green2, blue2, material, head;
    sint32 amplitude, oscillation, scale, threshold;
    sint16 tick;
    FUNCTION_MARKER(0x8001C598, "SLUS_003.73");
    old_position[0] = r_u32(position);
    old_position[1] = r_u32(position + 4);
    old_position[2] = r_u32(position + 8);
    AddVectors((uint32 *)psx_addr(position, 12), (const uint32 *)psx_addr(velocity, 12));
    RingEffect::Attach(r_u32(object + 76)).SetPosition((const uint32 *)psx_addr(position, 12));
    child = r_u32(object + 76);
    w_u16(child + 96, r_u16(child + 96) + 140);
    base = r_u32(object + 84);
    phase = r_u32(object + 92);
    increment = r_u32(object + 88);
    phase += increment;
    w_u32(object + 92, phase);
    amplitude = (sint32)(6 * base) / 16;
    oscillation = (sint32)((uint32)amplitude * (uint32)(sint32)(sint16)r_u16(0x800F863C + (phase & 4095) * 4)) / 4096;
    scale = (sint32)(base + (uint32)oscillation);
    RingEffect::Attach(r_u32(object + 76)).SetOddRadii((uint32)scale);
    RingEffect::Attach(r_u32(object + 76)).SetEvenRadii((uint32)scale);
    RingEffect::Attach(r_u32(object + 76)).SetRowExtent(0, (uint32)((sint32)(42 * (uint32)scale) / 16));
    head = r_u32(0x800FF5A0);
    hit = IntersectObjectSpheres(old_position, (const uint32 *)psx_addr(position, 12), hit_record, head, 0);
    if (hit)
    {
        NormalizeFixedVector(hit_vector, (const uint32 *)psx_addr(velocity, 12));
        uint32 table = r_u32(hit + 68);
        damage = r_u32(object + 72);
        sint32 adjustment = (sint16)r_u16(table + 48);
        uint32 target = r_u32(table + 52);
        apocalypse_object_virtual52_native(target, hit + (uint32)adjustment, damage, (const sint32 *)hit_vector, 29);
        child = AllocateEffectStorage(124);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
            new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(hit + 4, 12), 4, 255, 100, 0, 2, 255, 100, 0, 20, 80, 0, 0, 30, 30, 15, 15, 0, 0);
        }
        return Finish();
    }
    tick = (sint16)(r_u16(object + 8) + 1);
    threshold = (sint32)r_u32(object + 80);
    w_u16(object + 8, (uint16)tick);
    if (threshold < tick)
    {
        RingEffect::Attach(r_u32(object + 76)).FadeCenter(10);
        RingEffect::Attach(r_u32(object + 76)).FadeRing(10);
    }
    tick = (sint16)r_u16(object + 8);
    result = r_u16(object + 10);
    if (tick >= (sint32)result)
    {
        child = AllocateEffectStorage(124);
        if (child)
        {
            green1 = r_u8(object + 105);
            red1 = r_u8(object + 104);
            blue1 = r_u8(object + 106);
            red2 = r_u8(object + 104);
            green2 = r_u8(object + 105);
            blue2 = r_u8(object + 106);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
            new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(position, 12), 3, red1, green1, blue1, 2, red2, green2, blue2, 20, 40, 0, 0, 50, 50, 25, 25, 0, 0);
        }
        Finish();
        PlayPositionedSound(24, (const uint32 *)psx_addr(position, 12), 0);
        trigger = r_u32(object + 96);
        if (trigger && !(trigger & 3))
        {
            material = r_u32(object + 100);
            damage = r_u32(object + 72);
            DamageLevelObject(trigger, material, (sint32)damage);
        }
    }
    result = r_u32(r_u32(object + 76) + 88) & 0xFFFFFF;
    if (!result)
        return Finish();
    return result;
}

OscillatingProjectile &OscillatingProjectile::Attach(uint32 object)
{
    return *static_cast<OscillatingProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8001C158(uint32 object, uint32 position, uint32 angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2)
{
    FUNCTION_MARKER(0x8001C158, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(OscillatingProjectile));
    new (dram_addr) OscillatingProjectile((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), damage, lifetime, speed, amplitude, red1, green1, blue1, red2, green2, blue2);
    return object;
}

extern "C" uint32 apocalypse_projectile_construct_1C158(uint32 object, const void *position, const void *angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(OscillatingProjectile));
    new (dram_addr) OscillatingProjectile((const uint32 *)position, (const sint16 *)angles, damage, lifetime, speed, amplitude, red1, green1, blue1, red2, green2, blue2);
    return object;
}

extern "C" uint32 sub_8001C51C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001C51C, "SLUS_003.73");
    return OscillatingProjectile::Attach(object).OscillatingProjectile::Destroy(reason);
}

extern "C" uint32 sub_8001C598(uint32 object)
{
    FUNCTION_MARKER(0x8001C598, "SLUS_003.73");
    return OscillatingProjectile::Attach(object).OscillatingProjectile::Update();
}
