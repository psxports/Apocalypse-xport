#include "visual_object.h"
#include "angle_math.h"
#include "game_sdk_bindings.h"
#include "fire_cycle.h"
#include "audio_session.h"
#include "vector_distance.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "effect_splash.h"
#include "model_resources.h"
#include "object_collision_events.h"
#include "object_damage.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "particle_debris.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "level_hazards.h"
#include "object_parameters.h"
#include "record_data.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Native input bridge for the original position-word loads
static uint32 ReadMovingRecordPosition(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

FloatingHazard::FloatingHazard(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    if (!data)
    {
        // TODO Bind native record data to the existing parser
        GameDiagnostic("TODO Record409 native record data\n");
        abort();
    }
    uint32 object = NativeObjectGuestAddress(this);
    uint32 bank, animation, flags, cursor;
    FUNCTION_MARKER(0x8005A630, "SLUS_003.73");
    bank = r_u32(0x800FF62C);
    w_u32(object + 68, 0x800A2908);
    sub_800626C8(object, bank);
    animation = ::FindModelResourceIndex(0xB9E8D489, r_u8(0x800FF644));
    flags = this->model.flags;
    this->model.model_idx = animation;
    this->model.flags = (flags & 0xFFFD) | 8;
    AddToList(visual_lists.actors);
    w_u16(object + 212, 64);
    w_u16(object + 218, 20);
    this->kind = 9;
    const uint8 *angles = ReadPosition(record_data);
    const uint8 *script = ReadAngles(angles);
    w_u32(object + 400, GuestMemoryAddress(script));
    this->record_idx = index;
    motion.drag[0] = 2;
    motion.drag[1] = 2;
    motion.drag[2] = 2;
    motion.angular_drag[0] = 16;
    motion.angular_drag[1] = 16;
    motion.angular_drag[2] = 16;
    cursor = r_u32(object + 400);
    w_u16(object + 436, 12);
    w_u16(object + 438, 256);
    w_u16(object + 440, 20);
    w_u8(object + 380, 1);
    sub_8004BE90(object, cursor);
}

uint32 FloatingHazard::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8005A74C, "SLUS_003.73");
    w_u32(object + 68, 0x800A2908);
    RemoveFromList(visual_lists.actors);
    result = Enemy::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

void FloatingHazard::FinishFromTable()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    apocalypse_object_virtual20(target, receiver);
}

void FloatingHazard::Explode()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 variant;
    SpawnExplosionRing((const uint32 *)psx_addr(object + 4, 12), 96, 96, 128, 0, 4, 1, 100);
    variant = RandomRange(2);
    PlayPositionedSound((sint32)(variant + 1), (const uint32 *)psx_addr(object + 4, 12), 0);
}

uint32 FloatingHazard::Damage(uint32 damage, const sint32 *, uint32)
{
    uint32 object = NativeObjectGuestAddress(this);
    sint32 health;
    uint32 value;
    FUNCTION_MARKER(0x8005A7B0, "SLUS_003.73");
    health = (sint16)r_u16(object + 218);
    value = r_u16(object + 218) - damage;
    if (health > 0)
    {
        w_u16(object + 218, value);
        if ((sint32)(value << 16) <= 0)
        {
            Explode();
            FinishFromTable();
        }
    }
    return 1;
}

uint32 FloatingHazard::UpdateHeight()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 state, result, y, floor;
    FUNCTION_MARKER(0x8005AADC, "SLUS_003.73");
    if (r_u16(object + 472))
        return 1;
    state = r_u16(object + 474);
    result = state < 2;
    if (state != 1)
    {
        if (!result || state)
            return result;
        result = r_u16(object + 474) + 1;
        motion.acceleration[1] = 24576;
        w_u16(object + 474, result);
    }
    y = this->model.pos[1];
    floor = r_u32(0x800FF37C);
    result = (sint32)floor < (sint32)y;
    if (result)
    {
        result = r_u16(object + 474);
        motion.acceleration[1] = 0;
        motion.velocity[1] = 0;
        this->model.pos[1] = floor;
        ++result;
        w_u16(object + 474, result);
    }
    return result;
}

