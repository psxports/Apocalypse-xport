#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "scripted_effect_controller.h"
#include "panel_particle.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "object_force.h"
#include "effect_segments.h"
#include "waving_beam.h"
#include "level_hazards.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

ScriptedEffectController::ScriptedEffectController(const uint8 *record_data, uint32 index) : Enemy()
{
    uint32 object = NativeObjectGuestAddress(this), flags, activation;
    const uint8 *cursor;
    FUNCTION_MARKER(0x8004D604, "SLUS_003.73");
    // TODO Support native script storage before accepting unmapped record data
    if (!GuestMemoryAddress(record_data))
    {
        GameDiagnostic("Enemy203 record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A2AC0);
    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    w_u32(object + 400, GuestMemoryAddress(cursor));
    w_u8(object + 380, 1);
    flags = r_u16(object);
    w_u32(object + 516, 0xFFFFFFFF);
    activation = r_u16(object + 78);
    w_u16(object + 214, index);
    w_u16(object, flags | 1);
    w_u16(object + 78, activation & 65519);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    w_u16(object + 212, 0);
    w_u16(object + 58, 203);
}

ScriptedEffectController &ScriptedEffectController::Attach(uint32 object)
{
    return *static_cast<ScriptedEffectController *>(FindNativeObjectStorage(object));
}

uint32 ScriptedEffectController::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this), child, table, receiver, target, index = 0;
    FUNCTION_MARKER(0x8004D6B4, "SLUS_003.73");
    w_u32(object + 68, 0x800A2AC0);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    child = r_u32(object + 512);
    if (child)
        StopSoundVoices(child);
    child = r_u32(object + 496);
    if (child)
    {
        table = r_u32(child);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    child = r_u32(object + 508);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    if (r_u32(object + 504))
    {
        while ((sint32)index < (sint32)r_u16(object + 500))
        {
            child = r_u32(r_u32(object + 504) + index * 16 + 12);
            if (child)
            {
                table = r_u32(child + 68);
                receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
                target = r_u32(table + 12);
                apocalypse_object_cleanup(target, receiver, 3);
            }
            index += 1;
        }
        ReleaseHeapBlock(r_u32(object + 504));
    }
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(object);
    return 0;
}

void ScriptedEffectController::SetParameters(uint32 command)
{
    uint32 object = NativeObjectGuestAddress(this), child, cursor, value, mode;
    FUNCTION_MARKER(0x8004DB90, "SLUS_003.73");
    command &= 65535;
    if (command == 8960)
    {
        if (r_u32(object + 496))
        {
            cursor = r_u32(object + 400);
            child = r_u32(object + 496);
            value = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            ObjectForce::Attach(child).SetRadius(value);
        }
    }
    else if (command == 8961)
    {
        cursor = r_u32(object + 400);
        child = r_u32(object + 496);
        value = r_u16(cursor);
        cursor += 2;
        w_u32(object + 400, cursor);
        mode = r_u8(cursor);
        cursor += 2;
        w_u32(object + 400, cursor);
        if (child)
            ObjectForce::Attach(child).SetStrength((uint32)(sint32)(sint16)value, mode);
    }
    else
        Enemy::ReadParameters(command);
}

uint32 ScriptedEffectController::Damage(uint32 damage, const sint32 *vector, uint32 kind)
{
    FUNCTION_MARKER(0x80062F5C, "SLUS_003.73");
    return 1;
}

uint32 ScriptedEffectController::Update()
{
    uint32 object = NativeObjectGuestAddress(this), flags, counter, timer, script;
    uint32 table, adjustment, target, count, effect, period, frame, remainder, child, index;
    uint32 player, height, entry, output, offset[3], position[3], temporary[3], relative[3], scale[3], one;
    FUNCTION_MARKER(0x8004D800, "SLUS_003.73");
    flags = r_u16(object + 76);
    w_u16(object + 388, flags);
    if (flags & 1)
    {
        counter = r_u8(object + 384);
        w_u16(object + 76, flags & 65534);
        w_u8(object + 384, counter + 1);
    }
    if (r_u8(object + 380))
    {
        timer = r_u16(object + 476);
        if (timer)
            w_u16(object + 476, timer - 1);
        else
        {
            script = r_u32(object + 400);
            ExecuteScript((const uint16 *)psx_addr(script, 2));
        }
    }
    else
    {
        table = r_u32(object + 68);
        adjustment = (uint32)(sint32)(sint16)r_u16(table + 16);
        target = r_u32(table + 20);
        (void)DispatchObjectFinish(target, object + adjustment);
    }
    if ((r_u32(object + 396) & 256) && !RandomRange(4))
    {
        w_u32(0x800FF3AC, 0);
        count = RandomRange(3) + 2;
        while (count)
        {
            position[0] = r_u32(object + 4);
            position[1] = r_u32(object + 8);
            position[2] = r_u32(object + 12);
            position[0] += (RandomRange(128) - 64) << 12;
            position[1] += (RandomRange(128) - 64) << 12;
            position[2] += (RandomRange(128) - 64) << 12;
            effect = AllocateEffectStorage(116);
            // Original computes jitter but passes the object position
            if (effect)
            {
                void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(PanelParticle));
                new (dram_addr) PanelParticle((const uint32 *)psx_addr(object + 4, 12));
            }
            count -= 1;
        }
        w_u32(0x800FF3AC, 1);
    }
    period = r_u32(object + 516);
    if (period != 0xFFFFFFFF)
    {
        frame = r_u32(0x800FF2F0);
        if (!period)
            remainder = frame;
        else if (frame == 0x80000000 && period == 0xFFFFFFFF)
            remainder = 0;
        else
            remainder = (uint32)((sint32)frame % (sint32)period);
        if (!remainder)
            UpdatePositionedSoundVoices(r_u32(object + 512), (const uint32 *)psx_addr(object + 4, 12), 0);
    }
    child = r_u32(object + 496);
    if (child)
        ObjectForce::Attach(child).Update();
    if (!r_u32(object + 504))
        return 0;
    player = r_u32(0x800FF5A0);
    height = r_u32(player + 188);
    index = 0;
    while ((sint32)index < (sint32)r_u16(object + 500))
    {
        entry = r_u32(object + 504) + index * 16;
        child = r_u32(entry + 12);
        position[0] = r_u32(child + 80);
        position[1] = r_u32(child + 84);
        position[2] = r_u32(child + 88);
        AddVectors(position, (const uint32 *)psx_addr(entry, 12));
        if ((sint32)height < (sint32)position[1])
        {
            offset[0] = (RandomRange(2400) - 1200) << 12;
            offset[1] = (RandomRange(400) - 1200) << 12;
            offset[2] = (RandomRange(2400) - 1200) << 12;
            player = r_u32(0x800FF5A0);
            AddVectors(temporary, (const uint32 *)psx_addr(player + 4, 12), offset);
            position[0] = temporary[0];
            position[1] = temporary[1];
            position[2] = temporary[2];
        }
        child = r_u32(entry + 12);
        w_u32(child + 80, position[0]);
        w_u32(child + 84, position[1]);
        w_u32(child + 88, position[2]);
        child = r_u32(entry + 12);
        output = r_u32(child + 76);
        index += 1;
        SubtractVectors(relative, position, (const uint32 *)psx_addr(entry, 12));
        one = 1;
        ShiftVectorRight(scale, (const uint32 *)psx_addr(r_u32(0x800FF904) + 104, 12), &one);
        AddVectors(offset, relative, scale);
        w_u32(output, offset[0]);
        w_u32(output + 4, offset[1]);
        w_u32(output + 8, offset[2]);
    }
    return 0;
}

