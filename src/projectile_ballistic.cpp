#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"

#include "effect_object.h"
#include "visual_object.h"
#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "sprite_trail.h"
#include "collision.h"
#include "game_object_allocation.h"

#include "projectile_tracking.h"
#include "projectile_ballistic.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "explosion_effects.h"

BallisticProjectile::BallisticProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 speed, ModelInstance **list) : ProjectileBase()
{
    uint32 model;
    uint32 x;
    uint32 y;
    uint32 z;
    uint32 value;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8002B6A0, "SLUS_003.73");
    value = items_bank;
    model = FindModelResourceIndex(0x350E968C, value);
    this->model.model_idx = model;
    AddToList(visual_lists.projectiles);
    projectile.damage_kind = (uint32)(32);
    projectile.damage = (uint16)(damage);
    x = position[0];
    y = position[1];
    z = position[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    CopyAngleVector(this->model.angles.words, (const uint16 *)angles);
    BuildDirectionVector(motion.velocity.words, speed, this->model.angles.words);
    projectile.owner_list = list;
    this->kind = list == &visual_lists.players ? 65 : 66;
    projectile.lifetime = (uint32)(60);
    PrepareTrajectory(speed);
    if (!projectile.lifetime)
        ResolveImpact();
    motion.angular_velocity.words[2] = 400;
    if (RandomRange(2))
    {
        value = motion.angular_velocity.words[2];
        motion.angular_velocity.words[2] = 0 - value;
    }
    PlayPositionedSound(15, this->model.pos.words, 0);
    child = AllocateEffectStorage(124);
    if (child)
    {
        new (child) RingBurstEffect(this->model.pos.words, 5, 255, 255, 255, 2, 0, 0, 200, 20, 40, 0, 0, 28, 0, 20, 40, 0, 2);
    }
    BuildDirectionVector(projectile.glow_pos.words, (uint32)-48, this->model.angles.words);
    AddVectors(projectile.glow_pos.words, this->model.pos.words);
    child = AllocateEffectStorage(104);
    if (child)
    {
        projectile.glow_object = new (child) RingEffect(projectile.glow_pos.words, 32, 32, 128, 128, 128, 16, 16, 64);
    }
    if (!projectile.glow_object)
    {
        GameDiagnostic("Projectile ring allocation failed\n");
        abort();
    }
    projectile.glow_object->persistent = 1;
    SetTrailColor(12, 64, 64, 128);
}

uint32 BallisticProjectile::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002B918, "SLUS_003.73");
    RemoveFromList(visual_lists.projectiles);
    result = ProjectileBase::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(this);
    return result;
}

uint32 BallisticProjectile::ResolveImpact()
{
    Vector3 position;
    uint32 damage;
    ModelInstance *head;
    ModelInstance **list;
    ModelInstance *target;
    FUNCTION_MARKER(0x8002B97C, "SLUS_003.73");
    list = projectile.owner_list;
    head = *list;
    ApplyBlast(head, 2000, 10, 32);
    head = visual_lists.solids;
    ApplyBlast(head, 2000, 10, 32);
    target = projectile.impact.hit.GetObject();
    if (target)
    {
        damage = projectile.damage;
        DamageLevelObject(target, projectile.impact.hit.GetPolygon(), (sint32)damage);
    }
    DamageNearbyObjects(1000);
    SubtractVectors(position.words, this->model.pos.words, motion.velocity.words);
    HandleExplosionProximity(position.words, 1);
    return Finish();
}

uint32 BallisticProjectile::Update()
{
    GameHeapBlock *trail;
    uint32 green;
    uint32 kind;
    uint32 red;
    uint32 blue;
    Vector3 before;
    uint32 age;
    uint32 lifetime;
    ModelInstance *head;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002BA40, "SLUS_003.73");
    age = projectile.age;
    if (!age)
    {
        use_small_effect_pool = false;
        projectile.trail_object = 0;
        trail = AllocateEffectStorage(100);
        if (trail)
        {
            green = projectile.trail_color.g;
            kind = projectile.trail_kind;
            red = projectile.trail_color.r;
            blue = projectile.trail_color.b;
            projectile.trail_object = new (trail) RibbonTrail(projectile.glow_pos.words, kind, red, green, blue);
        }
        if (!projectile.trail_object)
        {
            GameDiagnostic("Projectile trail allocation failed\n");
            abort();
        }
        projectile.trail_object->persistent = 1;
        projectile.trail_object->SetWidth(800);
        use_small_effect_pool = true;
    }
    if (projectile.trail_object)
        projectile.trail_object->SetPosition(projectile.glow_pos.words);
    before = this->model.pos;
    AddVectors(this->model.pos.words, motion.velocity.words);
    AddVectors(projectile.glow_pos.words, motion.velocity.words);
    if (projectile.glow_object)
        projectile.glow_object->SetPosition(projectile.glow_pos.words);
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    age = projectile.age;
    list = projectile.owner_list;
    projectile.age = (uint32)(age + 1);
    head = *list;
    if (TraceSegmentCollision(before.words, this->model.pos.words, head))
        ResolveImpact();
    lifetime = projectile.lifetime;
    if (!lifetime)
        return ResolveImpact();
    --lifetime;
    projectile.lifetime = (uint32)(lifetime);
    if (!lifetime)
        return ResolveImpact();
    return lifetime;
}
