#include <stdlib.h>
#include <new>

#include "collision_ray.h"
#include "game_heap.h"
#include "object_initializers.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "visual_object.h"
#include "object_damage.h"
#include "model_projection.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "audio_session.h"
#include "angle_math.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"
#include "collision.h"
#include "game_object_allocation.h"
#include "weaving_projectile.h"
#include "ring_effect.h"
#include "projectile_base.h"

WeavingProjectile::WeavingProjectile(const Vector3 &pos, const ShortVector3 &angles, bool reverse, uint32 trail_segs, uint32 speed, ModelInstance **owner_pointer, const Vector3 &extra_velocity) : ProjectileBase(), trail_segments(0)
{
    FUNCTION_MARKER(0x8002C66C, "SLUS_003.73");
    GameHeapBlock *child;
    ShortVector3 rotation;
    for (uint32 i = 0; i < 2; ++i)
    {
        phase[i] = 0;
        for (uint32 j = 0; j < 3; ++j)
        {
            axis[i].comp[j] = 0;
            offset[i].comp[j] = 0;
        }
    }
    model.model_idx = ModelBank::FindResourceIdx(0x350E968C, items_bank);
    AddToList(visual_lists.projectiles);
    kind = 60;
    damage_kind = 1;
    damage = 300;
    trail_segments = trail_segs;
    model.angles = angles;
    motion.velocity.SetDir(speed, model.angles);
    motion.velocity.Add(extra_velocity);
    model.pos.Subtract(pos, motion.velocity);
    owner_list = owner_pointer;
    lifetime = 75;
    PrepareTrajectory(speed);
    motion.drag[2] = 31;
    motion.drag[1] = 31;
    motion.drag[0] = 31;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    motion.angular_velocity.z = reverse ? -400 : 400;
    use_small_effect_pool = false;
    child = EffectObject::AllocateStorage(104);
    if (child)
    {
        glow_object = new (child) RingEffect(&model.pos, 32, 32, 128, 128, 128, 16, 16, 64);
    }
    if (!glow_object)
    {
        GameDiagnostic(("Projectile ring allocation failed\n"));
        abort();
    }
    glow_object->persistent = true;
    glow_distance = -70;
    SyncChild();
    rotation = model.angles;
    use_small_effect_pool = true;
    rotation.y = ((uint16)rotation.y + 1024) & 4095;
    axis[0].SetDir(8, rotation);
    rotation.x = ((uint16)rotation.x + 1024) & 4095;
    rotation.y = model.angles.y;
    axis[1].SetDir(8, rotation);
    phase[0] = RandomRange(4096);
    phase[1] = RandomRange(4096);
    PlayPositionedSound(15, &pos, 0);
}

uint32 WeavingProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002C8F8, "SLUS_003.73");
    RemoveFromList(visual_lists.projectiles);
    uint32 result = ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

uint32 WeavingProjectile::Update()
{
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    Vector3 before, scaled, difference;
    uint32 age;
    uint32 wave;
    GameHeapBlock *effect;
    uint32 limit;
    uint32 distance;
    ModelInstance *hit;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002C95C, "SLUS_003.73");
    before = model.pos;
    model.pos.Subtract(offset[0]);
    model.pos.Subtract(offset[1]);
    model.pos.Add(motion.velocity);
    age = this->age;
    wave = rsin((500 * age + phase[0]) & 4095);
    scaled.Multiply(axis[0], wave);
    offset[0].Divide(scaled, ONE);
    age = this->age;
    wave = rsin((300 * age + phase[1]) & 4095);
    scaled.Multiply(axis[1], wave);
    offset[1].Divide(scaled, ONE);
    model.pos.Add(offset[0]);
    model.pos.Add(offset[1]);
    SyncChild();
    if (!this->age)
    {
        use_small_effect_pool = false;
        trail_object = 0;
        effect = EffectObject::AllocateStorage(100);
        if (effect)
        {
            trail_object = new (effect) RibbonTrail(&glow_pos, trail_segments, 128, 128, 128);
        }
        if (!trail_object)
        {
            GameDiagnostic(("Projectile trail allocation failed\n"));
            abort();
        }
        trail_object->persistent = true;
        use_small_effect_pool = true;
    }
    if (!trail_object)
    {
        GameDiagnostic(("Weaving projectile update has no trail\n"));
        abort();
    }
    trail_object->SetPos(&glow_pos);
    list = owner_list;
    hit = CollisionRay::TraceSeg(before, model.pos, *list);
    if (hit)
    {
        static_cast<VisualObject *>(hit->owner)->Damage(damage, &zero_vector, damage_kind);
        Explode();
    }
    age = this->age;
    limit = lifetime;
    ++age;
    this->age = age;
    if ((sint32)limit < (sint32)age)
        Explode();
    difference.Subtract(visual_lists.cameras->pos, model.pos);
    distance = difference.LenSq();
    limit = proj_cfg.far_depth;
    if ((sint32)(limit * limit) < (sint32)distance)
        return Finish();
    return 0;
}
