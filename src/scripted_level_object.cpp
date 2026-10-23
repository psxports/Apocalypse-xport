#include <new>
#include <stdlib.h>

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
#include "global.h"
#include "model_bank.h"
#include "record_data.h"
#include "collision.h"
#include "angle_math.h"
#include "model_render.h"
#include "resource_script.h"
#include "model_projection.h"
#include "player.h"
#include "animated_sprite.h"
#include "effect_lifecycle.h"
#include "enemy.h"
#include "effect_object.h"

static bool IsMotionSoundTick(sint32 tick, sint32 period)
{
    if (!period)
        return tick == 0;
    return tick % period == 0;
}

ScriptedLevelObject::ScriptedLevelObject(const RecordCursor &data, uint16 idx) : Enemy(), contact_damage(0), contacted(false), contact_age(0), sound_voices(0), sound_period(-1)
{
    RecordDataBorrow rec_borrow;
    ObjectBorrow object_borrow;
    uint32 flags;
    RecordCursor cursor;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        scale_ticks[axis] = 0;
        scale_step.comp[axis] = 0;
        push_extent.comp[axis] = 0;
    }

    FUNCTION_MARKER(0x8003B964, "SLUS_003.73");
    SetModelBank(GetRecModelBankName());
    AddToList(visual_lists.solids);
    flags = model.flags;
    kind = 402;
    model.flags = (flags | 273) & 65533;
    cursor = ReadPos(data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    ai.parameters[0] = 32;
    rec_idx = idx;
    control.script_active = 1;
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
    return (reason & 1) ? ReleaseStorage() : 0;
}

void ScriptedLevelObject::ApplyPlayerDamage()
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8003BB60, "SLUS_003.73");
    if (!contact_damage)
        return;
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    if (contact_damage < 0)
        player.RestoreHealth(-contact_damage);
    else
        player.Damage(contact_damage, &zero_vector, 0);
}

