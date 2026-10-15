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

#include "mesh_ring_effect.h"
#include "credits_player.h"
#include "angle_math.h"
#include "animation.h"
#include "model_animation.h"
#include "vector_arithmetic.h"

// Shared pose dispatch has no independently established class receiver

static void ReleaseCharacterAnimationBuffers(uint32 object)
{
    FUNCTION_MARKER(0x80062FB8, "SLUS_003.73");
    uint32 allocation = r_u32(object + 356);
    w_u32(object + 68, 0x800A3340);
    if (allocation)
        ReleaseHeapBlock(allocation);
    allocation = r_u32(object + 360);
    if (allocation)
        ReleaseHeapBlock(allocation);
}

static uint32 ReadAnimationFrameCount(uint32 object, uint32 animation)
{
    uint32 model, table;
    model = r_u8(object + 27);
    w_u8(object + 26, animation);
    table = r_u32(0x800EAEF8 + model * 64 + 24);
    return r_u8(table + (animation & 255) * 8 + 8);
}

static void CopyAnimationPosition(uint32 destination, uint32 source)
{
    uint32 x = r_u32(source);
    uint32 y = r_u32(source + 4);
    uint32 z = r_u32(source + 8);
    w_u32(destination, x);
    w_u32(destination + 4, y);
    w_u32(destination + 8, z);
}

static void CopyAnimationAngles(uint32 destination, uint32 source)
{
    uint32 upper, lower, shift, word, tail, lane;
    upper = r_u32((source + 3) & ~3);
    lower = r_u32(source & ~3);
    shift = (source & 3) * 8;
    word = shift ? (upper << (32 - shift)) | (lower >> shift) : lower;
    tail = r_u16(source + 4);
    lane = (destination + 3) & 3;
    w_masked_u32((destination + 3) & ~3, word >> (8 * (3 - lane)), (1 << (lane + 1)) - 1);
    lane = destination & 3;
    w_masked_u32(destination & ~3, word << (8 * lane), (15 << lane) & 15);
    w_u16(destination + 4, tail);
}

static uint32 DispatchAnimationObject(uint32 target, uint32 object)
{
    switch (target)
    {
        case 0x8002D3B4: return sub_8002D3B4(object);
        case 0x80040F34: return sub_80040F34(object);
        case 0x8003D790: return sub_8003D790(object);
        case 0x80047DBC: return sub_80047DBC(object);
        case 0x8002BA40: return sub_8002BA40(object);
        case 0x8002AD9C: return sub_8002AD9C(object);
        case 0x8002C95C: return sub_8002C95C(object);
        case 0x8002DC38: return sub_8002DC38(object);
        case 0x800639AC:
            nullsub_27();
            // The original empty callback retains the JALR target in V0
            return target;
        case 0x800449EC:
            sub_800449EC(object);
            return 0;
        case 0x80071EF0:
        case 0x8004A37C:
        case 0x80073A3C:
        case 0x80074BA8:
        case 0x8005848C:
        case 0x800563B4:
        case 0x80053FA8:
        case 0x800598B8:
            return apocalypse_enemy_update_cpp(target, object);
        case 0x8005B890: return sub_8005B890(object);
        case 0x800209EC: return sub_800209EC(object);
        case 0x80026488: return sub_80026488(object);
        case 0x80025BEC: return sub_80025BEC(object);
        case 0x80026C68: return sub_80026C68(object);
        case 0x8002948C: return MeshRingEffect::Attach(object).MeshRingEffect::Render();
        case 0x8003BBD8: return sub_8003BBD8(object);
        case 0x8004D800: return sub_8004D800(object);
        case 0x8004E71C: return apocalypse_enemy_update_cpp(target, object);
        case 0x800453F4: return apocalypse_enemy_update_cpp(target, object);
        case 0x8005BD48: return sub_8005BD48(object);
        case 0x8005F688: return sub_8005F688(object);
        case 0x80014920: return CreditsPlayer::Attach(object).CreditsPlayer::Update();
        case 0x80061F54: return sub_80061F54(object);
        case 0x80061D20: return sub_80061D20(object);
        case 0x80076D1C: return sub_80076D1C(object);
        case 0x8005A860: return sub_8005A860(object);
        case 0x8001A550: return sub_8001A550(object);
        case 0x800193F8: return sub_800193F8(object);
        case 0x800198A4: return sub_800198A4(object);
        case 0x8001A14C: return sub_8001A14C(object);
        case 0x8005AFA0: return sub_8005AFA0(object);
        case 0x8001F2D4: return sub_8001F2D4(object);
        case 0x8005C47C: return sub_8005C47C(object);
        case 0x800622D8:
            sub_800622D8(object);
            // The current empty-callee contract carries the JALR target
            return target;
        case 0x8007876C: return sub_8007876C(object);
        default:
            // TODO Resolve other targets from their complete original contracts
            GameDiagnostic("Unimplemented object callback 0x%08X\n", target);
            abort();
    }
}

