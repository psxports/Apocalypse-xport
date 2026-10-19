#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "game_tick.h"

#include "enemy_list.h"

#include "world_gravity.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "game_diagnostics.h"

#include "scripted_level_object.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "object_fragments.h"
#include "audio_session.h"
#include "game_random.h"

#include "model_resources.h"
#include "vector_arithmetic.h"
#include "record_data.h"

#include "collision.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "model_render.h"
#include "record_scripts.h"
#include "model_projection.h"
#include "player.h"
#include "effect_sprite.h"
#include "effect_lifecycle.h"

static uint32 DivideMotion(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 RemainderMotion(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return numerator;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return 0;
    return (uint32)((sint32)numerator % (sint32)denominator);
}

void ScriptedLevelObject::SetParameters(uint32 command)
{
    SetScriptParameters(command);
}

ScriptedLevelObject::ScriptedLevelObject(const RecordCursor &data, uint32 index) : Enemy(), contact_damage(0), contacted(0), contact_age(0), sound_voices(0), sound_period(0xFFFFFFFF)
{
    RecordDataBorrow record_borrow;
    ObjectBorrow object_borrow;
    uint32 flags;
    RecordCursor cursor;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        scale_ticks[axis] = 0;
        scale_step.words[axis] = 0;
        push_extent.words[axis] = 0;
    }

    FUNCTION_MARKER(0x8003B964, "SLUS_003.73");
    SetModelBank(GetRecordModelBankName());
    AddToList(visual_lists.solids);
    flags = this->model.flags;
    this->kind = 402;
    this->model.flags = (flags | 273) & 65533;
    cursor = ReadPosition(data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    ai.parameters[0] = 32;
    this->record_idx = index;
    this->control.script_active = 1;
}

ScriptedLevelObject::~ScriptedLevelObject()
{
    if (sound_voices)
        StopSoundVoices(sound_voices);
}

uint32 ScriptedLevelObject::Destroy(uint32 reason)
{
    ObjectBorrow object_borrow;
    uint32 sound;
    FUNCTION_MARKER(0x8003BAE0, "SLUS_003.73");
    RemoveFromList(visual_lists.solids);
    sound = sound_voices;
    if (sound)
    {
        StopSoundVoices(sound);
        sound_voices = 0;
    }
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

void ScriptedLevelObject::ApplyPlayerDamage()
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8003BB60, "SLUS_003.73");
    if (!contact_damage)
        return;
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    if (contact_damage < 0)
        player.RestoreHealth(0 - (sint32)contact_damage);
    else
        player.Damage(contact_damage, zero_vector.components, 0);
}

