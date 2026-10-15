#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

#include "character.h"
#include "object_lifecycle.h"
#include "object_damage.h"
#include "game_heap.h"
#include "model_resources.h"
#include "game_object_allocation.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Native lookup for the mapped single-inheritance visual hierarchy

// Native binding for the original addressable list-head word

// Native adapter for the original adjusted suspend callback

// Native input adapters for original record word and halfword loads

// Native extraction of original signed color DIV corner behavior

// Native and legacy destruction share only the original buffer field operations

#include "enemy_list.h"

#include "enemy.h"
#include "object_parameters.h"
#include "vector_arithmetic.h"
#include "angle_math.h"
#include "croc.h"
#include "bat.h"
#include "hound_rat.h"
#include "mini_tank.h"
#include "turret.h"
#include "tank.h"
#include "chopper.h"
#include "hovr_swat.h"
#include "swat.h"
#include "death.h"
#include "war.h"
#include "the_beast.h"
#include "plague.h"
#include "level_hazards.h"
#include "scripted_level_object.h"

// Native ABI lookup for the recovered single-inheritance enemy hierarchy

// Original slots 32, 40 and the default slot 56 are empty callbacks

// Legacy callers supply an already adjusted guest receiver

#include "scripted_effect_controller.h"
#include "game_random.h"
#include "record_data.h"
#include "vector_distance.h"
#include "object_collision_events.h"

// Native adapter selects the exact original command target

#include "movie_lifecycle.h"

#include "audio_session.h"
#include "particle_settings.h"
#include "fading_particle.h"
#include "fire_cycle.h"
#include "camera.h"
#include "effect_lifecycle.h"
#include <new>

// Native extractions of repeated inline command reads and adjusted getter calls

static uint32 ScriptEnemy(uint32 target, uint32 receiver, uint32 command, uint32 command_target, uint32 parameter_target, uint32 *result)
{
    if (target != command_target && target != parameter_target && target != 0x8004D2B4)
        return 0;
    if (target == command_target)
        *result = DispatchObjectScriptCommand(target, receiver, command);
    else if (target == parameter_target)
    {
        DispatchObjectParameterWriter(target, receiver, command);
        // Original parameter callback callers discard the return register
        *result = 0;
    }
    else
        *result = DispatchObjectParameterGetter(target, receiver, command);
    return 1;
}

static void WriteEnemyCommandParameter(uint32 target, uint32 buffer_owner, uint32 receiver, uint32 command, const uint16 *words, uint32 count)
{
    DispatchBufferedObjectParameterWriter(target, buffer_owner, receiver, command, words, count);
}

static uint32 ReadEnemyCommandWord(uint32 object)
{
    uint32 cursor = r_u32(object + 400);
    uint32 value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    return value;
}

static uint32 GetEnemyCommandValue(uint32 object, uint32 value)
{
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
    uint32 target = r_u32(table + 84);
    return DispatchObjectParameterGetter(target, receiver, value);
}

static uint32 ReadEnemyCommandValue(uint32 object)
{
    uint32 value = ReadEnemyCommandWord(object);
    return (value & 0x2000) ? GetEnemyCommandValue(object, value) & 0xFFFF : value;
}

static sint32 ReadSignedEnemyCommandValue(uint32 object)
{
    uint32 value = ReadEnemyCommandWord(object);
    return (value & 0x2000) ? (sint16)GetEnemyCommandValue(object, value) : (sint32)value;
}

static void PrepareEnemyDestruction(uint32 object)
{
    FUNCTION_MARKER(0x8004B868, "SLUS_003.73");
    uint32 count = r_u32(0x800FF4D8);
    w_u32(object + 68, 0x800A2B18);
    w_u32(0x800FF4D8, count - 1);
}

Enemy &Enemy::Attach(uint32 object)
{
    return *static_cast<Enemy *>(FindNativeObjectStorage(object));
}

uint32 Enemy::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    PrepareEnemyDestruction(object);
    uint32 result = Character::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

void Enemy::Callback639A4()
{
    FUNCTION_MARKER(0x800639A4, "SLUS_003.73");
}

void Enemy::Callback6399C()
{
    FUNCTION_MARKER(0x8006399C, "SLUS_003.73");
}

void Enemy::ResetWeapon()
{
}

void Enemy::SetParameters(uint32 command)
{
    ReadParameters(command);
}

uint32 apocalypse_enemy_update_cpp(uint32 target, uint32 receiver)
{
    switch (target)
    {
        case 0x80040F34: return TheBeast::Attach(receiver).TheBeast::Update();
        case 0x8003D790: return War::Attach(receiver).War::Update();
        case 0x80047DBC: return Death::Attach(receiver).Death::Update();
        case 0x800453F4: return Plague::Attach(receiver).Plague::Update();
        case 0x8003BBD8: return ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::Update();
        case 0x8005A860: return FloatingHazard::Attach(receiver).FloatingHazard::Update();
        case 0x8005AFA0: return BallisticHazard::Attach(receiver).BallisticHazard::Update();
        case 0x8004E71C: return Swat::Attach(receiver).Swat::Update();
        case 0x80071EF0: return Croc::Attach(receiver).Croc::Update();
        case 0x8004A37C: return Bat::Attach(receiver).Bat::Update();
        case 0x80073A3C: return HoundRat::Attach(receiver).HoundRat::Update();
        case 0x80074BA8: return MiniTank::Attach(receiver).MiniTank::Update();
        case 0x8005848C: return Turret::Attach(receiver).Turret::Update();
        case 0x800563B4: return Tank::Attach(receiver).Tank::Update();
        case 0x80053FA8: return Chopper::Attach(receiver).Chopper::Update();
        case 0x800598B8: return HovrSwat::Attach(receiver).HovrSwat::Update();
        default:
            GameDiagnostic("TODO C++ enemy update target %08X\n", target);
            abort();
    }
}

