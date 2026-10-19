#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tuning.h"
#include "game_tick.h"

#include "record_scripts.h"

#include "effect_object.h"
#include "visual_object.h"
#include "collision.h"
#include "game_diagnostics.h"
#include "projectile_launch.h"
#include "glow_effect.h"
#include "ring_burst_effect.h"
#include "explosion_effects.h"
#include "game_object_allocation.h"
#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "audio_session.h"

static uint32 DivideLaunchScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 CalculateLaunchAngle(uint32 input);

void LaunchedProjectile::Configure(ModelInstance **owner, uint32 radius, uint32 model, uint32 explode_on_contact)
{
    uint32 tag;
    uint32 tick;
    GameHeapBlock *child;
    uint32 random;
    FUNCTION_MARKER(0x8002A7F8, "SLUS_003.73");
    AddToList(visual_lists.projectiles);
    if (model)
        tag = GetRecordModelBankSlot();
    else
    {
        tag = items_bank;
        model = 0x51E46AAF;
    }
    this->model.model_idx = FindModelResourceIndex(model, tag);
    tick = game_clock.active_frame;
    projectile.damage_kind = (uint32)(26);
    shadow.max_height = 300;
    projectile.owner_list = owner;
    projectile.radius = (uint16)(radius);
    launch.explode_on_contact = (uint8)explode_on_contact;
    launch.spawn_tick = (uint32)(tick);
    this->kind = owner == &visual_lists.players ? 63 : 64;
    if (projectile.owner_list == &visual_lists.players)
        this->lifecycle_flags = this->lifecycle_flags & 0xFFEF;
    child = AllocateEffectStorage(104);
    if (!child)
    {
        GameDiagnostic("Launched projectile ring allocation failed\n");
        abort();
    }
    launch.glow_object = new (child) RingEffect(4, 2);
    launch.glow_object->hide_center = 1;
    launch.glow_object->SetRowExtent(0, 16);
    launch.glow_object->SetRowExtent(1, 16);
    launch.glow_object->SetCenterColor(0, 0, 0);
    launch.glow_object->SetRingColors(0, 0, 0);
    launch.glow_color.r = 100;
    launch.glow_color.g = 50;
    launch.glow_color.b = 0;
    launch.glow_object->SetRowColors(0, 100, 50, 0);
    launch.glow_object->SetRowColors(1, 0, 0, 0);
    random = RandomRange(1024);
    launch.glow_object->angle = random;
    launch.glow_object->SetRadii(0);
    launch.glow_object->SetPosition(this->model.pos.words);
    launch.glow_object->persistent = 1;
}

uint32 LaunchedProjectile::SetGlowColor(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8002A9F0, "SLUS_003.73");
    launch.glow_color.b = blue;
    launch.glow_color.r = red;
    launch.glow_color.g = green;
    return launch.glow_object ? launch.glow_object->SetRowColors(0, red & 255, green & 255, blue & 255) : 0;
}

LaunchedProjectile::LaunchedProjectile(const uint32 *position, const sint16 *angles, uint32 speed, uint32 damage, uint32 radius, ModelInstance **list, uint32 model, uint32 explode_on_contact) : ProjectileBase()
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002AA38, "SLUS_003.73");
    projectile.damage = (uint16)(damage);
    Configure(list, radius, model, explode_on_contact & 255);
    x = position[0];
    y = position[1];
    z = position[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    BuildDirectionVector(motion.velocity.words, speed, (const uint16 *)angles);
}

LaunchedProjectile::LaunchedProjectile(const uint32 *position, const uint32 *target, uint32 speed, uint32 damage, uint32 radius, ModelInstance **owner, uint32 model, uint32 explode_on_contact) : ProjectileBase()
{
    FUNCTION_MARKER(0x8002AB28, "SLUS_003.73");
    uint32 x, y, z, origin, aim, dx, dz, distance, qx, qz, gravity, dy, vertical, scale;
    projectile.damage = (uint16)(damage);
    Configure(owner, radius, model, explode_on_contact & 255);
    x = position[0];
    y = position[1];
    z = position[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    origin = position[0];
    aim = target[0];
    dx = (uint32)((sint32)(aim - origin) >> 12);
    aim = target[2];
    origin = position[2];
    dz = (uint32)((sint32)(aim - origin) >> 12);
    distance = LookupDistanceRoot(dx * dx + dz * dz);
    dx <<= 12;
    if (!distance)
    {
        motion.velocity.words[0] = 0;
        motion.velocity.words[1] = 0xFFFEC000;
        motion.velocity.words[2] = 0;
    }
    else
    {
        qx = DivideLaunchScalar(dx, distance);
        qz = DivideLaunchScalar(dz << 12, distance);
        gravity = DivideLaunchScalar(distance << 3, speed << 1);
        origin = position[1];
        aim = target[1];
        dy = (uint32)((sint32)(aim - origin) >> 12);
        vertical = DivideLaunchScalar(speed * dy, distance);
        scale = (uint32)((sint32)(speed * 9) / 10);
        motion.velocity.words[0] = qx * scale;
        motion.velocity.words[1] = (vertical - gravity) << 12;
        motion.velocity.words[2] = qz * scale;
    }
}

LaunchedProjectile::~LaunchedProjectile()
{
    if (launch.glow_object)
        launch.glow_object->Destroy(3);
}

uint32 LaunchedProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002AD10, "SLUS_003.73");
    if (launch.glow_object)
        launch.glow_object->Destroy(3);
    launch.glow_object = 0;
    RemoveFromList(visual_lists.projectiles);
    ProjectileBase::Destroy(0);
    return reason & 1 ? ReleaseObjectStorage(this) : 0;
}