uint32 ScriptedLevelObject::Update()
{
    ObjectBorrow object_borrow;
    RecordDataBorrow record_borrow;
    uint32 trig_angle;
    uint32 motion_distance;
    FUNCTION_MARKER(0x8003BBD8, "SLUS_003.73");
    sint32 condition;
    sint16 activation;
    sint8 activation_count;
    sint32 delay;
    sint16 next_delay;
    sint32 dst_y;
    sint32 dst_z;
    sint32 move_mode;
    sint32 turn_scale0;
    sint32 move_speed0;
    sint32 yaw_delta;
    sint32 yaw_fixed;
    sint32 yaw_negative;
    sint32 yaw_limit;
    sint32 limited_yaw_fixed;
    sint32 roll_delta;
    sint32 wrapped_roll;
    sint32 roll_fixed;
    sint32 short_roll;
    sint32 roll_accel;
    sint32 turn_scale1;
    sint32 move_speed1;
    sint32 rest_roll_delta;
    sint32 rest_roll_fixed;
    sint32 rest_roll;
    sint32 rest_roll_accel;
    sint32 playing_voices;
    sint32 spr_idx;
    sint16 spr_angle;
    sint32 spr_radius;
    sint32 spr_z;
    sint32 stopped_voices;
    sint32 period;
    sint32 result;
    Vector3 direction;
    ShortVector3 target_angles;

    Vector3 dir0, dir1;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        if (scale_ticks[axis])
        {
            --scale_ticks[axis];
            model.scale.words[axis] += scale_step.words[axis];
        }
        motion.acceleration.words[axis] = zero_vector[axis];
    }
    if (!contacted)
        ++contact_age;
    activation = this->activation_flags;
    this->control.prev_activation_flags = activation;
    if (((activation & 1) != 0))
    {
        activation_count = (uint32)this->control.activation_count;
        this->activation_flags = (activation & 0xFFFE);
        this->control.activation_count = (activation_count + 1);
    }
    if ((uint32)this->control.script_active)
    {
        delay = (uint32)this->control.script_delay;
        condition = (delay == 0);
        next_delay = (delay - 1);
        if (condition)
            ExecuteScript(script_cursor);
        else
            this->control.script_delay = next_delay;
    }
    if (((this->control.flags & 1) == 0))
    {
        if ((ai.parameters[2] != 1))
            goto IntegrateMovement;
        rest_roll_delta = (sint32)(0 - this->model.angles.words[2]);
        if ((sint16)rest_roll_delta < -2048)
            rest_roll_delta += 4096;
        rest_roll_fixed = (sint32)((uint32)rest_roll_delta << 16);
        if ((((sint16)(rest_roll_delta)) >= 2049))
            rest_roll_fixed = (sint32)(((uint32)rest_roll_delta - 4096) << 16);
        rest_roll = (rest_roll_fixed >> 16);
        condition = (rest_roll != 0);
        rest_roll_accel = (rest_roll >> 3);
        if (condition)
        {
            motion.angular_acceleration.words[2] = rest_roll_accel;
            goto IntegrateMovement;
        }
    StopRoll:
        motion.angular_velocity.words[2] = 0;

        goto IntegrateMovement;
    }
    SubtractVectors(direction.words, this->model.pos.words, this->destination.words);
    motion_distance = MeasureVectorLength(direction.words);
    if ((sint32)ai.parameters[0] < (sint32)motion_distance)
    {
        move_mode = ai.parameters[2];
        if (!move_mode)
        {
            CalculateDirectionAngles(target_angles.words, this->model.pos.words, this->destination.words);
            BuildDirectionVector(motion.velocity.words, ai.parameters[0], target_angles.words);
            goto IntegrateMovement;
        }
        if ((move_mode == 1))
        {
            CalculateDirectionAngles(target_angles.words, this->model.pos.words, this->destination.words);
            SubtractVectors(dir0.words, this->destination.words, this->model.pos.words);
            turn_scale0 = (sint32)DivideMotion(8160, MeasureVectorLength(dir0.words));
            if ((((turn_scale0 >= -256) && (turn_scale0 < 257)) && (((uint32)((turn_scale0 - 1))) >= 3)))
            {
                if ((turn_scale0 >= 0))
                {
                UpdateFacingVelocity0:
                    UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, target_angles, (uint32)(turn_scale0 < -256 ? -256 : turn_scale0 > 256 ? 256 : (uint32)(turn_scale0 - 1) < 3 ? 4 : turn_scale0 < 0 && turn_scale0 >= -3 ? -4 : turn_scale0));
                    move_speed0 = ai.parameters[0];
                    this->model.angles.words[0] = 0;
                    BuildDirectionVector(motion.acceleration.words, move_speed0, target_angles.words);
                    if (((this->control.flags & 0x20) != 0))
                        yaw_delta = ((target_angles.words[1] - this->model.angles.words[1]) & 0xFFF);
                    else
                        yaw_delta = 0;
                    yaw_fixed = (sint32)((uint32)yaw_delta << 16);
                    if ((((sint16)(yaw_delta)) >= 2049))
                    {
                        yaw_delta -= 4096;
                        yaw_fixed = (sint32)((uint32)yaw_delta << 16);
                    }
                    condition = ((yaw_fixed >> 16) < 1025);
                    yaw_negative = ((yaw_fixed >> 16) < -1024);
                    if (condition)
                    {
                        condition = !yaw_negative;
                        limited_yaw_fixed = (sint32)((uint32)yaw_delta << 16);
                        if (condition)
                            goto UpdateBankRoll;
                        yaw_limit = -1024;
                    }
                    else
                    {
                        yaw_limit = 1024;
                    }
                    limited_yaw_fixed = (sint32)((uint32)yaw_limit << 16);
                UpdateBankRoll:
                    roll_delta = (((-((limited_yaw_fixed >> 16)) >> 1) & 0xFFF) - this->model.angles.words[2]);

                    wrapped_roll = roll_delta;
                    if ((((sint16)(roll_delta)) < -2048))
                        wrapped_roll = (roll_delta + 4096);
                    roll_fixed = (sint32)((uint32)wrapped_roll << 16);
                    if ((((sint16)(wrapped_roll)) >= 2049))
                        roll_fixed = (sint32)(((uint32)wrapped_roll - 4096) << 16);
                    short_roll = (roll_fixed >> 16);
                    condition = (short_roll == 0);
                    roll_accel = (short_roll >> 3);
                    if (!condition)
                    {
                        motion.angular_acceleration.words[2] = roll_accel;
                        goto IntegrateMovement;
                    }
                    goto StopRoll;
                }
                if ((turn_scale0 < -3))
                    goto UpdateFacingVelocity0;
            }
            goto UpdateFacingVelocity0;
        }
        if ((move_mode != 2))
            goto IntegrateMovement;
        CalculateDirectionAngles(target_angles.words, this->model.pos.words, this->destination.words);
        SubtractVectors(dir1.words, this->destination.words, this->model.pos.words);
        turn_scale1 = (sint32)DivideMotion(8160, MeasureVectorLength(dir1.words));
        if ((((turn_scale1 >= -256) && (turn_scale1 < 257)) && (((uint32)((turn_scale1 - 1))) >= 3)))
        {
            if ((turn_scale1 >= 0))
            {
                goto UpdateFacingVelocity1;
            }
            if ((turn_scale1 < -3))
            {
            UpdateFacingVelocity1:
                UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, target_angles, (uint32)(turn_scale1 < -256 ? -256 : turn_scale1 > 256 ? 256 : (uint32)(turn_scale1 - 1) < 3 ? 4 : turn_scale1 < 0 && turn_scale1 >= -3 ? -4 : turn_scale1));
                move_speed1 = ai.parameters[0];
                this->model.angles.words[0] = 0;
                BuildDirectionVector(motion.acceleration.words, move_speed1, target_angles.words);
                goto IntegrateMovement;
            }
        }
        goto UpdateFacingVelocity1;
    }
    motion_distance = this->destination.words[0];
    dst_y = this->destination.words[1];
    dst_z = this->destination.words[2];
    this->model.pos.words[0] = motion_distance;
    this->model.pos.words[1] = dst_y;
    this->model.pos.words[2] = dst_z;
    if ((ai.parameters[2] < 2))
    {
        motion.velocity.words[2] = 0;
        motion.velocity.words[1] = 0;
        motion.velocity.words[0] = 0;
    }
    this->control.flags = (this->control.flags & (~1));
