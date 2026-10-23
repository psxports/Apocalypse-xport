#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "game_lighting.h"
#include "game_tuning.h"
#include "pickup.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "camera.h"
#include "player.h"
#include "flamethrower.h"
#include "weapon_type.h"
#include "particle_beam.h"
#include "cd_voice_event.h"
#include "enemy_list.h"
#include "record_factory.h"
#include "screen_flash.h"
#include "visual_object.h"
#include "angle_math.h"
#include "global.h"
#include "audio_session.h"
#include "object_damage.h"
#include "collision.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "the_beast.h"
#include "guided_projectile.h"
#include "game_panel.h"
#include "glow_effect.h"
#include "expanding_glow.h"
#include "ring_effect.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "fire_cycle.h"
#include "beast_final_visual.h"
#include "beast_orbit_effect.h"
#include "beast_ground_effect.h"
#include "record_data.h"
#include "enemy.h"

// Original module tables retain shared script and shuffle lifetime
struct BeastSpawnPoints
{
    uint16 records[4];
    uint16 grps[4][5];
    uint16 beam_point;
};

static BeastSpawnPoints spawn_points = {0};
static const uint8 voice_choices[3] = {5, 7, 8};

void TheBeast::ReadLightDir(Character &object, uint32 bone, bool horizontal, ShortVector3 &dir)
{
    Vector3 vector, normalized;
    GetModelAttachment(vector, object, bone);
    vector.Subtract(object.model.pos);
    if (horizontal)
        vector.y = 0;
    normalized.Normalize(vector);
    dir.x = normalized.x;
    dir.y = horizontal ? 0 : normalized.y;
    dir.z = normalized.z;
}

void TheBeast::AdvancePhase()
{
    uint16 phase = ai.phase;
    ai.phase = phase + 1;
}

void TheBeast::SelectState(uint16 state)
{
    ai.state = state;
    ai.phase = 0;
}

void TheBeast::RestoreState()
{
    uint16 state = ai.parameters[2];
    uint16 phase = ai.parameters[3];
    ai.state = state;
    ai.phase = phase;
}

void TheBeast::ClearVelocity()
{
    motion.velocity.z = 0;
    motion.velocity.y = 0;
    motion.velocity.x = 0;
}

void TheBeast::BeginAttack()
{
    uint32 flags = control.flags;
    control.flags = flags & 0xFFFFFFFE;
    ClearVelocity();
    TrackTarget(visual_lists.players);
}

void TheBeast::PlayAnimation(uint8 animation)
{
    SetAnimationRange(animation, 0, -1);
}

void TheBeast::JumpToPoint(Vector3 &pos)
{
    sint16 height;
    ClearFacing();
    height = ground_offset;
    pos.y = (uint32)pos.y - (height * ONE);
    SetJumpDst(&pos);
}

uint16 TheBeast::ReadBeamPoint()
{
    sint16 idx;
    if (ai.parameters[4])
    {
        idx = (sint16)ai.work[2];
        if (idx < 0 || idx >= 5)
        {
            GameDiagnostic(("TheBeast beam point index out of range: %d\n", idx));
            abort();
        }
        return spawn_points.grps[2][idx];
    }
    return spawn_points.beam_point;
}

void TheBeast::PosBeamPoint(Vector3 &pos, ShortVector3 &angles, uint8 turn)
{
    uint32 distance;
    uint16 yaw, offset;
    angles.SetDir(model.pos, pos);
    if (turn)
    {
        yaw = angles.y;
        offset = ai.parameters[5];
        angles.y = turn == 1 ? yaw + offset : yaw - offset;
    }
    distance = model.pos.EuclideanDistance(pos);
    pos.SetDir(distance, angles);
    pos.Add(model.pos);
}

TheBeast::TheBeast(const RecordCursor &rec_data, uint16 idx) : Enemy(), jump_timer(0)
{
    for (uint32 i = 0; i < 5; ++i)
        used_points[i] = 0;
    RecordDataBorrow rec_borrow;
    uint16 roll;
    RecordCursor cursor;
    FUNCTION_MARKER(0x800408A0, "SLUS_003.73");
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    bone_aim_pos.x = bone_aim_pos.y = bone_aim_pos.z = 0;

    cursor = ReadPos(rec_data);
    model.angles.x = cursor.Read16();
    model.angles.y = (cursor + 2).Read16();
    roll = (cursor + 4).Read16();
    script_cursor = cursor + 6;
    control.script_active = 1;
    rec_idx = idx;
    model.angles.z = roll;
    Configure();
    ExecuteScript(script_cursor);
}

void TheBeast::Configure()
{
    FUNCTION_MARKER(0x800409E0, "SLUS_003.73");
    SetModelBank("thebeast");
    motion.drag[0] = 31;
    motion.drag[1] = 31;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    this->model.flags |= 128;
    AllocateBehaviorTable(3, 5);
    SetBehaviorVal(0, 0, 0);
    SetBehaviorVal(1, 0, 0);
    SetBehaviorVal(1, 1, 1);
    SetBehaviorVal(2, 0, 0);
    SetBehaviorVal(2, 1, 2);
    ai.parameters[0] = 32;
    ai.parameters[1] = 32;
    kind = 220;
    ground_offset = 271;
    beams[0] = 0;
    beams[1] = 0;
    flamethrower = 0;
    ai.parameters[9] &= 0xFF00;
    ai.state = 0;
    ai.phase = 0;
    control.hierarchy_ready = 0;
    camera_subject.SetObject(&model);
    projection.enabled = true;
    projection.dir.x = 0;
    projection.dir.y = -ONE;
    projection.dir.z = 0;
    projection.color = 0x808080;
}

TheBeast::~TheBeast()
{
    for (uint32 idx = 0; idx < 2; ++idx)
        if (beams[idx])
            beams[idx]->Destroy(3);
    if (flamethrower)
        flamethrower->Destroy(3);
    if (held_projectile)
        held_projectile->Destroy(3);
    for (uint32 idx = 0; idx < 2; ++idx)
        if (charge_rings[idx])
            charge_rings[idx]->Destroy(3);
}

