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
#include "object_collision_events.h"
#include "game_object_allocation.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_weaving.h"
#include "ring_effect.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

WeavingProjectile::WeavingProjectile(const uint32 *position, const sint16 *angles, uint32 reverse, uint32 trail_segments, uint32 speed, ModelInstance **owner_pointer, const uint32 *additional_vector)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_angles = GuestMemoryAddress(angles);
    FUNCTION_MARKER(0x8002C66C, "SLUS_003.73");
    uint32 packed, tail, value, child, coordinates[3];
    uint16 rotation[3], tail_half;
    value = r_u8(0x800FF645);
    w_u32(object + 68, 0x800A16A8);
    value = FindModelResourceIndex(0x350E968C, value);
    this->model.model_idx = value;
    AddToList(visual_lists.projectiles);
    this->kind = 60;
    projectile.damage_kind = (uint32)(1);
    projectile.damage = (uint16)(300);
    weaving.trail_segments = (uint32)(trail_segments);
    if (guest_angles)
    {
        packed = ReadAnglesWord(guest_angles);
        tail = r_u16(guest_angles + 4);
    }
    else
    {
        memcpy(&packed, angles, 4);
        memcpy(&tail_half, (const uint8 *)angles + 4, 2);
        tail = tail_half;
    }
    // Allocated objects have word alignment for the original SWL/SWR pair
    this->model.SetAnglesXY(packed);
    this->model.SetAnglesXY(packed);
    this->model.angles[2] = tail;
    BuildDirectionVector((uint32 *)motion.velocity, speed, (const uint16 *)this->model.angles);
    AddVectors((uint32 *)motion.velocity, additional_vector);
    SubtractVectors(coordinates, position, (const uint32 *)motion.velocity);
    this->model.pos[0] = coordinates[0];
    this->model.pos[1] = coordinates[1];
    this->model.pos[2] = coordinates[2];
    projectile.owner_list = owner_pointer;
    projectile.lifetime = (uint32)(75);
    PrepareTrajectory(speed);
    motion.drag[2] = 31;
    motion.drag[1] = 31;
    motion.drag[0] = 31;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    motion.angular_velocity[2] = reverse ? (uint32)-400 : 400;
    w_u32(0x800FF3AC, 0);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect((const uint32 *)this->model.pos, 32, 32, 128, 128, 128, 16, 16, 64);
    }
    projectile.glow_object = (uint32)(child);
    w_u8(child + 66, 1);
    projectile.glow_distance = (uint32)((uint32)-70);
    SyncChild();
    packed = (this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
    tail = this->model.angles[2];
    rotation[0] = (uint16)packed;
    rotation[1] = (uint16)(packed >> 16);
    rotation[2] = (uint16)tail;
    w_u32(0x800FF3AC, 1);
    rotation[1] = (rotation[1] + 1024) & 4095;
    BuildDirectionVector((uint32 *)weaving.axis[0], 8, rotation);
    value = this->model.angles[1];
    rotation[0] = (rotation[0] + 1024) & 4095;
    rotation[1] = (uint16)value;
    BuildDirectionVector((uint32 *)weaving.axis[1], 8, rotation);
    value = RandomRange(4096);
    weaving.phase[0] = (uint32)(value);
    value = RandomRange(4096);
    weaving.phase[1] = (uint32)(value);
    PlayPositionedSound(15, position, 0);
}

uint32 WeavingProjectile::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002C8F8, "SLUS_003.73");
    w_u32(object + 68, 0x800A16A8);
    RemoveFromList(visual_lists.projectiles);
    uint32 result = ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

uint32 WeavingProjectile::Update()
{
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 before[3], scaled[3], offset[3], difference[3];
    uint32 age, phase, wave, divisor, effect, damage, kind, limit, distance;
    ModelInstance *receiver;
    ModelInstance *hit;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002C95C, "SLUS_003.73");
    before[0] = this->model.pos[0];
    before[1] = this->model.pos[1];
    before[2] = this->model.pos[2];
    SubtractVectors((uint32 *)this->model.pos, (const uint32 *)weaving.offset[0]);
    SubtractVectors((uint32 *)this->model.pos, (const uint32 *)weaving.offset[1]);
    AddVectors((uint32 *)this->model.pos, (const uint32 *)motion.velocity);
    age = projectile.age;
    phase = weaving.phase[0];
    wave = (uint32)rsin((500 * age + phase) & 4095);
    MultiplyVector(scaled, (const uint32 *)weaving.axis[0], &wave);
    divisor = 4096;
    DivideVector(offset, scaled, &divisor);
    weaving.offset[0][0] = (uint32)(offset[0]);
    weaving.offset[0][1] = (uint32)(offset[1]);
    weaving.offset[0][2] = (uint32)(offset[2]);
    age = projectile.age;
    phase = weaving.phase[1];
    wave = (uint32)rsin((300 * age + phase) & 4095);
    MultiplyVector(scaled, (const uint32 *)weaving.axis[1], &wave);
    divisor = 4096;
    DivideVector(offset, scaled, &divisor);
    weaving.offset[1][0] = (uint32)(offset[0]);
    weaving.offset[1][1] = (uint32)(offset[1]);
    weaving.offset[1][2] = (uint32)(offset[2]);
    AddVectors((uint32 *)this->model.pos, (const uint32 *)weaving.offset[0]);
    AddVectors((uint32 *)this->model.pos, (const uint32 *)weaving.offset[1]);
    SyncChild();
    if (!projectile.age)
    {
        w_u32(0x800FF3AC, 0);
        effect = AllocateEffectStorage(100);
        if (effect)
        {
            uint32 segments = weaving.trail_segments;
            void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)projectile.glow_pos, segments, 128, 128, 128);
        }
        projectile.trail_object = (uint32)(effect);
        w_u8(effect + 66, 1);
        w_u32(0x800FF3AC, 1);
    }
    effect = projectile.trail_object;
    SpriteTrail::Attach(effect).SetPosition((const uint32 *)projectile.glow_pos);
    list = projectile.owner_list;
    hit = TraceSegmentCollision(before, (const uint32 *)this->model.pos, *list);
    if (hit)
    {
        damage = projectile.damage;
        kind = projectile.damage_kind;
        receiver = hit;
        static_cast<VisualObject *>(receiver->owner)->Damage(damage, (const sint32 *)psx_addr(0x800A71CC, 12), kind);
        Explode();
    }
    age = projectile.age;
    limit = projectile.lifetime;
    ++age;
    projectile.age = (uint32)(age);
    if ((sint32)limit < (sint32)age)
        Explode();
    SubtractVectors(difference, (const uint32 *)psx_addr(visual_lists.cameras + 4, 12), (const uint32 *)this->model.pos);
    distance = MeasureSquaredVectorLength(difference);
    limit = proj_cfg.far_depth;
    if ((sint32)(limit * limit) < (sint32)distance)
        return Finish();
    return 0;
}



WeavingProjectile &WeavingProjectile::Attach(uint32 object)
{
    return *static_cast<WeavingProjectile *>(FindNativeObjectStorage(object));
}





extern "C" uint32 sub_8002C8F8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002C8F8, "SLUS_003.73");
    return WeavingProjectile::Attach(object).WeavingProjectile::Destroy(reason);
}

extern "C" uint32 sub_8002C95C(uint32 object)
{
    FUNCTION_MARKER(0x8002C95C, "SLUS_003.73");
    return WeavingProjectile::Attach(object).WeavingProjectile::Update();
}
