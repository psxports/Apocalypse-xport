#include <new>
#include <stdio.h>
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
#include "game_random.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "object_force.h"
#include "effect_segments.h"
#include "waving_beam.h"
#include "level_hazards.h"

ScriptedEffectController::ScriptedEffectController(const RecordCursor &record_data, uint32 index) : Enemy(), rain_count(0), rain(0), beam(), sound(0), sound_period(0xFFFFFFFF)
{
    RecordDataBorrow record_borrow;
    uint32 flags, activation;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004D604, "SLUS_003.73");

    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    this->control.script_active = 1;
    flags = this->model.flags;
    activation = lifecycle_flags;
    record_idx = index;
    this->model.flags = flags | 1;
    lifecycle_flags = activation & 65519;
    AddToList(visual_lists.actors);
    this->collision_radius = 0;
    kind = 203;
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

uint32 ScriptedEffectController::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004D6B4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    ReleaseChildren();
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(this);
    return 0;
}

void ScriptedEffectController::SetParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, mode;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004DB90, "SLUS_003.73");
    command &= 65535;
    if (command == 8960)
    {
        if (force)
        {
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2;
            force->SetRadius(value);
        }
    }
    else if (command == 8961)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        script_cursor = cursor;
        mode = (cursor)[0];
        cursor += 2;
        script_cursor = cursor;
        if (force)
            force->SetStrength(value, mode);
    }
    else
        Enemy::ReadParameters(command);
}

uint32 ScriptedEffectController::Update()
{
    GameHeapBlock *reservation;
    RecordDataBorrow record_borrow;
    uint32 flags, counter, timer;
    RecordCursor script;
    uint32 count, period, frame, remainder, index;
    uint32 height, one;
    Vector3 offset, position, relative, scale;
    FUNCTION_MARKER(0x8004D800, "SLUS_003.73");
    flags = activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1)
    {
        counter = (uint32)this->control.activation_count;
        activation_flags = flags & 65534;
        this->control.activation_count = counter + 1;
    }
    if ((uint32)this->control.script_active)
    {
        timer = (uint32)this->control.script_delay;
        if (timer)
            this->control.script_delay = timer - 1;
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
    if ((this->control.flags & 256) && !RandomRange(4))
    {
        use_small_effect_pool = false;
        count = RandomRange(3) + 2;
        while (count)
        {
            position = this->model.pos;
            position.words[0] += (RandomRange(128) - 64) << 12;
            position.words[1] += (RandomRange(128) - 64) << 12;
            position.words[2] += (RandomRange(128) - 64) << 12;
            reservation = AllocateEffectStorage(116);
            // Original computes jitter but passes the object position
            if (reservation)
            {
                new (reservation) PanelParticle(this->model.pos.words);
            }
            count -= 1;
        }
        use_small_effect_pool = true;
    }
    period = sound_period;
    if (period != 0xFFFFFFFF)
    {
        frame = game_clock.tick;
        if (!period)
            remainder = frame;
        else if (frame == 0x80000000 && period == 0xFFFFFFFF)
            remainder = 0;
        else
            remainder = (uint32)((sint32)frame % (sint32)period);
        if (!remainder)
            UpdatePositionedSoundVoices(sound, model.pos.words, 0);
    }
    if (force)
        force->Update();
    if (!rain)
        return 0;
    height = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words[1];
    for (index = 0; index < rain_count; ++index)
    {
        RainDrop *drop = rain + index;
        SegmentEffect *effect = drop->effect.Get();
        if (!effect)
            continue;
        for (uint32 axis = 0; axis < 3; ++axis)
            position.words[axis] = effect->origin.words[axis];
        AddVectors(position.words, drop->velocity.words);
        if ((sint32)height < (sint32)position.words[1])
        {
            offset.words[0] = (RandomRange(2400) - 1200) << 12;
            offset.words[1] = (RandomRange(400) - 1200) << 12;
            offset.words[2] = (RandomRange(2400) - 1200) << 12;
            AddVectors(position.words, visual_lists.players->pos.words, offset.words);
        }
        for (uint32 axis = 0; axis < 3; ++axis)
            effect->origin.words[axis] = position.words[axis];
        SubtractVectors(relative.words, position.words, drop->velocity.words);
        one = 1;
        ShiftVectorRight(scale.words, static_cast<Camera *>(visual_lists.cameras->owner)->motion.velocity.words, one);
        AddVectors(effect->points[0].pos.words, relative.words, scale.words);
    }
    return 0;
}

