#include "game_sdk_bindings.h"
#include "weapon_targeting.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "projectile_tracking.h"
#include "projectile_line.h"
#include "projectile_launch.h"
#include "projectile_ballistic.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "weapon_nodes.h"
#include "beam_segments.h"
#include "ring_burst_effect.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "object_collision_events.h"
#include "object_damage.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

// Native output views preserve the original ordered byte stores

#include <stdio.h>
#include <stdlib.h>

#include "spiral_ray_weapon.h"
#include "machine_gun.h"
#include "flamethrower.h"

static void StoreTimedWeaponLight(uint8 *output, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u8(address, value);
    else
        *output = (uint8)value;
}

static void DestroyParticleBeamChild(uint32 child)
{
    uint32 table, target;
    sint32 adjustment;
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
}

static sint32 DivideBeamScalar(sint32 numerator, sint32 denominator)
{
    if (!denominator)
        return numerator < 0 ? 1 : -1;
    if ((uint32)numerator == 0x80000000 && denominator == -1)
        return numerator;
    return numerator / denominator;
}

static uint32 ReadBeamWord(uint32 guest, const uint32 *native, uint32 index)
{
    return native ? native[index] : r_u32(guest + 4 * index);
}

static void ReadBeamPosition(uint32 guest, const uint32 *native, uint32 output[3])
{
    output[0] = ReadBeamWord(guest, native, 0);
    output[1] = ReadBeamWord(guest, native, 1);
    output[2] = ReadBeamWord(guest, native, 2);
}

static void StoreBeamVector(uint32 address, const uint32 input[3])
{
    w_u32(address, input[0]);
    w_u32(address + 4, input[1]);
    w_u32(address + 8, input[2]);
}

static void CopyBeamVector(uint32 destination, uint32 source)
{
    uint32 position[3];
    position[0] = r_u32(source);
    position[1] = r_u32(source + 4);
    position[2] = r_u32(source + 8);
    StoreBeamVector(destination, position);
}

static void ReadRipLaserPosition(uint32 guest, const uint32 *native, uint32 output[3])
{
    output[0] = native ? native[0] : r_u32(guest);
    output[1] = native ? native[1] : r_u32(guest + 4);
    output[2] = native ? native[2] : r_u32(guest + 8);
}

static uint32 ReadRipLaserAngle(uint32 guest, const sint16 *native, uint32 index)
{
    return native ? (uint16)native[index] : r_u16(guest + index * 2);
}

void ParticleBeam::DamageTarget(uint32 hit, const uint32 direction[3])
{
    uint32 owner = NativeObjectGuestAddress(this);
    uint32 table = r_u32(hit + 68);
    uint32 damage = r_u32(owner + 40);
    uint32 receiver = hit + (uint32)(sint32)(sint16)r_u16(table + 48);
    uint32 target = r_u32(table + 52);
    apocalypse_object_virtual52_native(target, receiver, damage, (const sint32 *)direction, 10);
}

ParticleBeam &ParticleBeam::Attach(uint32 object)
{
    return *static_cast<ParticleBeam *>(FindNativeObjectStorage(object));
}

void RipLaser::DamageTarget(uint32 hit)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 table, damage, target;
    sint32 adjustment;
    table = r_u32(hit + 68);
    damage = r_u32(object + 40);
    adjustment = (sint16)r_u16(table + 48);
    target = r_u32(table + 52);
    apocalypse_object_virtual52_native(target, hit + (uint32)adjustment, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 11);
}

WeaponNode &WeaponNode::Attach(uint32 object)
{
    return *static_cast<WeaponNode *>(FindNativeObjectStorage(object));
}

HomingMissiles &HomingMissiles::Attach(uint32 object)
{
    return *static_cast<HomingMissiles *>(FindNativeObjectStorage(object));
}

PulseLaser &PulseLaser::Attach(uint32 object)
{
    return *static_cast<PulseLaser *>(FindNativeObjectStorage(object));
}

RipLaser &RipLaser::Attach(uint32 object)
{
    return *static_cast<RipLaser *>(FindNativeObjectStorage(object));
}

Grenades &Grenades::Attach(uint32 object)
{
    return *static_cast<Grenades *>(FindNativeObjectStorage(object));
}

Rockets &Rockets::Attach(uint32 object)
{
    return *static_cast<Rockets *>(FindNativeObjectStorage(object));
}

uint32 WeaponNode::ReadAnglesWord(uint32 address)
{
    uint32 left = r_u32((address + 3) & ~3);
    uint32 right = r_u32(address & ~3);
    uint32 shift = (address & 3) * 8;
    return shift ? (left << (32 - shift)) | (right >> shift) : right;
}

WeaponNodeBase &WeaponNodeBase::Attach(uint32 object)
{
    return *static_cast<WeaponNodeBase *>(FindNativeObjectStorage(object));
}

uint32 WeaponNode::Fire(const uint32 *position, const sint16 *angles)
{
    return FireInactiveWeapon();
}

sint32 ReadWeaponLightRandom()
{
    return randPSX();
}

WeaponNode::WeaponNode(uint32 list, uint32 mode) : WeaponNodeBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80022A24, "SLUS_003.73");
    w_u32(object, 0x800A1570);
    w_u32(object + 44, list);
    w_u32(object + 48, mode);
    w_u32(object + 56, 1000);
}

uint32 WeaponNode::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 sound, result;
    FUNCTION_MARKER(0x80022A48, "SLUS_003.73");
    w_u32(object, 0x800A1570);
    ClearChild();
    sound = r_u32(object + 52);
    if (sound)
        StopSoundVoices(sound);
    w_u32(object, 0x800A15D8);
    result = reason & 1;
    if (result)
        result = ReleaseWeaponStorage(object);
    return result;
}

void WeaponNode::ClearChild()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x80022ABC, "SLUS_003.73");
    child = r_u32(object + 32);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    w_u32(object + 32, 0);
}

uint32 WeaponNode::ConsumeAmmo(uint32 amount)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80022B34, "SLUS_003.73");
    result = r_u32(object + 56) - amount;
    w_u32(object + 56, result);
    if ((sint32)result < 0)
        w_u32(object + 56, 0);
    return result;
}

HomingMissiles::HomingMissiles(uint32 list) : WeaponNode(list, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80025070, "SLUS_003.73");
    w_u32(object, 0x800A14A0);
    w_u32(object + 4, WPN_HOMING_MISSILES);
    w_u32(object + 60, 15);
}

uint32 HomingMissiles::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x800250B8, "SLUS_003.73");
    w_u32(object, 0x800A14A0);
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(object);
    return result;
}

