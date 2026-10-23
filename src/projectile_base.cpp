#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "model_bank.h"
#include "collision.h"
#include "global.h"
#include "game_diagnostics.h"
#include "projectile_base.h"
#include "sprite_trail.h"
#include "ring_effect.h"
#include "game_object_allocation.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "game_heap.h"
#include "object_damage.h"
#include "launch_explosion_effect.h"
#include "explosion_effects.h"
#include "tracking_projectile.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"

static uint32 projectile_count;

void ProjectileBase::ResetPopulation()
{
    projectile_count = 0;
}

ProjectileBase::ProjectileBase() : VisualObject(), trace_movement(false), age(0), lifetime(0), owner_list(0), damage(0), radius(0), damage_kind(0), glow_object(), glow_distance(0), trail_object(), impact_pending(false), impact_target(), impact_dynamic(false), trail_r(0), trail_g(0), trail_b(0), trail_kind(0)
{
    FUNCTION_MARKER(0x8002BCF8, "SLUS_003.73");
    glow_pos.x = glow_pos.y = glow_pos.z = 0;
    ++projectile_count;
    SetModelBank("items");
    model.scale.x = 2048;
    model.scale.y = 2048;
    model.scale.z = 2048;
    model.flags |= 512;
    lifecycle_flags &= 0xFFFD;
}

uint32 ProjectileBase::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002BD74, "SLUS_003.73");
    --projectile_count;
    if (glow_object)
        glow_object->Destroy(3);
    glow_object = 0;
    if (trail_object)
    {
        trail_object->persistent = false;
        trail_object->fading = true;
    }
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void ProjectileBase::PrepareTrajectory(uint32 scale)
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    Vector3 product, before, after;
    ModelInstance *node;
    sint32 distance;
    FUNCTION_MARKER(0x8002BE20, "SLUS_003.73");
    product.Multiply(motion.velocity, lifetime);
    before.Add(model.pos, product);
    product.SetDir(100, model.angles);
    after.Subtract(model.pos, product);
    shared_ray.endpoints[0] = after;
    shared_ray.endpoints[1] = before;
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(0);
    col_query.reject_surface40 = false;
    node = shared_ray.hit.GetObject();
    if (!node && impact_pending && impact_dynamic && !impact_target.GetObject())
    {
        GameDiagnostic(("Projectile missed trace retains an expired impact target\n"));
        abort();
    }
    impact_dynamic = false;
    if (!node)
        return;
    impact_pending = true;
    distance = shared_ray.distance;
    lifetime = distance < 101 ? 0 : DivideSignedWord(distance - 100, scale);
    impact_target = shared_ray.hit;
    surface.n.vx = shared_ray.n.vx;
    surface.n.vy = shared_ray.n.vy;
    surface.n.vz = shared_ray.n.vz;
    node = impact_target.GetObject();
    if (!(node->flags & 16))
        return;
    impact_dynamic = true;
}

uint32 ProjectileBase::Explode()
{
    ModelInstance *player;
    GameHeapBlock *child;
    uint32 distance;
    uint16 radius;
    uint32 damage;
    Vector3 vector;
    FUNCTION_MARKER(0x8002BFFC, "SLUS_003.73");
    if (owner_list == &visual_lists.players)
    {
        child = EffectObject::AllocateStorage(120);
        if (child)
        {
            ExplosionRing *ring = new (child) ExplosionRing();
            ring->SetGeometry(&model.pos, 0, 0);
            ring->lifetime = 16;
            ring->width_step = 16;
            ring->radius_step = 32;
        }
        player = visual_lists.players;
        if (player)
        {
            distance = model.pos.EuclideanDistance(player->pos);
            radius = this->radius;
            if ((sint32)distance < radius)
            {
                damage = this->damage * distance;
                damage = DivideSignedWord(damage, radius);
                player = visual_lists.players;
                static_cast<VisualObject *>(player->owner)->Damage(damage, &zero_vector, 0);
            }
        }
    }
    else
    {
        child = EffectObject::AllocateStorage(72);
        if (child)
        {
            new (child) LaunchExplosionEffect(&model.pos);
        }
        QueueExplosion(&model.pos, 512, 32, 256);
    }
    if (impact_pending)
    {
        if (impact_dynamic)
        {
            ModelInstance *target = impact_target.GetObject();
            if (target)
            {
                vector.x = -surface.n.vx;
                vector.y = -surface.n.vy;
                vector.z = -surface.n.vz;
                damage = this->damage;
                static_cast<VisualObject *>(target->owner)->Damage(damage, &vector, 1);
            }
        }
        else
        {
            ModelInstance *target = impact_target.GetObject();
            damage = this->damage;
            DamageLevelObject(target, impact_target.GetPolygon(), (sint32)damage);
        }
    }
    return Finish();
}

