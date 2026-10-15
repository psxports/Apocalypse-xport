extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "exp_act_burst.h"
#include "effect_sprite.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "effect_color.h"
#include "angle_math.h"
#include "game_random.h"
#include <new>
#include <string.h>

// Native boundary preserves mapped position reloads
static uint32 ReadBurstWord(const uint32 *input, uint32 axis)
{
    uint32 address = GuestMemoryAddress(input), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

ExpActBurst::ExpActBurst(const uint32 *position, uint32 count, uint32 spread, uint32 jitter_y, uint32 upward_speed, uint32 gravity, uint32 drag_y, uint32 speed, uint32 acceleration, uint32 drag_xz, uint32 angle_speed, uint32 angle_acceleration, uint32 size_step, uint32 color_step, uint32 fade_start, uint32 field92) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 origin[3], target[3], direction[3], temporary[3], child_position[3], pair, side, child, scalar, random, slot = object + 72;
    sint16 angles[4];
    FUNCTION_MARKER(0x8001D484, "SLUS_003.73");
    w_u32(object + 68, 0x800A1184);
    SetPosition(position);
    origin[0] = r_u32(0x800ED520) << 12;
    origin[2] = r_u32(0x800ED528) << 12;
    origin[1] = 0;
    target[0] = ReadBurstWord(position, 0);
    target[2] = ReadBurstWord(position, 2);
    target[1] = 0;
    CalculateDirectionAngles((uint16 *)angles, origin, target);
    angles[1] = (sint16)((uint32)(uint16)angles[1] + 1024);
    BuildDirectionVector(direction, 1, (const uint16 *)angles);
    uint32 velocity_y = ((uint32)0 - upward_speed) << 12;
    uint32 acceleration_y = (uint32)0 - gravity;
    uint32 vertical_range = jitter_y * 2 + 1;
    w_u32(object + 156, fade_start);
    w_u32(object + 152, count);
    for (pair = 0; (sint32)pair < (sint32)count / 2; ++pair)
    {
        for (side = 0; side < 2; ++side)
        {
            scalar = RandomRange(spread);
            MultiplyVector(temporary, direction, &scalar);
            if (!side)
                AddVectors(child_position, (const uint32 *)psx_addr(object + 24, 12), temporary);
            else
                SubtractVectors(child_position, (const uint32 *)psx_addr(object + 24, 12), temporary);
            random = RandomRange(vertical_range);
            child_position[1] += (random - jitter_y) << 12;
            child = AllocateEffectStorage(120);
            if (child)
            {
                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(AnimatedSprite));
                new (dram_addr) AnimatedSprite(child_position, VFX_EXP_ACT, 1000, 1, 0, (uint32)-2);
            }
            w_u8(child + 66, 1);
            w_u32(slot, child);
            slot += 4;
            scalar = side ? (uint32)0 - speed : speed;
            MultiplyVector(temporary, direction, &scalar);
            w_u32(child + 36, temporary[0]);
            w_u32(child + 40, temporary[1]);
            w_u32(child + 44, temporary[2]);
            w_u32(child + 40, velocity_y);
            scalar = side ? (uint32)0 - acceleration : acceleration;
            MultiplyVector(temporary, direction, &scalar);
            w_u32(child + 48, temporary[0]);
            w_u32(child + 52, temporary[1]);
            w_u32(child + 56, temporary[2]);
            w_u32(child + 52, acceleration_y);
            w_u8(child + 60, drag_xz);
            w_u8(child + 62, drag_xz);
            w_u8(child + 61, drag_y);
            random = RandomRange(4096);
            w_u16(child + 96, random);
            w_u16(child + 98, side ? (uint32)0 - angle_speed : angle_speed);
            w_u16(child + 100, side ? (uint32)0 - angle_acceleration : angle_acceleration);
            w_u16(child + 72, color_step);
            w_u16(child + 74, size_step);
            AnimatedSprite::Attach(child).SetFrameStep((uint32)(sint32)(sint16)field92);
            AnimatedSprite::Attach(child).AddFlags(8);
        }
    }
}

ExpActBurst &ExpActBurst::Attach(uint32 object)
{
    return *static_cast<ExpActBurst *>(FindNativeObjectStorage(object));
}

