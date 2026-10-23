#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "resource_script.h"
#include "collision.h"
#include "pickup.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "camera.h"
#include "player.h"
#include "projectile_line.h"
#include "line_projectile.h"
#include "enemy_list.h"
#include "model_bank.h"
#include "screen_flash.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "global.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "object_force.h"
#include "level_session.h"
#include "trail_projectile.h"
#include "game_panel.h"
#include "death.h"
#include "swat.h"
#include "death_persistent_ring.h"
#include "death_fading_ring.h"
#include "ring_burst_effect.h"
#include "record_data.h"
#include "debris.h"
#include "endpoint_beam.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "effect_lifecycle.h"
#include "enemy.h"
#include "effect_object.h"

// Original Death summon scripts at 0x800A6C18 and 0x800A6C28
static const uint16 spawn_swat_script[] = {0x4206, 0x70, 0x2100, 20, 0x2125, 0, 32, 0x4100};
static const uint16 summon_swat_script[] = {0x4206, 0x70, 0x2100, 20, 0x2125, 0, 32, 0x2114, 0x30, 0x4100};

Death::Death(const RecordCursor &rec_data, uint16 idx) : Enemy(), pickup_timer(0), spawn_sound(0), pulse_angle(0)
{
    RecordDataBorrow rec_borrow;
    uint16 roll;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80047A9C, "SLUS_003.73");
    aim_pos.x = aim_pos.y = aim_pos.z = 0;

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

void Death::Configure()
{
    uint16 flags, damage;
    uint32 control_flags;
    sint16 height;
    sint32 product;
    FUNCTION_MARKER(0x80047B34, "SLUS_003.73");
    SetModelBank("death");
    motion.drag[0] = 31;
    motion.drag[1] = 3;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    flags = this->model.flags;
    control_flags = control.flags;
    this->model.flags = flags | 2048;
    control.flags = control_flags | 64;
    StartAnimation(0, 1);
    AllocateBehaviorTable(2, 8);
    SetBehaviorVal(0, 0, 0);
    SetBehaviorVal(0, 1, 10);
    SetBehaviorVal(1, 0, 0);
    damage = ScaleDamageForDifficulty(10);
    SetBehaviorVal(1, 1, damage);
    SetBehaviorVal(1, 2, 96);
    SetBehaviorVal(1, 3, 30);
    SetBehaviorVal(1, 4, 45);
    ai.parameters[0] = 16;
    ai.parameters[1] = 32;
    // The original stores type221 although the factory branch is222
    kind = 221;
    contact_strength = 128;
    ground_offset = 128;
    height = ground_offset;
    health = 1200;
    ai.parameters[9] &= 0xFF00;
    ai.state = 0;
    ai.phase = 0;
    control.hierarchy_ready = 0;
    product = height * 6500;
    shadow.max_height = 1024;
    camera_subject.SetObject(&model);
    flags = this->model.flags;
    model_effect.depth_bias = 10;
    this->model.scale.z = 6500;
    this->model.scale.y = 6500;
    this->model.scale.x = 6500;
    this->model.flags = flags | 512;
    ground_offset = product >> 12;
}

Death::~Death()
{
    if (spawn_sound)
        StopSoundVoices(spawn_sound);
    for (uint32 idx = 0; idx < 10; ++idx)
        if (beams[idx])
            beams[idx]->Finish();
}

