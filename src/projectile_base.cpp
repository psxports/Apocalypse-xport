#include "object_collision_events.h"
#include "vector_distance.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_base.h"
#include "ring_effect.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "angle_math.h"
#include "game_heap.h"
#include "object_damage.h"
#include "effect_launch_explosion.h"
#include "explosion_effects.h"
#include <new>
#include "projectile_tracking.h"

static uint32 DivideProjectileTrace(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

ProjectileBase::ProjectileBase() : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, flags, misc;
    FUNCTION_MARKER(0x8002BCF8, "SLUS_003.73");
    count = r_u32(0x800FF224);
    w_u32(object + 68, 0x800A16F0);
    w_u32(0x800FF224, count + 1);
    SetModelBank((const char *)psx_addr(0x800FF228, 1));
    w_u16(object + 36, 2048);
    w_u16(object + 38, 2048);
    w_u16(object + 40, 2048);
    flags = r_u16(object);
    misc = r_u16(object + 78);
    w_u16(object, flags | 512);
    w_u16(object + 78, misc & 0xFFFD);
}

uint32 ProjectileBase::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, child, effect, table, target, result;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002BD74, "SLUS_003.73");
    w_u32(object + 68, 0x800A16F0);
    count = r_u32(0x800FF224);
    child = r_u32(object + 316);
    w_u32(0x800FF224, count - 1);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    effect = r_u32(object + 336);
    if (effect)
    {
        w_u8(effect + 66, 0);
        effect = r_u32(object + 336);
        w_u32(effect + 96, 1);
    }
    VisualObject::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(object);
    return result;
}

void ProjectileBase::PrepareTrajectory(uint32 scale)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 product[3], before[3], after[3], metadata[2];
    uint32 x, y, z, node, value, distance;
    FUNCTION_MARKER(0x8002BE20, "SLUS_003.73");
    MultiplyVector(product, (const uint32 *)psx_addr(object + 104, 12), (const uint32 *)psx_addr(object + 300, 4));
    AddVectors(before, (const uint32 *)psx_addr(object + 4, 12), product);
    BuildDirectionVector(product, 100, (const uint16 *)psx_addr(object + 16, 6));
    SubtractVectors(after, (const uint32 *)psx_addr(object + 4, 12), product);
    x = after[0];
    y = after[1];
    z = after[2];
    w_u32(0x800ED638, x);
    w_u32(0x800ED63C, y);
    w_u32(0x800ED640, z);
    w_u32(0x800ED644, before[0]);
    w_u32(0x800ED648, before[1]);
    w_u32(0x800ED64C, before[2]);
    InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
    w_u32(0x800FF974, 1);
    QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 0);
    w_u32(0x800FF974, 0);
    w_u8(object + 364, 0);
    node = r_u32(0x800ED6A0);
    if (!node)
        return;
    w_u32(object + 340, 1);
    distance = r_u32(0x800ED678);
    value = (sint32)distance < 101 ? 0 : DivideProjectileTrace(distance - 100, scale);
    w_u32(object + 300, value);
    node = r_u32(0x800ED6A0);
    w_u32(object + 344, node);
    value = r_u32(0x800ED6B8);
    w_u32(object + 348, value);
    value = r_u32(0x800ED6A4);
    w_u32(object + 352, value);
    value = r_u32(0x800ED6A8);
    w_u32(object + 356, value);
    value = r_u32(0x800ED6AC);
    w_u32(object + 360, value);
    value = r_u16(0x800ED6B0);
    w_u16(object + 164, value);
    value = r_u16(0x800ED6B2);
    w_u16(object + 166, value);
    value = r_u16(0x800ED6B4);
    node = r_u32(object + 344);
    w_u16(object + 168, value);
    if (!(r_u16(node) & 16))
        return;
    w_u8(object + 364, 1);
    MakeObjectReference(metadata, node);
    w_u32(object + 368, metadata[0]);
    w_u32(object + 372, metadata[1]);
}