static void UpdateAnimationCallback(uint32 object)
{
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 32);
    uint32 target = r_u32(table + 36);
    if (target == 0x800639A4)
        nullsub_22();
    else
        DispatchAnimationObject(target, receiver);
}

static uint32 CallAnimationBehavior(uint32 object, uint32 table)
{
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 24);
    uint32 target = r_u32(table + 28);
    return DispatchAnimationObject(target, receiver);
}

static uint32 UpdateAnimationBehavior(uint32 object)
{
    uint32 frame, animation, direction, table;
    frame = r_u8(object + 24);
    table = r_u32(object + 68);
    w_u16(object + 310, (uint32)(sint32)(sint8)frame);
    CallAnimationBehavior(object, table);
    frame = r_u8(object + 24);
    animation = r_u8(object + 26);
    direction = r_u8(object + 297);
    w_u16(object + 308, (uint32)(sint32)(sint8)frame);
    w_u8(object + 312, animation);
    w_u8(object + 313, direction);
    return (uint32)(sint32)(sint8)frame;
}

uint32 Character::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    ReleaseCharacterAnimationBuffers(object);
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 DestroyCharacterBase(uint32 object, uint32 reason)
{
    ReleaseCharacterAnimationBuffers(object);
    DestroyVisualObjectBase(object, 0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

Character::Character() : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80062F64, "SLUS_003.73");
    w_u32(object + 68, 0x800A3340);
    w_u32(object + 304, 65536);
    flags = r_u16(object);
    w_u8(object + 302, 1);
    w_u8(object + 303, 1);
    w_u16(object, flags | 2);
}

uint32 InitializeCharacterBase(uint32 object)
{
    uint32 flags;
    FUNCTION_MARKER(0x80062F64, "SLUS_003.73");
    sub_80062924(object);
    w_u32(object + 68, 0x800A3340);
    w_u32(object + 304, 65536);
    flags = r_u16(object);
    w_u8(object + 302, 1);
    w_u8(object + 303, 1);
    w_u16(object, flags | 2);
    return object;
}

uint32 SetAnimationRange(uint32 object, uint32 animation, uint32 first, uint32 last)
{
    uint32 count, result;
    sint32 current, final_frame;
    FUNCTION_MARKER(0x80063038, "SLUS_003.73");
    count = ReadAnimationFrameCount(object, animation);
    w_u8(object + 302, count);
    if (first == (uint32)-1)
        first = count - 1;
    if (last == (uint32)-1)
        last = r_u8(object + 302) - 1;
    if ((sint32)first < 0 || (sint32)first >= (sint32)r_u8(object + 302))
        first = 0;
    if ((sint32)last < 0 || (sint32)last >= (sint32)r_u8(object + 302))
        last = 0;
    w_u8(object + 296, 0);
    if ((sint32)first < (sint32)last)
        w_u8(object + 297, 1);
    else if ((sint32)last < (sint32)first)
        w_u8(object + 297, (uint32)-1);
    else
        w_u8(object + 297, 0);
    w_u8(object + 24, first);
    current = (sint8)r_u8(object + 24);
    w_u8(object + 298, last);
    final_frame = (sint8)r_u8(object + 298);
    w_u16(object + 300, 0);
    result = current == final_frame;
    w_u8(object + 303, result);
    return result;
}

