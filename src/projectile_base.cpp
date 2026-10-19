#include <new>

#include "object_initializers.h"

#include "visual_object.h"
#include "model_resources.h"
#include "collision.h"
#include "vector_distance.h"
#include "game_diagnostics.h"
#include "projectile_base.h"
#include "sprite_trail.h"
#include "ring_effect.h"
#include "game_object_allocation.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "angle_math.h"
#include "game_heap.h"
#include "object_damage.h"
#include "effect_launch_explosion.h"
#include "explosion_effects.h"
#include "projectile_tracking.h"

static uint32 projectile_count;

void ResetProjectilePopulation()
{
    projectile_count = 0;
}

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
    uint32 flags, misc;
    FUNCTION_MARKER(0x8002BCF8, "SLUS_003.73");
    ++projectile_count;
    SetModelBank("items");
    this->model.scale.words[0] = 2048;
    this->model.scale.words[1] = 2048;
    this->model.scale.words[2] = 2048;
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

ProjectileBase::~ProjectileBase()
{
    if (projectile.glow_object)
        projectile.glow_object->Destroy(3);
    if (projectile.trail_object)
    {
        projectile.trail_object->persistent = 0;
        projectile.trail_object->fading = 1;
    }
}

uint32 ProjectileBase::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002BD74, "SLUS_003.73");
    --projectile_count;
    if (projectile.glow_object)
        projectile.glow_object->Destroy(3);
    projectile.glow_object = 0;
    if (projectile.trail_object)
    {
        projectile.trail_object->persistent = 0;
        projectile.trail_object->fading = 1;
    }
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseObjectStorage(this) : 0;
}

void ProjectileBase::PrepareTrajectory(uint32 scale)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    Vector3 product, before, after;
    ModelInstance *node;
    uint32 value, distance;
    FUNCTION_MARKER(0x8002BE20, "SLUS_003.73");
    MultiplyVector(product.words, motion.velocity.words, projectile.lifetime);
    AddVectors(before.words, this->model.pos.words, product.words);
    BuildDirectionVector(product.words, 100, this->model.angles.words);
    SubtractVectors(after.words, this->model.pos.words, product.words);
    shared_ray.endpoints[0] = after;
    shared_ray.endpoints[1] = before;
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 0);
    collision_query.reject_surface40 = false;
    node = shared_ray.hit.GetObject();
    if (!node && projectile.impact.pending && projectile.impact.dynamic && !projectile.impact.hit.GetObject())
    {
        GameDiagnostic("Projectile missed trace retains an expired impact target\n");
        abort();
    }
    projectile.impact.dynamic = (uint8)(0);
    if (!node)
        return;
    projectile.impact.pending = (uint32)(1);
    distance = shared_ray.distance;
    value = (sint32)distance < 101 ? 0 : DivideProjectileTrace(distance - 100, scale);
    projectile.lifetime = (uint32)(value);
    node = shared_ray.hit.GetObject();
    projectile.impact.hit = shared_ray.hit;
    projectile.impact.pos = shared_ray.hit_pos;
    value = (uint16)shared_ray.n.vx;
    surface.n.vx = value;
    value = (uint16)shared_ray.n.vy;
    surface.n.vy = value;
    value = (uint16)shared_ray.n.vz;
    node = projectile.impact.hit.GetObject();
    surface.n.vz = value;
    if (!(node->flags & 16))
        return;
    projectile.impact.dynamic = (uint8)(1);
}