uint32 TheBeast::Destroy(uint32 reason)
{
    uint32 idx;
    FUNCTION_MARKER(0x80040B64, "SLUS_003.73");
    VisualObject::ClearTrackedHealth();
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    for (idx = 0; idx < 2; ++idx)
    {
        if (beams[idx])
            beams[idx]->Destroy(3);
        beams[idx] = 0;
    }
    if (flamethrower)
        flamethrower->Destroy(3);
    flamethrower = 0;
    if (held_projectile)
        held_projectile->Destroy(3);
    held_projectile = 0;
    projection.enabled = false;
    camera_subject.Reset();
    for (idx = 0; idx < 2; ++idx)
    {
        if (charge_rings[idx])
            charge_rings[idx]->Destroy(3);
        charge_rings[idx] = 0;
    }
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 TheBeast::SpawnPickup()
{
    RecordDataBorrow rec_borrow;
    uint16 mode, kind, rec_idx;
    uint32 idx, x, z, choice;
    Pickup *child;
    uint16 *entry;
    ModelInstance *player, *candidate;
    Vector3 pos;
    sint16 timer, health, maximum;
    FUNCTION_MARKER(0x80040D14, "SLUS_003.73");
    timer = (sint16)ai.work[1];
    if (timer < 150)
    {
        ai.work[1] = timer + 1;
        return timer + 1;
    }
    ai.work[1] = 0;
    mode = ai.parameters[2];
    if (mode < 5)
        return 0x800F0000;
    maximum = GetGameTuning().player.maximum_health;
    player = visual_lists.players;
    health = static_cast<VisualObject *>(player->owner)->health;
    kind = 0;
    if (health < (maximum >> 1) && RandomRange(3) == 0)
        kind = 14;
    if (!kind)
    {
        choice = RandomRange(6);
        switch (choice)
        {
            case 0:
                kind = 5;
                break;
            case 1:
                kind = 3;
                break;
            case 2:
                kind = 4;
                break;
            case 3:
                kind = 1;
                break;
            case 4:
                kind = 10;
                break;
            default:
                kind = 14;
                break;
        }
    }
    mode = ai.parameters[2];
    if (mode < 5 || mode >= 10)
        return 0;
    entry = spawn_points.grps[3];
    ShuffleSpawnPoints(entry, 5);
    for (idx = 0; idx < 5; ++idx)
    {
        rec_idx = *entry;
        ReadRecPos(&pos, rec_idx);
        if (CollisionRay::FindGroundHeight(pos, 0, 2048) != (uint32)-1)
        {
            candidate = visual_lists.pickups;
            if (candidate)
            {
                x = (uint32)pos.x;
                z = (uint32)pos.z;
                while (candidate)
                {
                    if (candidate->pos.x == x && candidate->pos.z == z)
                        break;
                    candidate = candidate->next;
                }
            }
            if (!candidate)
            {
                child = Pickup::Spawn(kind, &pos, 7, 600, &zero_vector);
                if (!child)
                    return 0;
                child->SetMotionParameters(8192, 4);
                uint32 floor_y = pos.y;
                child->ignore_floor = 0;
                child->floor_y = floor_y + 6553600;
                return 6553600;
            }
        }
        ++entry;
    }
    return 0;
}

uint32 TheBeast::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    ModelInstance *player;
    uint32 flags;
    uint8 mode;
    uint32 event;
    sint8 jump_frame;
    uint8 attack_frame;
    uint16 activation;
    uint8 activation_count;
    uint16 model_flags;
    GameHeapBlock *child;
    uint16 timer;
    uint16 damage;
    uint16 speed;
    uint32 turn_scale;
    uint32 duration;
    sint16 height;
    bool reached;
    uint32 distance;
    Vector3 pos, muzzle, player_ground, discarded_dir;
    ShortVector3 desired, weapon_angles;
    sint32 velocity_x, velocity_z, velocity_y;
    sint16 pitch;
    CollisionRay raycast;
    FUNCTION_MARKER(0x80040F34, "SLUS_003.73");
    motion.acceleration = zero_vector;
    UpdateModelAnimationData();
    UpdateWorldTransform();
    BuildModelPose();
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        activation_count = control.activation_count;
        activation_flags = activation & 65534;
        control.activation_count = activation_count + 1;
    }
    mode = model.animation;
    if (mode == 3)
    {
        event = DidAnimationCrossFrame(3, 5);
        if (UpdateEventLatch(event, 1))
        {
            PlayPositionedSound(137, &model.pos, 0);
            GetModelAttachment(pos, *this, 6);
            pos.y = (uint32)pos.y + (131072);
            child = EffectObject::AllocateStorage(112);
            if (child)
            {
                new (child) BeastGroundEffect(&pos, 10, 64, 96, 96, 20, 64, 64);
            }
        }
        event = DidAnimationCrossFrame(3, 31);
        if (UpdateEventLatch(event, 2))
        {
            PlayPositionedSound(137, &model.pos, 0);
            GetModelAttachment(pos, *this, 5);
            pos.y = (uint32)pos.y + (131072);
            child = EffectObject::AllocateStorage(112);
            if (child)
            {
                new (child) BeastGroundEffect(&pos, 10, 64, 96, 96, 20, 64, 64);
            }
        }
    }
    else if (mode == 4)
    {
        event = DidAnimationCrossFrame(4, 15);
        if (UpdateEventLatch(event, 1))
        {
            GetModelAttachment(pos, *this, 5);
            pos.y = (uint32)pos.y + (131072);
            child = EffectObject::AllocateStorage(112);
            if (child)
            {
                new (child) BeastGroundEffect(&pos, 10, 64, 96, 96, 20, 64, 128);
            }
            PlayPositionedSound(137, &pos, 0);
            static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(1);
        }
    }
    if (control.script_active)
    {
        timer = control.script_delay;
        if (timer)
            control.script_delay = timer - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateBehavior();
    if (!control.hierarchy_ready)
    {
        model.SetAttachmentTable(TheBeastAttachments);
        InitializeModelAnimation(TheBeastHierarchy);
        model_flags = model.flags;
        control.hierarchy_ready = 1;
        model.flags = model_flags | 4;
    }
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    raycast.endpoints[0] = model.pos;
    raycast.endpoints[0].y = (uint32)raycast.endpoints[0].y - (262144);
    raycast.endpoints[1] = raycast.endpoints[0];
    raycast.endpoints[1].y = (uint32)raycast.endpoints[1].y + (8388608);
    raycast.Initialize();
    raycast.Query(1);
    if (raycast.hit.GetObject())
    {
        projection.enabled = true;
        surface.pos = raycast.hit_pos;
        surface.n.vx = raycast.n.vx;
        surface.n.vy = raycast.n.vy;
        surface.n.vz = raycast.n.vz;
    }
    else
        projection.enabled = false;
    flags = control.flags;
    if (flags & 16)
    {
        ModelInstance *subject = tracked_target.GetObject();
        if (subject)
            aim_pos = subject->pos;
        else
            control.flags &= ~16;
        desired.SetDir(model.pos, aim_pos);
        desired.x = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 24);
    }
    else if (flags & 32)
    {
        desired.SetDir(model.pos, aim_pos);
        desired.x = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 16);
    }
    flags = control.flags;
    if (flags & 64)
    {
        desired.SetDir(model.pos, bone_aim_pos);
        desired.x = 0;
        desired.y = ((uint16)desired.y - (uint16)model.angles.y) & 4095;
    }
    else
    {
        desired.x = 0;
        desired.y = 0;
        desired.z = 0;
    }
    UpdateAngularAcceleration(aim.angles, aim.velocity, aim.acceleration, desired, 16);
    aim.angles.AddAngles(aim.velocity);
    aim.angles.NormalizeAngles();
    aim.velocity.AddAngles(aim.acceleration);
    aim.velocity.DampAngles(motion.angular_drag);
    aim.velocity.SnapAngles();
    GetModelJointAngles(17).rotation.y = aim.angles.y;
    flags = control.flags;
    if (!(flags & 1))
        goto StoppedAnimation;
    if (flags & 4)
    {
        jump_frame = model.frame;
        if (jump_frame < 18)
        {
            playback.speed = 65536;
            control.gravity_enabled = 0;
            motion.velocity.z = 0;
            motion.velocity.y = 0;
            motion.velocity.x = 0;
            motion.acceleration.z = 0;
            motion.acceleration.y = 0;
            motion.acceleration.x = 0;
        }
        else if (jump_timer)
            control.gravity_enabled = 1;
        jump_frame = model.frame;
        if (jump_frame >= 18 && !(control.flags & 8))
        {
            duration = jump_timer;
            velocity_x = DivideSignedWord((sint32)((uint32)dst.x - model.pos.x), duration);
            duration = jump_timer;
            velocity_z = DivideSignedWord((sint32)((uint32)dst.z - model.pos.z), duration);
            duration = jump_timer;
            velocity_y = DivideSignedWord((sint32)((uint32)dst.y - model.pos.y), duration);
            flags = control.flags;
            control.flags = flags | 8;
            motion.velocity.x = velocity_x;
            motion.velocity.z = velocity_z;
            motion.velocity.y = velocity_y - (duration << 14);
            PlayPositionedSound(139, &model.pos, 0);
        }
        else
        {
            jump_frame = model.frame;
            if (jump_frame >= 38)
            {
                if (!jump_timer)
                    goto JumpFinished;
                playback.speed = 0;
            }
        }
        timer = jump_timer;
        if (timer)
        {
            --timer;
            jump_timer = timer;
            if (!timer)
            {
                model.pos = dst;
                pos = model.pos;
                height = ground_offset;
                pos.y = (uint32)pos.y + ((height - 32) * ONE);
                if (ai.state != 4)
                {
                    child = EffectObject::AllocateStorage(112);
                    if (child)
                    {
                        new (child) BeastGroundEffect(&pos, 10, 64, 96, 96, 20, 64, 128);
                    }
                }
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
                motion.acceleration.z = 0;
                motion.acceleration.y = 0;
                control.gravity_enabled = 0;
                Camera *impact_camera = static_cast<Camera *>(visual_lists.cameras->owner);
                motion.acceleration.x = 0;
                playback.speed = 65536;
                impact_camera->ApplyExplosionShake(0);
                PlayPositionedSound(138, &model.pos, 0);
            }
        }
    JumpFinished:
        if (playback.complete)
        {
            flags = control.flags;
            control.flags = flags & 0xFFFFFFF2;
        }
    }
    else
    {
        distance = model.pos.Distance(dst);
        turn_scale = distance ? 32768 / distance : (uint32)-1;
        if ((sint32)turn_scale >= 129)
            turn_scale = 128;
        desired.SetDir(model.pos, dst);
        desired.x = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, turn_scale);
        speed = ai.parameters[0];
        motion.velocity.SetDir(speed, model.angles);
        flags = control.flags;
        if (flags & 2)
        {
            pos.x = model.pos.x;
            pos.y = dst.y;
            pos.z = model.pos.z;
            distance = pos.EuclideanDistance(dst);
            reached = distance < 64;
        }
        else
        {
            distance = model.pos.EuclideanDistance(dst);
            reached = distance < 128;
        }
        if (reached)
        {
            flags = control.flags;
            control.flags = flags & 0xFFFFFFFE;
        }
    }
    if (control.flags & 1)
    {
        if (!model.animation && playback.complete)
            StartAnimation(3, 1);
        mode = model.animation;
        goto AttackAnimation;
    }