uint32 Death::Destroy(uint32 reason)
{
    uint32 idx;
    FUNCTION_MARKER(0x80047CE0, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    VisualObject::ClearTrackedHealth();
    if (spawn_sound)
        StopSoundVoices(spawn_sound);
    spawn_sound = 0;
    for (idx = 0; idx < 10; ++idx)
    {
        if (beams[idx])
            beams[idx]->Finish();
        beams[idx] = 0;
    }
    camera_subject.Reset();
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 Death::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    ModelInstance *player;
    uint32 flags, idx, distance;
    uint16 effect_scale, phase, pickup_roll, kind, activation, script_delay, base_speed, turn_scale, speed;
    uint8 strength, activation_count, pickup_choice;
    sint32 pickup_x_offset, floor;
    Vector3 point, difference, normal;
    ShortVector3 desired;
    sint32 turn_speed;
    FUNCTION_MARKER(0x80047DBC, "SLUS_003.73");
    motion.acceleration = zero_vector;
    if (!control.hierarchy_ready)
    {
        model.SetAttachmentTable(DeathAttachments);
        control.hierarchy_ready = 1;
    }
    if (control.flags & 64)
    {
        if (!model_effect.color_mode)
            model_effect.color_mode = 0x06402020;
        effect_scale = model_effect.scale;
        if (effect_scale < 9216)
            model_effect.scale = effect_scale + 128;
    }
    else if (control.damage_phase)
    {
        uint8 flicker = RandomRange(64);
        model_effect.color_mode = ((flicker + 64) << 16) | 0x06004040;
        phase = pulse_angle;
        strength = control.damage_phase;
        sint32 pulse = strength * rsin(phase & 4095) / 16;
        if (pulse < 0)
            pulse = -pulse;
        phase = pulse_angle;
        model_effect.scale = pulse + 5632;
        strength = control.damage_phase;
        pulse_angle = (phase + 128) & 4095;
        control.damage_phase = strength - 1;
    }
    else
    {
        effect_scale = model_effect.scale;
        if (effect_scale < 129)
            model_effect.color_mode = 0;
        else
            model_effect.scale = effect_scale - 128;
    }
    ++pickup_timer;
    if (pickup_timer >= 451)
    {
        pickup_timer = 0;
        for (idx = 0; idx < 8; ++idx)
        {
            pickup_x_offset = RandomRange(512);
            player = visual_lists.players;
            point.x = (uint32)player->pos.x + (pickup_x_offset - 256) * ONE;
            point.y = (uint32)player->pos.y - 4194304;
            pickup_roll = RandomRange(1024);
            player = visual_lists.players;
            point.z = (uint32)player->pos.z + (pickup_roll + 1024) * ONE;
            floor = CollisionRay::FindGroundHeight(point, 0, 4096);
            if (floor == -1)
                continue;
            pickup_choice = RandomRange(6);
            switch (pickup_choice)
            {
                case 0:
                    kind = 7;
                    break;
                case 1:
                    kind = 4;
                    break;
                case 2:
                    kind = 6;
                    break;
                case 3:
                    kind = 3;
                    break;
                case 4:
                    kind = 2;
                    break;
                default:
                    kind = 1;
                    break;
            }
            Pickup::Spawn(kind, &point, 4, 240, &zero_vector);
            break;
        }
    }
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        activation_count = control.activation_count;
        activation_flags = activation & 0xFFFE;
        control.activation_count = activation_count + 1;
    }
    if (player_distance < 256)
    {
        player = visual_lists.players;
        if (player)
            static_cast<VisualObject *>(visual_lists.players->owner)->ApplyContactImpulse(*this);
    }
    if (control.script_active)
    {
        script_delay = control.script_delay;
        if (script_delay)
            control.script_delay = script_delay - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
    {
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
        ModelInstance *subject = tracked_target.GetObject();
        if (subject)
        {
            aim_pos = subject->pos;
        }
        else
            control.flags &= ~16;
        desired.SetDir(model.pos, aim_pos);
        desired.x = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
    }
    else if (flags & 32)
    {
        desired.SetDir(model.pos, aim_pos);
        desired.x = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
    }
    else if (flags & 1)
    {
        point.x = model.pos.x;
        point.y = dst.y;
        point.z = model.pos.z;
        distance = point.EuclideanDistance(dst);
        base_speed = ai.parameters[0];
        if (distance < (uint32)base_speed * 2)
            control.flags &= ~1;
        else
        {
            desired.SetDir(model.pos, dst);
            desired.x = 0;
            turn_scale = ai.parameters[1];
            UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, turn_scale);
            for (idx = 0; idx < 3; ++idx)
            {
                difference.comp[idx] = (sint32)((uint32)dst.comp[idx] - model.pos.comp[idx]) >> 12;
            }
            NormalizeShortVector(difference, normal);
            turn_speed = motion.angular_velocity.y;
            if (turn_speed < 0)
                turn_speed = -turn_speed;
            if (turn_speed >= 65)
                turn_speed = 64;
            base_speed = ai.parameters[0];
            speed = base_speed * (64 - turn_speed) / 64;
            motion.velocity.x = normal.x * speed;
            motion.velocity.y = normal.y * speed;
            motion.velocity.z = normal.z * speed;
        }
    }
    else
        return UpdateWorldTransform();
    if ((control.flags & 1) && model.animation != 2)
        StartAnimation(2, 1);
    return UpdateWorldTransform();
}

