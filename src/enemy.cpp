#include <stdlib.h>
#include <string.h>
#include <new>

#include "collision_ray.h"
#include "level_session.h"
#include "game_settings.h"
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
#include "model_bank.h"
#include "game_object_allocation.h"
#include "enemy_list.h"
#include "enemy.h"
#include "explosion_effects.h"
#include "global.h"
#include "angle_math.h"
#include "record_data.h"
#include "record_commands.h"
#include "collision.h"
#include "movie_lifecycle.h"
#include "audio_session.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "fading_particle.h"
#include "fire_cycle.h"
#include "camera.h"
#include "effect_lifecycle.h"

static uint32 enemy_count;

void Enemy::ResetPopulation()
{
    enemy_count = 0;
}

Enemy::Enemy() : Character(), script_cursor(0), ground_offset(0), sight_offset(0), behavior(0), behavior_rows(0), behavior_cols(0)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004B800, "SLUS_003.73");
    rec_idx = 65535;
    col_radius = 128;
    control.height_limit = 32;
    control.script_delay = 0;
    contact_strength = 64;
    dst.x = dst.y = dst.z = 0;
    for (uint32 idx = 0; idx < 8; ++idx)
        script_labels[idx] = 0;
    ++enemy_count;
}

uint32 Enemy::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004B868, "SLUS_003.73");
    --enemy_count;
    uint32 result = Character::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

uint32 Enemy::Finish()
{
    FUNCTION_MARKER(0x8004B8C8, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        ActivateLinkedRecs();
        result = lifecycle_flags | 64;
        lifecycle_flags = result;
        model.flags |= 1;
    }
    return result;
}

uint32 Enemy::ScaleDamageForDifficulty(uint32 damage)
{
    FUNCTION_MARKER(0x8004B914, "SLUS_003.73");
    uint32 difficulty = game_settings.difficulty;
    if (difficulty == 1)
        return damage;
    if (difficulty)
        return damage << 1;
    sint32 signed_damage = damage;
    return damage - (signed_damage >> 2);
}

uint32 Enemy::ActivateLinkedRecs()
{
    RecordDataBorrow rec_borrow;
    uint16 idx;
    FUNCTION_MARKER(0x8004B948, "SLUS_003.73");
    if (control.linked_recs_activated)
        return 65535;
    idx = rec_idx;
    if (idx == 65535)
        return 1;
    control.linked_recs_activated = true;
    return SpawnEnemyIndices(GetLevelRec(idx) + 6);
}

uint32 Enemy::IntegrateMotion()
{
    FUNCTION_MARKER(0x8004B9A4, "SLUS_003.73");
    Vector3 &velocity = motion.velocity;
    surface.n.vx = zero_angles.x;
    surface.n.vy = zero_angles.y;
    surface.n.vz = zero_angles.z;
    contact_flags = 0;
    velocity.Add(motion.acceleration);
    velocity.Damp(motion.drag);
    velocity.SnapSmall();
    model.pos.Add(velocity);
    model.angles.AddAngles(motion.angular_velocity);
    model.angles.NormalizeAngles();
    motion.angular_velocity.AddAngles(motion.angular_acceleration);
    motion.angular_velocity.DampAngles(motion.angular_drag);
    return motion.angular_velocity.SnapAngles();
}

bool Enemy::ChoosePos(Vector3 *output, const Vector3 *target, uint32 minimum, uint32 maximum, sint32 height_limit, sint16 spread, bool ground, bool check_col)
{
    FUNCTION_MARKER(0x8004BA64, "SLUS_003.73");
    uint32 angle, radius, delta, sine, offset;
    sint32 height;
    CollisionRay col;
    uint32 pass;
    angle = 1024 - (uint32)ratan2((sint32)((uint32)model.pos.z - target->z), (sint32)((uint32)model.pos.x - target->x));
    angle += RandomRange(spread);
    angle -= spread >> 1;
    radius = minimum + RandomRange(maximum - minimum);
    angle &= 4095;
    sine = rsin(angle);
    output->x = (uint32)target->x + radius * sine;
    output->y = target->y;
    sine = rcos(angle);
    output->z = (uint32)target->z + radius * sine;
    if (ground)
    {
        height = CollisionRay::FindGroundHeight(*output, 512, 512);
        if (height == -1)
            return false;
        offset = ground_offset * ONE;
        output->y = height - offset;
    }
    if (check_col)
    {
        for (pass = 0; pass < 2; ++pass)
        {
            col.endpoints[0] = model.pos;
            if (!pass)
            {
                offset = sight_offset * ONE;
                col.endpoints[0].y = (uint32)col.endpoints[0].y - offset;
            }
            col.endpoints[1] = *output;
            if (!pass)
            {
                offset = sight_offset * ONE;
                col.endpoints[1].y = (uint32)col.endpoints[1].y - offset;
            }
            col.Initialize();
            col.triggers = false;
            col_query.allow_surface20 = true;
            col.Query(1);
            ModelInstance *hit = col.hit.GetObject();
            col_query.allow_surface20 = false;
            if (hit)
                return false;
        }
    }
    delta = (uint32)model.pos.y - output->y;
    if ((sint32)delta < 0)
        delta = -delta;
    return ((sint32)delta >> 12) < height_limit;
}

