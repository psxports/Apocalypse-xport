#include "object_collision_events.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_launch.h"
#include "glow_effect.h"
#include "ring_burst_effect.h"
#include "explosion_effects.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "audio_session.h"
#include <new>

static uint32 DivideLaunchScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

// Native input adapter for the original position word loads
static uint32 ReadLaunchPositionWord(const uint32 *position, uint32 index)
{
    uint32 guest_position = GuestMemoryAddress(position);
    return guest_position ? r_u32(guest_position + index * 4) : position[index];
}

uint32 CalculateLaunchAngle(uint32 input);

uint32 LaunchedProjectile::Configure(uint32 owner, uint32 radius, uint32 model, uint32 byte5, uint32 byte6)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 tag, tick, child, random;
    FUNCTION_MARKER(0x8002A7F8, "SLUS_003.73");
    AddToList((uint32 *)psx_addr(0x800FF220, 4));
    if (model)
        tag = r_u8(0x800FF644);
    else
    {
        tag = r_u8(0x800FF645);
        model = 0x51E46AAF;
    }
    w_u16(object + 22, FindModelResourceIndex(model, tag));
    tick = r_u32(0x800FF650);
    w_u32(object + 312, 26);
    w_u16(object + 204, 300);
    w_u32(object + 304, owner);
    w_u16(object + 310, radius);
    w_u8(object + 395, byte5);
    w_u8(object + 396, byte6);
    w_u32(object + 380, tick);
    w_u16(object + 58, owner == 0x800FF5A0 ? 63 : 64);
    if (r_u32(object + 304) == 0x800FF5A0)
        w_u16(object + 78, r_u16(object + 78) & 0xFFEF);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect(4, 2);
    }
    w_u32(object + 388, child);
    w_u8(child + 94, 1);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetRowExtent( 0, 16);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetRowExtent( 1, 16);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetCenterColor( 0, 0, 0);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetRingColors( 0, 0, 0);
    w_u8(object + 392, 100);
    w_u8(object + 393, 50);
    w_u8(object + 394, 0);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetRowColors( 0, 100, 50, 0);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetRowColors( 1, 0, 0, 0);
    random = RandomRange(1024);
    child = r_u32(object + 388);
    w_u16(child + 96, random);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetRadii( 0);
    child = r_u32(object + 388);
    RingEffect::Attach(child).SetPosition((const uint32 *)psx_addr(object + 4, 12));
    child = r_u32(object + 388);
    w_u8(child + 66, 1);
    return child;
}

uint32 LaunchedProjectile::SetGlowColor(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child;
    FUNCTION_MARKER(0x8002A9F0, "SLUS_003.73");
    w_u8(object + 394, blue);
    w_u8(object + 392, red);
    w_u8(object + 393, green);
    child = r_u32(object + 388);
    return RingEffect::Attach(child).SetRowColors( 0, red & 255, green & 255, blue & 255);
}

LaunchedProjectile::LaunchedProjectile(const uint32 *position, const sint16 *angles, uint32 speed, uint32 damage, uint32 radius, uint32 list, uint32 model, uint32 first, uint32 second)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002AA38, "SLUS_003.73");
    w_u32(object + 68, 0x800A1338);
    w_u16(object + 308, damage);
    Configure(list, radius, model, first & 255, second & 255);
    if (native_position)
    {
        x = native_position[0];
        y = native_position[1];
        z = native_position[2];
    }
    else
    {
        x = r_u32(guest_position);
        y = r_u32(guest_position + 4);
        z = r_u32(guest_position + 8);
    }
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    BuildDirectionVector((uint32 *)psx_addr(object + 104, 12), speed, (const uint16 *)angles);
}

