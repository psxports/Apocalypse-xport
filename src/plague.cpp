#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "audio_session.h"
#include "game_tuning.h"
#include "game_tick.h"
#include "pickup.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "camera.h"
#include "player.h"
#include "weapon_type.h"
#include "particle_beam.h"
#include "projectile_line.h"
#include "line_projectile.h"
#include "enemy_list.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "global.h"
#include "collision.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "plague.h"
#include "ring_burst_effect.h"
#include "game_panel.h"
#include "glow_effect.h"
#include "expanding_glow.h"
#include "ring_effect.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "effect_lifecycle.h"
#include "record_data.h"
#include "object_force.h"
#include "level_session.h"
#include "trail_projectile.h"
#include "plague_ground_effect.h"
#include "textured_quad.h"
#include "plague_area.h"
#include "enemy.h"

void Plague::StopMotion()
{
    motion.velocity.z = 0;
    motion.velocity.y = 0;
    motion.velocity.x = 0;
    motion.acceleration.z = 0;
    motion.acceleration.y = 0;
    motion.acceleration.x = 0;
}

void Plague::SelectFarTarget(Vector3 &pos)
{
    RecordDataBorrow rec_borrow;
    uint32 random;
    do
    {
        random = RandomRange(8);
        ReadRecPos(&pos, spawn_points.far_points[random]);
    } while (pos.Distance(model.pos) < 257);
}

void Plague::ClearVelocity()
{
    motion.velocity.z = 0;
    motion.velocity.y = 0;
    motion.velocity.x = 0;
}

void Plague::SaveRecovery(uint16 state)
{
    uint16 prev = ai.state;
    uint16 substate = ai.phase;
    ai.state = state;
    ai.phase = 0;
    ai.parameters[2] = prev;
    ai.parameters[3] = substate;
}

void Plague::CreateOrbitWeapon(uint32 idx)
{
    GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
    ParticleBeam *created = 0;
    if (allocation)
    {
        created = new (allocation) ParticleBeam(&visual_lists.players, 64, 1, 0, 118);
    }
    orbit_weapons[idx] = created;
    if (!created)
    {
        GameDiagnostic(("Plague orbit weapon allocation failed\n"));
        abort();
    }
    created->damage = 4;
}

sint32 Plague::ReadOrbitDelta() const
{
    sint32 s = rsin(orbit_phase & 4095);
    return (19 * s) >> 11;
}

static const uint8 spread_radius[16] = {32, 60, 64, 60, 58, 58, 64, 76, 96, 76, 64, 58, 58, 60, 64, 60};

Plague::Plague(const RecordCursor &rec_data, uint16 idx) : Enemy(), last_pickup_tick(0), jump_idx(0), attack_count(0), jump_timer(0), pickup_stage(0), orbit_sound(0), orbit_phase(0), orbit_timer(0)
{
    RecordDataBorrow rec_borrow;

    uint16 val;
    RecordCursor rotation;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004509C, "SLUS_003.73");
    facing_pos.x = facing_pos.y = facing_pos.z = 0;
    orbit_angles.x = orbit_angles.y = orbit_angles.z = 0;
    rotation = ReadPos(rec_data);
    val = rotation.Read16();
    cursor = rotation + 6;
    model.angles.x = val;
    val = (rotation + 2).Read16();
    model.angles.y = val;
    val = (rotation + 4).Read16();
    script_cursor = cursor;
    control.script_active = 1;
    rec_idx = idx;
    model.angles.z = val;
    Configure();
    ExecuteScript(script_cursor);
}

void Plague::Configure()
{
    FUNCTION_MARKER(0x80045134, "SLUS_003.73");
    SetModelBank("plague");
    motion.drag[0] = 31;
    motion.drag[1] = 31;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    StartAnimation(0, 1);
    AllocateBehaviorTable(1, 5);
    uint16 damage = ScaleDamageForDifficulty(10);
    SetBehaviorVal(0, 1, damage);
    SetBehaviorVal(0, 2, 160);
    ai.parameters[0] = 16;
    ai.parameters[1] = 32;
    kind = 223;
    contact_strength = 128;
    ai.parameters[9] &= 0xFF00;
    ai.state = 0;
    ai.phase = 0;
    this->model.scale.x = 6000;
    this->model.scale.y = 6000;
    this->model.scale.z = 6000;
    ground_offset = 292;
    uint16 flags = this->model.flags;
    control.hierarchy_ready = 0;
    projection.enabled = true;
    this->model.flags = flags | 0x200;
    projection.dir.x = 0;
    projection.dir.y = -ONE;
    projection.dir.z = 0;
    projection.color = 0x808080;
    camera_subject.SetObject(&model);
}

