#include "particle_settings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "spiral_ray_weapon.h"
#include "ring_burst_effect.h"
#include "fading_particle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "object_damage.h"
#include "object_collision_events.h"
#include "audio_session.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

SpiralRayWeapon::SpiralRayWeapon(ModelInstance **target) : WeaponNode(target, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80023B2C, "SLUS_003.73");
    w_u32(object, 0x800A14F0);
    this->type = WPN_SPIRAL_RAY;
    Configure(0, 0, 1, 6000, 20, 256);
}

uint32 SpiralRayWeapon::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80023B98, "SLUS_003.73");
    w_u32(object, 0x800A14F0);
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(object);
    return result;
}

uint32 SpiralRayWeapon::Configure(uint32 first, uint32 second, uint32 enabled, uint32 speed, uint32 count, uint32 radius)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80023BEC, "SLUS_003.73");
    w_u32(object + 60, first);
    w_u32(object + 64, second);
    w_u8(object + 68, enabled);
    w_u32(object + 72, speed);
    w_u32(object + 76, count);
    w_u32(object + 80, radius);
    return speed;
}

SpiralRayWeapon &SpiralRayWeapon::Attach(uint32 object)
{
    return *static_cast<SpiralRayWeapon *>(FindNativeObjectStorage(object));
}

uint32 SpiralRayWeapon::Update(const sint32 *, const sint16 *)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 state;
    FUNCTION_MARKER(0x800243DC, "SLUS_003.73");
    state = r_u32(object + 20);
    if (state)
        w_u32(object + 20, 0);
    return state;
}

void SpiralRayWeapon::StoreLight(uint8 *output, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u8(address, value);
    else
        *output = (uint8)value;
}

uint32 SpiralRayWeapon::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800243F8, "SLUS_003.73");
    if (!r_u32(object + 84))
        return 0;
    StoreLight(red, 0);
    StoreLight(green, 0);
    StoreLight(blue, 255);
    w_u32(object + 84, 0);
    return 1;
}

extern "C" uint32 sub_800243DC(uint32 object)
{
    FUNCTION_MARKER(0x800243DC, "SLUS_003.73");
    return SpiralRayWeapon::Attach(object).SpiralRayWeapon::Update(0, 0);
}

extern "C" uint32 sub_800243F8(uint32 object, uint32 red_address, uint32 green_address, uint32 blue_address)
{
    FUNCTION_MARKER(0x800243F8, "SLUS_003.73");
    uint8 *red = (uint8 *)psx_addr(red_address, 1), *green = (uint8 *)psx_addr(green_address, 1), *blue = (uint8 *)psx_addr(blue_address, 1);
    return SpiralRayWeapon::Attach(object).SpiralRayWeapon::ReadLight(red, green, blue);
}

// Preserve the original C ABI entry for unmigrated callers
uint32 sub_80023B2C(uint32 object, uint32 target, uint32 unused_flags)
{
    FUNCTION_MARKER(0x80023B2C, "SLUS_003.73");
    // The original constructor ignores the legacy third C carrier
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(SpiralRayWeapon));
    new (native_instance_storage) SpiralRayWeapon(target);
    return object;
}

uint32 sub_80023B98(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80023B98, "SLUS_003.73");
    return SpiralRayWeapon::Attach(object).SpiralRayWeapon::Destroy(reason);
}

uint32 sub_80023BEC(uint32 object, uint32 first, uint32 second, uint32 enabled, uint32 speed, uint32 count, uint32 radius)
{
    FUNCTION_MARKER(0x80023BEC, "SLUS_003.73");
    return SpiralRayWeapon::Attach(object).Configure(first, second, enabled, speed, count, radius);
}

uint32 SpiralRayWeapon::DivideBy256(uint32 value)
{
    if ((sint32)value < 0)
        value += 255;
    return (uint32)((sint32)value >> 8);
}

void SpiralRayWeapon::CopyRotation(uint32 guest, const void *native, sint16 output[3])
{
    uint32 upper, lower, word, shift;
    uint16 last;
    if (native)
    {
        memcpy(&word, native, 4);
        memcpy(&last, (const uint8 *)native + 4, 2);
    }
    else
    {
        upper = SpiralRayWeapon::ReadMasked((guest + 3) & ~3,
            (1 << (((guest + 3) & 3) + 1)) - 1);
        lower = SpiralRayWeapon::ReadMasked(guest & ~3, (15 << (guest & 3)) & 15);
        shift = (guest & 3) * 8;
        word = shift ? (upper << (32 - shift)) | (lower >> shift) : lower;
        last = r_u16(guest + 4);
    }
    output[0] = (sint16)word;
    output[1] = (sint16)(word >> 16);
    output[2] = (sint16)last;
}

uint32 SpiralRayWeapon::ReadWord(uint32 guest, const uint32 *native, uint32 index)
{
    return native ? native[index] : r_u32(guest + index * 4);
}