uint32 ScriptedEffectController::ExecuteCommand(uint32 command)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004DC40, "SLUS_003.73");
    uint32 event = command & 65535;
    uint32 pointer, allocated, value, sound, sound_id;
    if (event == 0x4503)
    {
        uint32 count = ReadScriptValue() & 65535, index = 0;
        uint32 origin[3], position[3], difference[3], scaled[3], output[3];
        uint32 player;
        uint32 shift;
        w_u16(object + 500, count);
        allocated = AllocateHeapBlock(count * 16, 0, 1);
        player = r_u32(0x800FF5A0);
        w_u32(object + 504, allocated);
        w_u32(0x800FF3AC, 0);
        origin[0] = r_u32(player + 4);
        origin[1] = r_u32(player + 8);
        origin[2] = r_u32(player + 12);
        while ((sint32)index < (sint32)r_u16(object + 500))
        {
            uint32 entry = r_u32(object + 504) + 16 * index;
            uint32 x = (RandomRange(2400) - 1200) << 12;
            uint32 y = (RandomRange(400) - 1200) << 12;
            uint32 z = (RandomRange(2400) - 1200) << 12;
            position[0] = x;
            position[1] = y;
            position[2] = z;
            AddVectors(position, origin);
            x = (RandomRange(16) - 8) << 12;
            y = (RandomRange(48) + 48) << 12;
            z = (RandomRange(16) - 8) << 12;
            w_u32(entry, x);
            w_u32(entry + 4, y);
            w_u32(entry + 8, z);
            allocated = AllocateEffectStorage(100);
            if (allocated)
            {
                void *dram_addr = AllocateNativeObjectStorage(allocated, sizeof(SegmentEffect));
                new (dram_addr) SegmentEffect(1);
            }
            w_u32(entry + 12, allocated);
            w_u8(allocated + 66, 1);
            w_u32(allocated + 80, position[0]);
            w_u32(allocated + 84, position[1]);
            w_u32(allocated + 88, position[2]);
            pointer = r_u32(allocated + 76);
            SubtractVectors(difference, position, (const uint32 *)psx_addr(entry, 12));
            ++index;
            shift = 1;
            ShiftVectorRight(scaled, (const uint32 *)psx_addr(r_u32(0x800FF904) + 104, 12), &shift);
            AddVectors(output, difference, scaled);
            w_u32(pointer, output[0]);
            w_u32(pointer + 4, output[1]);
            w_u32(pointer + 8, output[2]);
            w_u8(allocated + 92, 64);
            w_u8(allocated + 93, 64);
            w_u8(allocated + 94, 80);
            w_u8(r_u32(allocated + 76) + 12, 16);
            w_u8(r_u32(allocated + 76) + 13, 16);
            w_u8(r_u32(allocated + 76) + 14, 32);
            SegmentEffect::Attach(allocated).Enable();
        }
        w_u32(0x800FF3AC, 1);
        return 1;
    }
    if (event == 0x4507 || event == 0x4508 || event == 0x4509)
    {
        sound = r_u32(object + 512);
        if (sound)
            StopSoundVoices(sound);
        if (event == 0x4509)
            w_u32(object + 512, 0);
        else
        {
            pointer = r_u32(object + 400);
            value = r_u16(pointer);
            sound_id = value;
            w_u32(object + 400, pointer + 2);
            if (event == 0x4507)
            {
                w_u32(object + 512, PlaySoundEffect(value, 8192, 0));
                w_u32(object + 516, 0xFFFFFFFF);
            }
            else
            {
                pointer += 2;
                value = r_u16(pointer);
                w_u32(object + 400, pointer + 2);
                w_u32(object + 516, value);
                w_u32(object + 512, PlayPositionedSound(sound_id, (const uint32 *)psx_addr(object + 4, 12), 0));
            }
        }
        return 1;
    }
    if (event == 0x4700)
    {
        pointer = r_u32(object + 400);
        value = r_u16(pointer);
        w_u32(object + 400, pointer + 2);
        w_u32(object + 396, value ? r_u32(object + 396) | 0x100 : r_u32(object + 396) & ~0x100);
        return 1;
    }
    if (event == 0x4506)
    {
        uint32 player = r_u32(0x800FF5A0);
        if (player)
        {
            uint32 x = r_u32(player + 4), y = r_u32(player + 8), z = r_u32(player + 12);
            w_u32(object + 4, x);
            w_u32(object + 8, y);
            w_u32(object + 12, z);
        }
        return 1;
    }
    if (event == 0x4504)
    {
        uint32 p0, p1, p2, p3, player;
        pointer = r_u32(object + 400);
        player = r_u32(0x800FF5A0);
        p0 = r_u16(pointer);
        w_u32(object + 400, pointer + 2);
        p1 = r_u16(pointer + 2);
        w_u32(object + 400, pointer + 4);
        p2 = r_u16(pointer + 4);
        w_u32(object + 400, pointer + 6);
        p3 = r_u16(pointer + 6);
        w_u32(object + 400, pointer + 8);
        if (player && MeasureHorizontalDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(player + 4, 12)) < 4096)
        {
            allocated = sub_800625AC(504);
            if (allocated)
            {
                void *dram_addr = AllocateNativeObjectStorage(allocated, sizeof(BallisticHazard));
                new (dram_addr) BallisticHazard((const uint32 *)psx_addr(object + 4, 12), p0, p1, p2, (sint16)p3);
            }
        }
        return 1;
    }
    if (event == 0x429B || event == 0x42A1)
    {
        uint32 kind, p5, p6, p7, p8, p9;
        allocated = AllocateEffectStorage(364);
        if (allocated)
        {
            kind = r_u16(r_u32(object + 400));
            p5 = r_u16(r_u32(object + 400) + 2);
            p6 = r_u16(r_u32(object + 400) + 4);
            p7 = r_u16(r_u32(object + 400) + 6);
            p8 = r_u16(r_u32(object + 400) + 8);
            p9 = r_u8(r_u32(object + 400) + 10);
            void *dram_addr = AllocateNativeObjectStorage(allocated, sizeof(WavingBeam));
            new (dram_addr) WavingBeam((const uint32 *)psx_addr(object + 4, 12), (const uint16 *)psx_addr(object + 16, 6), kind, 65535, p5, p6, p7, p8, p9, event == 0x429B ? 0xCCE0187C : 0x68B7B136);
        }
        if (event == 0x429B)
        {
            w_u32(object + 508, allocated);
            w_u8(allocated + 66, 1);
            pointer = r_u32(object + 400);
        }
        else
        {
            pointer = r_u32(object + 400);
            w_u32(object + 508, allocated);
        }
        w_u32(object + 400, pointer + 12);
        return 1;
    }
    if (event == 0x429F)
    {
        allocated = r_u32(object + 508);
        if (allocated)
        {
            uint32 table = r_u32(allocated + 68);
            uint32 receiver = allocated + (uint32)(sint32)(sint16)r_u16(table + 8);
            uint32 target = r_u32(table + 12);
            apocalypse_object_cleanup(target, receiver, 3);
        }
        w_u32(object + 508, 0);
        return 1;
    }
    if (event == 0x4500 || event == 0x4501 || event == 0x4502 || event == 0x4505)
    {
        allocated = AllocateZeroedHeapBlock(40);
        if (allocated)
        {
            void *dram_addr = AllocateNativeObjectStorage(allocated, sizeof(ObjectForce));
            new (dram_addr) ObjectForce(event == 0x4505 ? 3 : event - 0x4500, object, 0x800FF5A0, 0);
        }
        w_u32(object + 496, allocated);
        ObjectForce::Attach(allocated).SetPosition(object + 4);
        ObjectForce::Attach(r_u32(object + 496)).SetDirection((const uint16 *)psx_addr(object + 16, 4));
        return 1;
    }
    return Enemy::ExecuteCommand(event);
}

extern "C" uint32 sub_8004D604(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8004D604, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ScriptedEffectController));
    new (dram_addr) ScriptedEffectController((const uint8 *)psx_addr(data, 1), index);
    return object;
}

extern "C" uint32 sub_8004D6B4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8004D6B4, "SLUS_003.73");
    return ScriptedEffectController::Attach(object).ScriptedEffectController::Destroy(reason);
}
extern "C" void sub_8004DB90(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8004DB90, "SLUS_003.73");
    ScriptedEffectController::Attach(object).ScriptedEffectController::SetParameters(command);
}

extern "C" uint32 sub_8004D800(uint32 object)
{
    FUNCTION_MARKER(0x8004D800, "SLUS_003.73");
    return ScriptedEffectController::Attach(object).ScriptedEffectController::Update();
}

extern "C" uint32 sub_8004DC40(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8004DC40, "SLUS_003.73");
    return ScriptedEffectController::Attach(object).ScriptedEffectController::ExecuteCommand(command);
}