Plague::~Plague()
{
    if (attack_ring)
        attack_ring->Destroy(3);
    if (orbit_sound)
        StopSoundVoices(orbit_sound);
    for (uint32 idx = 0; idx < 3; ++idx)
        if (orbit_weapons[idx])
            orbit_weapons[idx]->Destroy(3);
}

uint32 Plague::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800452B4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    FreeBehaviorTable();
    VisualObject::ClearTrackedHealth();
    projection.enabled = false;
    if (attack_ring)
        attack_ring->Destroy(3);
    attack_ring = 0;
    if (orbit_sound)
        StopSoundVoices(orbit_sound);
    orbit_sound = 0;
    for (uint32 idx = 0; idx < 3; ++idx)
    {
        if (orbit_weapons[idx])
            orbit_weapons[idx]->Destroy(3);
        orbit_weapons[idx] = 0;
    }
    camera_subject.Reset();
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return 0;
}

uint32 Plague::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    ModelInstance *hero;
    uint32 flags, distance;
    uint16 scale, timer;
    CollisionRay rec;
    Vector3 delta, normal;
    ShortVector3 desired;
    sint8 frame;
    sint32 mag;
    FUNCTION_MARKER(0x800453F4, "SLUS_003.73");
    hero = visual_lists.players;
    if (!hero)
        return 0;
    motion.acceleration = zero_vector;
    if (!control.hierarchy_ready)
    {
        model.SetAttachmentTable(PlagueAttachments);
        control.hierarchy_ready = 1;
    }
    uint16 activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        uint8 count = control.activation_count;
        control.activation_count = count + 1;
        flags = control.flags;
        activation_flags = activation & 0xFFFE;
        control.flags = flags | 0x100;
    }
    if (player_distance < 256)
    {
        hero = visual_lists.players;
        if (hero)
            static_cast<VisualObject *>(visual_lists.players->owner)->ApplyContactImpulse(*this);
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
        UpdateState();
    UpdateOrbitingWeapons();
    SpawnPickup();
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    rec.endpoints[0] = model.pos;
    rec.endpoints[0].y = (uint32)rec.endpoints[0].y - (0x40000);
    rec.endpoints[1] = rec.endpoints[0];
    rec.endpoints[1].y = (uint32)rec.endpoints[0].y + 0x1000000;
    rec.Initialize();
    rec.Query(1);
    if (rec.hit.GetObject())
    {
        projection.enabled = true;
        surface.pos = rec.hit_pos;
        surface.n.vx = rec.n.vx;
        surface.n.vy = rec.n.vy;
        surface.n.vz = rec.n.vz;
    }
    else
        projection.enabled = false;
    flags = control.flags;
    if (flags & 0x31)
    {
        if (flags & 16)
        {
            ModelInstance *subject = tracked_target.GetObject();
            if (subject)
            {
                facing_pos = subject->pos;
            }
            else
                control.flags &= ~16;
        }
        const Vector3 *aim_pos = flags & 0x30 ? &facing_pos : &dst;
        desired.SetDir(model.pos, *aim_pos);
        desired.comp[0] = 0;
        scale = flags & 0x30 ? 32 : ai.parameters[1];
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, scale);
        if (flags & 0x30)
            goto attack;
        if (control.flags & 4)
        {
            frame = (sint8)model.frame;
            if (frame < 13)
            {
                playback.speed = 0x10000;
                control.gravity_enabled = 0;
                StopMotion();
            }
            else if (jump_timer)
                control.gravity_enabled = 1;
            frame = (sint8)model.frame;
            if (frame >= 13 && !(control.flags & 8))
            {
                sint32 vx = DivideSignedWord((uint32)dst.x - model.pos.x, jump_timer);
                sint32 vz = DivideSignedWord((uint32)dst.z - model.pos.z, jump_timer);
                uint32 duration = jump_timer;
                sint32 vy = DivideSignedWord((uint32)dst.y - model.pos.y, duration);
                flags = control.flags;
                control.flags = flags | 8;
                motion.velocity.x = vx;
                motion.velocity.z = vz;
                motion.velocity.y = vy - (duration << 14);
                PlayPositionedSound(114, &model.pos, 0);
            }
            else
            {
                frame = (sint8)model.frame;
                if (frame >= 23)
                {
                    if (!jump_timer)
                        goto jump_finished;
                    playback.speed = motion.velocity.y >= 0 && frame < 34 ? 0x10000 : 0;
                }
            }
            timer = jump_timer;
            if (timer)
            {
                --timer;
                jump_timer = timer;
                if (!timer)
                {
                    PlayPositionedSound(119, &model.pos, 0);
                    model.pos = dst;
                    StopMotion();
                    playback.speed = 0x10000;
                    control.gravity_enabled = 0;
                }
            }
        jump_finished:
            if (playback.complete)
                control.flags = control.flags & 0xFFFFFFF2;
        }
        else
        {
            uint8 animation = model.animation;
            if (animation != 6 && animation != 1)
                StartAnimation(1, 1);
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                delta.comp[axis] = (sint32)((uint32)dst.comp[axis] - model.pos.comp[axis]) >> 12;
            }
            NormalizeShortVector(delta, normal);
            mag = motion.angular_velocity.y;
            if (mag < 0)
                mag = -mag;
            if (mag > 128)
                mag = 128;
            scale = ai.parameters[0] * (128 - mag) / 128;
            motion.velocity.x = (uint32)normal.x * scale;
            motion.velocity.y = (uint32)normal.y * scale;
            motion.velocity.z = (uint32)normal.z * scale;
            distance = model.pos.HorizontalDistance(dst);
            scale = ai.parameters[0];
            if (distance < (uint32)(2 * scale))
                control.flags = control.flags & ~1;
        }
    }
