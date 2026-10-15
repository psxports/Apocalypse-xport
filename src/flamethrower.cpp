extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "flamethrower.h"
#include "audio_voice.h"
#include "angle_math.h"
#include "ribbon_segments.h"
#include "effect_sprite.h"
#include "native_object_storage.h"
#include "audio_session.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "object_collision_events.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include <new>
#include <string.h>

Flamethrower::Flamethrower(uint32 list, uint32 mode)
    : WeaponNode(list, mode)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80024428, "SLUS_003.73");
    w_u32(object, 0x800A14C8);
    w_u32(object + 4, WPN_FLAMETHROWER);
    Configure(3000);
}

uint32 Flamethrower::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80024470, "SLUS_003.73");
    w_u32(object, 0x800A14C8);
    ClearBeam();
    uint32 result = WeaponNode::Destroy(0);
    return (reason & 1) ? ReleaseWeaponStorage(object) : result;
}

// TODO Recover original units for configuration field68
uint32 Flamethrower::Configure(uint32 amount)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800244CC, "SLUS_003.73");
    uint32 result = (uint32)((sint32)amount / 20);
    w_u32(object + 68, result);
    return result;
}

void Flamethrower::ClearBeam()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, receiver, target, sound;
    FUNCTION_MARKER(0x800244F0, "SLUS_003.73");
    child = r_u32(object + 60);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
        w_u32(object + 60, 0);
    }
    sound = r_u32(object + 52);
    if (sound)
    {
        StopSoundVoices(sound);
        w_u32(object + 52, 0);
    }
}

// Original beam color attenuation helper
static uint32 ReadFlamethrowerShade(uint32 value)
{
    FUNCTION_MARKER(0x8002455C, "SLUS_003.73");
    if ((sint32)value < 0 || (sint32)value >= 2049)
        return 0;
    return 255 - ((255 * value) >> 11);
}

// Native helper retains the original signed DIV result words
static uint32 DivideFlamethrowerDistance(uint32 numerator, uint32 denominator)
{
    sint32 n = (sint32)numerator, d = (sint32)denominator;
    if (!d)
        return n < 0 ? 1 : (uint32)-1;
    if (n == (sint32)0x80000000 && d == -1)
        return 0x80000000;
    return (uint32)(n / d);
}

// Native input and output bridges preserve the original three-word copies
static void ReadFlamethrowerPoint(uint32 output[3], const uint32 *input)
{
    uint32 guest = GuestMemoryAddress(input), axis;
    for (axis = 0; axis < 3; ++axis)
        if (guest)
            output[axis] = r_u32(guest + 4 * axis);
        else
            memcpy(output + axis, (const uint8 *)input + 4 * axis, 4);
}

static void StoreFlamethrowerPoint(uint32 address, const uint32 input[3])
{
    w_u32(address, input[0]);
    w_u32(address + 4, input[1]);
    w_u32(address + 8, input[2]);
}

