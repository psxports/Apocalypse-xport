#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "psx.h"
#include "level_session.h"
#include "game_tick.h"
#include "collision.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "global.h"
#include "machine_gun.h"
#include "weaving_projectile.h"
#include "visual_object.h"
#include "record_factory.h"
#include "angle_math.h"
#include "model_animation.h"
#include "game_diagnostics.h"
#include "player.h"
#include "game_panel.h"
#include "chopper.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "projectile_base.h"
#include "tank.h"
#include "swat.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "items.h"
#include "enemy.h"

Chopper::Chopper(const RecordCursor &rec_data, uint16 idx) : Enemy(), fire_timer(0), hover_phase(0), sound_timer(0), rotor_sound(0), sound_distance(0)
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80053AB4, "SLUS_003.73");
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    aim_offset.x = aim_offset.y = aim_offset.z = 0;

    cursor = ReadPos(rec_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    control.script_active = 1;
    rec_idx = idx;
    Init();
}

uint16 Chopper::Init()
{
    GameHeapBlock *child;
    uint16 timer;
    FUNCTION_MARKER(0x80053BA8, "SLUS_003.73");
    ++game_progress.level.enemy_total;
    SetModelBank("Chopper");
    AddToList(visual_lists.actors);
    kind = 202;
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    AllocateBehaviorTable(4, 8);
    sound_distance = model.pos.Distance(visual_lists.cameras->pos);
    ai.state = 2;
    control.gravity_enabled = 0;
    ai.phase = 0;
    control.hierarchy_ready = 0;
    health = 100;
    child = AllocateZeroedHeapBlock(88);
    MachineGun *created = 0;
    if (child)
    {
        created = new (child) MachineGun(&visual_lists.players);
    }
    weapon = created;
    if (child)
    {
        weapon->damage = 5;
        weapon->Configure(2, 2, 10, 8096, 0, 11, 128);
    }
    ai.parameters[0] = 16;
    ai.parameters[1] = 48;
    SetBehaviorVal(0, 0, 0);
    SetBehaviorVal(0, 1, 0);
    SetBehaviorVal(0, 2, 0);
    SetBehaviorVal(1, 0, 0);
    SetBehaviorVal(1, 1, 20);
    SetBehaviorVal(1, 2, 30);
    SetBehaviorVal(1, 3, 30);
    SetBehaviorVal(2, 0, 0);
    SetBehaviorVal(2, 1, 20);
    SetBehaviorVal(2, 2, 30);
    SetBehaviorVal(2, 3, 512);
    SetBehaviorVal(2, 4, 8);
    SetBehaviorVal(2, 5, 300);
    SetBehaviorVal(2, 6, 256);
    SetBehaviorVal(2, 7, 128);
    SetBehaviorVal(3, 0, 0);
    SetBehaviorVal(3, 1, 90);
    SetBehaviorVal(3, 2, 60);
    rotor_sound = PlayPositionedSound(84, &model.pos, 0);
    timer = RandomRange(8) + 1;
    sound_timer = timer;
    return timer;
}

Chopper::~Chopper()
{
    if (weapon)
        weapon->Destroy(3);
    if (rotor_sound)
        StopSoundVoices(rotor_sound);
}

uint32 Chopper::Destroy(uint32 reason)
{
    uint32 sound;
    FUNCTION_MARKER(0x80053E6C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (weapon)
        weapon->Destroy(3);
    weapon = 0;
    Enemy::FreeBehaviorTable();
    if (control.flags & 128)
    {
        VisualObject::ClearTrackedHealth();
        control.flags &= 0xFFFFFF7F;
    }
    sound = rotor_sound;
    if (sound)
        StopSoundVoices(sound);
    rotor_sound = 0;
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

void Chopper::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80053F48, "SLUS_003.73");
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    cursor = script_cursor;
    script_cursor = cursor + 2;
    val = (cursor + 2).Read16();
    cursor += 4;
    script_cursor = cursor;
    ai.work[4] = val;
    val = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    ai.work[5] = val;
}

