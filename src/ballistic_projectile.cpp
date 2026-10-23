#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "visual_object.h"
#include "model_bank.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"
#include "collision.h"
#include "game_object_allocation.h"
#include "tracking_projectile.h"
#include "ballistic_projectile.h"
#include "object_damage.h"
#include "explosion_effects.h"
#include "projectile_base.h"

BallisticProjectile::BallisticProjectile(const Vector3 *pos, const ShortVector3 *angles, uint16 damage, uint32 speed, ModelInstance **list) : ProjectileBase()
{
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8002B6A0, "SLUS_003.73");
    model.model_idx = ModelBank::FindResourceIdx(0x350E968C, items_bank);
    AddToList(visual_lists.projectiles);
    damage_kind = 32;
    this->damage = damage;
    model.pos = *pos;
    model.angles = *angles;
    motion.velocity.SetDir(speed, model.angles);
    owner_list = list;
    kind = list == &visual_lists.players ? 65 : 66;
    lifetime = 60;
    PrepareTrajectory(speed);
    if (!lifetime)
        ResolveImpact();
    motion.angular_velocity.z = 400;
    if (RandomRange(2))
        motion.angular_velocity.z = -motion.angular_velocity.z;
    PlayPositionedSound(15, &model.pos, 0);
    child = EffectObject::AllocateStorage(124);
    if (child)
    {
        new (child) RingBurstEffect(&model.pos, 5, 255, 255, 255, 2, 0, 0, 200, 20, 40, 0, 0, 28, 0, 20, 40, 0, 2);
    }
    glow_pos.SetDir((uint32)-48, model.angles);
    glow_pos.Add(model.pos);
    child = EffectObject::AllocateStorage(104);
    if (child)
    {
        glow_object = new (child) RingEffect(&glow_pos, 32, 32, 128, 128, 128, 16, 16, 64);
    }
    if (!glow_object)
    {
        GameDiagnostic(("Projectile ring allocation failed\n"));
        abort();
    }
    glow_object->persistent = true;
    SetTrailColor(12, 64, 64, 128);
}

uint32 BallisticProjectile::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002B918, "SLUS_003.73");
    RemoveFromList(visual_lists.projectiles);
    result = ProjectileBase::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

uint32 BallisticProjectile::ResolveImpact()
{
    Vector3 pos;
    ModelInstance *head;
    ModelInstance **list;
    ModelInstance *target;
    FUNCTION_MARKER(0x8002B97C, "SLUS_003.73");
    list = owner_list;
    head = *list;
    ApplyBlast(head, 2000, 10, 32);
    head = visual_lists.solids;
    ApplyBlast(head, 2000, 10, 32);
    target = impact_target.GetObject();
    if (target)
    {
        DamageLevelObject(target, impact_target.GetPolygon(), damage);
    }
    DamageNearbyObjects(1000);
    pos.Subtract(model.pos, motion.velocity);
    HandleExplosionProximity(&pos, 1);
    return Finish();
}

uint32 BallisticProjectile::Update()
{
    GameHeapBlock *trail;
    Vector3 before;
    uint32 age;
    uint32 lifetime;
    ModelInstance *head;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002BA40, "SLUS_003.73");
    age = this->age;
    if (!age)
    {
        use_small_effect_pool = false;
        trail_object = 0;
        trail = EffectObject::AllocateStorage(100);
        if (trail)
        {
            trail_object = new (trail) RibbonTrail(&glow_pos, trail_kind, trail_r, trail_g, trail_b);
        }
        if (!trail_object)
        {
            GameDiagnostic(("Projectile trail allocation failed\n"));
            abort();
        }
        trail_object->persistent = true;
        trail_object->SetWidth(800);
        use_small_effect_pool = true;
    }
    if (trail_object)
        trail_object->SetPos(&glow_pos);
    before = model.pos;
    model.pos.Add(motion.velocity);
    glow_pos.Add(motion.velocity);
    if (glow_object)
        glow_object->SetPos(&glow_pos);
    model.angles.AddAngles(motion.angular_velocity);
    age = this->age;
    list = owner_list;
    this->age = age + 1;
    head = *list;
    if (CollisionRay::TraceSeg(before, model.pos, head))
        ResolveImpact();
    lifetime = this->lifetime;
    if (!lifetime)
        return ResolveImpact();
    --lifetime;
    this->lifetime = lifetime;
    if (!lifetime)
        return ResolveImpact();
    return lifetime;
}