attack:
    if (model.animation == 7)
    {
        hero = visual_lists.players;
        if (hero && model.frame >= 15 && model.frame < 20 && model.pos.HorizontalDistance(hero->pos) < 280)
        {
            uint32 damage = ScaleDamageForDifficulty(2);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage(damage, &zero_vector, 0);
        }
    }
    return UpdateWorldTransform();
}

uint32 Plague::Damage(uint32 damage, const Vector3 *, uint32 reason)
{
    uint8 count;
    sint32 reduced;
    FUNCTION_MARKER(0x80045B14, "SLUS_003.73");
    count = control.damage_phase;
    control.damage_phase = count + 1;
    reduced = (sint32)damage >> 1;
    if (reason == 30)
        reduced >>= 1;
    else if (reason == 11)
        reduced -= reduced >> 1;
    health -= reduced;
    if (health <= 0)
        VisualObject::ClearTrackedHealth();
    SetColor(3, 0, 255, 0);
    return 1;
}

void Plague::UpdateState()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    ModelInstance *hero;
    uint16 state = ai.state, substate, val;
    uint32 flags, pos_y;
    GameHeapBlock *child;
    uint16 activation_idx, scale_z;
    uint32 random;
    uint16 timer, phase, speed;
    uint32 damage;
    Vector3 pos, velocity, scaled, sum[4];
    uint32 scalar;
    sint32 x, y;
    uint16 angle;
    ShortVector3 dir, aim;
    sint16 health, height, inc;
    sint32 ground;
    FUNCTION_MARKER(0x80045BA8, "SLUS_003.73");
    if (state >= 33)
        return;
    switch (state)
    {
        case 0:
            substate = ai.phase;
            if (!substate)
            {
                ground = CollisionRay::FindGroundHeight(model.pos, 0, 4096);
                if (ground == -1)
                    return;
                pos_y = (uint32)this->model.pos.y;
                height = ground_offset;
                if (((sint32)((uint32)ground - pos_y) >> 12) >= 2 * height)
                    return;
                pos_y = (uint32)ground - ((uint32)height << 12);
                this->model.pos.y = pos_y;
                shadow.height = (sint32)((uint32)ground - pos_y) >> 12;
                surface.pos.y = ground;
                control.gravity_enabled = 0;
                ClearVelocity();
                ai.work[1] = 0;
                this->health = 800;
                TrackHealth(24, 42, 4096);
                StartAnimation(0, 1);
                goto advance_substate;
            }
            if (substate != 1)
                return;
            val = ai.work[1];
            inc = (sint16)ai.work[1];
            if (val < 180)
                ai.work[1] = inc + 1;
            else
            {
                ai.work[1] = 0;
                ai.state = substate;
                ai.phase = 0;
            }
            return;

        case 1:
            pickup_stage = 1;
            if (!RandomRange(64))
            {
                random = RandomRange(2);
                ReadRecPos(&pos, spawn_points.ground_effect[random]);
                child = EffectObject::AllocateStorage(112);
                if (child)
                {
                    damage = ScaleDamageForDifficulty(10);
                    new (child) PlagueGroundEffect(&pos, damage, 64, 128, 16, 20, 48);
                }
                PlayPositionedSound(116, &pos, 0);
            }
            substate = ai.phase;
            if (!substate)
            {
                if (control.flags & 1)
                    return;
                if (this->health < 560)
                {
                    activation_idx = spawn_points.ground_effect[0];
                    VisualObject::ActivateByIdx(visual_lists.solids, activation_idx);
                    activation_idx = spawn_points.ground_effect[1];
                    VisualObject::ActivateByIdx(visual_lists.solids, activation_idx);
                    goto advance_state;
                }
                uint8 next_jump = jump_idx + 1;
                jump_idx = next_jump;
                if (next_jump >= 2)
                    jump_idx = 0;
                val = jump_idx;
                ReadRecPos(&pos, spawn_points.near_points[val]);
                ground = CollisionRay::FindGroundHeight(pos, 512, 2048);
                height = ground_offset;
                pos.y = (uint32)ground - ((uint32)height << 12);
                SetJumpTarget(&pos);
                val = ai.phase;
                attack_count = 0;
                goto store_substate;
            }
            if (substate != 1 || (control.flags & 1))
                return;
            phase = attack_count;
            if (phase >= 2 || (phase == substate && RandomRange(2)))
                goto far_jump_and_repeat;
            if (this->health < 560)
                goto far_jump_and_repeat;
            phase = attack_count;
            state = ai.state;
            substate = ai.phase;
            ai.state = 16;
            ai.phase = 0;
            attack_count = phase + 1;
            ai.parameters[2] = state;
            ai.parameters[3] = substate;
            return;

        case 2:
            flags = control.flags;
            pickup_stage = 2;
            if (!(flags & 128))
            {
                inc = (sint16)ai.work[0];
                if (inc >= 101)
                {
                    uint32 sound = orbit_sound;
                    control.flags = flags | 128;
                    ::StopSoundVoices(sound);
                }
                else
                    ai.work[0] = inc + 1;
            }
            substate = ai.phase;
            if (!substate)
            {
                activation_idx = spawn_points.orbit_rec;
                VisualObject::ActivateByIdx(visual_lists.solids, activation_idx);
                orbit_sound = PlaySoundEffect(115, 8192, 0);
            }
            if (substate <= 1)
            {
                if (control.flags & 1)
                    return;
                random = RandomRange(2);
                ReadRecPos(&pos, spawn_points.near_points[random]);
                ground = CollisionRay::FindGroundHeight(pos, 512, 2048);
                height = ground_offset;
                pos.y = (uint32)ground - ((uint32)height << 12);
                SetJumpTarget(&pos);
                goto advance_substate;
            }
            if (substate != 2)
                return;
            flags = control.flags;
            if (flags & 1)
                return;
            if (this->health < 320 && (flags & 128))
            {
                activation_idx = spawn_points.orbit_rec;
                VisualObject::ActivateByIdx(visual_lists.solids, activation_idx);
                control.flags = control.flags & 0xFFFFFF7F;
                SelectFarTarget(pos);
                ground = CollisionRay::FindGroundHeight(pos, 512, 2048);
                height = ground_offset;
                pos.y = (uint32)ground - ((uint32)height << 12);
                SetJumpTarget(&pos);
                val = ai.state;
                ai.phase = 0;
                ai.work[0] = 0;
                goto store_state;
            }
            goto far_jump_and_repeat;

        case 3:
            substate = ai.phase;
            pickup_stage = 3;
            if (!substate)
            {
                if (control.flags & 1)
                    return;
                StartAnimation(0, 1);
                TrackTarget(visual_lists.players);
                goto advance_substate;
            }
            if (substate != 1)
                return;
            inc = (sint16)ai.work[0];
            if (inc < 161)
                ai.work[0] = inc + 1;
            else
            {
                ClearFacing();
                goto advance_state;
            }
            return;

        case 4:
            substate = ai.phase;
            pickup_stage = 4;
            if (!substate)
            {
                val = ai.phase;
                ai.parameters[0] = 42;
                goto store_substate;
            }
            if (substate == 1)
            {
                if (control.flags & 1)
                    return;
                if (this->model.animation != 9)
                    SetAnimationRange(9, 0, -1);
                else if (playback.complete)
                    ai.phase = 2;
                return;
            }
            if (substate != 2)
                return;
            if (this->health <= 0)
            {
                control.flags = control.flags & 0xFFFFFFFE;
                ClearVelocity();
                ai.state = 5;
                ai.phase = 0;
                return;
            }
            if (this->model.animation != 6)
                StartAnimation(6, 1);
            flags = control.flags;
            if (!(flags & 1) || !RandomRange(12))
            {
                if (RandomRange(3))
                {
                    hero = visual_lists.players;
                    pos = hero->pos;
                }
                else
                {
                    hero = visual_lists.players;
                    scalar = 8;
                    pos = hero->pos;
                    velocity.Scale(scalar, static_cast<VisualObject *>(hero->owner)->motion.velocity);
                    velocity.y = 0;
                    scalar = 8;
                    scaled.Scale(scalar, velocity);
                    pos.Add(scaled);
                }
                ground = CollisionRay::FindGroundHeight(pos, 512, 2048);
                if (ground == -1)
                    pos.y = this->model.pos.y;
                else
                {
                    height = ground_offset;
                    pos.y = (uint32)ground - ((uint32)height << 12);
                }
                SetMoveTarget(&pos);
            }
            inc = (sint16)ai.work[1];
            if (inc)
                val = inc - 1;
            else
            {
                pos = this->model.pos;
                pos_y = (uint32)pos.y;
                height = ground_offset;
                use_small_effect_pool = false;
                pos.y = pos_y + 0xFFFF0000 + ((uint32)height << 12);
                child = EffectObject::AllocateStorage(172);
                if (child)
                {
                    health = this->health;
                    sint32 lifetime = 150 * (240 - health) / 240 + 150;
                    PlagueArea *area = new (child) PlagueArea();
                    area->EnableSemiTransparency();
                    area->SelectResource(0x019D23F9);
                    area->depth_bias = 128;
                    dir.z = 0;
                    dir.x = 0;
                    dir.y = -ONE;
                    random = RandomRange(48);
                    area->SetColors(random + 64, 128, 0);
                    area->radius = 0;
                    area->SetPos(&pos, &dir, area->radius, 0);
                    area->lifetime = lifetime;
                    sum[0].Add(area->pos, area->corners[0]);
                    sum[1].Add(sum[0], area->corners[1]);
                    sum[2].Add(sum[1], area->corners[2]);
                    scalar = 2;
                    sum[3].ShiftRight(sum[2], scalar);
                    area->center = sum[3];
                }
                use_small_effect_pool = true;
                val = 5;
            }
            ai.work[1] = val;
            hero = visual_lists.players;
            if (model.pos.HorizontalDistance(hero->pos) < 256)
                SaveRecovery(32);
            return;

        case 5:
            substate = ai.phase;
            if (!substate)
            {
                hero = visual_lists.players;
                Camera *camera;
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                static_cast<Player *>(visual_lists.players->owner)->disable_timer = 900;
                if (camera)
                {
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(&model);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(&model);
                    camera->SetOrbitDistanceTransition(1584, 16);
                    camera->SetOrbitHeightTransition(-1208, 16);
                    camera->SetTargetLateralOffsetTransition(0, 16);
                    camera->SetTargetHeightOffsetTransition(-76, 16);
                    camera->SetTargetForwardOffsetTransition(240, 16);
                    camera->SetYawTransition(2496, 16);
                }
                SetAnimationRange(8, 0, -1);
                goto advance_substate;
            }
            if (substate == 3)
            {
                CompleteLevel(0);
                return;
            }
            if (substate == 1)
            {
                if (playback.complete)
                {
                    SetAnimationRange(4, 0, -1);
                    val = ai.phase;
                    ai.work[0] = 0;
                    ai.work[1] = 16;
                    ai.phase = val + 1;
                }
            }
            else if (substate != 2)
                return;
            inc = (sint8)this->model.frame;
            timer = ai.work[0];
            if (inc >= 45)
            {
                dir.x = 512;
                use_small_effect_pool = false;
                dir.y = 4096;
                dir.z = 0;
                PrimitiveParticle::SetSpread(&dir);
                PrimitiveParticle::SetSize(2);
                random = RandomRange(64);
                PrimitiveParticle::SetColor(random + 48, 128, 0);
                PrimitiveParticle::SetColorStep(1, 2, 0);
                GetModelAttachment(pos, *this, 0);
                aim.SetDir(model.pos, pos);
                velocity = pos;
                dir.x = -1024;
                dir.y = 0;
                dir.z = 0;
                velocity.y = (uint32)velocity.y + (0xFF800000);
                child = EffectObject::AllocateStorage(108);
                if (child)
                {
                    new (child) TrailProjectile(&pos, &velocity, &dir, 16, 64, 64, 200, 240, 200, 0, 128, 0);
                }
                GetModelAttachment(pos, *this, 1);
                aim.SetDir(model.pos, pos);
                velocity = pos;
                velocity.y = (uint32)velocity.y + (0xFF800000);
                child = EffectObject::AllocateStorage(108);
                if (child)
                {
                    new (child) TrailProjectile(&pos, &velocity, &dir, 16, 64, 64, 200, 240, 200, 0, 128, 0);
                }
                use_small_effect_pool = true;
                if ((sint8)this->model.frame >= 68)
                    this->model.frame = 68;
            }
            if (!RandomRange(12))
            {
                random = RandomRange(2);
                PlayPositionedSound(random + 1, &model.pos, 0);
                child = EffectObject::AllocateStorage(124);
                if (child)
                {
                    new (child) ExpandingGlow(&model.pos, 64, 128, 0, 512, 10);
                }
                child = EffectObject::AllocateStorage(124);
                if (child)
                {
                    new (child) RingBurstEffect(&model.pos, 5, 255, 255, 255, 1, 32, 128, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
                }
            }
            if (timer > 180)
            {
                if (model.scale.x > 512)
                {
                    scale_z = (uint16)this->model.scale.z;
                    model.scale.x -= 128;
                    this->model.scale.z = scale_z - 128;
                }
                else
                {
                    if (model.scale.y > 512)
                        model.scale.y -= 512;
                    else
                        ai.phase = ai.phase + 1;
                }
            }
            ai.work[0] = timer + 1;
            inc = (sint16)ai.work[1];
            angle = this->model.angles.y;
            this->model.angles.y = angle + inc;
            ai.work[1] = inc + 1;
            return;

        case 16:
            substate = ai.phase;
            if (!substate)
            {
                flags = control.flags;
                hero = visual_lists.players;
                control.flags = flags & 0xFFFFFFFE;
                ClearVelocity();
                TrackTarget(visual_lists.players);
                SetAnimationRange(2, 0, -1);
                GetModelAttachment(pos, *this, 0);
                GameHeapBlock *allocation = EffectObject::AllocateStorage(104);
                RingEffect *created = 0;
                if (allocation)
                {
                    created = new (allocation) RingEffect(&pos, 32, 64, 128, 128, 128, 16, 64, 0);
                }
                attack_ring = created;
                if (!attack_ring)
                {
                    GameDiagnostic(("Plague attack ring allocation failed\n"));
                    abort();
                }
                attack_ring->SetRadii(16);
                goto advance_substate;
            }
            if (substate == 2)
                goto restore_recovery;
            if (substate != 1)
                return;
            GetModelAttachment(pos, *this, 0);
            if ((sint8)this->model.frame < 37)
            {
                if (attack_ring)
                {
                    attack_ring->SetPos(&pos);
                    inc = (sint8)model.frame;
                    attack_ring->SetRadii(inc * 4 + 16);
                }
                return;
            }
            if (attack_ring)
            {
                attack_ring->Destroy(3);
                attack_ring = 0;
            }
            hero = visual_lists.players;
            aim.SetDir(pos, hero->pos);
            child = EffectObject::AllocateStorage(268);
            if (child)
            {
                speed = GetBehaviorVal(0, 2);
                damage = GetBehaviorVal(0, 1);
                new (child) LineProjectile(&pos, &aim, speed, damage, &visual_lists.players, 64, 96, 0, 0);
            }
            for (phase = 0; phase < 4096; phase += 256)
            {
                inc = rcos(phase);
                uint8 spread = spread_radius[phase >> 8];
                x = inc * spread;
                inc = rsin(phase);
                y = inc * spread;
                dir.z = 0;
                dir.x = ((uint16)aim.x - (x >> 12)) & 4095;
                dir.y = ((uint16)aim.y + (y >> 12)) & 4095;
                child = EffectObject::AllocateStorage(268);
                if (child)
                {
                    speed = GetBehaviorVal(0, 2);
                    damage = GetBehaviorVal(0, 1);
                    new (child) LineProjectile(&pos, &dir, speed, damage, &visual_lists.players, 64, 96, 0, 0);
                }
            }
            PlayPositionedSound(117, &model.pos, 0);
            goto advance_substate;

        case 32:
            substate = ai.phase;
            if (!substate)
            {
                flags = control.flags;
                hero = visual_lists.players;
                control.flags = flags & 0xFFFFFFFE;
                ClearVelocity();
                TrackTarget(visual_lists.players);
                SetAnimationRange(7, 0, -1);
                goto advance_substate;
            }
            if (substate == 1)
            {
                if (!playback.complete)
                    return;
                inc = (sint8)this->model.frame;
                SetAnimationRange(7, inc, 0);
                goto advance_substate;
            }
            if (substate == 2)
                goto restore_recovery;
            return;

        default:
            return;
    }

far_jump_and_repeat:
    SelectFarTarget(pos);
    ground = CollisionRay::FindGroundHeight(pos, 512, 2048);
    height = ground_offset;
    pos.y = (uint32)ground - ((uint32)height << 12);
    SetJumpTarget(&pos);
    val = ai.phase;
    ai.phase = val - 1;
    return;

advance_state:
    val = ai.state;
    ai.phase = 0;
store_state:
    ai.state = val + 1;
    return;

advance_substate:
    val = ai.phase;
store_substate:
    ai.phase = val + 1;
    return;

restore_recovery:
    if (!playback.complete)
        return;
    ClearFacing();
    state = ai.parameters[2];
    substate = ai.parameters[3];
    ai.state = state;
    ai.phase = substate;
}

