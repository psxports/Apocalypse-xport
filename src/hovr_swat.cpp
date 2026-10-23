#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "level_session.h"
#include "game_tick.h"
#include "resource_script.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "sprite.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "global.h"
#include "visual_object.h"
#include "model_bank.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "player.h"
#include "game_object_allocation.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "debris.h"
#include "oscillating_projectile.h"
#include "hovr_swat.h"
#include "record_data.h"
#include "audio_voice.h"
#include "enemy.h"
#include "swat.h"

HovrSwat::HovrSwat(const RecordCursor &rec_data, uint16 idx) : Enemy(), sound_timer(0), rotor_sound(0), sound_distance(0), debris_first(0), debris_count(0)
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x800590DC, "SLUS_003.73");
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    aim_offset.x = aim_offset.y = aim_offset.z = 0;

    cursor = ReadPos(rec_data);
    script_cursor = ReadAngles(cursor);
    rec_idx = idx;
    Init();
}

HovrSwat::~HovrSwat()
{
    if (rotor_sound)
        StopSoundVoices(rotor_sound);
    DestroySmoke();
}

uint32 HovrSwat::Init()
{
    uint8 random, bank;
    FUNCTION_MARKER(0x800591D4, "SLUS_003.73");
    ++game_progress.level.enemy_total;
    SetModelBank("HovrSwat");
    AddToList(visual_lists.actors);
    kind = 201;
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    sound_distance = model.pos.Distance(visual_lists.cameras->pos);
    AllocateBehaviorTable(1, 4);
    ai.parameters[0] = 12;
    ai.parameters[1] = 128;
    SetBehaviorVal(0, 1, 10);
    SetBehaviorVal(0, 2, 80);
    SetBehaviorVal(0, 3, 90);
    SetAnimationRange(0, 0, 0xFFFFFFFF);
    InitSmoke();
    health = 20;
    rotor_sound = PlayPositionedSound(101, &model.pos, 0);
    random = RandomRange(8);
    bank = GetRecModelBankSlot();
    sound_timer = random + 1;
    debris_first = ModelBank::FindResourceIdx(0xDB2D0C4F, bank);
    debris_count = 10;
    control.script_active = 1;
    return 1;
}

uint32 HovrSwat::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005933C, "SLUS_003.73");
    StopSoundVoices(rotor_sound);
    rotor_sound = 0;
    DestroySmoke();
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

uint32 HovrSwat::Damage(uint32 damage, const Vector3 *input_vector, uint32)
{
    FUNCTION_MARKER(0x800593CC, "SLUS_003.73");
    sint16 health = this->health;
    Vector3 impulse;
    ModelInstance *player;
    ++control.damage_phase;
    if (health > 0)
    {
        impulse.ShiftRight(*input_vector, 3);
        motion.velocity.Add(impulse);
        this->health -= damage;
        if (this->health > 0)
            SetColor(3, 255, 255, 255);
        else
        {
            player = visual_lists.players;
            if (player)
                static_cast<Player *>(player->owner)->TryPlayKillQuote();
            lifecycle_flags &= 0xFFEF;
            Fragment();
            Finish();
        }
    }
    return 1;
}

void HovrSwat::Fragment()
{
    uint16 trig_angle;
    Vector3 velocity;
    uint32 idx;
    uint16 angle, factor, model_idx;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x800594BC, "SLUS_003.73");
    HandleExplosionProximity(&model.pos, 0);
    RingBurstEffect::SpawnExplosion(&model.pos, 200, 240, 200, 0, 5, 0, 100);
    for (idx = 0; idx < 6; ++idx)
    {
        velocity = motion.velocity;
        angle = RandomRange(4096);
        factor = RandomRange(8);
        trig_angle = angle & 0xFFF;
        velocity.x = (uint32)velocity.x + (rsin(trig_angle) * (factor + 24));
        factor = RandomRange(16);
        velocity.y = (uint32)velocity.y - ((factor + 32) * ONE);
        factor = RandomRange(8);
        velocity.z = (uint32)velocity.z + (rcos(trig_angle) * (factor + 24));
        child = AllocateZeroedObject(320);
        if (child)
        {
            factor = RandomRange(debris_count);
            model_idx = debris_first + factor;
            new (child) Debris(&model.pos, &velocity, model_idx, 0, 2, 32, GetRecModelBankName());
        }
    }
}