void Chopper::UpdateRoll(sint16 difference)
{
    if (difference < -2048)
        difference += 4096;
    if (difference >= 2049)
        difference -= 4096;
    if (difference)
        motion.angular_acceleration.z = difference >> 3;
    else
        motion.angular_velocity.z = 0;
}

uint32 Chopper::Update()
{
    RecordDataBorrow rec_borrow;
    uint32 flags, distance, voice, prev_distance;
    uint16 val, activation, phase;
    uint8 count;
    bool firing;
    sint32 turn_scale, hover;
    uint32 y;
    Vector3 vector;
    ShortVector3 angles;
    sint32 difference;
    FUNCTION_MARKER(0x80053FA8, "SLUS_003.73");
    motion.acceleration = zero_vector;
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        count = control.activation_count;
        activation_flags = activation & 0xFFFE;
        control.activation_count = count + 1;
    }
    if (!control.hierarchy_ready)
    {
        InitializeModelAnimation(ChopperHierarchy);
        val = model.flags;
        control.hierarchy_ready = 1;
        model.flags = val | 4;
        SetAnimationRange(0, 0, 0xFFFFFFFF);
    }
    if (control.script_active)
    {
        val = control.script_delay;
        if (val)
            control.script_delay = val - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    flags = control.flags;
    if (flags & 16)
    {
        ModelInstance *subject = tracked_target.GetObject();
        if (subject)
        {
            VisualObject *visual = static_cast<VisualObject *>(subject->owner);
            aim_pos = visual->kind == 50 ? visual->surface.pos : subject->pos;
        }
        else
            control.flags &= ~16;
        goto TrackTarget;
    }
    if (flags & 8)
    {
        vector.Add(model.pos, aim_offset);
        aim_pos = vector;
        goto TrackTarget;
    }
    if (flags & 4)
    {
    TrackTarget:
        Aim();
        if (!(control.flags & 1))
            goto LevelRoll;
        angles.SetDir(model.pos, dst);
        goto MoveTarget;
    }
    if (!(flags & 1))
        goto LevelRoll;
    angles.SetDir(model.pos, dst);
    vector.Subtract(dst, model.pos);
    distance = vector.Len();
    val = ai.parameters[1];
    turn_scale = DivideSignedWord(val * 170, (sint32)distance);
    if (turn_scale < -256)
        turn_scale = -256;
    else if (turn_scale >= 257)
        turn_scale = 256;
    else if (turn_scale >= 1 && turn_scale <= 3)
        turn_scale = 4;
    else if (turn_scale < 0 && turn_scale >= -3)
        turn_scale = -4;
    angles.x = 0;
    UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, turn_scale);
    model.angles.x = 0;
MoveTarget:
    if (!(control.flags & 1))
        goto LevelRoll;
    vector.Subtract(model.pos, dst);
    if ((sint32)vector.LenSq() <= 65536)
    {
        control.flags &= 0xFFFFFFFE;
        goto UpdateModel;
    }
    angles.SetDir(model.pos, dst);
    val = ai.parameters[0];
    motion.acceleration.SetDir(val, angles);
    flags = control.flags;
    if (flags & 32)
        difference = ((uint16)angles.y - (uint16)model.angles.y) & 4095;
    else
        difference = 0;
    if (difference >= 2049)
        difference -= 4096;
    if (difference > 1024)
        difference = 1024;
    else if (difference < -1024)
        difference = -1024;
    difference = (-difference) >> 1;
    UpdateRoll((difference & 4095) - (uint16)model.angles.z);
    goto UpdateModel;
LevelRoll:
    UpdateRoll(-(uint16)model.angles.z);
