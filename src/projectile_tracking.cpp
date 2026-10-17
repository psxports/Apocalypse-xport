#include "visual_object.h"
#include "object_damage.h"
#include "audio_voice.h"
#include "game_object_allocation.h"
#include "sprite_trail.h"
#include "vector_arithmetic.h"
#include "object_collision_events.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "ring_effect.h"
#include "audio_session.h"
#include "fire_cycle.h"
#include "vector_distance.h"
#include "explosion_effects.h"
#include "game_random.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>
#include "projectile_tracking.h"
#include "game_heap.h"
#include "native_object_storage.h"
#include <new>




uint32 TrackingProjectile::Configure(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list)
{
    uint32 guest_position = GuestMemoryAddress(position), guest_angles = GuestMemoryAddress(angles);
    const uint32 *native_position = guest_position ? 0 : position;
    const sint16 *native_angles = guest_angles ? 0 : angles;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 value, x, y, z, packed_angles, pitch, numerator, denominator, quotient, child;
    sint16 roll;
    FUNCTION_MARKER(0x8002CC78, "SLUS_003.73");
    value = r_u8(0x800FF645);
    value = FindModelResourceIndex(0xD40B155E, value);
    this->model.model_idx = value;
    AddToList(visual_lists.projectiles);
    projectile.damage_kind = (uint32)(2);
    w_u16(object + 148, 1);
    tracking.speed = (uint8)(120);
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
    pitch = this->model.angles[0];
    tracking.range = (uint32)(range);
    projectile.damage = (uint16)(damage);
    projectile.owner_list = list;
    this->model.angles[0] = pitch - 10;
    this->kind = list == &visual_lists.players ? 61 : 62;
    tracking.turn_limit = (uint32)(60);
    BuildDirectionVector((uint32 *)motion.velocity, 120, (const uint16 *)this->model.angles);
    denominator = tracking.speed;
    numerator = tracking.range;
    if (!denominator)
        quotient = (sint32)numerator < 0 ? 1 : (uint32)-1;
    else
        quotient = (uint32)((sint32)numerator / (sint32)denominator);
    motion.drag[0] = 3;
    tracking.launch_delay = (uint32)(quotient);
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    motion.angular_velocity[2] = 400;
    if (RandomRange(2))
    {
        value = motion.angular_velocity[2];
        motion.angular_velocity[2] = 0 - value;
    }
    w_u32(0x800FF3AC, 0);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect((const uint32 *)this->model.pos, 32, 32, 128, 128, 128, 64, 32, 0);
    }
    projectile.glow_object = (uint32)(child);
    w_u32(0x800FF3AC, 1);
    w_u8(child + 66, 1);
    projectile.glow_distance = (uint32)((uint32)-30);
    SyncChild();
    value = PlayPositionedSound(15, (const uint32 *)this->model.pos, 0);
    tracking.sound = (uint32)(value);
    return SetTrailColor(9, 80, 0, 0);
}

TrackingProjectile::TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list) : ProjectileBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    ModelInstance *target;
    FUNCTION_MARKER(0x8002CE94, "SLUS_003.73");
    w_u32(object + 68, 0x800A1660);
    Configure(position, angles, range, damage, list);
    if (list == &visual_lists.players)
    {
        target = visual_lists.players;
        tracking.target.SetObject(target);
    }
    else
        SelectTarget();
}

TrackingProjectile::TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list, const uint32 *target_position) : ProjectileBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002CF60, "SLUS_003.73");
    w_u32(object + 68, 0x800A1660);
    Configure(position, angles, range, damage, list);
    tracking.fixed_target = (uint8)(1);
    uint32 target_address = GuestMemoryAddress(target_position);
    x = target_address ? r_u32(target_address) : target_position[0];
    y = target_address ? r_u32(target_address + 4) : target_position[1];
    z = target_address ? r_u32(target_address + 8) : target_position[2];
    tracking.target_pos[0] = (uint32)(x);
    tracking.target_pos[1] = (uint32)(y);
    tracking.target_pos[2] = (uint32)(z);
}

