#include <string.h>
#include <stdlib.h>

#include "game_diagnostics.h"
#include "character.h"
#include "model_bank.h"
#include "visual_object.h"
#include "psx.h"
#include "model_animation.h"
#include "global.h"
#include "angle_math.h"
#include "model_instance.h"
#include "model_attachment.h"
#include "model_render_view.h"
#include "model_matrix.h"
#include "vector3.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_lighting.h"
#include "resource_script.h"
#include "effect_lifecycle.h"
#include "model_render.h"
#include "model_subdivision.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_clipping.h"
#include "effect_object.h"
#include "texture_resources.h"

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
            GameDiagnostic(("Invalid pose hierarchy workspace size\n"));
            abort();
        }
        values = (PoseParent *)malloc(count * sizeof(PoseParent));
        if (!values)
        {
            GameDiagnostic(("Cannot allocate pose hierarchy workspace\n"));
            abort();
        }
    }

    ~PoseHierarchyWorkspace()
    {
        free(values);
    }

    PoseParent *Get()
    {
        return values;
    }

private:
    PoseHierarchyWorkspace(const PoseHierarchyWorkspace &);
    PoseHierarchyWorkspace &operator=(const PoseHierarchyWorkspace &);
    PoseParent *values;
};

static void MultiplyPosedModelMatrix(const MATRIX &left, const MATRIX &right, MATRIX &output);
static void MultiplyPosedModelMatrix(const MATRIX &left, const ModelPose &right, MATRIX &output);

uint8 Character::ReadAnimationFrameCount(uint8 animation)
{
    uint8 bank = model.bank;
    model.animation = animation;
    return GetModelAnimation(ModelBank::Get(bank).animations, animation).frame_count;
}

uint32 Character::GetAttachment(Vector3 &output, uint8 requested)
{
    FUNCTION_MARKER(0x8005CEE0, "SLUS_003.73");
    return GetModelAttachment(output, *this, requested);
}

Character::Character() : VisualObject(), hidden_models(0), model_hierarchy(0), model_angles(0), model_poses(0), model_pose_count(0)
{
    FUNCTION_MARKER(0x80062F64, "SLUS_003.73");
    projection.enabled = false;
    projection.dir.x = projection.dir.y = projection.dir.z = 0;
    projection.color = 0;
    memset(&world_transform, 0, sizeof(world_transform));
    playback.speed = 65536;
    playback.frame_count = 1;
    playback.complete = 1;
    model.flags |= 2;
}

uint32 Character::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80062FB8, "SLUS_003.73");
    ReleaseModelAnimation();
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 Character::SetAnimationRange(uint8 animation, sint32 first, sint32 last)
{
    uint8 count;
    FUNCTION_MARKER(0x80063038, "SLUS_003.73");
    count = ReadAnimationFrameCount(animation);
    playback.frame_count = count;
    if (first == -1)
        first = count - 1;
    if (last == -1)
        last = playback.frame_count - 1;
    if (first < 0 || first >= playback.frame_count)
        first = 0;
    if (last < 0 || last >= playback.frame_count)
        last = 0;
    playback.loop_mode = 0;
    if (first < last)
        playback.dir = 1;
    else if (last < first)
        playback.dir = -1;
    else
        playback.dir = 0;
    model.frame = first;
    sint8 cur_frame = model.frame;
    playback.last_frame = last;
    playback.phase = 0;
    bool result = cur_frame == playback.last_frame;
    playback.complete = result;
    return result;
}

uint32 Character::StartAnimation(uint8 animation, sint8 dir)
{
    uint8 count;
    FUNCTION_MARKER(0x80063118, "SLUS_003.73");
    count = ReadAnimationFrameCount(animation);
    playback.loop_mode = 1;
    playback.dir = dir;
    model.frame = 0;
    playback.phase = 0;
    playback.complete = 0;
    playback.frame_count = count;
    return 1;
}

uint32 Character::AdvanceAnimation()
{
    FUNCTION_MARKER(0x80063164, "SLUS_003.73");
    sint8 frame = model.frame;
    uint32 phase = frame;
    // Combine the signed frame byte with its Q16 fraction
    phase = (phase << 16) | playback.phase;
    sint8 dir = playback.dir;
    uint32 speed = playback.speed;
    if (dir == 1)
        phase += speed;
    if (dir == -1)
        phase -= speed;
    uint8 mode = playback.loop_mode;
    sint32 result = phase;
    result >>= 16;
    model.frame = result;
    playback.phase = phase;
    if (mode)
    {
        if (mode != 1)
            return result;
        frame = model.frame;
        if (frame >= playback.frame_count)
            model.frame = 0;
        frame = model.frame;
        if (frame < 0)
        {
            result = playback.frame_count - 1;
            model.frame = result;
            return result;
        }
        return frame;
    }
    dir = playback.dir;
    if (dir == 1)
    {
        frame = model.frame;
        result = frame < playback.last_frame;
        if (!result)
            goto Complete;
    }
    if (dir == -1)
    {
        frame = model.frame;
        result = playback.last_frame < frame;
        if (!result)
            goto Complete;
    }
    return result;
Complete:
    playback.complete = 1;
    model.frame = playback.last_frame;
    return 1;
}

