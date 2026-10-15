#include "game_sdk_bindings.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "object_collision_events.h"
#include "object_damage.h"
#include "vector_distance.h"
#include "ring_effect.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_line.h"
#include "model_projection.h"
#include "effect_sprite.h"
#include "smoke_emitter.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include "effect_color.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



uint32 LineProjectile::ReadAnglesWord(uint32 address, const void *native_angles)
{
    uint32 left, right, shift, value;
    if (native_angles)
    {
        memcpy(&value, native_angles, 4);
        return value;
    }
    left = r_u32((address + 3) & ~3);
    right = r_u32(address & ~3);
    shift = (address & 3) * 8;
    return shift ? (left << (32 - shift)) | (right >> shift) : right;
}

uint32 LineProjectile::ReadAnglesTail(uint32 address, const void *native_angles)
{
    sint16 value;
    if (!native_angles)
        return (uint32)(sint32)(sint16)r_u16(address + 4);
    memcpy(&value, (const unsigned char *)native_angles + 4, 2);
    return (uint32)(sint32)value;
}

uint32 LineProjectile::DivideLifetime(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return numerator & 0x80000000 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

LineProjectile::LineProjectile(const uint32 *input_position, const sint16 *input_angles, uint32 speed, uint32 damage, const uint32 *owner_pointer, uint32 red, uint32 green, uint32 blue, uint32 mode) : RenderableEffect()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 angles = GuestMemoryAddress(input_angles);
    const void *native_angles = angles ? 0 : input_angles;
    uint16 rotation[3];
    uint32 vector[3], scaled[3], packed, x, y, z, player, owner, scalar, effect;
    FUNCTION_MARKER(0x800256F0, "SLUS_003.73");
    const uint32 owner_address = GuestMemoryAddress(owner_pointer);
    if (!owner_address)
    {
        // TODO Bind native owner records to the original pointer contract
        GameDiagnostic("TODO LineProjectile native owner record\n");
        abort();
    }
    w_u32(object + 68, 0x800A1458);
    player = r_u32(0x800FF5A0);
    w_u32(object + 80, speed);
    w_u16(object + 76, damage);
    w_u32(object + 72, owner_address);
    owner = r_u32(owner_address);
    if (owner == player)
        w_u8(object + 67, 4);
    packed = ReadAnglesWord(angles, native_angles);
    z = ReadAnglesTail(angles, native_angles);
    memcpy(rotation, &packed, 4);
    rotation[2] = (uint16)z;
    scalar = RandomRange(21);
    rotation[0] = (uint16)((rotation[0] + scalar - 10) & 4095);
    speed = r_u32(object + 80);
    BuildDirectionVector((uint32 *)psx_addr(object + 36, 12), speed, rotation);
    DivideVector(vector, (const uint32 *)psx_addr(object + 36, 12), (const uint32 *)psx_addr(object + 80, 4));
    w_u32(object + 96, vector[0]);
    w_u32(object + 100, vector[1]);
    w_u32(object + 104, vector[2]);
    scalar = 100;
    MultiplyVector(scaled, (const uint32 *)psx_addr(object + 96, 12), &scalar);
    AddVectors(vector, input_position, scaled);
    w_u32(object + 24, vector[0]);
    w_u32(object + 28, vector[1]);
    w_u32(object + 32, vector[2]);
    x = r_u32(object + 24);
    y = r_u32(object + 28);
    z = r_u32(object + 32);
    w_u32(object + 84, x);
    w_u32(object + 88, y);
    w_u32(object + 92, z);
    scalar = RandomRange(100);
    MultiplyVector(vector, (const uint32 *)psx_addr(object + 96, 12), &scalar);
    AddVectors((uint32 *)psx_addr(object + 24, 12), vector);
    x = r_u32(object + 24);
    y = r_u32(object + 28);
    z = r_u32(object + 32);
    w_u32(object + 108, x);
    w_u32(object + 112, y);
    w_u32(object + 116, z);
    InitializeTrace();
    w_u8(object + 265, red);
    w_u8(object + 266, green);
    w_u8(object + 267, blue);
    w_u8(object + 264, mode);
    effect = SpawnProjectileRingTrail((const uint32 *)psx_addr(object + 24, 12), red & 255, green & 255, blue & 255);
    w_u32(object + 120, effect);
}