UpdateModel:
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    val = (uint16)GetModelJointAngles(1).rotation.y;
    GetModelJointAngles(1).rotation.y = (val + 512) & 4095;
    val = (uint16)GetModelJointAngles(2).rotation.y;
    GetModelJointAngles(2).rotation.y = (val + 512) & 4095;
    val = (uint16)GetModelJointAngles(3).rotation.y;
    GetModelJointAngles(3).rotation.y = (val + 512) & 4095;
    UpdateWorldTransform();
    BuildModelPose();
    if (GetBehaviorVal(0, 0))
        SetFireDuration(1);
    if (GetBehaviorVal(1, 0))
    {
        val = GetBehaviorVal(1, 3);
        if (!RandomRange(val))
            FireProjectile();
    }
    if (GetBehaviorVal(2, 0))
    {
        val = GetBehaviorVal(2, 2);
        if (!RandomRange(val))
            SpawnItem();
    }
    if (GetBehaviorVal(3, 0))
    {
        val = GetBehaviorVal(3, 2);
        if (!RandomRange(val))
            SpawnEnemy(-1);
    }
    firing = fire_timer != 0;
    if (!weapon)
    {
        GameDiagnostic(("Chopper machine gun is missing\n"));
        abort();
    }
    if (firing)
        weapon->Fire(model.pos, model.angles);
    else
        weapon->Update(&model.pos);
    if (firing)
        --fire_timer;
    if (control.flags & 64)
    {
        phase = hover_phase;
        hover = rsin(phase & 4095) * 16;
        y = model.pos.y;
        hover_phase = (phase + 64) & 4095;
        model.pos.y = y + hover;
    }
    if (rotor_sound)
    {
        val = sound_timer;
        if (val)
            sound_timer = val - 1;
        else
        {
            distance = model.pos.Distance(visual_lists.cameras->pos);
            voice = rotor_sound;
            prev_distance = sound_distance;
            sound_timer = 8;
            UpdatePositionedSoundVoices(voice, &model.pos, (sint32)(distance - prev_distance));
            sound_distance = distance;
        }
    }
    else if (!(game_clock.tick & 7))
    {
        rotor_sound = PlayPositionedSound(84, &model.pos, 0);
    }
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 Chopper::Damage(uint32 damage, const Vector3 *, uint32)
{
    uint16 flags, state;
    ModelInstance *hero;
    sint16 health;
    FUNCTION_MARKER(0x80054714, "SLUS_003.73");
    ++control.damage_phase;
    health = this->health;
    if (health <= 0)
        return 1;
    this->health = health - damage;
    if (this->health > 0)
        SetColor(3, 255, 255, 255);
    else
    {
        hero = visual_lists.players;
        if (hero)
            static_cast<Player *>(hero->owner)->TryPlayKillQuote();
        flags = lifecycle_flags;
        state = ai.state;
        lifecycle_flags = flags & 0xFFEF;
        if (state)
        {
            ai.state = 2;
            ai.phase = 0;
        }
        else
            ai.phase = 5;
        control.script_active = 0;
    }
    return 1;
}

void Chopper::SpawnRandomExplosion(bool large)
{
    sint32 x, y, z;
    Vector3 pos;
    x = RandomRange(256);
    y = RandomRange(256);
    z = RandomRange(256);
    pos.x = (x - 128) * ONE;
    pos.y = (y - 128) * ONE;
    pos.z = (z - 128) * ONE;
    pos.Add(model.pos);
    HandleExplosionProximity(&pos, large);
    if (large)
        RingBurstEffect::SpawnExplosion(&pos, 400, 240, 160, 40, 5, 0, 100);
}

void Chopper::SetDstRelative(sint16 x)
{
    sint32 offset;
    Vector3 pos;
    pos.x = x * 65536;
    ModelInstance *hero = visual_lists.players;
    if (hero)
    {
        offset = (sint16)ai.work[4] * ONE;
        pos.y = (uint32)hero->pos.y - offset;
        offset = (sint16)ai.work[3] * ONE;
        pos.z = (uint32)hero->pos.z - offset;
    }
    else
    {
        pos.y = model.pos.y;
        pos.z = model.pos.z;
    }
    SetDst(&pos);
    ++ai.phase;
}

void Chopper::TrackHero()
{
    ModelInstance *hero = visual_lists.players;
    if (!hero)
        return;
    uint32 y = (uint32)hero->pos.y - model.pos.y;
    uint32 z = (uint32)hero->pos.z - model.pos.z;
    SetRelativeTarget(0, (sint32)y >> 12, (sint32)z >> 12);
}

