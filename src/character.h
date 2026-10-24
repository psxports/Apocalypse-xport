#ifndef CHARACTER_H
#define CHARACTER_H
#include "visual_object.h"
#include "model_hierarchy.h"
#include "model_animation_data.h"

struct HD_OBJECT_STATE;
struct HD_MODEL_EFFECT_CHAIN;
struct HD_POSE_WORKSPACE;

class Character : public VisualObject
{
public:
    struct AnimationPlayback
    {
        uint8 loop_mode;
        sint8 dir;
        sint8 last_frame;
        uint16 phase;
        uint8 frame_count;
        uint8 complete;
        uint32 speed;
        sint16 cur_frame;
        sint16 prev_frame;
        uint8 prev_animation;
        sint8 prev_dir;

        AnimationPlayback() : loop_mode(0), dir(0), last_frame(0), phase(0), frame_count(0), complete(0), speed(0), cur_frame(0), prev_frame(0), prev_animation(0), prev_dir(0)
        {
        }
    } playback;

    struct ProjectionEffect
    {
        // Original producers select zero or one projection
        bool enabled;
        ShortVector3 dir;
        uint32 color;
    } projection;

    MATRIX world_transform;
    uint32 hidden_models;

public:
    // Native lifetime and decoded-owner helpers have no separate MIPS entry
    virtual ~Character()
    {
        ReleaseModelAnimation();
    }

private:
    const ModelHierarchyTable *model_hierarchy;
    ModelJointAngles *model_angles;
    ModelPose *model_poses;
    uint32 model_pose_count;
    mutable HD_MODEL_EFFECT_CHAIN *hd_effect_history;
    mutable HD_POSE_WORKSPACE *hd_pose_workspace;

public:
    uint32 GetAttachment(Vector3 &output, uint8 requested);

protected:
    Character();

public:
    virtual uint32 Destroy(uint32 reason);
    uint32 SetAnimationRange(uint8 animation, sint32 first, sint32 last);
    uint32 StartAnimation(uint8 animation, sint8 dir);
    uint32 AdvanceAnimation();
    bool DidAnimationCrossFrame(uint32 animation, sint32 target_frame);
    uint32 UpdateAnimationModel(const ModelHierarchyTable &table);
    void InitializeModelAnimation(const ModelHierarchyTable &table);
    void UpdateModelAnimationData();
    uint32 BuildModelPose();
    uint32 RenderModel();
    uint32 UpdateWorldTransform();
    const ModelHierarchyTable &GetModelHierarchy() const;
    bool HasModelJointAngles() const;
    bool HasModelPoses() const;
    uint32 GetModelPoseCount() const;
    ModelPose *GetModelPoses(uint32 first, uint32 count);
    ModelJointAngles &GetModelJointAngles(uint32 idx);
    void ReleaseModelAnimation();
    uint32 CaptureHdPoses(MATRIXF *output, uint32 count) const;
    void CaptureHdHierarchy(HD_OBJECT_STATE &state) const;
    bool CaptureHdModel(HD_OBJECT_STATE &state, const MATRIXF *poses, uint32 count, bool update_effects, bool logic) const;
    void ResetHdEffects();

private:
    uint8 ReadAnimationFrameCount(uint8 animation);
};

#endif
