#include "game_sdk_bindings.h"
#include "items.h"
#include "plague.h"
#include "the_beast.h"
#include "war.h"
#include "death.h"
#include "scripted_level_object.h"
#include "level_hazards.h"
#include "record_scripts.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "object_damage.h"
#include "object_fragments.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "glow_effect.h"
#include "native_object_storage.h"
#include "audio_session.h"
#include "game_random.h"
#include "vector_distance.h"
#include "effect_lifecycle.h"
#include "model_resources.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

void DamageLevelObject(uint32 object, uint32 polygon, sint32 damage)
{
    uint32 node, bank, index, table, descriptor, flags, health, remaining;
    FUNCTION_MARKER(0x8001E740, "SLUS_003.73");
    if (!object)
        return;
    if (r_u16(object) & 1)
        return;
    node = r_u32(0x800FF794);
    if (!node)
        return;
    while (node != object)
    {
        node = r_u32(node + 28);
        if (!node)
            return;
    }
    bank = r_u8(object + 27);
    index = r_u16(object + 22);
    table = r_u32(0x800EAEF8 + bank * 64 + 16);
    descriptor = r_u32(table + index * 4);
    flags = r_u32(descriptor);
    health = flags >> 24;
    if (flags & 2)
    {
        remaining = health;
        if (health || damage == 65535)
        {
            FlashLevelObject(object, 255, 255, 255, 3);
            remaining = health - (uint32)damage;
            if (damage >= (sint32)health)
            {
                remaining = 0;
                DestroyLevelObject(object);
            }
            flags = r_u32(descriptor);
        }
        w_u32(descriptor, (flags & 0xFFFFFF) | (remaining << 24));
    }
    else if (polygon)
        FragmentLevelPolygon(object, polygon, 1, 1, 1, 1, 1);
}

extern "C" void sub_8001E740(uint32 object, uint32 polygon, uint32 damage)
{
    FUNCTION_MARKER(0x8001E740, "SLUS_003.73");
    DamageLevelObject(object, polygon, (sint32)damage);
}

// Native helper models the original signed DIV carriers
static uint32 DivideFlashScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 InitializeObjectFlashes()
{
    uint32 entry = 0x800F2610, next = 0x800F2628, result;
    FUNCTION_MARKER(0x8001BAC0, "SLUS_003.73");
    w_u32(0x800FF1D4, 0);
    w_u32(0x800FF1D8, entry);
    do
    {
        w_u32(entry + 20, (sint32)entry < (sint32)0x800F3DF8 ? next : 0);
        w_u32(entry + 16, 0);
        entry += 24;
        result = (uint32)((sint32)entry < (sint32)0x800F3E10);
        next += 24;
    } while (result);
    return result;
}

extern "C" uint32 sub_8001BAC0(void)
{
    FUNCTION_MARKER(0x8001BAC0, "SLUS_003.73");
    return InitializeObjectFlashes();
}

void ReleaseObjectFlash(uint32 packet)
{
    uint32 current, previous = 0, next, free_head;
    FUNCTION_MARKER(0x8001BB14, "SLUS_003.73");
    current = r_u32(0x800FF1D4);
    while (current)
    {
        next = r_u32(current + 20);
        if (current == packet)
        {
            free_head = r_u32(0x800FF1D8);
            w_u32(0x800FF1D8, current);
            w_u32(current + 20, free_head);
            if (previous)
                w_u32(previous + 20, next);
            else
                w_u32(0x800FF1D4, next);
            return;
        }
        previous = current;
        current = next;
    }
}

void UpdateObjectFlashes()
{
    uint32 packet, next, duration, first, fraction, red_step, blue_step, red, green, blue, color, object, flags;
    FUNCTION_MARKER(0x8001BB68, "SLUS_003.73");
    packet = r_u32(0x800FF1D4);
    while (packet)
    {
        duration = r_u8(packet + 13);
        next = r_u32(packet + 20);
        if (duration)
        {
            first = r_u16(packet);
            fraction = r_u8(packet + 10);
            red_step = r_u16(packet + 4);
            blue_step = r_u16(packet + 8);
            red = (fraction | (first << 8)) + red_step;
            w_u8(packet + 10, red);
            red = (red >> 8) & 255;
            fraction = r_u8(packet + 11);
            green = r_u16(packet + 6);
            green = (fraction | (first & 0xFF00)) + green;
            w_u8(packet + 11, green);
            green &= 0xFF00;
            first = r_u32(packet);
            fraction = r_u8(packet + 12);
            blue = (fraction | ((first >> 8) & 0xFF00)) + blue_step;
            w_u8(packet + 12, blue);
            blue = (uint32)(sint32)(sint8)((blue >> 8) & 255);
            object = r_u32(packet + 16);
            color = (blue << 16) | green | red;
            w_u32(packet, color);
            w_u32(object + 32, color);
            duration = r_u8(packet + 13);
            w_u8(packet + 13, duration - 1);
        }
        else
        {
            object = r_u32(packet + 16);
            flags = r_u16(object);
            w_u16(object, flags & 0xFBFF);
            ReleaseObjectFlash(packet);
        }
        packet = next;
    }
}

