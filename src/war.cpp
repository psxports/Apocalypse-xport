#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "effect_lifecycle.h"
#include "game_tuning.h"
#include "explosion_effects.h"
#include "game_settings.h"
#include "record_data.h"
#include "collision.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "camera.h"
#include "player.h"
#include "weapon_type.h"
#include "particle_beam.h"
#include "visual_object.h"
#include "enemy_list.h"
#include "record_factory.h"
#include "cheat_input.h"
#include "global.h"
#include "angle_math.h"
#include "object_damage.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "object_force.h"
#include "level_session.h"
#include "fire_cycle.h"
#include "war.h"
#include "war_impact.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "game_panel.h"
#include "tracking_projectile.h"
#include "game_heap.h"
#include "audio_session.h"
#include "cd_voice_event.h"
#include "enemy.h"
#include "effect_object.h"

// Script tables retain their original shared module lifetime
struct WarSpawnPoints
{
    uint16 dsts[2][4];
    uint16 stage_recs[6];
    uint16 path[8];
};

static WarSpawnPoints spawn_points = {0};
static const sint32 progression_thresholds[34] = {-6488, -10079, -16450, -17012, -20206, -26773, -30390, -38543, -39406, -62300, -64000, -66000, -68200, -70200, -72100, -74200, -76500, -78800, -80900, -82900, -85000, -87500, -89400, -91900, -94100, -96200, -97600, -99500, -101500, -103500, -105600, -107700, -110100, -99999999};
static const uint8 voice_choices[6] = {0, 6, 10, 11, 12, 13};

struct WarActivationWords
{
    uint16 initial_words[256];
    uint16 *words;
    uint32 capacity;

    WarActivationWords() : words(initial_words), capacity(256)
    {
        memset(initial_words, 0, sizeof(initial_words));
    }

    ~WarActivationWords()
    {
        if (words != initial_words)
            free(words);
    }

    void SetWord(uint32 idx, uint16 val)
    {
        if (idx >= capacity)
        {
            uint32 new_capacity = capacity;
            while (idx >= new_capacity)
                new_capacity *= 2;
            uint16 *new_words = (uint16 *)calloc(new_capacity, sizeof(*new_words));
            if (!new_words)
            {
                GameDiagnostic(("Cannot allocate War activation words\n"));
                abort();
            }
            memcpy(new_words, words, capacity * sizeof(*words));
            if (words != initial_words)
                free(words);
            words = new_words;
            capacity = new_capacity;
        }
        words[idx] = val;
    }
};

static WarActivationWords activation_words;

War::War(const RecordCursor &rec_data, uint16 idx) : Enemy(), jump_timer(0), last_voice(0), grp_cursor(0), threshold_idx(0)
{
    RecordDataBorrow rec_borrow;
    uint16 roll;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8003D41C, "SLUS_003.73");
    aim_pos.x = aim_pos.y = aim_pos.z = 0;

    cursor = ReadPos(rec_data);
    model.angles.x = cursor.Read16();
    model.angles.y = (cursor + 2).Read16();
    roll = (cursor + 4).Read16();
    script_cursor = cursor + 6;
    control.script_active = 1;
    rec_idx = idx;
    col_radius = 0;
    model.angles.z = roll;
    Configure();
    ExecuteScript(script_cursor);
}

void War::Configure()
{
    uint16 flags;
    FUNCTION_MARKER(0x8003D4B8, "SLUS_003.73");
    SetModelBank("war");
    motion.drag[0] = 31;
    motion.drag[1] = 3;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    StartAnimation(0, 1);
    AllocateBehaviorTable(3, 8);
    SetBehaviorVal(0, 0, 0);
    SetBehaviorVal(0, 1, 10);
    SetBehaviorVal(1, 0, 0);
    SetBehaviorVal(2, 0, 0);
    SetBehaviorVal(2, 1, 20);
    SetBehaviorVal(2, 2, 30);
    SetBehaviorVal(2, 3, 512);
    SetBehaviorVal(2, 4, 8);
    SetBehaviorVal(2, 5, 600);
    SetBehaviorVal(2, 6, 256);
    SetBehaviorVal(2, 7, 128);
    flags = lifecycle_flags;
    ai.work[4] = 450;
    ai.parameters[0] = 12;
    ai.parameters[1] = 32;
    contact_strength = 128;
    kind = 221;
    ground_offset = 216;
    health = 800;
    beam = 0;
    ai.state = 0;
    ai.phase = 0;
    control.hierarchy_ready = 0;
    camera_subject.SetObject(&model);
    lifecycle_flags = flags & 65533;
}

War::~War()
{
    if (beam)
        beam->Destroy(3);
    if (attack_beam)
        attack_beam->Destroy(3);
    for (uint32 idx = 0; idx < 5; ++idx)
        if (death_beams[idx])
            death_beams[idx]->Destroy(3);
}