uint32 ScriptedEffectController::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8004DC40, "SLUS_003.73");
    uint32 event = command & 65535;
    GameHeapBlock *allocated;
    uint32 value;
    uint32 sound_id;
    RecordCursor pointer;
    if (event == 0x4503)
    {
        uint32 count = ReadScriptValue() & 65535;
        uint32 shift;
        Vector3 origin, position, difference, scaled;
        rain_count = count;
        rain = new RainDrop[count];
        use_small_effect_pool = false;
        for (uint32 axis = 0; axis < 3; ++axis)
            origin.words[axis] = visual_lists.players->pos.words[axis];
        for (uint32 i = 0; i < rain_count; ++i)
        {
            RainDrop *drop = rain + i;
            position.words[0] = (RandomRange(2400) - 1200) << 12;
            position.words[1] = (RandomRange(400) - 1200) << 12;
            position.words[2] = (RandomRange(2400) - 1200) << 12;
            AddVectors(position.words, origin.words);
            drop->velocity.words[0] = (RandomRange(16) - 8) << 12;
            drop->velocity.words[1] = (RandomRange(48) + 48) << 12;
            drop->velocity.words[2] = (RandomRange(16) - 8) << 12;
            allocated = AllocateEffectStorage(100);
            if (!allocated)
            {
                GameDiagnostic("Rain segment allocation failed\n");
                abort();
            }
            SegmentEffect *effect = new (allocated) SegmentEffect(1);
            drop->effect = effect;
            effect->persistent = 1;
            for (uint32 axis = 0; axis < 3; ++axis)
                effect->origin.words[axis] = position.words[axis];
            SubtractVectors(difference.words, position.words, drop->velocity.words);
            shift = 1;
            ShiftVectorRight(scaled.words, static_cast<Camera *>(visual_lists.cameras->owner)->motion.velocity.words, shift);
            AddVectors(effect->points[0].pos.words, difference.words, scaled.words);
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
    if (event == 0x4507 || event == 0x4508 || event == 0x4509)
    {
        if (sound)
            StopSoundVoices(sound);
        if (event == 0x4509)
            sound = 0;
        else
        {
            pointer = script_cursor;
            value = pointer.Read16();
            sound_id = value;
            script_cursor = pointer + 2;
            if (event == 0x4507)
            {
                sound = PlaySoundEffect(value, 8192, 0);
                sound_period = 0xFFFFFFFF;
            }
            else
            {
                pointer += 2;
                value = pointer.Read16();
                script_cursor = pointer + 2;
                sound_period = value;
                sound = PlayPositionedSound(sound_id, model.pos.words, 0);
            }
        }
        return 1;
    }
    if (event == 0x4700)
    {
        pointer = script_cursor;
        value = pointer.Read16();
        script_cursor = pointer + 2;
        this->control.flags = value ? this->control.flags | 0x100 : this->control.flags & ~0x100;
        return 1;
    }
    if (event == 0x4506)
    {
        ModelInstance *player = visual_lists.players;
        if (player)
        {
            uint32 x = player->pos.words[0], y = player->pos.words[1], z = player->pos.words[2];
            this->model.pos.words[0] = x;
            this->model.pos.words[1] = y;
            this->model.pos.words[2] = z;
        }
        return 1;
    }
    if (event == 0x4504)
    {
        uint32 p0, p1, p2, p3;
        ModelInstance *player;
        pointer = script_cursor;
        player = visual_lists.players;
        p0 = pointer.Read16();
        script_cursor = pointer + 2;
        p1 = (pointer + 2).Read16();
        script_cursor = pointer + 4;
        p2 = (pointer + 4).Read16();
        script_cursor = pointer + 6;
        p3 = (pointer + 6).Read16();
        script_cursor = pointer + 8;
        if (player && MeasureHorizontalDistance(model.pos.words, player->pos.words) < 4096)
        {
            allocated = AllocateZeroedObject(504);
            if (allocated)
            {
                new (allocated) BallisticHazard(this->model.pos.words, p0, p1, p2, (sint16)p3);
            }
        }
        return 1;
    }
    if (event == 0x429B || event == 0x42A1)
    {
        uint32 kind, p5, p6, p7, p8, p9;
        WavingBeam *effect = 0;
        allocated = AllocateEffectStorage(364);
        if (allocated)
        {
            kind = xport_load_le16(script_cursor);
            p5 = xport_load_le16(script_cursor + 2);
            p6 = xport_load_le16(script_cursor + 4);
            p7 = xport_load_le16(script_cursor + 6);
            p8 = xport_load_le16(script_cursor + 8);
            p9 = (script_cursor + 10)[0];
            effect = new (allocated) WavingBeam(this->model.pos.words, this->model.angles.words, kind, 65535, p5, p6, p7, p8, p9, event == 0x429B ? 0xCCE0187C : 0x68B7B136);
        }
        beam = effect;
        if (event == 0x429B)
        {
            if (!beam)
            {
                GameDiagnostic("Controller beam allocation failed\n");
                abort();
            }
            beam->persistent = 1;
        }
        pointer = script_cursor;
        script_cursor = pointer + 12;
        return 1;
    }
    if (event == 0x429F)
    {
        if (beam)
            beam->Destroy(3);
        beam = 0;
        return 1;
    }
    if (event == 0x4500 || event == 0x4501 || event == 0x4502 || event == 0x4505)
    {
        allocated = AllocateZeroedHeapBlock(40);
        if (!allocated)
        {
            GameDiagnostic("Object force allocation failed\n");
            abort();
        }
        force = new (allocated) ObjectForce(event == 0x4505 ? 3 : event - 0x4500, this, &visual_lists.players, 0);
        force->SetPosition(*this);
        force->SetDirection(model.angles.words);
        return 1;
    }
    return Enemy::ExecuteCommand(event);
}