uint32 apocalypse_enemy_damage_cpp(uint32 target, uint32 receiver, uint32 damage, uint32 guest_vector, const sint32 *native_vector, uint32 kind)
{
    switch (target)
    {
        case 0x800422C0: return TheBeast::Attach(receiver).TheBeast::Damage(damage, 0, kind);
        case 0x8003E354: return War::Attach(receiver).War::Damage(damage, 0, kind);
        case 0x80048428: return Death::Attach(receiver).Death::Damage(damage, 0, kind);
        case 0x80045B14: return Plague::Attach(receiver).Plague::Damage(damage, 0, kind);
        case 0x8003C490: return ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::Damage(damage, 0, kind);
        case 0x8005A7B0: return FloatingHazard::Attach(receiver).FloatingHazard::Damage(damage, 0, kind);
        case 0x8005AF98: return BallisticHazard::Attach(receiver).BallisticHazard::Damage(damage, 0, kind);
        case 0x800736AC: return Croc::Attach(receiver).Croc::Damage(damage, 0, kind);
        case 0x8004A25C: return Bat::Attach(receiver).Bat::Damage(damage, native_vector ? native_vector : (guest_vector ? (const sint32 *)psx_addr(guest_vector, 12) : 0), kind);
        case 0x800747A8: return HoundRat::Attach(receiver).HoundRat::Damage(damage, 0, kind);
        case 0x80075930: return MiniTank::Attach(receiver).MiniTank::Damage(damage, native_vector ? native_vector : (guest_vector ? (const sint32 *)psx_addr(guest_vector, 12) : 0), kind);
        case 0x80058A14: return Turret::Attach(receiver).Turret::Damage(damage, 0, kind);
        case 0x80056C50: return Tank::Attach(receiver).Tank::Damage(damage, 0, kind);
        case 0x80054714: return Chopper::Attach(receiver).Chopper::Damage(damage, 0, kind);
        case 0x800593CC: return HovrSwat::Attach(receiver).HovrSwat::Damage(damage, native_vector ? native_vector : (guest_vector ? (const sint32 *)psx_addr(guest_vector, 12) : 0), kind);
        case 0x8004F5C8: return Swat::Attach(receiver).Swat::Damage(damage, native_vector ? native_vector : (guest_vector ? (const sint32 *)psx_addr(guest_vector, 12) : 0), kind);
        default:
            GameDiagnostic("TODO C++ enemy damage target %08X\n", target);
            abort();
    }
}

uint32 apocalypse_enemy_script_cpp(uint32 target, uint32 receiver, uint32 command, uint32 *result)
{
    if (r_u32(receiver + 68) == 0x800A1DF0)
    {
        ScriptedLevelObject &owner = ScriptedLevelObject::Attach(receiver);
        if (target == 0x8003C7B4)
        {
            *result = owner.ScriptedLevelObject::ExecuteCommand(command);
            return 1;
        }
        if (target == 0x8003CF3C)
        {
            *result = owner.ScriptedLevelObject::SetScriptParameters(command);
            return 1;
        }
        if (target == 0x8003D0F0)
        {
            *result = owner.ScriptedLevelObject::ResolveParameter(command);
            return 1;
        }
    }

    if (r_u32(receiver + 68) == 0x800A20C0)
    {
        if (target == 0x80044608)
        {
            *result = TheBeast::Attach(receiver).TheBeast::ExecuteCommand(command);
            return 1;
        }
        if (target == 0x80044474)
        {
            *result = TheBeast::Attach(receiver).TheBeast::SetScriptParameters(command);
            return 1;
        }
    }

    if (r_u32(receiver + 68) == 0x800A252C)
    {
        if (target == 0x800476D0)
        {
            *result = Plague::Attach(receiver).Plague::ExecuteCommand(command);
            return 1;
        }
        if (target == 0x800474F0)
        {
            *result = Plague::Attach(receiver).Plague::ReadParameters(command);
            return 1;
        }
        if (target == 0x8004D2B4)
        {
            *result = Enemy::Attach(receiver).Enemy::ResolveParameter(command);
            return 1;
        }
    }

    switch (target)
    {
        case 0x8005AB58:
        case 0x800529AC:
        case 0x80072320:
        case 0x8004A690:
        case 0x80073C78:
        case 0x8004BF3C:
        case 0x80058DB8:
        case 0x80057840:
        case 0x8005568C:
        case 0x80059D1C:
        case 0x80072204:
        case 0x80053F48:
        case 0x8004CFDC:
        case 0x8004D2B4:
            break;
        default:
            return 0;
    }
    // Table identity binds the receiver to its recovered original owner
    switch (r_u32(receiver + 68))
    {
        case 0x800A2908:
            return ScriptEnemy(target, receiver, command, 0x8005AB58, 0x8004CFDC, result);
        case 0x800A28B0:
            return ScriptEnemy(target, receiver, command, 0x8004BF3C, 0x8004CFDC, result);
        case 0x800A2A68:
            return ScriptEnemy(target, receiver, command, 0x800529AC, 0x8004CFDC, result);
        case 0x800A3B64:
            return ScriptEnemy(target, receiver, command, 0x80072320, 0x80072204, result);
        case 0x800A26F0:
            return ScriptEnemy(target, receiver, command, 0x8004A690, 0x8004CFDC, result);
        case 0x800A3C30:
            return ScriptEnemy(target, receiver, command, 0x80073C78, 0x8004CFDC, result);
        case 0x800A3C98:
            return ScriptEnemy(target, receiver, command, 0x8004BF3C, 0x8004CFDC, result);
        case 0x800A2960:
            return ScriptEnemy(target, receiver, command, 0x80058DB8, 0x8004CFDC, result);
        case 0x800A29B8:
            return ScriptEnemy(target, receiver, command, 0x80057840, 0x8004CFDC, result);
        case 0x800A2A10:
            return ScriptEnemy(target, receiver, command, 0x8005568C, 0x80053F48, result);
        case 0x800A2818:
            return ScriptEnemy(target, receiver, command, 0x80059D1C, 0x8004CFDC, result);
        default:
            return 0;
    }
}

