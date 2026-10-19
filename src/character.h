#ifndef CHARACTER_H
#define CHARACTER_H
#include "visual_object.h"
#include "model_hierarchy.h"
#include "model_animation_data.h"

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
        bool enabled;
        MATRIX dir;
        uint32 color;
    } projection;

    MATRIX world_transform;
    uint32 hidden_models;
    uint32 UpdateWorldTransform();
    uint32 GetAttachment(uint32 *output, uint32 requested);

    virtual ~Character()
    {
        ReleaseModelAnimation();
    }

    virtual uint32 Destroy(uint32 reason);
    uint32 SetAnimationRange(uint32 animation, uint32 first, uint32 last);
    uint32 StartAnimation(uint32 animation, uint32 direction);
    uint32 AdvanceAnimation();
    uint32 UpdateAnimationModel(const ModelHierarchyTable &table);
    void UpdateModelAnimationData();
    void InitializeModelAnimation(const ModelHierarchyTable &table);
    const ModelHierarchyTable &GetModelHierarchy() const;
    bool HasModelJointAngles() const;
    bool HasModelPoses() const;
    uint32 GetModelPoseCount() const;
    ModelPose *GetModelPoses(uint32 first, uint32 count);
    ModelJointAngles &GetModelJointAngles(uint32 index);
    void ReleaseModelAnimation();
    uint32 DidAnimationCrossFrame(uint32 animation, uint32 target_frame);

protected:
    uint32 ReadAnimationFrameCount(uint32 animation);
    Character();

private:
    const ModelHierarchyTable *model_hierarchy;
    ModelJointAngles *model_angles;
    ModelPose *model_poses;
    uint32 model_pose_count;
};

#endif
