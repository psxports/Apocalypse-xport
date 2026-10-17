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

#include "angle_math.h"
#include "model_animation.h"
#include "vector_arithmetic.h"

// Shared pose dispatch has no independently established class receiver

static void ReleaseCharacterAnimationBuffers(uint32 object)
{
    FUNCTION_MARKER(0x80062FB8, "SLUS_003.73");
    w_u32(object + 68, 0x800A3340);
    static_cast<Character *>(FindNativeObjectStorage(object))->ReleaseModelAnimation();
}

static uint32 ReadAnimationFrameCount(uint32 object, uint32 animation)
{
    uint32 model = r_u8(object + 27);
    w_u8(object + 26, animation);
    return (uint8)GetModelAnimation(GetModelBank(model).animations, animation & 255).frame_count;
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

Character::Character() : VisualObject(), model_hierarchy(0), model_angles(0), model_poses(0), model_pose_count(0)
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

uint32 Character::UpdateAnimationModel(const ModelHierarchyTable &table)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8006331C, "SLUS_003.73");
    if (!HasModelJointAngles())
    {
        InitializeModelAnimation(table);
    }
    UpdateModelAnimationData(object);
    result = this->model.flags & 4;
    if (result)
    {
        UpdateModelWorldTransform(object);
        return BuildModelPose(*this);
    }
    return result;
}

void VisualObject::UpdateAnimationBehavior()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 frame = r_u8(object + 24);
    w_u16(object + 310, (uint32)(sint32)(sint8)frame);
    Update();
    frame = r_u8(object + 24);
    uint32 animation = r_u8(object + 26);
    uint32 direction = r_u8(object + 297);
    w_u16(object + 308, (uint32)(sint32)(sint8)frame);
    w_u8(object + 312, animation);
    w_u8(object + 313, direction);
}

void VisualObject::UpdateAnimationPose()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, step, count, delta[3], motion[3], axis, value;
    FUNCTION_MARKER(0x8006338C, "SLUS_003.73");
    flags = this->model.flags & 2;
    if (flags)
        AdvanceAnimation(object);
    if (pose_interpolation.count < 2)
    {
        if (flags)
            UpdateAnimationBehavior();
        else
            Update();
        return;
    }
    step = pose_interpolation.step;
    count = pose_interpolation.count;
    step = (step + 1) & 255;
    pose_interpolation.step = (uint8)step;
    if (step < count)
    {
        AddVectors((uint32 *)this->model.pos, pose_interpolation.delta_pos);
        AddAngleVector((uint16 *)this->model.angles, pose_interpolation.delta_angles);
        NormalizeAngles((uint16 *)this->model.angles);
        return;
    }
    pose_interpolation.step = 0;
    for (axis = 0; axis < 3; ++axis)
        this->model.pos[axis] = pose_interpolation.target_pos[axis];
    for (axis = 0; axis < 3; ++axis)
        pos_history.prev[axis] = this->model.pos[axis];
    for (axis = 0; axis < 3; ++axis)
        this->model.angles[axis] = pose_interpolation.target_angles[axis];
    for (axis = 0; axis < 3; ++axis)
        pose_interpolation.prev_angles[axis] = this->model.angles[axis];
    if (this->model.flags & 2)
        UpdateAnimationBehavior();
    else
        Update();
    for (axis = 0; axis < 3; ++axis)
        pose_interpolation.target_pos[axis] = this->model.pos[axis];
    for (axis = 0; axis < 3; ++axis)
        this->model.pos[axis] = pos_history.prev[axis];
    SubtractVectors(delta, pose_interpolation.target_pos, pos_history.prev);
    count = pose_interpolation.count;
    DivideVector(motion, delta, &count);
    for (axis = 0; axis < 3; ++axis)
        pose_interpolation.delta_pos[axis] = motion[axis];
    for (axis = 0; axis < 3; ++axis)
        pose_interpolation.target_angles[axis] = this->model.angles[axis];
    for (axis = 0; axis < 3; ++axis)
        this->model.angles[axis] = pose_interpolation.prev_angles[axis];
    SubtractAngleVector(pose_interpolation.delta_angles, pose_interpolation.target_angles, pose_interpolation.prev_angles);
    for (axis = 0; axis < 3; ++axis)
    {
        step = (uint32)(sint32)(sint16)pose_interpolation.delta_angles[axis];
        value = pose_interpolation.delta_angles[axis];
        if ((sint32)step < -2048)
            pose_interpolation.delta_angles[axis] = (uint16)(value + 4096);
        step = (uint32)(sint32)(sint16)pose_interpolation.delta_angles[axis];
        value = pose_interpolation.delta_angles[axis];
        if ((sint32)step >= 2049)
            pose_interpolation.delta_angles[axis] = (uint16)(value - 4096);
    }
    count = pose_interpolation.count;
    DivideAngleVector(pose_interpolation.delta_angles, (const uint32 *)&count);
    return;
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





const ModelHierarchyTable &Character::GetModelHierarchy() const
{
    if (!model_hierarchy)
    {
        GameDiagnostic("Character has no model hierarchy\n");
        abort();
    }
    return *model_hierarchy;
}

bool Character::HasModelJointAngles() const
{
    return model_angles != 0;
}

ModelJointAngles &Character::GetModelJointAngles(uint32 index)
{
    if (!model_angles || !model_hierarchy || index >= model_hierarchy->count)
    {
        GameDiagnostic("Invalid character joint angle index\n");
        abort();
    }
    return model_angles[index];
}

void Character::ReleaseModelAnimation()
{
    free(model_poses);
    model_poses = 0;
    model_pose_count = 0;
    free(model_angles);
    model_angles = 0;
    model_hierarchy = 0;
}

bool Character::HasModelPoses() const
{
    return model_poses != 0;
}

uint32 Character::GetModelPoseCount() const
{
    return model_pose_count;
}

ModelPose *Character::GetModelPoses(uint32 first, uint32 count)
{
    if (!model_poses || first > model_pose_count || count > model_pose_count - first)
    {
        GameDiagnostic("Invalid character pose span\n");
        abort();
    }
    return model_poses + first;
}