uint32 HomingMissiles::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, position[3], projectile, damage, list, duration;
    FUNCTION_MARKER(0x8002510C, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (timer)
    {
        w_u32(object + 64, timer - 1);
        return 0;
    }
    uint32 guest_position = GuestMemoryAddress(input_position);
    if (guest_position)
    {
        position[0] = r_u32(guest_position);
        position[1] = r_u32(guest_position + 4);
        position[2] = r_u32(guest_position + 8);
    }
    else
        memcpy(position, input_position, 12);
    OffsetPositionByAngles(position, (const uint16 *)input_angles);
    projectile = AllocateZeroedObject(420);
    if (projectile)
    {
        damage = r_u32(object + 40);
        list = r_u32(object + 44);
        void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(TrackingProjectile));
        new (dram_addr) TrackingProjectile(position, input_angles, 1000, damage, list);
    }
    ConsumeAmmo(50);
    if ((sint32)r_u32(object + 56) < 50)
        w_u32(object + 56, 0);
    duration = r_u32(object + 60);
    w_u32(object + 64, duration);
    return 1;
}

uint32 HomingMissiles::Update(const sint32 *, const sint16 *)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, result;
    FUNCTION_MARKER(0x800251E8, "SLUS_003.73");
    timer = r_u32(object + 64);
    result = timer - 1;
    if (timer)
        w_u32(object + 64, result);
    return result;
}

uint32 HomingMissiles::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer;
    FUNCTION_MARKER(0x80025204, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (!timer)
        return 0;
    if ((sint32)(r_u32(object + 60) - timer) >= 5)
        return 0;
    StoreTimedWeaponLight(red, 255);
    StoreTimedWeaponLight(green, 255);
    StoreTimedWeaponLight(blue, 0);
    return 1;
}

PulseLaser::PulseLaser(uint32 list) : WeaponNode(list, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800252CC, "SLUS_003.73");
    w_u32(object, 0x800A1478);
    w_u32(object + 4, WPN_PULSE_LASER);
    w_u32(object + 60, 3);
}

uint32 PulseLaser::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    uint32 sound;
    FUNCTION_MARKER(0x80025314, "SLUS_003.73");
    sound = r_u32(object + 52);
    w_u32(object, 0x800A1478);
    if (sound)
        StopSoundVoices(sound);
    w_u32(object + 52, 0);
    WeaponNode::Destroy(0);
    result = reason & 1;
    if (reason & 1)
        result = ReleaseWeaponStorage(object);
    return result;
}

uint32 PulseLaser::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 sound, timer, projectile, damage, owner, particle, duration;
    FUNCTION_MARKER(0x80025380, "SLUS_003.73");
    sound = r_u32(object + 52);
    if (!sound)
    {
        sound = PlaySoundEffect(30, 8192, 0);
        w_u32(object + 52, sound);
    }
    timer = r_u32(object + 64);
    if (timer)
    {
        w_u32(object + 64, timer - 1);
        return 0;
    }
    projectile = AllocateEffectStorage(268);
    if (projectile)
    {
        damage = r_u32(object + 40);
        owner = r_u32(object + 44);
        void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(LineProjectile));
        new (dram_addr) LineProjectile(input_position, input_angles, 500, damage, (const uint32 *)psx_addr(owner, 4), 0, 130, 0, 1);
    }
    particle = AllocateEffectStorage(124);
    if (particle)
    {
        void *dram_addr = AllocateNativeObjectStorage(particle, sizeof(RingBurstEffect));
        new (dram_addr) RingBurstEffect(input_position, 5, 255, 255, 255, 2, 0, 100, 0, 20, 20, 0, 0, 14, 0, 10, 20, 0, 1);
    }
    ConsumeAmmo(20);
    if ((sint32)r_u32(object + 56) < 20)
        w_u32(object + 56, 0);
    duration = r_u32(object + 60);
    w_u32(object + 64, duration);
    return 1;
}

uint32 PulseLaser::Update(const sint32 *, const sint16 *)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, sound;
    FUNCTION_MARKER(0x800254E8, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (timer)
        w_u32(object + 64, timer - 1);
    sound = r_u32(object + 52);
    if (sound)
        StopSoundVoices(sound);
    w_u32(object + 52, 0);
    return 0;
}

uint32 PulseLaser::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer;
    FUNCTION_MARKER(0x80025538, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (!timer)
        return 0;
    if ((sint32)(r_u32(object + 60) - timer) >= 3)
        return 0;
    StoreTimedWeaponLight(red, 0);
    StoreTimedWeaponLight(green, 255);
    StoreTimedWeaponLight(blue, 0);
    return 1;
}

ParticleBeam::ParticleBeam(uint32 list, uint32 range, uint32 mode, uint32 effect_scale, uint32 sound) : WeaponNode(list, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80027720, "SLUS_003.73");
    w_u32(object, 0x800A1408);
    w_u32(object + 4, WPN_PARTICLE_BEAM);
    w_u32(object + 76, mode);
    w_u32(object + 100, sound);
    w_u32(object + 96, effect_scale);
    w_u32(object + 72, (uint32)((sint32)range / 9));
    SetColors(255, 128, 0, 185, 185, 185, 160, 80, 0, 255, 255, 255);
}

uint32 ParticleBeam::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 sound, result;
    FUNCTION_MARKER(0x80027804, "SLUS_003.73");
    sound = r_u32(object + 52);
    w_u32(object, 0x800A1408);
    if (sound)
        StopSoundVoices(sound);
    w_u32(object + 52, 0);
    ClearChildren();
    WeaponNode::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseWeaponStorage(object);
    return result;
}

uint32 ParticleBeam::SetColors(uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2, uint32 red3, uint32 green3, uint32 blue3, uint32 red4, uint32 green4, uint32 blue4)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80027878, "SLUS_003.73");
    w_u8(object + 80, red1);
    w_u8(object + 81, green1);
    w_u8(object + 82, blue1);
    w_u8(object + 83, red2);
    w_u8(object + 84, green2);
    w_u8(object + 85, blue2);
    w_u8(object + 86, red3);
    w_u8(object + 87, green3);
    w_u8(object + 88, blue3);
    w_u8(object + 89, red4);
    w_u8(object + 90, green4);
    w_u8(object + 91, blue4);
    return red2;
}

void ParticleBeam::ClearChildren()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child;
    FUNCTION_MARKER(0x800278D0, "SLUS_003.73");
    child = r_u32(object + 60);
    DestroyParticleBeamChild(child);
    child = r_u32(object + 64);
    w_u32(object + 60, 0);
    DestroyParticleBeamChild(child);
    child = r_u32(object + 92);
    w_u32(object + 64, 0);
    DestroyParticleBeamChild(child);
    w_u32(object + 92, 0);
}