void Chopper::UpdateState()
{
    RecordDataBorrow rec_borrow;
    uint16 state, step, val;
    uint32 flags, idx, distance;
    sint32 best, delta, minimum;
    ModelInstance *hero;
    RecordCursor rec;
    sint32 distances[3], ground;
    uint32 x, y, z;
    Vector3 pos[3];
    sint16 timer;
    FUNCTION_MARKER(0x800547D0, "SLUS_003.73");
    state = ai.state;
    if (state > 4)
        return;
    if (state == 1 && (control.prev_activation_flags & 1))
    {
        pos[0] = model.pos;
        pos[0].y = (uint32)pos[0].y - (0x400000);
        SetDst(&pos[0]);
        ai.phase = 6;
    }
    step = ai.phase;
    if (state == 0 || state == 3)
    {
        if (step == 0)
        {
            TrackHealth(64, 32, 4096);
            if (state == 0)
            {
                flags = control.flags;
                hero = visual_lists.players;
            }
            else
            {
                hero = visual_lists.players;
                flags = control.flags;
                ai.work[1] = 0;
            }
            control.flags = flags | 128;
            SetTargetObject(hero);
            rec = GetRecPayload(rec_idx);
            val = (rec + 2).Read16();
            ai.work[0] = val;
            SetDstRec(val);
            ai.work[3] = 4096;
            ++ai.phase;
            return;
        }
        if (step == 1)
        {
            if (control.flags & 1)
                return;
            if (state == 3)
                ++ai.work[1];
            rec = GetRecPayload((sint16)ai.work[0]);
            val = (rec + 2).Read16();
            ai.work[0] = val;
            if (state == 0)
                SetDstRec(val);
            else if ((sint16)ai.work[1] < 3)
            {
                SetDstRec(val);
                return;
            }
            else
            {
                ReadRecPos(&pos[0], (sint16)val);
                ai.work[2] = (uint32)pos[0].x >> 16;
                ClearTarget();
                SetTargetRec(ai.work[0]);
            }
            ++ai.phase;
            return;
        }
        if (state == 3)
        {
            if (step == 2)
            {
                if (!(control.flags & 1))
                {
                    pos[0] = model.pos;
                    val = ai.parameters[3];
                    x = (sint16)ai.work[2] * 65536;
                    y = RandomRange(val);
                    val = ai.parameters[3];
                    pos[0].x = x - (((val >> 1) - y) << 12);
                    pos[0].z = (uint32)pos[0].z - (0x400000);
                    SetDst(&pos[0]);
                }
                ReadRecPos(&pos[0], (sint16)ai.work[0]);
                distance = model.pos.EuclideanDistance(pos[0]);
                if (distance >= ai.parameters[2])
                    return;
                FireProjectile();
                ::SpawnRec((sint16)ai.work[0]);
                rec = GetRecPayload((sint16)ai.work[0]);
                if (!rec.Read16())
                {
                    ++ai.phase;
                }
                else
                {
                    val = (rec + 2).Read16();
                    ai.work[0] = val;
                    SetTargetRec(val);
                }
            }
            else if (step == 3)
            {
                ClearTarget();
                SetTargetObject(visual_lists.players);
                val = (uint32)model.pos.x >> 16;
                ai.work[0] = val;
                SetBehaviorVal(0, 0, 1);
                SetBehaviorVal(1, 0, 1);
                ai.parameters[0] = 4;
                ++ai.phase;
            }
            else if (step == 4 && !(control.flags & 1))
            {
                pos[0].y = model.pos.y;
                x = (sint16)ai.work[0] * 65536;
                val = RandomRange(4096);
                pos[0].x = x + (val - 2048) * ONE;
                pos[0].z = model.pos.z;
                SetDst(&pos[0]);
            }
            return;
        }
        switch (step)
        {
            case 2:
                if (control.flags & 1)
                {
                    if ((sint32)swat_population.count < 4)
                    {
                        val = GetBehaviorVal(3, 2);
                        if (!RandomRange(val))
                        {
                            Swat *spawned_swat = SpawnEnemy(-1);
                            if (spawned_swat)
                                spawned_swat->SetMode(0);
                        }
                    }
                    return;
                }
                SetBehaviorVal(1, 0, 1);
                SetBehaviorVal(2, 0, 1);
                ReadRecPos(&pos[0], (sint16)ai.work[0]);
                rec = GetRecPayload((sint16)ai.work[0]);
                val = (rec + 2).Read16();
                ai.work[1] = val;
                ReadRecPos(&pos[1], (sint16)val);
                val = (rec + 4).Read16();
                ai.work[2] = val;
                ReadRecPos(&pos[2], (sint16)val);
                if (!RandomRange(3))
                {
                    val = RandomRange(4096);
                    ai.work[3] = val + 2048;
                }
                hero = visual_lists.players;
                if (hero)
                {
                    distances[0] = pos[0].Distance(hero->pos);
                    hero = visual_lists.players;
                    distances[1] = pos[1].Distance(hero->pos);
                    hero = visual_lists.players;
                    distances[2] = pos[2].Distance(hero->pos);
                    minimum = 0xFFFFFF;
                    best = -1;
                    for (idx = 0; idx < 3; ++idx)
                    {
                        delta = abs(distances[idx] - (sint16)ai.work[3]);
                        if (delta < minimum)
                        {
                            minimum = delta;
                            best = idx;
                        }
                    }
                    if (best > 0)
                    {
                        control.flags |= 32;
                        SetDstRec(ai.work[best]);
                        ai.work[0] = ai.work[best];
                        return;
                    }
                }
                val = RandomRange(16);
                ai.work[1] = val + 16;
                ai.phase = 3;
                return;
            case 3:
                Hover();
                ++ai.phase;
                return;
            case 4:
                timer = (sint16)ai.work[1];
                if (timer)
                {
                    SetBehaviorVal(2, 0, 0);
                    ai.work[1] = timer - 1;
                }
                else
                    ai.phase = 2;
                return;
            case 5:
                SetBehaviorVal(1, 0, 0);
                SetBehaviorVal(2, 0, 0);
                ::SpawnRec((sint16)ai.work[5]);
                ClearTarget();
                flags = control.flags;
                val = ai.work[4];
                control.flags = flags | 32;
                SetDstRec(val);
                ++ai.phase;
                return;
            case 6:
                if (!(control.flags & 1))
                    Finish();
                else
                    SpawnRandomExplosion(false);
                return;
            default:
                return;
        }
    }
    if (state == 1)
    {
        if (step == 0)
        {
            rec = GetRecPayload(rec_idx);
            ReadRecPos(&pos[0], (rec + 2).Read16());
            ReadRecPos(&pos[1], (rec + 4).Read16());
            ai.work[4] = 640;
            x = pos[0].x;
            y = (uint32)pos[1].x;
            if ((sint32)x < (sint32)y)
            {
                ai.work[0] = x >> 16;
                z = x + ((sint32)(y - x) >> 1);
                ai.work[1] = z >> 16;
                ai.work[2] = y >> 16;
            }
            else
            {
                ai.work[0] = y >> 16;
                z = y + ((sint32)(x - y) >> 1);
                ai.work[1] = z >> 16;
                ai.work[2] = x >> 16;
            }
            pos[0] = model.pos;
            pos[0].y = (uint32)pos[0].y - (7372800);
            SetDst(&pos[0]);
            ++ai.phase;
        }
        else if (step >= 1 && step <= 4)
        {
            flags = control.flags;
            if (flags & 1)
                TrackHero();
            else
            {
                if (step == 1)
                {
                    control.flags = flags | 32;
                    SetBehaviorVal(0, 0, 1);
                }
                idx = step == 1 ? 2 : step == 3 ? 0 : 1;
                SetDstRelative(ai.work[idx]);
            }
        }
        else if (step == 5)
        {
            if (control.flags & 1)
                TrackHero();
            else
                ai.phase = 1;
        }
        else if (step == 6 && !(control.flags & 1))
            Finish();
        return;
    }
    if (state == 2)
    {
        if (step == 0)
        {
            motion.velocity = zero_vector;
            motion.acceleration = zero_vector;
            motion.angular_velocity.x = 64;
            motion.angular_velocity.y = 0;
            motion.angular_velocity.z = 0;
            control.flags &= 0xFFFFFFE2;
            ground = CollisionRay::FindGroundHeight(model.pos, 0, 4096);
            if (ground == -1)
                ai.phase = 2;
            else
            {
                ai.work[0] = ground >> 13;
                control.gravity_enabled = 1;
                ai.phase = 1;
            }
        }
        else if (step == 1)
        {
            ground = (sint16)ai.work[0] * 8192;
            if (ground < model.pos.y)
            {
                model.pos.y = ground;
                control.gravity_enabled = 0;
                ai.phase = 2;
            }
            else if (!IsGameHeapUnderPressure())
                SpawnRandomExplosion(false);
        }
        else if (step == 2)
        {
            for (idx = 0; idx < 3; ++idx)
                if (!IsGameHeapUnderPressure())
                    SpawnRandomExplosion(true);
            Finish();
        }
        return;
    }
    if (step)
        return;
    motion.velocity = zero_vector;
    motion.acceleration = zero_vector;
    control.flags &= 0xFFFFFFE2;
    for (idx = 0; idx < 3; ++idx)
        SpawnRandomExplosion(true);
    Finish();
}