uint32 ProjectileBase::SyncChild()
{
    FUNCTION_MARKER(0x8002C250, "SLUS_003.73");
    glow_pos.SetDir(glow_distance, model.angles);
    glow_pos.Add(model.pos);
    if (!glow_object)
    {
        GameDiagnostic(("Projectile child synchronization has no glow\n"));
        abort();
    }
    return glow_object->SetPos(&glow_pos);
}

uint32 ProjectileBase::AdvanceMotion()
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    ModelInstance *hit;
    FUNCTION_MARKER(0x8002C2A4, "SLUS_003.73");
    pos_history.prev = model.pos;
    model.pos.Add(motion.velocity);
    if (trace_movement)
    {
        shared_ray.endpoints[0] = pos_history.prev;
        shared_ray.endpoints[1] = model.pos;
        shared_ray.Initialize();
        col_query.reject_surface40 = true;
        shared_ray.Query(1);
        hit = shared_ray.hit.GetObject();
        col_query.reject_surface40 = false;
        if (hit)
        {
            if (hit->flags & 0x10)
            {
                static_cast<VisualObject *>(hit->owner)->Damage(damage, &zero_vector, damage_kind);
            }
            else
            {
                DamageLevelObject(hit, shared_ray.hit.GetPolygon(), damage);
            }
            Explode();
        }
    }
    model.angles.AddAngles(motion.angular_velocity);
    model.angles.NormalizeAngles();
    motion.angular_velocity.x += motion.angular_acceleration.x;
    motion.angular_velocity.y += motion.angular_acceleration.y;
    motion.angular_velocity.x -= motion.angular_velocity.x >> (motion.angular_drag[0] & 31);
    motion.angular_velocity.y -= motion.angular_velocity.y >> (motion.angular_drag[1] & 31);
    return motion.angular_velocity.SnapAngles();
}

void ProjectileBase::ApplyBlast(ModelInstance *list, sint32 radius, uint32 impulse, uint32 kind)
{
    Vector3 delta, product, vector;
    uint32 distance, damage, reduction;
    FUNCTION_MARKER(0x8002C460, "SLUS_003.73");
    while (list)
    {
        delta.Subtract(list->pos, model.pos);
        distance = delta.Len();
        if (radius >= (sint32)distance)
        {
            damage = this->damage;
            reduction = DivideSignedWord((damage >> 1) * distance, radius);
            damage -= reduction;
            product.Scale(impulse, delta);
            vector.Divide(product, distance);
            static_cast<VisualObject *>(list->owner)->Damage(damage, &vector, kind);
        }
        list = list->next;
    }
}

void ProjectileBase::DamageNearbyObjects(sint32 radius)
{
    ModelBorrow borrow;
    ModelInstance *list;
    Vector3 pos;
    uint32 bound, x, z, delta;
    FUNCTION_MARKER(0x8002C56C, "SLUS_003.73");
    list = ModelBank::GetSceneObjects();
    bound = (uint32)radius << 12;
    while (list)
    {
        x = (uint32)list->pos.x;
        delta = x - (uint32)model.pos.x;
        if ((sint32)delta < 0)
            delta = -delta;
        if ((sint32)delta < (sint32)bound)
        {
            z = (uint32)list->pos.z;
            delta = z - (uint32)model.pos.z;
            if ((sint32)delta < 0)
                delta = -delta;
            if ((sint32)delta < (sint32)bound)
            {
                pos.y = list->pos.y;
                pos.x = x;
                pos.z = z;
                if ((sint32)model.pos.Distance(pos) < radius)
                {
                    DamageLevelObject(list, 0, damage);
                }
            }
        }
        list = list->next;
    }
}

uint32 ProjectileBase::SetTrailColor(uint8 kind, uint8 r, uint8 g, uint32 b)
{
    FUNCTION_MARKER(0x8002C654, "SLUS_003.73");
    trail_r = r;
    trail_g = g;
    trail_kind = kind;
    trail_b = b;
    return b;
}

void ProjectileBase::SetDamage(uint16 damage)
{
    this->damage = damage;
}

void ProjectileBase::SetBlastRadius(uint16 radius)
{
    this->radius = radius;
}

ProjectileBase::~ProjectileBase()
{
    if (glow_object)
        glow_object->Destroy(3);
    if (trail_object)
    {
        trail_object->persistent = false;
        trail_object->fading = true;
    }
}