uint32 War::Destroy(uint32 reason)
{
    uint32 idx;
    FUNCTION_MARKER(0x8003D66C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (beam)
        beam->Destroy(3);
    beam = 0;
    if (attack_beam)
        attack_beam->Destroy(3);
    attack_beam = 0;
    Enemy::FreeBehaviorTable();
    VisualObject::ClearTrackedHealth();
    camera_subject.Reset();
    for (idx = 0; idx < 5; ++idx)
    {
        if (death_beams[idx])
            death_beams[idx]->Destroy(3);
        death_beams[idx] = 0;
    }
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 War::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    uint32 flags, distance, speed, result;
    uint16 base_speed, turn_scale;
    uint8 animation;
    sint16 pitch;
    sint32 turn_speed, frame_phase, curve_phase, s;
    Vector3 pos, muzzle;
    ShortVector3 desired, bone_angles, weapon_angles;
    FUNCTION_MARKER(0x8003D790, "SLUS_003.73");
    motion.acceleration = zero_vector;
    if (!control.hierarchy_ready)
    {
        model.SetAttachmentTable(WarAttachments);
        InitializeModelAnimation(WarHierarchy);
        uint16 model_flags = model.flags;
        control.hierarchy_ready = 1;
        model.flags = model_flags | 4;
    }
    uint16 activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        uint8 count = control.activation_count;
        activation_flags = activation & 0xFFFE;
        control.activation_count = count + 1;
    }
    activation = activation_flags;
    if (activation & 4)
    {
        activation_flags = activation & 0xFFFB;
        distance = model.pos.HorizontalDistance(explosion_queue.entries[0].pos);
        if (ai.state == 5 && (sint32)distance < 4096)
        {
            uint32 blast_product = (4096 - distance) * 320;
            if ((sint32)blast_product < 0)
                blast_product += 4095;
            uint32 difficulty = game_settings.difficulty;
            uint32 damage_amount = (sint32)blast_product >> 12;
            if (!difficulty)
                damage_amount += (sint32)blast_product >> 13;
            Damage(damage_amount, &zero_vector, 0);
            if (health > 0)
            {
                ai.state = 64;
                ai.phase = 0;
            }
        }
    }
    if (player_distance < 256 && !(model.flags & 1))
    {
        ModelInstance *contact_player = visual_lists.players;
        if (contact_player)
            static_cast<VisualObject *>(visual_lists.players->owner)->ApplyContactImpulse(*this);
    }
    if (control.script_active)
    {
        uint16 timer = control.script_delay;
        if (timer)
            control.script_delay = timer - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
    {
        /* TODO Resolve the War AI undefined destination branch */
        UpdateState();
    }
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    flags = control.flags;
    surface.pos.x = model.pos.x;
    surface.pos.z = model.pos.z;
    if (flags & 16)
    {
        ModelInstance *target_model = tracked_target.GetObject();
        if (target_model)
        {
            aim_pos = target_model->pos;
        }
        else
            control.flags &= ~16;
        desired.SetDir(model.pos, aim_pos);
        desired.x = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
        goto MovementAnimation;
    }
    if (flags & 32)
    {
        desired.SetDir(model.pos, aim_pos);
        desired.x = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
        goto MovementAnimation;
    }
    if (!(flags & 1))
        goto Footsteps;
    if (flags & 4)
    {
        if ((sint8)model.frame < 3)
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
        if ((sint8)model.frame >= 3 && !(control.flags & 8))
        {
            sint32 jump_vel_x = DivideSignedWord((uint32)dst.x - model.pos.x, jump_timer);
            sint32 jump_vel_z = DivideSignedWord((uint32)dst.z - model.pos.z, jump_timer);
            uint32 duration = jump_timer;
            sint32 jump_vel_y = DivideSignedWord((uint32)dst.y - model.pos.y, duration);
            flags = control.flags;
            control.flags = flags | 8;
            motion.velocity.x = jump_vel_x;
            motion.velocity.z = jump_vel_z;
            motion.velocity.y = jump_vel_y - (duration << 14);
        }
        else if ((sint8)model.frame >= 22 && jump_timer)
            playback.speed = 0;
        uint16 timer = jump_timer;
        if (timer)
        {
            jump_timer = timer - 1;
            if (!jump_timer)
            {
                model.pos = dst;
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
                motion.acceleration.z = 0;
                motion.acceleration.y = 0;
                motion.acceleration.x = 0;
                playback.speed = 65536;
                control.gravity_enabled = 0;
            }
        }
        if (playback.complete)
            control.flags = control.flags & 0xFFFFFFF2;
        goto MovementAnimation;
    }
    desired.SetDir(model.pos, dst);
    pitch = desired.x;
    desired.x = 0;
    turn_scale = ai.parameters[1];
    UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, turn_scale);
    desired.x = pitch;
    turn_speed = motion.angular_velocity.y;
    if (turn_speed < 0)
        turn_speed = -turn_speed;
    if (turn_speed > 64)
        turn_speed = 64;
    base_speed = ai.parameters[0];
    speed = base_speed * (64 - turn_speed) / 64;
    animation = model.animation;
    if (animation < 2)
    {
        frame_phase = (sint8)model.frame * ONE;
        curve_phase = frame_phase / (animation ? 36 : 63);
        s = rsin(curve_phase & 4095);
        if (s < 0)
            s = -s;
        speed = (base_speed >> 2) + speed * s / ONE;
    }
    motion.velocity.SetDir(speed, desired);
    pos.x = model.pos.x;
    pos.y = dst.y;
    pos.z = model.pos.z;
    distance = pos.EuclideanDistance(dst);
    base_speed = ai.parameters[0];
    if (distance < base_speed * 2)
        control.flags = control.flags & ~1;
MovementAnimation:
    if ((control.flags & 1) && model.animation >= 2)
        StartAnimation(0, 1);
Footsteps:
    if (model.animation == 1)
    {
        uint32 event = DidAnimationCrossFrame(1, 7);
        if (UpdateEventLatch(event, 1))
        {
            PlaySoundEffect(128, 8192, 0);
            static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
        }
        event = DidAnimationCrossFrame(1, 25);
        if (UpdateEventLatch(event, 2))
        {
            PlaySoundEffect(129, 8192, 0);
            static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
        }
    }
    bone_angles.x = 0;
    bone_angles.y = 0;
    bone_angles.z = 0;
    flags = control.flags;
    if (flags & 16)
    {
        if (visual_lists.players)
        {
            GetModelAttachment(pos, *this, 2);
            bone_angles.SetDir(pos, static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos);
            bone_angles.y = ((uint16)bone_angles.y - (uint16)model.angles.y) & 4095;
        }
    }
    else if (flags & 32)
    {
        GetModelAttachment(pos, *this, 2);
        bone_angles.SetDir(pos, aim_pos);
        bone_angles.y = ((uint16)bone_angles.y - (uint16)model.angles.y) & 4095;
    }
    UpdateAngularAcceleration(aim.angles, aim.velocity, aim.acceleration, bone_angles, 8);
    aim.angles.AddAngles(aim.velocity);
    aim.angles.NormalizeAngles();
    aim.velocity.AddAngles(aim.acceleration);
    aim.velocity.DampAngles(motion.angular_drag);
    aim.velocity.SnapAngles();
    GetModelJointAngles(3).rotation.x = aim.angles.x;
    GetModelJointAngles(3).rotation.y = aim.angles.y;
    UpdateModelAnimationData();
    UpdateWorldTransform();
    BuildModelPose();
    if (model.animation == 4 && ai.state == 16)
    {
        GetModelAttachment(pos, *this, 0);
        GetModelAttachment(muzzle, *this, 1);
        weapon_angles.SetDir(pos, muzzle);
        if (!attack_beam)
        {
            GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
            ParticleBeam *created = 0;
            if (allocation)
            {
                sint32 beam_range = model.scale.x * 75;
                created = new (allocation) ParticleBeam(&visual_lists.players, beam_range >> 9, 1, 128, 127);
            }
            attack_beam = created;
            if (attack_beam)
            {
                attack_beam->SetColors(0, 64, 128, 185, 185, 185, 0, 64, 128, 255, 255, 255);
                sint16 max_health = GetGameTuning().player.maximum_health;
                uint32 beam_damage = ScaleDamageForDifficulty(max_health / 5);
                attack_beam->damage = beam_damage;
                attack_beam->Fire(pos, weapon_angles);
            }
        }
        else
            attack_beam->Fire(pos, weapon_angles);
    }
    else if (attack_beam)
    {
        attack_beam->Destroy(3);
        attack_beam = 0;
    }
    result = GetBehaviorVal(1, 0);
    if (result)
    {
        if (!beam)
        {
            GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
            ParticleBeam *created = 0;
            if (allocation)
            {
                created = new (allocation) ParticleBeam(&visual_lists.players, 3000, 0, 256, 9);
            }
            beam = created;
            if (!beam)
                return 0;
            beam->damage = 1;
            beam->SetColors(0, 0, 64, 185, 185, 185, 0, 0, 64, 255, 255, 255);
        }
        GetModelAttachment(pos, *this, 3);
        bone_angles.y = ((uint16)model.angles.y + (uint16)aim.angles.y) & 4095;
        return beam->Fire(pos, bone_angles);
    }
    if (beam)
    {
        result = beam->Destroy(3);
        beam = 0;
    }
    return result;
}