uint32 ProjectileBase::Explode()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, player, distance, radius, damage, table, target, kind;
    sint32 adjustment, vector[3];
    FUNCTION_MARKER(0x8002BFFC, "SLUS_003.73");
    if (r_u32(object + 304) == 0x800FF5A0)
    {
        child = AllocateEffectStorage(120);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExplosionRing));
            new (dram_addr) ExplosionRing();
            ExplosionRing::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 4, 12), 0, 0);
            w_u16(child + 10, 16);
            w_u16(child + 118, 16);
            w_u16(child + 116, 32);
        }
        player = r_u32(0x800FF5A0);
        if (player)
        {
            distance = MeasureEuclideanDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(player + 4, 12));
            radius = r_u16(object + 310);
            if ((sint32)distance < (sint32)radius)
            {
                damage = r_u16(object + 308) * distance;
                if (!radius)
                    damage = (sint32)damage < 0 ? 1 : (uint32)-1;
                else
                    damage = (uint32)((sint32)damage / (sint32)radius);
                player = r_u32(0x800FF5A0);
                table = r_u32(player + 68);
                adjustment = (sint16)r_u16(table + 48);
                target = r_u32(table + 52);
                apocalypse_object_virtual52_native(target, player + (uint32)adjustment, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            }
        }
    }
    else
    {
        child = AllocateEffectStorage(72);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(LaunchExplosionEffect));
            new (dram_addr) LaunchExplosionEffect((const uint32 *)psx_addr(object + 4, 12));
        }
        kind = r_u32(object + 312);
        QueueExplosion((const uint32 *)psx_addr(object + 4, 12), kind, 512, 32, 256);
    }
    if (r_u32(object + 340))
    {
        if (r_u8(object + 364))
        {
            child = ResolveHeapReference((uint32 *)psx_addr(object + 368, 8));
            if (child)
            {
                vector[0] = -(sint32)(sint16)r_u16(object + 164);
                vector[1] = -(sint32)(sint16)r_u16(object + 166);
                vector[2] = -(sint32)(sint16)r_u16(object + 168);
                table = r_u32(child + 68);
                damage = r_u16(object + 308);
                adjustment = (sint16)r_u16(table + 48);
                target = r_u32(table + 52);
                apocalypse_object_virtual52_native(target, child + (uint32)adjustment, damage, vector, 1);
            }
        }
        else
        {
            child = r_u32(object + 344);
            target = r_u32(object + 348);
            damage = r_u16(object + 308);
            DamageLevelObject(child, target, (sint32)damage);
        }
    }
    table = r_u32(object + 68);
    adjustment = (sint16)r_u16(table + 16);
    target = r_u32(table + 20);
    return xport_draft_guest_call1(target, object + (uint32)adjustment);
}

uint32 ProjectileBase::SyncChild()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child;
    FUNCTION_MARKER(0x8002C250, "SLUS_003.73");
    BuildDirectionVector((uint32 *)psx_addr(object + 324, 12), r_u32(object + 320), (const uint16 *)psx_addr(object + 16, 6));
    AddVectors((uint32 *)psx_addr(object + 324, 12), (const uint32 *)psx_addr(object + 4, 12));
    child = r_u32(object + 316);
    return RingEffect::Attach(child).SetPosition((const uint32 *)psx_addr(object + 324, 12));
}

