#ifndef VISUAL_OBJECT_H
#define VISUAL_OBJECT_H
#include "scaled_object.h"

void FlagObjectNearReference(uint32 object);
// Native adapter retains the exact original target and adjusted receiver
uint32 DispatchObjectFinish(uint32 target, uint32 receiver);

struct VisualLists
{
    ModelInstance *players;
    ModelInstance *cameras;
    ModelInstance *actors;
    ModelInstance *solids;
    ModelInstance *registry;
    ModelInstance *pickups;
    ModelInstance *projectiles;
    ModelInstance *suspended;
};

extern VisualLists visual_lists;
void ResetVisualLists();

class VisualObject : public ScaledObject
{
public:
    virtual ~VisualObject() {}
    static VisualObject &Attach(uint32 object);
    ModelInstance *AddToList(ModelInstance *&head);
    ModelInstance *RemoveFromList(ModelInstance *&head);
    uint32 RestoreList();
    uint32 MoveToTemporaryList(ModelInstance *&head);
    const uint8 *ReadPosition(const uint8 *data);
    const uint8 *ReadAngles(const uint8 *data);
    uint32 SetModelBank(const char *name);
    uint32 SetColor(sint32 duration, uint32 red, uint32 green, uint32 blue);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();
    virtual uint32 Update();
    virtual uint32 UpdateLighting();
    virtual uint32 ReleaseResources();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 reason);
    void UpdateAnimationPose();
    uint32 IsFinished();
    uint32 AdvanceColor();
    uint32 UpdateEventLatch(uint32 active, uint32 mask);
    uint32 EnableShadow();
    uint32 RemoveShadow();
    uint32 UpdateShadow();

    uint16 kind;
    uint16 activation_flags;
    uint16 lifecycle_flags;
    uint8 target_mark;
    uint8 event_flags;
    uint16 record_idx;

private:
    struct ColorTransition
    {
        uint32 saved;
        uint16 timer;
        uint16 step[3];

        ColorTransition() : saved(0), timer(0)
        {
            for (uint32 channel = 0; channel < 3; ++channel)
                step[channel] = 0;
        }
    } color_transition;

public:
    struct Motion
    {
        uint32 velocity[3];
        uint32 acceleration[3];
        uint8 drag[3];
        uint16 angular_velocity[3];
        uint16 angular_acceleration[3];
        uint8 angular_drag[3];

        void SetAngularVelocityXY(uint32 packed)
        {
            angular_velocity[0] = packed;
            angular_velocity[1] = packed >> 16;
        }

        void SetAngularAccelerationXY(uint32 packed)
        {
            angular_acceleration[0] = packed;
            angular_acceleration[1] = packed >> 16;
        }

        Motion()
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                velocity[axis] = 0;
                acceleration[axis] = 0;
                drag[axis] = 1;
                angular_velocity[axis] = 0;
                angular_acceleration[axis] = 0;
                angular_drag[axis] = 1;
            }
        }
    } motion;

protected:
    VisualObject();
    ModelInstance **saved_head;

    struct PositionHistory
    {
        uint32 prev[3];
        uint32 saved[3];

        PositionHistory()
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                prev[axis] = 0;
                saved[axis] = 0;
            }
        }
    } pos_history;

private:
    friend void SmoothObjectPositions(ModelInstance *instance);
    friend ModelInstance *SaveObjectPositions(ModelInstance *instance);
    friend ModelInstance *RestoreObjectPositions(ModelInstance *instance);

    struct PoseInterpolation
    {
        uint8 count;
        uint8 step;
        uint32 target_pos[3];
        uint32 delta_pos[3];
        uint16 prev_angles[3];
        uint16 target_angles[3];
        uint16 delta_angles[3];

        PoseInterpolation() : count(0), step(0)
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                target_pos[axis] = 0;
                delta_pos[axis] = 0;
                prev_angles[axis] = 0;
                target_angles[axis] = 0;
                delta_angles[axis] = 0;
            }
        }
    } pose_interpolation;

    void UpdateAnimationBehavior();
};

void SmoothObjectPositions(ModelInstance *instance);
ModelInstance *SaveObjectPositions(ModelInstance *instance);
ModelInstance *RestoreObjectPositions(ModelInstance *instance);
void UpdateVisualObjects(ModelInstance *&head);

#endif