void Plague::SpawnPickup()
{
    RecordDataBorrow rec_borrow;
    uint16 state, trigger, resource;
    uint32 now, elapsed, idx, choice;
    Vector3 pos;
    sint16 health, maximum;
    FUNCTION_MARKER(0x80046EBC, "SLUS_003.73");
    state = pickup_stage;
    now = game_clock.tick;
    elapsed = now - last_pickup_tick;
    if ((sint32)elapsed < 481)
        return;
    if (state == 1 || state == 2)
    {
        if (state == 2)
        {
            maximum = GetGameTuning().player.maximum_health;
            ModelInstance *player = visual_lists.players;
            health = static_cast<VisualObject *>(player->owner)->health;
            maximum /= 2;
            if (health < maximum)
            {
                idx = RandomRange(2);
                trigger = spawn_points.early_support[idx];
                ReadRecPos(&pos, trigger);
                resource = 14;
                goto create;
            }
        }
        choice = RandomRange(3);
        if (!choice)
        {
            idx = RandomRange(2);
            trigger = spawn_points.early_support[idx];
            ReadRecPos(&pos, trigger);
            resource = 5;
            goto create;
        }
        idx = RandomRange(4);
        trigger = spawn_points.early_pickups[idx];
    }
    else
    {
        choice = RandomRange(3);
        if (choice == 1)
        {
            idx = RandomRange(2);
            trigger = spawn_points.late_health[idx + 2];
            ReadRecPos(&pos, trigger);
            resource = 14;
            goto create;
        }
        if (choice)
        {
            idx = RandomRange(2);
            trigger = spawn_points.late_health[idx];
            ReadRecPos(&pos, trigger);
            resource = 15;
            goto create;
        }
        idx = RandomRange(4);
        trigger = spawn_points.late_pickups[idx];
    }
    ReadRecPos(&pos, trigger);
    choice = RandomRange(2);
    resource = choice ? 3 : 4;
create:
    Pickup::Spawn(resource, &pos, 4, 240, &zero_vector);
    now = game_clock.tick;
    last_pickup_tick = now;
}

