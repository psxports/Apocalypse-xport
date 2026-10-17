#include "visual_object.h"
#include "model_resources.h"
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
    this->model.scale[0] = 2048;
    this->model.scale[1] = 2048;
    this->model.scale[2] = 2048;
    flags = this->model.flags;
    misc = this->lifecycle_flags;
    this->model.flags = flags | 512;
    this->lifecycle_flags = misc & 0xFFFD;
}

void ProjectileBase::SetDamage(uint32 damage)
{
    projectile.damage = (uint16)damage;
}

void ProjectileBase::SetBlastRadius(uint32 radius)
{
    projectile.radius = (uint16)radius;
}

uint32 ProjectileBase::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, child, effect, table, target, result;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002BD74, "SLUS_003.73");
    w_u32(object + 68, 0x800A16F0);
    count = r_u32(0x800FF224);
    child = projectile.glow_object;
    w_u32(0x800FF224, count - 1);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    effect = projectile.trail_object;
    if (effect)
    {
        w_u8(effect + 66, 0);
        effect = projectile.trail_object;
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
    CollisionRay &shared_ray = GetSharedCollisionRay();
    uint32 object = NativeObjectGuestAddress(this);
    uint32 product[3], before[3], after[3];
    ModelInstance *node;
    uint32 x, y, z, value, distance;
    FUNCTION_MARKER(0x8002BE20, "SLUS_003.73");
    MultiplyVector(product, (const uint32 *)motion.velocity, (const uint32 *)&projectile.lifetime);
    AddVectors(before, (const uint32 *)this->model.pos, product);
    BuildDirectionVector(product, 100, (const uint16 *)this->model.angles);
    SubtractVectors(after, (const uint32 *)this->model.pos, product);
    x = after[0];
    y = after[1];
    z = after[2];
    shared_ray.pos[0][0] = (x);
    shared_ray.pos[0][1] = (y);
    shared_ray.pos[0][2] = (z);
    shared_ray.pos[1][0] = (before[0]);
    shared_ray.pos[1][1] = (before[1]);
    shared_ray.pos[1][2] = (before[2]);
    InitializeCollisionRay(shared_ray);
    w_u32(0x800FF974, 1);
    QueryCollisionRay(shared_ray, 0);
    w_u32(0x800FF974, 0);
    projectile.impact.dynamic = (uint8)(0);
    node = shared_ray.hit.GetObject();
    if (!node)
        return;
    projectile.impact.pending = (uint32)(1);
    distance = shared_ray.distance;
    value = (sint32)distance < 101 ? 0 : DivideProjectileTrace(distance - 100, scale);
    projectile.lifetime = (uint32)(value);
    node = shared_ray.hit.GetObject();
    projectile.impact.hit = shared_ray.hit;
    value = shared_ray.hit_pos[0];
    projectile.impact.pos[0] = (uint32)(value);
    value = shared_ray.hit_pos[1];
    projectile.impact.pos[1] = (uint32)(value);
    value = shared_ray.hit_pos[2];
    projectile.impact.pos[2] = (uint32)(value);
    value = (uint16)shared_ray.n.vx;
    w_u16(object + 164, value);
    value = (uint16)shared_ray.n.vy;
    w_u16(object + 166, value);
    value = (uint16)shared_ray.n.vz;
    node = projectile.impact.hit.GetObject();
    w_u16(object + 168, value);
    if (!(node->flags & 16))
        return;
    projectile.impact.dynamic = (uint8)(1);

}

