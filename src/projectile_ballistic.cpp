#include "visual_object.h"
#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "sprite_trail.h"
#include "object_collision_events.h"
#include "game_object_allocation.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_tracking.h"
#include "projectile_ballistic.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "explosion_effects.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

BallisticProjectile::BallisticProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 speed, ModelInstance **list) : ProjectileBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 guest_position = GuestMemoryAddress(position), guest_angles = GuestMemoryAddress(angles);
    const uint32 *native_position = guest_position ? 0 : position;
    const sint16 *native_angles = guest_angles ? 0 : angles;
    uint32 model, x, y, z, packed_angles, value, child;
    sint16 roll;
    FUNCTION_MARKER(0x8002B6A0, "SLUS_003.73");
    value = r_u8(0x800FF645);
    w_u32(object + 68, 0x800A12F0);
    model = FindModelResourceIndex(0x350E968C, value);
    this->model.model_idx = model;
    AddToList(visual_lists.projectiles);
    projectile.damage_kind = (uint32)(32);
    projectile.damage = (uint16)(damage);
    if (native_position)
    {
        x = native_position[0];
        y = native_position[1];
        z = native_position[2];
    }
    else
    {
        x = r_u32(guest_position);
        y = r_u32(guest_position + 4);
        z = r_u32(guest_position + 8);
    }
    this->model.pos[0] = x;
    this->model.pos[1] = y;
    this->model.pos[2] = z;
    if (native_angles)
    {
        memcpy(&packed_angles, native_angles, 4);
        memcpy(&roll, native_angles + 2, 2);
    }
    else
    {
        packed_angles = ReadAnglesWord(guest_angles);
        roll = (sint16)r_u16(guest_angles + 4);
    }
    this->model.SetAnglesXY(packed_angles);
    this->model.angles[2] = roll;
    BuildDirectionVector((uint32 *)motion.velocity, speed, (const uint16 *)this->model.angles);
    projectile.owner_list = list;
    this->kind = list == &visual_lists.players ? 65 : 66;
    projectile.lifetime = (uint32)(60);
    PrepareTrajectory(speed);
    if (!projectile.lifetime)
        ResolveImpact();
    motion.angular_velocity[2] = 400;
    if (RandomRange(2))
    {
        value = motion.angular_velocity[2];
        motion.angular_velocity[2] = 0 - value;
    }
    PlayPositionedSound(15, (const uint32 *)this->model.pos, 0);
    child = AllocateEffectStorage(124);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
        new (dram_addr) RingBurstEffect((const uint32 *)this->model.pos, 5, 255, 255, 255, 2, 0, 0, 200, 20, 40, 0, 0, 28, 0, 20, 40, 0, 2);
    }
    BuildDirectionVector((uint32 *)projectile.glow_pos, (uint32)-48, (const uint16 *)this->model.angles);
    AddVectors((uint32 *)projectile.glow_pos, (const uint32 *)this->model.pos);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect((const uint32 *)projectile.glow_pos, 32, 32, 128, 128, 128, 16, 16, 64);
    }
    projectile.glow_object = (uint32)(child);
    w_u8(child + 66, 1);
    SetTrailColor(12, 64, 64, 128);
}

uint32 BallisticProjectile::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8002B918, "SLUS_003.73");
    w_u32(object + 68, 0x800A12F0);
    RemoveFromList(visual_lists.projectiles);
    result = ProjectileBase::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 BallisticProjectile::ResolveImpact()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 damage, position[3];
    ModelInstance * head;
    ModelInstance ** list;
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
    SubtractVectors(position, (const uint32 *)this->model.pos, (const uint32 *)motion.velocity);
    HandleExplosionProximity(position, 1);
    return Finish();
}

uint32 BallisticProjectile::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 trail, green, kind, red, blue, before[3], child, age, lifetime;
    ModelInstance * head;
    ModelInstance ** list;
    FUNCTION_MARKER(0x8002BA40, "SLUS_003.73");
    age = projectile.age;
    if (!age)
    {
        w_u32(0x800FF3AC, 0);
        trail = AllocateEffectStorage(100);
        if (trail)
        {
            green = projectile.trail_rgb[1];
            kind = projectile.trail_kind;
            red = projectile.trail_rgb[0];
            blue = projectile.trail_rgb[2];
            void *dram_addr = AllocateNativeObjectStorage(trail, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)projectile.glow_pos, kind, red, green, blue);
        }
        projectile.trail_object = (uint32)(trail);
        w_u8(trail + 66, 1);
        trail = projectile.trail_object;
        SpriteTrail::Attach(trail).SetWidth(800);
        w_u32(0x800FF3AC, 1);
    }
    trail = projectile.trail_object;
    SpriteTrail::Attach(trail).SetPosition((const uint32 *)projectile.glow_pos);
    before[0] = this->model.pos[0];
    before[1] = this->model.pos[1];
    before[2] = this->model.pos[2];
    AddVectors((uint32 *)this->model.pos, (const uint32 *)motion.velocity);
    AddVectors((uint32 *)projectile.glow_pos, (const uint32 *)motion.velocity);
    child = projectile.glow_object;
    RingEffect::Attach(child).SetPosition((const uint32 *)projectile.glow_pos);
    AddAngleVector((uint16 *)this->model.angles, (const uint16 *)motion.angular_velocity);
    age = projectile.age;
    list = projectile.owner_list;
    projectile.age = (uint32)(age + 1);
    head = *list;
    if (TraceSegmentCollision(before, (const uint32 *)this->model.pos, head))
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

BallisticProjectile &BallisticProjectile::Attach(uint32 object)
{
    return *static_cast<BallisticProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8002B918(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002B918, "SLUS_003.73");
    return BallisticProjectile::Attach(object).BallisticProjectile::Destroy(reason);
}

extern "C" uint32 sub_8002BA40(uint32 object)
{
    FUNCTION_MARKER(0x8002BA40, "SLUS_003.73");
    return BallisticProjectile::Attach(object).BallisticProjectile::Update();
}