LaunchedProjectile::LaunchedProjectile(const uint32 *position, const uint32 *target, uint32 speed, uint32 damage, uint32 radius, uint32 owner, uint32 model, uint32 first, uint32 second)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002AB28, "SLUS_003.73");
    uint32 x, y, z, origin, aim, dx, dz, distance, qx, qz, gravity, dy, vertical, scale;
    w_u32(object + 68, 0x800A1338);
    w_u16(object + 308, damage);
    Configure(owner, radius, model, first & 255, second & 255);
    x = ReadLaunchPositionWord(position, 0);
    y = ReadLaunchPositionWord(position, 1);
    z = ReadLaunchPositionWord(position, 2);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    origin = ReadLaunchPositionWord(position, 0);
    aim = ReadLaunchPositionWord(target, 0);
    dx = (uint32)((sint32)(aim - origin) >> 12);
    aim = ReadLaunchPositionWord(target, 2);
    origin = ReadLaunchPositionWord(position, 2);
    dz = (uint32)((sint32)(aim - origin) >> 12);
    distance = LookupDistanceRoot(dx * dx + dz * dz);
    dx <<= 12;
    if (!distance)
    {
        w_u32(object + 104, 0);
        w_u32(object + 108, 0xFFFEC000);
        w_u32(object + 112, 0);
    }
    else
    {
        qx = DivideLaunchScalar(dx, distance);
        qz = DivideLaunchScalar(dz << 12, distance);
        gravity = DivideLaunchScalar(distance << 3, speed << 1);
        origin = ReadLaunchPositionWord(position, 1);
        aim = ReadLaunchPositionWord(target, 1);
        dy = (uint32)((sint32)(aim - origin) >> 12);
        vertical = DivideLaunchScalar(speed * dy, distance);
        scale = (uint32)((sint32)(speed * 9) / 10);
        w_u32(object + 104, qx * scale);
        w_u32(object + 108, (vertical - gravity) << 12);
        w_u32(object + 112, qz * scale);
    }
}