uint32 ScriptedLevelObject::Update()
{
    ObjectBorrow object_borrow;
    RecordDataBorrow rec_borrow;
    uint16 activation, delay, move_mode;
    uint8 activation_count;
    sint32 distance, turn_scale, yaw_delta;
    sint16 roll_delta;
    uint32 playing_voices, stopped_voices, spr_idx;
    sint32 spr_angle, spr_radius;
    sint32 period, result;
    Vector3 dir, motion_delta;
    ShortVector3 target_angles;
    FUNCTION_MARKER(0x8003BBD8, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        if (scale_ticks[axis])
        {
            --scale_ticks[axis];
            model.scale.comp[axis] = (uint16)model.scale.comp[axis] + ((uint16)scale_step.comp[axis]);
        }
    }
    motion.acceleration = zero_vector;
    if (!contacted)
        ++contact_age;
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        activation_count = control.activation_count;
        activation_flags = activation & 0xFFFE;
        control.activation_count = activation_count + 1;
    }
    if (control.script_active)
    {
        delay = control.script_delay;
        if (delay)
            control.script_delay = delay - 1;
        else
            ExecuteScript(script_cursor);
    }
    if (!(control.flags & 1))
    {
        if (ai.parameters[2] == 1)
        {
            roll_delta = -(uint16)model.angles.z;
            if (roll_delta < -2048)
                roll_delta += 4096;
            if (roll_delta > 2048)
                roll_delta -= 4096;
            if (roll_delta)
                motion.angular_acceleration.z = roll_delta >> 3;
            else
                motion.angular_velocity.z = 0;
        }
    }
    else
    {
        dir.Subtract(model.pos, dst);
        distance = dir.Len();
        if (ai.parameters[0] < distance)
        {
            move_mode = ai.parameters[2];
            if (!move_mode)
            {
                target_angles.SetDir(model.pos, dst);
                motion.velocity.SetDir(ai.parameters[0], target_angles);
            }
            else if (move_mode == 1 || move_mode == 2)
            {
                target_angles.SetDir(model.pos, dst);
                motion_delta.Subtract(dst, model.pos);
                turn_scale = DivideSignedWord(8160, motion_delta.Len());
                if (turn_scale < -256)
                    turn_scale = -256;
                else if (turn_scale > 256)
                    turn_scale = 256;
                else if (turn_scale >= 1 && turn_scale <= 3)
                    turn_scale = 4;
                else if (turn_scale < 0 && turn_scale >= -3)
                    turn_scale = -4;
                UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, target_angles, turn_scale);
                model.angles.x = 0;
                motion.acceleration.SetDir(ai.parameters[0], target_angles);
                if (move_mode == 1)
                {
                    yaw_delta = control.flags & 0x20 ? ((uint16)target_angles.y - (uint16)model.angles.y) & 0xFFF : 0;
                    if (yaw_delta > 2048)
                        yaw_delta -= 4096;
                    if (yaw_delta > 1024)
                        yaw_delta = 1024;
                    else if (yaw_delta < -1024)
                        yaw_delta = -1024;
                    roll_delta = ((-yaw_delta >> 1) & 0xFFF) - (uint16)model.angles.z;
                    if (roll_delta < -2048)
                        roll_delta += 4096;
                    if (roll_delta > 2048)
                        roll_delta -= 4096;
                    if (roll_delta)
                        motion.angular_acceleration.z = roll_delta >> 3;
                    else
                        motion.angular_velocity.z = 0;
                }
            }
        }
        else
        {
            model.pos = dst;
            if (ai.parameters[2] < 2)
                motion.velocity = zero_vector;
            control.flags &= ~1;
        }
    }
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    if (ai.parameters[2] == 2 && visual_lists.players)
    {
        distance = model.pos.HorizontalDistance(visual_lists.players->pos);
        if (distance >= ai.parameters[3])
        {
            stopped_voices = sound_voices;
            control.flags &= ~8;
            if (stopped_voices)
            {
                StopSoundVoices(stopped_voices);
                sound_voices = 0;
            }
        }
        else
        {
            if (!(control.flags & 8))
                SpawnEnemyIndices(GetLevelRec(rec_idx) + 6);
            playing_voices = sound_voices;
            control.flags |= 8;
            if (!playing_voices)
                sound_voices = PlaySoundEffect(11, 8192, 0);
            spr_idx = 0;
            static_cast<Player *>(visual_lists.players->owner)->Damage(2, &zero_vector, 0);
            while (spr_idx < RandomRange(4))
            {
                spr_angle = RandomRange(4096);
                spr_radius = RandomRange(ai.parameters[3] >> 1);
                dir.x = rsin(spr_angle) * spr_radius;
                dir.z = rcos(spr_angle) * spr_radius;
                dir.y = 0;
                dir.Add(visual_lists.players->pos);
                dir.y = model.pos.y;
                GameHeapBlock *allocation = EffectObject::AllocateStorage(120);
                if (!allocation)
                {
                    // TODO Original particle allocation failure writes through null
                    GameDiagnostic(("TODO ScriptedLevelObject particle allocation failed\n"));
                    abort();
                }
                AnimatedSprite *spr = new (allocation) AnimatedSprite(&dir, VFX_EXP_FIRE, 512, 1, 1, -1);
                spr->depth_bias = 64;
                ++spr_idx;
            }
        }
    }
    period = sound_period;
    if (period != -1 && IsMotionSoundTick(game_clock.tick, period))
        UpdatePositionedSoundVoices(sound_voices, &model.pos, 0);
    result = control.flags & 4;
    contacted = false;
    if (result)
    {
        ModelInstance *player = visual_lists.players;
        result = player != 0;
        if (player)
        {
            const Vector3 player_pos = player->pos;
            uint32 x = (uint32)model.pos.x, rx = (uint32)push_extent.x, z, rz, y, ry;
            sint32 upper = x + rx;
            result = x - rx;
            if (player_pos.x >= upper)
                return result;
            result = result < player_pos.x;
            if (!result)
                return result;
            z = (uint32)model.pos.z;
            rz = (uint32)push_extent.z;
            upper = z + rz;
            result = z - rz;
            if (player_pos.z >= upper)
                return result;
            result = result < player_pos.z;
            if (!result)
                return result;
            y = (uint32)model.pos.y;
            ry = (uint32)push_extent.y;
            upper = y + ry;
            result = y - ry;
            if (player_pos.y >= upper)
                return result;
            result = result < player_pos.y;
            if (result)
            {
                uint32 vx = (uint32)motion.velocity.x, vz = (uint32)motion.velocity.z;
                // Unsigned negation retains the original INT_MIN magnitude word
                sint32 ax = motion.velocity.x < 0 ? -vx : vx;
                sint32 az = motion.velocity.z < 0 ? -vz : vz;
                result = az;
                if (az < ax)
                {
                    result = motion.velocity.x > 0 ? x + rx + 0x40000 : x - rx - 0x40000;
                    visual_lists.players->pos.x = result;
                }
                else if (az > 0)
                {
                    result = motion.velocity.z > 0 ? z + rz + 0x40000 : z - rz - 0x40000;
                    visual_lists.players->pos.z = result;
                }
            }
        }
    }
    return result;
}