void Enemy::AllocateBehaviorTable(uint8 rows, uint8 columns)
{
    FUNCTION_MARKER(0x8004BD04, "SLUS_003.73");
    free(behavior);
    behavior = 0;
    behavior_rows = rows;
    behavior_cols = columns;
    uint32 count = behavior_rows * behavior_cols;
    if (count)
    {
        behavior = (uint16 *)calloc(count, sizeof(*behavior));
        if (!behavior)
        {
            GameDiagnostic(("Cannot allocate enemy behavior table %u/%u\n", (uint32)behavior_rows, (uint32)behavior_cols));
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

void Enemy::SetBehaviorVal(uint8 row, uint8 column, uint16 val)
{
    FUNCTION_MARKER(0x8004BDCC, "SLUS_003.73");
    behavior[GetBehaviorIdx(row, column)] = val;
}

uint16 Enemy::GetBehaviorVal(uint8 row, uint8 column) const
{
    FUNCTION_MARKER(0x8004BDFC, "SLUS_003.73");
    return behavior[GetBehaviorIdx(row, column)];
}

uint16 Enemy::SkipConditionalBlock()
{
    RecordDataBorrow rec_borrow;
    uint32 depth = 0;
    uint16 command;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004BE30, "SLUS_003.73");
    for (;;)
    {
        cursor = script_cursor;
        command = cursor.Read16();
        script_cursor = cursor + 2;
        if (command == 0x4120)
        {
            if (depth-- == 0)
                return command - 0x4112;
        }
        else if (command >= 0x4112 && command < 0x4117)
            ++depth;
        if (command == 0x4100)
            return command;
    }
}

uint32 Enemy::ExecuteScript(const RecordCursor &script)
{
    RecordDataBorrow rec_borrow;
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8004BE90, "SLUS_003.73");
    if (!script)
    {
        GameDiagnostic(("Enemy script is missing\n"));
        abort();
    }
    script_cursor = script;
    for (;;)
    {
        uint16 command = script_cursor.Read16();
        if (command == 0x4100)
        {
            control.script_active = 0;
            // Native callers observe completion rather than the old cursor carrier
            return 1;
        }
        script_cursor += 2;
        if (command & 0x4000)
        {
            if (!ExecuteCommand(command))
                return 0;
        }
        else if (command & 0x2000)
        {
            SetParameters(command);
        }
    }
}

uint32 Enemy::ExecuteCommand(uint16 opcode)
{
    GameHeapBlock *reservation;
    RecordDataBorrow rec_borrow;
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8004BF3C, "SLUS_003.73");
    uint16 val0, val1, val2, count, flash_mode;
    uint32 i, operand_bytes, operand_offset;
    sint32 active_effects;
    Pickup *created;
    RecordCursor cursor;
    RecordCursor saved;
    RecordCursor rec_payload;
    Vector3 pos;
    uint16 operand_words[8];
    uint8 *operands = (uint8 *)operand_words;
    ShortVector3 angles;
    sint32 left, right;
    switch (opcode)
    {
        case 0x4101:
        case 0x4102:
            val0 = ReadCommandWord();
            script_cursor = GetScriptLabel(val0);
            return opcode == 0x4101;
        case 0x4104:
            cursor = script_cursor;
            val0 = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            SetScriptLabel(val0, cursor);
            return 1;
        case 0x4105:
            saved = script_cursor;
            for (;;)
            {
                cursor = script_cursor;
                val0 = cursor.Read16();
                if (val0 == 0x4100)
                    break;
                script_cursor = cursor + 2;
                if (val0 == 0x4104)
                {
                    val0 = (cursor + 2).Read16();
                    script_cursor = cursor + 4;
                    SetScriptLabel(val0, cursor + 4);
                }
            }
            script_cursor = saved;
            return 1;
        case 0x4106:
            val0 = ReadCommandVal();
            cursor = ReadRecPos(&pos, val0) + 6;
            script_cursor = cursor;
            return 1;
        case 0x4107:
            return 0;
        case 0x4110:
        case 0x4111:
            cursor = script_cursor;
            val0 = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            left = (sint16)ResolveParameter(val0);
            operand_bytes = cursor.DistanceTo(script_cursor);
            if ((operand_bytes & 1) || operand_bytes > sizeof(operand_words) - 2)
            {
                GameDiagnostic(("Invalid arithmetic operand byte count\n"));
                abort();
            }
            // Replay the original local encoded operands and computed halfword
            operand_offset = 0;
            while (cursor != script_cursor)
            {
                operand_words[operand_offset / 2] = cursor.Read16();
                operand_offset += 2;
                cursor += 2;
            }
            right = ReadSignedCommandVal();
            operand_words[operand_offset / 2] = opcode == 0x4110 ? left + right : left - right;
            saved = script_cursor;
            script_cursor = RecordCursor(operands, operand_bytes + 2);
            SetParameters(val0);
            script_cursor = saved;
            return 1;
        case 0x4112:
        case 0x4113:
        case 0x4114:
            left = ReadSignedCommandVal();
            right = ReadSignedCommandVal();
            if ((opcode == 0x4112 && right >= left) || (opcode == 0x4113 && left >= right) || (opcode == 0x4114 && left != right))
                SkipConditionalBlock();
            return 1;
        case 0x4115:
        case 0x4116:
            val0 = ReadCommandWord();
            val1 = control.flags & val0;
            if ((opcode == 0x4115 && val1 != val0) || (opcode == 0x4116 && val1 != 0))
                SkipConditionalBlock();
            return 1;
        case 0x4200:
            VisualObject::SetModelBank(script_cursor.CStr());
            goto skip_string;
        case 0x4201:
            cursor = script_cursor;
            val0 = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            val1 = cursor.Read16();
            script_cursor = cursor + 2;
            StartAnimation(val0, (sint8)val1);
            return 1;
        case 0x4202:
            val0 = ReadCommandWord();
            SetAnimationRange(val0, 0, -1);
            return 1;
        case 0x4203:
            model.flags &= 0xFFFE;
            return 1;
        case 0x4204:
            model.flags |= 1;
            return 1;
        case 0x4205:
            Finish();
            return 1;
        case 0x4226:
            motion.velocity.x = motion.velocity.y = motion.velocity.z = 0;
            return 1;
        case 0x4227:
            val0 = ReadCommandWord();
            if (val0)
            {
                control.ground_period = val0;
                control.ground_tick = 0;
                control.flags |= 2;
            }
            else
                control.flags &= ~2;
            return 1;
        case 0x4240:
            control.script_active = 0;
            return 0;
        case 0x4260:
        case 0x4261:
            val0 = ReadCommandWord();
            SetBehaviorVal(val0, 0, opcode == 0x4260);
            return 1;
        case 0x4280:
            val0 = ReadCommandWord();
            control.script_delay = val0;
            if (val0 & 0x2000)
            {
                control.script_delay = ResolveParameter(control.script_delay);
            }
            return 0;
        case 0x4281:
            if (control.prev_activation_flags & 1)
                return 1;
            script_cursor = script_cursor - 2;
            return 0;
        case 0x4290:
            val0 = ReadCommandWord();
            PlaySoundEffect((sint16)val0, 8192, 0);
            return 1;
        case 0x4291:
            val0 = ReadCommandWord();
            PlayPositionedSound((sint16)val0, &model.pos, 0);
            return 1;
        case 0x4292:
            cursor = script_cursor;
            active_effects = effect_count;
            count = cursor.Read16();
            script_cursor = cursor + 2;
            if (active_effects >= 200)
                return 1;
            PrimitiveParticle::SetAngles(&model.angles);
            angles.comp[0] = 512;
            angles.comp[1] = 4096;
            angles.comp[2] = 0;
            PrimitiveParticle::SetSpread(&angles);
            PrimitiveParticle::SetSize(1);
            PrimitiveParticle::SetColor(128, 128, 128);
            PrimitiveParticle::SetColorStep(4, 4, 4);
            use_small_effect_pool = false;
            for (i = 0; i < count; ++i)
            {
                reservation = EffectObject::AllocateStorage(88);
                if (reservation)
                {
                    new (reservation) FadingParticle(&model.pos, 32, 8192, 32);
                }
            }
            use_small_effect_pool = true;
            return 1;
        case 0x4293:
            cursor = script_cursor.Aligned(4);
            ::PrepareMovieTex(cursor.Read32());
            script_cursor = cursor + 4;
            return 1;
        case 0x4294:
            QueueMoviePos(&model.pos);
            rec_payload = GetRecPayload(rec_idx);
            cursor = rec_payload;
            for (i = 0; i < rec_payload.Read16(); ++i)
            {
                val0 = (cursor + 2).Read16();
                cursor += 2;
                ReadRecPos(&pos, val0);
                QueueMoviePos(&model.pos);
            }
            ::PrepareMoviePlaybackBuffers();
            return 1;
        case 0x4295:
            ResetMoviePlayback();
            return 1;
        case 0x4296:
            ReadCommandWord();
            ::RestoreMusicPlayback();
            return 1;
        case 0x4297:
            ReadCommandWord();
            StopMusicSequence();
            return 1;
        case 0x4298:
            val0 = ReadCommandVal();
            if (visual_lists.cameras)
                static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(val0 == 0 ? 2 : val0 == 1 ? 1 : 0);
            return 1;
        case 0x4299:
            val0 = ReadCommandWord();
            ::SelectMovieRec(val0 & 0xFF);
            return 1;
        case 0x429A:
        case 0x42A6:
            if (opcode == 0x429A)
            {
                val0 = ReadCommandVal();
                ReadRecPos(&pos, val0);
            }
            else
            {
                pos = model.pos;
            }
            val0 = ReadCommandWord();
            if (val0 < 2)
                HandleExplosionProximity(&model.pos, val0);
            else if (val0 == 2)
            {
                val1 = ReadScriptVal();
                cursor = script_cursor + 2;
                script_cursor = cursor;
                val2 = cursor.Read16();
                script_cursor = cursor + 2;
                reservation = EffectObject::AllocateStorage(156);
                if (reservation)
                {
                    new (reservation) FireCycle(&pos, val1, val2 != 0, val2);
                }
            }
            return 1;
        case 0x429C:
            cursor = script_cursor;
            val0 = cursor[0];
            val1 = (cursor + 2)[0];
            val2 = (cursor + 4)[0];
            count = (cursor + 6).Read16();
            cursor = script_cursor;
            flash_mode = (cursor + 8).Read16();
            ::SetScrFlash(val0, val1, val2, count, 0, flash_mode);
            script_cursor = script_cursor + 10;
            return 1;
        case 0x429D:
            QueueMoviePos(&model.pos);
            RequestCdStreamRefresh();
            return 1;
        case 0x429E:
            level_environment.water_y = (uint32)model.pos.y;
            return 1;
        case 0x42A0:
            val0 = ReadCommandVal();
            cursor = script_cursor;
            val1 = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            val2 = cursor.Read16();
            script_cursor = cursor + 2;
            count = (cursor + 2).Read16();
            script_cursor = cursor + 4;
            created = Pickup::Spawn(val0, &model.pos, val1, val2, &zero_vector);
            if (created)
                created->SetMotionParameters(count << 12, 5);
            return 1;
        case 0x42A2:
            val0 = ReadCommandWord();
            level_environment.water_damage = val0;
            return 1;
        case 0x42A3:
            cursor = script_cursor;
            val0 = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            val1 = cursor.Read16();
            script_cursor = cursor + 2;
            ::SelectCdVoice(val0, val1);
            return 1;
        case 0x42A4:
            cursor = script_cursor.Aligned(4);
            script_cursor = cursor + 4;
            val0 = (cursor + 4).Read16();
            script_cursor = cursor + 6;
            val1 = (cursor + 6).Read16();
            script_cursor = cursor + 8;
            val2 = (cursor + 8).Read16();
            script_cursor = cursor + 10;
            ::CreateMenuBuffers(cursor.Read32(), val0, val1, val2);
            return 1;
        case 0x42A5:
            ReleaseMenuBuffers();
            return 1;
        case 0x42B0:
        skip_string:
            script_cursor = SkipRecString(script_cursor);
            return 1;
        case 0x42B1:
        case 0x42B2:
            val0 = ReadCommandVal();
            cursor = GetRecPayload(val0);
            if (opcode == 0x42B1)
                ActivateEnemyIndices(cursor);
            else
                SpawnEnemyIndices(cursor);
            return 1;
        case 0x450A:
            cursor = script_cursor;
            val0 = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            val1 = cursor.Read16();
            script_cursor = cursor + 2;
            PlayCdVoiceEvent(val0, val1, &model.pos);
            return 1;
        default:
            return 1;
    }
}