uint32 War::Damage(uint32 damage, const Vector3 *, uint32 kind)
{
    uint8 count;
    sint16 health;
    sint32 reduced;
    FUNCTION_MARKER(0x8003E354, "SLUS_003.73");
    if (ai.state < 2)
        return 0;
    health = this->health;
    if (health <= 0)
        return 1;
    count = control.damage_phase;
    control.damage_phase = count + 1;
    reduced = (sint32)damage >> 1;
    if (kind == 30)
        reduced >>= 1;
    else if (kind == 5 || kind == 10)
        reduced >>= 2;
    health = this->health;
    if (reduced < health)
    {
        this->health = health - reduced;
        SetColor(3, 255, 255, 255);
    }
    else
    {
        this->health = 0;
        ai.state = 9;
        ai.phase = 0;
    }
    return 1;
}

void War::Resize(sint32 inc)
{
    FUNCTION_MARKER(0x8003E418, "SLUS_003.73");
    uint16 flags = model.flags;
    uint32 scale = model.scale.x;
    scale += inc;
    model.flags = flags | 0x200;
    uint32 product = scale * 216;
    model.scale.z = scale;
    model.scale.y = scale;
    model.scale.x = scale;
    sint16 old_height = ground_offset;
    ground_offset = (sint32)product >> 12;
    sint32 displacement = (ground_offset - old_height) * ONE;
    model.pos.y = (uint32)model.pos.y - displacement;
}

// State-machine helpers are native extractions of original inline blocks
void War::AdvancePhase()
{
    uint16 phase = ai.phase;
    ai.phase = phase + 1;
}