uint32 Flamethrower::Fire(const uint32 *position, const sint16 *angles)
{
    const uint32 weapon = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80024598, "SLUS_003.73");
    uint32 step[3], direction[3], point[3], endpoint[3], start[3], scaled[3];
    uint32 temporary[3], difference[3], impact[3], fireball_velocity[3];
    uint32 surface, buffer, value, shade, index, offset, previous, count, scale;
    uint32 target, list, object_hit = 0, geometry_hit = 0, distance = 0;
    uint32 first_distance, second_distance, receiver, table, damage, material, fragment;
    sint32 wave;
    ConsumeAmmo(4);
    scale = r_u32(weapon + 68);
    BuildDirectionVector(step, scale, (const uint16 *)angles);
    DivideVector(direction, step, (const uint32 *)psx_addr(weapon + 68, 4));
    surface = r_u32(weapon + 60);
    if (!surface)
    {
        surface = AllocateEffectStorage(108);
        if (surface)
        {
            void *dram_addr = AllocateNativeObjectStorage(surface, sizeof(RibbonSegments));
            new (dram_addr) RibbonSegments(20, 1);
        }
        w_u32(weapon + 60, surface);
        w_u8(surface + 66, 1);
        RibbonSegments::Attach(r_u32(weapon + 60)).SetMaterial(0x68B7B136);
        surface = r_u32(weapon + 60);
        w_u32(surface + 96, 1);
        for (index = 0; index < 3; ++index)
        {
            buffer = r_u32(r_u32(weapon + 60) + 104);
            w_u8(buffer + 12 + index, 0);
        }
        for (index = 1, offset = 36; index < 20; ++index, offset += 36)
        {
            value = (index << 10) / 10;
            for (count = 0; count < 3; ++count)
            {
                shade = ReadFlamethrowerShade(value);
                buffer = r_u32(r_u32(weapon + 60) + 104);
                w_u8(buffer + offset + 12 + count, shade);
            }
            value = LookupDistanceRoot(((value * (index << 10)) / 10) + 0x100000);
            for (count = 0; count < 3; ++count)
            {
                shade = ReadFlamethrowerShade(value);
                buffer = r_u32(r_u32(weapon + 60) + 104);
                w_u8(buffer + offset + 20 + count, shade);
            }
        }
        w_u32(r_u32(weapon + 60) + 92, 8);
        ReadFlamethrowerPoint(point, position);
        for (index = 0, offset = 0; index < 20; ++index, offset += 36)
        {
            buffer = r_u32(r_u32(weapon + 60) + 104);
            StoreFlamethrowerPoint(buffer + offset, point);
            AddVectors(point, step);
        }
        value = PlaySoundEffect(18, 8192, 0);
        w_u32(weapon + 52, value);
        surface = r_u32(weapon + 60);
    }
    index = r_u32(surface + 100) - 1;
    if ((sint32)index > 0)
    {
        offset = index * 36;
        previous = offset - 36;
        do
        {
            buffer = r_u32(r_u32(weapon + 60) + 104);
            target = buffer + offset;
            scale = 1;
            MultiplyVector(temporary, step, &scale);
            buffer = r_u32(r_u32(weapon + 60) + 104);
            AddVectors(point, (const uint32 *)psx_addr(buffer + previous, 12), temporary);
            StoreFlamethrowerPoint(target, point);
            --index;
            offset -= 36;
            previous -= 36;
        } while ((sint32)index > 0);
    }
    ReadFlamethrowerPoint(point, position);
    buffer = r_u32(r_u32(weapon + 60) + 104);
    StoreFlamethrowerPoint(buffer, point);
    AddVectors(point, step);
    value = 2100 - r_u32(0x800FF2F0) * 800;
    for (index = 2, offset = 72, count = 12; index < 20;
         ++index, offset += 36, count += 6, value += 1050)
    {
        wave = (sint16)r_u16(0x800F863C + 4 * (value & 4095));
        buffer = r_u32(r_u32(weapon + 60) + 104) + offset;
        wave = (6 * wave) / 4096;
        w_u16(buffer + 16, (uint32)wave + 12 + count);
        buffer = r_u32(r_u32(weapon + 60) + 104) + offset;
        w_u16(buffer + 18, r_u16(buffer + 16) + 50);
    }
    buffer = r_u32(r_u32(weapon + 60) + 104);
    w_u16(buffer + 16, 2);
    buffer = r_u32(r_u32(weapon + 60) + 104);
    w_u16(buffer + 18, 15);
    surface = r_u32(weapon + 60);
    value = r_u32(0x800FF2F0);
    buffer = r_u32(surface + 104);
    w_u16(buffer + 52, 15);
    wave = (sint16)r_u16(0x800F863C + ((960 * value) & 0x3FC0));
    surface = r_u32(weapon + 60);
    buffer = r_u32(surface + 104);
    w_u16(buffer + 54, (uint32)((9 * wave) / 4096) + 45);
    surface = r_u32(weapon + 60);
    value = r_u32(surface + 88);
    scale = 20;
    w_u32(surface + 88, value + 8);
    MultiplyScalarVector(scaled, &scale, step);
    AddVectors(endpoint, position, scaled);
    ReadFlamethrowerPoint(start, position);
    OffsetPositionByAngles(start, (const uint16 *)angles);
    StoreFlamethrowerPoint(0x800ED638, start);
    StoreFlamethrowerPoint(0x800ED644, endpoint);
    InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
    w_u32(0x800FF974, 1);
    QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
    ReadFlamethrowerPoint(point, position);
    list = r_u32(weapon + 44);
    w_u32(0x800FF974, 0);
    target = IntersectObjectSpheres(point, endpoint, start, r_u32(list), 0);
    if (!target)
    {
        list = r_u32(weapon + 48);
        if (list)
            target = IntersectObjectSpheres(point, endpoint, start, r_u32(list), 0);
    }
    if (target && target == r_u32(0x800FF5A0))
    {
        first_distance = MeasurePositionDistance((const uint32 *)psx_addr(target + 4, 12), position);
        scale = r_u32(weapon + 68);
        if (19 * scale < first_distance)
            target = 0;
    }
    if (r_u32(0x800ED6A0))
    {
        endpoint[0] = r_u32(0x800ED6A4);
        endpoint[1] = r_u32(0x800ED6A8);
        distance = r_u32(0x800ED678) - 100;
        endpoint[2] = r_u32(0x800ED6AC);
        if (!target)
            geometry_hit = 1;
        else
        {
            first_distance = MeasurePositionDistance(position, (const uint32 *)psx_addr(target + 4, 12));
            second_distance = MeasurePositionDistance(position, endpoint);
            if (first_distance >= second_distance)
                geometry_hit = 1;
            else
                object_hit = 1;
        }
    }
    else if (target)
        object_hit = 1;
    if (object_hit)
    {
        ReadFlamethrowerPoint(endpoint, (const uint32 *)psx_addr(target + 4, 12));
        distance = MeasureEuclideanDistance(position, (const uint32 *)psx_addr(target + 4, 12));
        table = r_u32(target + 68);
        damage = r_u32(weapon + 40);
        receiver = target + (uint32)(sint32)(sint16)r_u16(table + 48);
        value = r_u32(table + 52);
        apocalypse_object_virtual52_native(value, receiver, damage, (const sint32 *)direction, 5);
    }
    if (geometry_hit)
    {
        value = r_u32(0x800ED6A0);
        material = r_u32(0x800ED6B8);
        damage = r_u32(weapon + 40);
        DamageLevelObject(value, material, damage);
        value = r_u32(0x800ED6A0);
        if (r_u16(value) & 16)
        {
            if (r_u16(value + 58) != 205 || r_u32(weapon + 44) != 0x800FF5A0)
            {
                table = r_u32(value + 68);
                damage = r_u32(weapon + 40);
                receiver = value + (uint32)(sint32)(sint16)r_u16(table + 48);
                value = r_u32(table + 52);
                apocalypse_object_virtual52_native(value, receiver, damage, (const sint32 *)direction, 5);
            }
        }
    }
    if (object_hit || geometry_hit)
    {
        scale = r_u32(weapon + 68);
        index = DivideFlamethrowerDistance(distance, scale);
        offset = index * 36;
        buffer = r_u32(r_u32(weapon + 60) + 104);
        previous = (sint32)index < 19 ? offset : offset - 36;
        value = (sint32)index < 19 ? offset + 36 : offset;
        SubtractVectors(difference, (const uint32 *)psx_addr(buffer + value, 12), (const uint32 *)psx_addr(buffer + previous, 12));
        scale = r_u32(weapon + 68);
        value = distance - index * scale;
        MultiplyVector(scaled, difference, &value);
        DivideVector(impact, scaled, (const uint32 *)psx_addr(weapon + 68, 4));
        buffer = r_u32(r_u32(weapon + 60) + 104);
        AddVectors(impact, (const uint32 *)psx_addr(buffer + offset, 12));
        for (++index; (sint32)index < 20; ++index, offset += 36)
        {
            value = index * 36;
            buffer = r_u32(r_u32(weapon + 60) + 104);
            StoreFlamethrowerPoint(buffer + value, impact);
            buffer = r_u32(r_u32(weapon + 60) + 104);
            w_u16(buffer + value + 16, 0);
            buffer = r_u32(r_u32(weapon + 60) + 104);
            w_u16(buffer + value + 18, 0);
        }
    }
    else
        ReadFlamethrowerPoint(impact, (const uint32 *)psx_addr(r_u32(r_u32(weapon + 60) + 104) + 648, 12));
    value = r_u32(weapon + 64) + 1;
    w_u32(weapon + 64, value);
    if (value & 1)
    {
        fireball_velocity[0] = 0;
        value = RandomRange(20);
        fireball_velocity[1] = (0 - 10 - value) << 12;
        fireball_velocity[2] = 0;
        fragment = AllocateEffectStorage(112);
        if (fragment)
        {
            value = RandomRange(1200) + 2900;
            void *dram_addr = AllocateNativeObjectStorage(fragment, sizeof(MovingSprite));
            new (dram_addr) MovingSprite(impact, fireball_velocity, (const char *)psx_addr(0x800A12E0, 1), value, 0, 10);
        }
        w_u16(fragment + 64, 300);
        MovingSprite::Attach(fragment).AddFlags(8);
    }
    return 1;
}