uint16 Enemy::ReadScriptVal()
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004CF78, "SLUS_003.73");
    RecordCursor stream(script_cursor);
    uint16 command = stream.Read16();
    script_cursor = stream + 2;
    if ((command & 0x8000) == 0 && (command & 0x2000) != 0)
    {
        command = ResolveParameter(command);
    }
    return command;
}

uint32 Enemy::ReadParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004CFDC, "SLUS_003.73");
    uint32 val, result, idx, bank;
    uint16 word;
    RecordCursor cursor;
    if (command < 0x2100 || command >= 0x2143)
        return 0;
    switch (command)
    {
        case 8501:
            cursor = script_cursor.Aligned(4);
            val = cursor.Read32();
            script_cursor = cursor + 4;
            ai.script_data = val;
            return 0;
        case 8495:
            cursor = script_cursor.Aligned(4);
            bank = model.bank;
            val = cursor.Read32();
            result = ModelBank::FindResourceIdx(val, bank);
            model.model_idx = result;
            script_cursor = cursor + 4;
            return result;
        case 8512:
        case 8513:
        case 8514:
            result = (sint16)ReadScriptVal() * ONE;
            model.pos.comp[command - 8512] = result;
            return result;
        case 8497:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            result = model.flags;
            result = val ? result | 8 : result & 0xFFF7;
            model.flags = result;
            return result;
        case 8480:
            cursor = script_cursor;
            idx = cursor.Read16() & 0xFF;
            script_cursor = cursor + 2;
            result = ReadScriptVal();
            WriteRegister(idx, result);
            return result;
        case 8485:
            cursor = script_cursor;
            idx = cursor.Read16();
            cursor += 2;
            script_cursor = cursor;
            val = cursor.Read16();
            WriteAIParameter(idx, val);
            script_cursor = cursor + 2;
            return 0;
        case 8486:
            cursor = script_cursor;
            idx = cursor.Read16() & 0xFF;
            cursor += 2;
            script_cursor = cursor;
            val = cursor.Read16() & 0xFF;
            script_cursor = cursor + 2;
            result = (cursor + 2).Read16();
            script_cursor = cursor + 4;
            SetBehaviorVal(idx, val, result);
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
            word = (command == 8492 || command == 8493) ? cursor[0] : cursor.Read16();
            script_cursor = cursor + 2;
            switch (command)
            {
                case 8448:
                    health = word;
                    break;
                case 8449:
                    col_radius = word;
                    break;
                case 8468:
                    ai.state = word;
                    break;
                case 8469:
                    ai.phase = word;
                    break;
                case 8481:
                    playback.speed = (sint16)word;
                    break;
                case 8482:
                    ground_offset = word;
                    break;
                case 8492:
                    control.activation_count = word;
                    break;
                case 8493:
                    control.damage_phase = word;
                    break;
                case 8494:
                    control.height_limit = word;
                    break;
            }
            return 0;
        default:
            return 0;
    }
}