uint32 Character::SetAnimationRange(uint32 animation, uint32 first, uint32 last)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, result;
    sint32 current, final_frame;
    FUNCTION_MARKER(0x80063038, "SLUS_003.73");
    count = ReadAnimationFrameCount(object, animation);
    w_u8(object + 302, count);
    if (first == (uint32)-1)
        first = count - 1;
    if (last == (uint32)-1)
        last = r_u8(object + 302) - 1;
    if ((sint32)first < 0 || (sint32)first >= (sint32)r_u8(object + 302))
        first = 0;
    if ((sint32)last < 0 || (sint32)last >= (sint32)r_u8(object + 302))
        last = 0;
    w_u8(object + 296, 0);
    if ((sint32)first < (sint32)last)
        w_u8(object + 297, 1);
    else if ((sint32)last < (sint32)first)
        w_u8(object + 297, (uint32)-1);
    else
        w_u8(object + 297, 0);
    w_u8(object + 24, first);
    current = (sint8)r_u8(object + 24);
    w_u8(object + 298, last);
    final_frame = (sint8)r_u8(object + 298);
    w_u16(object + 300, 0);
    result = current == final_frame;
    w_u8(object + 303, result);
    return result;
}

uint32 StartAnimation(uint32 object, uint32 animation, uint32 direction)
{
    uint32 count;
    FUNCTION_MARKER(0x80063118, "SLUS_003.73");
    count = ReadAnimationFrameCount(object, animation);
    w_u8(object + 296, 1);
    w_u8(object + 297, direction);
    w_u8(object + 24, 0);
    w_u16(object + 300, 0);
    w_u8(object + 303, 0);
    w_u8(object + 302, count);
    return 1;
}

uint32 Character::StartAnimation(uint32 animation, uint32 direction)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count;
    FUNCTION_MARKER(0x80063118, "SLUS_003.73");
    count = ReadAnimationFrameCount(object, animation);
    w_u8(object + 296, 1);
    w_u8(object + 297, direction);
    w_u8(object + 24, 0);
    w_u16(object + 300, 0);
    w_u8(object + 303, 0);
    w_u8(object + 302, count);
    return 1;
}

uint32 AdvanceAnimation(uint32 object)
{
    uint32 phase, fraction, speed, mode, count, result;
    sint32 frame, direction, last;
    FUNCTION_MARKER(0x80063164, "SLUS_003.73");
    frame = (sint8)r_u8(object + 24);
    fraction = r_u16(object + 300);
    direction = (sint8)r_u8(object + 297);
    phase = ((uint32)frame << 16) | fraction;
    speed = r_u32(object + 304);
    if (direction == 1)
        phase += speed;
    if (direction == -1)
        phase -= speed;
    mode = r_u8(object + 296);
    result = (uint32)((sint32)phase >> 16);
    w_u8(object + 24, result);
    w_u16(object + 300, phase);
    if (mode)
    {
        if (mode != 1)
            return result;
        frame = (sint8)r_u8(object + 24);
        count = r_u8(object + 302);
        if (frame >= (sint32)count)
            w_u8(object + 24, 0);
        result = (uint32)(sint32)(sint8)r_u8(object + 24);
        if ((sint32)result < 0)
        {
            result = r_u8(object + 302) - 1;
            w_u8(object + 24, result);
        }
        return result;
    }
    direction = (sint8)r_u8(object + 297);
    if (direction == 1)
    {
        frame = (sint8)r_u8(object + 24);
        last = (sint8)r_u8(object + 298);
        result = frame < last;
        if (!result)
            goto Complete;
    }
    if (direction == -1)
    {
        frame = (sint8)r_u8(object + 24);
        last = (sint8)r_u8(object + 298);
        result = last < frame;
        if (!result)
            goto Complete;
    }
    return result;
Complete:
    count = r_u8(object + 298);
    w_u8(object + 303, 1);
    w_u8(object + 24, count);
    return 1;
}

