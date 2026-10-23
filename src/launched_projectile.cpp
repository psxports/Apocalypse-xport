#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "game_tuning.h"
#include "game_tick.h"
#include "resource_script.h"
#include "effect_object.h"
#include "visual_object.h"
#include "collision.h"
#include "game_diagnostics.h"
#include "launched_projectile.h"
#include "glow_effect.h"
#include "expanding_glow.h"
#include "ring_burst_effect.h"
#include "explosion_effects.h"
#include "game_object_allocation.h"
#include "model_bank.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "audio_session.h"
#include "projectile_base.h"

void LaunchedProjectile::Configure(ModelInstance **owner, uint16 radius, uint32 model, uint8 explode_on_contact)
{
    uint32 tag;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8002A7F8, "SLUS_003.73");
    AddToList(visual_lists.projectiles);
    if (model)
        tag = GetRecModelBankSlot();
    else
    {
        tag = items_bank;
        model = 0x51E46AAF;
    }
    this->model.model_idx = ModelBank::FindResourceIdx(model, tag);
    damage_kind = 26;
    shadow.max_height = 300;
    owner_list = owner;
    this->radius = radius;
    this->explode_on_contact = explode_on_contact != 0;
    spawn_tick = game_clock.active_frame;
    kind = owner == &visual_lists.players ? 63 : 64;
    if (owner_list == &visual_lists.players)
        lifecycle_flags = lifecycle_flags & 0xFFEF;
    child = EffectObject::AllocateStorage(104);
    if (!child)
    {
        GameDiagnostic(("Launched projectile ring allocation failed\n"));
        abort();
    }
    glow_ring = new (child) RingEffect(4, 2);
    glow_ring->hide_center = true;
    glow_ring->SetRowExtent(0, 16);
    glow_ring->SetRowExtent(1, 16);
    glow_ring->SetCenterColor(0, 0, 0);
    glow_ring->SetRingColors(0, 0, 0);
    glow_r = 100;
    glow_g = 50;
    glow_b = 0;
    glow_ring->SetRowColors(0, 100, 50, 0);
    glow_ring->SetRowColors(1, 0, 0, 0);
    glow_ring->angle = RandomRange(1024);
    glow_ring->SetRadii(0);
    glow_ring->SetPos(&this->model.pos);
    glow_ring->persistent = true;
}

uint32 LaunchedProjectile::SetGlowColor(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x8002A9F0, "SLUS_003.73");
    glow_b = b;
    glow_r = r;
    glow_g = g;
    return glow_ring ? glow_ring->SetRowColors(0, r, g, b) : 0;
}

LaunchedProjectile::LaunchedProjectile(const Vector3 &pos, const ShortVector3 &angles, uint32 speed, uint16 damage, uint16 radius, ModelInstance **list, uint32 model, uint8 explode_on_contact) : ProjectileBase(), spawn_tick(0), settled(false), glow_ring(), glow_r(0), glow_g(0), glow_b(0), explode_on_contact(false)
{
    FUNCTION_MARKER(0x8002AA38, "SLUS_003.73");
    this->damage = damage;
    Configure(list, radius, model, explode_on_contact);
    this->model.pos = pos;
    motion.velocity.SetDir(speed, angles);
}

LaunchedProjectile::LaunchedProjectile(const Vector3 &pos, const Vector3 &target, uint32 speed, uint16 damage, uint16 radius, ModelInstance **owner, uint32 model, uint8 explode_on_contact) : ProjectileBase(), spawn_tick(0), settled(false), glow_ring(), glow_r(0), glow_g(0), glow_b(0), explode_on_contact(false)
{
    FUNCTION_MARKER(0x8002AB28, "SLUS_003.73");
    uint32 dx, dz, distance, qx, qz, gravity, dy, vertical, scale;
    this->damage = damage;
    Configure(owner, radius, model, explode_on_contact);
    this->model.pos = pos;
    dx = (sint32)((uint32)target.x - pos.x) >> 12;
    dz = (sint32)((uint32)target.z - pos.z) >> 12;
    distance = LookupDistanceRoot(dx * dx + dz * dz);
    dx <<= 12;
    if (!distance)
    {
        motion.velocity.x = 0;
        motion.velocity.y = -81920;
        motion.velocity.z = 0;
    }
    else
    {
        qx = DivideSignedWord(dx, distance);
        qz = DivideSignedWord(dz << 12, distance);
        gravity = DivideSignedWord(distance << 3, speed << 1);
        dy = (sint32)((uint32)target.y - pos.y) >> 12;
        vertical = DivideSignedWord(speed * dy, distance);
        scale = (sint32)(speed * 9) / 10;
        motion.velocity.x = qx * scale;
        motion.velocity.y = (vertical - gravity) << 12;
        motion.velocity.z = qz * scale;
    }
}

LaunchedProjectile::~LaunchedProjectile()
{
    if (glow_ring)
        glow_ring->Destroy(3);
}