uint32 Enemy::ResolveParameter(uint16 opcode)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004D2B4, "SLUS_003.73");
    uint32 val, result;
    ModelInstance *player;
    Vector3 pos;
    RecordCursor cursor;
    switch (opcode)
    {
        case 8448:
            return health;
        case 8480:
            cursor = script_cursor;
            val = cursor[0];
            script_cursor = cursor + 2;
            return (sint16)ReadRegister(val);
        case 8489:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            return RandomRange(val);
        case 8490:
            return rec_idx;
        case 8491:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (val & 0x2000)
            {
                val = ResolveParameter(val);
            }
            cursor = GetRecPayload(val & 65535);
            return cursor.Read16() ? (cursor + 2).Read16() : 0;
        case 8492:
            return control.activation_count;
        case 8493:
            return control.damage_phase;
        case 8494:
            return control.height_limit;
        case 8498:
            player = visual_lists.players;
            if (!player)
                return 8191;
            result = model.pos.HorizontalDistance(player->pos);
            return result < 8192 ? result : 8191;
        case 8499:
            player = visual_lists.players;
            if (!player)
                return 0;
            pos = model.pos;
            pos.y = (uint32)pos.y - (sight_offset * ONE);
            return CollisionRay::IsSegVisible(pos, player->pos);
        case 8502:
            return movie_playback.frame_number;
        case 8512:
        case 8513:
        case 8514:
            return model.pos.comp[opcode - 8512] >> 12;
        case 8528:
        case 8529:
        case 8530:
            player = visual_lists.players;
            return player ? player->pos.comp[opcode - 8528] >> 12 : 0;
        default:
            return 0;
    }
}

