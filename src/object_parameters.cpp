#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "enemy.h"
#include "scripted_level_object.h"
#include "chopper.h"
#include "croc.h"
#include "scripted_effect_controller.h"
#include "plague.h"
#include "death.h"
#include "war.h"
#include "the_beast.h"
#include "object_parameters.h"
#include "game_random.h"
#include "record_data.h"
#include "model_resources.h"
#include "object_force.h"
#include <stdio.h>
#include <stdlib.h>

uint32 DispatchObjectParameterGetter(uint32 target, uint32 receiver, uint32 command)
{
    switch (target)
    {
        case 0x8004D2B4:
            return Enemy::Attach(receiver).Enemy::ResolveParameter(command);
        case 0x8003D0F0:
            return ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::ResolveParameter(command);
        default:
            GameDiagnostic("Unimplemented object parameter getter %08X receiver %08X command %08X\n", target, receiver, command);
            abort();
    }
}

// Native adapter for original setter calls whose return register is discarded
void DispatchObjectParameterWriter(uint32 target, uint32 receiver, uint32 command)
{
    switch (target)
    {
        case 0x8004CFDC:
            Enemy::Attach(receiver).Enemy::ReadParameters(command);
            return;
        case 0x8003CF3C:
            ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::SetScriptParameters(command);
            return;
        case 0x8004DB90:
            ScriptedEffectController::Attach(receiver).ScriptedEffectController::SetParameters(command);
            return;
        case 0x80053F48:
            Chopper::Attach(receiver).Chopper::SetParameters(command);
            return;
        case 0x80072204:
            Croc::Attach(receiver).Croc::SetParameters(command);
            return;
        case 0x800474F0:
            Plague::Attach(receiver).Plague::ReadParameters(command);
            return;
        case 0x80049B38:
            Death::Attach(receiver).Death::ReadParameters(command);
            return;
        case 0x8003FCEC:
            War::Attach(receiver).War::ReadParameters(command);
            return;
        case 0x80044474:
            TheBeast::Attach(receiver).TheBeast::SetScriptParameters(command);
            return;
        default:
            GameDiagnostic("TODO Native parameter writer %08X receiver %08X command %08X\n", target, receiver, command);
            abort();
    }
}

// Native command buffer adapter preserves existing supported writer contracts
static uint32 ReadObjectParameterWord(const uint16 *words, uint32 count, uint32 index)
{
    if (index >= count)
    {
        GameDiagnostic("TODO Native parameter word %u exceeds supplied count %u\n", index, count);
        abort();
    }
    return words[index];
}

static uint32 ReadObjectParameter(uint32 object, uint32 command, const uint16 *words, uint32 count, uint32 *cursor);

static uint32 ReadVirtualObjectParameterForOwner(uint32 buffer_owner, uint32 object, uint32 command, const uint16 *words, uint32 count, uint32 *cursor)
{
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
    uint32 target = r_u32(table + 84);
    if (target != 0x8004D2B4 && target != 0x8003D0F0)
    {
        GameDiagnostic("TODO Native parameter getter %08X receiver %08X\n", target, receiver);
        abort();
    }
    if (receiver != buffer_owner)
    {
        if ((command & 0xFFFF) == 0x212B)
        {
            uint32 stream = r_u32(receiver + 400);
            uint32 value = r_u16(stream);
            w_u32(receiver + 400, stream + 2);
            if (value & 0x2000)
                value = ReadVirtualObjectParameterForOwner(buffer_owner, receiver, value, words, count, cursor);
            uint32 record = GetRecordPayload(value & 0xFFFF);
            value = r_u16(record) ? r_u16(record + 2) : 0;
            return target == 0x8003D0F0 ? (uint32)(sint32)(sint16)value : value;
        }
        // The adjusted receiver owns its guest stream rather than this buffer
        return DispatchObjectParameterGetter(target, receiver, command);
    }
    if (target == 0x8003D0F0)
    {
        if ((command & 0xFFFF) == 0x2200)
            return r_u32(receiver + 516);
        return (uint32)(sint32)(sint16)ReadObjectParameter(receiver, command, words, count, cursor);
    }
    return ReadObjectParameter(receiver, command, words, count, cursor);
}