uint32 ProjectileBase::AdvanceMotion()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, value, hit, table, target, damage, kind, receiver;
    uint32 x_sum, y_sum, x_shift, y_shift, y_word;
    sint32 adjustment, x_signed, y_signed;
    FUNCTION_MARKER(0x8002C2A4, "SLUS_003.73");
    x = r_u32(object + 4);
    y = r_u32(object + 8);
    z = r_u32(object + 12);
    w_u32(object + 228, x);
    w_u32(object + 232, y);
    w_u32(object + 236, z);
    AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
    if (r_u16(object + 148))
    {
        value = r_u32(object + 228);
        w_u32(0x800ED638, value);
        value = r_u32(object + 232);
        w_u32(0x800ED63C, value);
        value = r_u32(object + 236);
        w_u32(0x800ED640, value);
        value = r_u32(object + 4);
        w_u32(0x800ED644, value);
        value = r_u32(object + 8);
        w_u32(0x800ED648, value);
        value = r_u32(object + 12);
        w_u32(0x800ED64C, value);
        InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
        w_u32(0x800FF974, 1);
        QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
        hit = r_u32(0x800ED6A0);
        w_u32(0x800FF974, 0);
        if (hit)
        {
            if (r_u16(hit) & 0x10)
            {
                table = r_u32(hit + 68);
                damage = r_u16(object + 308);
                kind = r_u32(object + 312);
                adjustment = (sint16)r_u16(table + 48);
                target = r_u32(table + 52);
                receiver = hit + (uint32)adjustment;
                apocalypse_object_virtual52_native(target, receiver, damage, (const sint32 *)psx_addr(0x800A71CC, 12), kind);
            }
            else
            {
                target = r_u32(0x800ED6B8);
                damage = r_u16(object + 308);
                DamageLevelObject(hit, target, (sint32)damage);
            }
            table = r_u32(object + 68);
            adjustment = (sint16)r_u16(table + 64);
            target = r_u32(table + 68);
            receiver = object + (uint32)adjustment;
            if (target == 0x8002D2F0)
                sub_8002D2F0(receiver);
            else
                xport_draft_guest_call1(target, receiver);
        }
    }
    AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 132, 6));
    NormalizeAngles((uint16 *)psx_addr(object + 16, 6));
    x = r_u16(object + 132);
    x_shift = r_u16(object + 138);
    y = r_u16(object + 134);
    y_shift = r_u16(object + 140);
    x_sum = x + x_shift;
    y_sum = y + y_shift;
    w_u16(object + 132, x_sum);
    w_u16(object + 134, y_sum);
    x_signed = (sint16)r_u16(object + 132);
    x_shift = r_u8(object + 144) & 31;
    y_signed = (sint16)r_u16(object + 134);
    y_shift = r_u8(object + 145) & 31;
    w_u16(object + 132, x_sum - (uint32)(x_signed >> x_shift));
    y_word = r_u16(object + 134);
    w_u16(object + 134, y_word - (uint32)(y_signed >> y_shift));
    return SnapSmallAngles((uint16 *)psx_addr(object + 132, 6));
}

void ProjectileBase::ApplyBlast(uint32 list, uint32 radius, uint32 impulse, uint32 kind)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 delta[3], product[3], vector[3], distance, damage, reduction;
    FUNCTION_MARKER(0x8002C460, "SLUS_003.73");
    while (list)
    {
        SubtractVectors(delta, (const uint32 *)psx_addr(list + 4, 12), (const uint32 *)psx_addr(object + 4, 12));
        distance = MeasureVectorLength(delta);
        if ((sint32)radius >= (sint32)distance)
        {
            damage = r_u16(object + 308);
            reduction = DivideProjectileTrace((damage >> 1) * distance, radius);
            damage -= reduction;
            MultiplyScalarVector(product, &impulse, delta);
            DivideVector(vector, product, &distance);
            ApplyObjectDamage(list, damage, (const sint32 *)vector, kind);
        }
        list = r_u32(list + 28);
    }
}

void ProjectileBase::DamageNearbyObjects(uint32 radius)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 list, bound, x, z, delta, position[3], damage;
    FUNCTION_MARKER(0x8002C56C, "SLUS_003.73");
    list = r_u32(0x800FF794);
    bound = radius << 12;
    while (list)
    {
        x = r_u32(list + 4);
        delta = x - r_u32(object + 4);
        if ((sint32)delta < 0)
            delta = 0 - delta;
        if ((sint32)delta < (sint32)bound)
        {
            z = r_u32(list + 12);
            delta = z - r_u32(object + 12);
            if ((sint32)delta < 0)
                delta = 0 - delta;
            if ((sint32)delta < (sint32)bound)
            {
                position[1] = r_u32(list + 8);
                position[0] = x;
                position[2] = z;
                if ((sint32)MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), position) < (sint32)radius)
                {
                    damage = r_u16(object + 308);
                    DamageLevelObject(list, 0, (sint32)damage);
                }
            }
        }
        list = r_u32(list + 28);
    }
}

uint32 ProjectileBase::SetTrailColor(uint32 kind, uint32 red, uint32 green, uint32 blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002C654, "SLUS_003.73");
    w_u8(object + 376, red);
    w_u8(object + 377, green);
    w_u8(object + 379, kind);
    w_u8(object + 378, blue);
    return blue;
}

ProjectileBase &ProjectileBase::Attach(uint32 object)
{
    return *static_cast<ProjectileBase *>(FindNativeObjectStorage(object));
}

