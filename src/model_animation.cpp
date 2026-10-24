#include <stdlib.h>

#include "psx.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "model_animation.h"
#include "global.h"
#include "character.h"
#include "angle_math.h"
#include "hd.h"

void InterpolateModelAnimation(const ModelAnimation &animation, uint8 frame, bool loop, uint32 first, uint32 count)
{
    FUNCTION_MARKER(0x8007CF38, "SLUS_003.73");
    sint32 interval = animation.interval + 1;
    sint32 lower = frame - frame % interval;
    sint32 upper = lower + interval;
    sint32 limit = animation.frame_count;
    sint32 factor = 0;
    if (upper < limit)
        factor = DivideSignedWord((frame - lower) * ONE, upper - lower);
    else if (loop)
    {
        upper = 0;
        factor = DivideSignedWord((frame - lower) * ONE, limit - lower);
    }
    else
    {
        lower -= interval;
        if (lower < 0)
            lower = 0;
        upper -= interval;
        if (upper)
            factor = DivideSignedWord((frame - lower) * ONE, upper - lower);
    }
    if (first > animation.bone_count || count > animation.bone_count - first)
    {
        GameDiagnostic(("Invalid interpolated model bone span\n"));
        abort();
    }
    ModelPose *output = GetModelPoseWorkspace(animation.bone_count) + first;
    const ModelPose *source0 = GetModelAnimationPoses(animation, DivideSignedWord(animation.bone_count * lower, interval) + first, count);
    const ModelPose *source1 = GetModelAnimationPoses(animation, DivideSignedWord(animation.bone_count * upper, interval) + first, count);
    for (uint32 bone = 0; bone < count; ++bone)
        for (uint32 vector = 0; vector < 4; ++vector)
        {
            const ShortVector3 &left = vector < 3 ? source0[bone].rotation[vector] : source0[bone].translation;
            const ShortVector3 &right = vector < 3 ? source1[bone].rotation[vector] : source1[bone].translation;
            ShortVector3 &dst = vector < 3 ? output[bone].rotation[vector] : output[bone].translation;
            ShortVector3 difference;
            ShortVector3 interpolated;
            difference.SubtractAngles(right, left);
            for (uint32 axis = 0; axis < 3; ++axis)
                xport_gte_write_data(25 + axis, left.comp[axis]);
            for (uint32 axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9 + axis, difference.comp[axis]);
            xport_gte_write_data(8, factor);
            xport_gte_execute(0x1A8003E);
            for (uint32 axis = 0; axis < 3; ++axis)
                interpolated.comp[axis] = xport_gte_read_data(25 + axis);
            dst = interpolated;
        }
}

void ModelAnimation::GetHdFramePair(float frame, bool loop, const ModelPose *&a, const ModelPose *&b, float &factor) const
{
    sint32 step = interval + 1;
    sint32 whole_frame = (sint32)frame;
    sint32 lower = whole_frame - whole_frame % step;
    sint32 upper = lower + step;
    factor = 0;
    if (upper < frame_count)
        factor = (float)(frame - lower) / (upper - lower);
    else if (loop)
    {
        upper = 0;
        factor = (float)(frame - lower) / (frame_count - lower);
    }
    else
    {
        if (!interval)
        {
            upper = lower;
        }
        else
        {
            lower -= step;
            if (lower < 0)
                lower = 0;
            upper -= step;
        }
        if (upper > lower)
            factor = (float)(frame - lower) / (upper - lower);
    }
    a = GetModelAnimationPoses(*this, (lower / step) * bone_count, bone_count);
    b = factor == 0 ? a : GetModelAnimationPoses(*this, (upper / step) * bone_count, bone_count);
}

void ModelAnimation::CaptureHd(float frame, bool loop, MATRIXF *output, uint32 count) const
{
    if (count != bone_count)
    {
        GameDiagnostic(("HD animation pose count does not match its owner\n"));
        abort();
    }
    const ModelPose *a, *b;
    float factor;
    GetHdFramePair(frame, loop, a, b, factor);
    for (uint32 bone = 0; bone < count; ++bone)
    {
        bool interpolate_rotation = factor > 0 && factor < 1;
        if (interpolate_rotation)
        {
            MATRIXF left, right;
            HD_MATRIX_HISTORY history;
            a[bone].CaptureHd(left);
            b[bone].CaptureHd(right);
            hd_matrix_seed(&history, &left);
            hd_matrix_capture(&history, &right);
            hd_matrix_evaluate(&history, factor, output + bone);
        }
        else
            IdentityMatrixf(output + bone);
        for (uint32 row = 0; row < 4; ++row)
        {
            if (row < 3 && interpolate_rotation)
                continue;
            const ShortVector3 &left = row < 3 ? a[bone].rotation[row] : a[bone].translation;
            const ShortVector3 &right = row < 3 ? b[bone].rotation[row] : b[bone].translation;
            for (uint32 column = 0; column < 3; ++column)
            {
                // Key deltas retain the original signed halfword wrap
                sint16 delta = right.comp[column] - left.comp[column];
                float value = left.comp[column] + delta * factor;
                while (value < -32768)
                    value += 65536;
                while (value >= 32768)
                    value -= 65536;
                if (row < 3)
                    output[bone].m[row][column] = value / 4096;
                else
                    output[bone].t[column] = value;
            }
        }
    }
}

void ModelAnimation::CaptureHdHierarchy(float frame, bool loop, MATRIXF *output, uint32 count, const ModelHierarchyTable &hierarchy) const
{
    const ModelPose *a, *b;
    float factor;
    CaptureHd(frame, loop, output, count);
    GetHdFramePair(frame, loop, a, b, factor);
    if (factor <= 0 || factor >= 1)
        return;
    for (uint32 pass = 0; pass < 2; ++pass)
        for (uint32 index = 0; index < hierarchy.count; ++index)
        {
            const ModelHierarchyLink &link = hierarchy.links[index];
            if (link.parent == 65535)
                continue;
            uint32 parent = hierarchy.links[link.parent].bone;
            MATRIXF parent_a, parent_b, child_a, child_b, local_a, local_b;
            a[parent].CaptureHd(parent_a);
            b[parent].CaptureHd(parent_b);
            a[link.bone].CaptureHd(child_a);
            b[link.bone].CaptureHd(child_b);
            /* Check the complete pair before replacing any model-space result */
            if (!hd_matrix_relative(&parent_a, &child_a, &local_a) || !hd_matrix_relative(&parent_b, &child_b, &local_b))
                return;
            if (pass)
            {
                HD_MATRIX_HISTORY history;
                MATRIXF local;
                hd_matrix_seed(&history, &local_a);
                hd_matrix_capture(&history, &local_b);
                hd_matrix_evaluate(&history, factor, &local);
                CompMatrixf(output + parent, &local, output + link.bone);
            }
        }
}