static uint32 ReadVirtualObjectParameter(uint32 object, uint32 command, const uint16 *words, uint32 count, uint32 *cursor)
{
    return ReadVirtualObjectParameterForOwner(object, object, command, words, count, cursor);
}

static uint32 ReadObjectParameter(uint32 object, uint32 command, const uint16 *words, uint32 count, uint32 *cursor)
{
    uint32 value, record;
    switch (command & 0xFFFF)
    {
        case 0x2120:
            value = ReadObjectParameterWord(words, count, *cursor) & 255;
            ++*cursor;
            return (uint32)(sint32)(sint16)r_u16(object + 460 + 2 * value);
        case 0x2129:
            value = ReadObjectParameterWord(words, count, *cursor);
            ++*cursor;
            return RandomRange(value);
        case 0x212B:
            value = ReadObjectParameterWord(words, count, *cursor);
            ++*cursor;
            if (value & 0x2000)
                value = ReadVirtualObjectParameter(object, value, words, count, cursor);
            record = GetRecordPayload(value & 0xFFFF);
            return r_u16(record) ? r_u16(record + 2) : 0;
        default:
            return Enemy::Attach(object).Enemy::ResolveParameter(command);
    }
}

// Native extraction of the original script-value reader
static uint32 ReadBufferedScriptValue(uint32 object, const uint16 *words, uint32 count, uint32 *cursor)
{
    uint32 value = ReadObjectParameterWord(words, count, *cursor);
    ++*cursor;
    if (!(value & 0x8000) && (value & 0x2000))
        value = ReadVirtualObjectParameter(object, value, words, count, cursor);
    return value & 0xFFFF;
}

// Native context adapter for a writer with a separate guest stream owner
static uint32 ReadGuestScriptValueForOwner(uint32 buffer_owner, uint32 object, const uint16 *words, uint32 count, uint32 *cursor)
{
    uint32 stream = r_u32(object + 400);
    uint32 value = r_u16(stream);
    w_u32(object + 400, stream + 2);
    if (!(value & 0x8000) && (value & 0x2000))
        value = ReadVirtualObjectParameterForOwner(buffer_owner, object, value, words, count, cursor);
    return value & 0xFFFF;
}

void DispatchBufferedObjectParameterWriter(uint32 target, uint32 buffer_owner, uint32 receiver, uint32 command, const uint16 *words, uint32 count)
{
    if (receiver == buffer_owner)
    {
        apocalypse_object_parameter_write_native(target, receiver, command, words, count);
        return;
    }
    command &= 0xFFFF;
    switch (target)
    {
        case 0x8004CFDC:
        case 0x8003CF3C:
            break;
        case 0x80053F48:
            if (command == 0x2130)
            {
                Chopper::Attach(receiver).Chopper::SetParameters(command);
                return;
            }
            break;
        case 0x80072204:
            if (command == 0x2130)
            {
                Croc::Attach(receiver).Croc::SetParameters(command);
                return;
            }
            break;
        case 0x8004DB90:
            if (command == 0x2300 || command == 0x2301)
            {
                ScriptedEffectController::Attach(receiver).ScriptedEffectController::SetParameters(command);
                return;
            }
            break;
        case 0x800474F0:
            if (command == 0x2130)
            {
                Plague::Attach(receiver).Plague::ReadParameters(command);
                return;
            }
            break;
        case 0x80049B38:
            if (command == 0x2130)
            {
                Death::Attach(receiver).Death::ReadParameters(command);
                return;
            }
            break;
        case 0x8003FCEC:
            if (command == 0x2130)
            {
                War::Attach(receiver).War::ReadParameters(command);
                return;
            }
            break;
        case 0x80044474:
            if (command == 0x2130)
            {
                TheBeast::Attach(receiver).TheBeast::SetScriptParameters(command);
                return;
            }
            break;
        default:
            GameDiagnostic("TODO Adjusted native parameter writer %08X receiver %08X command %08X\n", target, receiver, command);
            abort();
    }
    uint32 cursor = 0;
    uint32 first, second, third, destination, stream, index;
    if (target == 0x8003CF3C && (command == 0x2127 || command == 0x2128 || command == 0x2134 || command == 0x2137))
    {
        first = ReadGuestScriptValueForOwner(buffer_owner, receiver, words, count, &cursor);
        second = ReadGuestScriptValueForOwner(buffer_owner, receiver, words, count, &cursor);
        third = ReadGuestScriptValueForOwner(buffer_owner, receiver, words, count, &cursor);
        if (command == 0x2134)
        {
            w_u32(receiver + 104, (uint32)((sint32)(first << 16) >> 4));
            w_u32(receiver + 108, (uint32)((sint32)(second << 16) >> 4));
            w_u32(receiver + 112, (uint32)((sint32)(third << 16) >> 4));
        }
        else
        {
            destination = receiver + (command == 0x2127 ? 132 : command == 0x2128 ? 138 : 16);
            w_u16(destination, first);
            w_u16(destination + 2, second);
            w_u16(destination + 4, third);
        }
        return;
    }
    if (command == 0x2120)
    {
        stream = r_u32(receiver + 400);
        index = r_u16(stream) & 255;
        w_u32(receiver + 400, stream + 2);
        first = ReadGuestScriptValueForOwner(buffer_owner, receiver, words, count, &cursor);
        w_u16(receiver + 460 + index * 2, first);
        return;
    }
    if (command >= 0x2140 && command <= 0x2142)
    {
        first = ReadGuestScriptValueForOwner(buffer_owner, receiver, words, count, &cursor);
        w_u32(receiver + 4 + (command - 0x2140) * 4, (uint32)((sint32)(first << 16) >> 4));
        return;
    }
    if (target == 0x8003CF3C)
        ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::SetScriptParameters(command);
    else
        Enemy::Attach(receiver).Enemy::ReadParameters(command);
}