uint32 LineProjectile::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, result;
    sint32 adjustment;
    FUNCTION_MARKER(0x800258F8, "SLUS_003.73");
    child = r_u32(object + 120);
    w_u32(object + 68, 0x800A1458);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    RenderableEffect::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 LineProjectile::Finish()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 particle, red, green, blue;
    FUNCTION_MARKER(0x80025974, "SLUS_003.73");
    particle = AllocateEffectStorage(124);
    if (particle)
    {
        red = ((59 * r_u8(object + 265)) >> 7) & 255;
        green = ((59 * r_u8(object + 266)) >> 7) & 255;
        blue = ((59 * r_u8(object + 267)) >> 7) & 255;
        void *dram_addr = AllocateNativeObjectStorage(particle, sizeof(RingBurstEffect));
        new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(object + 24, 12), 5, 255, 255, 255, 1, red, green, blue, 1, 60, 0, 1, 50, 80, 25, 40, 10, 5);
    }
    return EffectObject::Finish();
}

uint32 LineProjectile::InitializeTrace()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 dx, dy, dz, x, y, z, hit, distance, speed, result;
    FUNCTION_MARKER(0x80025A74, "SLUS_003.73");
    dx = r_u32(object + 96);
    dy = r_u32(object + 100);
    x = r_u32(object + 24);
    w_u32(object + 124, x - 300 * dx);
    y = r_u32(object + 28);
    dz = r_u32(object + 104);
    w_u32(object + 128, y - 300 * dy);
    z = r_u32(object + 32);
    dx = r_u32(object + 96);
    w_u32(object + 132, z - 300 * dz);
    x = r_u32(object + 24);
    dy = r_u32(object + 100);
    w_u32(object + 136, x + 10000 * dx);
    y = r_u32(object + 28);
    dz = r_u32(object + 104);
    w_u32(object + 140, y + 10000 * dy);
    z = r_u32(object + 32);
    w_u32(object + 144, z + 10000 * dz);
    InitializeCollisionRay((uint8 *)psx_addr(object + 124, 140));
    w_u32(0x800FF974, 1);
    QueryCollisionRay((uint8 *)psx_addr(object + 124, 140), 0);
    hit = r_u32(object + 228);
    w_u32(0x800FF974, 0);
    distance = hit ? r_u32(object + 188) : 10000;
    speed = r_u32(object + 80);
    result = DivideLifetime(distance, speed);
    w_u16(object + 10, result);
    return result;
}

uint32 LineProjectile::Render()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 packet, limit, next, red, green, blue, color, screen, depth, result;
    uint32 ordering_table, bucket, header, old, draw_mode, draw_word;
    FUNCTION_MARKER(0x80025BEC, "SLUS_003.73");
    packet = r_u32(0x800FF668);
    limit = r_u32(0x800FF374);
    next = packet + 28;
    result = limit < next;
    if (result)
        return result;
    w_u32(0x800FF668, next);
    w_u32(packet, 0x04000000);
    red = r_u8(object + 265);
    green = r_u8(object + 266);
    blue = r_u8(object + 267);
    color = (((197 * blue) >> 8) << 16) | ((197 * green) & 0xFF00) | 0x52000000 | ((197 * red) >> 8);
    w_u32(packet + 4, color);
    red = r_u8(object + 265);
    green = r_u8(object + 266);
    blue = r_u8(object + 267);
    color = (((49 * blue) >> 7) << 16) | (((49 * green) >> 7) << 8) | ((49 * red) >> 7);
    w_u32(packet + 12, color);
    depth = ProjectWorldPoint((const uint32 *)psx_addr(object + 24, 12), &screen);
    w_u32(packet + 16, screen);
    ProjectWorldPoint((const uint32 *)psx_addr(object + 108, 12), &screen);
    w_u32(packet + 8, screen);
    limit = r_u32(0x800FFAE8);
    result = (sint32)depth < (sint32)limit;
    if (!result)
    {
        limit = r_u32(0x800FFAD8);
        result = (sint32)limit < (sint32)depth;
    }
    if (result)
    {
        w_u32(0x800FF668, packet);
        return result;
    }
    ordering_table = r_u32(0x800FF660);
    bucket = ordering_table + (depth & 0x3FFC) + 112;
    header = r_u32(packet);
    old = r_u32(bucket);
    w_u32(packet, (header & 0xFF000000) | (old & 0xFFFFFF));
    old = r_u32(bucket);
    w_u32(bucket, (old & 0xFF000000) | (packet & 0xFFFFFF));
    draw_mode = r_u32(0x800FF468);
    draw_word = r_u32(0x800FF46C);
    w_u32(packet + 20, draw_mode);
    w_u32(packet + 24, draw_word);
    header = r_u32(packet + 20);
    old = r_u32(bucket);
    w_u32(packet + 20, (header & 0xFF000000) | (old & 0xFFFFFF));
    old = r_u32(bucket);
    result = (old & 0xFF000000) | ((packet + 20) & 0xFFFFFF);
    w_u32(bucket, result);
    return result;
}