uint32 Character::AdvanceAnimation()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 phase, fraction, speed, mode, count, result;
    sint32 frame, direction, last;
    FUNCTION_MARKER(0x80063164, "SLUS_003.73");
    frame = (sint8)r_u8(object + 24);
    fraction = r_u16(object + 300);
    direction = (sint8)r_u8(object + 297);
    phase = ((uint32)frame << 16) | fraction;
    speed = r_u32(object + 304);
    if (direction == 1)
        phase += speed;
    if (direction == -1)
        phase -= speed;
    mode = r_u8(object + 296);
    result = (uint32)((sint32)phase >> 16);
    w_u8(object + 24, result);
    w_u16(object + 300, phase);
    if (mode)
    {
        if (mode != 1)
            return result;
        frame = (sint8)r_u8(object + 24);
        count = r_u8(object + 302);
        if (frame >= (sint32)count)
            w_u8(object + 24, 0);
        result = (uint32)(sint32)(sint8)r_u8(object + 24);
        if ((sint32)result < 0)
        {
            result = r_u8(object + 302) - 1;
            w_u8(object + 24, result);
        }
        return result;
    }
    direction = (sint8)r_u8(object + 297);
    if (direction == 1)
    {
        frame = (sint8)r_u8(object + 24);
        last = (sint8)r_u8(object + 298);
        result = frame < last;
        if (!result)
            goto Complete;
    }
    if (direction == -1)
    {
        frame = (sint8)r_u8(object + 24);
        last = (sint8)r_u8(object + 298);
        result = last < frame;
        if (!result)
            goto Complete;
    }
    return result;
Complete:
    count = r_u8(object + 298);
    w_u8(object + 303, 1);
    w_u8(object + 24, count);
    return 1;
}

uint32 DidAnimationCrossFrame(uint32 object, uint32 animation, uint32 target_frame)
{
    sint32 current, previous, frame, direction;
    FUNCTION_MARKER(0x8006325C, "SLUS_003.73");
    if (animation != r_u8(object + 312))
        return 0;
    current = (sint16)r_u16(object + 308);
    previous = (sint16)r_u16(object + 310);
    frame = (sint32)target_frame;
    direction = (sint8)r_u8(object + 313);
    if (previous < current)
    {
        if (direction < 0)
            return frame >= previous && current >= frame;
        return previous >= frame || frame >= current;
    }
    if (direction < 0)
        return current >= frame || frame >= previous;
    return frame >= current && frame <= previous;
}

uint32 Character::DidAnimationCrossFrame(uint32 animation, uint32 target_frame)
{
    const uint32 object = NativeObjectGuestAddress(this);
    sint32 current, previous, frame, direction;
    FUNCTION_MARKER(0x8006325C, "SLUS_003.73");
    if (animation != r_u8(object + 312))
        return 0;
    current = (sint16)r_u16(object + 308);
    previous = (sint16)r_u16(object + 310);
    frame = (sint32)target_frame;
    direction = (sint8)r_u8(object + 313);
    if (previous < current)
    {
        if (direction < 0)
            return frame >= previous && current >= frame;
        return previous >= frame || frame >= current;
    }
    if (direction < 0)
        return current >= frame || frame >= previous;
    return frame >= current && frame <= previous;
}

uint32 Character::UpdateAnimationModel(uint32 model)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8006331C, "SLUS_003.73");
    if (!r_u32(object + 360))
    {
        InitializeModelAnimation(object, model);
        w_u32(object + 368, model);
    }
    UpdateModelAnimationData(object);
    result = r_u16(object) & 4;
    if (result)
    {
        UpdateModelWorldTransform(object);
        return BuildModelPose(object);
    }
    return result;
}