uint32 ParticleBeam::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 guest_position = GuestMemoryAddress(input_position);
    const uint32 *native_position = guest_position ? 0 : input_position;
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80027978, "SLUS_003.73");
    uint32 direction[3], normalized[3], position[3], endpoint[3], temporary[3], scaled[3];
    uint32 final[3], intersection[4], child, second, source, destination, count, index;
    uint32 red, green, blue, value, timer, random, sine, step, filter, hit, wall;
    uint32 object_hit = 0, wall_hit = 0, distance, other_distance, target, material, damage;
    uint32 scalar, offset, left, right;
    sint32 segment;
    if (!r_u32(object + 52))
    {
        value = PlaySoundEffect(r_u32(object + 100), 8192, 0);
        w_u32(object + 52, value);
    }
    ConsumeAmmo(4);
    value = r_u32(object + 72);
    BuildDirectionVector(direction, value, (const uint16 *)input_angles);
    DivideVector(normalized, direction, (const uint32 *)psx_addr(object + 72, 4));
    if (!r_u32(object + 60))
    {
        child = AllocateEffectStorage(84);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
            new (dram_addr) BeamSegments(10, 1);
        }
        w_u32(object + 60, child);
        w_u8(child + 66, 1);
        child = r_u32(object + 60);
        red = r_u8(object + 80);
        green = r_u8(object + 81);
        blue = r_u8(object + 82);
        BeamSegments::Attach(child).SetColors(red, green, blue);
        destination = r_u32(r_u32(object + 60) + 80);
        w_u16(destination + 340, 0);
        for (index = 300; index < 303; ++index)
        {
            destination = r_u32(r_u32(object + 60) + 80);
            value = r_u8(destination + index);
            w_u8(destination + index, value >> 1);
        }
        for (index = 336; index < 339; ++index)
            w_u8(r_u32(r_u32(object + 60) + 80) + index, 0);
        ReadBeamPosition(guest_position, native_position, position);
        for (index = 0; index < 10; ++index)
        {
            destination = r_u32(r_u32(object + 60) + 80) + 36 * index;
            StoreBeamVector(destination, position);
            AddVectors(position, direction);
        }
        child = AllocateEffectStorage(84);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
            new (dram_addr) BeamSegments(10, 0);
        }
        w_u32(object + 64, child);
        w_u8(child + 66, 1);
        child = r_u32(object + 64);
        red = r_u8(object + 83);
        green = r_u8(object + 84);
        blue = r_u8(object + 85);
        BeamSegments::Attach(child).SetColors(red, green, blue);
        BeamSegments::Attach(r_u32(object + 64)).SetSize(10);
        for (index = 0; index < 10; ++index)
        {
            second = r_u32(object + 64);
            child = r_u32(object + 60);
            destination = r_u32(second + 80) + 36 * index;
            source = r_u32(child + 80) + 36 * index;
            CopyBeamVector(destination, source);
        }
        for (index = 336; index < 339; ++index)
            w_u8(r_u32(r_u32(object + 64) + 80) + index, 0);
        if (r_u32(object + 96))
        {
            child = AllocateEffectStorage(104);
            if (child)
            {
                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
                new (dram_addr) RingEffect(7, 1);
            }
            w_u32(object + 92, child);
            w_u8(child + 66, 1);
            child = r_u32(object + 92);
            red = r_u8(object + 89);
            green = r_u8(object + 90);
            blue = r_u8(object + 91);
            RingEffect::Attach(child).SetCenterColor(red, green, blue);
            child = r_u32(object + 92);
            red = r_u8(object + 86);
            green = r_u8(object + 87);
            blue = r_u8(object + 88);
            RingEffect::Attach(child).SetRingColors(red, green, blue);
            RingEffect::Attach(r_u32(object + 92)).SetRowColors(0, 0, 0, 0);
            value = r_u32(object + 96);
            child = r_u32(object + 92);
            RingEffect::Attach(child).SetRowExtent(0, (uint32)((sint32)(30 * value) >> 8));
            child = r_u32(object + 92);
            count = r_u32(child + 80);
            index = 0;
            while (index < count)
            {
                w_u32(r_u32(child + 72) + 8 * index, 0);
                child = r_u32(object + 92);
                count = r_u32(child + 80);
                ++index;
            }
            RingEffect::Attach(r_u32(object + 92)).SetPosition((const uint32 *)psx_addr(object + 8, 12));
        }
    }
    step = r_u32(object + 76) ? 10 : 2;
    count = r_u32(r_u32(object + 60) + 76);
    segment = (sint32)(count - 1);
    while ((sint32)(step - 1) < segment)
    {
        second = r_u32(object + 64);
        child = r_u32(object + 60);
        destination = r_u32(second + 80) + 36 * (uint32)segment;
        source = r_u32(child + 80) + 36 * (uint32)segment;
        MultiplyVector(scaled, direction, &step);
        offset = 36 * ((uint32)segment - step);
        child = r_u32(object + 60);
        left = r_u32(child + 80) + offset;
        AddVectors(temporary, (const uint32 *)psx_addr(left, 12), scaled);
        segment = (sint32)((uint32)segment - 1);
        StoreBeamVector(source, temporary);
        CopyBeamVector(destination, source);
    }
    ReadBeamPosition(guest_position, native_position, position);
    for (index = 0; index < step; ++index)
    {
        child = r_u32(object + 60);
        second = r_u32(object + 64);
        source = r_u32(child + 80) + 36 * index;
        destination = r_u32(second + 80) + 36 * index;
        StoreBeamVector(source, position);
        CopyBeamVector(destination, source);
        AddVectors(position, direction);
    }
    timer = 0;
    for (index = 0; index < 9; ++index)
    {
        random = RandomRange(256);
        for (offset = 12; offset < 15; ++offset)
            w_u8(r_u32(r_u32(object + 64) + 80) + 36 * index + offset, random);
        value = r_u32(0x800FF2F0);
        sine = (uint32)(sint32)(sint16)r_u16(0x800F863C + 4 * ((900 * value + timer) & 4095));
        child = r_u32(object + 60);
        destination = r_u32(child + 80) + 36 * index;
        w_u16(destination + 16, (uint32)((sint32)(6 * sine) / 4096) + 19 + 6 * index);
        timer -= 1150;
    }
    child = r_u32(object + 92);
    if (child)
    {
        value = r_u32(object + 96);
        scalar = (uint32)((sint32)(20 * value) >> 8);
        count = r_u32(child + 80);
        index = 0;
        while (index < count)
        {
            random = RandomRange(scalar);
            child = r_u32(object + 92);
            destination = r_u32(child + 72) + 8 * index;
            w_u32(destination, scalar + random);
            child = r_u32(object + 92);
            count = r_u32(child + 80);
            ++index;
        }
        random = RandomRange(1024);
        w_u16(r_u32(object + 92) + 96, random);
        child = r_u32(object + 92);
        RingEffect::Attach(child).SetPosition(input_position);
    }
    scalar = 9;
    MultiplyScalarVector(scaled, &scalar, direction);
    AddVectors(endpoint, input_position, scaled);
    ReadBeamPosition(guest_position, native_position, temporary);
    OffsetPositionByAngles(temporary, (const uint16 *)input_angles);
    StoreBeamVector(0x800ED638, temporary);
    StoreBeamVector(0x800ED644, endpoint);
    InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
    w_u32(0x800FF974, 1);
    QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
    ReadBeamPosition(guest_position, native_position, position);
    filter = r_u32(object + 44);
    w_u32(0x800FF974, 0);
    hit = IntersectObjectSpheres(position, endpoint, intersection, r_u32(filter), 0);
    wall = r_u32(0x800ED6A0);
    if (wall)
    {
        endpoint[0] = r_u32(0x800ED6A4);
        endpoint[1] = r_u32(0x800ED6A8);
        endpoint[2] = r_u32(0x800ED6AC);
        if (!hit)
            wall_hit = 1;
        else
        {
            distance = MeasurePositionDistance(input_position, (const uint32 *)psx_addr(hit + 4, 12));
            other_distance = MeasurePositionDistance(input_position, endpoint);
            if (distance >= other_distance)
                wall_hit = 1;
            else
                object_hit = 1;
        }
    }
    else if (hit)
        object_hit = 1;
    if (object_hit)
    {
        endpoint[0] = r_u32(hit + 4);
        endpoint[1] = r_u32(hit + 8);
        endpoint[2] = r_u32(hit + 12);
        DamageTarget(hit, normalized);
    }
    if (wall_hit)
    {
        target = r_u32(0x800ED6A0);
        material = r_u32(0x800ED6B8);
        damage = r_u32(object + 40);
        DamageLevelObject(target, material, damage);
        target = r_u32(0x800ED6A0);
        if (r_u16(target) & 16)
            DamageTarget(target, normalized);
    }
    if (object_hit || wall_hit)
    {
        distance = MeasureEuclideanDistance(input_position, endpoint);
        segment = 0;
        if ((sint32)distance >= 101)
        {
            segment = DivideBeamScalar((sint32)distance, (sint32)r_u32(object + 72));
            if (segment >= 10)
                segment = 9;
            child = r_u32(object + 60);
            source = r_u32(child + 80);
            offset = 36 * (uint32)segment;
            left = source + offset + (segment < 9 ? 36 : 0);
            right = source + offset - (segment < 9 ? 0 : 36);
            SubtractVectors(temporary, (const uint32 *)psx_addr(left, 12), (const uint32 *)psx_addr(right, 12));
            final[0] = temporary[0];
            final[1] = temporary[1];
            final[2] = temporary[2];
            value = r_u32(object + 72);
            scalar = distance - (uint32)segment * value;
            MultiplyVector(scaled, final, &scalar);
            DivideVector(temporary, scaled, (const uint32 *)psx_addr(object + 72, 4));
            final[0] = temporary[0];
            final[1] = temporary[1];
            final[2] = temporary[2];
            source = r_u32(r_u32(object + 60) + 80) + 36 * (uint32)segment;
            AddVectors(final, (const uint32 *)psx_addr(source, 12));
        }
        else
        {
            final[0] = endpoint[0];
            final[1] = endpoint[1];
            final[2] = endpoint[2];
        }
        for (index = (uint32)segment; (sint32)index < 10; ++index)
        {
            source = r_u32(r_u32(object + 60) + 80) + 36 * index;
            StoreBeamVector(source, final);
            destination = r_u32(r_u32(object + 64) + 80) + 36 * index;
            StoreBeamVector(destination, final);
        }
        value = r_u32(object + 68) + 1;
        w_u32(object + 68, value);
        if (value & 1)
        {
            child = r_u32(object + 60);
            source = r_u32(child + 80) + 36 * (uint32)segment;
            red = r_u8(source + 12);
            green = r_u8(source + 13);
            blue = r_u8(source + 14);
            SpawnExplosionRing(final, 50, red, green, blue, 3, 1, 100);
        }
    }
    return 1;
}