uint32 TrackingProjectile::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 age, sound, result;
    ModelInstance *target;
    FUNCTION_MARKER(0x8002D018, "SLUS_003.73");
    age = projectile.age;
    w_u32(object + 68, 0x800A1660);
    if ((sint32)age < 15)
    {
        sound = tracking.sound;
        StopSoundVoices(sound);
    }
    RemoveFromList(visual_lists.projectiles);
    target = tracking.target.GetObject();
    if (target)
        static_cast<VisualObject *>(target->owner)->target_mark = 0;
    ProjectileBase::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 TrackingProjectile::ConsiderTarget(ModelInstance *target, sint32 *closest, uint32 pass)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    uint32 object = NativeObjectGuestAddress(this);
    uint32 value;
    ModelInstance *blocked;
    sint32 distance;
    FUNCTION_MARKER(0x8002D0A8, "SLUS_003.73");
    if (static_cast<VisualObject *>(target->owner)->target_mark && !pass)
        return 0;
    if (!(static_cast<VisualObject *>(target->owner)->lifecycle_flags & 0x10))
        return 0;
    if (static_cast<VisualObject *>(target->owner)->kind == 204)
        return 0;
    distance = (sint32)MeasurePositionDistance(target->pos, (const uint32 *)this->model.pos);
    if (distance >= *closest)
        return 0;
    value = this->model.pos[0];
    shared_ray.pos[0][0] = (value);
    value = this->model.pos[1];
    shared_ray.pos[0][1] = (value);
    uint32 start_z = this->model.pos[2];
    shared_ray.pos[0][1] = (value - 409600);
    shared_ray.pos[0][2] = start_z;
    value = target->pos[0];
    shared_ray.pos[1][0] = (value);
    value = target->pos[1];
    shared_ray.pos[1][1] = (value);
    value = target->pos[2];
    shared_ray.pos[1][2] = (value);
    InitializeCollisionRay(shared_ray);
    w_u32(0x800FF974, 1);
    QueryCollisionRay(shared_ray, 0);
    blocked = shared_ray.hit.GetObject();
    w_u32(0x800FF974, 0);
    if (blocked)
        return 0;
    *closest = distance;
    return 1;
}

void TrackingProjectile::SelectTarget()
{
    uint32 object = NativeObjectGuestAddress(this);
    ModelInstance *selected = 0;
    uint32 pass = 0;
    ModelInstance **list;
    ModelInstance *candidate;
    sint32 closest = 1000000;
    FUNCTION_MARKER(0x8002D1D4, "SLUS_003.73");
    while (pass < 2)
    {
        list = projectile.owner_list;
        candidate = *list;
        while (candidate)
        {
            if (ConsiderTarget(candidate, &closest, pass))
                selected = candidate;
            candidate = candidate->next;
        }
        candidate = visual_lists.solids;
        while (candidate)
        {
            if (static_cast<VisualObject *>(candidate->owner)->kind == 205 && ConsiderTarget(candidate, &closest, pass))
                selected = candidate;
            candidate = candidate->next;
        }
        ++pass;
        if (selected)
            break;
    }
    tracking.target.SetObject(selected);
}

uint32 TrackingProjectile::Explode()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 random, kind;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002D2F0, "SLUS_003.73");
    list = projectile.owner_list;
    if (list == &visual_lists.players)
    {
        SpawnFireCycle((const uint32 *)this->model.pos, 180, 1, 2);
        random = RandomRange(2);
        PlayPositionedSound(random + 1, (const uint32 *)this->model.pos, 0);
    }
    else
        HandleExplosionProximity((const uint32 *)this->model.pos, 0);
    if (r_u32(0x800FF378) != 9)
    {
        kind = projectile.damage_kind;
        QueueExplosion((const uint32 *)this->model.pos, kind, 512, 32, 256);
    }
    return Finish();
}

