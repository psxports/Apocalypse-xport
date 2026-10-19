#ifndef VISUAL_OBJECT_H
#define VISUAL_OBJECT_H
#include "scaled_object.h"
#include "record_data.h"
#include "vector3.h"

class VisualObject;
class TexturedQuad;
void FlagObjectNearReference(VisualObject &object);

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
extern uint32 visual_activation_distance;
void ResetVisualLists();

class VisualObject : public ScaledObject
{
public:
    virtual ~VisualObject();
    ModelInstance *AddToList(ModelInstance *&head);
    ModelInstance *RemoveFromList(ModelInstance *&head);
    uint32 RestoreList();
    uint32 MoveToTemporaryList(ModelInstance *&head);
    RecordCursor ReadPosition(RecordCursor data);
    RecordCursor ReadAngles(RecordCursor data);
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
    uint32 DeactivateIfDistant(uint32 distance);
    uint32 AdvanceColor();
    uint32 UpdateEventLatch(uint32 active, uint32 mask);
    uint32 EnableShadow();
    uint32 RemoveShadow();
    uint32 UpdateShadow();
    uint32 *ApplyContactImpulse(const VisualObject &object);

    struct Surface
    {
        SVECTOR n;
        Vector3 pos;
    } surface;

    struct Shadow
    {
        ObjectReference<TexturedQuad> effect;
        uint16 radius;
        uint16 height;
        uint16 max_height;

        Shadow() : radius(0), height(0), max_height(0)
        {
        }
    } shadow;

    uint32 contact_strength;
    uint32 player_distance;

    struct ModelEffect
    {
        uint32 color_mode;
        uint16 scale;
        uint16 depth_bias;

        ModelEffect() : color_mode(0), scale(0), depth_bias(0)
        {
        }
    } model_effect;

    uint16 kind;
    uint16 activation_flags;
    uint16 lifecycle_flags;
    uint8 target_mark;
    uint8 event_flags;
    uint16 record_idx;
    uint16 collision_radius;
    uint16 contact_flags;
    sint16 health;
    uint8 target_disabled;

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
        Vector3 velocity;
        Vector3 acceleration;
        uint8 drag[3];
        ShortVector3 angular_velocity;
        ShortVector3 angular_acceleration;
        uint8 angular_drag[3];
        Vector3 delta_velocity;

        void SetAngularVelocityXY(uint32 packed)
        {
            angular_velocity.words[0] = packed;
            angular_velocity.words[1] = packed >> 16;
        }

        void SetAngularAccelerationXY(uint32 packed)
        {
            angular_acceleration.words[0] = packed;
            angular_acceleration.words[1] = packed >> 16;
        }

        Motion()
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                velocity.words[axis] = 0;
                acceleration.words[axis] = 0;
                drag[axis] = 1;
                angular_velocity.words[axis] = 0;
                angular_acceleration.words[axis] = 0;
                angular_drag[axis] = 1;
                delta_velocity.words[axis] = 0;
            }
        }
    } motion;

protected:
    VisualObject();
    ModelInstance **saved_head;

    struct PositionHistory
    {
        Vector3 prev;
        Vector3 saved;

        PositionHistory()
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                prev.words[axis] = 0;
                saved.words[axis] = 0;
            }
        }
    } pos_history;

private:
    ModelInstance **list_head;
    friend void SmoothObjectPositions(ModelInstance *instance);
    friend ModelInstance *SaveObjectPositions(ModelInstance *instance);
    friend ModelInstance *RestoreObjectPositions(ModelInstance *instance);

    struct PoseInterpolation
    {
        uint8 count;
        uint8 step;
        Vector3 target_pos;
        Vector3 delta_pos;
        ShortVector3 prev_angles;
        ShortVector3 target_angles;
        ShortVector3 delta_angles;

        PoseInterpolation() : count(0), step(0)
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                target_pos.words[axis] = 0;
                delta_pos.words[axis] = 0;
                prev_angles.words[axis] = 0;
                target_angles.words[axis] = 0;
                delta_angles.words[axis] = 0;
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
