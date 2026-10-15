extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "ring_attack.h"
#include "mesh_ring_effect.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_arithmetic.h"
#include "glow_effect.h"
#include "vector_distance.h"
#include "audio_session.h"
#include "camera.h"
#include "object_damage.h"
#include <new>

RingAttack::RingAttack(uint32 start_damage, uint32 end_damage, uint32 falloff_distance) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player, x, y, z, child, first, second, list, index, value;
    FUNCTION_MARKER(0x80029D24, "SLUS_003.73");
    w_u8(object + 67, 3);
    player = r_u32(0x800FF5A0);
    w_u32(object + 68, 0x800A1380);
    x = r_u32(player + 4);
    y = r_u32(player + 8);
    z = r_u32(player + 12);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u32(object + 92, start_damage);
    w_u32(object + 96, end_damage);
    w_u32(object + 100, falloff_distance);
    if (!falloff_distance)
        w_u32(object + 100, 1);
    child = AllocateEffectStorage(116);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MeshRingEffect));
        new (dram_addr) MeshRingEffect(16, 0xCCE0187C);
    }
    w_u32(object + 72, child);
    // TODO Original allocation failure still writes through a null child
    w_u8(child + 66, 1);
    child = r_u32(object + 72);
    MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), 0, 0);
    first = RandomRange(64);
    second = RandomRange(64);
    child = r_u32(object + 72);
    MeshRingEffect::Attach(child).SetTextureOffsets(first, second, 2);
    w_u32(object + 80, 255);
    w_u32(object + 84, 100);
    child = r_u32(object + 72);
    w_u32(object + 88, 0);
    MeshRingEffect::Attach(child).SetColors(255, 100, 0);
    list = r_u32(0x800FF4E8);
    child = r_u32(object + 72);
    w_u16(child + 114, 1);
    ClearRingAttackHitFlags(list);
    list = r_u32(0x800FF5DC);
    ClearRingAttackHitFlags(list);
    for (index = 0; index < 16; ++index)
    {
        value = RandomRange(4096);
        w_u32(object + 104 + index * 4, value);
    }
    Camera::Attach(r_u32(0x800FF904)).ApplyExplosionShake(1);
    w_u32(0x800FF218, 1);
    w_u32(0x800FF21C, object);
    PlaySoundEffect(10, 8192, 0);
}

uint32 RingAttack::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x80029ED4, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A1380);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    w_u32(0x800FF218, 0);
    w_u32(0x800FF21C, 0);
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

RingAttack &RingAttack::Attach(uint32 object)
{
    return *static_cast<RingAttack *>(FindNativeObjectStorage(object));
}

uint32 ClearRingAttackHitFlags(uint32 list)
{
    uint32 flags = 0;
    FUNCTION_MARKER(0x80029F58, "SLUS_003.73");
    // Empty original list leaves V0 undefined and callers discard that carrier
    while (list)
    {
        flags = r_u16(list + 78) & 0xFEFF;
        w_u16(list + 78, flags);
        list = r_u32(list + 28);
    }
    return flags;
}

extern "C" uint32 sub_80029F58(uint32 unused_receiver, uint32 list)
{
    FUNCTION_MARKER(0x80029F58, "SLUS_003.73");
    return ClearRingAttackHitFlags(list);
}

extern "C" uint32 sub_80029D24(uint32 object, uint32 start_damage, uint32 end_damage, uint32 falloff_distance)
{
    FUNCTION_MARKER(0x80029D24, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RingAttack));
    new (dram_addr) RingAttack(start_damage, end_damage, falloff_distance);
    return object;
}

extern "C" uint32 sub_80029ED4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80029ED4, "SLUS_003.73");
    return RingAttack::Attach(object).RingAttack::Destroy(reason);
}