uint32 FloatingHazard::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, value, player, damage, distance, phase, height, y;
    sint16 angles[4];
    FUNCTION_MARKER(0x8005A860, "SLUS_003.73");
    if (!motion.velocity[1])
    {
        y = this->model.pos[1];
        height = r_u32(object + 500);
        this->model.pos[1] = y - height;
    }
    flags = this->activation_flags;
    w_u16(object + 388, flags);
    if (flags & 1)
    {
        value = r_u8(object + 384);
        this->activation_flags = flags & 0xFFFE;
        w_u8(object + 384, value + 1);
    }
    if (r_u8(object + 380))
    {
        value = r_u16(object + 476);
        if (value)
            w_u16(object + 476, value - 1);
        else
            sub_8004BE90(object, r_u32(object + 400));
    }
    else
        UpdateHeight();
    if (visual_lists.players)
    {
        value = r_u16(object + 438);
        distance = r_u32(object + 220);
        if (distance < value)
        {
            Explode();
            damage = ScaleDamageForDifficulty(r_u16(object + 440));
            player = visual_lists.players;
            ApplyObjectDamage(player, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            FinishFromTable();
        }
    }
    if (r_u32(object + 396) & 1)
    {
        distance = MeasurePositionDistance((const uint32 *)this->model.pos, (const uint32 *)psx_addr(object + 484, 12));
        if ((sint32)distance < 129)
            w_u32(object + 396, r_u32(object + 396) & ~1);
        else
        {
            xport_draft_host_sub_80066B8C_p1(angles, object + 4, object + 484);
            BuildDirectionVector(motion.acceleration, r_u16(object + 436), (const uint16 *)angles);
        }
    }
    if (!RandomRange(64))
    {
        value = RandomRange(256) - 128;
        motion.angular_velocity[0] = 0;
        motion.angular_velocity[1] = value;
        motion.angular_velocity[2] = 0;
    }
    if (!motion.velocity[1])
    {
        value = RandomRange(512);
        phase = (r_u16(object + 496) + value) & 4095;
        w_u16(object + 496, phase);
        height = (uint32)rsin(phase);
        y = this->model.pos[1];
        height <<= 5;
        w_u32(object + 500, height);
        this->model.pos[1] = y + height;
    }
    return sub_8004B9A4(object);
}

uint32 FloatingHazard::ExecuteCommand(uint32 command)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, table, receiver, target;
    FUNCTION_MARKER(0x8005AB58, "SLUS_003.73");
    command &= 65535;
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = r_u32(object + 400);
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    if (value & 0x2000)
    {
        table = r_u32(object + 68);
        receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
        target = r_u32(table + 84);
        value = DispatchObjectParameterGetter(target, receiver, value & 65535);
    }
    ReadRecordPosition((uint32 *)psx_addr(object + 484, 12), value & 65535);
    value = r_u32(object + 396);
    w_u32(object + 396, value | 1);
    return 1;
}



extern "C" uint32 sub_8005A74C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005A74C, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::Destroy(reason);
}

extern "C" uint32 sub_8005A7B0(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x8005A7B0, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::Damage(damage, 0, 0);
}