uint32 Death::Damage(uint32 damage, const Vector3 *, uint32 kind)
{
    sint32 reduced;
    FUNCTION_MARKER(0x80048428, "SLUS_003.73");
    if (control.flags & 64)
        return 0;
    control.damage_phase = 8;
    reduced = (sint32)damage >> 1;
    if (kind == 30)
        reduced >>= 1;
    if (kind == 5 || kind == 10)
        reduced >>= 2;
    health -= reduced;
    if (health <= 0)
        VisualObject::ClearTrackedHealth();
    SetColor(3, 255, 255, 255);
    return 1;
}

// Native helpers retain the previously saved decomposition of the original state machine
void Death::AdvancePhase()
{
    ai.phase = ai.phase + 1;
}

void Death::ClearVelocity()
{
    motion.velocity.z = 0;
    motion.velocity.y = 0;
    motion.velocity.x = 0;
}

void Death::HoldTarget(uint32 mask)
{
    uint32 flags = control.flags;
    control.flags = flags & mask;
    ClearVelocity();
    TrackTarget(visual_lists.players);
}

void Death::RestoreState()
{
    uint16 state = ai.parameters[2];
    uint16 phase = ai.parameters[3];
    ai.state = state;
    ai.phase = phase;
}

EndpointBeam *Death::CreateBeam(uint8 r, uint8 g, uint8 b)
{
    GameHeapBlock *allocation = EffectObject::AllocateStorage(164);
    if (!allocation)
        return 0;
    return new (allocation) EndpointBeam(r, g, b, 5, 700, 100, 100, 440, 110, 1100, 93);
}

void Death::ReadSpawnPoint(uint32 idx, Vector3 &point)
{
    RecordDataBorrow rec_borrow;
    uint16 state = ai.parameters[2];
    uint32 count = state == 5 ? sizeof(spawn_points.special_spawn) / sizeof(spawn_points.special_spawn[0]) : sizeof(spawn_points.spawn) / sizeof(spawn_points.spawn[0]);
    if (idx >= count)
    {
        GameDiagnostic(("Death spawn point index %u exceeds %u entries\n", idx, count));
        abort();
    }
    uint16 waypoint = state == 5 ? spawn_points.special_spawn[idx] : spawn_points.spawn[idx];
    ReadRecPos(&point, waypoint);
}

void Death::SelectDst()
{
    RecordDataBorrow rec_borrow;
    uint16 idx;
    sint32 floor;
    sint16 height;
    Vector3 point;
    do
    {
        idx = RandomRange(4);
        idx = spawn_points.dsts[idx];
        ReadRecPos(&point, idx);
    } while (point.Distance(model.pos) < 257);
    floor = CollisionRay::FindGroundHeight(point, 512, 2048);
    height = ground_offset;
    point.y = (uint32)floor - height * ONE;
    SetDst(&point);
    AdvancePhase();
}

