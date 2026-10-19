#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"

#include "character.h"
#include "object_damage.h"
#include "game_heap.h"
#include "model_resources.h"
#include "game_object_allocation.h"

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

uint32 Character::ReadAnimationFrameCount(uint32 animation)
{
    uint32 bank = model.bank;
    model.animation = animation;
    return (uint8)GetModelAnimation(GetModelBank(bank).animations, animation & 255).frame_count;
}

uint32 Character::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80062FB8, "SLUS_003.73");
    ReleaseModelAnimation();
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

Character::Character() : VisualObject(), hidden_models(0), model_hierarchy(0), model_angles(0), model_poses(0), model_pose_count(0)
{
    uint32 flags;
    FUNCTION_MARKER(0x80062F64, "SLUS_003.73");
    projection.enabled = false;
    // The original zeroed block supplies the first row direction and zero remaining rows
    memset(&projection.dir, 0, sizeof(projection.dir));
    projection.color = 0;
    memset(&world_transform, 0, sizeof(world_transform));
    this->playback.speed = 65536;
    flags = this->model.flags;
    this->playback.frame_count = 1;
    this->playback.complete = 1;
    this->model.flags = flags | 2;
}

uint32 Character::SetAnimationRange(uint32 animation, uint32 first, uint32 last)
{
    uint32 count, result;
    sint32 current, final_frame;
    FUNCTION_MARKER(0x80063038, "SLUS_003.73");
    count = ReadAnimationFrameCount(animation);
    this->playback.frame_count = count;
    if (first == (uint32)-1)
        first = count - 1;
    if (last == (uint32)-1)
        last = this->playback.frame_count - 1;
    if ((sint32)first < 0 || (sint32)first >= (sint32)this->playback.frame_count)
        first = 0;
    if ((sint32)last < 0 || (sint32)last >= (sint32)this->playback.frame_count)
        last = 0;
    this->playback.loop_mode = 0;
    if ((sint32)first < (sint32)last)
        this->playback.dir = 1;
    else if ((sint32)last < (sint32)first)
        this->playback.dir = -1;
    else
        this->playback.dir = 0;
    this->model.frame = first;
    current = (sint8)this->model.frame;
    this->playback.last_frame = (sint8)(uint8)(last);
    final_frame = this->playback.last_frame;
    this->playback.phase = 0;
    result = current == final_frame;
    this->playback.complete = result;
    return result;
}

uint32 Character::StartAnimation(uint32 animation, uint32 direction)
{
    uint32 count;
    FUNCTION_MARKER(0x80063118, "SLUS_003.73");
    count = ReadAnimationFrameCount(animation);
    this->playback.loop_mode = 1;
    this->playback.dir = (sint8)(uint8)(direction);
    this->model.frame = 0;
    this->playback.phase = 0;
    this->playback.complete = 0;
    this->playback.frame_count = count;
    return 1;
}

uint32 Character::AdvanceAnimation()
{
    uint32 phase, fraction, speed, mode, count, result;
    sint32 frame, direction, last;
    FUNCTION_MARKER(0x80063164, "SLUS_003.73");
    frame = (sint8)this->model.frame;
    fraction = this->playback.phase;
    direction = this->playback.dir;
    phase = ((uint32)frame << 16) | fraction;
    speed = this->playback.speed;
    if (direction == 1)
        phase += speed;
    if (direction == -1)
        phase -= speed;
    mode = this->playback.loop_mode;
    result = (uint32)((sint32)phase >> 16);
    this->model.frame = result;
    this->playback.phase = phase;
    if (mode)
    {
        if (mode != 1)
            return result;
        frame = (sint8)this->model.frame;
        count = this->playback.frame_count;
        if (frame >= (sint32)count)
            this->model.frame = 0;
        result = (uint32)(sint32)(sint8)this->model.frame;
        if ((sint32)result < 0)
        {
            result = this->playback.frame_count - 1;
            this->model.frame = result;
        }
        return result;
    }
    direction = this->playback.dir;
    if (direction == 1)
    {
        frame = (sint8)this->model.frame;
        last = this->playback.last_frame;
        result = frame < last;
        if (!result)
            goto Complete;
    }
    if (direction == -1)
    {
        frame = (sint8)this->model.frame;
        last = this->playback.last_frame;
        result = last < frame;
        if (!result)
            goto Complete;
    }
    return result;
Complete:
    count = (uint8)this->playback.last_frame;
    this->playback.complete = 1;
    this->model.frame = count;
    return 1;
}