extern "C" uint32 sub_8005A860(uint32 object)
{
    FUNCTION_MARKER(0x8005A860, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::Update();
}



extern "C" uint32 sub_8005AB58(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8005AB58, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::ExecuteCommand(command);
}

BallisticHazard::BallisticHazard(const uint32 *position, uint32 kind, uint32 flags, uint32 parameter, sint32 vertical_speed) : Enemy()
{
    uint32 trig_angle;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, value, radius, player, coefficient, floor;
    FUNCTION_MARKER(0x8005ABF8, "SLUS_003.73");
    w_u32(object + 68, 0x800A28B0);
    x = ReadMovingRecordPosition(position, 0);
    y = ReadMovingRecordPosition(position, 1);
    z = ReadMovingRecordPosition(position, 2);
    pos_history.saved[0] = x;
    pos_history.saved[1] = y;
    pos_history.saved[2] = z;
    x = pos_history.saved[0];
    y = pos_history.saved[1];
    z = pos_history.saved[2];
    this->model.pos[0] = x;
    this->model.pos[1] = y;
    this->model.pos[2] = z;
    if (kind == 2)
    {
        SetModelBank((const char *)psx_addr(0x800FF574, 1));
        value = FindModelResourceIndex(0x717FB6B2, r_u8(0x800FF645));
        this->model.model_idx = value;
        motion.acceleration[1] = (uint32)vertical_speed << 12;
        w_u16(object + 200, 128);
    }
    else if ((sint32)kind >= 0 && (sint32)kind <= 3)
    {
        SetModelBank((const char *)psx_addr(r_u32(0x800FF62C), 1));
        value = FindModelResourceIndex(0x2D990A1E, r_u8(0x800FF644));
        this->model.model_idx = value;
        motion.acceleration[1] = (uint32)vertical_speed << 12;
        this->model.angles[1] = RandomRange(4096);
        value = 256 - RandomRange(128);
        motion.angular_drag[0] = 16;
        w_u16(object + 200, 128);
        motion.angular_velocity[0] = value;
        w_u16(object + 436, kind == 3 ? 48 : 32);
        w_u16(object + 438, 256);
        w_u16(object + 440, kind == 3 ? 4 : 8);
        if (kind == 1)
        {
            radius = RandomRange(512);
            trig_angle = RandomRange(4096) & 4095;
            coefficient = (uint32)rsin(trig_angle);
            player = visual_lists.players;
            value = r_u32(player + 4);
            this->model.pos[0] = value + radius * coefficient;
            coefficient = (uint32)rcos(trig_angle);
            value = r_u32(player + 12);
            this->model.pos[2] = value + radius * coefficient;
            x = this->model.pos[0];
            y = this->model.pos[1];
            z = this->model.pos[2];
            pos_history.saved[0] = x;
            pos_history.saved[1] = y;
            pos_history.saved[2] = z;
        }
    }
    this->model.flags = this->model.flags & 0xFFFD;
    AddToList(visual_lists.actors);
    motion.drag[0] = 2;
    motion.drag[1] = 5;
    motion.drag[2] = 2;
    w_u16(object + 212, 0);
    this->kind = 410;
    w_u16(object + 496, flags);
    w_u32(object + 500, parameter);
    this->lifecycle_flags = this->lifecycle_flags & 0xFFEF;
    if (flags & 5)
    {
        floor = FindGroundHeight((const uint32 *)this->model.pos, 0, 8192);
        if (floor == (uint32)-1)
        {
            Enemy::Finish();
            return;
        }
        w_u32(object + 488, floor);
        EnableShadow();
        value = this->model.pos[1];
        x = this->model.pos[0];
        value = floor - value;
        w_u32(object + 184, x);
        z = this->model.pos[2];
        w_u16(object + 204, (uint32)((sint32)value >> 12));
        w_u32(object + 188, floor);
        w_u32(object + 192, z);
    }
    else if (flags & 2)
    {
        EnableShadow();
        floor = r_u32(0x800FF37C);
        value = this->model.pos[1];
        x = this->model.pos[0];
        w_u16(object + 204, (uint32)((sint32)(floor - value) >> 12));
        w_u32(object + 184, x);
        z = this->model.pos[2];
        w_u32(object + 188, floor - 65536);
        w_u32(object + 192, z);
    }
}

uint32 BallisticHazard::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8005AF34, "SLUS_003.73");
    w_u32(object + 68, 0x800A28B0);
    RemoveFromList(visual_lists.actors);
    result = Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

uint32 BallisticHazard::Damage(uint32, const sint32 *, uint32)
{
    FUNCTION_MARKER(0x8005AF98, "SLUS_003.73");
    return 0;
}



extern "C" uint32 sub_8005AF34(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005AF34, "SLUS_003.73");
    return BallisticHazard::Attach(object).BallisticHazard::Destroy(reason);
}

extern "C" uint32 sub_8005AF98(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x8005AF98, "SLUS_003.73");
    return BallisticHazard::Attach(object).BallisticHazard::Damage(damage, 0, 0);
}

uint32 BallisticHazard::FinishFromTable()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    return DispatchObjectFinish(target, receiver);
}