uint32 ProjectileBase::ReadAnglesWord(uint32 address)
{
    uint32 high = r_u32((address + 3) & ~3);
    uint32 low = r_u32(address & ~3);
    uint32 shift = (address & 3) << 3;
    return shift ? (high << (32 - shift)) | (low >> shift) : low;
}

uint32 SetProjectileTrailColor(uint32 object, uint32 kind, uint32 red, uint32 green, uint32 blue)
{
    return ProjectileBase::Attach(object).SetTrailColor(kind, red, green, blue);
}

extern "C" uint32 sub_8002BCF8(uint32 object)
{
    FUNCTION_MARKER(0x8002BCF8, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(ProjectileBase));
    new (storage) ProjectileBase();
    return object;
}

extern "C" uint32 sub_8002BD74(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002BD74, "SLUS_003.73");
    return ProjectileBase::Attach(object).ProjectileBase::Destroy(reason);
}

extern "C" void sub_8002BE20(uint32 object, uint32 scale)
{
    FUNCTION_MARKER(0x8002BE20, "SLUS_003.73");
    ProjectileBase::Attach(object).PrepareTrajectory(scale);
}

extern "C" void sub_8002C460(uint32 object, uint32 list, uint32 radius, uint32 impulse, uint32 kind)
{
    FUNCTION_MARKER(0x8002C460, "SLUS_003.73");
    ProjectileBase::Attach(object).ApplyBlast(list, radius, impulse, kind);
}
extern "C" void sub_8002C56C(uint32 object, uint32 radius)
{
    FUNCTION_MARKER(0x8002C56C, "SLUS_003.73");
    ProjectileBase::Attach(object).DamageNearbyObjects(radius);
}

extern "C" uint32 sub_8002C250(uint32 object)
{
    FUNCTION_MARKER(0x8002C250, "SLUS_003.73");
    return ProjectileBase::Attach(object).SyncChild();
}

extern "C" uint32 sub_8002C2A4(uint32 object)
{
    FUNCTION_MARKER(0x8002C2A4, "SLUS_003.73");
    return ProjectileBase::Attach(object).AdvanceMotion();
}

extern "C" uint32 sub_8002BFFC(uint32 object)
{
    FUNCTION_MARKER(0x8002BFFC, "SLUS_003.73");
    return ProjectileBase::Attach(object).ProjectileBase::Explode();
}

ExplosionRing::ExplosionRing() : MeshRingEffect(12, 0xCCE0187C)
{
    const uint32 object = NativeObjectGuestAddress(this);
    // Original constructor is inlined in ProjectileBase::Explode
    w_u32(object + 68, 0x800A15F8);
    w_u32(object + 36, 0);
    w_u32(object + 40, (uint32)-32768);
    w_u32(object + 44, 0);
}

uint32 ExplosionRing::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8002E0C4, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

uint32 ExplosionRing::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, lifetime, radius_step, radius, width_step, width, value, color;
    FUNCTION_MARKER(0x8002E008, "SLUS_003.73");
    age = r_u16(object + 8);
    lifetime = r_u16(object + 10);
    age += 1;
    w_u16(object + 8, age);
    if ((sint32)lifetime < (sint16)age)
        return Finish();
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    radius_step = (uint32)(sint32)(sint16)r_u16(object + 116);
    radius = r_u32(object + 92);
    width_step = (uint32)(sint32)(sint16)r_u16(object + 118);
    width = r_u32(object + 96);
    SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius + radius_step, width + width_step);
    lifetime = r_u16(object + 10);
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    value = lifetime - age;
    value = (value << 8) - value;
    color = lifetime ? (uint32)((sint32)value / (sint32)lifetime) : ((sint32)value < 0 ? 1 : (uint32)-1);
    return SetColors(color, color, (uint32)((sint32)color >> 1));
}

ExplosionRing &ExplosionRing::Attach(uint32 object)
{
    return *static_cast<ExplosionRing *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8002E0C4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002E0C4, "SLUS_003.73");
    return ExplosionRing::Attach(object).ExplosionRing::Destroy(reason);
}

extern "C" uint32 sub_8002E008(uint32 object)
{
    FUNCTION_MARKER(0x8002E008, "SLUS_003.73");
    return ExplosionRing::Attach(object).ExplosionRing::Update();
}