// Native Record402 writer retains the original payload reads and store order
static bool WriteScriptedLevelObjectParameters(uint32 object, uint32 command, const uint16 *words, uint32 count)
{
    uint32 value, bank, flags, first, second, third, destination, cursor = 0;
    switch (command)
    {
        case 0x2123:
            w_u8(object + 382, ReadObjectParameterWord(words, count, 0));
            return true;
        case 0x2124:
            w_u16(object + 22, ReadObjectParameterWord(words, count, 0));
            return true;
        case 0x212F:
            bank = r_u8(object + 27);
            value = ReadObjectParameterWord(words, count, 0);
            value |= ReadObjectParameterWord(words, count, 1) << 16;
            value = FindModelResourceIndex(value, bank);
            w_u16(object + 22, value);
            flags = r_u16(object);
            w_u16(object, flags & 0xFFFE);
            return true;
        case 0x2134:
        case 0x2127:
        case 0x2128:
        case 0x2137:
            first = ReadBufferedScriptValue(object, words, count, &cursor);
            second = ReadBufferedScriptValue(object, words, count, &cursor);
            third = ReadBufferedScriptValue(object, words, count, &cursor);
            if (command == 0x2134)
            {
                w_u32(object + 104, (uint32)((sint32)(first << 16) >> 4));
                w_u32(object + 108, (uint32)((sint32)(second << 16) >> 4));
                w_u32(object + 112, (uint32)((sint32)(third << 16) >> 4));
            }
            else
            {
                destination = object + (command == 0x2127 ? 132 : command == 0x2128 ? 138 : 16);
                w_u16(destination, first);
                w_u16(destination + 2, second);
                w_u16(destination + 4, third);
            }
            return true;
        default:
            return false;
    }
}

// Native extraction of the Enemy203 child-force parameter commands
static bool WriteScriptedEffectControllerParameters(uint32 object, uint32 command, const uint16 *words, uint32 count)
{
    uint32 child, value, mode;
    if (command == 0x2300)
    {
        if (r_u32(object + 496))
        {
            child = r_u32(object + 496);
            value = ReadObjectParameterWord(words, count, 0);
            ObjectForce::Attach(child).SetRadius(value);
        }
        return true;
    }
    if (command == 0x2301)
    {
        child = r_u32(object + 496);
        value = ReadObjectParameterWord(words, count, 0);
        mode = ReadObjectParameterWord(words, count, 1) & 255;
        if (child)
            ObjectForce::Attach(child).SetStrength((uint32)(sint32)(sint16)value, mode);
        return true;
    }
    return false;
}