void Enemy::NoOp()
{
    FUNCTION_MARKER(0x800639A4, "SLUS_003.73");
}

uint16 Enemy::ReadCommandWord()
{
    RecordDataBorrow rec_borrow;
    uint16 val = script_cursor.Read16();
    script_cursor += 2;
    return val;
}

uint16 Enemy::ReadCommandVal()
{
    uint16 val = ReadCommandWord();
    return (val & 0x2000) ? ResolveParameter(val) : val;
}

sint32 Enemy::ReadSignedCommandVal()
{
    uint16 val = ReadCommandWord();
    return (val & 0x2000) ? (sint16)ResolveParameter(val) : val;
}

void Enemy::SetParameters(uint16 command)
{
    ReadParameters(command);
}

Enemy::~Enemy()
{
    free(behavior);
}

RecordCursor Enemy::GetScriptLabel(uint32 idx) const
{
    if (idx >= 8)
    {
        GameDiagnostic(("Enemy script label index out of bounds: %u\n", idx));
        abort();
    }
    return script_labels[idx];
}

void Enemy::SetScriptLabel(uint32 idx, const RecordCursor &script)
{
    if (idx >= 8)
    {
        GameDiagnostic(("Enemy script label index out of bounds: %u\n", idx));
        abort();
    }
    script_labels[idx] = script;
}