uint32 ProjectileBase::Explode()
{
    const uint32 object = NativeObjectGuestAddress(this);
    ModelInstance *player;
    uint32 child, distance, radius, damage, kind;
    sint32 vector[3];
    FUNCTION_MARKER(0x8002BFFC, "SLUS_003.73");
    if (projectile.owner_list == &visual_lists.players)
    {
        child = AllocateEffectStorage(120);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExplosionRing));
            new (dram_addr) ExplosionRing();
            ExplosionRing::Attach(child).SetGeometry((const uint32 *)this->model.pos, 0, 0);
            w_u16(child + 10, 16);
            w_u16(child + 118, 16);
            w_u16(child + 116, 32);
        }
        player = visual_lists.players;
        if (player)
        {
            distance = MeasureEuclideanDistance((const uint32 *)this->model.pos, player->pos);
            radius = projectile.radius;
            if ((sint32)distance < (sint32)radius)
            {
                damage = projectile.damage * distance;
                if (!radius)
                    damage = (sint32)damage < 0 ? 1 : (uint32)-1;
                else
                    damage = (uint32)((sint32)damage / (sint32)radius);
                player = visual_lists.players;
                static_cast<VisualObject *>(player->owner)->Damage(damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            }
        }
    }
    else
    {
        child = AllocateEffectStorage(72);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(LaunchExplosionEffect));
            new (dram_addr) LaunchExplosionEffect((const uint32 *)this->model.pos);
        }
        kind = projectile.damage_kind;
        QueueExplosion((const uint32 *)this->model.pos, kind, 512, 32, 256);
    }
    if (projectile.impact.pending)
    {
        if (projectile.impact.dynamic)
        {
            ModelInstance *target = projectile.impact.hit.GetObject();
            if (target)
            {
                vector[0] = -(sint32)(sint16)r_u16(object + 164);
                vector[1] = -(sint32)(sint16)r_u16(object + 166);
                vector[2] = -(sint32)(sint16)r_u16(object + 168);
                damage = projectile.damage;
                static_cast<VisualObject *>(target->owner)->Damage(damage, vector, 1);
            }
        }
        else
        {
            ModelInstance *target = projectile.impact.hit.GetObject();
            damage = projectile.damage;
            DamageLevelObject(target, projectile.impact.hit.GetPolygon(), (sint32)damage);
        }
    }
    return Finish();
}

uint32 ProjectileBase::SyncChild()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child;
    FUNCTION_MARKER(0x8002C250, "SLUS_003.73");
    BuildDirectionVector((uint32 *)projectile.glow_pos, projectile.glow_distance, (const uint16 *)this->model.angles);
    AddVectors((uint32 *)projectile.glow_pos, (const uint32 *)this->model.pos);
    child = projectile.glow_object;
    return RingEffect::Attach(child).SetPosition((const uint32 *)projectile.glow_pos);
}

uint32 ProjectileBase::AdvanceMotion()
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, value, damage, kind;
    ModelInstance *hit;
    uint32 x_sum, y_sum, x_shift, y_shift, y_word;
    sint32 x_signed, y_signed;
    FUNCTION_MARKER(0x8002C2A4, "SLUS_003.73");
    x = this->model.pos[0];
    y = this->model.pos[1];
    z = this->model.pos[2];
    pos_history.prev[0] = x;
    pos_history.prev[1] = y;
    pos_history.prev[2] = z;
    AddVectors((uint32 *)this->model.pos, (const uint32 *)motion.velocity);
    if (r_u16(object + 148))
    {
        value = pos_history.prev[0];
        shared_ray.pos[0][0] = (value);
        value = pos_history.prev[1];
        shared_ray.pos[0][1] = (value);
        value = pos_history.prev[2];
        shared_ray.pos[0][2] = (value);
        value = this->model.pos[0];
        shared_ray.pos[1][0] = (value);
        value = this->model.pos[1];
        shared_ray.pos[1][1] = (value);
        value = this->model.pos[2];
        shared_ray.pos[1][2] = (value);
        InitializeCollisionRay(shared_ray);
        w_u32(0x800FF974, 1);
        QueryCollisionRay(shared_ray, 1);
        hit = shared_ray.hit.GetObject();
        w_u32(0x800FF974, 0);
        if (hit)
        {
            if (hit->flags & 0x10)
            {
                damage = projectile.damage;
                kind = projectile.damage_kind;
                static_cast<VisualObject *>(hit->owner)->Damage(damage, (const sint32 *)psx_addr(0x800A71CC, 12), kind);
            }
            else
            {
                damage = projectile.damage;
                DamageLevelObject(hit, shared_ray.hit.GetPolygon(), (sint32)damage);
            }
            Explode();
        }
    }
    AddAngleVector((uint16 *)this->model.angles, (const uint16 *)motion.angular_velocity);
    NormalizeAngles((uint16 *)this->model.angles);
    x = motion.angular_velocity[0];
    x_shift = motion.angular_acceleration[0];
    y = motion.angular_velocity[1];
    y_shift = motion.angular_acceleration[1];
    x_sum = x + x_shift;
    y_sum = y + y_shift;
    motion.angular_velocity[0] = x_sum;
    motion.angular_velocity[1] = y_sum;
    x_signed = (sint16)motion.angular_velocity[0];
    x_shift = motion.angular_drag[0] & 31;
    y_signed = (sint16)motion.angular_velocity[1];
    y_shift = motion.angular_drag[1] & 31;
    motion.angular_velocity[0] = x_sum - (uint32)(x_signed >> x_shift);
    y_word = motion.angular_velocity[1];
    motion.angular_velocity[1] = y_word - (uint32)(y_signed >> y_shift);
    return SnapSmallAngles((uint16 *)motion.angular_velocity);
}