uint32 Flamethrower::Update(const sint32 *, const sint16 *)
{
    FUNCTION_MARKER(0x80024FDC, "SLUS_003.73");
    ClearBeam();
    return 0;
}

// Native output bridge preserves the original RGB byte stores
static void StoreFlamethrowerLight(uint8 *output, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u8(address, value);
    else
        *output = (uint8)value;
}

uint32 Flamethrower::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80024FFC, "SLUS_003.73");
    if (!r_u32(object + 60))
        return 0;
    if (ReadWeaponLightRandom() >= 16384)
        return 0;
    StoreFlamethrowerLight(red, 255);
    StoreFlamethrowerLight(green, 128);
    StoreFlamethrowerLight(blue, 0);
    return 1;
}

Flamethrower &Flamethrower::Attach(uint32 object)
{
    return *static_cast<Flamethrower *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80024428(uint32 object, uint32 list, uint32 mode)
{
    FUNCTION_MARKER(0x80024428, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Flamethrower));
    new (dram_addr) Flamethrower(list, mode);
    return object;
}

extern "C" uint32 sub_80024470(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80024470, "SLUS_003.73");
    return Flamethrower::Attach(object).Flamethrower::Destroy(reason);
}

extern "C" uint32 sub_800244CC(uint32 object, uint32 amount)
{
    FUNCTION_MARKER(0x800244CC, "SLUS_003.73");
    return Flamethrower::Attach(object).Configure(amount);
}