// Native buffer extraction omits the temporary guest stream pointer
static void WriteBufferedParameterRange(uint32 destination, const uint16 *words, uint32 count, uint32 *cursor, uint32 length)
{
    for (uint32 index = 0; index < length; ++index)
    {
        uint32 value = ReadObjectParameterWord(words, count, *cursor);
        ++*cursor;
        w_u16(destination + index * 2, value);
    }
}

// Original class-specific readers skip the first payload word
static bool WriteDerivedObjectParameters(uint32 target, uint32 object, const uint16 *words, uint32 count)
{
    uint32 cursor = 1, value, index, group, length, word_index;
    if (target == 0x80044474)
    {
        WriteBufferedParameterRange(0x800FF4B4, words, count, &cursor, 4);
        for (group = 0; group < 5; ++group)
            WriteBufferedParameterRange(0x800A6B94 + group * 12, words, count, &cursor, 5);
        WriteBufferedParameterRange(0x800FF4BE, words, count, &cursor, 1);
        WriteBufferedParameterRange(0x800FF4BC, words, count, &cursor, 1);
        return true;
    }
    if (target == 0x80053F48)
    {
        WriteBufferedParameterRange(object + 468, words, count, &cursor, 2);
        return true;
    }
    if (target == 0x80072204)
    {
        WriteBufferedParameterRange(object + 496, words, count, &cursor, 7);
        WriteBufferedParameterRange(object + 510, words, count, &cursor, 4);
        WriteBufferedParameterRange(object + 518, words, count, &cursor, 1);
        WriteBufferedParameterRange(object + 520, words, count, &cursor, 13);
        return true;
    }
    if (target == 0x800474F0)
    {
        static const uint32 fields[8] = {536, 538, 512, 506, 540, 542, 544, 546};
        WriteBufferedParameterRange(object + 508, words, count, &cursor, 1);
        WriteBufferedParameterRange(object + 516, words, count, &cursor, 8);
        WriteBufferedParameterRange(object + 510, words, count, &cursor, 1);
        WriteBufferedParameterRange(object + 514, words, count, &cursor, 1);
        cursor += 16;
        WriteBufferedParameterRange(object + 532, words, count, &cursor, 2);
        for (index = 0; index < 8; ++index)
            WriteBufferedParameterRange(object + fields[index], words, count, &cursor, 1);
        WriteBufferedParameterRange(object + 548, words, count, &cursor, 4);
        WriteBufferedParameterRange(object + 556, words, count, &cursor, 4);
        WriteBufferedParameterRange(object + 564, words, count, &cursor, 4);
        return true;
    }
    if (target == 0x80049B38)
    {
        WriteBufferedParameterRange(object + 496, words, count, &cursor, 4);
        WriteBufferedParameterRange(object + 546, words, count, &cursor, 3);
        WriteBufferedParameterRange(object + 572, words, count, &cursor, 1);
        WriteBufferedParameterRange(object + 504, words, count, &cursor, 21);
        WriteBufferedParameterRange(object + 574, words, count, &cursor, 1);
        WriteBufferedParameterRange(object + 552, words, count, &cursor, 10);
        return true;
    }
    if (target == 0x8003FCEC)
    {
        WriteBufferedParameterRange(0x800FF490, words, count, &cursor, 4);
        WriteBufferedParameterRange(0x800FF498, words, count, &cursor, 4);
        WriteBufferedParameterRange(0x800FF4A0, words, count, &cursor, 1);
        cursor += 4;
        WriteBufferedParameterRange(0x800FF4A4, words, count, &cursor, 2);
        WriteBufferedParameterRange(0x800A6A60, words, count, &cursor, 8);
        word_index = 0;
        for (group = 0; group < 9; ++group)
        {
            ++cursor;
            length = ReadObjectParameterWord(words, count, cursor++);
            w_u16(0x800FFDF0 + word_index++ * 2, length);
            for (index = 0; index < length; ++index)
            {
                value = ReadObjectParameterWord(words, count, cursor++);
                w_u16(0x800FFDF0 + word_index++ * 2, value);
            }
        }
        WriteBufferedParameterRange(0x800FF4A8, words, count, &cursor, 1);
        for (group = 0; group < 24; ++group)
        {
            w_u16(0x800FFDF0 + word_index++ * 2, 1);
            value = ReadObjectParameterWord(words, count, cursor++);
            w_u16(0x800FFDF0 + word_index++ * 2, value);
        }
        WriteBufferedParameterRange(0x800FF4AA, words, count, &cursor, 2);
        return true;
    }
    return false;
}

