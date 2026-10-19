#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include "level_session.h"
#include "game_settings.h"

#include "pickup_factory.h"
#include "pickup.h"

#include "object_initializers.h"

#include "visual_object.h"
#include "cd_voice_event.h"
#include "game_panel.h"
#include "screen_flash.h"
#include "game_diagnostics.h"

#include "character.h"
#include "object_damage.h"
#include "game_heap.h"
#include "model_resources.h"
#include "game_object_allocation.h"

#include "enemy_list.h"

#include "enemy.h"
#include "explosion_effects.h"
#include "vector_arithmetic.h"
#include "angle_math.h"

#include "game_random.h"
#include "record_data.h"
#include "record_commands.h"
#include "vector_distance.h"
#include "collision.h"

#include "movie_lifecycle.h"

#include "audio_session.h"
#include "particle_settings.h"
#include "fading_particle.h"
#include "fire_cycle.h"
#include "camera.h"
#include "effect_lifecycle.h"

static uint32 enemy_count;

void ResetEnemyPopulation()
{
    enemy_count = 0;
}

uint32 Enemy::ReadCommandWord()
{
    RecordDataBorrow record_borrow;
    uint32 value = script_cursor.Read16();
    script_cursor += 2;
    return value;
}

uint32 Enemy::ReadCommandValue()
{
    uint32 value = ReadCommandWord();
    return (value & 0x2000) ? ResolveParameter(value) & 0xFFFF : value;
}

sint32 Enemy::ReadSignedCommandValue()
{
    uint32 value = ReadCommandWord();
    return (value & 0x2000) ? (sint16)ResolveParameter(value) : (sint32)value;
}

uint32 Enemy::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004B868, "SLUS_003.73");
    uint32 count = enemy_count;
    enemy_count = count - 1;
    uint32 result = Character::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}

void Enemy::NoOp()
{
    FUNCTION_MARKER(0x800639A4, "SLUS_003.73");
}

void Enemy::SetParameters(uint32 command)
{
    ReadParameters(command);
}

Enemy::Enemy() : Character(), script_cursor(0), ground_offset(0), sight_offset(0), behavior(0), behavior_rows(0), behavior_cols(0)
{
    RecordDataBorrow record_borrow;
    uint32 count;
    FUNCTION_MARKER(0x8004B800, "SLUS_003.73");
    count = enemy_count;
    this->record_idx = 65535;
    this->collision_radius = 128;
    this->control.height_limit = 32;
    this->control.script_delay = 0;
    contact_strength = 64;
    destination.words[0] = destination.words[1] = destination.words[2] = 0;
    for (uint32 index = 0; index < 8; ++index)
        script_labels[index] = 0;
    enemy_count = count + 1;
}

Enemy::~Enemy()
{
    free(behavior);
}

uint32 Enemy::Finish()
{
    uint32 flags, state;
    FUNCTION_MARKER(0x8004B8C8, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        ActivateLinkedRecords();
        flags = this->lifecycle_flags;
        state = this->model.flags;
        flags |= 64;
        state |= 1;
        this->lifecycle_flags = flags;
        this->model.flags = state;
        result = flags;
    }
    return result;
}

uint32 ScaleDamageForDifficulty(uint32 damage)
{
    FUNCTION_MARKER(0x8004B914, "SLUS_003.73");
    uint32 difficulty = game_settings.difficulty;
    if (difficulty == 1)
        return damage;
    if (difficulty)
        return damage << 1;
    return damage - (uint32)((sint32)damage >> 2);
}

uint32 Enemy::ActivateLinkedRecords()
{
    RecordDataBorrow record_borrow;
    uint32 index;
    FUNCTION_MARKER(0x8004B948, "SLUS_003.73");
    if ((uint32)this->control.linked_records_activated)
        return 65535;
    index = this->record_idx;
    if (index == 65535)
        return 1;
    this->control.linked_records_activated = 1;
    return SpawnEnemyIndices(GetLevelRecord(index) + 6);
}