uint32 LineProjectile::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, value, distance, scalar, scaled[3], position[4];
    uint32 hit, list, head, table, callback, damage, mode, node, next, particle;
    uint32 release = 0, age, lifetime, result, x, y, z;
    sint32 adjustment;
    FUNCTION_MARKER(0x80025E2C, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    child = r_u32(object + 120);
    if (child)
    {
        RingEffect::Attach(child).SetPosition((const uint32 *)psx_addr(object + 24, 12));
        child = r_u32(object + 120);
        value = r_u16(child + 96);
        w_u16(child + 96, value + 200);
    }
    age = r_u16(object + 8);
    w_u16(object + 8, age + 1);
    distance = MeasureEuclideanDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 84, 12));
    if ((sint32)distance >= 601)
        distance = 600;
    scalar = distance;
    MultiplyScalarVector(scaled, &scalar, (const uint32 *)psx_addr(object + 96, 12));
    SubtractVectors(position, (const uint32 *)psx_addr(object + 24, 12), scaled);
    w_u32(object + 108, position[0]);
    w_u32(object + 112, position[1]);
    w_u32(object + 116, position[2]);
    value = r_u8(object + 264);
    list = r_u32(object + 72);
    head = r_u32(list);
    if (value)
        hit = TraceSegmentCollision((const uint32 *)psx_addr(object + 108, 12), (const uint32 *)psx_addr(object + 24, 12), head);
    else
        hit = IntersectObjectSpheres((const uint32 *)psx_addr(object + 108, 12), (const uint32 *)psx_addr(object + 24, 12), position, head, 0);
    if (hit)
    {
        release = 1;
        table = r_u32(hit + 68);
        damage = r_u16(object + 76);
        adjustment = (sint16)r_u16(table + 48);
        callback = r_u32(table + 52);
        apocalypse_object_virtual52_native(callback, hit + (uint32)adjustment, damage, (const sint32 *)psx_addr(object + 96, 12), 9);
        mode = r_u32(0x800FF378);
        if (mode == 6)
        {
            list = r_u32(object + 72);
            if (list == 0x800FF5A0)
            {
                node = r_u32(0x800FF458);
                while (node)
                {
                    next = r_u32(node + 4);
                    if (node != object && r_u8(node + 67) == 4)
                        LineProjectile::Attach(node).LineProjectile::Finish();
                    node = next;
                }
                goto CheckLifetime;
            }
            mode = r_u32(0x800FF378);
        }
        if (mode == 3 && r_u8(object + 67) == 4)
        {
            particle = AllocateEffectStorage(124);
            if (particle)
            {
                void *dram_addr = AllocateNativeObjectStorage(particle, sizeof(RingBurstEffect));
                new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(object + 24, 12), 5, 255, 255, 255, 1, 64, 32, 160, 1, 60, 0, 1, 100, 120, 80, 100, 10, 8);
            }
        }
    }
CheckLifetime:
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    lifetime = r_u16(object + 10);
    result = (sint32)age < (sint32)lifetime;
    if (!result)
    {
        hit = r_u32(object + 228);
        release = 1;
        if (hit)
        {
            callback = r_u32(object + 252);
            damage = r_u16(object + 76);
            DamageLevelObject(hit, callback, (sint32)damage);
            x = r_u32(object + 232);
            y = r_u32(object + 236);
            z = r_u32(object + 240);
            w_u32(object + 24, x);
            w_u32(object + 28, y);
            w_u32(object + 32, z);
        }
    }
    if (release)
        return Finish();
    return result;
}