void War::AdvanceState()
{
    uint16 state = ai.state;
    ai.phase = 0;
    ai.state = state + 1;
}

void War::RestoreState()
{
    uint16 state = ai.parameters[2];
    uint16 phase = ai.parameters[3];
    ai.state = state;
    ai.phase = phase;
}

void War::ClearVelocity()
{
    motion.velocity.z = 0;
    motion.velocity.y = 0;
    motion.velocity.x = 0;
}

void War::SelectPoint(const uint16 *table, uint32 minimum, uint32 attempts, Vector3 &point)
{
    RecordDataBorrow rec_borrow;
    uint32 idx, attempt = 0, distance;
    uint16 rec_idx;
    do
    {
        ++attempt;
        idx = RandomRange(4);
        rec_idx = table[idx];
        ReadRecPos(&point, rec_idx);
        distance = point.Distance(model.pos);
        if (distance >= minimum)
            break;
        if (attempts)
        {
            uint32 retry = attempt < attempts;
            // Original retry delay slot increments the counter again
            ++attempt;
            if (!retry)
                break;
        }
    } while (1);
    SetDst(&point);
    AdvancePhase();
}

void War::ActivateGrp(uint32 maximum)
{
    ModelBorrow model_borrow;
    uint8 cursor;
    uint16 count, entry;
    uint32 idx;
    ModelInstance *list;
    cursor = grp_cursor;
    grp_cursor = cursor + 1;
    count = activation_words.words[cursor];
    if (count >= maximum)
        return;
    for (idx = 0; idx < count; ++idx)
    {
        cursor = grp_cursor;
        list = visual_lists.solids;
        grp_cursor = cursor + 1;
        entry = activation_words.words[cursor];
        VisualObject::ActivateByIdx(list, entry);
    }
}

void War::SetCamera(Camera &camera, uint16 distance, sint16 height, sint16 lateral_offset, sint16 height_offset, sint16 forward_offset, uint16 frames, uint16 yaw, bool set_yaw)
{
    camera.SetOrbitDistanceTransition(distance, frames);
    camera.SetOrbitHeightTransition(height, frames);
    camera.SetTargetLateralOffsetTransition(lateral_offset, frames);
    camera.SetTargetHeightOffsetTransition(height_offset, frames);
    camera.SetTargetForwardOffsetTransition(forward_offset, frames);
    if (set_yaw)
        camera.SetYawTransition(yaw, frames);
}

void War::FireDeathWeapons()
{
    Vector3 origin, endpoint;
    ShortVector3 angles;
    for (uint32 idx = 0; idx < 5; ++idx)
    {
        if (!death_beams[idx])
            continue;
        GetModelAttachment(origin, *this, 5 + idx * 2);
        GetModelAttachment(endpoint, *this, 6 + idx * 2);
        angles.SetDir(origin, endpoint);
        death_beams[idx]->Fire(origin, angles);
    }
}

void War::CreateDeathWeapon(uint32 idx)
{
    GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
    ParticleBeam *created = 0;
    if (allocation)
    {
        created = new (allocation) ParticleBeam(&visual_lists.actors, 4096, 1, 0, 9);
    }
    death_beams[idx] = created;
    if (!death_beams[idx])
    {
        GameDiagnostic(("War death beam allocation failed\n"));
        abort();
    }
    death_beams[idx]->SetColors(64, 64, 64, 185, 185, 185, 64, 64, 64, 255, 255, 255);
}