uint32 LaunchedProjectile::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, result;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002AD10, "SLUS_003.73");
    child = r_u32(object + 388);
    w_u32(object + 68, 0x800A1338);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    RemoveFromList((uint32 *)psx_addr(0x800FF220, 4));
    ProjectileBase::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 LaunchedProjectile::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 before[3], x, y, z, speed, dot, normal, product, value, one = 1;
    uint32 explode = 0, child, head, hit, elapsed, limit, red, green, blue, table, target, height;
    uint32 now, start_time;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002AD9C, "SLUS_003.73");
    w_u32(object + 8, r_u32(object + 8) + 81920);
    before[0] = r_u32(object + 4);
    before[1] = r_u32(object + 8);
    before[2] = r_u32(object + 12);
    if (!r_u8(object + 384))
    {
        x = r_u32(object + 4);
        w_u32(0x800ED638, x);
        y = r_u32(object + 8);
        w_u32(0x800ED63C, y);
        z = r_u32(object + 12);
        w_u32(0x800ED640, z);
        w_u32(0x800ED644, x + (r_u32(object + 104) << 1));
        w_u32(0x800ED648, y + (r_u32(object + 108) << 1));
        w_u32(0x800ED64C, z + (r_u32(object + 112) << 1));
        speed = MeasureVectorLength((const uint32 *)psx_addr(object + 104, 12));
        InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
        w_u32(0x800FF974, 1);
        QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
        w_u32(0x800FF974, 0);
        AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
        w_u32(object + 108, r_u32(object + 108) + 32768);
        if (r_u32(0x800ED6A0) && (sint32)speed >= (sint32)r_u32(0x800ED678))
        {
            w_u32(object + 4, r_u32(0x800ED6A4));
            w_u32(object + 8, r_u32(0x800ED6A8));
            w_u32(object + 12, r_u32(0x800ED6AC));
            PlayPositionedSound(22, (const uint32 *)psx_addr(object + 4, 12), 0);
            x = r_u32(object + 104);
            normal = (uint32)(sint32)(sint16)r_u16(0x800ED6B0);
            dot = (uint32)((sint32)x >> 6) * normal;
            y = r_u32(object + 108);
            dot += (uint32)((sint32)y >> 6) * (uint32)(sint32)(sint16)r_u16(0x800ED6B2);
            z = r_u32(object + 112);
            dot += (uint32)((sint32)z >> 6) * (uint32)(sint32)(sint16)r_u16(0x800ED6B4);
            dot = (uint32)((sint32)dot >> 12);
            product = dot * normal;
            w_u32(object + 104, x - (uint32)((sint32)product >> 6));
            normal = (uint32)(sint32)(sint16)r_u16(0x800ED6B2);
            y = r_u32(object + 108);
            product = dot * normal;
            w_u32(object + 108, y - (uint32)((sint32)product >> 6));
            normal = (uint32)(sint32)(sint16)r_u16(0x800ED6B4);
            explode = r_u8(object + 395) != 0;
            z = r_u32(object + 112);
            product = dot * normal;
            w_u32(object + 112, z - (uint32)((sint32)product >> 6));
            ShiftVectorRight((uint32 *)psx_addr(object + 104, 12), &one);
            normal = (uint32)(sint32)(sint16)r_u16(0x800ED6B0);
            x = r_u32(object + 104);
            product = dot * normal;
            w_u32(object + 104, x - (uint32)((sint32)product >> 7));
            normal = (uint32)(sint32)(sint16)r_u16(0x800ED6B2);
            y = r_u32(object + 108);
            product = dot * normal;
            w_u32(object + 108, y - (uint32)((sint32)product >> 7));
            normal = (uint32)(sint32)(sint16)r_u16(0x800ED6B4);
            z = r_u32(object + 112);
            product = dot * normal;
            w_u32(object + 112, z - (uint32)((sint32)product >> 7));
            value = (RandomRange(11) - 5) << 12;
            w_u32(object + 104, r_u32(object + 104) + value);
            value = (RandomRange(11) - 5) << 12;
            z = r_u32(object + 112);
            y = r_u32(object + 108);
            w_u32(object + 112, z + value);
            if ((sint32)y > -61440 && (sint16)r_u16(0x800ED6B2) < -3500)
                w_u8(object + 384, 1);
        }
        product = 410 * MeasureVectorLength((const uint32 *)psx_addr(object + 104, 12));
        value = r_u16(object + 16);
        w_u16(object + 16, value + (uint32)((sint32)product / 128));
        x = r_u32(object + 104);
        z = (uint32)((sint32)r_u32(object + 112) >> 12);
        x = (uint32)((sint32)x >> 12);
        if (!z)
            w_u16(object + 18, (sint32)x > 0 ? -1024 : 1024);
        else if ((sint32)z > 0)
            w_u16(object + 18, 2048 - CalculateLaunchAngle(DivideLaunchScalar(x << 12, 0 - z)));
        else
            w_u16(object + 18, CalculateLaunchAngle(DivideLaunchScalar(x << 12, z)));
    }
    w_u32(0x800ED638, r_u32(object + 4));
    w_u32(0x800ED63C, r_u32(object + 8));
    w_u32(0x800ED640, r_u32(object + 12));
    w_u32(0x800ED644, r_u32(object + 4));
    y = r_u32(object + 8);
    w_u32(0x800ED648, y);
    z = r_u32(object + 12);
    w_u32(0x800ED648, y + 12288000);
    w_u32(0x800ED64C, z);
    InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
    QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
    if (r_u32(0x800ED6A0))
    {
        w_u32(object + 184, r_u32(0x800ED6A4));
        y = r_u32(0x800ED6A8);
        w_u32(object + 188, y);
        value = r_u32(object + 8);
        z = r_u32(0x800ED6AC);
        height = (uint32)((sint32)(y - value) >> 12);
        product = (uint32)(21 * (sint32)(sint16)height);
        w_u32(object + 192, z);
        w_u16(object + 202, height);
        value = 50 - (uint32)((sint32)product / 128);
        w_u16(object + 200, value);
        if (value & 65535)
            EnableShadow();
        else
            RemoveShadow();
    }
    else
        RemoveShadow();
    now = r_u32(0x800FF650);
    start_time = r_u32(object + 380);
    elapsed = now - start_time;
    limit = 60 * (uint32)(sint32)(sint16)r_u16(0x800EC7FE);
    if ((sint32)(limit - 30) < (sint32)elapsed)
    {
        if (r_u32(0x800FF2F0) & 2)
        {
            value = r_u8(object + 392);
            red = r_u8(object + 392) * 2;
            if ((sint8)value < 0)
                red = 255;
            value = r_u8(object + 393);
            green = r_u8(object + 393) * 2;
            if ((sint8)value < 0)
                green = 255;
            value = r_u8(object + 394);
            blue = r_u8(object + 394) * 2;
            if ((sint8)value < 0)
                blue = 255;
            red &= 255;
            green &= 255;
            blue &= 255;
        }
        else
        {
            blue = r_u8(object + 394);
            red = r_u8(object + 392);
            green = r_u8(object + 393);
        }
        child = r_u32(object + 388);
        RingEffect::Attach(child).SetRowColors(0, red, green, blue);
    }
    limit = 60 * (uint32)(sint32)(sint16)r_u16(0x800EC7FE);
    if ((sint32)limit < (sint32)elapsed)
        explode = 1;
    head = r_u32(r_u32(object + 304));
    hit = TraceSegmentCollision(before, (const uint32 *)psx_addr(object + 4, 12), head);
    if (hit && (r_u16(hit + 58) == 205 || !(r_u16(hit) & 16)))
        explode = 1;
    if (explode)
    {
        table = r_u32(object + 304);
        value = r_u16(object + 310);
        head = r_u32(table);
        ApplyBlast(head, value, 10, 26);
        head = r_u32(0x800FF5DC);
        value = r_u16(object + 310);
        ApplyBlast(head, value, 10, 26);
        DamageNearbyObjects(r_u16(object + 310));
        child = AllocateEffectStorage(124);
        if (child)
        {
            value = r_u16(object + 310);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExpandingGlow));
            new (dram_addr) ExpandingGlow((const uint32 *)psx_addr(object + 4, 12), 128, 100, 0, value, 7);
        }
        value = RandomRange(2);
        PlayPositionedSound(value ? 1 : 2, (const uint32 *)psx_addr(object + 4, 12), 0);
        SpawnExplosionParticles((const uint32 *)psx_addr(object + 4, 12), 20, 10, 750, 80);
        child = AllocateEffectStorage(124);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
            new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(object + 4, 12), 5, 255, 255, 255, 1, 128, 100, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
        }
        table = r_u32(object + 68);
        adjustment = (sint16)r_u16(table + 16);
        target = r_u32(table + 20);
        xport_draft_guest_call1(target, object + (uint32)adjustment);
    }
    y = r_u32(object + 8);
    child = r_u32(object + 388);
    w_u32(object + 8, y - 81920);
    return RingEffect::Attach(child).SetPosition((const uint32 *)psx_addr(object + 4, 12));
}