uint32 ParticleBeam::Update(const sint32 *, const sint16 *)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 sound;
    FUNCTION_MARKER(0x800284A4, "SLUS_003.73");
    sound = r_u32(object + 52);
    if (sound)
        StopSoundVoices(sound);
    w_u32(object + 52, 0);
    ClearChildren();
    return 0;
}

uint32 ParticleBeam::ReadLight( uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800284E8, "SLUS_003.73");
    if (!r_u32(object + 60))
        return 0;
    if (ReadWeaponLightRandom() >= 16384)
        return 0;
    StoreTimedWeaponLight(red, r_u8(object + 80));
    StoreTimedWeaponLight(green, r_u8(object + 81));
    StoreTimedWeaponLight(blue, r_u8(object + 82));
    return 1;
}

RipLaser::RipLaser(uint32 list) : WeaponNode(list, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80028578, "SLUS_003.73");
    w_u32(object, 0x800A1430);
    w_u32(object + 4, WPN_RIP_LASER);
    SetColors(64, 32, 128, 255, 255, 255);
}

uint32 RipLaser::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    uint32 sound;
    FUNCTION_MARKER(0x800285DC, "SLUS_003.73");
    sound = r_u32(object + 52);
    w_u32(object, 0x800A1430);
    if (sound)
        StopSoundVoices(sound);
    w_u32(object + 52, 0);
    ClearChildren();
    WeaponNode::Destroy(0);
    result = reason & 1;
    if (reason & 1)
        result = ReleaseWeaponStorage(object);
    return result;
}

uint32 RipLaser::SetColors(uint32 first_red, uint32 first_green, uint32 first_blue, uint32 second_red, uint32 second_green, uint32 second_blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80028650, "SLUS_003.73");
    w_u8(object + 92, first_red);
    w_u8(object + 93, first_green);
    w_u8(object + 94, first_blue);
    w_u8(object + 95, second_red);
    w_u8(object + 96, second_green);
    w_u8(object + 97, second_blue);
    return second_red;
}