extern "C" void sub_8001BB14(uint32 packet)
{
    FUNCTION_MARKER(0x8001BB14, "SLUS_003.73");
    ReleaseObjectFlash(packet);
}

extern "C" void sub_8001BB68(void)
{
    FUNCTION_MARKER(0x8001BB68, "SLUS_003.73");
    UpdateObjectFlashes();
}

uint32 AcquireObjectFlash(uint32 object)
{
    uint32 packet = 0, next, active;
    FUNCTION_MARKER(0x8001BC58, "SLUS_003.73");
    if (!r_u32(0x800FF1D8))
        return 0;
    if (r_u16(object) & 1024)
    {
        packet = r_u32(0x800FF1D4);
        while (packet)
        {
            if (r_u32(packet + 16) == object)
                break;
            packet = r_u32(packet + 20);
        }
    }
    if (!packet)
    {
        packet = r_u32(0x800FF1D8);
        active = r_u32(0x800FF1D4);
        next = r_u32(packet + 20);
        w_u32(packet + 20, active);
        w_u32(0x800FF1D4, packet);
        w_u32(0x800FF1D8, next);
    }
    return packet;
}

uint32 FlashLevelObject(uint32 object, uint32 red, uint32 green, uint32 blue, uint32 duration)
{
    uint32 packet, red_step, green_step, blue_step, color, flags;
    FUNCTION_MARKER(0x8001BCDC, "SLUS_003.73");
    packet = AcquireObjectFlash(object);
    if (!packet)
        return packet;
    red &= 255;
    red_step = DivideFlashScalar(32768 - (red << 8), duration);
    green &= 255;
    green_step = DivideFlashScalar(32768 - (green << 8), duration);
    blue &= 255;
    blue_step = DivideFlashScalar(32768 - (blue << 8), duration);
    color = (blue << 16) | (green << 8) | red;
    w_u32(packet + 16, object);
    w_u32(packet, color);
    w_u8(packet + 12, 0);
    w_u8(packet + 11, 0);
    w_u8(packet + 10, 0);
    w_u8(packet + 13, duration);
    w_u16(packet + 4, red_step);
    w_u16(packet + 6, green_step);
    w_u16(packet + 8, blue_step);
    flags = r_u16(object);
    w_u16(object, flags | 1024);
    color = r_u32(packet);
    w_u32(object + 32, color);
    return color;
}

uint32 FadeLevelObjectToBlack(uint32 object, uint32 duration)
{
    uint32 packet, result, step, flags;
    FUNCTION_MARKER(0x8001BDC4, "SLUS_003.73");
    packet = AcquireObjectFlash(object);
    result = 32768;
    if (!packet)
        return result;
    step = DivideFlashScalar(32768, duration);
    w_u32(packet + 16, object);
    w_u32(packet, 0x808080);
    w_u8(packet + 12, 0);
    w_u8(packet + 11, 0);
    w_u8(packet + 10, 0);
    w_u8(packet + 13, duration);
    step = (uint32)0 - step;
    w_u16(packet + 8, step);
    w_u16(packet + 6, step);
    w_u16(packet + 4, step);
    flags = r_u16(object);
    w_u16(object, flags | 1024);
    result = r_u32(packet);
    w_u32(object + 32, result);
    return result;
}

extern "C" uint32 sub_8001BDC4(uint32 object, uint32 duration)
{
    FUNCTION_MARKER(0x8001BDC4, "SLUS_003.73");
    return FadeLevelObjectToBlack(object, duration);
}