uint32 Enemy::IntegrateMotion()
{
    FUNCTION_MARKER(0x8004B9A4, "SLUS_003.73");
    Vector3 &velocity = motion.velocity;
    surface.n.vx = zero_angles.x;
    surface.n.vy = zero_angles.y;
    surface.n.vz = zero_angles.z;
    this->contact_flags = 0;
    AddVectors(velocity.words, motion.acceleration.words);
    DampVector(velocity.words, motion.drag);
    SnapSmallVector(velocity.words);
    AddVectors(this->model.pos.words, velocity.words);
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    NormalizeAngles(this->model.angles.words);
    AddAngleVector(motion.angular_velocity.words, motion.angular_acceleration.words);
    DampAngles(motion.angular_velocity.words, motion.angular_drag);
    return SnapSmallAngles(motion.angular_velocity.words);
}

uint32 Enemy::ChoosePosition(uint32 *output, const uint32 *target, uint32 minimum, uint32 maximum, uint32 height_limit, uint32 spread, uint32 ground, uint32 check_collision)
{
    uint32 trig_angle;
    FUNCTION_MARKER(0x8004BA64, "SLUS_003.73");
    uint32 angle, radius, delta, sign, absolute, left, right, sine, offset;
    sint32 height;
    CollisionRay collision;
    uint32 pass;
    left = model.pos.words[2];
    right = target[2];
    delta = left - right;
    left = model.pos.words[0];
    right = target[0];
    angle = 1024 - (uint32)ratan2((sint32)delta, (sint32)(left - right));
    angle += RandomRange((uint32)(sint32)(sint16)spread);
    angle -= (uint32)((sint32)(sint16)spread >> 1);
    radius = minimum + RandomRange(maximum - minimum);
    trig_angle = angle & 4095;
    sine = (uint32)rsin(trig_angle);
    left = target[0];
    output[0] = left + radius * sine;
    left = target[1];
    output[1] = left;
    sine = (uint32)rcos(trig_angle);
    left = target[2];
    output[2] = left + radius * sine;
    if (ground != 0)
    {
        height = (sint32)FindGroundHeight(output, 512, 512);
        if (height == -1)
            return 0;
        offset = (uint32)(sint32)(sint16)ground_offset << 12;
        output[1] = (uint32)height - offset;
    }
    if (check_collision != 0)
    {
        for (pass = 0; pass < 2; ++pass)
        {
            collision.endpoints[0].words[0] = model.pos.words[0];
            collision.endpoints[0].words[1] = model.pos.words[1];
            collision.endpoints[0].words[2] = model.pos.words[2];
            if (!pass)
            {
                offset = (uint32)(sint32)(sint16)sight_offset << 12;
                collision.endpoints[0].words[1] -= offset;
            }
            collision.endpoints[1].words[0] = output[0];
            collision.endpoints[1].words[1] = output[1];
            collision.endpoints[1].words[2] = output[2];
            if (!pass)
            {
                offset = (uint32)(sint32)(sint16)sight_offset << 12;
                collision.endpoints[1].words[1] -= offset;
            }
            InitializeCollisionRay(collision);
            collision.triggers = 0;
            collision_query.allow_surface20 = true;
            QueryCollisionRay(collision, 1);
            ModelInstance *hit = collision.hit.GetObject();
            collision_query.allow_surface20 = false;
            if (hit != 0)
                return 0;
        }
    }
    left = model.pos.words[1];
    right = output[1];
    delta = left - right;
    sign = (uint32)((sint32)delta >> 31);
    absolute = (delta ^ sign) - sign;
    return (uint32)(((sint32)absolute >> 12) < (sint32)height_limit);
}

void Enemy::AllocateBehaviorTable(uint32 rows, uint32 columns)
{
    FUNCTION_MARKER(0x8004BD04, "SLUS_003.73");
    free(behavior);
    behavior = 0;
    behavior_rows = rows;
    behavior_cols = columns;
    uint32 count = (uint32)behavior_rows * behavior_cols;
    if (count)
    {
        behavior = (uint16 *)calloc(count, sizeof(*behavior));
        if (!behavior)
        {
            GameDiagnostic("Cannot allocate enemy behavior table %u/%u\n", (uint32)behavior_rows, (uint32)behavior_cols);
            abort();
        }
    }
}

