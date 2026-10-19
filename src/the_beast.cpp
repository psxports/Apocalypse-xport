#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_lighting.h"
#include "game_tuning.h"
#include "pickup_factory.h"
#include "pickup.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "effect_object.h"

#include "camera.h"
#include "player.h"

#include "flamethrower.h"
#include "weapon_nodes.h"
#include "cd_voice_event.h"
#include "enemy_list.h"
#include "record_factory.h"
#include "screen_flash.h"
#include "visual_object.h"
#include "angle_math.h"
#include "vector_distance.h"
#include "audio_session.h"
#include "object_damage.h"
#include "collision.h"
#include "model_animation.h"
#include "model_attachment.h"
#include "game_random.h"
#include "game_diagnostics.h"

#include "the_beast.h"
#include "projectile_guided.h"
#include "game_panel.h"
#include "spawn_points.h"
#include "glow_effect.h"
#include "ring_effect.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "effect_lifecycle.h"

// Native extraction of the original bone-vector and normalization call sequence

#include "level_session.h"
#include "fire_cycle.h"

#include "beast_final_visual.h"
#include "beast_effects.h"

// Native extractions of repeated blocks in the original behavior function

#include "record_data.h"
#include "vector_arithmetic.h"

// Original module tables retain shared script and shuffle lifetime
struct BeastSpawnPoints
{
    uint16 records[4];
    uint16 groups[5][5];
    uint16 beam_point;
    uint16 extra_record;
};

static BeastSpawnPoints spawn_points = {0};
static const uint8 voice_choices[3] = {5, 7, 8};