void RipLaser::ClearChildren()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x80028678, "SLUS_003.73");
    child = r_u32(object + 60);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    child = r_u32(object + 64);
    w_u32(object + 60, 0);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    w_u32(object + 64, 0);
}

uint32 RipLaser::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 guest_position = GuestMemoryAddress(input_position), guest_angles = GuestMemoryAddress(input_angles);
    const uint32 *native_position = guest_position ? 0 : input_position;
    const sint16 *native_angles = guest_angles ? 0 : input_angles;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, red, green, blue, value, selected, head, hit, position[3], endpoint[3];
    uint32 offset, index, phase, sine, product, storage, step[3], delta[3], axis1[3], axis2[3];
    uint32 scaled1[3], scaled2[3], intermediate[3], final[3], scalar, divisor = 9;
    sint16 angles[3];
    FUNCTION_MARKER(0x800286F4, "SLUS_003.73");
    ConsumeAmmo(4);
    if (!r_u32(object + 52))
        w_u32(object + 52, PlaySoundEffect(9, 8192, 0));
    if (!r_u32(object + 60))
    {
        child = AllocateEffectStorage(84);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
            new (dram_addr) BeamSegments(10, 1);
        }
        red = r_u8(object + 92);
        green = r_u8(object + 93);
        blue = r_u8(object + 94);
        w_u32(object + 60, child);
        BeamSegments::Attach(child).SetColors(red, green, blue);
        w_u8(r_u32(object + 60) + 66, 1);
        child = AllocateEffectStorage(84);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
            new (dram_addr) BeamSegments(10, 0);
        }
        red = r_u8(object + 95);
        green = r_u8(object + 96);
        blue = r_u8(object + 97);
        w_u32(object + 64, child);
        BeamSegments::Attach(child).SetColors(red, green, blue);
        BeamSegments::Attach(r_u32(object + 64)).SetSize(10);
        w_u8(r_u32(object + 64) + 66, 1);
    }
    value = r_u32(object + 80);
    if (value)
        w_u32(object + 80, value - 1);
    head = r_u32(r_u32(object + 44));
    selected = SelectWeaponTarget(input_position, input_angles, head, 3000, 512, 0);
    if (selected)
    {
        ReadRipLaserPosition(guest_position, native_position, position);
        OffsetPositionByAngles(position, (const uint16 *)input_angles);
        w_u32(0x800ED638, position[0]);
        w_u32(0x800ED640, position[2]);
        w_u32(0x800ED63C, position[1]);
        w_u32(0x800ED644, r_u32(selected + 4));
        w_u32(0x800ED648, r_u32(selected + 8));
        w_u32(0x800ED64C, r_u32(selected + 12));
        InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
        w_u32(0x800FF974, 1);
        QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
        hit = r_u32(0x800ED6A0);
        w_u32(0x800FF974, 0);
        if (hit)
        {
            w_u32(object + 68, r_u32(0x800ED6A4));
            w_u32(object + 72, r_u32(0x800ED6A8));
            selected = 0;
            w_u32(object + 76, r_u32(0x800ED6AC));
        }
        else
        {
            endpoint[0] = r_u32(selected + 4);
            endpoint[1] = r_u32(selected + 8);
            endpoint[2] = r_u32(selected + 12);
            w_u32(object + 68, endpoint[0]);
            w_u32(object + 72, endpoint[1]);
            w_u32(object + 76, endpoint[2]);
        }
        w_u32(object + 84, 1);
    }
    if (!selected && !r_u32(object + 80))
    {
        if (RandomRange(2))
        {
            value = RandomRange(50);
            angles[0] = (sint16)(ReadRipLaserAngle(guest_angles, native_angles, 0) - value);
        }
        else
        {
            value = RandomRange(40);
            angles[0] = (sint16)(ReadRipLaserAngle(guest_angles, native_angles, 0) + value);
        }
        value = RandomRange(321);
        angles[2] = 0;
        angles[1] = (sint16)(ReadRipLaserAngle(guest_angles, native_angles, 1) + value - 160);
        BuildDirectionVector(endpoint, 3000, (const uint16 *)angles);
        AddVectors(endpoint, input_position);
        ReadRipLaserPosition(guest_position, native_position, position);
        OffsetPositionByAngles(position, (const uint16 *)input_angles);
        w_u32(0x800ED638, position[0]);
        w_u32(0x800ED63C, position[1]);
        w_u32(0x800ED640, position[2]);
        w_u32(0x800ED644, endpoint[0]);
        w_u32(0x800ED648, endpoint[1]);
        w_u32(0x800ED64C, endpoint[2]);
        InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
        w_u32(0x800FF974, 1);
        QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
        hit = r_u32(0x800ED6A0);
        w_u32(0x800FF974, 0);
        if (hit)
        {
            w_u32(object + 68, r_u32(0x800ED6A4));
            w_u32(object + 72, r_u32(0x800ED6A8));
            value = r_u32(0x800ED6AC);
            w_u32(object + 84, 1);
            w_u32(object + 76, value);
        }
        else
        {
            w_u32(object + 68, endpoint[0]);
            w_u32(object + 72, endpoint[1]);
            w_u32(object + 76, endpoint[2]);
            w_u32(object + 84, 0);
        }
        w_u32(object + 80, RandomRange(3) + 2);
    }
    if (selected)
        DamageTarget(selected);
    hit = r_u32(0x800ED6A0);
    if (hit)
    {
        value = r_u32(0x800ED6B8);
        product = r_u32(object + 40);
        DamageLevelObject(hit, value, product);
        hit = r_u32(0x800ED6A0);
        if (r_u16(hit) & 16)
            DamageTarget(hit);
    }
    phase = 0;
    offset = 0;
    for (index = 0; index < 10; ++index)
    {
        value = RandomRange(256);
        storage = r_u32(r_u32(object + 64) + 80);
        w_u8(storage + offset + 12, value);
        storage = r_u32(r_u32(object + 64) + 80);
        w_u8(storage + offset + 13, value);
        storage = r_u32(r_u32(object + 64) + 80);
        w_u8(storage + offset + 14, value);
        value = r_u32(0x800FF2F0);
        sine = (uint32)(sint32)(sint16)r_u16(0x800F863C + ((900 * value + phase) & 4095) * 4);
        child = r_u32(object + 60);
        storage = r_u32(child + 80);
        product = sine * 6;
        w_u16(storage + offset + 16, (uint32)((sint32)product / 4096) + 10 + index * 5);
        phase -= 1150;
        offset += 36;
    }
    ReadRipLaserPosition(guest_position, native_position, position);
    SubtractVectors(delta, (const uint32 *)psx_addr(object + 68, 12), input_position);
    DivideVector(step, delta, &divisor);
    phase = 0;
    offset = 0;
    for (index = 0; index < 10; ++index)
    {
        angles[0] = (sint16)ReadRipLaserAngle(guest_angles, native_angles, 0);
        angles[1] = (sint16)(ReadRipLaserAngle(guest_angles, native_angles, 1) + 1024);
        angles[2] = (sint16)ReadRipLaserAngle(guest_angles, native_angles, 2);
        BuildDirectionVector(axis1, 1, (const uint16 *)angles);
        angles[0] = (sint16)(ReadRipLaserAngle(guest_angles, native_angles, 0) + 1024);
        angles[1] = (sint16)ReadRipLaserAngle(guest_angles, native_angles, 1);
        BuildDirectionVector(axis2, 1, (const uint16 *)angles);
        sine = (uint32)(sint32)(sint16)r_u16(0x800F863C + (phase & 4095) * 4);
        product = (RandomRange(301) - 150) * sine;
        scalar = (uint32)((sint32)product / 4096);
        MultiplyVector(scaled1, axis1, &scalar);
        AddVectors(intermediate, position, scaled1);
        product = (RandomRange(261) - 130) * sine;
        scalar = (uint32)((sint32)product / 4096);
        MultiplyVector(scaled2, axis2, &scalar);
        AddVectors(final, intermediate, scaled2);
        storage = r_u32(r_u32(object + 60) + 80) + offset;
        w_u32(storage, final[0]);
        w_u32(storage + 4, final[1]);
        w_u32(storage + 8, final[2]);
        storage = r_u32(r_u32(object + 64) + 80) + offset;
        w_u32(storage, final[0]);
        w_u32(storage + 4, final[1]);
        w_u32(storage + 8, final[2]);
        AddVectors(position, step);
        phase += 227;
        offset += 36;
    }
    value = r_u32(object + 88) + 1;
    w_u32(object + 88, value);
    if (value & 1)
    {
        child = AllocateEffectStorage(124);
        if (child)
        {
            red = r_u8(object + 92);
            green = r_u8(object + 93);
            blue = r_u8(object + 94);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
            new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(object + 68, 12), 5, 255, 255, 255, 1, red, green, blue, 1, 60, 0, 1, 50, 80, 25, 40, 10, 5);
        }
    }
    return 1;
}