static uint32 DamageBallisticHazardPlayer(uint32 damage)
{
    uint32 player = visual_lists.players;
    return ApplyObjectDamage(player, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
}

uint32 BallisticHazard::Update()
{
    uint32 trig_angle;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, flags, result, y, floor, parameter, distance, threshold, damage;
    uint32 value, point[3], velocity[3], random, product, kind, name, index;
    sint32 quotient;
    FUNCTION_MARKER(0x8005AFA0, "SLUS_003.73");
    child = r_u32(object + 196);
    if (child)
        w_u16(child + 64, 128);
    Enemy::IntegrateMotion();
    flags = r_u16(object + 496);
    result = flags & 2;
    if (flags & 5)
    {
        y = this->model.pos[1];
        floor = r_u32(object + 488);
        if ((sint32)y >= (sint32)floor)
        {
            if (flags == 4)
            {
                this->model.pos[1] = floor;
                SpawnFireCycle((const uint32 *)this->model.pos, 180, 1, 2);
                parameter = r_u32(object + 500);
                SpawnExplosionRing((const uint32 *)this->model.pos, (uint32)((sint32)parameter >> 1), 255, 128, 0, 4, 1, 100);
                distance = r_u32(object + 220);
                parameter = r_u32(object + 500);
                if (distance < parameter)
                {
                    damage = ScaleDamageForDifficulty(20);
                    DamageBallisticHazardPlayer(damage);
                }
                child = AllocateEffectStorage(120);
                if (child)
                {
                    parameter = r_u32(object + 500);
                    velocity[0] = r_u32(0x800A71CC);
                    velocity[1] = r_u32(0x800A71D0);
                    velocity[2] = r_u32(0x800A71D4);
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ImpactRing));
                    new (dram_addr) ImpactRing(velocity);
                    ImpactRing::Attach(child).SetGeometry((const uint32 *)this->model.pos, 0, 0);
                    value = (uint32)((sint32)parameter / 16);
                    w_u16(child + 10, 16);
                    w_u16(child + 116, value);
                    quotient = (sint16)value;
                    w_u16(child + 118, (uint32)(quotient / 2));
                }
            }
            else if (flags == 1)
            {
                if (r_u32(0x800FF378) == 1)
                    PlayPositionedSound(59, (const uint32 *)this->model.pos, 0);
                for (index = 0; index < r_u16(object + 440); ++index)
                {
                    random = RandomRange(4096);
                    floor = r_u32(object + 488);
                    this->model.pos[1] = floor - 0x20000;
                    trig_angle = random & 4095;
                    value = (uint32)rsin(trig_angle);
                    parameter = r_u32(object + 500);
                    product = value * parameter;
                    velocity[0] = (uint32)((sint32)product / 8);
                    random = RandomRange(32);
                    value = (uint32)rcos(trig_angle);
                    parameter = r_u32(object + 500);
                    product = value * parameter;
                    velocity[1] = (0 - 16 - random) << 12;
                    velocity[2] = (uint32)((sint32)product / 8);
                    child = AllocateZeroedObject(320);
                    if (child)
                    {
                        kind = FindModelResourceIndex(0x51FBD441, r_u8(0x800FF644)) & 65535;
                        floor = r_u32(object + 488);
                        name = r_u32(0x800FF62C);
                        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(Debris));
                        new (dram_addr) Debris((const uint32 *)this->model.pos, velocity, kind, floor, 193, 24, psx_addr(name, 1));
                    }
                    // Skip the missing child under the approved allocation-failure contract
                    value = r_u16(object + 436) >> 3;
                    if (child)
                        w_u8(child + 297, value);
                }
            }
            return FinishFromTable();
        }
        value = r_u32(object + 188);
        distance = r_u32(object + 220);
        threshold = r_u16(object + 438);
        result = (uint32)((sint32)(value - y) >> 12);
    }
    else
    {
        if (!result)
            return result;
        y = this->model.pos[1];
        floor = r_u32(0x800FF37C);
        if ((sint32)y >= (sint32)floor)
        {
            point[0] = this->model.pos[0];
            point[1] = this->model.pos[1];
            point[2] = this->model.pos[2];
            point[1] -= 286720;
            CreateSplash(point);
            PlayPositionedSound(57, (const uint32 *)this->model.pos, 0);
            return FinishFromTable();
        }
        value = r_u32(object + 188);
        threshold = r_u16(object + 438);
        distance = r_u32(object + 220);
        result = (uint32)((sint32)(value - y) >> 12);
    }
    w_u16(object + 202, result);
    if (distance < threshold)
    {
        result = visual_lists.players;
        if (result)
        {
            damage = ScaleDamageForDifficulty(r_u16(object + 436));
            return DamageBallisticHazardPlayer(damage);
        }
    }
    return result;
}

extern "C" uint32 sub_8005AFA0(uint32 object)
{
    FUNCTION_MARKER(0x8005AFA0, "SLUS_003.73");
    return BallisticHazard::Attach(object).BallisticHazard::Update();
}

FloatingHazard &FloatingHazard::Attach(uint32 object)
{
    return *static_cast<FloatingHazard *>(FindNativeObjectStorage(object));
}

BallisticHazard &BallisticHazard::Attach(uint32 object)
{
    return *static_cast<BallisticHazard *>(FindNativeObjectStorage(object));
}