IntegrateMovement:
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);

    IntegrateMotion();
    if (((ai.parameters[2] == 2) && visual_lists.players))
    {
        if (((sint32)MeasureHorizontalDistance(model.pos.words, visual_lists.players->pos.words) >= ai.parameters[3]))
        {
            stopped_voices = sound_voices;
            this->control.flags = (this->control.flags & (~8));
            if (stopped_voices)
            {
                ::StopSoundVoices(stopped_voices);
                sound_voices = 0;
            }
        }
        else
        {
            if (((this->control.flags & 8) == 0))
                SpawnEnemyIndices(GetLevelRecord(this->record_idx) + 6);
            motion_distance = this->control.flags;
            playing_voices = sound_voices;
            this->control.flags = motion_distance | 8;
            if (!playing_voices)
                sound_voices = PlaySoundEffect(11, 8192, 0);
            spr_idx = 0;
            static_cast<Player *>(visual_lists.players->owner)->Damage(2, zero_vector.components, 0);
            while (spr_idx < (sint32)RandomRange(4))
            {
                spr_angle = RandomRange(4096);
                spr_radius = RandomRange((ai.parameters[3] >> 1));
                trig_angle = spr_angle & 0xFFF;
                direction.components[0] = (sint32)((uint32)rsin(trig_angle) * (uint32)spr_radius);
                spr_z = (sint32)((uint32)rcos(trig_angle) * (uint32)spr_radius);
                direction.components[1] = 0;
                direction.components[2] = spr_z;
                AddVectors(direction.words, visual_lists.players->pos.words);
                direction.components[1] = this->model.pos.words[1];
                GameHeapBlock *allocation = AllocateEffectStorage(120);
                if (!allocation)
                {
                    // TODO Original particle allocation failure writes through null
                    GameDiagnostic("TODO ScriptedLevelObject particle allocation failed\n");
                    abort();
                }
                AnimatedSprite *spr = new (allocation) AnimatedSprite(direction.words, (uint32)VFX_EXP_FIRE, 512, 1, 1, 0xFFFFFFFF);
                spr->depth_bias = 64;
                ++spr_idx;
            }
        }
    }
    period = sound_period;
    if (((period != -1) && !RemainderMotion(game_clock.tick, (uint32)period)))
        UpdatePositionedSoundVoices(sound_voices, model.pos.words, 0);
    result = this->control.flags & 4;
    contacted = 0;
    if (result)
    {
        ModelInstance *player = visual_lists.players;
        result = player != 0;
        if (player)
        {
            uint32 px = player->pos.words[0], py = player->pos.words[1], pz = player->pos.words[2];
            uint32 x = this->model.pos.words[0], rx = push_extent.words[0], z, rz, y, ry;
            result = x - rx;
            if (!((sint32)px < (sint32)(x + rx)))
                return result;
            result = (sint32)(x - rx) < (sint32)px;
            if (!result)
                return result;
            z = this->model.pos.words[2];
            rz = push_extent.words[2];
            result = z - rz;
            if (!((sint32)pz < (sint32)(z + rz)))
                return result;
            result = (sint32)(z - rz) < (sint32)pz;
            if (!result)
                return result;
            y = this->model.pos.words[1];
            ry = push_extent.words[1];
            result = y - ry;
            if (!((sint32)py < (sint32)(y + ry)))
                return result;
            result = (sint32)(y - ry) < (sint32)py;
            if (result)
            {
                uint32 vx = motion.velocity.words[0], vz = motion.velocity.words[2];
                uint32 ax = (sint32)vx < 0 ? 0 - vx : vx;
                uint32 az = (sint32)vz < 0 ? 0 - vz : vz;
                result = az;
                if ((sint32)az < (sint32)ax)
                {
                    result = (sint32)vx > 0 ? x + rx + 0x40000 : x - rx - 0x40000;
                    visual_lists.players->pos.words[0] = result;
                }
                else if ((sint32)az > 0)
                {
                    result = (sint32)vz > 0 ? z + rz + 0x40000 : z - rz - 0x40000;
                    visual_lists.players->pos.words[2] = result;
                }
            }
        }
    }
    return result;
}