uint32 DestroyEnemyBase(uint32 object, uint32 reason)
{
    PrepareEnemyDestruction(object);
    uint32 result = DestroyCharacterBase(object, 0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

uint32 DispatchObjectScriptCommand(uint32 target, uint32 receiver, uint32 command)
{
    switch (target)
    {
        case 0x8004DC40:
            return ScriptedEffectController::Attach(receiver).ScriptedEffectController::ExecuteCommand(command);
        case 0x8004BF3C:
            return Enemy::Attach(receiver).Enemy::ExecuteCommand(command);
        case 0x800476D0:
            return Plague::Attach(receiver).Plague::ExecuteCommand(command);
        case 0x80049C48:
            return Death::Attach(receiver).Death::ExecuteCommand(command);
        case 0x8003FF2C:
            return War::Attach(receiver).War::ExecuteCommand(command);
        case 0x80044608:
            return TheBeast::Attach(receiver).TheBeast::ExecuteCommand(command);
        case 0x80072320:
            return Croc::Attach(receiver).Croc::ExecuteCommand(command);
        case 0x80073C78:
            return HoundRat::Attach(receiver).HoundRat::ExecuteCommand(command);
        case 0x8004A690:
            return Bat::Attach(receiver).Bat::ExecuteCommand(command);
        case 0x80058DB8:
            return Turret::Attach(receiver).Turret::ExecuteCommand(command);
        case 0x80057840:
            return Tank::Attach(receiver).Tank::ExecuteCommand(command);
        case 0x80059D1C:
            return HovrSwat::Attach(receiver).HovrSwat::ExecuteCommand(command);
        case 0x8005568C:
            return Chopper::Attach(receiver).Chopper::ExecuteCommand(command);
        case 0x800529AC:
            return Swat::Attach(receiver).Swat::ExecuteCommand(command);
        case 0x8003C7B4:
            return ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::ExecuteCommand(command);
        case 0x8005AB58:
            return FloatingHazard::Attach(receiver).FloatingHazard::ExecuteCommand(command);
        default:
            // TODO Classify targets outside the reviewed Enemy table inventory
            return xport_draft_guest_call2(target, receiver, command);
    }
}

uint32 InitializeEnemyBase(uint32 object)
{
    uint32 count;
    FUNCTION_MARKER(0x8004B800, "SLUS_003.73");
    sub_80062F64(object);
    count = r_u32(0x800FF4D8);
    w_u32(object + 68, 0x800A2B18);
    w_u16(object + 214, 65535);
    w_u16(object + 212, 128);
    w_u16(object + 392, 32);
    w_u16(object + 476, 0);
    w_u32(object + 208, 64);
    w_u32(object + 448, 0);
    w_u32(0x800FF4D8, count + 1);
    return object;
}

Enemy::Enemy() : Character()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count;
    FUNCTION_MARKER(0x8004B800, "SLUS_003.73");
    count = r_u32(0x800FF4D8);
    w_u32(object + 68, 0x800A2B18);
    w_u16(object + 214, 65535);
    w_u16(object + 212, 128);
    w_u16(object + 392, 32);
    w_u16(object + 476, 0);
    w_u32(object + 208, 64);
    w_u32(object + 448, 0);
    w_u32(0x800FF4D8, count + 1);
}

uint32 Enemy::Finish()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, state;
    FUNCTION_MARKER(0x8004B8C8, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        ActivateLinkedRecords();
        flags = r_u16(object + 78);
        state = r_u16(object);
        flags |= 64;
        state |= 1;
        w_u16(object + 78, flags);
        w_u16(object, state);
        result = flags;
    }
    return result;
}

