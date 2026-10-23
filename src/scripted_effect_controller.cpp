#include <new>
#include <stdlib.h>

#include "game_tick.h"
#include "record_data.h"
#include "camera.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "scripted_effect_controller.h"
#include "panel_particle.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "object_force.h"
#include "segment_effect.h"
#include "waving_beam.h"
#include "ballistic_hazard.h"
#include "enemy.h"
#include "effect_object.h"

ScriptedEffectController::ScriptedEffectController(const RecordCursor &rec_data, uint16 idx) : Enemy(), rain_count(0), rain(0), beam(), sound(0), sound_period(-1)
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004D604, "SLUS_003.73");

    cursor = ReadPos(rec_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    control.script_active = 1;
    rec_idx = idx;
    model.flags |= 1;
    lifecycle_flags &= 0xFFEF;
    AddToList(visual_lists.actors);
    col_radius = 0;
    kind = 203;
}

uint32 ScriptedEffectController::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004D6B4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    ReleaseChildren();
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return 0;
}

uint32 ScriptedEffectController::Update()
{
    GameHeapBlock *reservation;
    RecordDataBorrow rec_borrow;
    uint16 flags, timer;
    uint8 counter;
    RecordCursor script;
    uint32 count, idx;
    sint32 period, frame, remainder, height;
    Vector3 offset, pos, relative, scale;
    FUNCTION_MARKER(0x8004D800, "SLUS_003.73");
    flags = activation_flags;
    control.prev_activation_flags = flags;
    if (flags & 1)
    {
        counter = control.activation_count;
        activation_flags = flags & 65534;
        control.activation_count = counter + 1;
    }
    if (control.script_active)
    {
        timer = control.script_delay;
        if (timer)
            control.script_delay = timer - 1;
        else
        {
            script = script_cursor;
            ExecuteScript(RecordCursor(script));
        }
    }
    else
    {
        Finish();
    }
    if ((control.flags & 256) && !RandomRange(4))
    {
        use_small_effect_pool = false;
        count = RandomRange(3) + 2;
        while (count)
        {
            pos = model.pos;
            pos.x = (uint32)pos.x + ((RandomRange(128) - 64) << 12);
            pos.y = (uint32)pos.y + ((RandomRange(128) - 64) << 12);
            pos.z = (uint32)pos.z + ((RandomRange(128) - 64) << 12);
            reservation = EffectObject::AllocateStorage(116);
            // Original computes jitter but passes the object position
            if (reservation)
            {
                new (reservation) PanelParticle(&model.pos);
            }
            count -= 1;
        }
        use_small_effect_pool = true;
    }
    period = sound_period;
    if (period != -1)
    {
        frame = game_clock.tick;
        if (!period)
            remainder = frame;
        else
            remainder = frame % period;
        if (!remainder)
            UpdatePositionedSoundVoices(sound, &model.pos, 0);
    }
    if (force)
        force->Update();
    if (!rain)
        return 0;
    height = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.y;
    for (idx = 0; idx < rain_count; ++idx)
    {
        RainDrop *drop = rain + idx;
        SegmentEffect *effect = drop->effect.Get();
        if (!effect)
            continue;
        pos = effect->origin;
        pos.Add(drop->velocity);
        if (height < pos.y)
        {
            offset.x = (RandomRange(2400) - 1200) << 12;
            offset.y = (RandomRange(400) - 1200) << 12;
            offset.z = (RandomRange(2400) - 1200) << 12;
            pos.Add(visual_lists.players->pos, offset);
        }
        effect->origin = pos;
        relative.Subtract(pos, drop->velocity);
        scale.ShiftRight(static_cast<Camera *>(visual_lists.cameras->owner)->motion.velocity, 1);
        effect->points[0].pos.Add(relative, scale);
    }
    return 0;
}

void ScriptedEffectController::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 radius;
    sint16 strength;
    uint8 range_shift;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004DB90, "SLUS_003.73");
    if (command == 8960)
    {
        if (force)
        {
            cursor = script_cursor;
            radius = cursor.Read16();
            script_cursor = cursor + 2;
            force->SetRadius(radius);
        }
    }
    else if (command == 8961)
    {
        cursor = script_cursor;
        strength = cursor.Read16();
        cursor += 2;
        script_cursor = cursor;
        range_shift = cursor[0];
        cursor += 2;
        script_cursor = cursor;
        if (force)
            force->SetStrength(strength, range_shift);
    }
    else
        Enemy::ReadParameters(command);
}