StoppedAnimation:
    mode = model.animation;
    if (mode == 3)
    {
        event = DidAnimationCrossFrame(3, 43);
        if (UpdateEventLatch(event, 1))
            SetAnimationRange(8, 0, -1);
        event = DidAnimationCrossFrame(3, 17);
        if (UpdateEventLatch(event, 2))
            SetAnimationRange(9, 0, -1);
        mode = model.animation;
    }
AttackAnimation:
    if (mode == 2)
    {
        attack_frame = model.frame;
        if (attack_frame >= 15 && attack_frame < 21)
        {
            player = visual_lists.players;
            if (player)
            {
                GetModelAttachment(pos, *this, 0);
                player = visual_lists.players;
                discarded_dir.Subtract(pos, player->pos);
                discarded_dir.Len();
            }
        }
    }
    if (GetBehaviorVal(0, 0))
    {
        for (uint32 idx = 0; idx < 2; ++idx)
        {
            if (!beams[idx])
            {
                GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
                ParticleBeam *created = 0;
                if (allocation)
                {
                    created = new (allocation) ParticleBeam(&visual_lists.players, 4096, 0, 128, 134);
                }
                beams[idx] = created;
                damage = GetBehaviorVal(2, 1);
                if (!beams[idx])
                {
                    GameDiagnostic(("TheBeast particle beam allocation failed\n"));
                    abort();
                }
                beams[idx]->damage = damage;
                beams[idx]->SetColors(255, 64, 0, 185, 185, 185, 160, 40, 0, 255, 255, 255);
            }
        }
        if (beams[0] || beams[1])
        {
            GetModelAttachment(pos, *this, 3);
            weapon_angles.SetDir(pos, visual_lists.players->pos);
            player_ground = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos;
            distance = player_ground.HorizontalDistance(pos);
            if ((sint32)distance < 768)
                distance = 768;
            pitch = ratan2((sint32)((uint32)player_ground.y - pos.y) >> 12, (sint32)distance);
            GetModelAttachment(muzzle, *this, 1);
            weapon_angles.SetDir(pos, muzzle);
            weapon_angles.x = pitch;
            if (beams[0])
                beams[0]->Fire(muzzle, weapon_angles);
            if (beams[1])
            {
                GetModelAttachment(muzzle, *this, 2);
                weapon_angles.SetDir(pos, muzzle);
                weapon_angles.x = pitch;
                beams[1]->Fire(muzzle, weapon_angles);
            }
        }
    }
    else
    {
        for (uint32 idx = 0; idx < 2; ++idx)
        {
            if (beams[idx])
                beams[idx]->Destroy(3);
            beams[idx] = 0;
        }
    }
    if (GetBehaviorVal(1, 0))
    {
        if (!flamethrower)
        {
            GameHeapBlock *allocation = AllocateZeroedHeapBlock(72);
            Flamethrower *created = 0;
            if (allocation)
            {
                created = new (allocation) Flamethrower(&visual_lists.players, 0);
            }
            flamethrower = created;
            damage = GetBehaviorVal(1, 1);
            if (!flamethrower)
            {
                GameDiagnostic(("TheBeast flamethrower allocation failed\n"));
                abort();
            }
            flamethrower->damage = damage;
            flamethrower->Configure(2000);
        }
        if (flamethrower)
        {
            GetModelAttachment(muzzle, *this, 3);
            mode = model.animation;
            if (mode == 6)
            {
                weapon_angles.SetDir(muzzle, visual_lists.players->pos);
                pitch = weapon_angles.x;
                GetModelAttachment(pos, *this, 4);
                weapon_angles.SetDir(muzzle, pos);
                weapon_angles.x = pitch;
            }
            else
            {
                GetModelAttachment(pos, *this, 4);
                weapon_angles.SetDir(muzzle, pos);
            }
            flamethrower->Fire(pos, weapon_angles);
        }
    }
    else if (flamethrower)
    {
        flamethrower->Destroy(3);
        flamethrower = 0;
    }
    mode = model.animation;
    if (mode == 7)
    {
        event = DidAnimationCrossFrame(7, 25);
        if (UpdateEventLatch(event, 1))
        {
            GetModelAttachment(pos, *this, 0);
            GameHeapBlock *allocation = AllocateZeroedObject(428);
            GuidedProjectile *created = 0;
            if (allocation)
            {
                created = new (allocation) GuidedProjectile(pos, 10, &visual_lists.players, 1);
            }
            held_projectile = created;
            if (!held_projectile)
            {
                GameDiagnostic(("TheBeast held projectile allocation failed\n"));
                abort();
            }
            held_projectile->lifecycle_flags |= 32;
            held_projectile->SetLaunchDelay(0);
        }
        event = DidAnimationCrossFrame(7, 68);
        if (UpdateEventLatch(event, 2))
        {
            if (held_projectile)
            {
                GetModelAttachment(muzzle, *this, 0);
                weapon_angles.SetDir(muzzle, visual_lists.players->pos);
                weapon_angles.y = model.angles.y;
                held_projectile->SetAngles(weapon_angles);
                held_projectile->Launch(0, &static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos);
                held_projectile->lifecycle_flags &= 65503;
            }
            held_projectile = 0;
        }
        if (held_projectile)
        {
            GetModelAttachment(pos, *this, 0);
            held_projectile->model.pos = pos;
        }
    }
    return SpawnPickup();
}