void Death::UpdateAttack(uint16 phase)
{
    RecordDataBorrow rec_borrow;
    Vector3 point, origin;
    GameHeapBlock *child;
    uint16 val;
    uint16 idx;
    uint16 state;
    uint16 next_phase;
    uint16 speed;
    uint16 damage;
    EndpointBeam *created;
    ModelInstance *player, *activation_list;
    ShortVector3 angles;
    sint16 timer;
    sint32 next_attack;
    sint32 attack_count = sizeof(spawn_points.attack) / sizeof(spawn_points.attack[0]);
    switch (phase)
    {
        case 0:
            control.flags |= 64;
            StartAnimation(0, 1);
            ai.work[1] = 190;
            HoldTarget(0xFFFFFFFE);
            activation_list = visual_lists.solids;
            idx = spawn_points.activation_rec;
            VisualObject::ActivateByIdx(activation_list, idx);
            AdvancePhase();
            return;
        case 1:
            val = ai.work[1];
            timer = (sint16)ai.work[1];
            if (val)
                ai.work[1] = timer - 1;
            else
            {
                ClearFacing();
                AdvancePhase();
            }
            return;
        case 2:
            timer = (sint16)ai.work[1];
            if (timer < 0 || timer >= attack_count)
            {
                GameDiagnostic(("Death attack point index %d exceeds %d entries\n", timer, attack_count));
                abort();
            }
            val = spawn_points.attack[timer];
            ReadRecPos(&point, val);
            point.y = (uint32)point.y - (ground_offset * ONE);
            SetDst(&point);
            AdvancePhase();
            return;
        case 3:
            if (!(control.flags & 1))
            {
                timer = (sint16)ai.work[1];
                next_attack = timer + 1;
                if (next_attack < 3)
                {
                    ai.work[1] = next_attack;
                    ai.phase = ai.phase - 1;
                }
                else
                    AdvancePhase();
            }
            return;
        case 4:
            HoldTarget(0xFFFFFFBE);
            SetAnimationRange(6, 0, -1);
            created = Death::CreateBeam(32, 0, 96);
            val = spawn_points.beam_target;
            beams[0] = created;
            ReadRecPos(&point, val);
            GetModelAttachment(origin, *this, 0);
            if (!beams[0])
            {
                GameDiagnostic(("Death attack beam allocation failed\n"));
                abort();
            }
            beams[0]->SetEndpoints(&origin, &point);
            val = ai.phase;
            ai.work[0] = 0;
            ai.work[1] = 0;
            ai.phase = val + 1;
            return;
        case 5:
            val = ai.work[0];
            timer = (sint16)ai.work[0];
            if (val < 15)
            {
                ai.work[0] = timer + 1;
                model.frame = 0;
            }
            else if (val == 15)
            {
                ai.work[0] = timer + 1;
                PlayPositionedSound(90, &model.pos, 0);
            }
            val = model.frame;
            if (val >= 4 && val < 13)
            {
                player = visual_lists.players;
                angles.SetDir(model.pos, player->pos);
                timer = (sint8)model.frame;
                angles.y = ((uint16)angles.y - (timer * 64 - 512)) & 4095;
                child = EffectObject::AllocateStorage(268);
                if (child)
                {
                    speed = GetBehaviorVal(1, 2);
                    damage = GetBehaviorVal(1, 1);
                    new (child) LineProjectile(&model.pos, &angles, speed, damage, &visual_lists.players, 64, 32, 128, 1);
                }
                return;
            }
            if (!playback.complete)
                return;
            val = ai.work[1];
            if (val < 4 || (sint32)swat_population.zombies >= 4)
            {
                ai.work[0] = 0;
                ai.work[1] = val + 1;
                SetAnimationRange(6, 0, -1);
            }
            else
            {
                state = ai.state;
                next_phase = ai.phase;
                ai.state = 32;
                ai.phase = 0;
                ai.parameters[2] = state;
                ai.parameters[3] = next_phase + 1;
            }
            return;
        case 6:
            ai.work[0] = 0;
            ai.work[1] = 0;
            SetAnimationRange(6, 0, -1);
            ai.phase = ai.phase - 1;
            return;
    }
}