void HovrSwat::InitSmoke()
{
    uint32 idx;
    uint16 size = 1024;
    sint32 offset_y = 0;
    Vector3 pos;
    uint8 r = 64, g = 64, b = 128;
    FUNCTION_MARKER(0x80059644, "SLUS_003.73");
    pos = model.pos;
    use_small_effect_pool = false;
    for (idx = 0; idx < 8; ++idx)
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(112);
        if (allocation)
        {
            Sprite *spr = new (allocation) Sprite();
            spr->persistent = true;
            pos.y = (uint32)model.pos.y + offset_y;
            spr->SetPos(&pos);
            spr->SelectResource("Smoke");
            spr->SetSize(size);
            spr->EnableSemiTransparency();
            spr->SetColors(r, g, b);
            r -= 8;
            g -= 8;
            b -= 12;
            smoke[idx] = spr;
        }
        size += 256;
        offset_y += 0x18000;
    }
    use_small_effect_pool = true;
}

void HovrSwat::DestroySmoke()
{
    FUNCTION_MARKER(0x8005979C, "SLUS_003.73");
    for (uint32 idx = 0; idx < 8; ++idx)
    {
        Sprite *spr = smoke[idx];
        if (spr)
        {
            spr->Destroy(3);
            smoke[idx] = 0;
        }
    }
}

void HovrSwat::UpdateSmoke()
{
    Vector3 pos;
    FUNCTION_MARKER(0x80059808, "SLUS_003.73");
    pos = model.pos;
    for (sint32 idx = 0; idx < 8; ++idx)
    {
        Sprite *spr = smoke[idx];
        if (spr)
        {
            uint8 random = RandomRange(32);
            sint32 offset = (idx * 32 + random - 16) * ONE;
            pos.y = (uint32)model.pos.y + offset;
            spr->SetPos(&pos);
        }
    }
}

uint32 HovrSwat::Update()
{
    RecordDataBorrow rec_borrow;
    uint32 flags, distance;
    uint16 activation, timer, scale;
    Vector3 pos;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800598B8, "SLUS_003.73");
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        activation_flags = activation & 0xFFFE;
        ++control.activation_count;
    }
    motion.acceleration = zero_vector;
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
    if (control.flags & 1)
    {
        distance = model.pos.Distance(dst);
        if (distance < 129)
            control.flags &= ~1;
        else
        {
            angles.SetDir(model.pos, dst);
            scale = ai.parameters[0];
            motion.acceleration.SetDir(scale, angles);
        }
    }
    flags = control.flags;
    if (flags & 0x1D)
    {
        if (flags & 0x10)
        {
            ModelInstance *subject = tracked_target.GetObject();
            if (subject)
            {
                aim_pos = subject->pos;
            }
            else
                control.flags &= ~16;
        }
        else if (flags & 8)
        {
            pos.Add(model.pos, aim_offset);
            aim_pos = pos;
        }
        if (flags & 0x1C)
        {
            pos = model.pos;
            angles.SetDir(pos, aim_pos);
            if (flags & 0x10)
                angles.x = 0;
        }
        else
            angles.SetDir(model.pos, dst);
        scale = ai.parameters[1];
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, scale);
    }
    else
    {
        motion.angular_velocity.z = 0;
        motion.angular_velocity.y = 0;
        motion.angular_velocity.x = 0;
        motion.angular_acceleration.z = 0;
        motion.angular_acceleration.y = 0;
        motion.angular_acceleration.x = 0;
    }
    model.angles.x = 0;
    UpdateWorldTransform();
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    if (rotor_sound)
    {
        timer = sound_timer;
        if (timer)
            sound_timer = timer - 1;
        else
        {
            distance = model.pos.Distance(visual_lists.cameras->pos);
            sound_timer = 8;
            UpdatePositionedSoundVoices(rotor_sound, &model.pos, (sint32)(distance - sound_distance));
            sound_distance = distance;
        }
    }
    else if (!(game_clock.tick & 7))
        rotor_sound = PlayPositionedSound(101, &model.pos, 0);
    UpdateSmoke();
    return 0;
}

uint32 HovrSwat::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    Vector3 pos;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80059D1C, "SLUS_003.73");
    switch (command)
    {
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
            SetDst(&pos);
            return 1;
        case 0x4221:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (val & 0x2000)
            {
                val = ResolveParameter(val);
            }
            ReadRecPos(&pos, val);
            SetDst(&pos);
            return 1;
        case 0x4262:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (!val)
                Fire();
            return 1;
        case 0x4406:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (val)
                SetTargetObject(visual_lists.players);
            else
                ClearTarget();
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 HovrSwat::SetDst(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80059F28, "SLUS_003.73");
    dst = *pos;
    control.flags |= 1;
    return control.flags;
}

uint32 HovrSwat::SetTargetObject(ModelInstance *target)
{
    FUNCTION_MARKER(0x8005A004, "SLUS_003.73");
    uint32 flags = control.flags;
    tracked_target.SetObject(target);
    flags = (flags | 16) & 0xFFFFFFF3;
    control.flags = flags;
    return flags;
}

uint32 HovrSwat::ClearTarget()
{
    uint32 flags = control.flags & 0xFFFFFFE3;
    FUNCTION_MARKER(0x8005A020, "SLUS_003.73");
    control.flags = flags;
    return flags;
}