uint16 Enemy::ReadRegister(uint32 idx) const
{
    if (idx < 6)
        return ai.work[idx];
    switch (idx)
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
            if (idx < 18)
                return (uint32)dst.comp[(idx - 12) / 2] >> ((idx & 1) * 16);
            GameDiagnostic(("Enemy script register index out of bounds: %u\n", idx));
            abort();
    }
}

void Enemy::WriteRegister(uint32 idx, uint16 val)
{
    if (idx < 6)
    {
        ai.work[idx] = val;
        return;
    }
    switch (idx)
    {
        case 6:
            ai.state = val;
            return;
        case 7:
            ai.phase = val;
            return;
        case 8:
            control.script_delay = val;
            return;
        case 9:
            ai.script_register9 = val;
            return;
        case 10:
            ai.script_data = (ai.script_data & 0xFFFF0000) | val;
            return;
        case 11:
            ai.script_data = (ai.script_data & 0xFFFF) | ((uint32)val << 16);
            return;
        default:
            if (idx < 18)
            {
                sint32 &word = dst.comp[(idx - 12) / 2];
                if (idx & 1)
                    word = (word & 0xFFFF) | ((uint32)val << 16);
                else
                    word = (word & 0xFFFF0000) | val;
                return;
            }
            GameDiagnostic(("Enemy script register index out of bounds: %u\n", idx));
            abort();
    }
}

void Enemy::WriteAIParameter(uint32 idx, uint16 val)
{
    if (idx < 10)
        ai.parameters[idx] = val;
    else if (idx == 10)
        ground_offset = val;
    else if (idx == 11)
        sight_offset = val;
    else
        WriteRegister(idx - 12, val);
}

uint16 Enemy::GetBehaviorIdx(uint8 row, uint8 column) const
{
    uint16 idx = row * behavior_cols + column;
    if (!behavior || idx >= behavior_rows * behavior_cols)
    {
        GameDiagnostic(("Enemy behavior index %u/%u exceeds %u/%u\n", (uint32)row, (uint32)column, (uint32)behavior_rows, (uint32)behavior_cols));
        abort();
    }
    return idx;
}