void Death::UpdateSpawn(uint16 phase)
{
    Vector3 point, origin;
    sint32 idx;
    GameHeapBlock *child;
    uint32 resource;
    uint32 ground_y;
    uint16 drop_height;
    uint8 bank_slot;
    uint16 kind;
    uint16 val;
    uint16 state;
    sint16 timer;
    sint32 beam_count = sizeof(beams) / sizeof(beams[0]);
    switch (phase)
    {
        case 0:
            SetAnimationRange(3, 0, -1);
            HoldTarget(0xFFFFFFFE);
            val = ai.phase;
            ai.work[1] = 0;
            ai.phase = val + 1;
            return;
        case 1:
            timer = (sint8)model.frame;
            val = model.frame;
            if (timer < 14)
                return;
            if (!(val & 1))
            {
                timer = (sint16)ai.work[1];
                if (!timer)
                {
                    spawn_sound = PlayPositionedSound(95, &model.pos, 0);
                }
                if (timer < 10)
                {
                    ReadSpawnPoint(timer, point);
                    beams[timer] = Death::CreateBeam(8, 24, 8);
                    if (!beams[timer])
                    {
                        GameDiagnostic(("Death spawn beam allocation failed\n"));
                        abort();
                    }
                    ai.work[1] = timer + 1;
                }
                else if ((sint8)model.frame >= 58)
                    AdvancePhase();
            }
            GetModelAttachment(origin, *this, 0);
            for (idx = 0; idx < (sint16)ai.work[1]; ++idx)
            {
                if (idx >= beam_count)
                {
                    GameDiagnostic(("Death beam index %d exceeds %d entries\n", idx, beam_count));
                    abort();
                }
                ReadSpawnPoint(idx, point);
                if (beams[idx])
                    beams[idx]->SetEndpoints(&origin, &point);
            }
            return;
        case 2:
            ::StopSoundVoices(spawn_sound);
            spawn_sound = 0;
            for (idx = 0; idx < 10; ++idx)
            {
                if (beams[idx])
                    beams[idx]->Finish();
                beams[idx] = 0;
                ReadSpawnPoint(idx, point);
                DeathFadingRing::Spawn(&point);
                ground_y = point.y;
                drop_height = RandomRange(256) + 128;
                point.y = (uint32)point.y - (drop_height * ONE);
                child = AllocateZeroedObject(320);
                Debris *created = 0;
                if (child)
                {
                    bank_slot = GetRecModelBankSlot();
                    resource = ModelBank::FindResourceIdx(0x3CD11145, bank_slot);
                    kind = resource + RandomRange(16);
                    created = new (child) Debris(&point, &zero_vector, kind, ground_y, 9, 999, GetRecModelBankName());
                }
                spawn_debris[idx] = created;
                if (!created)
                {
                    GameDiagnostic(("Death spawn debris allocation failed\n"));
                    abort();
                }
                created->model.flags |= 512;
                created->model.scale.x = 8192;
                created->model.scale.y = 8192;
                created->model.scale.z = 8192;
                created->bounce_count = 64;
            }
            AdvancePhase();
            return;
        case 3:
            if ((sint8)model.frame < 58)
                return;
            PlaySoundEffect(94, 8192, 0);
            ReadSpawnPoint(0, point);
            point.y = model.pos.y;
            for (idx = 0; idx < 10; ++idx)
            {
                Debris *debris = spawn_debris[idx];
                if (debris)
                {
                    debris->target_pos = point;
                    debris->flags &= 0xFFFE;
                    debris->flags |= 256;
                }
            }
            AdvancePhase();
            return;
        case 4:
            if ((sint8)model.frame < 72)
                return;
            ::SetScrFlash(64, 255, 0, 16, 0, 0);
            for (idx = 0; idx < 10; ++idx)
            {
                if (spawn_debris[idx])
                    spawn_debris[idx]->Finish();
                spawn_debris[idx] = 0;
            }
            ReadSpawnPoint(0, point);
            point.y = model.pos.y;
            child = AllocateZeroedObject(624);
            if (child)
            {
                new (child) Swat(&point, RecordCursor((const uint8 *)spawn_swat_script, sizeof(spawn_swat_script)));
            }
            PlaySoundEffect(93, 8192, 0);
            state = ai.parameters[2];
            ShuffleSpawnPoints(state == 5 ? spawn_points.special_spawn : spawn_points.spawn, state == 5 ? 10 : 21);
            AdvancePhase();
            return;
        case 5:
            if (playback.complete)
            {
                ClearFacing();
                RestoreState();
            }
            return;
    }
}