uint32 ScriptedLevelObject::Damage(uint32 damage, const sint32 *, uint32)
{
    ObjectBorrow object_borrow;
    uint32 count, enabled, remaining, sound;
    sint32 health;
    FUNCTION_MARKER(0x8003C490, "SLUS_003.73");
    count = (uint32)this->control.damage_phase;
    enabled = ai.parameters[1];
    this->control.damage_phase = count + 1;
    if (enabled)
    {
        health = this->health;
        remaining = (uint16)this->health - damage;
        if (health > 0)
        {
            this->health = (sint16)(uint16)(remaining);
            if ((sint32)(remaining << 16) > 0)
                SetColor(3, 255, 255, 255);
            else
            {
                Finish();
                sound = RandomRange(2) + 1;
                PlayPositionedSound((sint32)sound, this->model.pos.words, 0);
                FragmentLevelObject(&model, 0, 1);
                this->model.flags = this->model.flags | 1;
            }
        }
    }
    return 1;
}

uint32 ScriptedLevelObject::IntegrateMotion()
{
    uint32 mode, result;
    FUNCTION_MARKER(0x8003C558, "SLUS_003.73");
    surface.n.vx = zero_angles.x;
    surface.n.vy = zero_angles.y;
    surface.n.vz = zero_angles.z;
    mode = ai.parameters[2];
    if (mode)
    {
        result = ((mode - 1) & 65535) < 2;
        if (!result)
            return result;
    }
    AddVectors(motion.velocity.words, motion.acceleration.words);
    if (mode)
        DampVector(motion.velocity.words, motion.drag);
    SnapSmallVector(motion.velocity.words);
    AddVectors(this->model.pos.words, motion.velocity.words);
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    NormalizeAngles(model.angles.words);
    AddAngleVector(motion.angular_velocity.words, motion.angular_acceleration.words);
    if (mode)
        DampAngles(motion.angular_velocity.words, motion.angular_drag);
    return SnapSmallAngles(motion.angular_velocity.words);
}