void Enemy::FreeBehaviorTable()
{
    FUNCTION_MARKER(0x8004BD94, "SLUS_003.73");
    free(behavior);
    behavior = 0;
    behavior_rows = 0;
    behavior_cols = 0;
}

RecordCursor Enemy::AlignScriptCursor(RecordCursor cursor)
{
    return cursor.Aligned(4);
}

RecordCursor Enemy::GetScriptLabel(uint32 index) const
{
    if (index >= 8)
    {
        GameDiagnostic("Enemy script label index out of bounds: %u\n", index);
        abort();
    }
    return script_labels[index];
}

void Enemy::SetScriptLabel(uint32 index, const RecordCursor &script)
{
    if (index >= 8)
    {
        GameDiagnostic("Enemy script label index out of bounds: %u\n", index);
        abort();
    }
    script_labels[index] = script;
}

uint16 Enemy::ReadRegister(uint32 index) const
{
    if (index < 6)
        return ai.work[index];
    switch (index)
    {
        case 6:
            return ai.state;
        case 7:
            return ai.phase;
        case 8:
            return control.script_delay;
        case 9:
            return ai.script_register9;
        case 10:
            return ai.script_data;
        case 11:
            return ai.script_data >> 16;
        default:
            if (index < 18)
                return destination.words[(index - 12) / 2] >> ((index & 1) * 16);
            GameDiagnostic("Enemy script register index out of bounds: %u\n", index);
            abort();
    }
}

void Enemy::WriteRegister(uint32 index, uint32 value)
{
    if (index < 6)
    {
        ai.work[index] = value;
        return;
    }
    switch (index)
    {
        case 6:
            ai.state = value;
            return;
        case 7:
            ai.phase = value;
            return;
        case 8:
            control.script_delay = value;
            return;
        case 9:
            ai.script_register9 = value;
            return;
        case 10:
            ai.script_data = (ai.script_data & 0xFFFF0000) | (uint16)value;
            return;
        case 11:
            ai.script_data = (ai.script_data & 0xFFFF) | (value << 16);
            return;
        default:
            if (index < 18)
            {
                uint32 &word = destination.words[(index - 12) / 2];
                if (index & 1)
                    word = (word & 0xFFFF) | (value << 16);
                else
                    word = (word & 0xFFFF0000) | (uint16)value;
                return;
            }
            GameDiagnostic("Enemy script register index out of bounds: %u\n", index);
            abort();
    }
}

void Enemy::WriteAIParameter(uint32 index, uint32 value)
{
    if (index < 10)
        ai.parameters[index] = value;
    else if (index == 10)
        ground_offset = value;
    else if (index == 11)
        sight_offset = value;
    else
        WriteRegister(index - 12, value);
}

uint32 Enemy::GetBehaviorIndex(uint32 row, uint32 column) const
{
    uint32 idx = (row & 255) * behavior_cols + (column & 255);
    if (!behavior || idx >= (uint32)behavior_rows * behavior_cols)
    {
        GameDiagnostic("Enemy behavior index %u/%u exceeds %u/%u\n", row & 255, column & 255, (uint32)behavior_rows, (uint32)behavior_cols);
        abort();
    }
    return idx;
}

void Enemy::SetBehaviorValue(uint32 row, uint32 column, uint32 value)
{
    FUNCTION_MARKER(0x8004BDCC, "SLUS_003.73");
    behavior[GetBehaviorIndex(row, column)] = value;
}

uint32 Enemy::GetBehaviorValue(uint32 row, uint32 column)
{
    FUNCTION_MARKER(0x8004BDFC, "SLUS_003.73");
    return behavior[GetBehaviorIndex(row, column)];
}

uint32 Enemy::SkipConditionalBlock()
{
    RecordDataBorrow record_borrow;
    uint32 depth = 0, command, result;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004BE30, "SLUS_003.73");
    for (;;)
    {
        cursor = script_cursor;
        command = cursor.Read16();
        script_cursor = cursor + 2;
        result = command - 0x4112;
        if (command == 0x4120)
        {
            if (depth-- == 0)
                return result;
        }
        else if ((result & 0xFFFF) < 5)
            ++depth;
        result = command;
        if (result == 0x4100)
            return result;
    }
}