uint32 RipLaser::Update(const sint32 *, const sint16 *)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 sound;
    FUNCTION_MARKER(0x80028EE4, "SLUS_003.73");
    sound = r_u32(object + 52);
    if (sound)
        StopSoundVoices(sound);
    w_u32(object + 52, 0);
    ClearChildren();
    return 0;
}

uint32 RipLaser::ReadLight( uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80028F28, "SLUS_003.73");
    if (!r_u32(object + 60))
        return 0;
    if (ReadWeaponLightRandom() >= 16384)
        return 0;
    StoreTimedWeaponLight(red, r_u8(object + 92));
    StoreTimedWeaponLight(green, r_u8(object + 93));
    StoreTimedWeaponLight(blue, r_u8(object + 94));
    return 1;
}

Grenades::Grenades(uint32 list) : WeaponNode(list, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002A5C0, "SLUS_003.73");
    w_u32(object, 0x800A13B8);
    w_u32(object + 4, WPN_GRENADES);
    w_u32(object + 60, 15);
}

uint32 Grenades::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8002A608, "SLUS_003.73");
    w_u32(object, 0x800A13B8);
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(object);
    return result;
}

uint32 Grenades::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 guest_position = GuestMemoryAddress(input_position), guest_angles = GuestMemoryAddress(input_angles);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, position[3], packed, projectile, damage, radius, duration;
    uint16 angles[3];
    FUNCTION_MARKER(0x8002A65C, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (timer)
    {
        w_u32(object + 64, timer - 1);
        return 0;
    }
    PlaySoundEffect(23, 8192, 0);
    if (guest_position)
    {
        position[0] = r_u32(guest_position);
        position[1] = r_u32(guest_position + 4);
        position[2] = r_u32(guest_position + 8);
    }
    else
        memcpy(position, input_position, 12);
    OffsetPositionByAngles(position, (const uint16 *)input_angles);
    if (!guest_angles)
    {
        memcpy(&packed, input_angles, 4);
        memcpy(angles + 2, input_angles + 2, 2);
    }
    else
    {
        packed = ReadAnglesWord(guest_angles);
        angles[2] = r_u16(guest_angles + 4);
    }
    memcpy(angles, &packed, 4);
    angles[0] = (uint16)(angles[0] - 420);
    projectile = AllocateZeroedObject(400);
    if (projectile)
    {
        damage = r_u32(object + 40);
        radius = (uint32)(sint32)(sint16)r_u16(0x800EC804);
        void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(LaunchedProjectile));
        new (dram_addr) LaunchedProjectile(position, (const sint16 *)angles, 100, damage, radius, 0x800FF4E8, 0, 0, 0);
    }
    ConsumeAmmo(66);
    if ((sint32)r_u32(object + 56) < 66)
        w_u32(object + 56, 0);
    duration = r_u32(object + 60);
    w_u32(object + 64, duration);
    return 1;
}

uint32 Grenades::Update(const sint32 *, const sint16 *)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, result;
    FUNCTION_MARKER(0x8002A798, "SLUS_003.73");
    timer = r_u32(object + 64);
    result = timer - 1;
    if (timer)
        w_u32(object + 64, result);
    return result;
}

uint32 Grenades::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer;
    FUNCTION_MARKER(0x8002A7B4, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (!timer)
        return 0;
    if ((sint32)(r_u32(object + 60) - timer) >= 5)
        return 0;
    StoreTimedWeaponLight(red, 255);
    StoreTimedWeaponLight(green, 255);
    StoreTimedWeaponLight(blue, 0);
    return 1;
}

Rockets::Rockets(uint32 list) : WeaponNode(list, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002B4EC, "SLUS_003.73");
    w_u32(object, 0x800A13E0);
    w_u32(object + 4, WPN_ROCKETS);
    w_u32(object + 60, 15);
}

uint32 Rockets::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8002B534, "SLUS_003.73");
    w_u32(object, 0x800A13E0);
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(object);
    return result;
}