void War::UpdateState()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    uint16 state;
    uint16 phase;
    uint32 flags;
    uint16 model_flags, lifecycle, rec_idx, move_speed, work, health_threshold;
    uint32 floor, random, pos_y;
    sint32 coord_z, threshold_z;
    uint8 voice_idx;
    uint32 idx;
    uint16 count, next_timer;
    uint8 cursor;
    GameHeapBlock *target;
    Camera *camera;
    ModelInstance *player;
    Vector3 point;
    sint16 height, timer;
    sint32 distance;
    FUNCTION_MARKER(0x8003E478, "SLUS_003.73");
    state = ai.state;
    if (state > 64)
        return;
    switch (state)
    {
        case 0:
            phase = ai.phase;
            if (phase)
                return;
            model_flags = this->model.flags;
            this->model.flags = model_flags | 1;
            floor = CollisionRay::FindGroundHeight(model.pos, 0, 4096);
            if (floor == (uint32)-1)
                return;
            pos_y = (uint32)this->model.pos.y;
            height = ground_offset;
            if (((sint32)(floor - pos_y) >> 12) < height * 2)
            {
                pos_y = floor - ((uint32)height << 12);
                surface.pos.y = pos_y;
                this->model.pos.y = pos_y;
                shadow.height = (sint32)(floor - pos_y) >> 12;
                ai.state = 1;
                control.gravity_enabled = 0;
                ClearVelocity();
                ai.work[1] = 0;
            }
            return;
        case 1:
            phase = ai.phase;
            if (!phase)
                SelectPoint(spawn_points.dsts[0], 257, 0, point);
            else if (phase == 1)
            {
                if (control.activation_count)
                {
                    control.activation_count = 0;
                    ai.state = 2;
                    ai.phase = 0;
                }
                else if (!(control.flags & 1))
                    ai.phase = 0;
            }
            return;
        case 2:
            phase = ai.phase;
            if (!phase)
            {
                col_radius = 128;
                model_flags = this->model.flags;
                ai.parameters[0] = 20;
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                this->model.flags = model_flags & 0xFFFE;
                camera->mode = 18;
                health = 1800;
                TrackHealth(64, 32, 2048);
                AdvancePhase();
            }
            else if (phase == 1)
                SelectPoint(spawn_points.dsts[1], 201, 17, point);
            else if (phase == 2 && !(control.flags & 1))
            {
                player = visual_lists.actors;
                rec_idx = spawn_points.stage_recs[5];
                VisualObject::ActivateByIdx(player, rec_idx);
                state = ai.state;
                ai.parameters[0] = 14;
                ai.phase = 0;
                ai.state = state + 1;
            }
            return;
        case 3:
            phase = ai.phase;
            if (!phase)
            {
                ai.work[1] = 0;
                ai.phase = phase + 1;
                phase = 1;
            }
            if (phase == 1)
                SelectPoint(spawn_points.dsts[1], 201, 17, point);
            else if (phase == 2)
            {
                timer = (sint16)ai.work[1];
                if (!timer)
                {
                    ai.work[1] = 90;
                    height = health;
                    SpawnProjectileWave(height < 1656 ? 2 : 1);
                }
                else
                    ai.work[1] = timer - 1;
                if (health < 1512)
                    AdvanceState();
                else if (!(control.flags & 1))
                {
                    phase = ai.phase;
                    ai.phase = phase - 1;
                }
                else if (player_distance < 512 && !RandomRange(2))
                {
                    state = ai.state;
                    ai.parameters[3] = 1;
                    ai.parameters[4] = 1512;
                    ai.state = 16;
                    ai.phase = 0;
                    ai.parameters[2] = state;
                }
            }
            return;
        case 4:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    rec_idx = spawn_points.stage_recs[0];
                    ReadRecPos(&point, rec_idx);
                    SetDst(&point);
                    AdvancePhase();
                    return;
                case 1:
                    if (!(control.flags & 1))
                    {
                        state = ai.state;
                        ai.parameters[3] = 2;
                        ai.state = 32;
                        ai.phase = 0;
                        ai.parameters[2] = state;
                    }
                    return;
                case 2:
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    player = visual_lists.players;
                    camera->mode = 3;
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(player);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(player);
                    War::SetCamera(*camera, 2276, -520, 0, -1590, -3280, 30, 2048, true);
                    SetAnimationRange(2, 0, -1);
                    AdvancePhase();
                    return;
                case 3:
                    if (playback.complete)
                    {
                        SetAnimationRange(3, 0, -1);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (playback.complete)
                        AdvancePhase();
                    else
                        Resize(178);
                    return;
                case 5:
                    SetAnimationRange(7, 0, -1);
                    AdvancePhase();
                    return;
                case 6:
                    if ((sint8)this->model.frame >= 25)
                    {
                        PlayPositionedSound(131, &model.pos, 0);
                        for (idx = 0; idx < 2; ++idx)
                        {
                            rec_idx = spawn_points.stage_recs[1 + idx];
                            player = visual_lists.solids;
                            VisualObject::ActivateByIdx(player, rec_idx);
                        }
                        AdvancePhase();
                    }
                    return;
                case 7:
                    if (playback.complete)
                    {
                        SetAnimationRange(7, (sint8)model.frame, 0);
                        AdvancePhase();
                    }
                    return;
                case 8:
                    if (playback.complete)
                    {
                        if (GetCheatOptions().war_rec_spawn)
                        {
                            rec_idx = spawn_points.stage_recs[4];
                            ::SpawnRec(rec_idx);
                            model_effect.color_mode = 0x06403030;
                            model_effect.scale = 8192;
                            model_effect.depth_bias = 10;
                        }
                        SetAnimationRange(4, 0, -1);
                        AdvancePhase();
                    }
                    return;
                case 9:
                    if (playback.complete)
                    {
                        phase = ai.phase;
                        ai.work[1] = 0;
                        ai.phase = phase + 1;
                    }
                    return;
                case 10:
                    timer = (sint16)ai.work[1];
                    if (timer < 60)
                        ai.work[1] = timer + 1;
                    else
                    {
                        ai.work[1] = 0;
                        AdvanceState();
                    }
                    return;
                default:
                    return;
            }
        case 5:
            player = visual_lists.players;
            coord_z = player->pos.z;
            if (model.pos.z < coord_z)
            {
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                ai.state = 48;
                flags = control.flags;
                motion.velocity.x = 0;
                ai.phase = 0;
                control.flags = flags & ~1;
                return;
            }
            if (coord_z < -247808000)
            {
                flags = control.flags;
                if (!(flags & 64))
                {
                    control.flags = flags | 64;
                    motion.velocity.z = 0;
                    motion.velocity.y = 0;
                    flags = control.flags;
                    state = ai.state;
                    motion.velocity.x = 0;
                    ai.phase = 0;
                    control.flags = flags & ~1;
                    ai.state = state + 1;
                    return;
                }
            }
            idx = threshold_idx;
            if (idx >= sizeof(progression_thresholds) / sizeof(progression_thresholds[0]))
            {
                GameDiagnostic(("War progression threshold index out of range: %u\n", idx));
                abort();
            }
            threshold_z = (uint32)progression_thresholds[idx] << 12;
            if (model.pos.z < threshold_z)
            {
                cursor = grp_cursor;
                threshold_idx = idx + 1;
                grp_cursor = cursor + 1;
                count = activation_words.words[cursor];
                for (idx = 0; idx < count; ++idx)
                {
                    cursor = grp_cursor;
                    player = visual_lists.solids;
                    grp_cursor = cursor + 1;
                    rec_idx = activation_words.words[cursor];
                    VisualObject::ActivateByIdx(player, rec_idx);
                }
            }
            if (this->model.animation != 1)
                StartAnimation(1, 1);
            phase = ai.phase;
            if (phase == 1)
            {
                SetDst(&dst);
                phase = ai.phase;
                ai.phase = phase - 1;
            }
            else if (!phase)
            {
                player = visual_lists.players;
                distance = (uint32)model.pos.z - player->pos.z;
                if (distance <= 10649599)
                {
                    move_speed = ai.parameters[0];
                    if (move_speed > ((control.flags & 64) ? 40 : 60))
                        ai.parameters[0] = move_speed - 2;
                }
                else if (distance > 11468800)
                {
                    move_speed = ai.parameters[0];
                    if (move_speed < ((control.flags & 64) ? 80 : 128))
                        ai.parameters[0] = move_speed + 1;
                }
                if (!(control.flags & 1))
                {
                    idx = ai.work[1];
                    timer = (sint16)ai.work[1];
                    if (idx >= 8)
                    {
                        /* TODO Original local XYZ is uninitialized; native recovery awaits human contract */
                        GameDiagnostic(("TODO War AI state5 exhausted path vector\n"));
                        abort();
                    }
                    rec_idx = spawn_points.path[idx];
                    ReadRecPos(&point, rec_idx);
                    SetDst(&point);
                    ai.work[1] = timer + 1;
                }
            }
            return;
        case 6:
            phase = ai.phase;
            if (!phase)
            {
                player = visual_lists.players;
                static_cast<Player *>(player->owner)->disable_timer = 240;
                SetAnimationRange(7, 0, -1);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if ((sint8)this->model.frame >= 24)
                {
                    player = visual_lists.actors;
                    rec_idx = spawn_points.stage_recs[3];
                    VisualObject::ActivateByIdx(player, rec_idx);
                    phase = ai.phase;
                    ai.work[0] = 0;
                    ai.phase = phase + 1;
                }
            }
            else if (phase == 2 && playback.complete)
            {
                work = ai.work[0];
                timer = (sint16)ai.work[0];
                if (work < 150)
                    ai.work[0] = timer + 1;
                else
                {
                    SetAnimationRange(7, (sint8)model.frame, 0);
                    AdvancePhase();
                }
            }
            else if (phase == 3 && playback.complete)
            {
                model_flags = this->model.flags;
                this->model.flags = model_flags | 8;
                SetDst(&dst);
                state = ai.state;
                ai.phase = 0;
                ai.state = state - 1;
            }
            return;
        case 9:
            phase = ai.phase;
            if (!phase)
            {
                if (control.flags & 64)
                    ActivateGrp(4);
                static_cast<Player *>(visual_lists.players->owner)->SetWeaponStop(900);
                PlayCdVoiceEvent(18, 4, &model.pos);
                playback.speed = 32768;
                VisualObject::ClearTrackedHealth();
                flags = control.flags;
                control.flags = flags & ~1;
                ClearVelocity();
                ClearFacing();
                SetAnimationRange(8, 0, -1);
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                camera->aim.flags = 1;
                camera->aim.target.SetObject(&model);
                camera->follow.flags = 1;
                camera->follow.target.SetObject(&model);
                War::SetCamera(*camera, 2276, -1872, 64, -30, -714, 30, 2048, true);
                phase = ai.phase;
                ai.work[0] = 0;
                ai.phase = phase + 1;
            }
            else if (phase == 1)
            {
                if (!RandomRange(3))
                {
                    point = this->model.pos;
                    point.y = (uint32)point.y + (1638400);
                    point.z = (uint32)point.z - (1048576);
                    random = RandomRange(512);
                    point.x = (uint32)point.x + ((random - 256) << 12);
                    random = RandomRange(512);
                    point.y = (uint32)point.y + ((random - 256) << 12);
                    random = RandomRange(512);
                    point.z = (uint32)point.z + ((random - 256) << 12);
                    FireCycle::Spawn(&point, 60, 1, 2);
                    random = RandomRange(2);
                    PlayPositionedSound(random + 1, &model.pos, 2);
                }
                if ((sint8)this->model.frame >= 12)
                    this->model.frame = 13;
                timer = (sint16)ai.work[0];
                ai.work[0] = timer + 1;
                if (timer == 42)
                    CreateDeathWeapon(0);
                else if (timer == 58)
                    CreateDeathWeapon(1);
                else if (timer == 68)
                    CreateDeathWeapon(2);
                else if (timer == 73)
                    CreateDeathWeapon(3);
                else if (timer == 89)
                    CreateDeathWeapon(4);
                else if (timer == 90)
                {
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    War::SetCamera(*camera, 2252, -552, 60, 522, 268, 40, 0, false);
                }
                else if (timer >= 141)
                    AdvancePhase();
                FireDeathWeapons();
            }
            else if (phase == 2)
            {
                FireDeathWeapons();
                if ((sint8)this->model.frame >= 28)
                {
                    for (idx = 0; idx < 5; ++idx)
                    {
                        if (death_beams[idx])
                        {
                            death_beams[idx]->Destroy(3);
                            death_beams[idx] = 0;
                        }
                    }
                }
                if ((sint8)this->model.frame >= 33)
                {
                    static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(0);
                    PlayPositionedSound(130, &model.pos, 0);
                    phase = ai.phase;
                    ai.work[0] = 0;
                    ai.phase = phase + 1;
                }
            }
            else if (phase == 3 && playback.complete)
            {
                work = ai.work[0];
                timer = (sint16)ai.work[0];
                if (work < 17)
                    ai.work[0] = timer + 1;
                else
                    CompleteLevel(0);
            }
            return;
        case 16:
            phase = ai.phase;
            if (!phase)
            {
                flags = control.flags;
                ModelInstance *target_model = visual_lists.players;
                control.flags = flags & ~1;
                ClearVelocity();
                TrackTarget(target_model);
                SetAnimationRange(4, 0, -1);
                AdvancePhase();
            }
            else if (phase == 1 && playback.complete)
            {
                if (player_distance < 512)
                {
                    height = health;
                    health_threshold = ai.parameters[4];
                    if (health_threshold < height)
                    {
                        SetAnimationRange(4, 0, -1);
                        return;
                    }
                }
                SetAnimationRange(6, 0, -1);
                ClearFacing();
                AdvancePhase();
            }
            else if (phase == 2 && playback.complete)
                RestoreState();
            return;
        case 32:
            phase = ai.phase;
            if (!phase)
            {
                flags = control.flags;
                ModelInstance *target_model = visual_lists.players;
                control.flags = flags & ~1;
                ClearVelocity();
                TrackTarget(target_model);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                StartAnimation(0, 1);
                SetBehaviorVal(1, 0, 1);
                phase = ai.phase;
                ai.work[0] = 180;
                ai.phase = phase + 1;
            }
            else if (phase == 2)
            {
                if (visual_lists.players)
                {
                    timer = (sint16)ai.work[0];
                    if (timer)
                    {
                        ai.work[0] = timer - 1;
                        return;
                    }
                }
                ai.phase = 3;
            }
            else if (phase == 3)
            {
                SetBehaviorVal(1, 0, 0);
                ClearFacing();
                RestoreState();
            }
            return;
        case 48:
            phase = ai.phase;
            if (!phase)
            {
                player = visual_lists.players;
                static_cast<Player *>(player->owner)->disable_timer = 900;
                SetAnimationRange(7, 0, -1);
                if (control.flags & 64)
                    ActivateGrp(4);
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                camera->aim.flags = 1;
                camera->aim.target.SetObject(&model);
                camera->follow.flags = 1;
                camera->follow.target.SetObject(&model);
                War::SetCamera(*camera, 832, -568, 208, -624, -1104, 16, 1736, true);
                AdvancePhase();
            }
            else if (phase == 1 && (sint8)this->model.frame >= 25)
            {
                PlayPositionedSound(131, &model.pos, 0);
                GetModelAttachment(point, *this, 5);
                target = EffectObject::AllocateStorage(84);
                if (target)
                {
                    new (target) WarImpact(&point);
                }
                player = visual_lists.players;
                height = static_cast<Player *>(player->owner)->health;
                static_cast<Player *>(player->owner)->disable_timer = 0;
                static_cast<Player *>(player->owner)->death_camera_enabled = 0;
                static_cast<Player *>(player->owner)->Damage((uint32)height << 2, &zero_vector, 0);
                player = visual_lists.players;
                static_cast<Player *>(player->owner)->disable_timer = 900;
                AdvancePhase();
            }
            return;
        case 64:
            phase = ai.phase;
            if (!phase)
            {
                flags = control.flags;
                motion.velocity.z = 0;
                control.flags = flags & ~1;
                SetAnimationRange(9, 0, -1);
                ai.phase = 3;
            }
            else if (phase == 2 && playback.complete)
            {
                lifecycle = lifecycle_flags;
                lifecycle_flags = lifecycle | 16;
                StartAnimation(1, 1);
                ai.state = 5;
                ai.phase = 1;
            }
            else if (phase == 3 && (sint8)this->model.frame >= 20)
            {
                lifecycle = lifecycle_flags;
                lifecycle_flags = lifecycle & 0xFFEF;
                ClearVelocity();
                PlaySoundEffect(130, 8192, 0);
                phase = ai.phase;
                ai.work[4] = 0;
                ai.phase = phase + 1;
            }
            else if (phase == 4 && playback.complete)
            {
                timer = (sint16)ai.work[4];
                next_timer = timer + 1;
                if (next_timer < 9)
                    ai.work[4] = next_timer;
                else
                {
                    do
                    {
                        voice_idx = RandomRange(6);
                    } while (voice_idx == last_voice);
                    uint8 voice = voice_choices[voice_idx];
                    PlayCdVoiceEvent(18, voice, &model.pos);
                    last_voice = voice_idx;
                    SetAnimationRange(10, 0, -1);
                    ai.phase = 2;
                }
            }
            return;
        default:
            return;
    }
}