bool Character::DidAnimationCrossFrame(uint32 animation, sint32 target_frame)
{
    sint32 cur, prev, dir;
    FUNCTION_MARKER(0x8006325C, "SLUS_003.73");
    if (animation != playback.prev_animation)
        return 0;
    cur = playback.cur_frame;
    prev = playback.prev_frame;
    dir = playback.prev_dir;
    if (prev < cur)
    {
        if (dir < 0)
            return target_frame >= prev && cur >= target_frame;
        return prev >= target_frame || target_frame >= cur;
    }
    if (dir < 0)
        return cur >= target_frame || target_frame >= prev;
    return target_frame >= cur && target_frame <= prev;
}

const ModelHierarchyTable &Character::GetModelHierarchy() const
{
    if (!model_hierarchy)
    {
        GameDiagnostic(("Character has no model hierarchy\n"));
        abort();
    }
    return *model_hierarchy;
}

bool Character::HasModelJointAngles() const
{
    return model_angles != 0;
}

ModelJointAngles &Character::GetModelJointAngles(uint32 idx)
{
    if (!model_angles || !model_hierarchy || idx >= model_hierarchy->count)
    {
        GameDiagnostic(("Invalid character joint angle index\n"));
        abort();
    }
    return model_angles[idx];
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
        GameDiagnostic(("Invalid character pose span\n"));
        abort();
    }
    return model_poses + first;
}

uint32 Character::UpdateAnimationModel(const ModelHierarchyTable &table)
{
    FUNCTION_MARKER(0x8006331C, "SLUS_003.73");
    if (!HasModelJointAngles())
        InitializeModelAnimation(table);
    UpdateModelAnimationData();
    if (model.flags & 4)
    {
        UpdateWorldTransform();
        return BuildModelPose();
    }
    return 0;
}

void Character::InitializeModelAnimation(const ModelHierarchyTable &table)
{
    FUNCTION_MARKER(0x8007CAC8, "SLUS_003.73");
    uint8 bank = model.bank;
    uint32 bones = ModelBank::Get(bank).geometry.count;
    for (uint32 row = 0; row < table.count; ++row)
        if (table.links[row].bone >= bones || (table.links[row].parent != 65535 && table.links[row].parent >= table.count))
        {
            GameDiagnostic(("Invalid character model hierarchy\n"));
            abort();
        }
    if (!bones || bones > 0xFFFFFFFF / sizeof(ModelPose) || !table.count)
    {
        GameDiagnostic(("Invalid character animation dimensions\n"));
        abort();
    }
    ModelPose *poses = (ModelPose *)malloc(bones * sizeof(ModelPose));
    ModelJointAngles *angles = (ModelJointAngles *)malloc(table.count * sizeof(ModelJointAngles));
    if (!poses || !angles)
    {
        free(poses);
        free(angles);
        GameDiagnostic(("Cannot allocate character model animation\n"));
        abort();
    }
    for (uint32 row = 0; row < table.count; ++row)
        angles[row] = ModelJointAngles();
    const ModelAnimation &animation = GetModelAnimation(ModelBank::Get(bank).animations, model.animation);
    if (animation.bone_count != bones)
    {
        free(poses);
        free(angles);
        GameDiagnostic(("Character animation bone count mismatch\n"));
        abort();
    }
    // Prepare the current zero-angle pose before attachment queries can read it
    UpdateModelAnimationData();
    const ModelPose *src = animation.interval ? GetModelPoseWorkspace(bones) : GetModelAnimationPoses(animation, model.frame * bones, bones);
    for (uint32 bone = 0; bone < bones; ++bone)
        poses[bone] = src[bone];
    ReleaseModelAnimation();
    model_hierarchy = &table;
    model_angles = angles;
    model_poses = poses;
    model_pose_count = bones;
}

void Character::UpdateModelAnimationData()
{
    FUNCTION_MARKER(0x8007CECC, "SLUS_003.73");
    const ModelAnimation &animation = GetModelAnimation(ModelBank::Get(model.bank).animations, model.animation);
    if (animation.interval)
        InterpolateModelAnimation(animation, model.frame, playback.loop_mode == 1, 0, animation.bone_count);
}