uint32 ScaleDamageForDifficulty(uint32 damage)
{
    FUNCTION_MARKER(0x8004B914, "SLUS_003.73");
    uint32 difficulty = r_u32(0x800FF384);
    if (difficulty == 1)
        return damage;
    if (difficulty)
        return damage << 1;
    return damage - (uint32)((sint32)damage >> 2);
}

uint32 Enemy::ActivateLinkedRecords()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index, table, record;
    FUNCTION_MARKER(0x8004B948, "SLUS_003.73");
    if (r_u8(object + 385))
        return 65535;
    index = r_u16(object + 214);
    if (index == 65535)
        return 1;
    w_u8(object + 385, 1);
    table = r_u32(0x800FF624);
    record = r_u32(table + index * 4);
    return SpawnEnemyIndices((const uint16 *)psx_addr(record + 6, 2));
}

uint32 Enemy::IntegrateMotion()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004B9A4, "SLUS_003.73");
    uint32 vector = object + 104;
    uint32 packed = r_u32(0x800FF5E4);
    uint32 second = r_u32(0x800FF5E4);
    uint32 z = (uint32)(sint32)(sint16)r_u16(0x800FF5E8);
    packed = second;
    // Preserve both stores of the aligned MIPS USW pair
    w_u32(object + 164, packed);
    w_u32(object + 164, packed);
    w_u16(object + 168, z);
    w_u16(object + 216, 0);
    AddVectors((uint32 *)psx_addr(vector, 12), (const uint32 *)psx_addr(object + 116, 12));
    DampVector((uint32 *)psx_addr(vector, 12), (const uint8 *)psx_addr(object + 129, 3));
    SnapSmallVector((uint32 *)psx_addr(vector, 12));
    AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(vector, 12));
    AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 132, 6));
    NormalizeAngles((uint16 *)psx_addr(object + 16, 6));
    AddAngleVector((uint16 *)psx_addr(object + 132, 6), (const uint16 *)psx_addr(object + 138, 6));
    DampAngles((uint16 *)psx_addr(object + 132, 6), (const uint8 *)psx_addr(object + 144, 3));
    return SnapSmallAngles((uint16 *)psx_addr(object + 132, 6));
}

uint32 Enemy::AllocateBehaviorTable(uint32 rows, uint32 columns)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004BD04, "SLUS_003.73");
    uint32 old_table = r_u32(object + 448);
    if (old_table)
        ReleaseHeapBlock(old_table);
    uint32 table = AllocateHeapBlock(2 * (rows & 255) * (columns & 255), 0, 1);
    w_u32(object + 448, table);
    w_u8(object + 452, rows);
    w_u8(object + 453, columns);
    return table;
}

uint32 Enemy::FreeBehaviorTable()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004BD94, "SLUS_003.73");
    uint32 result = ReleaseHeapBlock(r_u32(object + 448));
    w_u32(object + 448, 0);
    w_u8(object + 452, 0);
    w_u8(object + 453, 0);
    return result;
}

uint32 Enemy::SetBehaviorValue(uint32 row, uint32 column, uint32 value)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004BDCC, "SLUS_003.73");
    uint32 stride = r_u8(object + 453);
    uint32 table = r_u32(object + 448);
    uint32 base = table + 2 * (row & 255) * stride;
    w_u16(base + 2 * (column & 255), value);
    return base;
}

uint32 Enemy::GetBehaviorValue(uint32 row, uint32 column)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004BDFC, "SLUS_003.73");
    uint32 stride = r_u8(object + 453);
    uint32 table = r_u32(object + 448);
    return r_u16(table + 2 * (row & 255) * stride + 2 * (column & 255));
}

uint32 Enemy::SkipConditionalBlock()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 depth = 0, cursor, command, result;
    FUNCTION_MARKER(0x8004BE30, "SLUS_003.73");
    for (;;)
    {
        cursor = r_u32(object + 400);
        command = r_u16(cursor);
        w_u32(object + 400, cursor + 2);
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

uint32 Enemy::ExecuteScript(const uint16 *script)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor = GuestMemoryAddress(script);
    if (!cursor)
    {
        // TODO Bind native script data to the original stream layout
        GameDiagnostic("TODO Enemy native script data\n");
        abort();
    }
    FUNCTION_MARKER(0x8004BE90, "SLUS_003.73");
    w_u32(object + 400, cursor);
    for (;;)
    {
        cursor = r_u32(object + 400);
        uint32 command = r_u16(cursor);
        uint32 result = cursor + 2;
        if (command == 0x4100)
        {
            w_u8(object + 380, 0);
            return result;
        }
        w_u32(object + 400, result);
        if (command & 0x4000)
        {
            uint32 table = r_u32(object + 68);
            sint32 adjustment = (sint16)r_u16(table + 64);
            uint32 target = r_u32(table + 68);
            result = DispatchObjectScriptCommand(target, object + (uint32)adjustment, command);
            if (!result)
                return result;
        }
        else if (command & 0x2000)
        {
            uint32 table = r_u32(object + 68);
            sint32 adjustment = (sint16)r_u16(table + 72);
            uint32 target = r_u32(table + 76);
            DispatchObjectParameterWriter(target, object + (uint32)adjustment, command);
        }
    }
}