uint32 Enemy::ExecuteScript(const RecordCursor &script)
{
    RecordDataBorrow record_borrow;
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8004BE90, "SLUS_003.73");
    if (!script)
    {
        GameDiagnostic("Enemy script is missing\n");
        abort();
    }
    script_cursor = script;
    for (;;)
    {
        uint32 command = script_cursor.Read16();
        if (command == 0x4100)
        {
            control.script_active = 0;
            // Native callers observe completion rather than the old cursor carrier
            return 1;
        }
        script_cursor += 2;
        if (command & 0x4000)
        {
            uint32 result = ExecuteCommand(command);
            if (!result)
                return result;
        }
        else if (command & 0x2000)
        {
            SetParameters(command);
        }
    }
}

uint32 Enemy::ExecuteCommand(uint32 opcode)
{
    GameHeapBlock *reservation;
    RecordDataBorrow record_borrow;
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8004BF3C, "SLUS_003.73");
    uint32 value, second, third, count, i, flash_mode;
    Pickup *created;
    RecordCursor cursor;
    RecordCursor saved;
    RecordCursor record_payload;
    Vector3 position;
    uint8 *operands;
    ShortVector3 angles;
    sint32 left, right;
    opcode &= 0xFFFF;
    switch (opcode)
    {
        case 0x4101:
        case 0x4102:
            value = ReadCommandWord();
            script_cursor = GetScriptLabel(value);
            return opcode == 0x4101;
        case 0x4104:
            cursor = script_cursor;
            value = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            SetScriptLabel(value, cursor);
            return 1;
        case 0x4105:
            saved = script_cursor;
            for (;;)
            {
                cursor = script_cursor;
                value = cursor.Read16();
                if (value == 0x4100)
                    break;
                script_cursor = cursor + 2;
                if (value == 0x4104)
                {
                    value = (cursor + 2).Read16();
                    script_cursor = cursor + 4;
                    SetScriptLabel(value, cursor + 4);
                }
            }
            script_cursor = saved;
            return 1;
        case 0x4106:
            value = ReadCommandValue();
            cursor = ReadRecordPosition(position.words, value) + 6;
            script_cursor = cursor;
            return 1;
        case 0x4107:
            return 0;
        case 0x4110:
        case 0x4111:
            cursor = script_cursor;
            value = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            left = (sint16)ResolveParameter(value);
            second = cursor.DistanceTo(script_cursor);
            if ((second & 1) || second > 0xFFFFFFFD)
            {
                GameDiagnostic("Invalid arithmetic operand byte count\n");
                abort();
            }
            // Rebuild the variable-length encoded command for the record reader
            operands = (uint8 *)malloc(second + 2);
            if (!operands)
                abort();
            count = 0;
            while (cursor != script_cursor)
            {
                xport_store_le16(operands + count, cursor.Read16());
                count += 2;
                cursor += 2;
            }
            right = ReadSignedCommandValue();
            xport_store_le16(operands + count, (uint16)(opcode == 0x4110 ? left + right : left - right));
            saved = script_cursor;
            script_cursor = RecordCursor(operands, second + 2);
            SetParameters(value);
            script_cursor = saved;
            free(operands);
            return 1;
        case 0x4112:
        case 0x4113:
        case 0x4114:
            left = ReadSignedCommandValue();
            right = ReadSignedCommandValue();
            if ((opcode == 0x4112 && !(right < left)) || (opcode == 0x4113 && !(left < right)) || (opcode == 0x4114 && left != right))
                SkipConditionalBlock();
            return 1;
        case 0x4115:
        case 0x4116:
            value = ReadCommandWord();
            second = this->control.flags & value;
            if ((opcode == 0x4115 && second != value) || (opcode == 0x4116 && second != 0))
                SkipConditionalBlock();
            return 1;
        case 0x4200:
            VisualObject::SetModelBank(script_cursor.CStr());
            goto skip_string;
        case 0x4201:
            cursor = script_cursor;
            value = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            second = cursor.Read16();
            script_cursor = cursor + 2;
            StartAnimation(value, (uint32)(sint32)(sint8)second);
            return 1;
        case 0x4202:
            value = ReadCommandWord();
            SetAnimationRange(value, 0, 0xFFFFFFFF);
            return 1;
        case 0x4203:
            this->model.flags = this->model.flags & 0xFFFE;
            return 1;
        case 0x4204:
            this->model.flags = this->model.flags | 1;
            return 1;
        case 0x4205:
            Finish();
            return 1;
        case 0x4226:
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            motion.velocity.words[0] = 0;
            return 1;
        case 0x4227:
            value = ReadCommandWord();
            if (value)
            {
                second = this->control.flags;
                this->control.ground_period = value;
                this->control.ground_tick = 0;
                this->control.flags = second | 2;
            }
            else
                this->control.flags = this->control.flags & ~2;
            return 1;
        case 0x4240:
            this->control.script_active = 0;
            return 0;
        case 0x4260:
        case 0x4261:
            value = ReadCommandWord();
            SetBehaviorValue(value & 0xFF, 0, opcode == 0x4260);
            return 1;
        case 0x4280:
            value = ReadCommandWord();
            this->control.script_delay = value;
            if (value & 0x2000)
            {
                this->control.script_delay = ResolveParameter(this->control.script_delay);
            }
            return 0;
        case 0x4281:
            if ((uint32)this->control.prev_activation_flags & 1)
                return 1;
            script_cursor = script_cursor - 2;
            return 0;
        case 0x4290:
            value = ReadCommandWord();
            PlaySoundEffect((sint16)value, 8192, 0);
            return 1;
        case 0x4291:
            value = ReadCommandWord();
            PlayPositionedSound((sint16)value, this->model.pos.words, 0);
            return 1;
        case 0x4292:
            cursor = script_cursor;
            second = effect_count;
            count = cursor.Read16();
            script_cursor = cursor + 2;
            if ((sint32)second >= 200)
                return 1;
            SetParticleAngles(this->model.angles.components);
            angles.components[0] = 512;
            angles.components[1] = 4096;
            angles.components[2] = 0;
            SetParticleSpread(angles.components);
            SetParticleSize(1);
            SetParticleColor(128, 128, 128);
            SetParticleColorStep(4, 4, 4);
            use_small_effect_pool = false;
            for (i = 0; i < count; ++i)
            {
                reservation = AllocateEffectStorage(88);
                if (reservation)
                {
                    new (reservation) FadingParticle(this->model.pos.words, 32, 8192, 32);
                }
            }
            use_small_effect_pool = true;
            return 1;
        case 0x4293:
            cursor = AlignScriptCursor(script_cursor);
            ::PrepareMovieTexture(cursor.Read32());
            script_cursor = cursor + 4;
            return 1;
        case 0x4294:
            QueueMoviePosition(model.pos.words);
            record_payload = GetRecordPayload(this->record_idx);
            cursor = record_payload;
            for (i = 0; i < record_payload.Read16(); ++i)
            {
                value = (cursor + 2).Read16();
                cursor += 2;
                ReadRecordPosition(position.words, value);
                QueueMoviePosition(model.pos.words);
            }
            ::PrepareMoviePlaybackBuffers();
            return 1;
        case 0x4295:
            ResetMoviePlayback();
            return 1;
        case 0x4296:
            value = ReadCommandWord();
            ::RestoreMusicPlayback();
            return 1;
        case 0x4297:
            value = ReadCommandWord();
            StopMusicSequence();
            return 1;
        case 0x4298:
            value = ReadCommandValue();
            if (visual_lists.cameras)
                static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(value == 0 ? 2 : value == 1 ? 1 : 0);
            return 1;
        case 0x4299:
            value = ReadCommandWord();
            ::SelectMovieRecord(value & 0xFF);
            return 1;
        case 0x429A:
        case 0x42A6:
            if (opcode == 0x429A)
            {
                value = ReadCommandValue();
                ReadRecordPosition(position.words, value);
            }
            else
            {
                position = this->model.pos;
            }
            value = ReadCommandWord();
            if (value < 2)
                HandleExplosionProximity(model.pos.words, value);
            else if (value == 2)
            {
                second = ReadScriptValue();
                cursor = script_cursor + 2;
                script_cursor = cursor;
                third = cursor.Read16();
                script_cursor = cursor + 2;
                reservation = AllocateEffectStorage(156);
                if (reservation)
                {
                    new (reservation) FireCycle(position.words, (uint16)second, third != 0, (uint16)third);
                }
            }
            return 1;
        case 0x429C:
            cursor = script_cursor;
            value = (cursor)[0];
            second = (cursor + 2)[0];
            third = (cursor + 4)[0];
            count = (cursor + 6).Read16();
            cursor = script_cursor;
            flash_mode = (cursor + 8).Read16();
            ::SetScreenFlash(value, second, third, count, 0, flash_mode);
            script_cursor = script_cursor + 10;
            return 1;
        case 0x429D:
            QueueMoviePosition(model.pos.words);
            RequestCdStreamRefresh();
            return 1;
        case 0x429E:
            level_environment.water_y = this->model.pos.words[1];
            return 1;
        case 0x42A0:
            value = ReadCommandValue();
            cursor = script_cursor;
            second = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            third = cursor.Read16();
            script_cursor = cursor + 2;
            count = (cursor + 2).Read16();
            script_cursor = cursor + 4;
            created = ::SpawnPickup(value, model.pos.words, second, third, zero_vector.words);
            if (created)
                created->SetMotionParameters(count << 12, 5);
            return 1;
        case 0x42A2:
            value = ReadCommandWord();
            level_environment.water_damage = value;
            return 1;
        case 0x42A3:
            cursor = script_cursor;
            value = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            second = cursor.Read16();
            script_cursor = cursor + 2;
            ::SelectCdVoice(value, second);
            return 1;
        case 0x42A4:
            cursor = AlignScriptCursor(script_cursor);
            script_cursor = cursor + 4;
            value = (cursor + 4).Read16();
            script_cursor = cursor + 6;
            second = (cursor + 6).Read16();
            script_cursor = cursor + 8;
            third = (cursor + 8).Read16();
            script_cursor = cursor + 10;
            ::CreateMenuBuffers(cursor.Read32(), value, second, third & 0xFF);
            return 1;
        case 0x42A5:
            ReleaseMenuBuffers();
            return 1;
        case 0x42B0:
        skip_string:
            script_cursor = SkipRecordString(script_cursor);
            return 1;
        case 0x42B1:
        case 0x42B2:
            value = ReadCommandValue();
            cursor = GetRecordPayload(value);
            if (opcode == 0x42B1)
                ActivateEnemyIndices(cursor);
            else
                SpawnEnemyIndices(cursor);
            return 1;
        case 0x450A:
            cursor = script_cursor;
            value = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            second = cursor.Read16();
            script_cursor = cursor + 2;
            PlayCdVoiceEvent(value, second, model.pos.words, 0);
            return 1;
        default:
            return 1;
    }
}