uint32 ExpActBurst::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, count, child, table, receiver, target;
    FUNCTION_MARKER(0x8001D918, "SLUS_003.73");
    count = r_u32(object + 152);
    w_u32(object + 68, 0x800A1184);
    if ((sint32)count > 0)
    {
        do
        {
            child = r_u32(object + 72 + index * 4);
            if (child)
            {
                table = r_u32(child + 68);
                receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
                target = r_u32(table + 12);
                apocalypse_object_cleanup(target, receiver, 3);
            }
            count = r_u32(object + 152);
            ++index;
        } while ((sint32)index < (sint32)count);
    }
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

uint32 ExpActBurst::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, slot = object + 72, child, result, empty = 1, first, second, table, receiver, target;
    FUNCTION_MARKER(0x8001D9D0, "SLUS_003.73");
    sint16 age = (sint16)(r_u16(object + 8) + 1);
    w_u16(object + 8, (uint16)age);
    if (age == 1)
        return 1;
    result = r_u32(object + 152);
    if ((sint32)result > 0)
    {
        do
        {
            child = r_u32(slot);
            if (child)
            {
                AddVectors((uint32 *)psx_addr(child + 24, 12), (const uint32 *)psx_addr(child + 36, 12));
                child = r_u32(slot);
                AddVectors((uint32 *)psx_addr(child + 36, 12), (const uint32 *)psx_addr(child + 48, 12));
                child = r_u32(slot);
                DampVector((uint32 *)psx_addr(child + 36, 12), (const uint8 *)psx_addr(child + 60, 3));
                child = r_u32(slot);
                first = r_u16(child + 96);
                second = r_u16(child + 98);
                w_u16(child + 96, first + second);
                child = r_u32(slot);
                first = r_u16(child + 98);
                second = r_u16(child + 100);
                w_u16(child + 98, first + second);
                child = r_u32(slot);
                first = r_u16(child + 94);
                second = r_u16(child + 74);
                w_u16(child + 94, first + second);
                age = (sint16)r_u16(object + 8);
                first = r_u32(object + 156);
                empty = 0;
                if ((sint32)first < age)
                {
                    child = r_u32(slot);
                    first = (uint32)(sint32)(sint16)r_u16(child + 72);
                    FadePackedColor((uint32 *)psx_addr(child + 76, 4), (sint32)first);
                    child = r_u32(slot);
                    if (!(r_u32(child + 76) & 0xFFFFFF))
                    {
                        if (child)
                        {
                            table = r_u32(child + 68);
                            receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
                            target = r_u32(table + 12);
                            apocalypse_object_cleanup(target, receiver, 3);
                        }
                        w_u32(slot, 0);
                    }
                }
            }
            result = r_u32(object + 152);
            ++index;
            result = (uint32)((sint32)index < (sint32)result);
            slot += 4;
        } while (result);
    }
    return empty ? Finish() : result;
}

extern "C" uint32 apocalypse_explosion_debris_native(uint32 object, const void *position, uint32 count, uint32 spread, uint32 jitter_y, uint32 upward_speed, uint32 gravity, uint32 drag_y, uint32 speed, uint32 acceleration, uint32 drag_xz, uint32 angle_speed, uint32 angle_acceleration, uint32 size_step, uint32 color_step, uint32 fade_start, uint32 field92)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ExpActBurst));
    new (dram_addr) ExpActBurst((const uint32 *)position, count, spread, jitter_y, upward_speed, gravity, drag_y, speed, acceleration, drag_xz, angle_speed, angle_acceleration, size_step, color_step, fade_start, field92);
    return object;
}

extern "C" uint32 sub_8001D484(uint32 object, uint32 position, uint32 count, uint32 spread, uint32 jitter_y, uint32 upward_speed, uint32 gravity, uint32 drag_y, uint32 speed, uint32 acceleration, uint32 drag_xz, uint32 angle_speed, uint32 angle_acceleration, uint32 size_step, uint32 color_step, uint32 fade_start, uint32 field92)
{
    FUNCTION_MARKER(0x8001D484, "SLUS_003.73");
    return apocalypse_explosion_debris_native(object, psx_addr(position, 12), count, spread, jitter_y, upward_speed, gravity, drag_y, speed, acceleration, drag_xz, angle_speed, angle_acceleration, size_step, color_step, fade_start, field92);
}

extern "C" uint32 sub_8001D918(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001D918, "SLUS_003.73");
    return ExpActBurst::Attach(object).ExpActBurst::Destroy(reason);
}

extern "C" uint32 sub_8001D9D0(uint32 object)
{
    FUNCTION_MARKER(0x8001D9D0, "SLUS_003.73");
    return ExpActBurst::Attach(object).ExpActBurst::Update();
}