uint32 SpiralRayWeapon::ReadMasked(uint32 address, uint32 mask)
{
    uint32 result = 0, lane;
    if (mask == 15)
        return r_u32(address);
    /* Read only selected bytes of a partial RAM word */
    for (lane = 0; lane < 4; ++lane)
        if (mask & (1 << lane))
            result |= (uint32)r_u8(address + lane) << (lane * 8);
    return result;
}

uint32 SpiralRayWeapon::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 trig_angle;
    CollisionRay &shared_ray = GetSharedCollisionRay();
    uint32 guest_position = GuestMemoryAddress(input_position), guest_angles = GuestMemoryAddress(input_angles);
    const uint32 *native_position = guest_position ? 0 : input_position;
    const void *native_angles = guest_angles ? 0 : input_angles;
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80023C14, "SLUS_003.73");
    uint32 x, y, z, enabled, previous, frame, sound, radius, child;
    uint32 scale20, scale14, scale10, random, spread, target, damage;
    ModelInstance **filter;
    uint32 distance, offset, ring_index, index, cosine, sine, old_x;
    uint32 direction[3], endpoint[3], ring[16][3], temporary[3], side[3], scaled[3];
    sint16 angles[3], effect_angles[3];
    x = SpiralRayWeapon::ReadWord(guest_position, native_position, 0);
    y = SpiralRayWeapon::ReadWord(guest_position, native_position, 1);
    z = SpiralRayWeapon::ReadWord(guest_position, native_position, 2);
    w_u32(object + 8, x);
    w_u32(object + 12, y);
    w_u32(object + 16, z);
    enabled = r_u8(object + 68);
    previous = r_u32(object + 24);
    frame = r_u32(0x800FF2F0);
    if ((sint32)(frame - previous) < (enabled ? 30 / (sint32)enabled : -1))
    {
        w_u32(object + 20, 1);
        return 0;
    }
    sound = PlaySoundEffect((sint32)r_u32(object + 76), 8192, 0);
    radius = r_u32(object + 80);
    this->sound = sound;
    w_u32(object + 84, 1);
    if (radius)
    {
        child = AllocateEffectStorage(124);
        if (child)
        {
            scale20 = SpiralRayWeapon::DivideBy256(20 * r_u32(object + 80));
            scale14 = SpiralRayWeapon::DivideBy256(14 * r_u32(object + 80));
            scale10 = SpiralRayWeapon::DivideBy256(10 * r_u32(object + 80));
            radius = SpiralRayWeapon::DivideBy256(20 * r_u32(object + 80));
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
            new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(object + 8, 12), 5, 255, 235, 0, 2, 90, 10, 0, 20, scale20, 0, 0, scale14, 0, scale10, radius, 0, 2);
        }
    }
    OffsetPositionByAngles((uint32 *)psx_addr(object + 8, 12), (const uint16 *)input_angles);
    SpiralRayWeapon::CopyRotation(guest_angles, native_angles, angles);
    spread = r_u32(object + 60);
    random = RandomRange((spread << 1) | 1);
    spread = r_u16(object + 60);
    angles[0] = (sint16)((uint32)(uint16)angles[0] + random - spread);
    spread = r_u32(object + 64);
    random = RandomRange((spread << 1) | 1);
    spread = r_u16(object + 64);
    angles[1] = (sint16)((uint32)(uint16)angles[1] + random - spread);
    NormalizeAngles((uint16 *)angles);
    BuildDirectionVector(endpoint, r_u32(object + 72), (const uint16 *)angles);
    DivideVector(direction, endpoint, (const uint32 *)psx_addr(object + 72, 4));
    AddVectors(endpoint, (const uint32 *)psx_addr(object + 8, 12));
    shared_ray.pos[0][0] = (r_u32(object + 8));
    shared_ray.pos[0][1] = (r_u32(object + 12));
    z = r_u32(object + 16);
    shared_ray.pos[1][0] = (endpoint[0]);
    shared_ray.pos[1][1] = (endpoint[1]);
    shared_ray.pos[1][2] = (endpoint[2]);
    shared_ray.pos[0][2] = (z);
    InitializeCollisionRay(shared_ray);
    w_u32(0x800FF974, 1);
    QueryCollisionRay(shared_ray, 1);
    w_u32(0x800FF974, 0);
    w_u8(object + 88, 0);
    target = shared_ray.hit.GetObject();
    if (target)
    {
        endpoint[0] = shared_ray.hit_pos[0];

        endpoint[1] = shared_ray.hit_pos[1];
        endpoint[2] = shared_ray.hit_pos[2];
        damage = this->damage;
        DamageLevelObject(target, shared_ray.hit.GetPolygon(), (sint32)damage);
        target = shared_ray.hit.GetObject();
        if (r_u16(target) & 16)
        {
            uint32 receiver;
            damage = this->damage;
            receiver = target;
            ApplyObjectDamage(receiver, damage, (const sint32 *)direction, 28);
        }
        filter = this->owner_list;
        target = IntersectObjectSpheres((const uint32 *)psx_addr(object + 8, 12), endpoint, endpoint, *filter, 0);
        if (target)
        {
            uint32 receiver;
            damage = this->damage;
            receiver = target;
            ApplyObjectDamage(receiver, damage, (const sint32 *)direction, 28);
        }
        else
        {
            w_u8(object + 88, 1);
            w_u32(object + 92, endpoint[0]);
            w_u32(object + 96, endpoint[1]);
            w_u32(object + 100, endpoint[2]);
            w_u16(object + 104, (uint16)shared_ray.n.vx);
            w_u16(object + 106, (uint16)shared_ray.n.vy);
            w_u16(object + 108, (uint16)shared_ray.n.vz);
            x = r_u32(object + 8);
            y = r_u32(object + 12);
            z = r_u32(object + 16);
            w_u32(object + 112, x);
            w_u32(object + 116, y);
            w_u32(object + 120, z);
        }
        apocalypse_projectile_impact_2289C(4, (const sint32 *)endpoint);
    }
    else
    {
        filter = this->owner_list;
        target = IntersectObjectSpheres((const uint32 *)psx_addr(object + 8, 12), endpoint, endpoint, *filter, 0);
        if (target)
        {
            uint32 receiver;
            damage = this->damage;
            receiver = target;
            ApplyObjectDamage(receiver, damage, (const sint32 *)direction, 28);
            apocalypse_projectile_impact_2289C(4, (const sint32 *)endpoint);
        }
    }
    /* The original delay loop has no memory or call side effects */
    for (index = 0; index < 16; ++index)
    {
        trig_angle = (index << 8) & 0xF00;
        ring[index][0] = 48 * (uint32)rsin(trig_angle);
        ring[index][2] = 0;
        ring[index][1] = 0 - 48 * (uint32)rcos(trig_angle);
        if (angles[0])
        {
            trig_angle = (uint16)angles[0] & 4095;
            cosine = (uint32)rcos(trig_angle);
            ring[index][1] = (uint32)((sint32)(ring[index][1] * cosine) >> 12);
            sine = (uint32)rsin(trig_angle);
            ring[index][2] = (uint32)((sint32)((0 - ring[index][1]) * sine) >> 12);
        }
        if (angles[1])
        {
            trig_angle = (uint16)angles[1] & 4095;
            old_x = ring[index][0];
            cosine = (uint32)rcos(trig_angle);
            sine = (uint32)rsin(trig_angle);
            ring[index][0] = (uint32)((sint32)(old_x * cosine + ring[index][2] * sine) >> 12);
            sine = (uint32)rsin(trig_angle);
            cosine = (uint32)rcos(trig_angle);
            ring[index][2] = (uint32)((sint32)(old_x * sine - ring[index][2] * cosine) >> 12);
        }
    }
    effect_angles[0] = -1024;
    effect_angles[1] = effect_angles[2] = 0;
    SetParticleDirection((const sint16 *)effect_angles);
    SetParticleSpread((const sint16 *)psx_addr(0x800FF5E4, 6));
    SetParticleSize(2);
    SetParticleColor(32, 32, 128);
    SetParticleColorStep(2, 2, 4);
    w_u32(0x800FF3AC, 0);
    SubtractVectors(temporary, endpoint, (const uint32 *)psx_addr(object + 8, 12));
    distance = MeasureVectorLength(temporary);
    ring_index = 0;
    for (offset = 0; (sint32)offset < (sint32)distance; offset += 64)
    {
        AddVectors(side, (const uint32 *)psx_addr(object + 8, 12), ring[ring_index]);
        MultiplyVector(scaled, direction, &offset);
        AddVectors(temporary, side, scaled);
        child = AllocateEffectStorage(88);
        if (child)
        {
            random = RandomRange(1024);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(FadingParticle));
            new (dram_addr) FadingParticle(temporary, 0, 0 - (random + 2048), 16);
        }
        ring_index = (ring_index + 2) & 15;
    }
    SetParticleColor(64, 64, 64);
    SetParticleColorStep(4, 4, 4);
    for (offset = 0; (sint32)offset < (sint32)distance; offset += 48)
    {
        MultiplyVector(side, direction, &offset);
        AddVectors(temporary, (const uint32 *)psx_addr(object + 8, 12), side);
        child = AllocateEffectStorage(88);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(FadingParticle));
            new (dram_addr) FadingParticle(temporary, 0, 0, 12);
        }
    }
    frame = r_u32(0x800FF2F0);
    w_u32(0x800FF3AC, 1);
    w_u32(object + 20, 1);
    w_u32(object + 24, frame);
    return 1;
}

uint32 sub_80023C14(uint32 object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x80023C14, "SLUS_003.73");
    return SpiralRayWeapon::Attach(object).SpiralRayWeapon::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

uint32 apocalypse_weapon_fire_23C14(uint32 object, const sint32 position[3], const sint16 angles[3])
{
    return SpiralRayWeapon::Attach(object).SpiralRayWeapon::Fire((const uint32 *)position, angles);
}