extern "C" uint32 sub_800258F8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800258F8, "SLUS_003.73");
    return LineProjectile::Attach(object).LineProjectile::Destroy(reason);
}

extern "C" uint32 sub_80025974(uint32 object)
{
    FUNCTION_MARKER(0x80025974, "SLUS_003.73");
    return LineProjectile::Attach(object).Finish();
}

extern "C" uint32 sub_80025BEC(uint32 object)
{
    FUNCTION_MARKER(0x80025BEC, "SLUS_003.73");
    return LineProjectile::Attach(object).LineProjectile::Render();
}

extern "C" uint32 sub_80025E2C(uint32 object)
{
    FUNCTION_MARKER(0x80025E2C, "SLUS_003.73");
    return LineProjectile::Attach(object).LineProjectile::Update();
}

LineProjectile &LineProjectile::Attach(uint32 object)
{
    return *static_cast<LineProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_800256F0(uint32 object, uint32 position, uint32 angles, uint32 speed, uint32 damage, uint32 owner_pointer, uint32 red, uint32 green, uint32 blue, uint32 mode)
{
    FUNCTION_MARKER(0x800256F0, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(LineProjectile));
    new (storage) LineProjectile((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), speed, damage, (const uint32 *)psx_addr(owner_pointer, 4), red, green, blue, mode);
    return object;
}

extern "C" uint32 apocalypse_line_projectile_native(uint32 object, const void *position, const void *angles, uint32 speed, uint32 damage, uint32 owner_pointer, uint32 red, uint32 green, uint32 blue, uint32 mode)
{
    void *storage = AllocateNativeObjectStorage(object, sizeof(LineProjectile));
    new (storage) LineProjectile((const uint32 *)position, (const sint16 *)angles, speed, damage, (const uint32 *)psx_addr(owner_pointer, 4), red, green, blue, mode);
    return object;
}


extern "C" uint32 sub_80025A74(uint32 object)
{
    FUNCTION_MARKER(0x80025A74, "SLUS_003.73");
    return LineProjectile::Attach(object).InitializeTrace();
}

static uint32 ReadTracerEndpoint(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position);
    return address ? r_u32(address + axis * 4) : xport_load_le32((const uint8 *)position + axis * 4);
}

ProjectileTracer::ProjectileTracer(const uint32 *start, const uint32 *end) : LineEffect()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 endpoint[3], direction[3], scaled[3], point[3], delta[3], movement[3];
    uint32 distance, offset, divisor = 5, shift = 1, axis;
    FUNCTION_MARKER(0x800234CC, "SLUS_003.73");
    w_u32(object + 68, 0x800A1530);
    for (axis = 0; axis < 3; ++axis)
        endpoint[axis] = ReadTracerEndpoint(end, axis);
    SubtractVectors(direction, endpoint, start);
    distance = MeasureVectorLength(direction);
    if (distance)
        DivideVector(direction, &distance);
    else
        Finish();
    if ((sint32)distance >= 3001)
    {
        distance = 3000;
        MultiplyScalarVector(scaled, &distance, direction);
        AddVectors(point, start, scaled);
        for (axis = 0; axis < 3; ++axis)
            endpoint[axis] = point[axis];
    }
    offset = RandomRange((uint32)((sint32)distance / 3));
    MultiplyVector(scaled, direction, &offset);
    AddVectors(point, start, scaled);
    for (axis = 0; axis < 3; ++axis)
        w_u32(object + 80 + axis * 4, point[axis]);
    SubtractVectors(delta, endpoint, (const uint32 *)psx_addr(object + 80, 12));
    DivideVector(movement, delta, &divisor);
    AddVectors(point, (const uint32 *)psx_addr(object + 80, 12), movement);
    for (axis = 0; axis < 3; ++axis)
        w_u32(object + 92 + axis * 4, point[axis]);
    ShiftVectorLeft(point, movement, &shift);
    for (axis = 0; axis < 3; ++axis)
        w_u32(object + 36 + axis * 4, point[axis]);
    w_u32(object + 72, (r_u32(object + 72) & 0xFF000000) | 0x2000000);
}