uint32 HovrSwat::Fire()
{
    Vector3 pos;
    GameHeapBlock *projectile;
    uint32 damage;
    uint16 speed;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x8005A034, "SLUS_003.73");
    if (!visual_lists.players)
        return 0;
    SetAnimationRange(2, 0, 0xFFFFFFFF);
    pos = model.pos;
    rotation.SetDir(pos, visual_lists.players->pos);
    projectile = EffectObject::AllocateStorage(108);
    if (projectile)
    {
        damage = GetBehaviorVal(0, 1);
        damage = ScaleDamageForDifficulty(damage);
        speed = GetBehaviorVal(0, 2);
        new (projectile) OscillatingProjectile(&pos, &rotation, damage, 45, speed, 16, 240, 240, 240, 64, 32, 128);
    }
    return PlayPositionedSound(20, &model.pos, 0);
}

void HovrSwat::UpdateState()
{
    uint16 phase, radius, orbit_size, yaw, yaw_step, trig_angle;
    uint16 delay;
    uint8 animation;
    sint8 frame;
    bool clockwise;
    Vector3 pos, vector, normal;
    ModelInstance *player;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x8005A170, "SLUS_003.73");
    if (ai.state)
        return;
    phase = ai.phase;
    if (phase == 0 || phase == 1)
    {
        if (control.prev_activation_flags & 1)
        {
            pos = model.pos;
            pos.y = (uint32)pos.y - (0x800000);
            SetDst(&pos);
            ai.phase = 2;
            return;
        }
        if (!phase)
        {
            player = visual_lists.players;
            if (!player)
                return;
            vector.x = (sint32)((uint32)model.pos.x - player->pos.x) >> 12;
            vector.y = 0;
            vector.z = (sint32)((uint32)model.pos.z - player->pos.z) >> 12;
            NormalizeShortVector(vector, normal);
            ai.work[0] = normal.x;
            ai.work[2] = 1024;
            ai.work[3] = 512;
            ai.work[4] = 0;
            ai.work[1] = normal.z;
            if (RandomRange(64) & 1)
                control.flags |= 0x20;
            ++ai.phase;
            return;
        }
        if (!(control.flags & 1))
        {
            if (!RandomRange(32))
            {
                trig_angle = RandomRange(4096) & 0xFFF;
                ai.work[0] = rsin(trig_angle);
                ai.work[1] = rcos(trig_angle);
                return;
            }
            player = visual_lists.players;
            if (!player)
                return;
            SetTargetObject(visual_lists.players);
            player = visual_lists.players;
            radius = ai.work[2];
            orbit_size = ai.work[3];
            yaw = ai.work[4];
            pos = player->pos;
            pos.x = (uint32)pos.x + ((sint16)ai.work[0] * radius);
            pos.y = (uint32)pos.y - (0x100000);
            pos.z = (uint32)pos.z + ((sint16)ai.work[1] * radius);
            trig_angle = RandomRange(4096) & 0xFFF;
            rotation.x = rsin(trig_angle) >> 6;
            rotation.y = yaw;
            rotation.z = 0;
            vector.SetDir(orbit_size, rotation);
            pos.Add(vector);
            SetDst(&pos);
            clockwise = (control.flags & 0x20) != 0;
            yaw_step = RandomRange(64);
            ai.work[4] = (clockwise ? yaw + 256 + yaw_step : yaw - 256 - yaw_step) & 0xFFF;
            if (!RandomRange(4))
            {
                if (RandomRange(2))
                {
                    if (radius < 2048)
                        ai.work[2] = radius + 32;
                }
                else if (radius >= 513)
                    ai.work[2] = radius - 32;
            }
            if (RandomRange(4))
                return;
            if (RandomRange(2))
            {
                if (orbit_size < 1024)
                    ai.work[3] = orbit_size + 16;
            }
            else if (orbit_size >= 257)
                ai.work[3] = orbit_size - 16;
            return;
        }
        animation = model.animation;
        if (animation == 1)
        {
            if (!playback.complete)
                return;
            frame = model.frame;
            if (!frame)
            {
                delay = GetBehaviorVal(0, 3);
                if (RandomRange(delay))
                    return;
                SetAnimationRange(1, 0, 0xFFFFFFFF);
                return;
            }
            ai.work[5] = frame;
        }
        else if (animation == 2)
        {
            if (!playback.complete)
                return;
            if (RandomRange(3))
            {
                SetAnimationRange(1, (sint16)ai.work[5], 0);
                return;
            }
        }
        else
        {
            SetAnimationRange(1, 0, 0xFFFFFFFF);
            return;
        }
        Fire();
        return;
    }
    if (phase == 2 && !(control.flags & 1))
        Finish();
}