uint32 Enemy::ReadScriptValue()
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8004CF78, "SLUS_003.73");
    RecordCursor stream(script_cursor);
    uint32 command = stream.Read16();
    script_cursor = stream + 2;
    if ((command & 0x8000) == 0 && (command & 0x2000) != 0)
    {
        command = ResolveParameter(command);
    }
    return command & 0xFFFF;
}

uint32 Enemy::ReadParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8004CFDC, "SLUS_003.73");
    uint32 value, result, index, bank;
    RecordCursor cursor;
    command &= 0xFFFF;
    if (command - 8448 >= 67)
        return 0;
    switch (command)
    {
        case 8501:
            cursor = AlignScriptCursor(script_cursor);
            value = cursor.Read32();
            script_cursor = cursor + 4;
            ai.script_data = value;
            return 0;
        case 8495:
            cursor = AlignScriptCursor(script_cursor);
            bank = this->model.bank;
            value = cursor.Read32();
            result = FindModelResourceIndex(value, bank);
            this->model.model_idx = result;
            script_cursor = cursor + 4;
            return result;
        case 8512:
        case 8513:
        case 8514:
            result = (uint32)((sint32)(ReadScriptValue() << 16) >> 4);
            model.pos.words[command - 8512] = result;
            return result;
        case 8497:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2;
            result = this->model.flags;
            result = value ? result | 8 : result & 0xFFF7;
            this->model.flags = result;
            return result;
        case 8480:
            cursor = script_cursor;
            index = cursor.Read16() & 0xFF;
            script_cursor = cursor + 2;
            result = ReadScriptValue();
            WriteRegister(index, result);
            return result;
        case 8485:
            cursor = script_cursor;
            index = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            value = cursor.Read16();
            WriteAIParameter(index, value);
            script_cursor = cursor + 2;
            return 0;
        case 8486:
            cursor = script_cursor;
            index = cursor.Read16() & 0xFF;
            cursor += 2;
            script_cursor = cursor;
            value = cursor.Read16() & 0xFF;
            script_cursor = cursor + 2;
            result = (cursor + 2).Read16();
            script_cursor = cursor + 4;
            SetBehaviorValue(index, value, result);
            return 0;
        case 8448:
        case 8449:
        case 8468:
        case 8469:
        case 8481:
        case 8482:
        case 8492:
        case 8493:
        case 8494:
            cursor = script_cursor;
            value = (command == 8492 || command == 8493) ? (cursor)[0] : cursor.Read16();
            script_cursor = cursor + 2;
            switch (command)
            {
                case 8448:
                    this->health = (sint16)(uint16)(value);
                    break;
                case 8449:
                    this->collision_radius = value;
                    break;
                case 8468:
                    ai.state = value;
                    break;
                case 8469:
                    ai.phase = value;
                    break;
                case 8481:
                    this->playback.speed = (uint32)(sint32)(sint16)value;
                    break;
                case 8482:
                    this->ground_offset = value;
                    break;
                case 8492:
                    this->control.activation_count = value;
                    break;
                case 8493:
                    this->control.damage_phase = value;
                    break;
                case 8494:
                    this->control.height_limit = value;
                    break;
            }
            return 0;
        default:
            return 0;
    }
}