void TheBeast::Resize(sint32 inc)
{
    FUNCTION_MARKER(0x80041FDC, "SLUS_003.73");
    uint16 flags = model.flags;
    uint32 scale = model.scale.x;
    scale += inc;
    model.flags = flags | 0x200;
    uint32 product = scale * 271;
    model.scale.z = scale;
    model.scale.y = scale;
    model.scale.x = scale;
    sint16 old_height = ground_offset;
    ground_offset = (sint32)product >> 12;
    sint32 displacement = (ground_offset - old_height) * ONE;
    model.pos.y = (uint32)model.pos.y - displacement;
}

uint32 TheBeast::UpdateLighting()
{
    uint32 random;
    bool active = false;
    uint8 r, g, b;
    LightColor flicker, color;
    ShortVector3 dir;
    FUNCTION_MARKER(0x80042038, "SLUS_003.73");
    random = RandomRange(256);
    flicker.r = random << 4;
    random = RandomRange(random >> 1);
    flicker.g = random << 4;
    flicker.b = 0;
    if (beams[0])
    {
        if (beams[0]->ReadLight(&r, &g, &b))
        {
            TheBeast::ReadLightDir(*this, 1, true, dir);
            color.r = r << 4;
            color.g = g << 4;
            color.b = b << 4;
            active = true;
        }
    }
    else if (flamethrower)
    {
        if (flamethrower->ReadLight(&r, &g, &b))
        {
            TheBeast::ReadLightDir(*this, 4, false, dir);
            color.r = r << 4;
            color.g = g << 4;
            color.b = b << 4;
            active = true;
        }
    }
    else if (held_projectile)
    {
        TheBeast::ReadLightDir(*this, 0, false, dir);
        color.r = 2560;
        color.g = 1024;
        color.b = 0;
        active = true;
    }
    if (!active)
    {
        dir.x = 0;
        dir.y = 0;
        dir.z = 0;
        color.r = 0;
        color.g = 0;
        color.b = 0;
    }
    static const ShortVector3 dir0 = {{{0, -ONE, 0}}};
    static const ShortVector3 dir1 = {{{0, ONE, 0}}};
    static const LightColor base_color = {2048, 2048, 2048};
    SetLightingDirs(dir0, dir1, dir);
    SetLightingColors(base_color, flicker, color);
    game_lighting.background_color.b = 64;
    game_lighting.background_color.g = 64;
    game_lighting.background_color.r = 64;
    return 64;
}

uint32 TheBeast::Damage(uint32 damage, const Vector3 *, uint32 reason)
{
    uint8 pulse;
    sint16 health;
    sint32 applied;
    FUNCTION_MARKER(0x800422C0, "SLUS_003.73");
    pulse = control.damage_phase;
    control.damage_phase = pulse + 1;
    health = this->health;
    if (health <= 0)
        return 0;
    applied = (sint32)damage >> 1;
    if (reason == 5)
        applied >>= 1;
    else if (reason == 10)
        applied >>= 2;
    this->health = health - applied;
    if (this->health <= 0)
        VisualObject::ClearTrackedHealth();
    SetColor(3, 255, 255, 255);
    return 1;
}