uint32 ScriptedEffectController::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004DC40, "SLUS_003.73");
    GameHeapBlock *allocated;
    uint16 val, sound_id;
    RecordCursor cursor;
    if (command == 0x4503)
    {
        uint16 count = ReadScriptVal();
        Vector3 origin, pos, difference, scaled;
        rain_count = count;
        rain = new RainDrop[count];
        use_small_effect_pool = false;
        origin = visual_lists.players->pos;
        for (uint32 i = 0; i < rain_count; ++i)
        {
            RainDrop *drop = rain + i;
            pos.x = (RandomRange(2400) - 1200) << 12;
            pos.y = (RandomRange(400) - 1200) << 12;
            pos.z = (RandomRange(2400) - 1200) << 12;
            pos.Add(origin);
            drop->velocity.x = (RandomRange(16) - 8) << 12;
            drop->velocity.y = (RandomRange(48) + 48) << 12;
            drop->velocity.z = (RandomRange(16) - 8) << 12;
            allocated = EffectObject::AllocateStorage(100);
            if (!allocated)
            {
                GameDiagnostic(("Rain segment allocation failed\n"));
                abort();
            }
            SegmentEffect *effect = new (allocated) SegmentEffect(1);
            drop->effect = effect;
            effect->persistent = true;
            effect->origin = pos;
            difference.Subtract(pos, drop->velocity);
            scaled.ShiftRight(static_cast<Camera *>(visual_lists.cameras->owner)->motion.velocity, 1);
            effect->points[0].pos.Add(difference, scaled);
            effect->rgbc.r = 64;
            effect->rgbc.g = 64;
            effect->rgbc.b = 80;
            effect->points[0].rgbc.r = 16;
            effect->points[0].rgbc.g = 16;
            effect->points[0].rgbc.b = 32;
            effect->Enable();
        }
        use_small_effect_pool = true;
        return 1;
    }
    if (command == 0x4507 || command == 0x4508 || command == 0x4509)
    {
        if (sound)
            StopSoundVoices(sound);
        if (command == 0x4509)
            sound = 0;
        else
        {
            cursor = script_cursor;
            val = cursor.Read16();
            sound_id = val;
            script_cursor = cursor + 2;
            if (command == 0x4507)
            {
                sound = PlaySoundEffect(val, 8192, 0);
                sound_period = -1;
            }
            else
            {
                cursor += 2;
                val = cursor.Read16();
                script_cursor = cursor + 2;
                sound_period = val;
                sound = PlayPositionedSound(sound_id, &model.pos, 0);
            }
        }
        return 1;
    }
    if (command == 0x4700)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        script_cursor = cursor + 2;
        control.flags = val ? control.flags | 0x100 : control.flags & ~0x100;
        return 1;
    }
    if (command == 0x4506)
    {
        ModelInstance *player = visual_lists.players;
        if (player)
        {
            model.pos = player->pos;
        }
        return 1;
    }
    if (command == 0x4504)
    {
        uint16 kind, flags, radius;
        sint16 vertical_accel;
        ModelInstance *player;
        cursor = script_cursor;
        player = visual_lists.players;
        kind = cursor.Read16();
        script_cursor = cursor + 2;
        flags = (cursor + 2).Read16();
        script_cursor = cursor + 4;
        radius = (cursor + 4).Read16();
        script_cursor = cursor + 6;
        vertical_accel = (cursor + 6).Read16();
        script_cursor = cursor + 8;
        if (player && model.pos.HorizontalDistance(player->pos) < 4096)
        {
            allocated = AllocateZeroedObject(504);
            if (allocated)
            {
                new (allocated) BallisticHazard(&model.pos, kind, flags, radius, vertical_accel);
            }
        }
        return 1;
    }
    if (command == 0x429B || command == 0x42A1)
    {
        uint16 len, damage, r, g, b;
        uint8 scroll_step;
        WavingBeam *effect = 0;
        allocated = EffectObject::AllocateStorage(364);
        if (allocated)
        {
            len = script_cursor.Read16();
            damage = (script_cursor + 2).Read16();
            r = (script_cursor + 4).Read16();
            g = (script_cursor + 6).Read16();
            b = (script_cursor + 8).Read16();
            scroll_step = (script_cursor + 10)[0];
            effect = new (allocated) WavingBeam(&model.pos, &model.angles, len, 65535, damage, r, g, b, scroll_step, command == 0x429B ? 0xCCE0187C : 0x68B7B136);
        }
        beam = effect;
        if (command == 0x429B)
        {
            if (!beam)
            {
                GameDiagnostic(("Controller beam allocation failed\n"));
                abort();
            }
            beam->persistent = true;
        }
        cursor = script_cursor;
        script_cursor = cursor + 12;
        return 1;
    }
    if (command == 0x429F)
    {
        if (beam)
            beam->Destroy(3);
        beam = 0;
        return 1;
    }
    if (command == 0x4500 || command == 0x4501 || command == 0x4502 || command == 0x4505)
    {
        allocated = AllocateZeroedHeapBlock(40);
        if (!allocated)
        {
            GameDiagnostic(("Object force allocation failed\n"));
            abort();
        }
        force = new (allocated) ObjectForce(command == 0x4505 ? 3 : command - 0x4500, this, &visual_lists.players, 0);
        force->SetPos(*this);
        force->SetDir(model.angles);
        return 1;
    }
    return Enemy::ExecuteCommand(command);
}

void ScriptedEffectController::ReleaseChildren()
{
    if (sound)
        StopSoundVoices(sound);
    sound = 0;
    if (force)
        force->Destroy(3);
    force = 0;
    if (beam)
        beam->Destroy(3);
    beam = 0;
    for (uint32 i = 0; i < rain_count; ++i)
        if (rain[i].effect)
            rain[i].effect->Destroy(3);
    delete[] rain;
    rain = 0;
    rain_count = 0;
}

ScriptedEffectController::~ScriptedEffectController()
{
    ReleaseChildren();
}