uint32 Character::DidAnimationCrossFrame(uint32 animation, uint32 target_frame)
{
    sint32 current, previous, frame, direction;
    FUNCTION_MARKER(0x8006325C, "SLUS_003.73");
    if (animation != this->playback.prev_animation)
        return 0;
    current = this->playback.cur_frame;
    previous = this->playback.prev_frame;
    frame = (sint32)target_frame;
    direction = this->playback.prev_dir;
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
    uint32 result;
    FUNCTION_MARKER(0x8006331C, "SLUS_003.73");
    if (!HasModelJointAngles())
    {
        InitializeModelAnimation(table);
    }
    UpdateModelAnimationData();
    result = this->model.flags & 4;
    if (result)
    {
        UpdateWorldTransform();
        return BuildModelPose(*this);
    }
    return result;
}

void VisualObject::UpdateAnimationBehavior()
{
    Character &character = *static_cast<Character *>(this);
    uint32 frame = model.frame;
    character.playback.prev_frame = (sint8)frame;
    Update();
    frame = model.frame;
    uint32 animation = model.animation;
    uint32 direction = (uint8)character.playback.dir;
    character.playback.cur_frame = (sint8)frame;
    character.playback.prev_animation = animation;
    character.playback.prev_dir = (sint8)(uint8)direction;
}

void VisualObject::UpdateAnimationPose()
{
    uint32 flags, step, count, axis, value;
    Vector3 delta, motion;
    FUNCTION_MARKER(0x8006338C, "SLUS_003.73");
    flags = this->model.flags & 2;
    if (flags)
        static_cast<Character *>(this)->AdvanceAnimation();
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
        AddVectors(this->model.pos.words, pose_interpolation.delta_pos.words);
        AddAngleVector(this->model.angles.words, pose_interpolation.delta_angles.words);
        NormalizeAngles(this->model.angles.words);
        return;
    }
    pose_interpolation.step = 0;
    this->model.pos = pose_interpolation.target_pos;
    pos_history.prev = this->model.pos;
    this->model.angles = pose_interpolation.target_angles;
    pose_interpolation.prev_angles = this->model.angles;
    if (this->model.flags & 2)
        UpdateAnimationBehavior();
    else
        Update();
    pose_interpolation.target_pos = this->model.pos;
    this->model.pos = pos_history.prev;
    SubtractVectors(delta.words, pose_interpolation.target_pos.words, pos_history.prev.words);
    count = pose_interpolation.count;
    DivideVector(motion.words, delta.words, count);
    pose_interpolation.delta_pos = motion;
    pose_interpolation.target_angles = this->model.angles;
    this->model.angles = pose_interpolation.prev_angles;
    SubtractAngleVector(pose_interpolation.delta_angles.words, pose_interpolation.target_angles.words, pose_interpolation.prev_angles.words);
    for (axis = 0; axis < 3; ++axis)
    {
        step = (uint32)(sint32)(sint16)pose_interpolation.delta_angles.words[axis];
        value = pose_interpolation.delta_angles.words[axis];
        if ((sint32)step < -2048)
            pose_interpolation.delta_angles.words[axis] = (uint16)(value + 4096);
        step = (uint32)(sint32)(sint16)pose_interpolation.delta_angles.words[axis];
        value = pose_interpolation.delta_angles.words[axis];
        if ((sint32)step >= 2049)
            pose_interpolation.delta_angles.words[axis] = (uint16)(value - 4096);
    }
    count = pose_interpolation.count;
    DivideAngleVector(pose_interpolation.delta_angles.words, count);
    return;
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