uint32 Character::BuildModelPose()
{
    FUNCTION_MARKER(0x8007D148, "SLUS_003.73");
    sint32 top = -1;
    uint32 result = model.flags & 4;
    if (!result || !HasModelPoses())
        return result;
    if (!HasModelJointAngles())
        return 0x800F0000;
    const uint32 total = GetModelPoseCount();
    ModelPose *output = GetModelPoses(0, total);
    const ModelAnimation &animation = GetModelAnimation(ModelBank::Get(model.bank).animations, model.animation);
    if (animation.bone_count != total)
    {
        GameDiagnostic(("Character pose owner no longer matches its model bank\n"));
        abort();
    }
    for (uint32 bone = 0; bone < total; ++bone)
        output[bone].rotation[0].x = -32768;
    const ModelHierarchyTable &hierarchy = GetModelHierarchy();
    PoseHierarchyWorkspace workspace(hierarchy.count);
    PoseParent *parents = workspace.Get();
    const ModelPose *src = animation.interval ? GetModelPoseWorkspace(total) : GetModelAnimationPoses(animation, model.frame * total, total);
    for (uint32 ordinal = 0; ordinal < hierarchy.count; ++ordinal)
    {
        const ModelHierarchyLink &link = hierarchy.links[ordinal];
        uint16 joint = link.bone;
        ModelPose &pose = output[joint];
        while (top >= 0 && link.parent < parents[top].ordinal)
            --top;
        const ModelPose &base = src[joint];
        const ModelJointAngles &angles = GetModelJointAngles(ordinal);
        if ((uint16)angles.rotation.x || (uint16)angles.rotation.y || (uint16)angles.rotation.z)
        {
            MATRIX joint_rotation;
            angles.rotation.BuildRotationMatrix(joint_rotation);
            ModelPose matrix;
            for (uint32 row = 0; row < 3; ++row)
                for (uint32 col = 0; col < 3; ++col)
                    matrix.rotation[row].comp[col] = joint_rotation.m[row][col];
            for (uint32 axis = 0; axis < 3; ++axis)
                matrix.translation.comp[axis] = 0;
            matrix.Compose(base, matrix);
            if (top >= 0)
                matrix.Compose(parents[top].pose, matrix);
            ++top;
            parents[top].pose.SetRelative(matrix, base);
            parents[top].ordinal = ordinal;
            pose = matrix;
        }
        else if (top < 0)
            pose = base;
        else
            pose.Compose(parents[top].pose, base);
    }
    result = animation.interval;
    if (!result)
        for (uint32 bone = 0; bone < total; ++bone)
        {
            if (output[bone].rotation[0].x == -32768)
                output[bone] = src[bone];
            result = bone + 1 < total;
        }
    return result;
}