void Plague::UpdateOrbitingWeapons()
{
    RecordDataBorrow rec_borrow;
    ModelInstance *node = visual_lists.solids;
    sint32 timer;
    uint16 angle;
    uint32 idx, random;
    Vector3 pos, dir;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800470B0, "SLUS_003.73");
    orbit_object.Reset();
    if (node)
    {
        uint16 identity = spawn_points.orbit_rec;
        while (static_cast<VisualObject *>(node->owner)->rec_idx != identity)
        {
            node = node->next;
            if (!node)
                break;
        }
        if (node)
            orbit_object.SetObject(node);
    }
    if (control.flags & 128)
    {
        ReadRecPos(&pos, spawn_points.orbit_center);
        if (!orbit_weapons[0])
        {
            orbit_angles.y = 1365;
            orbit_angles.x = 0;
            orbit_angles.z = 2730;
            CreateOrbitWeapon(0);
        }
        else if (!orbit_weapons[1])
        {
            if (orbit_timer >= 151)
                CreateOrbitWeapon(1);
            else
            {
                timer = orbit_timer;
                orbit_weapons[0]->seg_len = timer * ONE / 150 / 9;
            }
        }
        else if (!orbit_weapons[2])
        {
            if (orbit_timer >= 301)
                CreateOrbitWeapon(2);
            else
            {
                timer = orbit_timer - 150;
                orbit_weapons[1]->seg_len = timer * ONE / 150 / 9;
            }
        }
        else if (orbit_timer < 450)
        {
            timer = orbit_timer - 300;
            orbit_weapons[2]->seg_len = timer * ONE / 150 / 9;
        }
        for (idx = 0; idx < 3; ++idx)
        {
            sint32 delta = ReadOrbitDelta();
            angle = ((uint16)orbit_angles.comp[idx] + delta) & 4095;
            orbit_angles.comp[idx] = angle;
            if (orbit_weapons[idx])
            {
                angles.comp[0] = 0;
                angles.comp[1] = angle;
                angles.comp[2] = 0;
                dir.SetDir(145, angles);
                dir.Add(pos);
                orbit_weapons[idx]->Fire(dir, angles);
            }
        }
        node = orbit_object.GetObject();
        if (node)
            node->angles.y = ((uint16)node->angles.y + ReadOrbitDelta()) & 4095;
        random = RandomRange(4);
        orbit_phase += 4 + random;
        ++orbit_timer;
        return;
    }
    node = orbit_object.GetObject();
    if (node)
        node->angles.y = ((uint16)node->angles.y + 32) & 4095;
    for (idx = 0; idx < 3; ++idx)
    {
        if (orbit_weapons[idx])
            orbit_weapons[idx]->Destroy(3);
        orbit_weapons[idx] = 0;
    }
}

