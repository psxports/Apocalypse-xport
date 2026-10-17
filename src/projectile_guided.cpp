#include "visual_object.h"
#include "object_damage.h"
#include "game_sdk_bindings.h"
#include "game_object_allocation.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "object_collision_events.h"
#include "sprite_trail.h"
#include "game_random.h"
#include "explosion_effects.h"
#include "angle_math.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

#include "projectile_guided.h"
#include "effect_sprite.h"
#include "waving_beam.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Native input bridge for original vector loads
static uint32 ReadGuidedPositionWord(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

GuidedProjectile::GuidedProjectile(const uint32 *position, uint32 damage, ModelInstance **owner_pointer, uint32 flags)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 tag, x, y, z, child;
    FUNCTION_MARKER(0x8002D7C4, "SLUS_003.73");
    tag = r_u8(0x800FF645);
    w_u32(object + 68, 0x800A1618);
    tag = FindModelResourceIndex(0x717FB6B2, tag);
    this->model.model_idx = tag;
    AddToList(visual_lists.projectiles);
    this->kind = 61;
    projectile.damage_kind = (uint32)(2);
    w_u16(object + 148, 1);
    x = ReadGuidedPositionWord(position, 0);
    y = ReadGuidedPositionWord(position, 1);
    z = ReadGuidedPositionWord(position, 2);
    this->model.pos[0] = x;
    this->model.pos[1] = y;
    this->model.pos[2] = z;
    projectile.damage = (uint16)(damage);
    projectile.owner_list = owner_pointer;
    guided.turn_limit = (uint32)(48);
    SetAngles((const uint16 *)this->model.angles);
    guided.launch_delay = (uint32)(1);
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    w_u32(0x800FF3AC, 0);
    child = AllocateEffectStorage(120);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(AnimatedSprite));
        new (dram_addr) AnimatedSprite((const uint32 *)this->model.pos, (const char *)psx_addr(0x800A15E8, 1), 2048, 1, 0, (uint32)-1);
    }
    guided.sprite_object = (uint32)(child);
    w_u8(child + 66, 1);
    w_u32(0x800FF3AC, 1);
    CaptureOwner();
    guided.held = (uint32)(1);
    guided.flags = (uint16)(flags);
}

uint32 GuidedProjectile::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002D930, "SLUS_003.73");
    w_u32(object + 68, 0x800A1618);
    RemoveFromList(visual_lists.projectiles);
    child = guided.sprite_object;
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 GuidedProjectile::SetAngles(const uint16 *angles)
{
    FUNCTION_MARKER(0x8002D9C0, "SLUS_003.73");
    CopyAngleVector(model.angles, angles);
    return BuildDirectionVector(motion.velocity, 128, model.angles);
}

uint32 GuidedProjectile::SetSize(uint32 size)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, child;
    FUNCTION_MARKER(0x8002DA04, "SLUS_003.73");
    flags = this->model.flags;
    child = guided.sprite_object;
    this->model.scale[2] = size;
    this->model.scale[1] = size;
    this->model.scale[0] = size;
    flags |= 512;
    this->model.flags = flags;
    if (child)
        AnimatedSprite::Attach(child).SetSize((size >> 1) & 65535);
    return flags;
}

ModelInstance *GuidedProjectile::CaptureOwner()
{
    FUNCTION_MARKER(0x8002DA4C, "SLUS_003.73");
    ModelInstance *owner = *projectile.owner_list;
    guided.target.SetObject(owner);
    return owner;
}

void GuidedProjectile::Launch(uint32 mode, const uint32 *target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002DC0C, "SLUS_003.73");
    guided.held = (uint32)(0);
    guided.target_mode = (uint32)(mode);
    if (!mode)
    {
        x = ReadGuidedPositionWord(target, 0);
        y = ReadGuidedPositionWord(target, 1);
        z = ReadGuidedPositionWord(target, 2);
        guided.target_pos[0] = (uint32)(x);
        guided.target_pos[1] = (uint32)(y);
        guided.target_pos[2] = (uint32)(z);
    }
}

uint32 GuidedProjectile::SpawnDebris()
{
    uint32 trig_angle;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index, angle, radius, position[3], child;
    uint16 angles[3];
    FUNCTION_MARKER(0x8002DA60, "SLUS_003.73");
    for (index = 0; index < 6; ++index)
    {
        angle = RandomRange(4096);
        radius = RandomRange(256) + 128;
        trig_angle = angle & 4095;
        position[0] = this->model.pos[0];
        position[1] = this->model.pos[1];
        position[2] = this->model.pos[2];
        position[0] += radius * (uint32)rsin(trig_angle);
        position[2] += radius * (uint32)rcos(trig_angle);
        angles[1] = 0;
        angles[2] = 0;
        angles[0] = (uint16)-1024;
        child = AllocateEffectStorage(364);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(WavingBeam));
            new (dram_addr) WavingBeam(position, angles, 512, 60, 2, 160, 160, 160, 5, 0x68B7B136);
        }
    }
    return 0;
}

uint32 GuidedProjectile::Explode()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 kind;
    FUNCTION_MARKER(0x8002DB9C, "SLUS_003.73");
    HandleExplosionProximity((const uint32 *)this->model.pos, 0);
    kind = projectile.damage_kind;
    QueueExplosion((const uint32 *)this->model.pos, kind, 512, 32, 256);
    return Finish();
}

void GuidedProjectile::SetLaunchDelay(uint32 delay)
{
    guided.launch_delay = delay;
}



