#include "model_resources.h"
#include "game_sdk_bindings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "angle_math.h"
#include "model_animation.h"
#include "character.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 ReadPoseWord(const uint8 *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u32(address + offset) : xport_load_le32(input + offset);
}

static uint32 ReadPoseHalf(const uint8 *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u16(address + offset) : xport_load_le16(input + offset);
}

static void StorePoseHalf(uint8 *output, uint32 offset, uint16 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u16(address + offset, value);
    else
        xport_store_le16(output + offset, value);
}

static void LoadPoseRotation(const uint8 *matrix)
{
    uint32 words[5], i;
    for (i = 0; i < 5; ++i)
        words[i] = ReadPoseWord(matrix, 4 * i);
    for (i = 0; i < 5; ++i)
        xport_gte_write_control(i, words[i]);
}

static void StorePoseColumn(uint8 *output, uint32 column)
{
    uint32 values[3], row;
    for (row = 0; row < 3; ++row)
        values[row] = xport_gte_read_data(9 + row);
    for (row = 0; row < 3; ++row)
        StorePoseHalf(output, 6 * row + 2 * column, (uint16)values[row]);
}

void ComposePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right)
{
    FUNCTION_MARKER(0x80085674, "SLUS_003.73");
    uint32 words[5], translation[3], i;
    LoadPoseRotation(left);
    for (i = 0; i < 3; ++i)
        translation[i] = (uint32)(sint32)(sint16)ReadPoseHalf(left, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        xport_gte_write_control(5 + i, translation[i]);
    for (i = 0; i < 5; ++i)
        words[i] = ReadPoseWord(right, 4 * i);
    xport_gte_write_data(0, (words[0] & 0xffff) | (words[1] & 0xffff0000));
    xport_gte_write_data(1, words[3]);
    xport_gte_write_data(2, (words[0] >> 16) | (words[2] << 16));
    xport_gte_write_data(3, words[3] >> 16);
    xport_gte_write_data(4, (words[1] & 0xffff) | (words[2] & 0xffff0000));
    xport_gte_write_data(5, words[4]);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 0);
    xport_gte_execute(0x48E012);
    StorePoseColumn(output, 1);
    xport_gte_execute(0x496012);
    StorePoseColumn(output, 2);
    for (i = 0; i < 3; ++i)
        translation[i] = (uint32)(sint32)(sint16)ReadPoseHalf(right, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, translation[i]);
    xport_gte_execute(0x498012);
    for (i = 0; i < 3; ++i)
        words[i] = xport_gte_read_data(9 + i);
    for (i = 0; i < 3; ++i)
        StorePoseHalf(output, 18 + 2 * i, (uint16)words[i]);
}

void RelativePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right)
{
    FUNCTION_MARKER(0x800857A8, "SLUS_003.73");
    uint32 first, second, third, values[3], translation[3], i;
    LoadPoseRotation(left);
    first = ReadPoseWord(right, 0);
    second = ReadPoseWord(right, 4);
    xport_gte_write_data(0, first);
    xport_gte_write_data(1, second);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 0);
    third = ReadPoseWord(right, 8);
    xport_gte_write_data(0, (third << 16) | (second >> 16));
    xport_gte_write_data(1, third >> 16);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 1);
    first = ReadPoseWord(right, 12);
    second = ReadPoseWord(right, 16);
    xport_gte_write_data(0, first);
    xport_gte_write_data(1, second);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 2);
    LoadPoseRotation(output);
    for (i = 0; i < 3; ++i)
        translation[i] = (uint32)(sint32)(sint16)ReadPoseHalf(right, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, translation[i]);
    xport_gte_execute(0x49E012);
    for (i = 0; i < 3; ++i)
        values[i] = (uint32)(sint32)(sint16)ReadPoseHalf(left, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        values[i] -= xport_gte_read_data(9 + i);
    for (i = 0; i < 3; ++i)
        StorePoseHalf(output, 18 + 2 * i, (uint16)values[i]);
}

struct PoseParent
{
    ModelPose pose;
    uint32 ordinal;
};

class PoseHierarchyWorkspace
{
public:
    PoseHierarchyWorkspace(uint32 count)
    {
        if (!count || count > 0xFFFFFFFF / sizeof(PoseParent))
        {
            GameDiagnostic("Invalid pose hierarchy workspace size\n");
            abort();
        }
        values = (PoseParent *)malloc(count * sizeof(PoseParent));
        if (!values)
        {
            GameDiagnostic("Cannot allocate pose hierarchy workspace\n");
            abort();
        }
    }