uint32 ScriptedLevelObject::Damage(uint32 damage, const Vector3 *, uint32)
{
    ObjectBorrow object_borrow;
    uint8 count;
    uint16 enabled;
    uint32 remaining;
    sint32 sound;
    FUNCTION_MARKER(0x8003C490, "SLUS_003.73");
    count = control.damage_phase;
    enabled = ai.parameters[1];
    control.damage_phase = count + 1;
    if (enabled)
    {
        if (health > 0)
        {
            remaining = health - damage;
            health = remaining;
            if (health > 0)
                SetColor(3, 255, 255, 255);
            else
            {
                Finish();
                sound = RandomRange(2) + 1;
                PlayPositionedSound(sound, &model.pos, 0);
                model.Fragment(false, true);
                model.flags = model.flags | 1;
            }
        }
    }
    return 1;
}

uint32 ScriptedLevelObject::IntegrateMotion()
{
    uint16 mode;
    FUNCTION_MARKER(0x8003C558, "SLUS_003.73");
    surface.n.vx = zero_angles.x;
    surface.n.vy = zero_angles.y;
    surface.n.vz = zero_angles.z;
    mode = ai.parameters[2];
    if (mode > 2)
        return 0;
    motion.velocity.Add(motion.acceleration);
    if (mode)
        motion.velocity.Damp(motion.drag);
    motion.velocity.SnapSmall();
    model.pos.Add(motion.velocity);
    model.angles.AddAngles(motion.angular_velocity);
    model.angles.NormalizeAngles();
    motion.angular_velocity.AddAngles(motion.angular_acceleration);
    if (mode)
        motion.angular_velocity.DampAngles(motion.angular_drag);
    return motion.angular_velocity.SnapAngles();
}

uint32 ScriptedLevelObject::SetDst(const Vector3 &pos)
{
    FUNCTION_MARKER(0x8003C684, "SLUS_003.73");
    uint32 result = dst.Differs(pos);
    if (result)
    {
        dst = pos;
        result = control.flags | 1;
        control.flags = result;
    }
    return result;
}

uint32 ScriptedLevelObject::ActivateContact()
{
    FUNCTION_MARKER(0x8003C79C, "SLUS_003.73");
    contacted = true;
    contact_age = 0;
    return 1;
}