uint32 Rockets::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, projectile, damage, duration;
    FUNCTION_MARKER(0x8002B588, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (timer)
    {
        w_u32(object + 64, timer - 1);
        return 0;
    }
    projectile = AllocateZeroedObject(380);
    if (projectile)
    {
        damage = r_u32(object + 40);
        void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(BallisticProjectile));
        new (dram_addr) BallisticProjectile(input_position, input_angles, damage, 256, 0x800FF4E8);
    }
    ConsumeAmmo(50);
    if ((sint32)r_u32(object + 56) < 50)
        w_u32(object + 56, 0);
    duration = r_u32(object + 60);
    w_u32(object + 64, duration);
    return 1;
}

uint32 Rockets::Update(const sint32 *, const sint16 *)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, result;
    FUNCTION_MARKER(0x8002B640, "SLUS_003.73");
    timer = r_u32(object + 64);
    result = timer - 1;
    if (timer)
        w_u32(object + 64, result);
    return result;
}

uint32 Rockets::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 timer;
    FUNCTION_MARKER(0x8002B65C, "SLUS_003.73");
    timer = r_u32(object + 64);
    if (!timer)
        return 0;
    if ((sint32)(r_u32(object + 60) - timer) >= 5)
        return 0;
    StoreTimedWeaponLight(red, 255);
    StoreTimedWeaponLight(green, 255);
    StoreTimedWeaponLight(blue, 0);
    return 1;
}

uint32 WeaponNode::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    FUNCTION_MARKER(0x8002BC98, "SLUS_003.73");
    return 0;
}

uint32 WeaponNode::Update(const sint32 *, const sint16 *)
{
    FUNCTION_MARKER(0x8002BCA0, "SLUS_003.73");
    return 0;
}

uint32 FireInactiveWeapon()
{
    FUNCTION_MARKER(0x8002BCA8, "SLUS_003.73");
    return 0;
}

uint32 WeaponNodeBase::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002BCC4, "SLUS_003.73");
    w_u32(object, 0x800A15D8);
    if (reason & 1)
        return ReleaseWeaponStorage(object);
    return 0x800A15D8;
}

uint32 ReleaseWeaponStorage(uint32 allocation)
{
    FUNCTION_MARKER(0x8002FF34, "SLUS_003.73");
    return ReleaseHeapBlock(allocation);
}

extern "C" uint32 sub_80027720(uint32 object, uint32 list, uint32 range, uint32 mode, uint32 effect_scale, uint32 sound)
{
    FUNCTION_MARKER(0x80027720, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(ParticleBeam));
    new (native_instance_storage) ParticleBeam(list, range, mode, effect_scale, sound);
    return object;
}

extern "C" uint32 sub_80027804(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80027804, "SLUS_003.73");
    return ParticleBeam::Attach(object).ParticleBeam::Destroy(reason);
}

extern "C" uint32 sub_80027878(uint32 object, uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2, uint32 red3, uint32 green3, uint32 blue3, uint32 red4, uint32 green4, uint32 blue4)
{
    FUNCTION_MARKER(0x80027878, "SLUS_003.73");
    return ParticleBeam::Attach(object).SetColors(red1, green1, blue1, red2, green2, blue2, red3, green3, blue3, red4, green4, blue4);
}

extern "C" void sub_800278D0(uint32 object)
{
    FUNCTION_MARKER(0x800278D0, "SLUS_003.73");
    ParticleBeam::Attach(object).ClearChildren();
}

extern "C" void sub_800284A4(uint32 object)
{
    FUNCTION_MARKER(0x800284A4, "SLUS_003.73");
    ParticleBeam::Attach(object).ParticleBeam::Update(0, 0);
}

extern "C" uint32 sub_80022A24(uint32 object, uint32 list, uint32 mode)
{
    FUNCTION_MARKER(0x80022A24, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(WeaponNode));
    new (native_instance_storage) WeaponNode(list, mode);
    return object;
}

extern "C" uint32 sub_80022A48(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80022A48, "SLUS_003.73");
    return WeaponNode::Attach(object).WeaponNode::Destroy(reason);
}

extern "C" void sub_80022ABC(uint32 object)
{
    FUNCTION_MARKER(0x80022ABC, "SLUS_003.73");
    WeaponNode::Attach(object).ClearChild();
}

extern "C" uint32 sub_80022B34(uint32 object, uint32 amount)
{
    FUNCTION_MARKER(0x80022B34, "SLUS_003.73");
    return WeaponNode::Attach(object).ConsumeAmmo(amount);
}

extern "C" uint32 sub_80025070(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x80025070, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(HomingMissiles));
    new (native_instance_storage) HomingMissiles(list);
    return object;
}

extern "C" uint32 sub_800250B8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800250B8, "SLUS_003.73");
    return HomingMissiles::Attach(object).HomingMissiles::Destroy(reason);
}

extern "C" uint32 sub_800251E8(uint32 object)
{
    FUNCTION_MARKER(0x800251E8, "SLUS_003.73");
    return HomingMissiles::Attach(object).HomingMissiles::Update(0, 0);
}

extern "C" uint32 sub_800252CC(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x800252CC, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(PulseLaser));
    new (native_instance_storage) PulseLaser(list);
    return object;
}

extern "C" uint32 sub_80025314(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80025314, "SLUS_003.73");
    return PulseLaser::Attach(object).PulseLaser::Destroy(reason);
}

extern "C" void sub_800254E8(uint32 object)
{
    FUNCTION_MARKER(0x800254E8, "SLUS_003.73");
    PulseLaser::Attach(object).PulseLaser::Update(0, 0);
}

extern "C" uint32 sub_80028578(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x80028578, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(RipLaser));
    new (native_instance_storage) RipLaser(list);
    return object;
}

extern "C" uint32 sub_800285DC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800285DC, "SLUS_003.73");
    return RipLaser::Attach(object).RipLaser::Destroy(reason);
}

extern "C" uint32 sub_80028650(uint32 object, uint32 first_red, uint32 first_green, uint32 first_blue, uint32 second_red, uint32 second_green, uint32 second_blue)
{
    FUNCTION_MARKER(0x80028650, "SLUS_003.73");
    return RipLaser::Attach(object).SetColors(first_red, first_green, first_blue, second_red, second_green, second_blue);
}

extern "C" void sub_80028678(uint32 object)
{
    FUNCTION_MARKER(0x80028678, "SLUS_003.73");
    RipLaser::Attach(object).ClearChildren();
}

extern "C" void sub_80028EE4(uint32 object)
{
    FUNCTION_MARKER(0x80028EE4, "SLUS_003.73");
    RipLaser::Attach(object).RipLaser::Update(0, 0);
}