void Plague::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 *fields[8] = {spawn_points.ground_effect, spawn_points.ground_effect + 1, &spawn_points.orbit_center, 0, 0, 0, spawn_points.early_support, spawn_points.early_support + 1};
    uint16 val;
    uint32 idx;
    RecordCursor cursor;
    RecordCursor cached;
    FUNCTION_MARKER(0x800474F0, "SLUS_003.73");
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    cursor = script_cursor + 2;
    script_cursor = cursor;
    val = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    for (idx = 0; idx < 8; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        cursor += 2;
        spawn_points.far_points[idx] = val;
        script_cursor = cursor;
    }
    cached = cursor;
    val = cached.Read16();
    script_cursor = cached + 2;
    spawn_points.orbit_rec = val;
    val = (cached + 2).Read16();
    script_cursor = cached + 36;
    for (idx = 0; idx < 2; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        cursor += 2;
        spawn_points.near_points[idx] = val;
        script_cursor = cursor;
    }
    cached = cursor;
    for (idx = 0; idx < 8; ++idx)
    {
        val = (cached + 2 * idx).Read16();
        script_cursor = cached + 2 * idx + 2;
        // Consume unused encoded slots without retaining dead runtime fields
        if (fields[idx])
            *fields[idx] = val;
    }
    for (idx = 0; idx < 8; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        cursor += 2;
        if (idx < 4)
            spawn_points.early_pickups[idx] = val;
        else
            spawn_points.late_pickups[idx - 4] = val;
        script_cursor = cursor;
    }
    cached = cursor;
    for (idx = 0; idx < 4; ++idx)
    {
        val = (cached + 2 * idx).Read16();
        script_cursor = cached + 2 * idx + 2;
        spawn_points.late_health[idx] = val;
    }
}