uint32 Enemy::ExecuteCommand(uint32 opcode)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004BF3C, "SLUS_003.73");
    uint32 value, second, third, cursor, saved, table, created, count, i, receiver, target;
    uint32 position[3];
    uint16 *words;
    sint16 angles[3];
    sint32 left, right;
    opcode &= 0xFFFF;
    switch (opcode)
    {
        case 0x4101:
        case 0x4102:
            value = ReadEnemyCommandWord(object);
            w_u32(object + 400, r_u32(object + value * 4 + 404));
            return opcode == 0x4101;
        case 0x4104:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            cursor += 2;
            w_u32(object + 400, cursor);
            w_u32(object + value * 4 + 404, cursor);
            return 1;
        case 0x4105:
            saved = r_u32(object + 400);
            for (;;)
            {
                cursor = r_u32(object + 400);
                value = r_u16(cursor);
                if (value == 0x4100)
                    break;
                w_u32(object + 400, cursor + 2);
                if (value == 0x4104)
                {
                    value = r_u16(cursor + 2);
                    w_u32(object + 400, cursor + 4);
                    w_u32(object + value * 4 + 404, cursor + 4);
                }
            }
            w_u32(object + 400, saved);
            return 1;
        case 0x4106:
            value = ReadEnemyCommandValue(object);
            cursor = ReadRecordPosition(position, value) + 6;
            w_u32(object + 400, cursor);
            return 1;
        case 0x4107:
            return 0;
        case 0x4110:
        case 0x4111:
            cursor = r_u32(object + 400);
            table = r_u32(object + 68);
            value = r_u16(cursor);
            cursor += 2;
            w_u32(object + 400, cursor);
            receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
            target = r_u32(table + 84);
            left = (sint16)DispatchObjectParameterGetter(target, receiver, value);
            second = r_u32(object + 400) - cursor;
            if ((second & 1) || second > PSX_DRAM_SIZE - 2)
                abort();
            words = (uint16 *)malloc(second + 2);
            if (!words)
                abort();
            count = 0;
            while (cursor != r_u32(object + 400))
            {
                words[count] = r_u16(cursor);
                ++count;
                cursor += 2;
            }
            right = ReadSignedEnemyCommandValue(object);
            words[count] = (uint16)(opcode == 0x4110 ? left + right : left - right);
            saved = r_u32(object + 400);
            table = r_u32(object + 68);
            receiver = object + (uint32)(sint32)(sint16)r_u16(table + 72);
            target = r_u32(table + 76);
            WriteEnemyCommandParameter(target, object, receiver, value, &words[0], count + 1);
            free(words);
            w_u32(object + 400, saved);
            return 1;
        case 0x4112:
        case 0x4113:
        case 0x4114:
            left = ReadSignedEnemyCommandValue(object);
            right = ReadSignedEnemyCommandValue(object);
            if ((opcode == 0x4112 && !(right < left)) || (opcode == 0x4113 && !(left < right)) || (opcode == 0x4114 && left != right))
                SkipConditionalBlock();
            return 1;
        case 0x4115:
        case 0x4116:
            value = ReadEnemyCommandWord(object);
            second = r_u32(object + 396) & value;
            if ((opcode == 0x4115 && second != value) || (opcode == 0x4116 && second != 0))
                SkipConditionalBlock();
            return 1;
        case 0x4200:
            VisualObject::SetModelBank((const char *)psx_addr(r_u32(object + 400), 1));
            goto skip_string;
        case 0x4201:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            cursor += 2;
            w_u32(object + 400, cursor);
            second = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            StartAnimation(value, (uint32)(sint32)(sint8)second);
            return 1;
        case 0x4202:
            value = ReadEnemyCommandWord(object);
            SetAnimationRange(value, 0, 0xFFFFFFFF);
            return 1;
        case 0x4203:
            w_u16(object, r_u16(object) & 0xFFFE);
            return 1;
        case 0x4204:
            w_u16(object, r_u16(object) | 1);
            return 1;
        case 0x4205:
            table = r_u32(object + 68);
            receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
            target = r_u32(table + 20);
            apocalypse_object_virtual20(target, receiver);
            return 1;
        case 0x4226:
            w_u32(object + 112, 0);
            w_u32(object + 108, 0);
            w_u32(object + 104, 0);
            return 1;
        case 0x4227:
            value = ReadEnemyCommandWord(object);
            if (value)
            {
                second = r_u32(object + 396);
                w_u8(object + 386, value);
                w_u8(object + 387, 0);
                w_u32(object + 396, second | 2);
            }
            else
                w_u32(object + 396, r_u32(object + 396) & ~2);
            return 1;
        case 0x4240:
            w_u8(object + 380, 0);
            return 0;
        case 0x4260:
        case 0x4261:
            value = ReadEnemyCommandWord(object);
            SetBehaviorValue(value & 0xFF, 0, opcode == 0x4260);
            return 1;
        case 0x4280:
            value = ReadEnemyCommandWord(object);
            w_u16(object + 476, value);
            if (value & 0x2000)
            {
                table = r_u32(object + 68);
                value = r_u16(object + 476);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
                target = r_u32(table + 84);
                w_u16(object + 476, DispatchObjectParameterGetter(target, receiver, value));
            }
            return 0;
        case 0x4281:
            if (r_u16(object + 388) & 1)
                return 1;
            w_u32(object + 400, r_u32(object + 400) - 2);
            return 0;
        case 0x4290:
            value = ReadEnemyCommandWord(object);
            PlaySoundEffect((sint16)value, 8192, 0);
            return 1;
        case 0x4291:
            value = ReadEnemyCommandWord(object);
            PlayPositionedSound((sint16)value, (const uint32 *)psx_addr(object + 4, 12), 0);
            return 1;
        case 0x4292:
            cursor = r_u32(object + 400);
            second = r_u32(0x800FF3A8);
            count = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            if ((sint32)second >= 200)
                return 1;
            SetParticleDirection((const sint16 *)psx_addr(object + 16, 6));
            angles[0] = 512;
            angles[1] = 4096;
            angles[2] = 0;
            SetParticleSpread(angles);
            SetParticleSize(1);
            SetParticleColor(128, 128, 128);
            SetParticleColorStep(4, 4, 4);
            w_u32(0x800FF3AC, 0);
            for (i = 0; i < count; ++i)
            {
                created = AllocateEffectStorage(88);
                if (created)
                {
                    void *dram_addr = AllocateNativeObjectStorage(created, sizeof(FadingParticle));
                    new (dram_addr) FadingParticle((const uint32 *)psx_addr(object + 4, 12), 32, 8192, 32);
                }
            }
            w_u32(0x800FF3AC, 1);
            return 1;
        case 0x4293:
            cursor = (r_u32(object + 400) + 3) & ~3;
            sub_8002E600(r_u32(cursor));
            w_u32(object + 400, cursor + 4);
            return 1;
        case 0x4294:
            sub_8002E6D8(object + 4);
            table = GetRecordPayload(r_u16(object + 214));
            cursor = table;
            for (i = 0; i < r_u16(table); ++i)
            {
                value = r_u16(cursor + 2);
                cursor += 2;
                ReadRecordPosition(position, value);
                sub_8002E6D8(object + 4);
            }
            sub_8002EF68();
            return 1;
        case 0x4295:
            ResetMoviePlayback();
            return 1;
        case 0x4296:
            value = ReadEnemyCommandWord(object);
            sub_8006A428();
            return 1;
        case 0x4297:
            value = ReadEnemyCommandWord(object);
            sub_8006A3C4();
            return 1;
        case 0x4298:
            value = ReadEnemyCommandValue(object);
            created = r_u32(0x800FF904);
            if (created)
                Camera::Attach(created).ApplyExplosionShake(value == 0 ? 2 : value == 1 ? 1 : 0);
            return 1;
        case 0x4299:
            value = ReadEnemyCommandWord(object);
            sub_8002EE7C(value & 0xFF);
            return 1;
        case 0x429A:
        case 0x42A6:
            if (opcode == 0x429A)
            {
                value = ReadEnemyCommandValue(object);
                ReadRecordPosition(position, value);
            }
            else
            {
                position[0] = r_u32(object + 4);
                position[1] = r_u32(object + 8);
                position[2] = r_u32(object + 12);
            }
            value = ReadEnemyCommandWord(object);
            if (value < 2)
                sub_8001E1B8(object + 4, value);
            else if (value == 2)
            {
                second = ReadScriptValue();
                cursor = r_u32(object + 400) + 2;
                w_u32(object + 400, cursor);
                third = r_u16(cursor);
                w_u32(object + 400, cursor + 2);
                created = AllocateEffectStorage(156);
                if (created)
                {
                    void *dram_addr = AllocateNativeObjectStorage(created, sizeof(FireCycle));
                    new (dram_addr) FireCycle(position, (uint16)second, third != 0, (uint16)third);
                }
            }
            return 1;
        case 0x429C:
            cursor = r_u32(object + 400);
            value = r_u8(cursor);
            second = r_u8(cursor + 2);
            third = r_u8(cursor + 4);
            count = r_u16(cursor + 6);
            cursor = r_u32(object + 400);
            created = r_u16(cursor + 8);
            sub_8001BE78(value, second, third, count, 0, created);
            w_u32(object + 400, r_u32(object + 400) + 10);
            return 1;
        case 0x429D:
            sub_8002E6D8(object + 4);
            sub_8002EF58();
            return 1;
        case 0x429E:
            w_u32(0x800FF37C, r_u32(object + 8));
            return 1;
        case 0x42A0:
            value = ReadEnemyCommandValue(object);
            cursor = r_u32(object + 400);
            second = r_u16(cursor);
            cursor += 2;
            w_u32(object + 400, cursor);
            third = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            count = r_u16(cursor + 2);
            w_u32(object + 400, cursor + 4);
            created = sub_80020EF8(value, object + 4, second, third, 0x800A71CC);
            if (created)
                sub_8002005C(created, count << 12, 5);
            return 1;
        case 0x42A2:
            value = ReadEnemyCommandWord(object);
            w_u32(0x800FF380, value);
            return 1;
        case 0x42A3:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            cursor += 2;
            w_u32(object + 400, cursor);
            second = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            sub_8002FD2C(value, second);
            return 1;
        case 0x42A4:
            cursor = (r_u32(object + 400) + 3) & ~3;
            w_u32(object + 400, cursor + 4);
            value = r_u16(cursor + 4);
            w_u32(object + 400, cursor + 6);
            second = r_u16(cursor + 6);
            w_u32(object + 400, cursor + 8);
            third = r_u16(cursor + 8);
            w_u32(object + 400, cursor + 10);
            sub_8002E148(r_u32(cursor), value, second, third & 0xFF);
            return 1;
        case 0x42A5:
            ReleaseMovieFrameBuffers();
            return 1;
        case 0x42B0:
        skip_string:
            cursor = r_u32(object + 400);
            while (r_u8(cursor))
                ++cursor;
            w_u32(object + 400, cursor + ((cursor & 1) ? 1 : 2));
            return 1;
        case 0x42B1:
        case 0x42B2:
            value = ReadEnemyCommandValue(object);
            cursor = GetRecordPayload(value);
            if (opcode == 0x42B1)
                ActivateEnemyIndices((const uint16 *)psx_addr(cursor, 2));
            else
                SpawnEnemyIndices((const uint16 *)psx_addr(cursor, 2));
            return 1;
        case 0x450A:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            cursor += 2;
            w_u32(object + 400, cursor);
            second = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            sub_8002FC64(value, second, object + 4, 0);
            return 1;
        default:
            return 1;
    }
}