static uint32 DivideBeastMovement(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 ReadBeastPlanarDistance(const uint32 *left, const uint32 *right)
{
    uint32 x = left[0], other_x = right[0], z, other_z;
    x = (uint32)((sint32)(x - other_x) >> 12);
    z = left[2];
    other_z = right[2];
    z = (uint32)((sint32)(z - other_z) >> 12);
    return LookupDistanceRoot(x * x + z * z);
}

static void ReadLightDirection(Character &object, uint32 bone, uint32 horizontal, ShortVector3 &direction)
{
    uint32 lane;
    Vector3 vector, normalized;
    GetModelAttachment(vector.words, object, bone);
    for (lane = 0; lane < 3; ++lane)
        vector.words[lane] -= object.model.pos.words[lane];
    if (horizontal)
        vector.words[1] = 0;
    NormalizeFixedVector(normalized.words, vector.words);
    direction.components[0] = (sint16)normalized.words[0];
    direction.components[1] = horizontal ? 0 : (sint16)normalized.words[1];
    direction.components[2] = (sint16)normalized.words[2];
}

void TheBeast::AdvancePhase()
{
    uint32 phase = ai.phase;
    ai.phase = phase + 1;
}

void TheBeast::SelectState(uint32 state)
{
    ai.state = state;
    ai.phase = 0;
}

void TheBeast::RestoreState()
{
    uint32 state = ai.parameters[2];
    uint32 phase = ai.parameters[3];
    ai.state = state;
    ai.phase = phase;
}

void TheBeast::ClearVelocity()
{
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
}

void TheBeast::BeginAttack()
{
    uint32 flags = control.flags;
    control.flags = flags & 0xFFFFFFFE;
    ClearVelocity();
    TrackTarget(visual_lists.players);
}

void TheBeast::PlayAnimation(uint32 animation)
{
    SetAnimationRange(animation, 0, (uint32)-1);
}

void TheBeast::JumpToPoint(uint32 *position)
{
    uint32 height;
    ClearFacing();
    height = (uint32)(sint32)(sint16)(uint32)ground_offset;
    position[1] -= height << 12;
    SetJumpDestination(position);
}

uint32 TheBeast::ReadBeamPoint()
{
    uint32 index;
    if (ai.parameters[4])
    {
        index = (uint32)(sint32)(sint16)ai.work[2];
        return spawn_points.groups[2][index];
    }
    return spawn_points.beam_point;
}

void TheBeast::PositionBeamPoint(uint32 *position, uint16 *angles, uint32 turn)
{
    uint32 distance, yaw, offset;
    CalculateDirectionAngles(angles, model.pos.words, position);
    if (turn)
    {
        yaw = angles[1];
        offset = ai.parameters[5];
        angles[1] = turn == 1 ? yaw + offset : yaw - offset;
    }
    distance = MeasureEuclideanDistance(model.pos.words, position);
    BuildDirectionVector(position, distance, angles);
    AddVectors(position, model.pos.words);
}

TheBeast::TheBeast(const RecordCursor &record_data, uint32 index) : Enemy(), jump_timer(0)
{
    for (uint32 index = 0; index < 5; ++index)
        used_points[index] = 0;
    RecordDataBorrow record_borrow;
    uint32 roll;
    RecordCursor cursor;
    FUNCTION_MARKER(0x800408A0, "SLUS_003.73");
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;
    bone_aim_pos.words[0] = bone_aim_pos.words[1] = bone_aim_pos.words[2] = 0;

    cursor = ReadPosition(record_data);
    this->model.angles.words[0] = cursor.Read16();
    this->model.angles.words[1] = (cursor + 2).Read16();
    roll = (cursor + 4).Read16();
    script_cursor = cursor + 6;
    this->control.script_active = 1;
    this->record_idx = index;
    this->model.angles.words[2] = roll;
    Configure();
    cursor = script_cursor;
    ExecuteScript(RecordCursor(cursor));
}

void TheBeast::Configure()
{
    FUNCTION_MARKER(0x800409E0, "SLUS_003.73");
    SetModelBank("thebeast");
    motion.drag[0] = 31;
    motion.drag[1] = 31;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    this->model.flags = this->model.flags | 128;
    AllocateBehaviorTable(3, 5);
    SetBehaviorValue(0, 0, 0);
    SetBehaviorValue(1, 0, 0);
    SetBehaviorValue(1, 1, 1);
    SetBehaviorValue(2, 0, 0);
    SetBehaviorValue(2, 1, 2);
    ai.parameters[0] = 32;
    ai.parameters[1] = 32;
    this->kind = 220;
    this->ground_offset = 271;
    beams[0] = 0;
    beams[1] = 0;
    flamethrower = 0;
    ai.parameters[9] = (ai.parameters[9] & 0xFF00) | (uint8)(0);
    ai.state = 0;
    ai.phase = 0;
    this->control.hierarchy_ready = 0;
    camera_subject.SetObject(&model);
    projection.enabled = true;
    memset(&projection.dir, 0, sizeof(projection.dir));
    projection.dir.m[0][1] = -ONE;
    projection.color = 0x808080;
}

TheBeast::~TheBeast()
{
    for (uint32 index = 0; index < 2; ++index)
        if (beams[index])
            beams[index]->Destroy(3);
    if (flamethrower)
        flamethrower->Destroy(3);
    if (held_projectile)
        held_projectile->Destroy(3);
    for (uint32 index = 0; index < 2; ++index)
        if (charge_rings[index])
            charge_rings[index]->Destroy(3);
}

uint32 TheBeast::Destroy(uint32 reason)
{
    uint32 index, result;
    FUNCTION_MARKER(0x80040B64, "SLUS_003.73");
    ClearTrackedObjectHealth();
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    for (index = 0; index < 2; ++index)
    {
        if (beams[index])
            beams[index]->Destroy(3);
        beams[index] = 0;
    }
    if (flamethrower)
        flamethrower->Destroy(3);
    flamethrower = 0;
    if (held_projectile)
        held_projectile->Destroy(3);
    held_projectile = 0;
    projection.enabled = false;
    camera_subject.Reset();
    for (index = 0; index < 2; ++index)
    {
        if (charge_rings[index])
            charge_rings[index]->Destroy(3);
        charge_rings[index] = 0;
    }
    Enemy::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(this);
    return result;
}

uint32 TheBeast::SpawnPickup()
{
    RecordDataBorrow record_borrow;
    uint32 mode, threshold, kind, index, x, z, choice;
    Pickup *child;
    uint16 *entry;
    ModelInstance *player, *candidate;
    Vector3 position;
    sint32 timer, health;
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
    threshold = GetGameTuning().player.maximum_health;
    player = visual_lists.players;
    health = static_cast<VisualObject *>(player->owner)->health;
    kind = 0;
    if (health < ((sint32)(threshold << 16) >> 17) && RandomRange(3) == 0)
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
    if (mode - 5 >= 5)
        return 0;
    entry = spawn_points.groups[3];
    ShuffleSpawnPoints(entry, 5);
    for (index = 0; index < 5; ++index)
    {
        choice = *entry;
        ReadRecordPosition(position.words, choice);
        if (FindGroundHeight(position.words, 0, 2048) != (uint32)-1)
        {
            candidate = visual_lists.pickups;
            if (candidate)
            {
                x = position.words[0];
                z = position.words[2];
                while (candidate)
                {
                    if (candidate->pos.words[0] == x && candidate->pos.words[2] == z)
                        break;
                    candidate = candidate->next;
                }
            }
            if (!candidate)
            {
                child = ::SpawnPickup(kind & 65535, position.words, 7, 600, zero_vector.words);
                if (!child)
                    return 0;
                child->SetMotionParameters(8192, 4);
                x = position.words[1];
                child->ignore_floor = 0;
                child->floor_y = x + 6553600;
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
    RecordDataBorrow record_borrow;
    ModelInstance *player;
    uint32 x;
    uint32 y;
    uint32 z;
    uint32 flags;
    uint32 mode;
    uint32 frame;
    GameHeapBlock *child;
    uint32 timer;
    uint32 value;
    uint32 speed;
    uint32 weapon;
    uint32 distance;
    uint32 current;
    uint32 target;
    Vector3 position, muzzle, player_ground, discarded_direction;
    ShortVector3 desired, weapon_angles;
    uint32 velocity_x, velocity_z, velocity_y;
    sint16 pitch;
    CollisionRay raycast;
    FUNCTION_MARKER(0x80040F34, "SLUS_003.73");
    x = (uint32)zero_vector[0];
    y = (uint32)zero_vector[1];
    z = (uint32)zero_vector[2];
    motion.acceleration.words[0] = x;
    motion.acceleration.words[1] = y;
    motion.acceleration.words[2] = z;
    UpdateModelAnimationData();
    UpdateWorldTransform();
    BuildModelPose(*this);
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1)
    {
        value = (uint32)this->control.activation_count;
        this->activation_flags = flags & 65534;
        this->control.activation_count = value + 1;
    }
    mode = this->model.animation;
    if (mode == 3)
    {
        frame = DidAnimationCrossFrame(3, 5);
        if (UpdateEventLatch(frame, 1))
        {
            PlayPositionedSound(137, model.pos.words, 0);
            GetModelAttachment(position.words, *this, 6);
            position.words[1] += 131072;
            child = AllocateEffectStorage(112);
            if (child)
            {
                new (child) BeastGroundEffect(position.words, 10, 64, 96, 96, 20, 64, 64);
            }
        }
        frame = DidAnimationCrossFrame(3, 31);
        if (UpdateEventLatch(frame, 2))
        {
            PlayPositionedSound(137, model.pos.words, 0);
            GetModelAttachment(position.words, *this, 5);
            position.words[1] += 131072;
            child = AllocateEffectStorage(112);
            if (child)
            {
                new (child) BeastGroundEffect(position.words, 10, 64, 96, 96, 20, 64, 64);
            }
        }
    }
    else if (mode == 4)
    {
        frame = DidAnimationCrossFrame(4, 15);
        if (UpdateEventLatch(frame, 1))
        {
            GetModelAttachment(position.words, *this, 5);
            position.words[1] += 131072;
            child = AllocateEffectStorage(112);
            if (child)
            {
                new (child) BeastGroundEffect(position.words, 10, 64, 96, 96, 20, 64, 128);
            }
            PlayPositionedSound(137, position.words, 0);
            static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(1);
        }
    }
    if ((uint32)this->control.script_active)
    {
        timer = (uint32)this->control.script_delay;
        if (timer)
            this->control.script_delay = timer - 1;
        else
        {
            ExecuteScript(script_cursor);
        }
    }
    else
        UpdateBehavior();
    if (!(uint32)this->control.hierarchy_ready)
    {
        SetModelBoneTable(this->model.bank, TheBeastAttachments);
        InitializeModelAnimation(TheBeastHierarchy);
        flags = this->model.flags;
        this->control.hierarchy_ready = 1;
        this->model.flags = flags | 4;
    }
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    x = this->model.pos.words[0];
    raycast.endpoints[0].words[0] = x;
    y = this->model.pos.words[1] - 262144;
    raycast.endpoints[0].words[1] = y;
    z = this->model.pos.words[2];
    raycast.endpoints[1].words[0] = x;
    raycast.endpoints[1].words[1] = y + 8388608;
    raycast.endpoints[0].words[2] = z;
    raycast.endpoints[1].words[2] = z;
    InitializeCollisionRay(raycast);
    QueryCollisionRay(raycast, 1);
    if (raycast.hit.GetObject())
    {
        projection.enabled = true;
        surface.pos.words[0] = raycast.hit_pos.words[0];
        surface.pos.words[1] = raycast.hit_pos.words[1];
        surface.pos.words[2] = raycast.hit_pos.words[2];
        surface.n.vx = (uint16)raycast.n.vx;
        surface.n.vy = (uint16)raycast.n.vy;
        surface.n.vz = (uint16)raycast.n.vz;
    }
    else
        projection.enabled = false;
    flags = this->control.flags;
    if (flags & 16)
    {
        ModelInstance *subject = tracked_target.GetObject();
        if (subject)
        {
            aim_pos.words[0] = subject->pos.words[0];
            aim_pos.words[1] = subject->pos.words[1];
            aim_pos.words[2] = subject->pos.words[2];
        }
        else
            control.flags &= ~16;
        CalculateDirectionAngles(desired.words, model.pos.words, this->aim_pos.words);
        desired.words[0] = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 24);
    }
    else if (flags & 32)
    {
        CalculateDirectionAngles(desired.words, model.pos.words, this->aim_pos.words);
        desired.words[0] = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 16);
    }
    flags = this->control.flags;
    if (flags & 64)
    {
        CalculateDirectionAngles(desired.words, model.pos.words, this->bone_aim_pos.words);
        value = desired.words[1];
        desired.words[0] = 0;
        current = this->model.angles.words[1];
        desired.words[1] = (value - current) & 4095;
    }
    else
    {
        desired.words[0] = 0;
        desired.words[1] = 0;
        desired.words[2] = 0;
    }
    UpdateAngleAcceleration(aim.angles, aim.velocity, aim.acceleration, desired, 16);
    AddAngleVector(aim.angles.words, aim.velocity.words);
    NormalizeAngles(aim.angles.words);
    AddAngleVector(aim.velocity.words, aim.acceleration.words);
    DampAngles(aim.velocity.words, motion.angular_drag);
    SnapSmallAngles(aim.velocity.words);
    GetModelJointAngles(17).rotation.words[1] = aim.angles.words[1];
    flags = this->control.flags;
    if (!(flags & 1))
        goto StoppedAnimation;
    if (flags & 4)
    {
        frame = (uint32)(sint32)(sint8)this->model.frame;
        if ((sint32)frame < 18)
        {
            this->playback.speed = 65536;
            this->control.gravity_enabled = 0;
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            motion.velocity.words[0] = 0;
            motion.acceleration.words[2] = 0;
            motion.acceleration.words[1] = 0;
            motion.acceleration.words[0] = 0;
        }
        else if ((uint32)jump_timer)
            this->control.gravity_enabled = 1;
        frame = (uint32)(sint32)(sint8)this->model.frame;
        if ((sint32)frame >= 18 && !(this->control.flags & 8))
        {
            x = this->destination.words[0];
            y = this->model.pos.words[0];
            timer = (uint32)jump_timer;
            velocity_x = DivideBeastMovement(x - y, timer);
            x = this->destination.words[2];
            y = this->model.pos.words[2];
            timer = (uint32)jump_timer;
            velocity_z = DivideBeastMovement(x - y, timer);
            x = this->destination.words[1];
            y = this->model.pos.words[1];
            timer = (uint32)jump_timer;
            velocity_y = DivideBeastMovement(x - y, timer);
            flags = this->control.flags;
            this->control.flags = flags | 8;
            motion.velocity.words[0] = velocity_x;
            motion.velocity.words[2] = velocity_z;
            motion.velocity.words[1] = velocity_y - (timer << 14);
            PlayPositionedSound(139, model.pos.words, 0);
        }
        else
        {
            frame = (uint32)(sint32)(sint8)this->model.frame;
            if ((sint32)frame >= 38)
            {
                if (!(uint32)jump_timer)
                    goto JumpFinished;
                this->playback.speed = 0;
            }
        }
        timer = (uint32)jump_timer;
        if (timer)
        {
            timer = (timer - 1) & 65535;
            jump_timer = timer;
            if (!timer)
            {
                x = this->destination.words[0];
                y = this->destination.words[1];
                z = this->destination.words[2];
                this->model.pos.words[0] = x;
                this->model.pos.words[1] = y;
                this->model.pos.words[2] = z;
                position = this->model.pos;
                value = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
                position.words[1] += (value - 32) << 12;
                if (ai.state != 4)
                {
                    child = AllocateEffectStorage(112);
                    if (child)
                    {
                        new (child) BeastGroundEffect(position.words, 10, 64, 96, 96, 20, 64, 128);
                    }
                }
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
                motion.acceleration.words[2] = 0;
                motion.acceleration.words[1] = 0;
                this->control.gravity_enabled = 0;
                Camera *impact_camera = static_cast<Camera *>(visual_lists.cameras->owner);
                motion.acceleration.words[0] = 0;
                this->playback.speed = 65536;
                impact_camera->ApplyExplosionShake(0);
                PlayPositionedSound(138, model.pos.words, 0);
            }
        }
    JumpFinished:
        if (this->playback.complete)
        {
            flags = this->control.flags;
            this->control.flags = flags & 0xFFFFFFF2;
        }
    }
    else
    {
        distance = MeasurePositionDistance(this->model.pos.words, this->destination.words);
        speed = distance ? 32768 / distance : (uint32)-1;
        if ((sint32)speed >= 129)
            speed = 128;
        CalculateDirectionAngles(desired.words, this->model.pos.words, this->destination.words);
        desired.words[0] = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, speed);
        speed = ai.parameters[0];
        BuildDirectionVector(motion.velocity.words, speed, this->model.angles.words);
        flags = this->control.flags;
        if (flags & 2)
        {
            position.words[0] = this->model.pos.words[0];
            position.words[1] = this->destination.words[1];
            position.words[2] = this->model.pos.words[2];
            distance = MeasureEuclideanDistance(position.words, this->destination.words);
            value = distance < 64;
        }
        else
        {
            distance = MeasureEuclideanDistance(this->model.pos.words, this->destination.words);
            value = distance < 128;
        }
        if (value)
        {
            flags = this->control.flags;
            this->control.flags = flags & 0xFFFFFFFE;
        }
    }
    if (this->control.flags & 1)
    {
        if (!this->model.animation && this->playback.complete)
            StartAnimation(3, 1);
        mode = this->model.animation;
        goto AttackAnimation;
    }
StoppedAnimation:
    mode = this->model.animation;
    if (mode == 3)
    {
        frame = DidAnimationCrossFrame(3, 43);
        if (UpdateEventLatch(frame, 1))
            SetAnimationRange(8, 0, (uint32)-1);
        frame = DidAnimationCrossFrame(3, 17);
        if (UpdateEventLatch(frame, 2))
            SetAnimationRange(9, 0, (uint32)-1);
        mode = this->model.animation;
    }
AttackAnimation:
    if (mode == 2)
    {
        frame = this->model.frame;
        if (frame - 15 < 6)
        {
            player = visual_lists.players;
            if (player)
            {
                GetModelAttachment(position.words, *this, 0);
                player = visual_lists.players;
                SubtractVectors(discarded_direction.words, position.words, player->pos.words);
                MeasureVectorLength(discarded_direction.words);
            }
        }
    }
    if (GetBehaviorValue(0, 0) & 65535)
    {
        for (uint32 index = 0; index < 2; ++index)
        {
            if (!beams[index])
            {
                GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
                ParticleBeam *created = 0;
                if (allocation)
                {
                    created = new (allocation) ParticleBeam(&visual_lists.players, 4096, 0, 128, 134);
                }
                beams[index] = created;
                value = GetBehaviorValue(2, 1) & 65535;
                if (!beams[index])
                {
                    GameDiagnostic("TheBeast particle beam allocation failed\n");
                    abort();
                }
                beams[index]->damage = value;
                beams[index]->SetColors(255, 64, 0, 185, 185, 185, 160, 40, 0, 255, 255, 255);
            }
        }
        if (beams[0] || beams[1])
        {
            GetModelAttachment(position.words, *this, 3);
            CalculateDirectionAngles(weapon_angles.words, position.words, visual_lists.players->pos.words);
            player_ground.words[0] = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words[0];
            player_ground.words[1] = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words[1];
            player_ground.words[2] = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words[2];
            distance = ReadBeastPlanarDistance(player_ground.words, position.words);
            if ((sint32)distance < 768)
                distance = 768;
            pitch = (sint16)ratan2((sint32)(player_ground.words[1] - position.words[1]) >> 12, (sint32)distance);
            GetModelAttachment(muzzle.words, *this, 1);
            CalculateDirectionAngles(weapon_angles.words, position.words, muzzle.words);
            weapon_angles.words[0] = pitch;
            if (beams[0])
                beams[0]->Fire(muzzle.words, weapon_angles.components);
            if (beams[1])
            {
                GetModelAttachment(muzzle.words, *this, 2);
                CalculateDirectionAngles(weapon_angles.words, position.words, muzzle.words);
                weapon_angles.words[0] = pitch;
                beams[1]->Fire(muzzle.words, weapon_angles.components);
            }
        }
    }
    else
    {
        for (uint32 index = 0; index < 2; ++index)
        {
            if (beams[index])
                beams[index]->Destroy(3);
            beams[index] = 0;
        }
    }
    if (GetBehaviorValue(1, 0) & 65535)
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
            value = GetBehaviorValue(1, 1) & 65535;
            if (!flamethrower)
            {
                GameDiagnostic("TheBeast flamethrower allocation failed\n");
                abort();
            }
            flamethrower->damage = value;
            flamethrower->Configure(2000);
        }
        if (flamethrower)
        {
            GetModelAttachment(muzzle.words, *this, 3);
            mode = model.animation;
            if (mode == 6)
            {
                CalculateDirectionAngles(weapon_angles.words, muzzle.words, visual_lists.players->pos.words);
                pitch = weapon_angles.words[0];
                GetModelAttachment(position.words, *this, 4);
                CalculateDirectionAngles(weapon_angles.words, muzzle.words, position.words);
                weapon_angles.words[0] = pitch;
            }
            else
            {
                GetModelAttachment(position.words, *this, 4);
                CalculateDirectionAngles(weapon_angles.words, muzzle.words, position.words);
            }
            flamethrower->Fire(position.words, weapon_angles.components);
        }
    }
    else if (flamethrower)
    {
        flamethrower->Destroy(3);
        flamethrower = 0;
    }
    mode = this->model.animation;
    if (mode == 7)
    {
        frame = DidAnimationCrossFrame(7, 25);
        if (UpdateEventLatch(frame, 1))
        {
            GetModelAttachment(position.words, *this, 0);
            GameHeapBlock *allocation = AllocateZeroedObject(428);
            GuidedProjectile *created = 0;
            if (allocation)
            {
                created = new (allocation) GuidedProjectile(position.words, 10, &visual_lists.players, 1);
            }
            held_projectile = created;
            if (!held_projectile)
            {
                GameDiagnostic("TheBeast held projectile allocation failed\n");
                abort();
            }
            held_projectile->lifecycle_flags |= 32;
            held_projectile->SetLaunchDelay(0);
        }
        frame = DidAnimationCrossFrame(7, 68);
        if (UpdateEventLatch(frame, 2))
        {
            if (held_projectile)
            {
                GetModelAttachment(muzzle.words, *this, 0);
                CalculateDirectionAngles(weapon_angles.words, muzzle.words, visual_lists.players->pos.words);
                weapon_angles.words[1] = model.angles.words[1];
                held_projectile->SetAngles(weapon_angles.words);
                held_projectile->Launch(0, static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words);
                held_projectile->lifecycle_flags &= 65503;
            }
            held_projectile = 0;
        }
        if (held_projectile)
        {
            GetModelAttachment(position.words, *this, 0);
            held_projectile->model.pos.words[0] = position.words[0];
            held_projectile->model.pos.words[1] = position.words[1];
            held_projectile->model.pos.words[2] = position.words[2];
        }
    }
    return SpawnPickup();
}