static uint32 DivideRingAttack(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

void RingAttack::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 old_width, old_radius, inner_radius, width, radius, child, u, v, red, green, blue;
    uint32 count, index, phase, phase_cursor, offset, sine, product, buffer, point, value;
    uint32 node, flags, kind, distance, table, target, allocation, list, outer_radius, divisor, start, end, damage;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002A138, "SLUS_003.73");
    old_width = r_u32(object + 168);
    old_radius = r_u32(object + 76);
    width = old_width + 130;
    w_u32(object + 168, width);
    inner_radius = old_width + old_radius;
    if ((sint32)width >= 2001)
        w_u32(object + 76, old_radius + 100);
    child = r_u32(object + 72);
    radius = r_u32(object + 76);
    width = r_u32(object + 168);
    MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius, width);
    child = r_u32(object + 72);
    v = r_u32(child + 104);
    u = r_u32(child + 100);
    MeshRingEffect::Attach(child).SetTextureOffsets(u, v - 1, 2);
    red = r_u32(object + 80);
    child = r_u32(object + 72);
    blue = r_u32(object + 88);
    w_u32(object + 80, red - 3);
    green = r_u32(object + 84);
    red = r_u32(object + 80);
    blue -= 3;
    w_u32(object + 88, blue);
    green -= 3;
    w_u32(object + 84, green);
    MeshRingEffect::Attach(child).SetColors(red, green, blue);
    if ((sint32)r_u32(object + 80) <= 0 && (sint32)r_u32(object + 84) <= 0 && (sint32)r_u32(object + 88) <= 0)
        Finish();
    width = r_u32(object + 168);
    if ((sint32)width >= 2001)
    {
        child = r_u32(object + 72);
        count = r_u32(child + 76);
        index = 0;
        if ((sint32)count > 0)
        {
            phase_cursor = object;
            phase = r_u32(0x800FF2F0) * 400;
            offset = 0;
            do
            {
                value = r_u32(phase_cursor + 104);
                sine = (uint32)(sint32)(sint16)r_u16(0x800F863C + ((phase + value) & 4095) * 4);
                product = sine * 200;
                buffer = r_u32(child + 80);
                point = buffer + offset;
                if ((sint32)product < 0)
                    product += 4095;
                phase_cursor += 4;
                product = (uint32)((sint32)product >> 12) << 12;
                value = r_u32(point + 132);
                w_u32(point + 132, value + product);
                child = r_u32(object + 72);
                count = r_u32(child + 76);
                index += 1;
                offset += 140;
            } while ((sint32)index < (sint32)count);
        }
    }
    list = r_u32(0x800FF4E8);
    DamageObjects(list, 1);
    list = r_u32(0x800FF5DC);
    DamageObjects(list, 0);
    node = r_u32(0x800FF220);
    while (node)
    {
        flags = r_u32(node + 76);
        if (!(flags & 0x1200000))
        {
            kind = r_u16(node + 58);
            if (kind != 62 && kind != 66 && kind != 64)
            {
                distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(node + 4, 12));
                width = r_u32(object + 168);
                if (width >= distance)
                {
                    table = r_u32(node + 68);
                    adjustment = (sint16)r_u16(table + 16);
                    target = r_u32(table + 20);
                    apocalypse_object_virtual20(target, node + (uint32)adjustment);
                    sub_8001E1B8(node + 4, 0);
                    flags = r_u16(node + 78);
                    w_u16(node + 78, flags | 256);
                }
            }
        }
        node = r_u32(node + 28);
    }
    node = r_u32(0x800FF434);
    while (node)
    {
        kind = r_u8(node + 67);
        if (kind == 1)
        {
            distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(node + 24, 12));
            width = r_u32(object + 168);
            if (width >= distance)
            {
                allocation = AllocateEffectStorage(120);
                if (allocation)
                {
                    red = r_u8(node + 104);
                    green = r_u8(node + 105);
                    blue = r_u8(node + 106);
                    void *dram_addr = AllocateNativeObjectStorage(allocation, sizeof(GlowEffect));
                    new (dram_addr) GlowEffect((const uint32 *)psx_addr(node + 24, 12), red, green, blue, 8, 18, 50, 4);
                }
                sub_80032ED8(node);
            }
        }
        node = r_u32(node + 4);
    }
    node = r_u32(0x800FF458);
    while (node)
    {
        kind = r_u8(node + 67);
        if (kind == 4)
        {
            distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(node + 24, 12));
            width = r_u32(object + 168);
            if (width >= distance)
            {
                allocation = AllocateEffectStorage(120);
                if (allocation)
                {
                    red = r_u8(node + 265);
                    green = r_u8(node + 266);
                    blue = r_u8(node + 267);
                    void *dram_addr = AllocateNativeObjectStorage(allocation, sizeof(GlowEffect));
                    new (dram_addr) GlowEffect((const uint32 *)psx_addr(node + 24, 12), red, green, blue, 8, 18, 50, kind);
                }
                sub_80032ED8(node);
            }
        }
        node = r_u32(node + 4);
    }
    node = r_u32(0x800FF794);
    width = r_u32(object + 168);
    radius = r_u32(object + 76);
    divisor = r_u32(object + 100);
    outer_radius = width + radius;
    if ((sint32)divisor < (sint32)width)
        damage = (uint32)((sint32)r_u32(object + 96) >> 2);
    else
    {
        start = r_u32(object + 92);
        end = r_u32(object + 96);
        product = (start - end) * width;
        value = start - DivideRingAttack(product, divisor);
        damage = (uint32)((sint32)value >> 2);
    }
    while (node)
    {
        distance = MeasurePositionDistance((const uint32 *)psx_addr(node + 4, 12), (const uint32 *)psx_addr(object + 24, 12));
        if ((sint32)distance >= (sint32)inner_radius && (sint32)outer_radius >= (sint32)distance)
            DamageLevelObject(node, 0, (sint32)damage);
        node = r_u32(node + 28);
    }
}