uint32 LaunchedProjectile::Update()
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    Vector3 before;
    uint32 x, y, z, speed, dot, normal, product, value, one = 1;
    uint32 explode = 0;
    GameHeapBlock *child;
    uint32 elapsed;
    uint32 limit;
    uint32 red;
    uint32 green;
    uint32 blue;
    uint32 height;
    ModelInstance *hit;
    ModelInstance *head;
    ModelInstance **table;
    uint32 now, start_time;
    FUNCTION_MARKER(0x8002AD9C, "SLUS_003.73");
    this->model.pos.words[1] = this->model.pos.words[1] + 81920;
    before = this->model.pos;
    if (!launch.settled)
    {
        x = this->model.pos.words[0];
        shared_ray.endpoints[0].words[0] = (x);
        y = this->model.pos.words[1];
        shared_ray.endpoints[0].words[1] = (y);
        z = this->model.pos.words[2];
        shared_ray.endpoints[0].words[2] = (z);
        shared_ray.endpoints[1].words[0] = (x + (motion.velocity.words[0] << 1));
        shared_ray.endpoints[1].words[1] = (y + (motion.velocity.words[1] << 1));
        shared_ray.endpoints[1].words[2] = (z + (motion.velocity.words[2] << 1));
        speed = MeasureVectorLength(motion.velocity.words);
        InitializeCollisionRay(shared_ray);
        collision_query.reject_surface40 = true;
        QueryCollisionRay(shared_ray, 1);
        collision_query.reject_surface40 = false;
        AddVectors(this->model.pos.words, motion.velocity.words);
        motion.velocity.words[1] = motion.velocity.words[1] + 32768;
        if (shared_ray.hit.GetObject() && (sint32)speed >= (sint32)shared_ray.distance)
        {
            this->model.pos = shared_ray.hit_pos;
            PlayPositionedSound(22, this->model.pos.words, 0);
            x = motion.velocity.words[0];
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vx;
            dot = (uint32)((sint32)x >> 6) * normal;
            y = motion.velocity.words[1];
            dot += (uint32)((sint32)y >> 6) * (uint32)(sint32)(sint16)(uint16)shared_ray.n.vy;
            z = motion.velocity.words[2];
            dot += (uint32)((sint32)z >> 6) * (uint32)(sint32)(sint16)(uint16)shared_ray.n.vz;
            dot = (uint32)((sint32)dot >> 12);
            product = dot * normal;
            motion.velocity.words[0] = x - (uint32)((sint32)product >> 6);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vy;
            y = motion.velocity.words[1];
            product = dot * normal;
            motion.velocity.words[1] = y - (uint32)((sint32)product >> 6);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vz;
            explode = launch.explode_on_contact != 0;
            z = motion.velocity.words[2];
            product = dot * normal;
            motion.velocity.words[2] = z - (uint32)((sint32)product >> 6);
            ShiftVectorRight(motion.velocity.words, one);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vx;
            x = motion.velocity.words[0];
            product = dot * normal;
            motion.velocity.words[0] = x - (uint32)((sint32)product >> 7);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vy;
            y = motion.velocity.words[1];
            product = dot * normal;
            motion.velocity.words[1] = y - (uint32)((sint32)product >> 7);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vz;
            z = motion.velocity.words[2];
            product = dot * normal;
            motion.velocity.words[2] = z - (uint32)((sint32)product >> 7);
            value = (RandomRange(11) - 5) << 12;
            motion.velocity.words[0] = motion.velocity.words[0] + value;
            value = (RandomRange(11) - 5) << 12;
            z = motion.velocity.words[2];
            y = motion.velocity.words[1];
            motion.velocity.words[2] = z + value;
            if ((sint32)y > -61440 && (sint16)(uint16)shared_ray.n.vy < -3500)
                launch.settled = (uint8)(1);
        }
        product = 410 * MeasureVectorLength(motion.velocity.words);
        value = this->model.angles.words[0];
        this->model.angles.words[0] = value + (uint32)((sint32)product / 128);
        x = motion.velocity.words[0];
        z = (uint32)((sint32)motion.velocity.words[2] >> 12);
        x = (uint32)((sint32)x >> 12);
        if (!z)
            this->model.angles.words[1] = (sint32)x > 0 ? -1024 : 1024;
        else if ((sint32)z > 0)
            this->model.angles.words[1] = 2048 - CalculateLaunchAngle(DivideLaunchScalar(x << 12, 0 - z));
        else
            this->model.angles.words[1] = CalculateLaunchAngle(DivideLaunchScalar(x << 12, z));
    }
    shared_ray.endpoints[0].words[0] = (this->model.pos.words[0]);
    shared_ray.endpoints[0].words[1] = (this->model.pos.words[1]);
    shared_ray.endpoints[0].words[2] = (this->model.pos.words[2]);
    shared_ray.endpoints[1].words[0] = (this->model.pos.words[0]);
    y = this->model.pos.words[1];
    shared_ray.endpoints[1].words[1] = (y);
    z = this->model.pos.words[2];
    shared_ray.endpoints[1].words[1] = (y + 12288000);
    shared_ray.endpoints[1].words[2] = (z);
    InitializeCollisionRay(shared_ray);
    QueryCollisionRay(shared_ray, 1);
    if (shared_ray.hit.GetObject())
    {
        surface.pos.words[0] = shared_ray.hit_pos.words[0];
        y = shared_ray.hit_pos.words[1];
        surface.pos.words[1] = y;
        value = this->model.pos.words[1];
        z = shared_ray.hit_pos.words[2];
        height = (uint32)((sint32)(y - value) >> 12);
        product = (uint32)(21 * (sint32)(sint16)height);
        surface.pos.words[2] = z;
        shadow.height = height;
        value = 50 - (uint32)((sint32)product / 128);
        shadow.radius = value;
        if (value & 65535)
            EnableShadow();
        else
            RemoveShadow();
    }
    else
        RemoveShadow();
    now = game_clock.active_frame;
    start_time = launch.spawn_tick;
    elapsed = now - start_time;
    limit = 60 * (uint32)GetGameTuning().launched_projectile_seconds;
    if ((sint32)(limit - 30) < (sint32)elapsed)
    {
        if (game_clock.tick & 2)
        {
            value = launch.glow_color.r;
            red = launch.glow_color.r * 2;
            if ((sint8)value < 0)
                red = 255;
            value = launch.glow_color.g;
            green = launch.glow_color.g * 2;
            if ((sint8)value < 0)
                green = 255;
            value = launch.glow_color.b;
            blue = launch.glow_color.b * 2;
            if ((sint8)value < 0)
                blue = 255;
            red &= 255;
            green &= 255;
            blue &= 255;
        }
        else
        {
            blue = launch.glow_color.b;
            red = launch.glow_color.r;
            green = launch.glow_color.g;
        }
        if (launch.glow_object)
            launch.glow_object->SetRowColors(0, red, green, blue);
    }
    limit = 60 * (uint32)GetGameTuning().launched_projectile_seconds;
    if ((sint32)limit < (sint32)elapsed)
        explode = 1;
    head = *projectile.owner_list;
    hit = TraceSegmentCollision(before.words, this->model.pos.words, head);
    if (hit && (!(hit->flags & 16) || static_cast<VisualObject *>(hit->owner)->kind == 205))
        explode = 1;
    if (explode)
    {
        table = projectile.owner_list;
        value = projectile.radius;
        head = *table;
        ApplyBlast(head, value, 10, 26);
        head = visual_lists.solids;
        value = projectile.radius;
        ApplyBlast(head, value, 10, 26);
        DamageNearbyObjects(projectile.radius);
        child = AllocateEffectStorage(124);
        if (child)
        {
            value = projectile.radius;
            new (child) ExpandingGlow(this->model.pos.words, 128, 100, 0, value, 7);
        }
        value = RandomRange(2);
        PlayPositionedSound(value ? 1 : 2, this->model.pos.words, 0);
        SpawnExplosionParticles(this->model.pos.words, 20, 10, 750, 80);
        child = AllocateEffectStorage(124);
        if (child)
        {
            new (child) RingBurstEffect(this->model.pos.words, 5, 255, 255, 255, 1, 128, 100, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
        }
        Finish();
    }
    y = this->model.pos.words[1];
    this->model.pos.words[1] = y - 81920;
    return launch.glow_object ? launch.glow_object->SetPosition(this->model.pos.words) : 0;
}

static uint32 CalculateLaunchAngle(uint32 input)
{
    FUNCTION_MARKER(0x8008702C, "SLUS_003.73");
    return (uint32)CalculateAngleFromSlope((sint32)input);
}