uint16 Chopper::ResolveCommandVal(uint16 val)
{
    return (val & 0x2000) ? ResolveParameter(val) : val;
}

uint32 Chopper::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    sint16 script_idx;
    Vector3 pos;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8005568C, "SLUS_003.73");
    switch (command)
    {
        case 0x4220:
        case 0x4222:
            cursor = script_cursor.Aligned(4);
            pos.x = cursor.Read32();
            pos.y = (cursor + 4).Read32();
            pos.z = (cursor + 8).Read32();
            cursor += 12;
            pos.ShiftLeft(12);
            if (command == 0x4222)
                pos.Add(model.pos);
            script_cursor = cursor;
            SetDst(&pos);
            return 1;
        case 0x4221:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            val = ResolveCommandVal(val);
            ReadRecPos(&pos, val);
            SetDst(&pos);
            return 1;
        case 0x4262:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (val == 0)
                SetFireDuration(1);
            else if (val == 1)
                FireProjectile();
            else if (val == 2)
                SpawnItem();
            return 1;
        case 0x4402:
        case 0x4403:
            cursor = script_cursor;
            script_idx = cursor.Read16();
            if (command == 0x4402)
                SpawnTank(script_idx);
            else
                SpawnEnemy(script_idx);
            cursor = script_cursor;
            script_cursor = cursor + 2;
            return 1;
        case 0x4404:
            control.flags |= 32;
            return 1;
        case 0x4405:
            control.flags &= 0xFFFFFFDF;
            return 1;
        case 0x4406:
        case 0x4407:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (!val)
                ClearTarget();
            else if (command == 0x4406)
                SetTargetObject(visual_lists.players);
            else
            {
                val = ResolveCommandVal(val);
                SetTargetRec(val);
            }
            return 1;
        case 0x4408:
            Hover();
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Chopper::SetDst(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80055A1C, "SLUS_003.73");
    dst = *pos;
    control.flags |= 1;
    return control.flags;
}