extern "C" void sub_8002A138(uint32 object)
{
    FUNCTION_MARKER(0x8002A138, "SLUS_003.73");
    RingAttack::Attach(object).RingAttack::Update();
}

void RingAttack::DamageObjects(uint32 list, uint32 effects)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, kind, length, width, divisor, start, end, product, damage, table, target;
    uint32 vector[3];
    sint32 adjustment;
    FUNCTION_MARKER(0x80029F88, "SLUS_003.73");
    while (list)
    {
        flags = r_u16(list + 78);
        if (flags & 16)
        {
            kind = r_u16(list + 58);
            if (kind != 206 && !(flags & 256))
            {
                length = MeasurePositionDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(list + 4, 12));
                width = r_u32(object + 168);
                if ((sint32)width >= (sint32)length)
                {
                    if (effects && !r_u32(0x800FF738))
                        sub_8001D320(list + 4, 70, 240, 200, 0, 5, 0, 100);
                    SubtractVectors(vector, (const uint32 *)psx_addr(list + 4, 12), (const uint32 *)psx_addr(object + 24, 12));
                    if (length)
                        DivideVector(vector, &length);
                    else
                    {
                        vector[0] = 0;
                        vector[1] = 0;
                        vector[2] = 0;
                    }
                    width = r_u32(object + 168);
                    divisor = r_u32(object + 100);
                    if ((sint32)divisor < (sint32)width)
                        damage = r_u32(object + 96);
                    else
                    {
                        start = r_u32(object + 92);
                        end = r_u32(object + 96);
                        product = (start - end) * width;
                        damage = start - DivideRingAttack(product, divisor);
                    }
                    table = r_u32(list + 68);
                    adjustment = (sint16)r_u16(table + 48);
                    target = r_u32(table + 52);
                    apocalypse_object_virtual52_native(target, list + (uint32)adjustment, damage, (const sint32 *)vector, 30);
                    flags = r_u16(list + 78);
                    w_u16(list + 78, flags | 256);
                }
            }
        }
        list = r_u32(list + 28);
    }
}

extern "C" void sub_80029F88(uint32 object, uint32 list, uint32 effects)
{
    FUNCTION_MARKER(0x80029F88, "SLUS_003.73");
    RingAttack::Attach(object).DamageObjects(list, effects);
}