extern "C" void apocalypse_object_parameter_write_native(uint32 target, uint32 object, uint32 command, const uint16 *words, uint32 count)
{
    uint32 value, index, byte, result, cursor;
    command &= 0xFFFF;
    if (target == 0x8003CF3C && WriteScriptedLevelObjectParameters(object, command, words, count))
        return;
    if (target == 0x8004DB90 && WriteScriptedEffectControllerParameters(object, command, words, count))
        return;
    if (command == 0x2130 && WriteDerivedObjectParameters(target, object, words, count))
        return;
    if (target != 0x8004CFDC && target != 0x8003CF3C && target != 0x8004DB90
        && ((target != 0x800474F0 && target != 0x80053F48 && target != 0x80072204 && target != 0x8003FCEC && target != 0x80049B38 && target != 0x80044474) || command == 0x2130))
    {
        GameDiagnostic("TODO Native parameter writer %08X command %04X\n", target, command);
        abort();
    }
    switch (command)
    {
        case 0x2100:
        case 0x2101:
        case 0x2114:
        case 0x2115:
        case 0x2121:
        case 0x2122:
        case 0x212C:
        case 0x212D:
        case 0x212E:
            value = ReadObjectParameterWord(words, count, 0);
            switch (command)
            {
                case 0x2100: w_u16(object + 218, value); break;
                case 0x2101: w_u16(object + 212, value); break;
                case 0x2114: w_u16(object + 472, value); break;
                case 0x2115: w_u16(object + 474, value); break;
                case 0x2121: w_u32(object + 304, (uint32)(sint32)(sint16)value); break;
                case 0x2122: w_u16(object + 456, value); break;
                case 0x212C: w_u8(object + 384, value); break;
                case 0x212D: w_u8(object + 383, value); break;
                case 0x212E: w_u16(object + 392, value); break;
            }
            return;
        case 0x2125:
            index = ReadObjectParameterWord(words, count, 0);
            value = ReadObjectParameterWord(words, count, 1);
            w_u16(object + 436 + 2 * index, value);
            return;
        case 0x2126:
            index = ReadObjectParameterWord(words, count, 0) & 255;
            byte = ReadObjectParameterWord(words, count, 1) & 255;
            value = ReadObjectParameterWord(words, count, 2);
            Enemy::Attach(object).Enemy::SetBehaviorValue(index, byte, value);
            return;
        case 0x2131:
            value = ReadObjectParameterWord(words, count, 0);
            result = r_u16(object);
            w_u16(object, value ? result | 8 : result & 0xFFF7);
            return;
        case 0x2135:
            value = ReadObjectParameterWord(words, count, 0);
            value |= ReadObjectParameterWord(words, count, 1) << 16;
            w_u32(object + 480, value);
            return;
        case 0x212F:
            byte = r_u8(object + 27);
            value = ReadObjectParameterWord(words, count, 0);
            value |= ReadObjectParameterWord(words, count, 1) << 16;
            result = FindModelResourceIndex(value, byte);
            w_u16(object + 22, result);
            return;
        case 0x2120:
            index = ReadObjectParameterWord(words, count, 0) & 255;
            value = ReadObjectParameterWord(words, count, 1);
            cursor = 2;
            break;
        case 0x2140:
        case 0x2141:
        case 0x2142:
            value = ReadObjectParameterWord(words, count, 0);
            index = 0;
            cursor = 1;
            break;
        default:
            return;
    }
    if (!(value & 0x8000) && (value & 0x2000))
        value = ReadVirtualObjectParameter(object, value, words, count, &cursor) & 0xFFFF;
    if (command == 0x2120)
        w_u16(object + 460 + 2 * index, value);
    else
        w_u32(object + 4 + 4 * (command - 0x2140), (uint32)((sint32)(value << 16) >> 4));
}