uint32 TheBeast::Resize(uint32 increment)
{
    uint32 flags, scale, product, height, old_height, displacement, y;
    FUNCTION_MARKER(0x80041FDC, "SLUS_003.73");
    flags = this->model.flags;
    scale = (uint32)(sint32)(sint16)this->model.scale.words[0];
    scale += increment;
    this->model.flags = flags | 512;
    product = ((scale << 4) + scale) << 4;
    product -= scale;
    this->model.scale.words[2] = scale;
    this->model.scale.words[1] = scale;
    this->model.scale.words[0] = scale;
    old_height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
    height = (uint32)((sint32)product >> 12);
    this->ground_offset = height;
    height = (uint32)(sint32)(sint16)height;
    displacement = (height - old_height) << 12;
    y = this->model.pos.words[1];
    this->model.pos.words[1] = y - displacement;
    return displacement;
}

uint32 TheBeast::UpdateLighting()
{
    uint32 random, active = 0;
    uint8 red, green, blue;
    sint16 flicker[3], color[3];
    ShortVector3 direction;
    FUNCTION_MARKER(0x80042038, "SLUS_003.73");
    random = RandomRange(256);
    flicker[0] = (sint16)(random << 4);
    random = RandomRange((uint32)((sint32)random >> 1));
    flicker[1] = (sint16)(random << 4);
    flicker[2] = 0;
    if (beams[0])
    {
        if (beams[0]->ReadLight(&red, &green, &blue))
        {
            ReadLightDirection(*this, 1, 1, direction);
            color[0] = (sint16)(red << 4);
            color[1] = (sint16)(green << 4);
            color[2] = (sint16)(blue << 4);
            active = 1;
        }
    }
    else
    {
        if (flamethrower)
        {
            if (flamethrower->ReadLight(&red, &green, &blue))
            {
                ReadLightDirection(*this, 4, 0, direction);
                color[0] = (sint16)(red << 4);
                color[1] = (sint16)(green << 4);
                color[2] = (sint16)(blue << 4);
                active = 1;
            }
        }
        else if (held_projectile)
        {
            ReadLightDirection(*this, 0, 0, direction);
            color[0] = 2560;
            color[1] = 1024;
            color[2] = 0;
            active = 1;
        }
    }
    if (!active)
    {
        direction.components[0] = 0;
        direction.components[1] = 0;
        direction.components[2] = 0;
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }
    static const ShortVector3 dir0 = {{{0, -ONE, 0}}};
    static const ShortVector3 dir1 = {{{0, ONE, 0}}};
    static const sint16 base_color[3] = {2048, 2048, 2048};
    SetLightingDirections(dir0.components, dir1.components, direction.components);
    SetLightingColors(base_color, flicker, color);
    game_lighting.background_color.b = 64;
    game_lighting.background_color.g = 64;
    game_lighting.background_color.r = 64;
    return 64;
}