uint32 Enemy::ResolveParameter(uint32 opcode)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8004D2B4, "SLUS_003.73");
    uint32 value, result;
    ModelInstance *player;
    RecordCursor cursor;
    opcode &= 65535;
    switch (opcode)
    {
        case 8448:
            return (uint32)(sint32)this->health;
        case 8480:
            cursor = script_cursor;
            value = (cursor)[0];
            script_cursor = cursor + 2;
            return (uint32)(sint32)(sint16)ReadRegister(value);
        case 8489:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2;
            return RandomRange(value);
        case 8490:
            return this->record_idx;
        case 8491:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2;
            if (value & 0x2000)
            {
                value = ResolveParameter(value);
            }
            cursor = GetRecordPayload(value & 65535);
            return cursor.Read16() ? (cursor + 2).Read16() : 0;
        case 8492:
            return (uint32)this->control.activation_count;
        case 8493:
            return (uint32)this->control.damage_phase;
        case 8494:
            return (uint32)this->control.height_limit;
        case 8498:
            player = visual_lists.players;
            if (!player)
                return 8191;
            result = MeasureHorizontalDistance(this->model.pos.words, player->pos.words);
            return result < 8192 ? result : 8191;
        case 8499:
            player = visual_lists.players;
            if (!player)
                return 0;
            {
                Vector3 position;
                position = this->model.pos;
                position.words[1] -= (uint32)(sint32)(sint16)(uint32)this->sight_offset << 12;
                return IsSegmentVisible(position.words, player->pos.words);
            }
        case 8502:
            return movie_playback.frame_number;
        case 8512:
        case 8513:
        case 8514:
            return (uint32)((sint32)model.pos.words[opcode - 8512] >> 12);
        case 8528:
        case 8529:
        case 8530:
            player = visual_lists.players;
            return player ? (uint32)((sint32)player->pos.words[opcode - 8528] >> 12) : 0;
        default:
            return 0;
    }
}