uint32 ScriptedLevelObject::ExecuteCommand(uint16 command)
{
    ObjectBorrow object_borrow;
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8003C7B4, "SLUS_003.73");
    uint16 val, duration, x, y, z;
    sint16 scale_delta, step;
    sint32 scale_divisor;
    uint32 axis;
    Vector3 pos;
    RecordCursor cursor;
    uint32 sound, flags;
    switch (command)
    {
        case 0x4303:
            contact_damage = ReadCommandWord();
            return 1;
        case 0x4305:
            val = ReadCommandWord();
            model.flags = val ? model.flags | 8 : model.flags & 0xFFF7;
            return 1;
        case 0x4304:
        case 0x4306:
        case 0x4307:
        case 0x4308:
            flags = model.flags;
            if (!(flags & 0x200))
            {
                model.flags = flags | 0x200;
                model.scale.z = ONE;
                model.scale.y = ONE;
                model.scale.x = ONE;
            }
            scale_delta = ReadCommandWord();
            duration = ReadCommandWord();
            axis = command == 0x4304 ? 0 : command - 0x4306;
            scale_ticks[axis] = duration;
            if (command == 0x4304)
            {
                scale_ticks[2] = duration;
                scale_ticks[1] = duration;
            }
            scale_divisor = duration ? 100 * scale_ticks[axis] : 100;
            step = scale_delta * ONE / scale_divisor;
            scale_step.comp[axis] = step;
            if (command == 0x4304)
            {
                scale_step.z = step;
                scale_step.y = step;
                for (uint32 i = 0; i < 3; ++i)
                    model.scale.comp[i] = (uint16)model.scale.comp[i] + step;
            }
            else
                model.scale.comp[axis] = (uint16)model.scale.comp[axis] + step;
            return 1;
        case 0x4507:
            sound = sound_voices;
            if (sound)
                StopSoundVoices(sound);
            val = ReadCommandWord();
            sound_voices = PlaySoundEffect(val, 8192, 0);
            sound_period = -1;
            return 1;
        case 0x4508:
            sound = sound_voices;
            if (sound)
                StopSoundVoices(sound);
            val = ReadCommandWord();
            duration = ReadCommandWord();
            sound_period = duration;
            sound_voices = PlayPositionedSound(val, &model.pos, 0);
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
            val = model.flags;
            model.flags = val | 1;
            Finish();
            model.Fragment(false, false);
            return 0;
        case 0x4302:
            cursor = script_cursor;
            x = cursor.Read16();
            script_cursor = cursor + 2;
            y = (cursor + 2).Read16();
            script_cursor = cursor + 4;
            z = (cursor + 4).Read16();
            flags = control.flags;
            script_cursor = cursor + 6;
            control.flags = flags | 4;
            push_extent.x = x << 11;
            push_extent.y = y << 11;
            push_extent.z = z << 11;
            return 1;
        case 0x4220:
        case 0x4222:
            cursor = script_cursor.Aligned(4);
            pos.x = cursor.Read32();
            pos.y = (cursor + 4).Read32();
            pos.z = (cursor + 8).Read32();
            pos.ShiftLeft(12);
            if (command == 0x4222)
                pos.Add(model.pos);
            script_cursor = cursor + 12;
            SetDst(pos);
            return 1;
        case 0x4221:
            val = ReadCommandWord();
            if (val & 0x2000)
            {
                val = ResolveParameter(val);
            }
            ReadRecPos(&pos, val);
            SetDst(pos);
            return 1;
        case 0x4205:
            Finish();
            return 0;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

void ScriptedLevelObject::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint32 key, bank;
    uint16 model_idx, flags;
    uint8 gravity;
    ShortVector3 values;
    ShortVector3 *angles;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8003CF3C, "SLUS_003.73");
    switch (command)
    {
        case 0x2123:
            cursor = script_cursor;
            gravity = cursor.Read8();
            script_cursor = cursor + 2;
            control.gravity_enabled = gravity;
            return;
        case 0x2124:
            cursor = script_cursor;
            model_idx = cursor.Read16();
            script_cursor = cursor + 2;
            model.model_idx = model_idx;
            return;
        case 0x212F:
            cursor = script_cursor;
            bank = model.bank;
            cursor = cursor.Aligned(4);
            key = cursor.Read32();
            model_idx = ModelBank::FindResourceIdx(key, bank);
            model.model_idx = model_idx;
            flags = model.flags;
            script_cursor = cursor + 4;
            model.flags = flags & 0xFFFE;
            return;
        case 0x2134:
            values.x = ReadScriptVal();
            values.y = ReadScriptVal();
            values.z = ReadScriptVal();
            motion.velocity.x = values.x * ONE;
            motion.velocity.y = values.y * ONE;
            motion.velocity.z = values.z * ONE;
            return;
        case 0x2127:
        case 0x2128:
        case 0x2137:
            values.x = ReadScriptVal();
            values.y = ReadScriptVal();
            values.z = ReadScriptVal();
            angles = command == 0x2127 ? &motion.angular_velocity : command == 0x2128 ? &motion.angular_acceleration : &model.angles;
            *angles = values;
            return;
        default:
            Enemy::ReadParameters(command);
            return;
    }
}

uint32 ScriptedLevelObject::ResolveParameter(uint16 command)
{
    FUNCTION_MARKER(0x8003D0F0, "SLUS_003.73");
    if (command == 0x2200)
        return contact_age;
    return (uint32)(sint16)Enemy::ResolveParameter(command);
}
