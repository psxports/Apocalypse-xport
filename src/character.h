#ifndef CHARACTER_H
#define CHARACTER_H
#include "visual_object.h"
#include "model_hierarchy.h"
#include "model_animation_data.h"

class Character : public VisualObject
{
public:
    virtual ~Character() { ReleaseModelAnimation(); }
    virtual uint32 Destroy(uint32 reason);
    uint32 SetAnimationRange(uint32 animation, uint32 first, uint32 last);
    uint32 StartAnimation(uint32 animation, uint32 direction);
    uint32 AdvanceAnimation();
    uint32 UpdateAnimationModel(const ModelHierarchyTable &table);
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
    Character();

private:
    const ModelHierarchyTable *model_hierarchy;
    ModelJointAngles *model_angles;
    ModelPose *model_poses;
    uint32 model_pose_count;
};

#endif