// Native ABI helper retains the return discarded by the original update dispatcher
static uint32 UpdateProjectileTracerData(uint32 object)
{
    FUNCTION_MARKER(0x800236D4, "SLUS_003.73");
    uint32 age = r_u32(object + 104);
    if (age)
    {
        AddVectors((uint32 *)psx_addr(object + 80, 12), (const uint32 *)psx_addr(object + 36, 12));
        AddVectors((uint32 *)psx_addr(object + 92, 12), (const uint32 *)psx_addr(object + 36, 12));
        age = r_u32(object + 104);
    }
    ++age;
    w_u32(object + 104, age);
    if (age == 4)
        sub_80032ED8(object);
    return FadePackedColor((uint32 *)psx_addr(object + 76, 4), 16);
}

void ProjectileTracer::Update()
{
    UpdateProjectileTracerData(NativeObjectGuestAddress(this));
}

ProjectileTracer &ProjectileTracer::Attach(uint32 object)
{
    return *static_cast<ProjectileTracer *>(FindNativeObjectStorage(object));
}

uint32 ProjectileTracer::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002BC44, "SLUS_003.73");
    w_u32(object + 68, 0x800A1530);
    uint32 result = LineEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

extern "C" uint32 apocalypse_projectile_spawn_234CC(uint32 object, const sint32 start[3], const sint32 end[3])
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ProjectileTracer));
    new (dram_addr) ProjectileTracer((const uint32 *)start, (const uint32 *)end);
    return object;
}

extern "C" uint32 sub_800234CC(uint32 object, uint32 start, uint32 end)
{
    FUNCTION_MARKER(0x800234CC, "SLUS_003.73");
    return apocalypse_projectile_spawn_234CC(object, (const sint32 *)psx_addr(start, 12), (const sint32 *)psx_addr(end, 12));
}

extern "C" uint32 sub_800236D4(uint32 object)
{
    FUNCTION_MARKER(0x800236D4, "SLUS_003.73");
    return UpdateProjectileTracerData(object);
}
extern "C" uint32 sub_8002BC44(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002BC44, "SLUS_003.73");
    return ProjectileTracer::Attach(object).ProjectileTracer::Destroy(reason);
}

uint32 CreateProjectileImpact(uint32 flags, const uint32 *position)
{
    FUNCTION_MARKER(0x8002289C, "SLUS_003.73");
    uint32 effect;
    if (flags & 4)
    {
        effect = AllocateEffectStorage(120);
        if (effect)
        {
            void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(AnimatedSprite));
            new (dram_addr) AnimatedSprite(position, (uint32)0, 744, 1, 1, 0xFFFFFFFF);
        }
        // TODO: Resolve original null allocation accesses before frame selection
        w_u8(effect + 90, RandomRange(r_u8(effect + 89) - 1));
        w_u16(effect + 96, RandomRange(4096));
        w_u16(effect + 64, 100);
    }
    if (flags & 8)
    {
        for (uint32 index = 0; index < 4; ++index)
        {
            effect = AllocateEffectStorage(112);
            if (effect)
            {
                void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(ParticleSprite));
                new (dram_addr) ParticleSprite(position, 128, 128, 128, 6, 1000, 1, 40, 0x3000, 20);
            }
        }
    }
    uint32 result = flags & 16;
    if (result)
    {
        result = AllocateEffectStorage(96);
        if (result)
        {
            uint32 count = RandomRange(3) + 1;
            void *dram_addr = AllocateNativeObjectStorage(result, sizeof(SmokeEmitter));
            new (dram_addr) SmokeEmitter(position, 1, count, 128, 128, 128, 8, 5, 700, 700);
        }
    }
    return result;
}

extern "C" uint32 apocalypse_projectile_impact_2289C(uint32 flags, const sint32 position[3])
{
    return CreateProjectileImpact(flags, (const uint32 *)position);
}

extern "C" uint32 sub_8002289C(uint32 flags, uint32 position)
{
    FUNCTION_MARKER(0x8002289C, "SLUS_003.73");
    return CreateProjectileImpact(flags, (const uint32 *)psx_addr(position, 12));
}