uint32 LaunchedProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002AD10, "SLUS_003.73");
    if (glow_ring)
        glow_ring->Destroy(3);
    glow_ring = 0;
    RemoveFromList(visual_lists.projectiles);
    ProjectileBase::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 LaunchedProjectile::Update()
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    Vector3 before;
    uint32 dot, product, val;
    sint32 speed, vx, vz, jitter;
    bool explode = false;
    GameHeapBlock *child;
    sint32 elapsed;
    sint32 limit;
    uint8 r;
    uint8 g;
    uint8 b;
    sint16 height;
    ModelInstance *hit;
    ModelInstance *head;
    ModelInstance **table;
    FUNCTION_MARKER(0x8002AD9C, "SLUS_003.73");
    model.pos.y = (uint32)model.pos.y + (81920);
    before = model.pos;
    if (!settled)
    {
        shared_ray.endpoints[0] = model.pos;
        shared_ray.endpoints[1].x = (uint32)model.pos.x + ((uint32)motion.velocity.x << 1);
        shared_ray.endpoints[1].y = (uint32)model.pos.y + ((uint32)motion.velocity.y << 1);
        shared_ray.endpoints[1].z = (uint32)model.pos.z + ((uint32)motion.velocity.z << 1);
        speed = motion.velocity.Len();
        shared_ray.Initialize();
        col_query.reject_surface40 = true;
        shared_ray.Query(1);
        col_query.reject_surface40 = false;
        model.pos.Add(motion.velocity);
        motion.velocity.y = (uint32)motion.velocity.y + (32768);
        if (shared_ray.hit.GetObject() && speed >= (sint32)shared_ray.distance)
        {
            model.pos = shared_ray.hit_pos;
            PlayPositionedSound(22, &model.pos, 0);
            dot = (uint32)(motion.velocity.x >> 6) * shared_ray.n.vx;
            dot += (uint32)(motion.velocity.y >> 6) * shared_ray.n.vy;
            dot += (uint32)(motion.velocity.z >> 6) * shared_ray.n.vz;
            dot = (sint32)dot >> 12;
            motion.velocity.x = (uint32)motion.velocity.x - ((sint32)(dot * shared_ray.n.vx) >> 6);
            motion.velocity.y = (uint32)motion.velocity.y - ((sint32)(dot * shared_ray.n.vy) >> 6);
            explode = explode_on_contact;
            motion.velocity.z = (uint32)motion.velocity.z - ((sint32)(dot * shared_ray.n.vz) >> 6);
            motion.velocity.ShiftRight(1);
            motion.velocity.x = (uint32)motion.velocity.x - ((sint32)(dot * shared_ray.n.vx) >> 7);
            motion.velocity.y = (uint32)motion.velocity.y - ((sint32)(dot * shared_ray.n.vy) >> 7);
            motion.velocity.z = (uint32)motion.velocity.z - ((sint32)(dot * shared_ray.n.vz) >> 7);
            jitter = RandomRange(11);
            motion.velocity.x = (uint32)motion.velocity.x + ((jitter - 5) * ONE);
            jitter = RandomRange(11);
            motion.velocity.z = (uint32)motion.velocity.z + ((jitter - 5) * ONE);
            if (motion.velocity.y > -61440 && shared_ray.n.vy < -3500)
                settled = true;
        }
        product = 410 * motion.velocity.Len();
        model.angles.x += (sint32)product / 128;
        vx = motion.velocity.x >> 12;
        vz = motion.velocity.z >> 12;
        if (!vz)
            model.angles.y = vx > 0 ? -1024 : 1024;
        else if (vz > 0)
            model.angles.y = 2048 - CalculateLaunchAngle(DivideSignedWord(vx * ONE, -vz));
        else
            model.angles.y = CalculateLaunchAngle(DivideSignedWord(vx * ONE, vz));
    }
    shared_ray.endpoints[0] = model.pos;
    shared_ray.endpoints[1] = model.pos;
    shared_ray.endpoints[1].y = (uint32)shared_ray.endpoints[1].y + (12288000);
    shared_ray.Initialize();
    shared_ray.Query(1);
    if (shared_ray.hit.GetObject())
    {
        surface.pos = shared_ray.hit_pos;
        height = (sint32)((uint32)shared_ray.hit_pos.y - model.pos.y) >> 12;
        shadow.height = height;
        shadow.radius = 50 - 21 * height / 128;
        if (shadow.radius)
            EnableShadow();
        else
            RemoveShadow();
    }
    else
        RemoveShadow();
    elapsed = game_clock.active_frame - spawn_tick;
    limit = 60 * GetGameTuning().launched_projectile_seconds;
    if (limit - 30 < elapsed)
    {
        if (game_clock.tick & 2)
        {
            r = glow_r < 128 ? glow_r * 2 : 255;
            g = glow_g < 128 ? glow_g * 2 : 255;
            b = glow_b < 128 ? glow_b * 2 : 255;
        }
        else
        {
            b = glow_b;
            r = glow_r;
            g = glow_g;
        }
        if (glow_ring)
            glow_ring->SetRowColors(0, r, g, b);
    }
    limit = 60 * GetGameTuning().launched_projectile_seconds;
    if (limit < elapsed)
        explode = true;
    head = *owner_list;
    hit = CollisionRay::TraceSeg(before, model.pos, head);
    if (hit && (!(hit->flags & 16) || static_cast<VisualObject *>(hit->owner)->kind == 205))
        explode = true;
    if (explode)
    {
        table = owner_list;
        val = radius;
        head = *table;
        ApplyBlast(head, val, 10, 26);
        head = visual_lists.solids;
        val = radius;
        ApplyBlast(head, val, 10, 26);
        DamageNearbyObjects(radius);
        child = EffectObject::AllocateStorage(124);
        if (child)
        {
            val = radius;
            new (child) ExpandingGlow(&model.pos, 128, 100, 0, val, 7);
        }
        val = RandomRange(2);
        PlayPositionedSound(val ? 1 : 2, &model.pos, 0);
        SpawnExplosionParticles(&model.pos, 20, 10, 750, 80);
        child = EffectObject::AllocateStorage(124);
        if (child)
        {
            new (child) RingBurstEffect(&model.pos, 5, 255, 255, 255, 1, 128, 100, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
        }
        Finish();
    }
    model.pos.y = (uint32)model.pos.y - (81920);
    return glow_ring ? glow_ring->SetPos(&model.pos) : 0;
}