void Death::UpdateSummon(uint16 phase)
{
    RecordDataBorrow rec_borrow;
    Vector3 point, origin;
    uint16 val;
    uint32 idx;
    GameHeapBlock *child;
    sint16 timer;
    switch (phase)
    {
        case 0:
            HoldTarget(0xFFFFFFFE);
            SetAnimationRange(3, 0, -1);
            val = ai.phase;
            ai.work[1] = 0;
            ai.phase = val + 1;
            return;
        case 1:
            if ((sint8)model.frame < 14)
                return;
            spawn_sound = PlayPositionedSound(95, &model.pos, 0);
            for (idx = 0; idx < 5; ++idx)
            {
                val = spawn_points.spawn[idx];
                ReadRecPos(&point, val);
                beams[idx] = Death::CreateBeam(8, 8, 24);
                GetModelAttachment(origin, *this, 0);
                if (!beams[idx])
                {
                    GameDiagnostic(("Death summon beam allocation failed\n"));
                    abort();
                }
                beams[idx]->SetEndpoints(&origin, &point);
                DeathPersistentRing::Spawn(&point);
                child = AllocateZeroedObject(624);
                if (child)
                {
                    new (child) Swat(&point, RecordCursor((const uint8 *)summon_swat_script, sizeof(summon_swat_script)));
                }
            }
            ShuffleSpawnPoints(spawn_points.spawn, 21);
            AdvancePhase();
            return;
        case 2:
            timer = (sint16)ai.work[1];
            if (timer < 30)
            {
                ai.work[1] = timer + 1;
                return;
            }
            ::StopSoundVoices(spawn_sound);
            spawn_sound = 0;
            for (idx = 0; idx < 5; ++idx)
            {
                if (beams[idx])
                    beams[idx]->Finish();
                beams[idx] = 0;
            }
            val = RandomRange(2);
            PlaySoundEffect(val ? 91 : 92, 8192, 0);
            AdvancePhase();
            return;
        case 3:
            if (playback.complete)
            {
                ClearFacing();
                RestoreState();
            }
            return;
    }
}

void Death::UpdateEnding(uint16 phase)
{
    Vector3 point, dst;
    uint16 val;
    uint32 sound, damage;
    uint32 idx;
    GameHeapBlock *child;
    Camera *camera;
    ModelInstance *node;
    ShortVector3 angles;
    sint16 timer, interval;
    if (phase)
    {
        val = (ai.work[2] - 32) & 4095;
        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
        ai.work[2] = val;
        camera->SetYawTransition(val, 4);
        if (!RandomRange(30))
        {
            val = RandomRange(2);
            PlayPositionedSound(val + 96, &model.pos, 3);
        }
        timer = (sint16)ai.work[3];
        if (timer)
            ai.work[3] = timer - 1;
        else
        {
            interval = (sint16)ai.work[4];
            ai.work[4] = interval >= 4 ? interval - 3 : 1;
            ai.work[3] = interval - 1;
            GetModelAttachment(point, *this, 3);
            val = RandomRange(16);
            point.y = (uint32)point.y + (val << 12);
            dst = point;
            dst.y = (uint32)dst.y - (0x800000);
            angles.comp[0] = -1024;
            angles.comp[1] = 0;
            angles.comp[2] = 0;
            child = EffectObject::AllocateStorage(108);
            if (child)
            {
                new (child) TrailProjectile(&point, &dst, &angles, 16, 64, 64, 240, 200, 200, 0, 0, 128);
            }
        }
    }
    phase = ai.phase;
    switch (phase)
    {
        case 0:
            node = visual_lists.actors;
            static_cast<Player *>(visual_lists.players->owner)->disable_timer = 900;
            while (node)
            {
                if (static_cast<VisualObject *>(node->owner)->kind == 200 && static_cast<Enemy *>(node->owner)->control.variant == 128)
                {
                    damage = static_cast<VisualObject *>(node->owner)->health;
                    static_cast<VisualObject *>(node->owner)->Damage(damage, &zero_vector, 0);
                }
                node = node->next;
            }
            sound = spawn_sound;
            if (sound)
            {
                ::StopSoundVoices(sound);
                spawn_sound = 0;
            }
            PlayPositionedSound(96, &model.pos, 3);
            PlayPositionedSound(96, &model.pos, 3);
            PlayPositionedSound(96, &model.pos, 3);
            for (idx = 0; idx < 10; ++idx)
            {
                if (beams[idx])
                {
                    beams[idx]->Finish();
                    beams[idx] = 0;
                }
            }
            ClearFacing();
            control.flags &= ~1;
            ClearVelocity();
            motion.angular_velocity.z = 0;
            motion.angular_velocity.y = 0;
            motion.angular_velocity.x = 0;
            motion.angular_acceleration.z = 0;
            motion.angular_acceleration.y = 0;
            motion.angular_acceleration.x = 0;
            SetAnimationRange(8, 0, -1);
            camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
            ai.work[2] = 2048;
            ai.work[3] = 16;
            ai.work[4] = 16;
            ai.work[1] = 0;
            if (camera)
            {
                camera->follow.flags = 1;
                camera->follow.target.SetObject(&model);
                camera->aim.flags = 1;
                camera->aim.target.SetObject(&model);
                camera->SetYawTransition(2048, 4);
                static_cast<Camera *>(visual_lists.cameras->owner)->SetOrbitDistanceTransition(1332, 16);
                static_cast<Camera *>(visual_lists.cameras->owner)->SetOrbitHeightTransition(-1608, 64);
                static_cast<Camera *>(visual_lists.cameras->owner)->SetTargetLateralOffsetTransition(0, 16);
                static_cast<Camera *>(visual_lists.cameras->owner)->SetTargetHeightOffsetTransition(460, 16);
                static_cast<Camera *>(visual_lists.cameras->owner)->SetTargetForwardOffsetTransition(564, 16);
                static_cast<Camera *>(visual_lists.cameras->owner)->SetColDistances(0);
            }
            AdvancePhase();
            return;
        case 1:
            if ((sint8)model.frame < 89)
                return;
            RingBurstEffect::SpawnExplosion(&model.pos, 256, 32, 32, 128, 4, phase, 100);
            val = ai.phase;
            ai.work[0] = 0;
            ai.phase = val + 1;
            return;
        case 2:
            timer = (sint16)ai.work[0];
            model.frame = 90;
            if (timer < 180)
                ai.work[0] = timer + 1;
            else
            {
                val = ai.phase;
                ai.work[0] = 0;
                ai.phase = val + 1;
            }
            return;
        case 3:
            timer = (sint16)ai.work[0];
            model.frame = 90;
            if (timer < 128)
                ai.work[0] = timer + 1;
            else
            {
                Finish();
                CompleteLevel(0);
            }
            return;
    }
}