uint32 TrackingProjectile::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 trail, green, kind, red, blue, counter, x, y, z;
    ModelInstance *target;
    uint32 position[3], vector[3];
    uint16 desired[3];
    uint32 scale, distance, packed, tail, desired_tail, age;
    ModelInstance *hit;
    ModelInstance *head;
    ModelInstance **list;
    uint32 damage, result;
    ModelInstance *player;
    sint32 limit;
    FUNCTION_MARKER(0x8002D3B4, "SLUS_003.73");
    AdvanceMotion();
    SyncChild();
    if (!projectile.age)
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
        w_u32(0x800FF3AC, 1);
    }
    trail = projectile.trail_object;
    SpriteTrail::Attach(trail).SetPosition((const uint32 *)projectile.glow_pos);
    target = tracking.target.GetObject();
    if (target && (static_cast<VisualObject *>(target->owner)->IsFinished() || !(static_cast<VisualObject *>(target->owner)->lifecycle_flags & 0x10)))
        target = 0;
    counter = tracking.launch_delay;
    if (counter)
    {
        --counter;
        tracking.launch_delay = (uint32)(counter);
        if (!counter)
        {
            this->model.angles[0] = 0;
            motion.velocity[1] = 0;
        }
        goto CheckCollision;
    }
    if (!target && !tracking.fixed_target)
    {
        motion.angular_velocity[0] = 0;
        motion.angular_velocity[1] = 0;
        packed = ReadAnglesWord(0x800FF5E4);
        tail = r_u16(0x800FF5E8);
        motion.SetAngularAccelerationXY(packed);
        motion.angular_acceleration[2] = tail;
        goto CheckCollision;
    }
    if (!tracking.fixed_target && r_u32(0x800FF378) == 9 && (sint32)projectile.age >= 60 && projectile.owner_list == &visual_lists.players)
    {
        x = this->model.pos[0];
        tracking.fixed_target = (uint8)(1);
        y = this->model.pos[1];
        z = this->model.pos[2];
        tracking.target.Reset();
        tracking.target_pos[0] = (uint32)(x);
        tracking.target_pos[1] = (uint32)(y + 0x1000000);
        tracking.target_pos[2] = (uint32)(z);
    }
    if (target)
    {
        position[0] = target->pos[0];
        position[1] = target->pos[1];
        position[2] = target->pos[2];
        static_cast<VisualObject *>(target->owner)->target_mark = 1;
    }
    else
    {
        position[0] = tracking.target_pos[0];
        position[1] = tracking.target_pos[1];
        position[2] = tracking.target_pos[2];
    }
    CalculateDirectionAngles(desired, (const uint32 *)this->model.pos, position);
    scale = tracking.turn_limit;
    if ((sint32)scale >= 200 || MeasurePositionDistance((const uint32 *)this->model.pos, position) < 101)
    {
        this->model.SetAnglesXY(desired[0] | ((uint32)desired[1] << 16));
        this->model.angles[2] = desired[2];
    }
    else
    {
        packed = (this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
        tail = this->model.angles[2];
        desired_tail = desired[2];
        scale = tracking.turn_limit;
        SVECTOR current_angles, target_angles;
        current_angles.vx = (sint16)packed;
        current_angles.vy = (sint16)(packed >> 16);
        current_angles.vz = (sint16)tail;
        current_angles.pad = 0;
        target_angles.vx = (sint16)desired[0];
        target_angles.vy = (sint16)desired[1];
        target_angles.vz = (sint16)desired_tail;
        target_angles.pad = 0;
        UpdateAngleAcceleration(current_angles, (uint16 *)motion.angular_velocity, (uint16 *)motion.angular_acceleration, target_angles, scale);
        scale = tracking.turn_limit;
        tracking.turn_limit = (uint32)(scale + 3);
    }
    scale = tracking.speed;
    BuildDirectionVector((uint32 *)motion.velocity, scale, (const uint16 *)this->model.angles);
CheckCollision:
    list = projectile.owner_list;
    head = *list;
    hit = TraceSegmentCollision(pos_history.prev, (const uint32 *)this->model.pos, head);
    if (hit)
    {
        NormalizeFixedVector(vector, (const uint32 *)motion.velocity);
        damage = projectile.damage;
        kind = projectile.damage_kind;
        static_cast<VisualObject *>(hit->owner)->Damage(damage, (const sint32 *)vector, kind);
        x = hit->pos[0];
        y = hit->pos[1];
        z = hit->pos[2];
        this->model.pos[0] = x;
        this->model.pos[1] = y;
        this->model.pos[2] = z;
        Explode();
    }
    age = projectile.age + 1;
    projectile.age = (uint32)(age);
    limit = (sint16)r_u16(0x800EC578);
    if (limit < (sint32)age)
        goto Expired;
    result = tracking.fixed_target;
    if (!result)
        return result;
    result = tracking.launch_delay;
    if (result)
        return result;
    distance = MeasurePositionDistance((const uint32 *)this->model.pos, (const uint32 *)tracking.target_pos);
    result = distance < 64;
    if (!result)
        return result;
Expired:
    player = visual_lists.players;
    distance = MeasurePositionDistance((const uint32 *)this->model.pos, (const uint32 *)psx_addr(player + 4, 12));
    if (distance < 8000)
        return Explode();
    return Finish();
}

void TrackingProjectile::SetSpeed(uint32 speed)
{
    tracking.speed = (uint8)speed;
}









TrackingProjectile &TrackingProjectile::Attach(uint32 object)
{
    return *static_cast<TrackingProjectile *>(FindNativeObjectStorage(object));
}



extern "C" void apocalypse_collision_metadata_native(uint32 output[2], uint32 node)
{
    MakeObjectReference(output, node);
}

extern "C" uint32 sub_8002D018(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002D018, "SLUS_003.73");
    return TrackingProjectile::Attach(object).TrackingProjectile::Destroy(reason);
}



extern "C" uint32 sub_8002D3B4(uint32 object)
{
    FUNCTION_MARKER(0x8002D3B4, "SLUS_003.73");
    return TrackingProjectile::Attach(object).TrackingProjectile::Update();
}