extern "C" uint32 sub_8002A5C0(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x8002A5C0, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Grenades));
    new (native_instance_storage) Grenades(list);
    return object;
}

extern "C" uint32 sub_8002A608(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002A608, "SLUS_003.73");
    return Grenades::Attach(object).Grenades::Destroy(reason);
}

extern "C" uint32 sub_8002A798(uint32 object)
{
    FUNCTION_MARKER(0x8002A798, "SLUS_003.73");
    return Grenades::Attach(object).Grenades::Update(0, 0);
}

extern "C" uint32 sub_8002B4EC(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x8002B4EC, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Rockets));
    new (native_instance_storage) Rockets(list);
    return object;
}

extern "C" uint32 sub_8002B534(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002B534, "SLUS_003.73");
    return Rockets::Attach(object).Rockets::Destroy(reason);
}

extern "C" uint32 sub_8002B640(uint32 object)
{
    FUNCTION_MARKER(0x8002B640, "SLUS_003.73");
    return Rockets::Attach(object).Rockets::Update(0, 0);
}

extern "C" uint32 sub_8002510C(uint32 object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x8002510C, "SLUS_003.73");
    return HomingMissiles::Attach(object).HomingMissiles::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 sub_8002A65C(uint32 object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x8002A65C, "SLUS_003.73");
    return Grenades::Attach(object).Grenades::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_weapon_fire_2A65C(uint32 object, const sint32 *position, const sint16 *angles)
{
    return Grenades::Attach(object).Grenades::Fire((const uint32 *)position, angles);
}

extern "C" uint32 sub_80025380(uint32 object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x80025380, "SLUS_003.73");
    return PulseLaser::Attach(object).PulseLaser::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_weapon_fire_25380(uint32 object, const sint32 *position, const sint16 *angles)
{
    return PulseLaser::Attach(object).PulseLaser::Fire((const uint32 *)position, angles);
}

extern "C" uint32 sub_8002B588(uint32 object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x8002B588, "SLUS_003.73");
    return Rockets::Attach(object).Rockets::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_weapon_fire_2510C(uint32 object, const sint32 *position, const sint16 *angles)
{
    return HomingMissiles::Attach(object).HomingMissiles::Fire((const uint32 *)position, angles);
}

extern "C" uint32 apocalypse_weapon_fire_2B588(uint32 object, const sint32 *position, const sint16 *angles)
{
    return Rockets::Attach(object).Rockets::Fire((const uint32 *)position, angles);
}

extern "C" uint32 sub_800286F4(uint32 object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x800286F4, "SLUS_003.73");
    return RipLaser::Attach(object).RipLaser::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_weapon_fire_286F4(uint32 object, const sint32 *position, const sint16 *angles)
{
    return RipLaser::Attach(object).RipLaser::Fire((const uint32 *)position, angles);
}

extern "C" uint32 sub_80027978(uint32 object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x80027978, "SLUS_003.73");
    return ParticleBeam::Attach(object).ParticleBeam::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_beam_fire_27978(uint32 object, const uint32 position[3], const sint16 angles[3])
{
    return ParticleBeam::Attach(object).ParticleBeam::Fire(position, angles);
}

extern "C" uint32 sub_8002FF34(uint32 allocation)
{
    FUNCTION_MARKER(0x8002FF34, "SLUS_003.73");
    return ReleaseWeaponStorage(allocation);
}

extern "C" uint32 sub_8002BCA8(void)
{
    FUNCTION_MARKER(0x8002BCA8, "SLUS_003.73");
    return FireInactiveWeapon();
}

extern "C" uint32 sub_8002BCC4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002BCC4, "SLUS_003.73");
    return WeaponNodeBase::Attach(object).WeaponNodeBase::Destroy(reason);
}

extern "C" uint32 apocalypse_weapon_light_colors(uint32 weapon, uint8 *red_pointer, uint8 *green_pointer, uint8 *blue_pointer)
{
    uint8 *red = red_pointer, *green = green_pointer, *blue = blue_pointer;
    uint32 table, target, receiver;
    sint32 adjustment;
    table = r_u32(weapon);
    adjustment = (sint16)r_u16(table + 32);
    target = r_u32(table + 36);
    receiver = weapon + (uint32)adjustment;
    switch (target)
    {
        case 0x800243F8: return SpiralRayWeapon::Attach(receiver).SpiralRayWeapon::ReadLight(red, green, blue);
        case 0x80024FFC: return Flamethrower::Attach(receiver).Flamethrower::ReadLight(red, green, blue);
        case 0x8002349C: return MachineGun::Attach(receiver).MachineGun::ReadLight(red, green, blue);
        case 0x80025204: return HomingMissiles::Attach(receiver).HomingMissiles::ReadLight(red, green, blue);
        case 0x80025538: return PulseLaser::Attach(receiver).PulseLaser::ReadLight(red, green, blue);
        case 0x800284E8: return ParticleBeam::Attach(receiver).ParticleBeam::ReadLight(red, green, blue);
        case 0x80028F28: return RipLaser::Attach(receiver).RipLaser::ReadLight(red, green, blue);
        case 0x8002A7B4: return Grenades::Attach(receiver).Grenades::ReadLight(red, green, blue);
        case 0x8002B65C: return Rockets::Attach(receiver).Rockets::ReadLight(red, green, blue);
        case 0x8002BC98: return WeaponNode::Attach(receiver).WeaponNode::ReadLight(red, green, blue);
    }
    /* TODO Audit additional weapon lighting targets */
    GameDiagnostic("Unknown weapon lighting target=%08X receiver=%08X\n", target, receiver);
    abort();
    return 0;
}

extern "C" uint32 sub_8002349C(uint32 object, uint32 red_address, uint32 green_address, uint32 blue_address)
{
    FUNCTION_MARKER(0x8002349C, "SLUS_003.73");
    uint8 *red = (uint8 *)psx_addr(red_address, 1), *green = (uint8 *)psx_addr(green_address, 1), *blue = (uint8 *)psx_addr(blue_address, 1);
    return MachineGun::Attach(object).MachineGun::ReadLight(red, green, blue);
}

extern "C" uint32 sub_800284E8(uint32 object, uint32 red_address, uint32 green_address, uint32 blue_address)
{
    FUNCTION_MARKER(0x800284E8, "SLUS_003.73");
    uint8 *red = (uint8 *)psx_addr(red_address, 1), *green = (uint8 *)psx_addr(green_address, 1), *blue = (uint8 *)psx_addr(blue_address, 1);
    return ParticleBeam::Attach(object).ParticleBeam::ReadLight(red, green, blue);
}