void ProjectileBase::ApplyBlast(ModelInstance *list, uint32 radius, uint32 impulse, uint32 kind)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 delta[3], product[3], vector[3], distance, damage, reduction;
    FUNCTION_MARKER(0x8002C460, "SLUS_003.73");
    while (list)
    {
        SubtractVectors(delta, list->pos, (const uint32 *)this->model.pos);
        distance = MeasureVectorLength(delta);
        if ((sint32)radius >= (sint32)distance)
        {
            damage = projectile.damage;
            reduction = DivideProjectileTrace((damage >> 1) * distance, radius);
            damage -= reduction;
            MultiplyScalarVector(product, &impulse, delta);
            DivideVector(vector, product, &distance);
            static_cast<VisualObject *>(list->owner)->Damage(damage, (const sint32 *)vector, kind);
        }
        list = list->next;
    }
}

void ProjectileBase::DamageNearbyObjects(uint32 radius)
{
    uint32 object = NativeObjectGuestAddress(this);
    ModelBorrow borrow;
    ModelInstance *list;
    uint32 bound, x, z, delta, position[3], damage;
    FUNCTION_MARKER(0x8002C56C, "SLUS_003.73");
    list = GetModelSceneObjects();
    bound = radius << 12;
    while (list)
    {
        x = list->pos[0];
        delta = x - this->model.pos[0];
        if ((sint32)delta < 0)
            delta = 0 - delta;
        if ((sint32)delta < (sint32)bound)
        {
            z = list->pos[2];
            delta = z - this->model.pos[2];
            if ((sint32)delta < 0)
                delta = 0 - delta;
            if ((sint32)delta < (sint32)bound)
            {
                position[1] = list->pos[1];
                position[0] = x;
                position[2] = z;
                if ((sint32)MeasurePositionDistance((const uint32 *)this->model.pos, position) < (sint32)radius)
                {
                    damage = projectile.damage;
                    DamageLevelObject(list, 0, (sint32)damage);
                }
            }
        }
        list = list->next;
    }
}

uint32 ProjectileBase::SetTrailColor(uint32 kind, uint32 red, uint32 green, uint32 blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002C654, "SLUS_003.73");
    projectile.trail_rgb[0] = (uint8)(red);
    projectile.trail_rgb[1] = (uint8)(green);
    projectile.trail_kind = (uint8)(kind);
    projectile.trail_rgb[2] = (uint8)(blue);
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



extern "C" uint32 sub_8002BD74(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002BD74, "SLUS_003.73");
    return ProjectileBase::Attach(object).ProjectileBase::Destroy(reason);
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