uint32 Enemy::ReadScriptValue()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004CF78, "SLUS_003.73");
    uint32 stream = r_u32(object + 400);
    uint32 command = r_u16(stream);
    w_u32(object + 400, stream + 2);
    if ((command & 0x8000) == 0 && (command & 0x2000) != 0)
    {
        uint32 table = r_u32(object + 68);
        uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
        command = DispatchObjectParameterGetter(r_u32(table + 84), receiver, command);
    }
    return command & 0xFFFF;
}

uint32 Enemy::ReadParameters(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004CFDC, "SLUS_003.73");
    uint32 cursor, value, result, index, bank;
    command &= 0xFFFF;
    if (command - 8448 >= 67)
        return 0;
    switch (command)
    {
        case 8501:
            cursor = (r_u32(object + 400) + 3) & 0xFFFFFFFC;
            value = r_u32(cursor);
            w_u32(object + 400, cursor + 4);
            w_u32(object + 480, value);
            return cursor + 4;
        case 8495:
            cursor = (r_u32(object + 400) + 3) & 0xFFFFFFFC;
            bank = r_u8(object + 27);
            value = r_u32(cursor);
            result = FindModelResourceIndex(value, bank);
            w_u16(object + 22, result);
            w_u32(object + 400, cursor + 4);
            return result;
        case 8512:
        case 8513:
        case 8514:
            result = (uint32)((sint32)(ReadScriptValue() << 16) >> 4);
            w_u32(object + 4 + (command - 8512) * 4, result);
            return result;
        case 8497:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            result = r_u16(object);
            result = value ? result | 8 : result & 0xFFF7;
            w_u16(object, result);
            return result;
        case 8480:
            cursor = r_u32(object + 400);
            index = r_u16(cursor) & 0xFF;
            w_u32(object + 400, cursor + 2);
            result = ReadScriptValue();
            w_u16(object + 460 + index * 2, result);
            return result;
        case 8485:
            cursor = r_u32(object + 400);
            index = r_u16(cursor);
            cursor += 2;
            w_u32(object + 400, cursor);
            value = r_u16(cursor);
            w_u16(object + 436 + index * 2, value);
            w_u32(object + 400, cursor + 2);
            return cursor + 2;
        case 8486:
            cursor = r_u32(object + 400);
            index = r_u16(cursor) & 0xFF;
            cursor += 2;
            w_u32(object + 400, cursor);
            value = r_u16(cursor) & 0xFF;
            w_u32(object + 400, cursor + 2);
            result = r_u16(cursor + 2);
            w_u32(object + 400, cursor + 4);
            return SetBehaviorValue(index, value, result);
        case 8448:
        case 8449:
        case 8468:
        case 8469:
        case 8481:
        case 8482:
        case 8492:
        case 8493:
        case 8494:
            cursor = r_u32(object + 400);
            value = (command == 8492 || command == 8493) ? r_u8(cursor) : r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            switch (command)
            {
                case 8448:
                    w_u16(object + 218, value);
                    break;
                case 8449:
                    w_u16(object + 212, value);
                    break;
                case 8468:
                    w_u16(object + 472, value);
                    break;
                case 8469:
                    w_u16(object + 474, value);
                    break;
                case 8481:
                    w_u32(object + 304, (uint32)(sint32)(sint16)value);
                    break;
                case 8482:
                    w_u16(object + 456, value);
                    break;
                case 8492:
                    w_u8(object + 384, value);
                    break;
                case 8493:
                    w_u8(object + 383, value);
                    break;
                case 8494:
                    w_u16(object + 392, value);
                    break;
            }
            return cursor + 2;
        default:
            return 0x8004D2A0;
    }
}

