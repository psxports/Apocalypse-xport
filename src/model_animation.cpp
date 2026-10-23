#include <stdlib.h>

#include "psx.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "model_animation.h"
#include "global.h"
#include "character.h"
#include "angle_math.h"

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
