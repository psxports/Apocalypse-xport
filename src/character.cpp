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
#include "hd.h"

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

struct HD_POSE_WORKSPACE
{
    struct Parent
    {
        MATRIXF pose;
        uint32 ordinal;
    };

    MATRIXF *base;
    Parent *parents;
    HD_POSE_LINK *links;
    uint32 bone_capacity;
    uint32 parent_capacity;

    HD_POSE_WORKSPACE() : base(0), parents(0), links(0), bone_capacity(0), parent_capacity(0)
    {
    }

    ~HD_POSE_WORKSPACE()
    {
        free(base);
        free(parents);
        free(links);
    }

    void Reserve(uint32 bones, uint32 links)
    {
        if (bones > bone_capacity)
        {
            free(base);
            base = (MATRIXF *)malloc(bones * sizeof(MATRIXF));
            bone_capacity = bones;
        }
        if (links > parent_capacity)
        {
            free(parents);
            parents = (Parent *)malloc(links * sizeof(Parent));
            free(this->links);
            this->links = (HD_POSE_LINK *)malloc(links * sizeof(HD_POSE_LINK));
            parent_capacity = links;
        }
        if (!base || !parents || !this->links)
        {
            GameDiagnostic(("Cannot allocate HD pose hierarchy workspace\n"));
            abort();
        }
    }