    ~PoseHierarchyWorkspace() { free(values); }
    PoseParent *Get() { return values; }

private:
    PoseHierarchyWorkspace(const PoseHierarchyWorkspace &);
    PoseHierarchyWorkspace &operator=(const PoseHierarchyWorkspace &);
    PoseParent *values;
};

uint32 BuildModelPose(Character &character)
{
    const uint32 object = NativeObjectGuestAddress(&character);
    FUNCTION_MARKER(0x8007D148, "SLUS_003.73");
    sint32 top = -1;
    uint32 result = character.model.flags & 4;
    if (!result || !character.HasModelPoses())
        return result;
    if (!character.HasModelJointAngles())
        return 0x800F0000;
    const uint32 total = character.GetModelPoseCount();
    ModelPose *output = character.GetModelPoses(0, total);
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(character.model.bank).animations, character.model.animation);
    if (animation.bone_count != total)
    {
        GameDiagnostic("Character pose owner no longer matches its model bank\n");
        abort();
    }
    for (uint32 bone = 0; bone < total; ++bone)
        output[bone].rotation[0][0] = 0x8000;
    const ModelHierarchyTable &hierarchy = character.GetModelHierarchy();
    PoseHierarchyWorkspace workspace(hierarchy.count);
    PoseParent *parents = workspace.Get();
    const ModelPose *source = animation.interval ? GetModelPoseWorkspace(total) : GetModelAnimationPoses(animation, character.model.frame * total, total);
    for (uint32 ordinal = 0; ordinal < hierarchy.count; ++ordinal)
    {
        const ModelHierarchyLink &link = hierarchy.links[ordinal];
        const uint32 joint = link.bone;
        if (top >= 0)
            while (top >= 0 && link.parent < parents[top].ordinal)
                --top;
        const ModelPose &base = source[joint];
        const ModelJointAngles &angles = character.GetModelJointAngles(ordinal);
        if (angles.rotation[0] || angles.rotation[1] || angles.rotation[2])
        {
            ModelPose matrix;
            BuildModelRotationMatrix(angles.rotation, (uint16 *)&matrix);
            for (uint32 axis = 0; axis < 3; ++axis)
                matrix.translation[axis] = 0;
            ComposePoseMatrix((uint8 *)&matrix, (const uint8 *)&base, (const uint8 *)&matrix);
            if (top >= 0)
                ComposePoseMatrix((uint8 *)&matrix, (const uint8 *)&parents[top].pose, (const uint8 *)&matrix);
            ++top;
            RelativePoseMatrix((uint8 *)&parents[top].pose, (const uint8 *)&matrix, (const uint8 *)&base);
            parents[top].ordinal = ordinal;
            output[joint] = matrix;
        }
        else if (top < 0)
            output[joint] = base;
        else
            ComposePoseMatrix((uint8 *)(output + joint), (const uint8 *)&parents[top].pose, (const uint8 *)&base);
    }
    result = animation.interval;
    if (!result)
        for (uint32 bone = 0; bone < total; ++bone)
        {
            if ((sint16)output[bone].rotation[0][0] == -32768)
                output[bone] = source[bone];
            result = bone + 1 < total;
        }
    return result;
}

uint32 UpdateModelWorldTransform(uint32 object)
{
    FUNCTION_MARKER(0x8007FDF0, "SLUS_003.73");
    uint32 x, z, y;
    const ModelInstance &model = VisualObject::Attach(object).model;
    BuildModelRotationMatrix(model.angles, (uint16 *)psx_addr(object + 324, 18));
    if (model.flags & 0x200)
        ScaleModelRotationMatrix(model.scale, (uint16 *)psx_addr(object + 324, 18));
    x = model.pos[0];
    z = model.pos[2];
    w_u32(object + 344, (uint32)((sint32)x >> 12));
    y = model.pos[1];
    w_u32(object + 352, (uint32)((sint32)z >> 12));
    y = (uint32)((sint32)y >> 12);
    w_u32(object + 348, y);
    return y;
}

extern "C" uint32 sub_8007FDF0(uint32 object)
{
    FUNCTION_MARKER(0x8007FDF0, "SLUS_003.73");
    return UpdateModelWorldTransform(object);
}