uint32 UpdateAnimationPose(uint8 *object_data)
{
    const uint32 object = GuestMemoryAddress(object_data);
    if (!object)
    {
        // TODO Bind native object data to the original pose memory contract
        GameDiagnostic("TODO Animation pose data is outside guest memory\n");
        abort();
    }
    uint32 flags, step, count, delta[3], motion[3], axis, value;
    uint16 angles[4];
    FUNCTION_MARKER(0x8006338C, "SLUS_003.73");
    flags = r_u16(object) & 2;
    if (flags)
        AdvanceAnimation(object);
    UpdateAnimationCallback(object);
    if (r_u8(object + 225) < 2)
        return flags ? UpdateAnimationBehavior(object) : CallAnimationBehavior(object, r_u32(object + 68));
    step = r_u8(object + 226);
    count = r_u8(object + 225);
    step = (step + 1) & 255;
    w_u8(object + 226, step);
    if (step < count)
    {
        AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 264, 12));
        AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 288, 6));
        return NormalizeAngles((uint16 *)psx_addr(object + 16, 6));
    }
    w_u8(object + 226, 0);
    CopyAnimationPosition(object + 4, object + 252);
    CopyAnimationPosition(object + 228, object + 4);
    CopyAnimationAngles(object + 16, object + 282);
    CopyAnimationAngles(object + 276, object + 16);
    if (r_u16(object) & 2)
        UpdateAnimationBehavior(object);
    else
        CallAnimationBehavior(object, r_u32(object + 68));
    CopyAnimationPosition(object + 252, object + 4);
    CopyAnimationPosition(object + 4, object + 228);
    SubtractVectors(delta, (const uint32 *)psx_addr(object + 252, 12), (const uint32 *)psx_addr(object + 228, 12));
    count = r_u8(object + 225);
    DivideVector(motion, delta, &count);
    w_u32(object + 264, motion[0]);
    w_u32(object + 268, motion[1]);
    w_u32(object + 272, motion[2]);
    CopyAnimationAngles(object + 282, object + 16);
    CopyAnimationAngles(object + 16, object + 276);
    SubtractAngleVector((uint16 *)angles, (const uint16 *)psx_addr(object + 282, 6), (const uint16 *)psx_addr(object + 276, 6));
    value = (uint32)angles[0] | ((uint32)angles[1] << 16);
    w_masked_u32(object + 288, value, 15);
    w_masked_u32(object + 288, value, 15);
    w_u16(object + 292, angles[2]);
    for (axis = 0; axis < 3; ++axis)
    {
        step = (uint32)(sint32)(sint16)r_u16(object + 288 + axis * 2);
        value = r_u16(object + 288 + axis * 2);
        if ((sint32)step < -2048)
            w_u16(object + 288 + axis * 2, value + 4096);
        step = (uint32)(sint32)(sint16)r_u16(object + 288 + axis * 2);
        value = r_u16(object + 288 + axis * 2);
        if ((sint32)step >= 2049)
            w_u16(object + 288 + axis * 2, value - 4096);
    }
    count = r_u8(object + 225);
    DivideAngleVector((uint16 *)psx_addr(object + 288, 6), (const uint32 *)&count);
    return object + 288;
}

extern "C" uint32 sub_80062F64(uint32 object)
{
    FUNCTION_MARKER(0x80062F64, "SLUS_003.73");
    return InitializeCharacterBase(object);
}

extern "C" uint32 sub_80062FB8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80062FB8, "SLUS_003.73");
    return DestroyCharacterBase(object, reason);
}

extern "C" uint32 sub_80063038(uint32 object, uint32 animation, uint32 first, uint32 last)
{
    FUNCTION_MARKER(0x80063038, "SLUS_003.73");
    return SetAnimationRange(object, animation, first, last);
}

extern "C" uint32 sub_80063118(uint32 object, uint32 animation, uint32 direction)
{
    FUNCTION_MARKER(0x80063118, "SLUS_003.73");
    return StartAnimation(object, animation, direction);
}

extern "C" uint32 sub_8006325C(uint32 object, uint32 animation, uint32 target_frame)
{
    FUNCTION_MARKER(0x8006325C, "SLUS_003.73");
    return DidAnimationCrossFrame(object, animation, target_frame);
}

extern "C" uint32 sub_80063164(uint32 object)
{
    FUNCTION_MARKER(0x80063164, "SLUS_003.73");
    return AdvanceAnimation(object);
}

extern "C" uint32 sub_8006331C(uint32 object, uint32 model)
{
    FUNCTION_MARKER(0x8006331C, "SLUS_003.73");
    return static_cast<Character *>(FindNativeObjectStorage(object))->UpdateAnimationModel(model);
}

extern "C" uint32 sub_8006338C(uint32 object)
{
    FUNCTION_MARKER(0x8006338C, "SLUS_003.73");
    return UpdateAnimationPose((uint8 *)psx_addr(object, 1));
}