    void Apply(const ModelHierarchyTable &hierarchy, const ModelJointAngles *angles, MATRIXF *output)
    {
        sint32 top = -1;
        for (uint32 ordinal = 0; ordinal < hierarchy.count; ++ordinal)
        {
            const ModelHierarchyLink &link = hierarchy.links[ordinal];
            while (top >= 0 && link.parent < parents[top].ordinal)
                --top;
            const MATRIXF &source = base[link.bone];
            const ShortVector3 &rotation = angles[ordinal].rotation;
            MATRIXF &pose = output[link.bone];
            if (rotation.x || rotation.y || rotation.z)
            {
                MATRIXF correction, inverse;
                rotation.BuildRotationMatrixf(correction);
                CompMatrixf(&source, &correction, &pose);
                if (top >= 0)
                    CompMatrixf(&parents[top].pose, &pose, &pose);
                ++top;
                // Source poses are model-space; descendants inherit the relative correction
                IdentityMatrixf(&inverse);
                TransposeMatrixf(&source, &inverse);
                MulMatrix0f(&pose, &inverse, &parents[top].pose);
                for (uint32 row = 0; row < 3; ++row)
                {
                    parents[top].pose.t[row] = pose.t[row];
                    for (uint32 column = 0; column < 3; ++column)
                        parents[top].pose.t[row] -= parents[top].pose.m[row][column] * source.t[column];
                }
                parents[top].ordinal = ordinal;
            }
            else if (top >= 0)
                CompMatrixf(&parents[top].pose, &source, &pose);
            else
                pose = source;
        }
    }
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

Character::Character() : VisualObject(), hidden_models(0), model_hierarchy(0), model_angles(0), model_poses(0), model_pose_count(0), hd_effect_history(0), hd_pose_workspace(0)
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
    delete hd_pose_workspace;
    hd_pose_workspace = 0;
    hd_model_effect_free(hd_effect_history);
    hd_effect_history = 0;
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
    hd_model_effect_reset(hd_effect_history);
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

void Character::CaptureHdHierarchy(HD_OBJECT_STATE &state) const
{
    state.pose_links = 0;
    state.pose_link_count = 0;
    if (!model_hierarchy || !model_pose_count)
        return;
    if (!hd_pose_workspace)
        hd_pose_workspace = new HD_POSE_WORKSPACE;
    hd_pose_workspace->Reserve(model_pose_count, model_hierarchy->count);
    for (uint32 index = 0; index < model_hierarchy->count; ++index)
    {
        const ModelHierarchyLink &link = model_hierarchy->links[index];
        hd_pose_workspace->links[index].part = link.bone;
        hd_pose_workspace->links[index].parent = link.parent == 65535 ? 0xFFFFFFFF : model_hierarchy->links[link.parent].bone;
    }
    state.pose_links = hd_pose_workspace->links;
    state.pose_link_count = model_hierarchy->count;
}

uint32 Character::CaptureHdPoses(MATRIXF *output, uint32 count) const
{
    const ModelBank &bank = ModelBank::Get(model.bank);
    if (count != bank.geometry.count || !output)
        return 0;
    const ModelAnimation &animation = GetModelAnimation(bank.animations, model.animation);
    float frame = model.frame;
    if (playback.dir && !playback.complete)
        frame += playback.phase / 65536.0f;
    if (model_hierarchy && model_pose_count == count)
        animation.CaptureHdHierarchy(frame, playback.loop_mode == 1, output, count, *model_hierarchy);
    else
        animation.CaptureHd(frame, playback.loop_mode == 1, output, count);
    if (model.flags & 4)
    {
        if (!model_poses || !model_angles || !model_hierarchy || count != model_pose_count)
            return 0;
        if (!hd_pose_workspace)
            hd_pose_workspace = new HD_POSE_WORKSPACE;
        hd_pose_workspace->Reserve(count, model_hierarchy->count);
        memcpy(hd_pose_workspace->base, output, count * sizeof(MATRIXF));
        hd_pose_workspace->Apply(*model_hierarchy, model_angles, output);
    }
    MATRIXF world;
    model.angles.BuildRotationMatrixf(world);
    if (model.flags & 0x200)
    {
        VECTORF scale = {model.scale.x / 4096.0f, model.scale.y / 4096.0f, model.scale.z / 4096.0f};
        ScaleMatrixf(&world, &scale);
    }
    world.t[0] = model.pos.x / 4096.0f;
    world.t[1] = model.pos.y / 4096.0f;
    world.t[2] = model.pos.z / 4096.0f;
    for (uint32 part = 0; part < count; ++part)
        CompMatrixf(&world, output + part, output + part);
    return count;
}

bool Character::CaptureHdModel(HD_OBJECT_STATE &state, const MATRIXF *poses, uint32 count, bool update_effects, bool logic) const
{
    const ModelGeometrySet &models = ModelBank::Get(model.bank).geometry;
    uint32 mode = model_effect.color_mode >> 24;
    const TextureGroup *animation = 0;
    sint32 animation_frame = 0;
    if (mode & 127)
    {
        uint32 index = (mode - 1) & 127;
        animation = EffectObject::GetResourceGrp(index < 15 ? index : 14);
        if (!animation)
        {
            GameDiagnostic(("Missing model effect texture group\n"));
            abort();
        }
        uint32 pass = model_render_state.effect_frame == game_clock.active_frame ? model_render_state.effect_pass : model_render_state.pass_count;
        if (!logic && update_effects && model_render_state.effect_frame != game_clock.active_frame)
        {
            model_render_state.effect_pass = pass;
            model_render_state.effect_frame = game_clock.active_frame;
        }
        sint32 dividend = (sint32)pass, divisor = (sint32)animation->count;
        animation_frame = divisor == 0 ? dividend : (dividend == (-2147483647 - 1) && divisor == -1 ? 0 : dividend % divisor);
    }
    GameLighting lighting = game_lighting;
    if (!(model.flags & 128))
        memset(&lighting.directions, 0, sizeof(lighting.directions));
    const ModelProjectionConfig &config = GetModelProjConfig();
    VECTORF constant = {(float)model_render_state.draw.light.x, (float)model_render_state.draw.light.y, (float)model_render_state.draw.light.z};
    for (uint32 part = 0; part < count; ++part)
    {
        if (hidden_models & ((uint32)1 << (part & 31)))
        {
            if (logic)
                for (uint32 pass = 0; pass < 2; ++pass)
                    if (!hd_model_effect_tick(&hd_effect_history, part * 2 + pass, count * 2, model.bank, 0))
                        return false;
            continue;
        }
        const ModelGeometry &geometry = models.models[part];
        state.model = part;
        state.reflection = geometry.flags & 1;
        if (!logic && !(mode & 128))
        {
            lighting.CaptureHd(state.lighting, poses[part], model_render_state.draw.mode, constant, (float)config.fog_start, (sint32)config.fog_shift);
            if (!hd_scene_add(&state, poses, count))
                return false;
        }
        HD_MODEL_EFFECT effect = {0};
        effect.matrix = poses[part];
        effect.minimum.vx = (float)geometry.bounds[0].min;
        effect.minimum.vy = (float)geometry.bounds[1].min;
        effect.minimum.vz = (float)geometry.bounds[2].min;
        effect.maximum.vx = (float)(uint16)geometry.bounds[0].max;
        effect.maximum.vy = (float)(uint16)geometry.bounds[1].max;
        effect.maximum.vz = (float)(uint16)geometry.bounds[2].max;
        for (uint32 pass = 0; pass < 2; ++pass)
        {
            const TextureResource *texture;
            if (!pass)
            {
                if (!animation || (state.water_clip && poses[part].t[1] >= state.water_y - 40))
                {
                    if (logic && !hd_model_effect_tick(&hd_effect_history, part * 2 + pass, count * 2, model.bank, 0))
                        return false;
                    continue;
                }
                texture = TextureGroup::GetFrame(animation, animation_frame).tex;
                effect.color = model_effect.color_mode & 0xFFFFFF;
                effect.scale = model_effect.scale / 4096.0f;
                effect.depth_bias = model_effect.depth_bias;
            }
            else
            {
                if (!projection.enabled)
                {
                    if (logic && !hd_model_effect_tick(&hd_effect_history, part * 2 + pass, count * 2, model.bank, 0))
                        return false;
                    continue;
                }
                texture = EffectObject::GetShadowTex();
                effect.projection = 1;
                effect.color = projection.color;
                effect.normal.vx = surface.n.vx / 4096.0f;
                effect.normal.vy = surface.n.vy / 4096.0f;
                effect.normal.vz = surface.n.vz / 4096.0f;
                effect.plane.vx = surface.pos.x / 4096.0f;
                effect.plane.vy = surface.pos.y / 4096.0f;
                effect.plane.vz = surface.pos.z / 4096.0f;
                effect.direction.vx = projection.dir.x / 4096.0f;
                effect.direction.vy = projection.dir.y / 4096.0f;
                effect.direction.vz = projection.dir.z / 4096.0f;
            }
            if (!texture)
            {
                GameDiagnostic(("Missing projection shadow texture\n"));
                abort();
            }
            effect.image.page = texture->tpage;
            effect.image.clut = texture->clut;
            effect.image.region = texture->region;
            effect.image.texel_flags = texture->HdTexelFlags(effect.image.clut);
            for (uint32 corner = 0; corner < 4; ++corner)
                effect.image.uv[corner] = texture->uv[corner].uv;
            if (!(logic ? hd_model_effect_tick(&hd_effect_history, part * 2 + pass, count * 2, model.bank, &effect) : hd_model_effect_capture(&hd_effect_history, part * 2 + pass, count * 2, model.bank, &effect)))
                return false;
        }
    }
    return true;
}

void Character::ResetHdEffects()
{
    hd_model_effect_reset(hd_effect_history);
}