void War::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val, count;
    uint32 idx, grp, word_idx;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8003FCEC, "SLUS_003.73");
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    cursor = script_cursor;
    script_cursor = cursor + 2;
    for (idx = 0; idx < 4; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        spawn_points.dsts[0][idx] = val;
        script_cursor = cursor + 2;
    }
    for (idx = 0; idx < 4; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        cursor += 2;
        spawn_points.dsts[1][idx] = val;
        script_cursor = cursor;
    }
    val = cursor.Read16();
    script_cursor = cursor + 10;
    spawn_points.stage_recs[0] = val;
    for (idx = 0; idx < 2; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        spawn_points.stage_recs[1 + idx] = val;
        script_cursor = cursor + 2;
    }
    for (idx = 0; idx < 8; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        spawn_points.path[idx] = val;
        script_cursor = cursor + 2;
    }
    word_idx = 0;
    for (grp = 0; grp < 9; ++grp)
    {
        cursor = script_cursor;
        script_cursor = cursor + 2;
        count = (cursor + 2).Read16();
        script_cursor = cursor + 4;
        activation_words.SetWord(word_idx++, count);
        if (count)
        {
            for (idx = 0; idx < count; ++idx)
            {
                cursor = script_cursor;
                val = cursor.Read16();
                activation_words.SetWord(word_idx, val);
                ++word_idx;
                script_cursor = cursor + 2;
            }
        }
    }
    cursor = script_cursor;
    val = cursor.Read16();
    script_cursor = cursor + 2;
    spawn_points.stage_recs[3] = val;
    for (grp = 0; grp < 24; ++grp)
    {
        activation_words.SetWord(word_idx++, 1);
        cursor = script_cursor;
        val = cursor.Read16();
        activation_words.SetWord(word_idx, val);
        ++word_idx;
        script_cursor = cursor + 2;
    }
    cursor = script_cursor;
    val = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    spawn_points.stage_recs[4] = val;
    val = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    spawn_points.stage_recs[5] = val;
}