void TheBeast::UpdateBehavior()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    uint16 state;
    uint16 phase;
    uint16 val;
    sint32 floor;
    sint16 height;
    uint8 attack_frame;
    uint16 model_flags;
    uint32 flags;
    GameHeapBlock *child;
    uint32 idx;
    uint32 best;
    uint32 sel;
    sint16 chosen;
    bool use_beam;
    uint32 distance;
    uint32 scale;
    uint16 counter;
    uint16 yaw;
    Camera *camera;
    ModelInstance *player;
    const uint16 *entry;
    Vector3 pos, dst, delta, normalized, scaled, sample;
    uint32 shift, scalar;
    ShortVector3 angles;
    uint8 voices[3];
    sint32 health, old_counter;
    sint16 step;
    bool selection_valid;
    CollisionRay raycast;
    FUNCTION_MARKER(0x80042360, "SLUS_003.73");
    state = ai.state;
    if (state > 128)
        return;
    switch (state)
    {
        case 0:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    floor = CollisionRay::FindGroundHeight(model.pos, 256, 1024);
                    if (floor != -1)
                    {
                        height = ground_offset;
                        model.pos.y = (uint32)floor - height * ONE;
                    }
                    player = visual_lists.players;
                    static_cast<Player *>(visual_lists.players->owner)->disable_timer = 150;
                    this->health = 300;
                    TrackHealth(24, 42, 4096);
                    AdvancePhase();
                    return;
                case 1:
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(&model);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(&model);
                    camera->SetOrbitDistanceTransition(1100, 0);
                    camera->SetOrbitHeightTransition(-4096, 0);
                    camera->SetTargetLateralOffsetTransition(0, 0);
                    camera->SetTargetHeightOffsetTransition(3402, 0);
                    camera->SetTargetForwardOffsetTransition(-916, 0);
                    phase = ai.phase;
                    ai.work[0] = 45;
                    ai.phase = phase + 1;
                    return;
                case 2:
                case 3:
                    counter = ai.work[0];
                    old_counter = (sint16)ai.work[0];
                    if (counter)
                    {
                        ai.work[0] = old_counter - 1;
                        return;
                    }
                    if (phase == 2)
                    {
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        camera->SetOrbitHeightTransition(-206, 30);
                        camera->SetTargetHeightOffsetTransition(90, 30);
                        camera->SetTargetForwardOffsetTransition(604, 30);
                        phase = ai.phase;
                        ai.work[0] = 30;
                        ai.phase = phase + 1;
                    }
                    else
                    {
                        voices[0] = voice_choices[0];
                        voices[1] = voice_choices[1];
                        voices[2] = voice_choices[2];
                        idx = RandomRange(3);
                        ::SelectCdVoice(19, voices[idx]);
                        PlayAnimation(6);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (!playback.complete)
                        return;
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    player = visual_lists.players;
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(visual_lists.players);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(visual_lists.players);
                    camera->SetOrbitDistanceTransition(1100, 30);
                    camera->SetOrbitHeightTransition(-308, 30);
                    camera->SetTargetLateralOffsetTransition(0, 30);
                    camera->SetTargetHeightOffsetTransition(68, 30);
                    camera->SetTargetForwardOffsetTransition(216, 30);
                    SelectState(1);
                    return;
                default:
                    return;
            }
        case 1:
            if (ai.phase)
                return;
            if (this->health <= 0)
            {
                ai.state = 3;
                return;
            }
            if (player_distance < 768)
            {
                state = ai.state;
                phase = ai.phase;
                SelectState(80);
                ai.parameters[2] = state;
                ai.parameters[3] = phase;
                return;
            }
            state = ai.state;
            phase = ai.phase;
            ai.parameters[2] = state;
            ai.parameters[3] = phase;
            if (!RandomRange(2))
                SelectState(64);
            else
            {
                ai.parameters[5] = 768;
                ai.parameters[4] = 0;
                SelectState(112);
            }
            return;
        case 3:
            phase = ai.phase;
            if (!phase)
            {
                idx = spawn_points.records[0];
                ::SpawnRec(idx);
                PlayPositionedSound(132, &model.pos, 0);
                PlayAnimation(5);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if (playback.complete)
                {
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->ApplyExplosionShake(1);
                    idx = spawn_points.records[1];
                    ::SpawnRec(idx);
                    idx = spawn_points.records[2];
                    ::SpawnRec(idx);
                    SelectState(4);
                }
                else
                    Resize(20);
            }
            return;
        case 4:
            phase = ai.phase;
            if (!phase)
            {
                ai.parameters[1] = 48;
                SetJumpDst(&this->model.pos);
                AdvancePhase();
            }
            else if (phase == 1 && !(control.flags & 1))
            {
                ::SetScrFlash(255, 255, 255, 8, 0, 0);
                SelectState(5);
                ai.parameters[1] = 48;
            }
            return;
        case 5:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    this->health = 200;
                    TrackHealth(24, 42, 4096);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    ai.parameters[0] = 48;
                    camera->mode = 18;
                    AdvancePhase();
                    return;
                case 1:
                    if (this->health <= 0)
                    {
                        ai.phase = 3;
                        return;
                    }
                    distance = player_distance;
                    if (distance < 3072)
                    {
                        state = ai.state;
                        phase = ai.phase;
                        SelectState(distance < 768 ? 80 : 96);
                        ai.parameters[2] = state;
                        ai.parameters[3] = phase;
                        return;
                    }
                    player = visual_lists.players;
                    if (!player)
                        return;
                    if (ChoosePos(&dst, &visual_lists.players->pos, 512, 1024, 128, 1024, 1, 0))
                    {
                        ClearFacing();
                        SetDst(&dst);
                        AdvancePhase();
                    }
                    else
                        ClearVelocity();
                    return;
                case 2:
                    if (!(control.flags & 1) || !RandomRange(32))
                        ai.phase = 1;
                    return;
                case 3:
                    idx = spawn_points.records[3];
                    ReadRecPos(&pos, idx);
                    floor = CollisionRay::FindGroundHeight(pos, 2048, 2048);
                    pos.y = floor;
                    if (floor != -1)
                    {
                        JumpToPoint(pos);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (!(control.flags & 1))
                    {
                        PlayPositionedSound(132, &model.pos, 0);
                        PlayAnimation(5);
                        AdvancePhase();
                    }
                    return;
                case 5:
                    if (!playback.complete)
                        Resize(20);
                    else
                    {
                        state = ai.state;
                        ai.phase = 0;
                        ai.state = state + 1;
                    }
                    return;
                default:
                    return;
            }
        case 6:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    this->health = 600;
                    TrackHealth(24, 42, 4096);
                    phase = ai.phase;
                    ai.work[0] = 0;
                    ai.phase = phase + 1;
                    return;
                case 1:
                    health = this->health;
                    step = (sint16)ai.work[0];
                    if (health <= 0 && step >= 5)
                    {
                        idx = spawn_points.records[3];
                        ReadRecPos(&pos, idx);
                        floor = CollisionRay::FindGroundHeight(pos, 2048, 2048);
                        pos.y = floor;
                        if (floor != -1)
                        {
                            JumpToPoint(pos);
                            ai.phase = 8;
                        }
                        return;
                    }
                    if (step < 5 && this->health < 600 - 90 * (step + 1))
                    {
                        AdvancePhase();
                        return;
                    }
                    if (player_distance < 3072)
                    {
                        ai.phase = 3;
                        return;
                    }
                    player = visual_lists.players;
                    if (!player)
                        return;
                    if (!ChoosePos(&pos, &visual_lists.players->pos, 512, 1024, 128, 1024, 1, 0))
                    {
                        ClearVelocity();
                        ai.phase = 3;
                        return;
                    }
                    delta.Subtract(pos, this->model.pos);
                    delta.y = 0;
                    scalar = 0;
                    while (scalar < 8)
                    {
                        shift = 3;
                        normalized.ShiftRight(delta, shift);
                        scaled.Multiply(normalized, scalar);
                        sample.Add(model.pos, scaled);
                        raycast.endpoints[0] = sample;
                        raycast.endpoints[0].y = (uint32)raycast.endpoints[0].y - (2097152);
                        raycast.endpoints[1] = sample;
                        raycast.endpoints[1].y = (uint32)raycast.endpoints[1].y + (8388608);
                        raycast.Initialize();
                        raycast.triggers = false;
                        raycast.Query(1);
                        if (!raycast.hit.GetObject())
                        {
                            ClearVelocity();
                            ai.phase = 3;
                            return;
                        }
                        ++scalar;
                    }
                    ClearFacing();
                    SetDst(&pos);
                    AdvancePhase();
                    return;
                case 2:
                case 4:
                    step = (sint16)ai.work[0];
                    if (step >= 5 || this->health >= 600 - 90 * (step + 1))
                    {
                        if (phase == 4)
                            ai.phase = 1;
                        else if (!(control.flags & 1) || !RandomRange(32))
                        {
                            phase = ai.phase;
                            ai.phase = phase - 1;
                        }
                        return;
                    }
                    if (step < 0 || step >= 5)
                    {
                        GameDiagnostic(("TheBeast spawn point index out of range: %d\n", step));
                        abort();
                    }
                    idx = spawn_points.grps[0][step];
                    ReadRecPos(&pos, idx);
                    floor = CollisionRay::FindGroundHeight(pos, 2048, 2048);
                    pos.y = floor;
                    if (floor == -1)
                        pos.y = surface.pos.y;
                    ClearFacing();
                    height = ground_offset;
                    pos.y = (uint32)pos.y - (height * ONE);
                    if (phase == 2)
                        ClearVelocity();
                    SetJumpDst(&pos);
                    if (phase == 2)
                        ai.phase = 5;
                    else
                        AdvancePhase();
                    return;
                case 3:
                    state = ai.state;
                    phase = ai.phase;
                    ai.phase = 0;
                    ai.parameters[2] = state;
                    distance = player_distance;
                    ai.parameters[3] = phase - 1;
                    ai.state = distance < 768 ? 80 : distance < 1024 ? 96 : 64;
                    return;
                case 5:
                    if (!(control.flags & 1))
                    {
                        PlayAnimation(4);
                        AdvancePhase();
                    }
                    return;
                case 6:
                    if (!playback.complete)
                    {
                        if ((sint8)this->model.frame == 15)
                        {
                            step = (sint16)ai.work[0];
                            player = visual_lists.solids;
                            if (step < 0 || step >= 5)
                            {
                                GameDiagnostic(("TheBeast spawn point index out of range: %d\n", step));
                                abort();
                            }
                            idx = spawn_points.grps[0][step];
                            VisualObject::ActivateByIdx(player, idx);
                            step = (sint16)ai.work[0];
                            ai.work[0] = step + 1;
                        }
                        return;
                    }
                    idx = spawn_points.records[3];
                    ReadRecPos(&pos, idx);
                    floor = CollisionRay::FindGroundHeight(pos, 2048, 2048);
                    pos.y = floor;
                    if (floor != -1)
                    {
                        JumpToPoint(pos);
                        AdvancePhase();
                    }
                    return;
                case 7:
                    if (!(control.flags & 1))
                        ai.phase = 1;
                    return;
                case 8:
                    if (!(control.flags & 1))
                    {
                        PlayPositionedSound(132, &model.pos, 0);
                        PlayAnimation(5);
                        AdvancePhase();
                    }
                    return;
                case 9:
                    if (!playback.complete)
                        Resize(20);
                    else
                    {
                        state = ai.state;
                        ai.phase = 0;
                        ai.state = state + 1;
                    }
                    return;
                default:
                    return;
            }
        case 7:
            phase = ai.phase;
            if (!phase)
            {
                this->health = 600;
                TrackHealth(24, 42, 4096);
                PlayAnimation(4);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if (playback.complete)
                    ai.phase = 2;
                else if ((sint8)this->model.frame == 15)
                {
                    entry = spawn_points.grps[1];
                    for (idx = 0; idx < 5; ++idx)
                    {
                        val = *entry;
                        ++entry;
                        player = visual_lists.solids;
                        VisualObject::ActivateByIdx(player, val);
                    }
                }
            }
            else if (phase == 2)
                SelectState(8);
            return;
        case 8:
            phase = ai.phase;
            if (phase == 0)
            {
                phase = ai.phase;
                ai.work[0] = 0;
                ai.phase = phase + 1;
                phase = 1;
            }
            switch (phase)
            {
                case 1:
                    if (this->health <= 0 && (sint16)ai.work[0] == 4)
                    {
                        for (idx = 0; idx < 5; ++idx)
                            if (!used_points[idx])
                            {
                                ai.work[2] = idx;
                                break;
                            }
                        SelectState(9);
                        return;
                    }
                    player = visual_lists.players;
                    distance = model.pos.HorizontalDistance(player->pos);
                    scale = (uint32)((3 * model.scale.x) >> 4);
                    state = ai.state;
                    phase = ai.phase;
                    SelectState(distance < scale ? 80 : 64);
                    ai.parameters[2] = state;
                    ai.parameters[3] = phase + 1;
                    return;
                case 2:
                    step = (sint16)ai.work[0];
                    if (step >= 4 || this->health >= 600 - 120 * (step + 1))
                    {
                        ai.phase = 1;
                        return;
                    }
                    best = (uint32)-1;
                    selection_valid = false;
                    sel = 0;
                    entry = spawn_points.grps[2];
                    for (idx = 0; idx < 5; ++idx)
                    {
                        if (!used_points[idx])
                        {
                            val = *entry;
                            ReadRecPos(&pos, val);
                            player = visual_lists.players;
                            distance = pos.EuclideanDistance(player->pos);
                            if (distance < best)
                            {
                                sel = idx;
                                best = distance;
                                selection_valid = true;
                            }
                        }
                        ++entry;
                    }
                    if (!selection_valid)
                    {
                        // TODO Original reads an undefined saved register when no candidate wins
                        GameDiagnostic(("TODO TheBeast AI state8 has no selected point\n"));
                        abort();
                    }
                    used_points[sel] = 1;
                    val = spawn_points.grps[2][sel];
                    ReadRecPos(&pos, val);
                    floor = CollisionRay::FindGroundHeight(pos, 2048, 2048);
                    pos.y = floor;
                    if (floor == -1)
                        ai.phase = 1;
                    else
                    {
                        ai.work[2] = sel;
                        JumpToPoint(pos);
                        AdvancePhase();
                    }
                    return;
                case 3:
                    if (!(control.flags & 1))
                    {
                        PlayAnimation(4);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (!playback.complete)
                    {
                        if ((sint8)this->model.frame == 15)
                        {
                            step = (sint16)ai.work[0];
                            ai.work[0] = step + 1;
                            chosen = (sint16)ai.work[2];
                            player = visual_lists.solids;
                            if (chosen < 0 || chosen >= 5)
                            {
                                GameDiagnostic(("TheBeast spawn point index out of range: %d\n", chosen));
                                abort();
                            }
                            val = spawn_points.grps[2][chosen];
                            VisualObject::ActivateByIdx(player, val);
                        }
                        return;
                    }
                    val = spawn_points.records[3];
                    ReadRecPos(&pos, val);
                    floor = CollisionRay::FindGroundHeight(pos, 2048, 2048);
                    pos.y = floor;
                    if (floor != -1)
                    {
                        JumpToPoint(pos);
                        AdvancePhase();
                    }
                    return;
                case 5:
                    if (!(control.flags & 1))
                        ai.phase = 1;
                    return;
                default:
                    return;
            }
        case 9:
            phase = ai.phase;
            if (!phase)
            {
                this->health = 400;
                SetBehaviorVal(2, 1, 4);
                TrackHealth(24, 42, 4096);
                val = spawn_points.records[3];
                ReadRecPos(&pos, val);
                chosen = (sint16)ai.work[2];
                if (chosen < 0 || chosen >= 5)
                {
                    GameDiagnostic(("TheBeast spawn point index out of range: %d\n", chosen));
                    abort();
                }
                val = spawn_points.grps[2][chosen];
                ReadRecPos(&delta, val);
                delta.Subtract(pos);
                delta.y = 0;
                normalized.Normalize(delta);
                scalar = 512;
                scaled.Multiply(normalized, scalar);
                pos.Add(scaled);
                floor = CollisionRay::FindGroundHeight(pos, 2048, 2048);
                pos.y = floor;
                if (floor != -1)
                {
                    ClearFacing();
                    height = ground_offset;
                    pos.y = (uint32)pos.y - (height * ONE);
                    SetDst(&pos);
                    AdvancePhase();
                }
            }
            else if (phase == 1)
            {
                if (!(control.flags & 1))
                    ai.phase = 2;
            }
            else if (phase == 2)
            {
                if (this->health <= 0)
                {
                    SelectState(10);
                    return;
                }
                player = visual_lists.players;
                distance = model.pos.HorizontalDistance(player->pos);
                scale = (uint32)((3 * model.scale.x) >> 4);
                if (distance < scale)
                {
                    state = ai.state;
                    phase = ai.phase;
                    SelectState(128);
                    ai.parameters[2] = state;
                    ai.parameters[3] = phase;
                    return;
                }
                state = ai.state;
                phase = ai.phase;
                ai.parameters[3] = phase;
                ai.parameters[2] = state;
                chosen = (sint16)ai.work[2];
                if (chosen < 0 || chosen >= 5)
                {
                    GameDiagnostic(("TheBeast spawn point index out of range: %d\n", chosen));
                    abort();
                }
                val = spawn_points.grps[2][chosen];
                ReadRecPos(&pos, val);
                player = visual_lists.players;
                use_beam = false;
                if (pos.HorizontalDistance(player->pos) < 1024)
                    use_beam = RandomRange(3) != 0;
                if (use_beam)
                {
                    ai.parameters[5] = 768;
                    ai.parameters[4] = 1;
                    SelectState(112);
                }
                else
                    SelectState(64);
            }
            return;
        case 10:
            phase = ai.phase;
            if (phase >= 1 && phase <= 3)
            {
                if (!RandomRange(60))
                    PlayPositionedSound(132, &model.pos, 3);
                yaw = ai.work[2];
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                yaw = (yaw - 16) & 4095;
                ai.work[2] = yaw;
                camera->SetYawTransition(yaw, 4);
            }
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    player = visual_lists.players;
                    static_cast<Player *>(visual_lists.players->owner)->disable_timer = 900;
                    PlayPositionedSound(132, &model.pos, 0);
                    StartAnimation(12, 1);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    ai.work[0] = 0;
                    ai.work[2] = 2048;
                    if (camera)
                    {
                        camera->follow.flags = 1;
                        camera->aim.flags = 1;
                        camera->follow.target.SetObject(&model);
                        camera->aim.target.SetObject(&model);
                        camera->mode = 3;
                        camera->SetYawTransition(2048, 4);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        camera->SetOrbitDistanceTransition(1324, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        camera->SetOrbitHeightTransition(-854, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        camera->SetTargetLateralOffsetTransition(0, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        camera->SetTargetHeightOffsetTransition(-144, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        camera->SetTargetForwardOffsetTransition(-472, 16);
                    }
                    AdvancePhase();
                    return;
                case 1:
                    counter = ai.work[0];
                    old_counter = (sint16)ai.work[0];
                    if (counter < 180)
                    {
                        ai.work[0] = old_counter + 1;
                        if (!RandomRange(3))
                        {
                            use_small_effect_pool = false;
                            child = EffectObject::AllocateStorage(92);
                            if (child)
                            {
                                new (child) BeastOrbitEffect(&this->model.pos);
                            }
                            use_small_effect_pool = true;
                        }
                        return;
                    }
                    child = EffectObject::AllocateStorage(124);
                    if (child)
                    {
                        new (child) ExpandingGlow(&model.pos, 255, 128, 0, 512, 32);
                    }
                    PlayPositionedSound(1, &model.pos, 0);
                    AdvancePhase();
                    return;
                case 2:
                    model_flags = model.flags;
                    model.flags = model_flags & 65407;
                    SetColor(0, 255, 0, 0);
                    model_effect.color_mode = 0x02404040;
                    model_effect.scale = 10240;
                    phase = ai.phase;
                    model_effect.depth_bias = 10;
                    ai.phase = phase + 1;
                    return;
                case 3:
                    if (model.scale.x >= 2049)
                    {
                        Resize(-16);
                        return;
                    }
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    old_counter = camera->angles.y - 2048;
                    if (old_counter < 0)
                        old_counter = -old_counter;
                    if (old_counter < 128)
                    {
                        phase = ai.phase;
                        ai.work[0] = 0;
                        ai.phase = phase + 1;
                    }
                    return;
                case 4:
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->mode = 3;
                    camera->SetYawTransition(2048, 4);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->SetOrbitDistanceTransition(1324, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->SetOrbitHeightTransition(-316, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->SetTargetLateralOffsetTransition(0, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->SetTargetHeightOffsetTransition(220, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->SetTargetForwardOffsetTransition(-328, 16);
                    AdvancePhase();
                    return;
                case 5:
                    old_counter = (sint16)ai.work[0];
                    counter = old_counter;
                    ai.work[0] = old_counter + 1;
                    if (counter == 16)
                    {
                        model_flags = model.flags;
                        model_effect.color_mode = 0;
                        model.flags = model_flags | 1;
                        ::SetScrFlash(240, 240, 240, 32, 0, 0);
                        pos = this->model.pos;
                        height = ground_offset;
                        pos.y = (uint32)pos.y + (height * ONE);
                        child = AllocateZeroedObject(296);
                        if (child)
                        {
                            new (child) BeastFinalVisual(&pos);
                        }
                        PlayPositionedSound(1, &model.pos, 0);
                        pos.z = (uint32)pos.z - (524288);
                        pos.x = (uint32)pos.x + (737280);
                        FireCycle::Spawn(&pos, 220, 0, 2);
                        pos.x = (uint32)pos.x - (1474560);
                        FireCycle::Spawn(&pos, 220, 0, 2);
                        return;
                    }
                    if (counter >= 220)
                    {
                        Finish();
                        CompleteLevel(0);
                        return;
                    }
                    if (counter >= 17 && !(old_counter & 15))
                    {
                        child = EffectObject::AllocateStorage(124);
                        if (child)
                        {
                            new (child) ExpandingGlow(&model.pos, 255, 128, 0, 512, 32);
                        }
                    }
                    if (counter == 190)
                        ::FadeScrToWhite(30, 1);
                    return;
                default:
                    return;
            }
        case 64:
        case 80:
        case 96:
            phase = ai.phase;
            if (!phase)
            {
                BeginAttack();
                if (playback.loop_mode == 1)
                {
                    PlayAnimation(state == 64 ? 7 : state == 80 ? 2 : 6);
                    if (state == 80)
                        PlayPositionedSound(135, &model.pos, 0);
                    phase = ai.phase;
                    ai.phase = phase + 2;
                }
                else
                    AdvancePhase();
            }
            else if (phase == 1)
            {
                if (playback.complete)
                {
                    PlayAnimation(state == 64 ? 7 : state == 80 ? 2 : 6);
                    if (state == 80)
                        PlayPositionedSound(135, &model.pos, 0);
                    AdvancePhase();
                }
            }
            else if (phase == 2)
            {
                if (state == 64)
                {
                    attack_frame = model.frame;
                    SetBehaviorVal(1, 0, attack_frame >= 18 && attack_frame < 40);
                }
                else if (state == 96 && (sint8)this->model.frame >= 28)
                    SetBehaviorVal(1, 0, 1);
                player = visual_lists.players;
                if (!player || playback.complete)
                {
                    if (state == 80)
                        ai.phase = 3;
                    else
                        AdvancePhase();
                }
                else if (state == 80)
                {
                    attack_frame = model.frame;
                    if (attack_frame >= 13 && attack_frame < 22)
                    {
                        distance = model.pos.HorizontalDistance(player->pos);
                        scale = (uint32)((3 * model.scale.x) >> 4);
                        if (distance < scale)
                        {
                            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                            damage_target->Damage(2, &zero_vector, 0);
                        }
                    }
                }
            }
            else if (phase == 3)
            {
                if (state == 96)
                    SetBehaviorVal(1, 0, 0);
                ClearFacing();
                RestoreState();
            }
            return;
        case 112:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    flags = control.flags;
                    control.flags = flags & 0xFFFFFFFE;
                    motion.velocity.z = 0;
                    motion.velocity.y = 0;
                    use_beam = ai.parameters[4] != 0;
                    motion.velocity.x = 0;
                    if (use_beam)
                    {
                        chosen = (sint16)ai.work[2];
                        if (chosen < 0 || chosen >= 5)
                        {
                            GameDiagnostic(("TheBeast spawn point index out of range: %d\n", chosen));
                            abort();
                        }
                        val = spawn_points.grps[2][chosen];
                    }
                    else
                        val = spawn_points.beam_point;
                    ReadRecPos(&pos, val);
                    PosBeamPoint(pos, angles, 0);
                    SetFacingPoint(&pos);
                    phase = ai.phase;
                    ai.work[3] = 0;
                    ai.phase = phase + 1;
                    return;
                case 1:
                    counter = ai.work[3];
                    if (counter >= 32)
                    {
                        for (idx = 0; idx < 2; ++idx)
                        {
                            if (charge_rings[idx])
                            {
                                charge_rings[idx]->Destroy(3);
                                charge_rings[idx] = 0;
                            }
                        }
                        ClearFacing();
                        StartAnimation(1, 1);
                        SetBehaviorVal(0, 0, 1);
                        val = ReadBeamPoint();
                        ReadRecPos(&pos, val);
                        PosBeamPoint(pos, angles, 1);
                        SetBoneFacingPoint(&pos);
                        phase = ai.phase;
                        ai.work[3] = 0;
                        ai.phase = phase + 1;
                        return;
                    }
                    for (idx = 0; idx < 2; ++idx)
                    {
                        GetModelAttachment(pos, *this, idx + 1);
                        if (!charge_rings[idx])
                        {
                            GameHeapBlock *allocation = EffectObject::AllocateStorage(104);
                            RingEffect *created = 0;
                            if (allocation)
                            {
                                created = new (allocation) RingEffect(&pos, 32, 32, 128, 128, 128, 64, 16, 0);
                            }
                            charge_rings[idx] = created;
                        }
                        if (!charge_rings[idx])
                        {
                            GameDiagnostic(("TheBeast charge ring allocation failed\n"));
                            abort();
                        }
                        charge_rings[idx]->SetPos(&pos);
                        step = (sint16)ai.work[3];
                        charge_rings[idx]->SetRadii(2 * step + 16);
                    }
                    ai.work[3] = counter + 1;
                    return;
                case 2:
                case 3:
                    if (phase == 3 && player_distance < 768)
                    {
                        SetBehaviorVal(0, 0, 0);
                        ClearBoneFacing();
                        RestoreState();
                        return;
                    }
                    step = (sint16)ai.work[3];
                    if (step < (phase == 2 ? 48 : 64))
                    {
                        ai.work[3] = step + 1;
                        return;
                    }
                    val = ReadBeamPoint();
                    ReadRecPos(&pos, val);
                    PosBeamPoint(pos, angles, phase == 2 ? 2 : 0);
                    SetBoneFacingPoint(&pos);
                    phase = ai.phase;
                    ai.work[3] = 0;
                    ai.phase = phase + 1;
                    return;
                case 4:
                    step = (sint16)ai.work[3];
                    if (step < 48)
                    {
                        player = visual_lists.players;
                        distance = model.pos.HorizontalDistance(player->pos);
                        scale = (uint32)((3 * model.scale.x) >> 4);
                        if (distance >= scale)
                        {
                            ai.work[3] = step + 1;
                            return;
                        }
                    }
                    SetBehaviorVal(0, 0, 0);
                    ClearBoneFacing();
                    RestoreState();
                    return;
                default:
                    return;
            }
        case 128:
            phase = ai.phase;
            if (!phase)
            {
                BeginAttack();
                PlayAnimation(4);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if ((sint8)this->model.frame == 15)
                {
                    player = visual_lists.players;
                    distance = model.pos.HorizontalDistance(player->pos);
                    scale = (uint32)((3 * model.scale.x) >> 4);
                    if (distance < scale)
                    {
                        VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                        health = (sint16)GetGameTuning().player.maximum_health;
                        health /= 4;
                        damage_target->Damage(health, &zero_vector, 0);
                    }
                }
                else if (playback.complete)
                {
                    ClearFacing();
                    RestoreState();
                }
            }
            return;
        default:
            return;
    }
}

void TheBeast::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    uint32 grp, lane;
    RecordCursor cursor;
    RecordCursor base;
    FUNCTION_MARKER(0x80044474, "SLUS_003.73");
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    base = script_cursor + 2;
    script_cursor = base;
    for (lane = 0; lane < 4; ++lane)
    {
        val = (base + lane * 2).Read16();
        script_cursor = base + lane * 2 + 2;
        spawn_points.records[lane] = val;
    }
    for (grp = 0; grp < 4; ++grp)
    {
        for (lane = 0; lane < 5; ++lane)
        {
            cursor = script_cursor;
            val = cursor.Read16();
            cursor += 2;
            spawn_points.grps[grp][lane] = val;
            script_cursor = cursor;
        }
    }
    // Consume the unused fifth group and extra record
    for (lane = 0; lane < 5; ++lane)
    {
        cursor = script_cursor;
        cursor.Read16();
        script_cursor = cursor + 2;
    }
    cursor = script_cursor;
    cursor.Read16();
    script_cursor = cursor + 2;
    val = (cursor + 2).Read16();
    script_cursor = cursor + 4;
    spawn_points.beam_point = val;
}

uint32 TheBeast::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 operand;
    sint32 height;
    sint16 offset;
    Vector3 pos;
    RecordCursor cursor;

    FUNCTION_MARKER(0x80044608, "SLUS_003.73");
    switch (command)
    {
        case 0x4220:
        case 0x4223:
        case 0x4222:
        case 0x4225:
            cursor = script_cursor.Aligned(4);
            pos.x = cursor.Read32();
            pos.y = (cursor + 4).Read32();
            pos.z = (cursor + 8).Read32();
            pos.ShiftLeft(12);
            if (command == 0x4222 || command == 0x4225)
                pos.Add(model.pos);
            script_cursor = cursor + 12;
            break;
        case 0x4221:
        case 0x4224:
            cursor = script_cursor;
            operand = cursor.Read16();
            script_cursor = cursor + 2;
            if (operand & 0x2000)
                operand = ResolveParameter(operand);
            ReadRecPos(&pos, operand);
            height = CollisionRay::FindGroundHeight(pos, 512, 2048);
            if (height != -1)
            {
                offset = ground_offset;
                pos.y = (uint32)height - offset * ONE;
            }
            break;
        case 0x4262:
            script_cursor += 2;
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
    if (command == 0x4220 || command == 0x4221 || command == 0x4222)
        SetDst(&pos);
    else
        SetJumpDst(&pos);
    return 1;
}

void TheBeast::SetDst(const Vector3 *pos)
{
    uint8 mode;
    FUNCTION_MARKER(0x8004481C, "SLUS_003.73");
    mode = model.animation;
    if (mode && mode != 3)
        SetAnimationRange(0, 0, -1);
    dst = *pos;
    control.flags = (control.flags | 1) & 0xFFFFFFF3;
}

void TheBeast::SetJumpDst(const Vector3 *pos)
{
    uint32 flags;
    uint16 timer;
    FUNCTION_MARKER(0x80044898, "SLUS_003.73");
    SetAnimationRange(10, 0, -1);
    dst = *pos;
    flags = control.flags;
    timer = ai.parameters[1];
    flags |= 5;
    jump_timer = timer;
    control.flags = flags;
}

void TheBeast::TrackTarget(ModelInstance *target)
{
    uint32 flags;
    FUNCTION_MARKER(0x80044900, "SLUS_003.73");
    if (static_cast<VisualObject *>(target->owner)->IsFinished())
        return;
    flags = control.flags;
    tracked_target.SetObject(target);
    control.flags = flags | 16;
}

void TheBeast::SetFacingPoint(const Vector3 *pos)
{
    FUNCTION_MARKER(0x8004494C, "SLUS_003.73");
    aim_pos = *pos;
    control.flags |= 32;
}

void TheBeast::ClearFacing()
{
    FUNCTION_MARKER(0x80044978, "SLUS_003.73");
    control.flags &= 0xFFFFFFCF;
    motion.angular_velocity.z = 0;
    motion.angular_velocity.y = 0;
    motion.angular_velocity.x = 0;
    motion.angular_acceleration.z = 0;
    motion.angular_acceleration.y = 0;
    motion.angular_acceleration.x = 0;
}

void TheBeast::SetBoneFacingPoint(const Vector3 *pos)
{
    FUNCTION_MARKER(0x800449AC, "SLUS_003.73");
    bone_aim_pos = *pos;
    control.flags |= 64;
}

void TheBeast::ClearBoneFacing()
{
    FUNCTION_MARKER(0x800449D8, "SLUS_003.73");
    control.flags &= 0xFFFFFFBF;
}
