#include <stdlib.h>
#include <new>
#include <string.h>

#include "game_heap.h"
#include "object_initializers.h"

#include "game_diagnostics.h"

#include "effect_object.h"

#include "visual_object.h"
#include "object_damage.h"
#include "model_projection.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "audio_session.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "sprite_trail.h"
#include "collision.h"
#include "game_object_allocation.h"

#include "projectile_weaving.h"
#include "ring_effect.h"

WeavingProjectile::WeavingProjectile(const uint32 *position, const sint16 *angles, uint32 reverse, uint32 trail_segments, uint32 speed, ModelInstance **owner_pointer, const uint32 *additional_vector) : ProjectileBase()
{
    FUNCTION_MARKER(0x8002C66C, "SLUS_003.73");
    uint32 value;
    GameHeapBlock *child;
    Vector3 coordinates;
    ShortVector3 rotation;
    value = items_bank;
    value = FindModelResourceIndex(0x350E968C, value);
    this->model.model_idx = value;
    AddToList(visual_lists.projectiles);
    this->kind = 60;
    projectile.damage_kind = (uint32)(1);
    projectile.damage = (uint16)(300);
    weaving.trail_segments = (uint32)(trail_segments);
    CopyAngleVector(model.angles.words, (const uint16 *)angles);
    BuildDirectionVector(motion.velocity.words, speed, this->model.angles.words);
    AddVectors(motion.velocity.words, additional_vector);
    SubtractVectors(coordinates.words, position, motion.velocity.words);
    this->model.pos = coordinates;
    projectile.owner_list = owner_pointer;
    projectile.lifetime = (uint32)(75);
    PrepareTrajectory(speed);
    motion.drag[2] = 31;
    motion.drag[1] = 31;
    motion.drag[0] = 31;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    motion.angular_velocity.words[2] = reverse ? (uint32)-400 : 400;
    use_small_effect_pool = false;
    child = AllocateEffectStorage(104);
    if (child)
    {
        projectile.glow_object = new (child) RingEffect(this->model.pos.words, 32, 32, 128, 128, 128, 16, 16, 64);
    }
    if (!projectile.glow_object)
    {
        GameDiagnostic("Projectile ring allocation failed\n");
        abort();
    }
    projectile.glow_object->persistent = 1;
    projectile.glow_distance = (uint32)((uint32)-70);
    SyncChild();
    CopyAngleVector(rotation.words, model.angles.words);
    use_small_effect_pool = true;
    rotation.words[1] = (rotation.words[1] + 1024) & 4095;
    BuildDirectionVector(weaving.axis[0].words, 8, rotation.words);
    value = this->model.angles.words[1];
    rotation.words[0] = (rotation.words[0] + 1024) & 4095;
    rotation.words[1] = (uint16)value;
    BuildDirectionVector(weaving.axis[1].words, 8, rotation.words);
    value = RandomRange(4096);
    weaving.phase[0] = (uint32)(value);
    value = RandomRange(4096);
    weaving.phase[1] = (uint32)(value);
    PlayPositionedSound(15, position, 0);
}

uint32 WeavingProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002C8F8, "SLUS_003.73");
    RemoveFromList(visual_lists.projectiles);
    uint32 result = ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}

uint32 WeavingProjectile::Update()
{
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    Vector3 before, scaled, offset, difference;
    uint32 age;
    uint32 phase;
    uint32 wave;
    uint32 divisor;
    GameHeapBlock *effect;
    uint32 damage;
    uint32 kind;
    uint32 limit;
    uint32 distance;
    ModelInstance *receiver;
    ModelInstance *hit;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002C95C, "SLUS_003.73");
    before = this->model.pos;
    SubtractVectors(this->model.pos.words, weaving.offset[0].words);
    SubtractVectors(this->model.pos.words, weaving.offset[1].words);
    AddVectors(this->model.pos.words, motion.velocity.words);
    age = projectile.age;
    phase = weaving.phase[0];
    wave = (uint32)rsin((500 * age + phase) & 4095);
    MultiplyVector(scaled.words, weaving.axis[0].words, wave);
    divisor = 4096;
    DivideVector(offset.words, scaled.words, divisor);
    weaving.offset[0] = offset;
    age = projectile.age;
    phase = weaving.phase[1];
    wave = (uint32)rsin((300 * age + phase) & 4095);
    MultiplyVector(scaled.words, weaving.axis[1].words, wave);
    divisor = 4096;
    DivideVector(offset.words, scaled.words, divisor);
    weaving.offset[1] = offset;
    AddVectors(this->model.pos.words, weaving.offset[0].words);
    AddVectors(this->model.pos.words, weaving.offset[1].words);
    SyncChild();
    if (!projectile.age)
    {
        use_small_effect_pool = false;
        projectile.trail_object = 0;
        effect = AllocateEffectStorage(100);
        if (effect)
        {
            uint32 segments = weaving.trail_segments;
            projectile.trail_object = new (effect) RibbonTrail(projectile.glow_pos.words, segments, 128, 128, 128);
        }
        if (!projectile.trail_object)
        {
            GameDiagnostic("Projectile trail allocation failed\n");
            abort();
        }
        projectile.trail_object->persistent = 1;
        use_small_effect_pool = true;
    }
    if (!projectile.trail_object)
    {
        GameDiagnostic("Weaving projectile update has no trail\n");
        abort();
    }
    projectile.trail_object->SetPosition(projectile.glow_pos.words);
    list = projectile.owner_list;
    hit = TraceSegmentCollision(before.words, this->model.pos.words, *list);
    if (hit)
    {
        damage = projectile.damage;
        kind = projectile.damage_kind;
        receiver = hit;
        static_cast<VisualObject *>(receiver->owner)->Damage(damage, zero_vector.components, kind);
        Explode();
    }
    age = projectile.age;
    limit = projectile.lifetime;
    ++age;
    projectile.age = (uint32)(age);
    if ((sint32)limit < (sint32)age)
        Explode();
    SubtractVectors(difference.words, visual_lists.cameras->pos.words, this->model.pos.words);
    distance = MeasureSquaredVectorLength(difference.words);
    limit = proj_cfg.far_depth;
    if ((sint32)(limit * limit) < (sint32)distance)
        return Finish();
    return 0;
}