void Death::UpdateState()
{
    ModelBorrow model_borrow;
    uint16 state, phase, val, yaw;
    uint8 frame;
    uint32 flags, pos_y, height_word;
    sint16 height;
    sint32 floor;
    ModelInstance *player;
    sint16 timer;
    FUNCTION_MARKER(0x800484D0, "SLUS_003.73");
    if (health <= 0 && ai.state != 96)
    {
        ai.state = 96;
        ai.phase = 0;
    }
    state = ai.state;
    phase = ai.phase;
    switch (state)
    {
        case 0:
            if (!phase)
            {
                floor = CollisionRay::FindGroundHeight(model.pos, 0, 4096);
                if (floor == -1)
                    return;
                pos_y = (uint32)model.pos.y;
                height = ground_offset;
                if (((sint32)((uint32)floor - pos_y) >> 12) >= height * 2)
                    return;
                model.pos.y = (uint32)floor - height * ONE;
                height_word = (uint32)floor - (uint32)model.pos.y;
                phase = ai.phase;
                surface.pos.y = floor;
                control.gravity_enabled = 0;
                shadow.height = (sint32)height_word >> 12;
                ai.phase = phase + 1;
                ClearVelocity();
                ai.work[1] = 0;
                health = 1200;
                TrackHealth(24, 42, 4096);
            }
            else if (phase == 1)
            {
                timer = (sint16)ai.work[1];
                ai.work[1] = timer + 1;
                if (timer >= 180)
                {
                    flags = control.flags;
                    val = ai.state;
                    ai.phase = 0;
                    control.flags = flags & 0xFFFFFFBF;
                    ai.state = val + 1;
                }
            }
            return;
        case 1:
            if (!phase)
            {
                val = ai.phase;
                ai.work[1] = 0;
                ai.phase = val + 1;
                SelectDst();
            }
            else if (phase == 1)
                SelectDst();
            else if (phase == 2)
            {
                if (health < 960)
                {
                    ai.state = phase;
                    ai.phase = 0;
                    return;
                }
                timer = (sint16)ai.work[1];
                ai.work[1] = timer + 1;
                if (player_distance < 768)
                {
                    val = ai.state;
                    phase = ai.phase;
                    ai.state = 16;
                    ai.phase = 0;
                    ai.parameters[2] = val;
                    ai.parameters[3] = phase;
                }
                else if (timer == 30 && (sint32)swat_population.zombies < 6)
                {
                    val = ai.state;
                    ai.parameters[3] = 0;
                    ai.state = 32;
                    ai.phase = 0;
                    ai.parameters[2] = val;
                }
                else if (!(control.flags & 1))
                    ai.phase = 1;
            }
            return;
        case 2:
            if (!phase)
                SelectDst();
            else if (phase == 1)
            {
                if (health < 720)
                {
                    ai.state = 5;
                    ai.phase = 0;
                }
                else if (player_distance < 768)
                {
                    ai.parameters[2] = state;
                    val = ai.phase;
                    ai.state = 16;
                    ai.phase = 0;
                    ai.parameters[3] = val;
                }
                else if ((sint32)swat_population.zombies < 4)
                {
                    ai.parameters[2] = state;
                    ai.parameters[3] = 0;
                    ai.state = 64;
                    ai.phase = 0;
                }
                else if (!(control.flags & 1))
                    ai.phase = 0;
            }
            return;
        case 5:
            UpdateAttack(phase);
            return;
        case 16:
            if (!phase)
            {
                control.flags = control.flags & 0xFFFFFFFE;
                ClearVelocity();
                ClearFacing();
                SetAnimationRange(5, 0, -1);
                PlayPositionedSound(90, &model.pos, 0);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                yaw = (uint16)model.angles.y;
                frame = model.frame;
                model.angles.y = (yaw - 113) & 4095;
                if (frame >= 8 && frame < 21 && player_distance < 768)
                {
                    player = visual_lists.players;
                    static_cast<VisualObject *>(player->owner)->Damage(2, &zero_vector, 0);
                }
                if (playback.complete)
                {
                    SetAnimationRange(7, 0, -1);
                    AdvancePhase();
                }
            }
            else if (phase == 2 && playback.complete)
                RestoreState();
            return;
        case 32:
            UpdateSpawn(phase);
            return;
        case 64:
            UpdateSummon(phase);
            return;
        case 96:
            UpdateEnding(phase);
            return;
    }
}