uint32 ScriptedLevelObject::SetDestination(const uint32 *position)
{
    uint32 result = 0, axis, first, second;
    Vector3 coordinates;
    FUNCTION_MARKER(0x8003C684, "SLUS_003.73");
    for (axis = 0; axis < 3; ++axis)
    {
        first = this->destination.words[axis];
        second = position[axis];
        if (first != second)
        {
            result = 1;
            break;
        }
    }
    if (result)
    {
        coordinates.words[0] = position[0];
        coordinates.words[1] = position[1];
        coordinates.words[2] = position[2];
        this->destination = coordinates;
        result = this->control.flags | 1;
        this->control.flags = result;
    }
    return result;
}

uint32 ScriptedLevelObject::ActivateContact()
{
    FUNCTION_MARKER(0x8003C79C, "SLUS_003.73");
    contacted = 1;
    contact_age = 0;
    return 1;
}

uint32 ScriptedLevelObject::ExecuteCommand(uint32 command)
{
    ObjectBorrow object_borrow;
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8003C7B4, "SLUS_003.73");
    uint32 value, duration, axis, step, shift = 12;
    Vector3 position;
    RecordCursor cursor;
    uint32 sound, x, y, z, flags;
    command = (uint16)command;
    switch (command)
    {
        case 0x4303:
            contact_damage = ReadCommandWord();
            return 1;
        case 0x4305:
            value = ReadCommandWord();
            this->model.flags = value ? this->model.flags | 8 : this->model.flags & 0xFFF7;
            return 1;
        case 0x4304:
        case 0x4306:
        case 0x4307:
        case 0x4308:
            flags = this->model.flags;
            if (!(flags & 0x200))
            {
                this->model.flags = flags | 0x200;
                this->model.scale.words[2] = 4096;
                this->model.scale.words[1] = 4096;
                this->model.scale.words[0] = 4096;
            }
            value = ReadCommandWord();
            duration = ReadCommandWord();
            axis = command == 0x4304 ? 0 : command - 0x4306;
            scale_ticks[axis] = duration;
            if (command == 0x4304)
            {
                scale_ticks[2] = duration;
                scale_ticks[1] = duration;
            }
            step = (uint32)(((sint32)(value << 16) >> 4) / (sint32)(duration ? 100 * scale_ticks[axis] : 100));
            scale_step.words[axis] = step;
            if (command == 0x4304)
            {
                scale_step.words[2] = step;
                scale_step.words[1] = step;
                x = this->model.scale.words[0];
                step = scale_step.words[0];
                y = scale_step.words[1];
                z = scale_step.words[2];
                this->model.scale.words[0] = x + step;
                x = this->model.scale.words[1];
                value = this->model.scale.words[2];
                this->model.scale.words[1] = x + y;
                this->model.scale.words[2] = value + z;
            }
            else
                this->model.scale.words[axis] = this->model.scale.words[axis] + scale_step.words[axis];
            return 1;
        case 0x4507:
            sound = sound_voices;
            if (sound)
                StopSoundVoices(sound);
            value = ReadCommandWord();
            sound_voices = PlaySoundEffect(value, 8192, 0);
            sound_period = 0xFFFFFFFF;
            return 1;
        case 0x4508:
            sound = sound_voices;
            if (sound)
                StopSoundVoices(sound);
            value = ReadCommandWord();
            duration = ReadCommandWord();
            sound_period = duration;
            sound_voices = PlayPositionedSound(value, this->model.pos.words, 0);
            return 1;
        case 0x4509:
            sound = sound_voices;
            if (sound)
                StopSoundVoices(sound);
            sound_voices = 0;
            return 1;
        case 0x4300:
            if (contacted)
                return 1;
            script_cursor = script_cursor - 2;
            return 0;
        case 0x4301:
            if (static_cast<Player *>(visual_lists.players->owner)->ground.GetObject() == &model)
                static_cast<Player *>(visual_lists.players->owner)->ground.Reset();
            value = this->model.flags;
            this->model.flags = value | 1;
            Finish();
            FragmentLevelObject(&model, 0, 0);
            return 0;
        case 0x4302:
            cursor = script_cursor;
            x = cursor.Read16();
            script_cursor = cursor + 2;
            y = (cursor + 2).Read16();
            script_cursor = cursor + 4;
            z = (cursor + 4).Read16();
            flags = this->control.flags;
            script_cursor = cursor + 6;
            this->control.flags = flags | 4;
            push_extent.words[0] = x << 11;
            push_extent.words[1] = y << 11;
            push_extent.words[2] = z << 11;
            return 1;
        case 0x4220:
        case 0x4222:
            cursor = AlignScriptCursor(script_cursor);
            position.words[0] = cursor.Read32();
            position.words[1] = (cursor + 4).Read32();
            position.words[2] = (cursor + 8).Read32();
            ShiftVectorLeft(position.words, shift);
            if (command == 0x4222)
                AddVectors(position.words, this->model.pos.words);
            script_cursor = cursor + 12;
            SetDestination(position.words);
            return 1;
        case 0x4221:
            value = ReadCommandWord();
            if (value & 0x2000)
            {
                value = ResolveParameter(value);
            }
            ReadRecordPosition(position.words, (uint16)value);
            SetDestination(position.words);
            return 1;
        case 0x4205:
            Finish();
            return 0;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 ScriptedLevelObject::SetScriptParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, bank, flags, first, second, result;
    uint16 *destination;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8003CF3C, "SLUS_003.73");
    command &= 65535;
    switch (command)
    {
        case 0x2123:
            cursor = script_cursor;
            value = (cursor)[0];
            script_cursor = cursor + 2;
            result = 0;
            this->control.gravity_enabled = value;
            return result;
        case 0x2124:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2;
            result = 0;
            this->model.model_idx = value;
            return result;
        case 0x212F:
            cursor = script_cursor;
            bank = this->model.bank;
            cursor = AlignScriptCursor(cursor);
            value = cursor.Read32();
            value = FindModelResourceIndex(value, bank);
            this->model.model_idx = value;
            flags = this->model.flags;
            script_cursor = cursor + 4;
            result = flags & 65534;
            this->model.flags = result;
            return result;
        case 0x2134:
            first = ReadScriptValue();
            second = ReadScriptValue();
            result = ReadScriptValue();
            first = (uint32)((sint32)(first << 16) >> 4);
            second = (uint32)((sint32)(second << 16) >> 4);
            result = (uint32)((sint32)(result << 16) >> 4);
            motion.velocity.words[0] = first;
            motion.velocity.words[1] = second;
            motion.velocity.words[2] = result;
            return result;
        case 0x2127:
        case 0x2128:
        case 0x2137:
            first = ReadScriptValue();
            second = ReadScriptValue();
            result = ReadScriptValue();
            destination = command == 0x2127 ? motion.angular_velocity.words : command == 0x2128 ? motion.angular_acceleration.words : model.angles.words;
            destination[0] = first;
            destination[1] = second;
            destination[2] = result;
            return result;
        default:
            return Enemy::ReadParameters(command);
    }
}

uint32 ScriptedLevelObject::ResolveParameter(uint32 command)
{
    FUNCTION_MARKER(0x8003D0F0, "SLUS_003.73");
    command &= 65535;
    if (command == 0x2200)
        return contact_age;
    return (uint32)(sint32)(sint16)Enemy::ResolveParameter(command);
}