uint32 DestroyLevelObject(uint32 object)
{
    uint32 previous, count, result, bank, index, table, resource, record, type, explosive = 0;
    uint32 position[3], child, radius = 512, node, descriptor, flags, distance, selected;
    sint32 height;
    FUNCTION_MARKER(0x8001CC1C, "SLUS_003.73");
    previous = r_u32(0x800FF1F0);
    count = previous + 1;
    w_u32(0x800FF1F0, count);
    result = (sint32)count < 10;
    if (!result)
    {
        w_u32(0x800FF1F0, previous);
        return result;
    }
    if (FragmentLevelObject(object, 1, 1))
    {
        bank = FindActiveModelBankSlot(r_u8(object + 27));
        index = r_u16(object + 22);
        table = r_u32(0x800FF7B4 + bank * 4);
        resource = r_u32(table + index * 4);
        record = ActivateResourceScript(resource);
        if (record)
        {
            index = r_u16(record + 10);
            table = r_u32(0x800FF624);
            type = r_u32(table + index * 4);
            explosive = (sint16)r_u16(type) == 9;
        }
        if (!r_u32(0x800FF738))
        {
            position[0] = r_u32(object + 4);
            position[1] = r_u32(object + 8);
            position[2] = r_u32(object + 12);
            if (explosive)
            {
                sint32 upper = (sint16)r_u16(0x800EC51C);
                sint32 lower = (sint16)r_u16(0x800EC51A);
                QueueExplosion(position, 0, (uint32)upper, (uint32)lower, 1);
                selected = r_u32(0x800FF2F0) & 1;
                PlayPositionedSound(selected ? 1 : 2, position, 0);
                child = AllocateEffectStorage(124);
                if (child)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExpandingGlow));
                    new (dram_addr) ExpandingGlow(position, 64, 128, 0, 512, 10);
                }
                SpawnExplosionParticles(position, 20, 10, 750, 80);
                child = AllocateEffectStorage(124);
                if (child)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
                    new (dram_addr) RingBurstEffect(position, 5, 255, 255, 255, 1, 16, 64, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
                }
            }
            else if (!RandomRange(5))
                SpawnExplosion(position, 1);
            else
                SpawnExplosion(position, 0);
        }
        if (explosive)
            radius = 1024;
        node = r_u32(0x800FF794);
        while (node)
        {
            if (node != object)
            {
                bank = r_u8(node + 27);
                index = r_u16(node + 22);
                table = r_u32(0x800EAEF8 + bank * 64 + 16);
                descriptor = r_u32(table + index * 4);
                flags = r_u32(descriptor);
                if ((flags & 2) && (flags >> 24))
                {
                    selected = 0;
                    distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(node + 4, 12));
                    if (explosive)
                        selected = distance < radius;
                    else if (distance < radius)
                    {
                        uint32 source_y = r_u32(object + 8);
                        uint32 neighbor_y = r_u32(node + 8);
                        height = (sint32)(source_y - neighbor_y);
                        if (height > 409600 && height <= 1638399)
                            selected = 1;
                    }
                    if (selected)
                        DestroyLevelObject(node);
                }
            }
            node = r_u32(node + 28);
        }
    }
    result = r_u32(0x800FF1F0) - 1;
    w_u32(0x800FF1F0, result);
    return result;
}

extern "C" uint32 sub_8001BC58(uint32 object)
{
    FUNCTION_MARKER(0x8001BC58, "SLUS_003.73");
    return AcquireObjectFlash(object);
}
extern "C" uint32 sub_8001BCDC(uint32 object, uint32 red, uint32 green, uint32 blue, uint32 duration)
{
    FUNCTION_MARKER(0x8001BCDC, "SLUS_003.73");
    return FlashLevelObject(object, red, green, blue, duration);
}
extern "C" uint32 sub_8001CC1C(uint32 object)
{
    FUNCTION_MARKER(0x8001CC1C, "SLUS_003.73");
    return DestroyLevelObject(object);
}

// Native callback adapter retains the existing reviewed target contracts
static uint32 DispatchNativeObjectDamage(uint32 target, uint32 receiver, uint32 damage, const sint32 vector[3], uint32 reason)
{
    switch (target)
    {
        case 0x80062F5C:
            // Original leaf returns one without reading the receiver or arguments
            return 1;
        case 0x800736AC:
        case 0x8004A25C:
        case 0x800747A8:
        case 0x80075930:
        case 0x80058A14:
        case 0x80056C50:
        case 0x80054714:
        case 0x800593CC:
            return apocalypse_enemy_damage_cpp(target, receiver, damage, 0, vector, reason);
        case 0x8004F5C8:
            return apocalypse_enemy_damage_cpp(target, receiver, damage, 0, vector, reason);
        case 0x8005B864:
            return Items::Attach(receiver).Items::Damage(damage);
        case 0x80045B14:
            return Plague::Attach(receiver).Plague::Damage(damage, vector, reason);
        case 0x800422C0:
            return TheBeast::Attach(receiver).TheBeast::Damage(damage, vector, reason);
        case 0x8003E354:
            return War::Attach(receiver).War::Damage(damage, vector, reason);
        case 0x80048428:
            return Death::Attach(receiver).Death::Damage(damage, vector, reason);
        case 0x8005DDF0:
            return apocalypse_player_damage_native(receiver, damage, vector, reason);
        case 0x8003C490:
            return ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::Damage(damage, 0, 0);
        case 0x8005A7B0:
            return FloatingHazard::Attach(receiver).FloatingHazard::Damage(damage, 0, 0);
        case 0x8005AF98:
            return BallisticHazard::Attach(receiver).BallisticHazard::Damage(damage, 0, 0);
        default:
            GameDiagnostic("TODO Native damage callback %08X receiver %08X reason %u\n", target, receiver, reason);
            abort();
    }
}

// Native extraction of the original adjusted virtual damage call
uint32 ApplyObjectDamage(uint32 object, uint32 damage, const sint32 *vector, uint32 reason)
{
    uint32 table = r_u32(object + 68);
    sint32 adjustment = (sint16)r_u16(table + 48);
    uint32 target = r_u32(table + 52);
    return DispatchNativeObjectDamage(target, object + (uint32)adjustment, damage, vector, reason);
}

extern "C" uint32 apocalypse_object_virtual52_native(uint32 target, uint32 receiver, uint32 damage, const sint32 vector[3], uint32 reason)
{
    return DispatchNativeObjectDamage(target, receiver, damage, vector, reason);
}