uint32 CalculateLaunchAngle(uint32 input)
{
    uint32 x = 4096, y = input, angle = 0, next_x, table, step;
    FUNCTION_MARKER(0x8008702C, "SLUS_003.73");
    for (step = 0; step < 12; ++step)
    {
        if ((sint32)y < 0)
        {
            next_x = x - (uint32)((sint32)y >> step);
            y += (uint32)((sint32)x >> step);
            table = r_u32(0x800F82DC + step * 4);
            angle -= table;
        }
        else
        {
            next_x = x + (uint32)((sint32)y >> step);
            y -= (uint32)((sint32)x >> step);
            table = r_u32(0x800F82DC + step * 4);
            angle += table;
        }
        x = next_x;
    }
    return angle;
}

LaunchedProjectile &LaunchedProjectile::Attach(uint32 object)
{
    return *static_cast<LaunchedProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8002A7F8(uint32 object, uint32 owner, uint32 radius, uint32 model, uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x8002A7F8, "SLUS_003.73");
    return LaunchedProjectile::Attach(object).Configure(owner, radius, model, first, second);
}

extern "C" uint32 sub_8002AA38(uint32 object, uint32 position, uint32 angles, uint32 speed, uint32 damage, uint32 radius, uint32 list, uint32 model, uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x8002AA38, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(LaunchedProjectile));
    new (dram_addr) LaunchedProjectile((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), speed, damage, radius, list, model, first, second);
    return object;
}

extern "C" uint32 apocalypse_projectile_2AA38_native(uint32 object, const uint32 *position, const sint16 *angles, uint32 speed, uint32 damage, uint32 radius, uint32 list, uint32 model, uint32 first, uint32 second)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(LaunchedProjectile));
    new (dram_addr) LaunchedProjectile(position, angles, speed, damage, radius, list, model, first, second);
    return object;
}

extern "C" uint32 sub_8002AB28(uint32 object, uint32 position, uint32 target, uint32 speed, uint32 damage, uint32 radius, uint32 owner, uint32 model, uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x8002AB28, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(LaunchedProjectile));
    new (dram_addr) LaunchedProjectile((const uint32 *)psx_addr(position, 12), (const uint32 *)psx_addr(target, 12), speed, damage, radius, owner, model, first, second);
    return object;
}

extern "C" uint32 xport_draft_host_sub_8002AB28_p23(uint32 object, const uint32 *position, uint32 target, const uint32 *native_target, uint32 speed, uint32 damage, uint32 radius, uint32 owner, uint32 model, uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x8002AB28, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(LaunchedProjectile));
    new (dram_addr) LaunchedProjectile(position, native_target ? native_target : (const uint32 *)psx_addr(target, 12), speed, damage, radius, owner, model, first, second);
    return object;
}

extern "C" uint32 sub_8002AD10(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002AD10, "SLUS_003.73");
    return LaunchedProjectile::Attach(object).LaunchedProjectile::Destroy(reason);
}

extern "C" uint32 sub_8002AD9C(uint32 object)
{
    FUNCTION_MARKER(0x8002AD9C, "SLUS_003.73");
    return LaunchedProjectile::Attach(object).LaunchedProjectile::Update();
}




extern "C" uint32 sub_8002A9F0(uint32 object, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8002A9F0, "SLUS_003.73");
    // TODO Original allocation failure falls through with a null receiver; native binding fails fast
    return LaunchedProjectile::Attach(object).SetGlowColor(red, green, blue);
}