uint32 Chopper::SetDstRec(uint16 idx)
{
    RecordDataBorrow rec_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80055A48, "SLUS_003.73");
    ReadRecPos(&dst, idx);
    flags = (control.flags | 1) & 0xFFFFFFBF;
    control.flags = flags;
    return flags;
}

uint32 Chopper::Hover()
{
    uint32 flags;
    FUNCTION_MARKER(0x80055A88, "SLUS_003.73");
    motion.velocity.z = 0;
    motion.velocity.y = 0;
    motion.velocity.x = 0;
    motion.acceleration.z = 0;
    motion.acceleration.y = 0;
    flags = control.flags;
    motion.acceleration.x = 0;
    flags = (flags & 0xFFFFFFDF) | 64;
    control.flags = flags;
    return flags;
}

void Chopper::Aim()
{
    FUNCTION_MARKER(0x80055AC0, "SLUS_003.73");
    ShortVector3 desired;
    desired.SetDir(model.pos, aim_pos);
    UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, ai.parameters[1]);
}

uint32 Chopper::SetTargetRec(uint16 idx)
{
    RecordDataBorrow rec_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80055B48, "SLUS_003.73");
    ReadRecPos(&aim_pos, idx);
    flags = control.flags | 4;
    control.flags = flags;
    return flags;
}