extern "C" void sub_800244F0(uint32 object)
{
    FUNCTION_MARKER(0x800244F0, "SLUS_003.73");
    Flamethrower::Attach(object).ClearBeam();
}

extern "C" void sub_80024FDC(uint32 object)
{
    FUNCTION_MARKER(0x80024FDC, "SLUS_003.73");
    Flamethrower::Attach(object).Flamethrower::Update(0, 0);
}

extern "C" uint32 sub_80024FFC(uint32 object, uint32 red_address, uint32 green_address, uint32 blue_address)
{
    FUNCTION_MARKER(0x80024FFC, "SLUS_003.73");
    return Flamethrower::Attach(object).Flamethrower::ReadLight((uint8 *)psx_addr(red_address, 1), (uint8 *)psx_addr(green_address, 1), (uint8 *)psx_addr(blue_address, 1));
}

extern "C" uint32 sub_8002455C(uint32 value)
{
    FUNCTION_MARKER(0x8002455C, "SLUS_003.73");
    return ReadFlamethrowerShade(value);
}

extern "C" uint32 sub_80024598(uint32 weapon, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x80024598, "SLUS_003.73");
    return Flamethrower::Attach(weapon).Flamethrower::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_beam_24598_native(uint32 weapon, const void *position, const void *angles)
{
    return Flamethrower::Attach(weapon).Flamethrower::Fire((const uint32 *)position, (const sint16 *)angles);
}