uint32 War::ExecuteCommand(uint16 command)
{
    FUNCTION_MARKER(0x8003FF2C, "SLUS_003.73");
    return Enemy::ExecuteCommand(command);
}

void War::SetDst(const Vector3 *pos)
{
    FUNCTION_MARKER(0x8003FF4C, "SLUS_003.73");
    dst = *pos;
    uint32 floor = CollisionRay::FindGroundHeight(*pos, 1024, 4096);
    if (floor != (uint32)-1)
        dst.y = floor - ((uint32)ground_offset << 12);
    else
        dst.y = model.pos.y;
    control.flags = (control.flags | 1) & 0xFFFFFFF3;
}

void War::TrackTarget(ModelInstance *target)
{
    FUNCTION_MARKER(0x8003FFD8, "SLUS_003.73");
    if (!static_cast<VisualObject *>(target->owner)->IsFinished())
    {
        uint32 flags = control.flags;
        tracked_target.SetObject(target);
        control.flags = (flags & 0xFFFFFFDF) | 16;
    }
}

void War::ClearFacing()
{
    uint32 flags;
    FUNCTION_MARKER(0x80040058, "SLUS_003.73");
    flags = control.flags;
    control.flags = flags & 0xFFFFFFCF;
    motion.angular_velocity.z = 0;
    motion.angular_velocity.y = 0;
    motion.angular_velocity.x = 0;
    motion.angular_acceleration.z = 0;
    motion.angular_acceleration.y = 0;
    motion.angular_acceleration.x = 0;
}