uint32 Chopper::SetRelativeTarget(uint32 x, uint32 y, uint32 z)
{
    uint32 flags;
    FUNCTION_MARKER(0x80055B84, "SLUS_003.73");
    aim_offset.x = x << 12;
    aim_offset.y = y << 12;
    aim_offset.z = z << 12;
    flags = control.flags | 8;
    control.flags = flags;
    return flags;
}

uint32 Chopper::SetTargetObject(ModelInstance *target)
{
    FUNCTION_MARKER(0x80055BB4, "SLUS_003.73");
    uint32 result = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (result)
        return result;
    tracked_target.SetObject(target);
    result = control.flags | 16;
    control.flags = result;
    return result;
}

uint32 Chopper::ClearTarget()
{
    uint32 result = control.flags & 0xFFFFFFE3;
    FUNCTION_MARKER(0x80055C00, "SLUS_003.73");
    control.flags = result;
    return result;
}

void Chopper::FireProjectile()
{
    Vector3 pos;
    GameHeapBlock *child;
    uint32 val;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80055C14, "SLUS_003.73");
    pos = model.pos;
    angles.x = model.angles.x;
    angles.y = model.angles.y;
    angles.z = 0;
    WeavingProjectile *projectile = 0;
    child = AllocateZeroedObject(440);
    if (child)
    {
        projectile = new (child) WeavingProjectile(pos, angles, 0, 6, 128, &visual_lists.players, zero_vector);
    }
    val = GetBehaviorVal(1, 1);
    val = ScaleDamageForDifficulty(val);
    if (!projectile)
    {
        GameDiagnostic(("Chopper projectile allocation failed\n"));
        abort();
    }
    projectile->SetDamage(val);
    projectile->SetBlastRadius(256);
}

void Chopper::SetFireDuration(uint32 seconds)
{
    FUNCTION_MARKER(0x80055CEC, "SLUS_003.73");
    fire_timer = seconds ? seconds * 30 : 0xFFFFFFFF;
}

Items *Chopper::SpawnItem()
{
    ObjectBorrow object_borrow;
    GameHeapBlock *child;
    uint16 damage, arm_radius, blast_radius, fuse_ticks, life_ticks, hover_height;
    FUNCTION_MARKER(0x80055D10, "SLUS_003.73");
    child = AllocateZeroedObject(328);
    if (!child)
        return 0;
    damage = GetBehaviorVal(2, 1);
    arm_radius = GetBehaviorVal(2, 3);
    blast_radius = GetBehaviorVal(2, 6);
    fuse_ticks = GetBehaviorVal(2, 4);
    life_ticks = GetBehaviorVal(2, 5);
    hover_height = GetBehaviorVal(2, 7);
    return new (child) Items(&model.pos, damage, arm_radius, blast_radius, fuse_ticks, life_ticks, hover_height);
}

Swat *Chopper::SpawnEnemy(sint32 idx)
{
    RecordDataBorrow rec_borrow;
    GameHeapBlock *child;
    Swat *created = 0;
    FUNCTION_MARKER(0x80055E0C, "SLUS_003.73");
    child = AllocateZeroedObject(624);
    if (child)
    {
        RecordCursor script = idx < 0 ? RecordCursor() : GetScriptLabel(idx);
        created = new (child) Swat(&model.pos, script);
    }
    if (!created)
    {
        GameDiagnostic(("Chopper child allocation failed before control setup\n"));
        abort();
    }
    created->control.flags |= 512;
    return created;
}

Tank *Chopper::SpawnTank(sint32 idx)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80055E90, "SLUS_003.73");
    GameHeapBlock *child = AllocateZeroedObject(576);
    if (!child)
        return 0;
    RecordCursor script = idx < 0 ? RecordCursor() : GetScriptLabel(idx);
    return new (child) Tank(&model.pos, script);
}