uint32 Character::RenderModel()
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007F340, "SLUS_003.73");
    ModelRenderView *camera;
    uint16 flags = model.flags;
    uint32 count;
    const ModelPose *pose;
    uint32 idx, rgb = 0;
    uint8 mode;
    uint16 scale = 0;
    sint16 depth_bias = 0;
    const TextureGroup *animation = 0;
    sint32 animation_frame = 0;
    MATRIX rotation, light, matrix;
    Vector3 t, relative;
    uint32 axis;
    camera = &GetConfiguredModelRenderView();
    model_render_state.use_scratch = false;
    SetRotMatrix(&camera->projection);
    TransformModelMatrix(world_transform, rotation);
    camera = &GetConfiguredModelRenderView();
    for (axis = 0; axis < 3; ++axis)
        relative.comp[axis] = (uint32)(model.pos.comp[axis] >> 12) - (uint32)camera->pos.comp[axis];
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, (uint32)relative.comp[axis]);
    xport_gte_execute(0x49E012);
    const ModelGeometrySet &models = ModelBank::Get(this->model.bank).geometry;
    count = models.count;
    for (axis = 0; axis < 3; ++axis)
    {
        t.comp[axis] = xport_gte_read_data(25 + axis);
        rotation.t[axis] = t.comp[axis];
    }
    if (flags & 4)
        pose = GetModelPoses(0, count);
    else
    {
        const ModelAnimation &animation_data = GetModelAnimation(ModelBank::Get(this->model.bank).animations, this->model.animation);
        if (animation_data.interval)
        {
            UpdateModelAnimationData();
            pose = GetModelPoseWorkspace(count);
        }
        else
            pose = GetModelAnimationPoses(animation_data, this->model.frame * count, count);
    }
    if (flags & 0x80)
    {
        SetColorMatrix(&game_lighting.colors);
        SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
        SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
        MultiplyPosedModelMatrix(game_lighting.directions, world_transform, light);
    }
    else
    {
        // Use the approved zero light matrix
        memset(&light, 0, sizeof(light));
    }
    mode = model_effect.color_mode >> 24;
    if (mode & 0x7F)
    {
        uint32 animation_idx = (mode - 1) & 0x7F;
        if (animation_idx >= 15)
            animation_idx = 14;
        animation = EffectObject::GetResourceGrp(animation_idx);
        if (!animation)
        {
            GameDiagnostic(("Missing model effect texture group\n"));
            abort();
        }
        if (model_render_state.effect_frame != game_clock.active_frame)
        {
            model_render_state.effect_pass = model_render_state.pass_count;
            model_render_state.effect_frame = game_clock.active_frame;
        }
        sint32 dividend = model_render_state.effect_pass;
        sint32 divisor = animation->count;
        animation_frame = divisor == 0 ? dividend : (dividend == (-2147483647 - 1) && divisor == -1 ? 0 : dividend % divisor);
        scale = model_effect.scale;
        depth_bias = model_effect.depth_bias;
        rgb = model_effect.color_mode & 0xFFFFFF;
    }
    if (model_render_state.use_scratch)
        model_render_state.scratch_cache_idx = model_scratch_vertex_count - 1;
    else
        model_render_state.cache_index = model_projection_vertex_count - 1;
    SelectModelPal(ModelBank::Get(this->model.bank).colors);
    for (idx = 0; idx < count; ++idx, ++pose)
    {
        model_render_state.part_hidden = (hidden_models & ((uint32)1 << (idx & 31))) != 0;
        const ModelGeometry &model = models.models[idx];
        SetupModelGteMatrices(rotation, light, *pose);
        if (model.flags & 1)
        {
            MultiplyPosedModelMatrix(world_transform, *pose, matrix);
            GenerateModelReflectionCoords(matrix, &t, model);
        }
        else if (flags & 128)
        {
            uint32 normal_count = model.n_count == model.poly_count ? model.n_count : model.vertex_count;
            GenerateModelNormalColors(model.normals, normal_count);
        }
        if (!(mode & 0x80))
        {
            if (model_render_state.use_scratch)
                model.RenderScratch();
            else
                model.RenderTranslated();
        }
        if (model_render_state.part_hidden)
            continue;
        bool effects = projection.enabled;
        if ((mode & 0x7F) || effects)
        {
            MultiplyPosedModelMatrix(world_transform, *pose, matrix);
            ShortVector3 local_translation = pose->translation;
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9 + axis, (uint16)local_translation.comp[axis]);
            LoadModelRotationAndTransformIr(world_transform);
            Vector3 translation;
            for (axis = 0; axis < 3; ++axis)
                translation.comp[axis] = xport_gte_read_data(25 + axis);
            for (axis = 0; axis < 3; ++axis)
                matrix.t[axis] = (uint32)translation.comp[axis] + (uint32)(this->model.pos.comp[axis] >> 12);
        }
        if ((mode & 0x7F) && (!model_render_state.water_clip_enabled || matrix.t[1] < (level_environment.water_y >> 12) - 40))
            model.RenderShadow(matrix, rgb, scale, depth_bias, *TextureGroup::GetFrame(animation, animation_frame).tex);
        if (effects)
        {
            model.RenderProjectionEffect(matrix, surface.n, &surface.pos, projection.dir, projection.color);
        }
    }
    return 0;
}

static void MultiplyPosedModelMatrix(const MATRIX &left, const MATRIX &right, MATRIX &output)
{
    MATRIX local = {0};
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            local.m[row][col] = right.m[row][col];
    MulMatrix0((MATRIX *)&left, &local, &output);
}

static void MultiplyPosedModelMatrix(const MATRIX &left, const ModelPose &right, MATRIX &output)
{
    MATRIX local = {0};
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            local.m[row][col] = right.rotation[row].comp[col];
    MulMatrix0((MATRIX *)&left, &local, &output);
}

uint32 Character::UpdateWorldTransform()
{
    FUNCTION_MARKER(0x8007FDF0, "SLUS_003.73");
    model.angles.BuildRotationMatrix(world_transform);
    if (model.flags & 0x200)
        model.ScaleRotationMatrix(world_transform);
    world_transform.t[0] = model.pos.x >> 12;
    world_transform.t[2] = model.pos.z >> 12;
    world_transform.t[1] = model.pos.y >> 12;
    return world_transform.t[1];
}