uint32 ProjectileBase::Explode()
{
    ModelInstance *player;
    GameHeapBlock *child;
    uint32 distance;
    uint32 radius;
    uint32 damage;
    Vector3 vector;
    FUNCTION_MARKER(0x8002BFFC, "SLUS_003.73");
    if (projectile.owner_list == &visual_lists.players)
    {
        child = AllocateEffectStorage(120);
        if (child)
        {
            ExplosionRing *ring = new (child) ExplosionRing();
            ring->SetGeometry(this->model.pos.words, 0, 0);
            ring->lifetime = 16;
            ring->width_step = 16;
            ring->radius_step = 32;
        }
        player = visual_lists.players;
        if (player)
        {
            distance = MeasureEuclideanDistance(this->model.pos.words, player->pos.words);
            radius = projectile.radius;
            if ((sint32)distance < (sint32)radius)
            {
                damage = projectile.damage * distance;
                if (!radius)
                    damage = (sint32)damage < 0 ? 1 : (uint32)-1;
                else
                    damage = (uint32)((sint32)damage / (sint32)radius);
                player = visual_lists.players;
                static_cast<VisualObject *>(player->owner)->Damage(damage, zero_vector.components, 0);
            }
        }
    }
    else
    {
        child = AllocateEffectStorage(72);
        if (child)
        {
            new (child) LaunchExplosionEffect(this->model.pos.words);
        }
        QueueExplosion(model.pos.words, 512, 32, 256);
    }
    if (projectile.impact.pending)
    {
        if (projectile.impact.dynamic)
        {
            ModelInstance *target = projectile.impact.hit.GetObject();
            if (target)
            {
                vector.components[0] = -(sint32)(sint16)(uint16)surface.n.vx;
                vector.components[1] = -(sint32)(sint16)(uint16)surface.n.vy;
                vector.components[2] = -(sint32)(sint16)(uint16)surface.n.vz;
                damage = projectile.damage;
                static_cast<VisualObject *>(target->owner)->Damage(damage, vector.components, 1);
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
    FUNCTION_MARKER(0x8002C250, "SLUS_003.73");
    BuildDirectionVector(projectile.glow_pos.words, projectile.glow_distance, this->model.angles.words);
    AddVectors(projectile.glow_pos.words, this->model.pos.words);
    if (!projectile.glow_object)
    {
        GameDiagnostic("Projectile child synchronization has no glow\n");
        abort();
    }
    return projectile.glow_object->SetPosition(projectile.glow_pos.words);
}

uint32 ProjectileBase::AdvanceMotion()
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    uint32 x, y, damage, kind;
    ModelInstance *hit;
    uint32 x_sum, y_sum, x_shift, y_shift, y_word;
    sint32 x_signed, y_signed;
    FUNCTION_MARKER(0x8002C2A4, "SLUS_003.73");
    pos_history.prev = this->model.pos;
    AddVectors(this->model.pos.words, motion.velocity.words);
    if (projectile.trace_movement)
    {
        shared_ray.endpoints[0] = pos_history.prev;
        shared_ray.endpoints[1] = this->model.pos;
        InitializeCollisionRay(shared_ray);
        collision_query.reject_surface40 = true;
        QueryCollisionRay(shared_ray, 1);
        hit = shared_ray.hit.GetObject();
        collision_query.reject_surface40 = false;
        if (hit)
        {
            if (hit->flags & 0x10)
            {
                damage = projectile.damage;
                kind = projectile.damage_kind;
                static_cast<VisualObject *>(hit->owner)->Damage(damage, zero_vector.components, kind);
            }
            else
            {
                damage = projectile.damage;
                DamageLevelObject(hit, shared_ray.hit.GetPolygon(), (sint32)damage);
            }
            Explode();
        }
    }
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    NormalizeAngles(this->model.angles.words);
    x = motion.angular_velocity.words[0];
    x_shift = motion.angular_acceleration.words[0];
    y = motion.angular_velocity.words[1];
    y_shift = motion.angular_acceleration.words[1];
    x_sum = x + x_shift;
    y_sum = y + y_shift;
    motion.angular_velocity.words[0] = x_sum;
    motion.angular_velocity.words[1] = y_sum;
    x_signed = (sint16)motion.angular_velocity.words[0];
    x_shift = motion.angular_drag[0] & 31;
    y_signed = (sint16)motion.angular_velocity.words[1];
    y_shift = motion.angular_drag[1] & 31;
    motion.angular_velocity.words[0] = x_sum - (uint32)(x_signed >> x_shift);
    y_word = motion.angular_velocity.words[1];
    motion.angular_velocity.words[1] = y_word - (uint32)(y_signed >> y_shift);
    return SnapSmallAngles(motion.angular_velocity.words);
}

void ProjectileBase::ApplyBlast(ModelInstance *list, uint32 radius, uint32 impulse, uint32 kind)
{
    Vector3 delta, product, vector;
    uint32 distance, damage, reduction;
    FUNCTION_MARKER(0x8002C460, "SLUS_003.73");
    while (list)
    {
        SubtractVectors(delta.words, list->pos.words, this->model.pos.words);
        distance = MeasureVectorLength(delta.words);
        if ((sint32)radius >= (sint32)distance)
        {
            damage = projectile.damage;
            reduction = DivideProjectileTrace((damage >> 1) * distance, radius);
            damage -= reduction;
            MultiplyScalarVector(product.words, impulse, delta.words);
            DivideVector(vector.words, product.words, distance);
            static_cast<VisualObject *>(list->owner)->Damage(damage, vector.components, kind);
        }
        list = list->next;
    }
}

void ProjectileBase::DamageNearbyObjects(uint32 radius)
{
    ModelBorrow borrow;
    ModelInstance *list;
    Vector3 position;
    uint32 bound, x, z, delta, damage;
    FUNCTION_MARKER(0x8002C56C, "SLUS_003.73");
    list = GetModelSceneObjects();
    bound = radius << 12;
    while (list)
    {
        x = list->pos.words[0];
        delta = x - this->model.pos.words[0];
        if ((sint32)delta < 0)
            delta = 0 - delta;
        if ((sint32)delta < (sint32)bound)
        {
            z = list->pos.words[2];
            delta = z - this->model.pos.words[2];
            if ((sint32)delta < 0)
                delta = 0 - delta;
            if ((sint32)delta < (sint32)bound)
            {
                position.words[1] = list->pos.words[1];
                position.words[0] = x;
                position.words[2] = z;
                if ((sint32)MeasurePositionDistance(this->model.pos.words, position.words) < (sint32)radius)
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
    FUNCTION_MARKER(0x8002C654, "SLUS_003.73");
    projectile.trail_color.r = (uint8)(red);
    projectile.trail_color.g = (uint8)(green);
    projectile.trail_kind = (uint8)(kind);
    projectile.trail_color.b = (uint8)(blue);
    return blue;
}

ExplosionRing::ExplosionRing() : MeshRingEffect(12, 0xCCE0187C), age(0), lifetime(0), radius_step(0), width_step(0)
{
    // Original constructor is inlined in ProjectileBase::Explode
    this->velocity.words[0] = 0;
    this->velocity.words[1] = (uint32)-32768;
    this->velocity.words[2] = 0;
}

uint32 ExplosionRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002E0C4, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return result;
}

void ExplosionRing::Update()
{
    FUNCTION_MARKER(0x8002E008, "SLUS_003.73");
    ++age;
    if (lifetime < (sint16)age)
    {
        Finish();
        return;
    }
    AddVectors(pos.words, velocity.words);
    SetGeometry(pos.words, radius + (uint32)(sint32)radius_step, expansion + (uint32)(sint32)width_step);
    uint32 value = lifetime - (uint32)(sint32)(sint16)age;
    value = (value << 8) - value;
    uint32 color = lifetime ? (uint32)((sint32)value / (sint32)lifetime) : ((sint32)value < 0 ? 1 : (uint32)-1);
    SetColors(color, color, (sint32)color >> 1);
}