uint32 Enemy::ResolveParameter(uint32 opcode)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004D2B4, "SLUS_003.73");
    uint32 cursor, value, player, result;
    opcode &= 65535;
    switch (opcode)
    {
        case 8448:
            return (uint32)(sint32)(sint16)r_u16(object + 218);
        case 8480:
            cursor = r_u32(object + 400);
            value = r_u8(cursor);
            w_u32(object + 400, cursor + 2);
            return (uint32)(sint32)(sint16)r_u16(object + 460 + value * 2);
        case 8489:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            return RandomRange(value);
        case 8490:
            return r_u16(object + 214);
        case 8491:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            if (value & 0x2000)
            {
                uint32 table = r_u32(object + 68);
                uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
                uint32 target = r_u32(table + 84);
                value = DispatchObjectParameterGetter(target, receiver, value);
            }
            cursor = GetRecordPayload(value & 65535);
            return r_u16(cursor) ? r_u16(cursor + 2) : 0;
        case 8492:
            return r_u8(object + 384);
        case 8493:
            return r_u8(object + 383);
        case 8494:
            return r_u16(object + 392);
        case 8498:
            player = r_u32(0x800FF5A0);
            if (!player)
                return 8191;
            result = MeasureHorizontalDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(player + 4, 12));
            return result < 8192 ? result : 8191;
        case 8499:
            player = r_u32(0x800FF5A0);
            if (!player)
                return 0;
            {
                uint32 position[3];
                position[0] = r_u32(object + 4);
                position[1] = r_u32(object + 8);
                position[2] = r_u32(object + 12);
                position[1] -= (uint32)(sint32)(sint16)r_u16(object + 458) << 12;
                return IsSegmentVisible(position, (const uint32 *)psx_addr(player + 4, 12));
            }
        case 8502:
            return r_u32(0x800FF274);
        case 8512:
        case 8513:
        case 8514:
            return (uint32)((sint32)r_u32(object + 4 + (opcode - 8512) * 4) >> 12);
        case 8528:
        case 8529:
        case 8530:
            player = r_u32(0x800FF5A0);
            return player ? (uint32)((sint32)r_u32(player + 4 + (opcode - 8528) * 4) >> 12) : 0;
        default:
            return 0;
    }
}