uint32 TheBeast::Damage(uint32 damage, const sint32 *, uint32 reason)
{
    uint32 pulse, health, applied;
    sint32 alive;
    FUNCTION_MARKER(0x800422C0, "SLUS_003.73");
    pulse = (uint32)this->control.damage_phase;
    this->control.damage_phase = pulse + 1;
    alive = this->health;
    health = (uint16)this->health;
    if (alive <= 0)
        return 0;
    if (reason == 5)
        damage = (uint32)((sint32)damage >> 1);
    applied = (uint32)((sint32)damage >> 1);
    if (reason == 10)
    {
        damage = (uint32)((sint32)damage >> 2);
        applied = (uint32)((sint32)damage >> 1);
    }
    health -= applied;
    this->health = (sint16)(uint16)(health);
    if ((sint32)(health << 16) <= 0)
        ClearTrackedObjectHealth();
    SetColor(3, 255, 255, 255);
    return 1;
}

void TheBeast::UpdateBehavior()
{
    ModelBorrow model_borrow;
    RecordDataBorrow record_borrow;
    uint32 state;
    uint32 phase;
    uint32 value;
    uint32 floor;
    uint32 flags;
    GameHeapBlock *child;
    uint32 index;
    uint32 best;
    uint32 selected;
    uint32 chosen;
    uint32 distance;
    uint32 scale;
    uint32 counter;
    uint32 yaw;
    Camera *camera;
    ModelInstance *player;
    const uint16 *entry;
    Vector3 position, destination, delta, normalized, scaled, sample;
    uint32 shift, scalar;
    ShortVector3 angles;
    uint8 voices[3];
    sint32 health, step, old_counter;
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
                    floor = FindGroundHeight(model.pos.words, 256, 1024);
                    if (floor != (uint32)-1)
                    {
                        value = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
                        this->model.pos.words[1] = floor - (value << 12);
                    }
                    player = visual_lists.players;
                    static_cast<Player *>(visual_lists.players->owner)->disable_timer = 150;
                    this->health = (sint16)(uint16)(300);
                    ::TrackObjectHealth(*this, 24, 42, 4096);
                    AdvancePhase();
                    return;
                case 1:
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(&model);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(&model);
                    SetCameraOrbitDistanceTransition(1100, 0);
                    SetCameraOrbitHeightTransition((uint32)-4096, 0);
                    SetCameraTargetLateralOffsetTransition(0, 0);
                    SetCameraTargetHeightOffsetTransition(3402, 0);
                    SetCameraTargetForwardOffsetTransition((uint32)-916, 0);
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
                        SetCameraOrbitHeightTransition((uint32)-206, 30);
                        SetCameraTargetHeightOffsetTransition(90, 30);
                        SetCameraTargetForwardOffsetTransition(604, 30);
                        phase = ai.phase;
                        ai.work[0] = 30;
                        ai.phase = phase + 1;
                    }
                    else
                    {
                        voices[0] = voice_choices[0];
                        voices[1] = voice_choices[1];
                        voices[2] = voice_choices[2];
                        index = RandomRange(3);
                        ::SelectCdVoice(19, voices[index]);
                        PlayAnimation(6);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (!this->playback.complete)
                        return;
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    player = visual_lists.players;
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(visual_lists.players);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(visual_lists.players);
                    SetCameraOrbitDistanceTransition(1100, 30);
                    SetCameraOrbitHeightTransition((uint32)-308, 30);
                    SetCameraTargetLateralOffsetTransition(0, 30);
                    SetCameraTargetHeightOffsetTransition(68, 30);
                    SetCameraTargetForwardOffsetTransition(216, 30);
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
                index = spawn_points.records[0];
                ::SpawnRecord(index);
                PlayPositionedSound(132, model.pos.words, 0);
                PlayAnimation(5);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if (this->playback.complete)
                {
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    camera->ApplyExplosionShake(1);
                    index = spawn_points.records[1];
                    ::SpawnRecord(index);
                    index = spawn_points.records[2];
                    ::SpawnRecord(index);
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
                SetJumpDestination(this->model.pos.words);
                AdvancePhase();
            }
            else if (phase == 1 && !(this->control.flags & 1))
            {
                ::SetScreenFlash(255, 255, 255, 8, 0, 0);
                SelectState(5);
                ai.parameters[1] = 48;
            }
            return;
        case 5:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    this->health = (sint16)(uint16)(200);
                    ::TrackObjectHealth(*this, 24, 42, 4096);
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
                    if (ChoosePosition(destination.words, visual_lists.players->pos.words, 512, 1024, 128, 1024, 1, 0))
                    {
                        ClearFacing();
                        SetDestination(destination.words);
                        AdvancePhase();
                    }
                    else
                        ClearVelocity();
                    return;
                case 2:
                    if (!(this->control.flags & 1) || !RandomRange(32))
                        ai.phase = 1;
                    return;
                case 3:
                    index = spawn_points.records[3];
                    ReadRecordPosition(position.words, index);
                    floor = FindGroundHeight(position.words, 2048, 2048);
                    position.words[1] = floor;
                    if (floor != (uint32)-1)
                    {
                        JumpToPoint(position.words);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (!(this->control.flags & 1))
                    {
                        PlayPositionedSound(132, model.pos.words, 0);
                        PlayAnimation(5);
                        AdvancePhase();
                    }
                    return;
                case 5:
                    if (!this->playback.complete)
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
                    this->health = (sint16)(uint16)(600);
                    ::TrackObjectHealth(*this, 24, 42, 4096);
                    phase = ai.phase;
                    ai.work[0] = 0;
                    ai.phase = phase + 1;
                    return;
                case 1:
                    health = this->health;
                    step = (sint16)ai.work[0];
                    if (health <= 0 && step >= 5)
                    {
                        index = spawn_points.records[3];
                        ReadRecordPosition(position.words, index);
                        floor = FindGroundHeight(position.words, 2048, 2048);
                        position.words[1] = floor;
                        if (floor != (uint32)-1)
                        {
                            JumpToPoint(position.words);
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
                    if (!ChoosePosition(position.words, visual_lists.players->pos.words, 512, 1024, 128, 1024, 1, 0))
                    {
                        ClearVelocity();
                        ai.phase = 3;
                        return;
                    }
                    SubtractVectors(delta.words, position.words, this->model.pos.words);
                    delta.words[1] = 0;
                    scalar = 0;
                    while (scalar < 8)
                    {
                        shift = 3;
                        ShiftVectorRight(normalized.words, delta.words, shift);
                        MultiplyVector(scaled.words, normalized.words, scalar);
                        AddVectors(sample.words, model.pos.words, scaled.words);
                        raycast.endpoints[0].words[1] = sample.words[1];
                        raycast.endpoints[0].words[1] = sample.words[1] - 2097152;
                        raycast.endpoints[1].words[1] = sample.words[1];
                        raycast.endpoints[0].words[0] = sample.words[0];
                        raycast.endpoints[0].words[2] = sample.words[2];
                        raycast.endpoints[1].words[0] = sample.words[0];
                        raycast.endpoints[1].words[2] = sample.words[2];
                        raycast.endpoints[1].words[1] = sample.words[1] + 8388608;
                        InitializeCollisionRay(raycast);
                        raycast.triggers = 0;
                        QueryCollisionRay(raycast, 1);
                        if (!raycast.hit.GetObject())
                        {
                            ClearVelocity();
                            ai.phase = 3;
                            return;
                        }
                        ++scalar;
                    }
                    ClearFacing();
                    SetDestination(position.words);
                    AdvancePhase();
                    return;
                case 2:
                case 4:
                    step = (sint16)ai.work[0];
                    if (step >= 5 || this->health >= 600 - 90 * (step + 1))
                    {
                        if (phase == 4)
                            ai.phase = 1;
                        else if (!(this->control.flags & 1) || !RandomRange(32))
                        {
                            phase = ai.phase;
                            ai.phase = phase - 1;
                        }
                        return;
                    }
                    index = spawn_points.groups[0][step];
                    ReadRecordPosition(position.words, index);
                    floor = FindGroundHeight(position.words, 2048, 2048);
                    position.words[1] = floor;
                    if (floor == (uint32)-1)
                        position.words[1] = surface.pos.words[1];
                    ClearFacing();
                    value = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
                    position.words[1] -= value << 12;
                    if (phase == 2)
                        ClearVelocity();
                    SetJumpDestination(position.words);
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
                    if (!(this->control.flags & 1))
                    {
                        PlayAnimation(4);
                        AdvancePhase();
                    }
                    return;
                case 6:
                    if (!this->playback.complete)
                    {
                        if ((sint8)this->model.frame == 15)
                        {
                            step = (sint16)ai.work[0];
                            player = visual_lists.solids;
                            index = spawn_points.groups[0][step];
                            ::ActivateObjectsByIndex(player, index);
                            step = (sint16)ai.work[0];
                            ai.work[0] = step + 1;
                        }
                        return;
                    }
                    index = spawn_points.records[3];
                    ReadRecordPosition(position.words, index);
                    floor = FindGroundHeight(position.words, 2048, 2048);
                    position.words[1] = floor;
                    if (floor != (uint32)-1)
                    {
                        JumpToPoint(position.words);
                        AdvancePhase();
                    }
                    return;
                case 7:
                    if (!(this->control.flags & 1))
                        ai.phase = 1;
                    return;
                case 8:
                    if (!(this->control.flags & 1))
                    {
                        PlayPositionedSound(132, model.pos.words, 0);
                        PlayAnimation(5);
                        AdvancePhase();
                    }
                    return;
                case 9:
                    if (!this->playback.complete)
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
                this->health = (sint16)(uint16)(600);
                ::TrackObjectHealth(*this, 24, 42, 4096);
                PlayAnimation(4);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if (this->playback.complete)
                    ai.phase = 2;
                else if ((sint8)this->model.frame == 15)
                {
                    entry = spawn_points.groups[1];
                    for (index = 0; index < 5; ++index)
                    {
                        value = *entry;
                        ++entry;
                        player = visual_lists.solids;
                        ::ActivateObjectsByIndex(player, value);
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
                        for (index = 0; index < 5; ++index)
                            if (!used_points[index])
                            {
                                ai.work[2] = index;
                                break;
                            }
                        SelectState(9);
                        return;
                    }
                    player = visual_lists.players;
                    distance = MeasureHorizontalDistance(model.pos.words, player->pos.words);
                    scale = (uint32)((3 * (sint16)this->model.scale.words[0]) >> 4);
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
                    selected = 0;
                    entry = spawn_points.groups[2];
                    for (index = 0; index < 5; ++index)
                    {
                        if (!used_points[index])
                        {
                            value = *entry;
                            ReadRecordPosition(position.words, value);
                            player = visual_lists.players;
                            distance = MeasureEuclideanDistance(position.words, player->pos.words);
                            if (distance < best)
                            {
                                selected = index;
                                best = distance;
                                selection_valid = true;
                            }
                        }
                        ++entry;
                    }
                    if (!selection_valid)
                    {
                        // TODO Original reads an undefined saved register when no candidate wins
                        GameDiagnostic("TODO TheBeast AI state8 has no selected point\n");
                        abort();
                    }
                    used_points[selected] = 1;
                    value = spawn_points.groups[2][selected];
                    ReadRecordPosition(position.words, value);
                    floor = FindGroundHeight(position.words, 2048, 2048);
                    position.words[1] = floor;
                    if (floor == (uint32)-1)
                        ai.phase = 1;
                    else
                    {
                        ai.work[2] = selected;
                        JumpToPoint(position.words);
                        AdvancePhase();
                    }
                    return;
                case 3:
                    if (!(this->control.flags & 1))
                    {
                        PlayAnimation(4);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (!this->playback.complete)
                    {
                        if ((sint8)this->model.frame == 15)
                        {
                            step = (sint16)ai.work[0];
                            ai.work[0] = step + 1;
                            chosen = (uint32)(sint32)(sint16)ai.work[2];
                            player = visual_lists.solids;
                            value = spawn_points.groups[2][chosen];
                            ::ActivateObjectsByIndex(player, value);
                        }
                        return;
                    }
                    value = spawn_points.records[3];
                    ReadRecordPosition(position.words, value);
                    floor = FindGroundHeight(position.words, 2048, 2048);
                    position.words[1] = floor;
                    if (floor != (uint32)-1)
                    {
                        JumpToPoint(position.words);
                        AdvancePhase();
                    }
                    return;
                case 5:
                    if (!(this->control.flags & 1))
                        ai.phase = 1;
                    return;
                default:
                    return;
            }
        case 9:
            phase = ai.phase;
            if (!phase)
            {
                this->health = (sint16)(uint16)(400);
                SetBehaviorValue(2, 1, 4);
                ::TrackObjectHealth(*this, 24, 42, 4096);
                value = spawn_points.records[3];
                ReadRecordPosition(position.words, value);
                chosen = (uint32)(sint32)(sint16)ai.work[2];
                value = spawn_points.groups[2][chosen];
                ReadRecordPosition(delta.words, value);
                SubtractVectors(delta.words, delta.words, position.words);
                delta.words[1] = 0;
                NormalizeFixedVector(normalized.words, delta.words);
                scalar = 512;
                MultiplyVector(scaled.words, normalized.words, scalar);
                AddVectors(position.words, scaled.words);
                floor = FindGroundHeight(position.words, 2048, 2048);
                position.words[1] = floor;
                if (floor != (uint32)-1)
                {
                    ClearFacing();
                    value = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
                    position.words[1] -= value << 12;
                    SetDestination(position.words);
                    AdvancePhase();
                }
            }
            else if (phase == 1)
            {
                if (!(this->control.flags & 1))
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
                distance = MeasureHorizontalDistance(model.pos.words, player->pos.words);
                scale = (uint32)((3 * (sint16)this->model.scale.words[0]) >> 4);
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
                chosen = (uint32)(sint32)(sint16)ai.work[2];
                value = spawn_points.groups[2][chosen];
                ReadRecordPosition(position.words, value);
                player = visual_lists.players;
                chosen = 0;
                if (ReadBeastPlanarDistance(position.words, player->pos.words) < 1024)
                    chosen = RandomRange(3) != 0;
                if (chosen)
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
            if (phase - 1 < 3)
            {
                if (!RandomRange(60))
                    PlayPositionedSound(132, model.pos.words, 3);
                value = (uint32)(sint32)(sint16)ai.work[2];
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                yaw = (value - 16) & 4095;
                ai.work[2] = yaw;
                camera->SetYawTransition(yaw, 4);
            }
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    player = visual_lists.players;
                    static_cast<Player *>(visual_lists.players->owner)->disable_timer = 900;
                    PlayPositionedSound(132, model.pos.words, 0);
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
                        SetCameraOrbitDistanceTransition(1324, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        SetCameraOrbitHeightTransition((uint32)-854, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        SetCameraTargetLateralOffsetTransition(0, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        SetCameraTargetHeightOffsetTransition((uint32)-144, 16);
                        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                        SetCameraTargetForwardOffsetTransition((uint32)-472, 16);
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
                            child = AllocateEffectStorage(92);
                            if (child)
                            {
                                new (child) BeastOrbitEffect(this->model.pos.words);
                            }
                            use_small_effect_pool = true;
                        }
                        return;
                    }
                    child = AllocateEffectStorage(124);
                    if (child)
                    {
                        new (child) ExpandingGlow(model.pos.words, 255, 128, 0, 512, 32);
                    }
                    PlayPositionedSound(1, model.pos.words, 0);
                    AdvancePhase();
                    return;
                case 2:
                    flags = this->model.flags;
                    this->model.flags = flags & 65407;
                    SetColor(0, 255, 0, 0);
                    model_effect.color_mode = 0x02404040;
                    model_effect.scale = 10240;
                    phase = ai.phase;
                    model_effect.depth_bias = 10;
                    ai.phase = phase + 1;
                    return;
                case 3:
                    if ((sint16)this->model.scale.words[0] >= 2049)
                    {
                        Resize((uint32)-16);
                        return;
                    }
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    old_counter = (sint16)camera->angles.words[1] - 2048;
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
                    SetCameraOrbitDistanceTransition(1324, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    SetCameraOrbitHeightTransition((uint32)-316, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    SetCameraTargetLateralOffsetTransition(0, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    SetCameraTargetHeightOffsetTransition(220, 16);
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    SetCameraTargetForwardOffsetTransition((uint32)-328, 16);
                    AdvancePhase();
                    return;
                case 5:
                    old_counter = (sint16)ai.work[0];
                    counter = (uint16)old_counter;
                    ai.work[0] = old_counter + 1;
                    if (counter == 16)
                    {
                        flags = this->model.flags;
                        model_effect.color_mode = 0;
                        this->model.flags = flags | 1;
                        ::SetScreenFlash(240, 240, 240, 32, 0, 0);
                        position = this->model.pos;
                        value = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
                        position.words[1] += value << 12;
                        child = AllocateZeroedObject(296);
                        if (child)
                        {
                            new (child) BeastFinalVisual(position.words);
                        }
                        PlayPositionedSound(1, model.pos.words, 0);
                        position.words[2] -= 524288;
                        position.words[0] += 737280;
                        SpawnFireCycle(position.words, 220, 0, 2);
                        position.words[0] -= 1474560;
                        SpawnFireCycle(position.words, 220, 0, 2);
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
                        child = AllocateEffectStorage(124);
                        if (child)
                        {
                            new (child) ExpandingGlow(model.pos.words, 255, 128, 0, 512, 32);
                        }
                    }
                    if (counter == 190)
                        ::FadeScreenToWhite(30, 1);
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
                if (this->playback.loop_mode == 1)
                {
                    PlayAnimation(state == 64 ? 7 : state == 80 ? 2 : 6);
                    if (state == 80)
                        PlayPositionedSound(135, model.pos.words, 0);
                    phase = ai.phase;
                    ai.phase = phase + 2;
                }
                else
                    AdvancePhase();
            }
            else if (phase == 1)
            {
                if (this->playback.complete)
                {
                    PlayAnimation(state == 64 ? 7 : state == 80 ? 2 : 6);
                    if (state == 80)
                        PlayPositionedSound(135, model.pos.words, 0);
                    AdvancePhase();
                }
            }
            else if (phase == 2)
            {
                if (state == 64)
                {
                    value = this->model.frame;
                    SetBehaviorValue(1, 0, value - 18 < 22);
                }
                else if (state == 96 && (sint8)this->model.frame >= 28)
                    SetBehaviorValue(1, 0, 1);
                player = visual_lists.players;
                if (!player || this->playback.complete)
                {
                    if (state == 80)
                        ai.phase = 3;
                    else
                        AdvancePhase();
                }
                else if (state == 80)
                {
                    value = this->model.frame;
                    if (value - 13 < 9)
                    {
                        distance = MeasureHorizontalDistance(model.pos.words, player->pos.words);
                        scale = (uint32)((3 * (sint16)this->model.scale.words[0]) >> 4);
                        if (distance < scale)
                        {
                            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                            damage_target->Damage(2, zero_vector.components, 0);
                        }
                    }
                }
            }
            else if (phase == 3)
            {
                if (state == 96)
                    SetBehaviorValue(1, 0, 0);
                ClearFacing();
                RestoreState();
            }
            return;
        case 112:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    flags = this->control.flags;
                    this->control.flags = flags & 0xFFFFFFFE;
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    chosen = ai.parameters[4];
                    motion.velocity.words[0] = 0;
                    if (chosen)
                    {
                        chosen = (uint32)(sint32)(sint16)ai.work[2];
                        value = spawn_points.groups[2][chosen];
                    }
                    else
                        value = spawn_points.beam_point;
                    ReadRecordPosition(position.words, value);
                    PositionBeamPoint(position.words, angles.words, 0);
                    SetFacingPoint(position.words);
                    phase = ai.phase;
                    ai.work[3] = 0;
                    ai.phase = phase + 1;
                    return;
                case 1:
                    counter = ai.work[3];
                    if (counter >= 32)
                    {
                        for (index = 0; index < 2; ++index)
                        {
                            if (charge_rings[index])
                            {
                                charge_rings[index]->Destroy(3);
                                charge_rings[index] = 0;
                            }
                        }
                        ClearFacing();
                        StartAnimation(1, 1);
                        SetBehaviorValue(0, 0, 1);
                        value = ReadBeamPoint();
                        ReadRecordPosition(position.words, value);
                        PositionBeamPoint(position.words, angles.words, 1);
                        SetBoneFacingPoint(position.words);
                        phase = ai.phase;
                        ai.work[3] = 0;
                        ai.phase = phase + 1;
                        return;
                    }
                    for (index = 0; index < 2; ++index)
                    {
                        GetModelAttachment(position.words, *this, index + 1);
                        if (!charge_rings[index])
                        {
                            GameHeapBlock *allocation = AllocateEffectStorage(104);
                            RingEffect *created = 0;
                            if (allocation)
                            {
                                created = new (allocation) RingEffect(position.words, 32, 32, 128, 128, 128, 64, 16, 0);
                            }
                            charge_rings[index] = created;
                        }
                        if (!charge_rings[index])
                        {
                            GameDiagnostic("TheBeast charge ring allocation failed\n");
                            abort();
                        }
                        charge_rings[index]->SetPosition(position.words);
                        step = (sint16)ai.work[3];
                        charge_rings[index]->SetRadii(2 * step + 16);
                    }
                    ai.work[3] = counter + 1;
                    return;
                case 2:
                case 3:
                    if (phase == 3 && player_distance < 768)
                    {
                        SetBehaviorValue(0, 0, 0);
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
                    value = ReadBeamPoint();
                    ReadRecordPosition(position.words, value);
                    PositionBeamPoint(position.words, angles.words, phase == 2 ? 2 : 0);
                    SetBoneFacingPoint(position.words);
                    phase = ai.phase;
                    ai.work[3] = 0;
                    ai.phase = phase + 1;
                    return;
                case 4:
                    step = (sint16)ai.work[3];
                    if (step < 48)
                    {
                        player = visual_lists.players;
                        distance = MeasureHorizontalDistance(model.pos.words, player->pos.words);
                        scale = (uint32)((3 * (sint16)this->model.scale.words[0]) >> 4);
                        if (distance >= scale)
                        {
                            ai.work[3] = step + 1;
                            return;
                        }
                    }
                    SetBehaviorValue(0, 0, 0);
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
                    distance = MeasureHorizontalDistance(model.pos.words, player->pos.words);
                    scale = (uint32)((3 * (sint16)this->model.scale.words[0]) >> 4);
                    if (distance < scale)
                    {
                        VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                        health = (sint16)GetGameTuning().player.maximum_health;
                        if (health < 0)
                            health += 3;
                        health >>= 2;
                        damage_target->Damage(health, zero_vector.components, 0);
                    }
                }
                else if (this->playback.complete)
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

void TheBeast::SetParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, group, lane;
    RecordCursor cursor;
    RecordCursor base;
    FUNCTION_MARKER(0x80044474, "SLUS_003.73");
    command &= 65535;
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    base = script_cursor + 2;
    script_cursor = base;
    for (lane = 0; lane < 4; ++lane)
    {
        value = (base + lane * 2).Read16();
        script_cursor = base + lane * 2 + 2;
        spawn_points.records[lane] = value;
    }
    cursor = base + 8;
    for (group = 0; group < 5; ++group)
    {
        for (lane = 0; lane < 5; ++lane)
        {
            cursor = script_cursor;
            value = cursor.Read16();
            cursor += 2;
            spawn_points.groups[group][lane] = value;
            script_cursor = cursor;
        }
    }
    value = cursor.Read16();
    script_cursor = cursor + 2;
    spawn_points.extra_record = value;
    value = (cursor + 2).Read16();
    script_cursor = cursor + 4;
    spawn_points.beam_point = value;
    return;
}

uint32 TheBeast::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 shift = 12, operand, height;
    Vector3 position;
    RecordCursor cursor;

    FUNCTION_MARKER(0x80044608, "SLUS_003.73");
    command &= 65535;
    switch (command)
    {
        case 0x4220:
        case 0x4223:
        case 0x4222:
        case 0x4225:
            cursor = AlignScriptCursor(script_cursor);
            position.words[0] = cursor.Read32();
            position.words[1] = (cursor + 4).Read32();
            position.words[2] = (cursor + 8).Read32();
            ShiftVectorLeft(position.words, shift);
            if (command == 0x4222 || command == 0x4225)
                AddVectors(position.words, this->model.pos.words);
            script_cursor = cursor + 12;
            break;
        case 0x4221:
        case 0x4224:
            cursor = script_cursor;
            operand = cursor.Read16();
            script_cursor = cursor + 2;
            if (operand & 0x2000)
            {
                operand = ResolveParameter(operand);
            }
            ReadRecordPosition(position.words, operand & 65535);
            height = FindGroundHeight(position.words, 512, 2048);
            if (height != (uint32)-1)
                position.words[1] = height - ((uint32)(sint32)(sint16)(uint32)this->ground_offset << 12);
            break;
        case 0x4262:
            script_cursor = script_cursor + 2;
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
    if (command == 0x4220 || command == 0x4221 || command == 0x4222)
        TheBeast::SetDestination(position.words);
    else
        TheBeast::SetJumpDestination(position.words);
    return 1;
}

uint32 TheBeast::SetDestination(const uint32 *position)
{
    uint32 mode, x, y, z, flags;
    FUNCTION_MARKER(0x8004481C, "SLUS_003.73");
    mode = this->model.animation;
    if (mode && mode != 3)
        SetAnimationRange(0, 0, (uint32)-1);
    x = position[0];
    y = position[1];
    z = position[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    flags = (this->control.flags | 1) & 0xFFFFFFF3;
    this->control.flags = flags;
    return flags;
}

uint32 TheBeast::SetJumpDestination(const uint32 *position)
{
    uint32 x, y, z, flags, timer;
    FUNCTION_MARKER(0x80044898, "SLUS_003.73");
    SetAnimationRange(10, 0, (uint32)-1);
    x = position[0];
    y = position[1];
    z = position[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    flags = this->control.flags;
    timer = ai.parameters[1];
    flags |= 5;
    jump_timer = timer;
    this->control.flags = flags;
    return flags;
}

uint32 TheBeast::TrackTarget(ModelInstance *target)
{
    FUNCTION_MARKER(0x80044900, "SLUS_003.73");
    uint32 result = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (result)
        return result;
    tracked_target.SetObject(target);
    result = control.flags | 16;
    control.flags = result;
    return result;
}

uint32 TheBeast::SetFacingPoint(const uint32 *position)
{
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x8004494C, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->aim_pos.words[0] = x;
    this->aim_pos.words[1] = y;
    this->aim_pos.words[2] = z;
    flags = this->control.flags | 32;
    this->control.flags = flags;
    return flags;
}

uint16 *TheBeast::ClearFacing()
{
    uint32 flags;
    FUNCTION_MARKER(0x80044978, "SLUS_003.73");
    flags = this->control.flags;
    this->control.flags = flags & 0xFFFFFFCF;
    motion.angular_velocity.words[2] = 0;
    motion.angular_velocity.words[1] = 0;
    motion.angular_velocity.words[0] = 0;
    motion.angular_acceleration.words[2] = 0;
    motion.angular_acceleration.words[1] = 0;
    motion.angular_acceleration.words[0] = 0;
    return motion.angular_acceleration.words;
}

uint32 TheBeast::SetBoneFacingPoint(const uint32 *position)
{
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x800449AC, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->bone_aim_pos.words[0] = x;
    this->bone_aim_pos.words[1] = y;
    this->bone_aim_pos.words[2] = z;
    flags = this->control.flags | 64;
    this->control.flags = flags;
    return flags;
}

uint32 TheBeast::ClearBoneFacing()
{
    uint32 flags;
    FUNCTION_MARKER(0x800449D8, "SLUS_003.73");
    flags = this->control.flags & 0xFFFFFFBF;
    this->control.flags = flags;
    return flags;
}