uint32 GuidedProjectile::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, child, delay, mode, packed, tail, scale;
    ModelInstance *target;
    ModelInstance *owner;
    uint32 vector[4], damage, kind, x, y, z, count, age;
    ModelInstance *hit;
    ModelInstance **list;
    uint16 desired[4];
    FUNCTION_MARKER(0x8002DC38, "SLUS_003.73");
    owner = guided.target.GetObject();
    if (owner)
    {
        result = static_cast<VisualObject *>(owner->owner)->IsFinished();
        if (result)
            guided.target.Reset();
        else
        {
            owner = guided.target.GetObject();
            if (r_u8(NativeObjectGuestAddress(owner->owner) + 224))
                guided.target.Reset();
        }
    }
    result = guided.held;
    if (result)
    {
        child = guided.sprite_object;
        return child ? static_cast<EffectObject *>(FindNativeObjectStorage(child))->SetPosition((const uint32 *)this->model.pos) : result;
    }
    AdvanceMotion();
    if (this->lifecycle_flags & 64)
    {
        result = guided.flags & 1;
        return result ? SpawnDebris() : result;
    }
    child = guided.sprite_object;
    if (child)
        static_cast<EffectObject *>(FindNativeObjectStorage(child))->SetPosition((const uint32 *)this->model.pos);
    child = projectile.trail_object;
    if (!child)
    {
        w_u32(0x800FF3AC, 0);
        child = AllocateEffectStorage(100);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)this->model.pos, 9, 80, 0, 0);
        }
        projectile.trail_object = (uint32)(child);
        w_u8(child + 66, 1);
        child = projectile.trail_object;
        w_u32(0x800FF3AC, 1);
    }
    if (child)
        SpriteTrail::Attach(projectile.trail_object).SetPosition((const uint32 *)this->model.pos);
    delay = guided.launch_delay;
    if (delay)
    {
        --delay;
        guided.launch_delay = (uint32)(delay);
        if (!delay)
        {
            this->model.angles[0] = 0;
            motion.velocity[1] = 0;
        }
    }
    else
    {
        target = guided.target.GetObject();
        if (target || !guided.target_mode)
        {
            mode = guided.target_mode;
            CalculateDirectionAngles(desired, (const uint32 *)this->model.pos, (mode ? target->pos : guided.target_pos));
            scale = guided.turn_limit;
            if ((sint32)scale < 100)
            {
                packed = (this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
                tail = this->model.angles[2];
                scale = guided.turn_limit;
                SVECTOR current_angles, target_angles;
                current_angles.vx = (sint16)packed;
                current_angles.vy = (sint16)(packed >> 16);
                current_angles.vz = (sint16)tail;
                current_angles.pad = 0;
                target_angles.vx = (sint16)desired[0];
                target_angles.vy = (sint16)desired[1];
                target_angles.vz = (sint16)desired[2];
                target_angles.pad = 0;
                UpdateAngleAcceleration(current_angles, (uint16 *)motion.angular_velocity, (uint16 *)motion.angular_acceleration, target_angles, scale);
                scale = guided.turn_limit;
                guided.turn_limit = (uint32)(scale + 2);
            }
            else
            {
                packed = desired[0] | ((uint32)desired[1] << 16);
                this->model.SetAnglesXY(packed);
                this->model.SetAnglesXY(packed);
                this->model.angles[2] = desired[2];
            }
            BuildDirectionVector((uint32 *)motion.velocity, 128, (const uint16 *)this->model.angles);
        }
        else
        {
            motion.angular_velocity[0] = 0;
            motion.angular_velocity[1] = 0;
            packed = ReadAnglesWord(0x800FF5E4);
            tail = r_u16(0x800FF5E8);
            motion.angular_acceleration[1] = packed >> 16;
            motion.angular_acceleration[0] = packed;
            motion.angular_acceleration[2] = tail;
        }
    }
    list = projectile.owner_list;
    owner = *list;
    hit = TraceSegmentCollision(pos_history.prev, (const uint32 *)this->model.pos, owner);
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
    if (!guided.target_mode && (sint32)MeasurePositionDistance((const uint32 *)this->model.pos, (const uint32 *)guided.target_pos) < 128)
    {
        count = (guided.arrival_count + 1) & 255;
        guided.arrival_count = (uint8)(count);
        if (count >= 3)
            Explode();
    }
    age = projectile.age + 1;
    projectile.age = (uint32)(age);
    result = (sint32)age < 121;
    return result ? result : Explode();
}

GuidedProjectile &GuidedProjectile::Attach(uint32 object)
{
    return *static_cast<GuidedProjectile *>(FindNativeObjectStorage(object));
}







extern "C" uint32 apocalypse_guided_projectile_angles_native(uint32 object, const uint16 *angles)
{
    return GuidedProjectile::Attach(object).GuidedProjectile::SetAngles(angles);
}



extern "C" void sub_8002DC0C(uint32 object, uint32 mode, uint32 target)
{
    FUNCTION_MARKER(0x8002DC0C, "SLUS_003.73");
    GuidedProjectile::Attach(object).GuidedProjectile::Launch(mode, mode ? 0 : (const uint32 *)psx_addr(target, 12));
}

extern "C" uint32 sub_8002D930(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002D930, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::Destroy(reason);
}



extern "C" uint32 sub_8002DB9C(uint32 object)
{
    FUNCTION_MARKER(0x8002DB9C, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::Explode();
}

extern "C" uint32 sub_8002DC38(uint32 object)
{
    FUNCTION_MARKER(0x8002DC38, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::Update();
}