extern "C" uint32 sub_8004B800(uint32 object)
{
    FUNCTION_MARKER(0x8004B800, "SLUS_003.73");
    return InitializeEnemyBase(object);
}

extern "C" uint32 sub_8004B9A4(uint32 object)
{
    FUNCTION_MARKER(0x8004B9A4, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::IntegrateMotion();
}

extern "C" uint32 sub_8004BD04(uint32 object, uint32 rows, uint32 columns)
{
    FUNCTION_MARKER(0x8004BD04, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::AllocateBehaviorTable(rows, columns);
}

extern "C" uint32 sub_8004BD94(uint32 object, uint32)
{
    FUNCTION_MARKER(0x8004BD94, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::FreeBehaviorTable();
}

extern "C" uint32 sub_8004BDCC(uint32 object, uint32 row, uint32 column, uint32 value)
{
    FUNCTION_MARKER(0x8004BDCC, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::SetBehaviorValue(row, column, value);
}

extern "C" uint32 sub_8004BDFC(uint32 object, uint32 row, uint32 column)
{
    FUNCTION_MARKER(0x8004BDFC, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::GetBehaviorValue(row, column);
}

extern "C" uint32 sub_8004B948(uint32 object)
{
    FUNCTION_MARKER(0x8004B948, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::ActivateLinkedRecords();
}

extern "C" uint32 sub_8004B8C8(uint32 object)
{
    FUNCTION_MARKER(0x8004B8C8, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::Finish();
}

extern "C" uint32 sub_8004B868(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8004B868, "SLUS_003.73");
    return DestroyEnemyBase(object, reason);
}

extern "C" uint32 sub_8004B914(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x8004B914, "SLUS_003.73");
    return ScaleDamageForDifficulty(damage);
}

extern "C" uint32 sub_8004CF78(uint32 object)
{
    FUNCTION_MARKER(0x8004CF78, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::ReadScriptValue();
}

extern "C" uint32 sub_8004CFDC(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8004CFDC, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::ReadParameters(command);
}

extern "C" uint32 sub_8004BE30(uint32 object)
{
    FUNCTION_MARKER(0x8004BE30, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::SkipConditionalBlock();
}

extern "C" uint32 sub_8004D2B4(uint32 object, uint32 opcode)
{
    FUNCTION_MARKER(0x8004D2B4, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::ResolveParameter(opcode);
}

extern "C" uint32 sub_8004BE90(uint32 object, uint32 cursor)
{
    FUNCTION_MARKER(0x8004BE90, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::ExecuteScript((const uint16 *)psx_addr(cursor, 2));
}

extern "C" uint32 sub_8004BF3C(uint32 object, uint32 opcode)
{
    FUNCTION_MARKER(0x8004BF3C, "SLUS_003.73");
    return Enemy::Attach(object).Enemy::ExecuteCommand(opcode);
}