void War::SpawnProjectileWave(sint32 count)
{
    ModelInstance *player;
    GameHeapBlock *allocation;
    uint32 damage;
    sint32 idx;
    uint32 turn = 1024;
    Vector3 origin, target, offset;
    ShortVector3 angles, dir;
    FUNCTION_MARKER(0x80040094, "SLUS_003.73");
    player = visual_lists.players;
    if (!player)
        return;
    GetModelAttachment(origin, *this, 4);
    RingBurstEffect::SpawnExplosion(&origin, 64, 64, 64, 160, 5, 0, 100);
    for (idx = 0; idx < count; ++idx)
    {
        player = visual_lists.players;
        angles.x = -1024;
        angles.y = 0;
        angles.z = 0;
        target = static_cast<VisualObject *>(player->owner)->surface.pos;
        target.y = (uint32)target.y + (262144);
        if (count == 2)
        {
            player = visual_lists.players;
            dir.SetDir(model.pos, player->pos);
            dir.x = 0;
            dir.y = ((uint16)dir.y + turn) & 4095;
            offset.SetDir(180, dir);
            target.x = (uint32)target.x + offset.x;
            target.z = (uint32)target.z + offset.z;
            angles.y = idx ? 512 : -512;
        }
        /* Original computed target is not passed to the projectile constructor */
        (void)target;
        allocation = AllocateZeroedObject(420);
        if (allocation)
        {
            damage = ScaleDamageForDifficulty(10);
            TrackingProjectile *projectile = new (allocation) TrackingProjectile(origin, angles, 1024, damage, &visual_lists.players);
            projectile->SetTrailColor(8, 0, 64, 128);
            projectile->SetSpeed(80);
        }
        turn += 2048;
    }
}