uint32 Plague::ExecuteCommand(uint16 command)
{
    FUNCTION_MARKER(0x800476D0, "SLUS_003.73");
    return Enemy::ExecuteCommand(command);
}

void Plague::SetJumpTarget(const Vector3 *pos)
{
    FUNCTION_MARKER(0x800476F0, "SLUS_003.73");
    SetAnimationRange(5, 0, -1);
    dst = *pos;
    uint32 flags = control.flags;
    jump_timer = 40;
    flags |= 5;
    control.flags = flags;
}

void Plague::SetMoveTarget(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80047758, "SLUS_003.73");
    dst = *pos;
    uint32 flags = (control.flags | 1) & 0xFFFFFFF3;
    control.flags = flags;
}

void Plague::TrackTarget(ModelInstance *target)
{
    FUNCTION_MARKER(0x80047788, "SLUS_003.73");
    if (static_cast<VisualObject *>(target->owner)->IsFinished())
        return;
    uint32 flags = control.flags;
    tracked_target.SetObject(target);
    control.flags = (flags & 0xFFFFFFDF) | 16;
}

void Plague::ClearFacing()
{
    FUNCTION_MARKER(0x80047808, "SLUS_003.73");
    uint32 flags = control.flags & 0xFFFFFFCF;
    control.flags = flags;
    motion.angular_velocity.z = 0;
    motion.angular_velocity.y = 0;
    motion.angular_velocity.x = 0;
    motion.angular_acceleration.z = 0;
    motion.angular_acceleration.y = 0;
    motion.angular_acceleration.x = 0;
}