void Death::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    uint32 idx;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80049B38, "SLUS_003.73");
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
        spawn_points.dsts[idx] = val;
        script_cursor = cursor + 2;
    }
    for (idx = 0; idx < 3; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        cursor += 2;
        spawn_points.attack[idx] = val;
        script_cursor = cursor;
    }
    val = cursor.Read16();
    script_cursor = cursor + 2;
    spawn_points.beam_target = val;
    for (idx = 0; idx < 21; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        cursor += 2;
        spawn_points.spawn[idx] = val;
        script_cursor = cursor;
    }
    val = cursor.Read16();
    script_cursor = cursor + 2;
    spawn_points.activation_rec = val;
    for (idx = 0; idx < 10; ++idx)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        spawn_points.special_spawn[idx] = val;
        script_cursor = cursor + 2;
    }
}

uint32 Death::ExecuteCommand(uint16 command)
{
    FUNCTION_MARKER(0x80049C48, "SLUS_003.73");
    return Enemy::ExecuteCommand(command);
}

void Death::SetDst(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80049C68, "SLUS_003.73");
    dst = *pos;
    control.flags = (control.flags | 1) & 0xFFFFFFF3;
}

void Death::TrackTarget(ModelInstance *target)
{
    FUNCTION_MARKER(0x80049C98, "SLUS_003.73");
    if (static_cast<VisualObject *>(target->owner)->IsFinished())
        return;
    tracked_target.SetObject(target);
    control.flags = (control.flags & 0xFFFFFFDF) | 16;
}

void Death::ClearFacing()
{
    FUNCTION_MARKER(0x80049D18, "SLUS_003.73");
    control.flags &= ~0x30;
    motion.angular_velocity.z = 0;
    motion.angular_velocity.y = 0;
    motion.angular_velocity.x = 0;
    motion.angular_acceleration.z = 0;
    motion.angular_acceleration.y = 0;
    motion.angular_acceleration.x = 0;
}
