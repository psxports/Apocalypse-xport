#ifndef VISUAL_OBJECT_H
#define VISUAL_OBJECT_H

#include "object_reference.h"
#include "scaled_object.h"
#include "record_data.h"
#include "vector3.h"

class VisualObject;
class TexturedQuad;

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

class VisualObject : public ScaledObject
{
public:
    struct Surface
    {
        SVECTOR n;
        Vector3 pos;
    } surface;

    struct Shadow
    {
        ObjectReference<TexturedQuad> effect;
        uint16 radius;
        sint16 height;
        sint16 max_height;

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
    uint16 rec_idx;
    uint16 col_radius;
    uint16 contact_flags;
    sint16 health;
    uint8 target_disabled;

private:
    struct ColorTransition
    {
        uint32 saved;
        uint16 timer;

        struct ColorStep
        {
            sint16 r;
            sint16 g;
            sint16 b;
        } step;

        ColorTransition() : saved(0), timer(0), step()
        {
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
            angular_velocity.x = packed;
            angular_velocity.y = packed >> 16;
        }

        void SetAngularAccelerationXY(uint32 packed)
        {
            angular_acceleration.x = packed;
            angular_acceleration.y = packed >> 16;
        }

        Motion()
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                velocity.comp[axis] = 0;
                acceleration.comp[axis] = 0;
                drag[axis] = 1;
                angular_velocity.comp[axis] = 0;
                angular_acceleration.comp[axis] = 0;
                angular_drag[axis] = 1;
                delta_velocity.comp[axis] = 0;
            }
        }
    } motion;

protected:
    ModelInstance **saved_head;

    struct PositionHistory
    {
        Vector3 prev;
        Vector3 saved;

        PositionHistory()
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                prev.comp[axis] = 0;
                saved.comp[axis] = 0;
            }
        }
    } pos_history;

private:
    ModelInstance **list_head;

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
                target_pos.comp[axis] = 0;
                delta_pos.comp[axis] = 0;
                prev_angles.comp[axis] = 0;
                target_angles.comp[axis] = 0;
                delta_angles.comp[axis] = 0;
            }
        }
    } pose_interpolation;

public:
    void FlagNearRingAttack();
    static void SmoothListPos(ModelInstance *instance);
    static ModelInstance *SaveListPos(ModelInstance *instance);
    static ModelInstance *RestoreListPos(ModelInstance *instance);
    static void FinishSelEnemies();
    uint32 SetModelBank(const char *name);
    uint32 SetColor(sint32 duration, uint8 r, uint8 g, uint8 b);
    uint32 AdvanceColor();

protected:
    VisualObject();

public:
    virtual uint32 Destroy(uint32 reason);
    ModelInstance *AddToList(ModelInstance *&head);
    ModelInstance *RemoveFromList(ModelInstance *&head);
    RecordCursor ReadPos(RecordCursor data);
    RecordCursor ReadAngles(RecordCursor data);
    uint32 MoveToTmpList(ModelInstance *&head);
    uint32 RestoreList();
    Vector3 *ApplyContactImpulse(const VisualObject &object);
    bool UpdateEventLatch(uint32 active, uint32 mask);
    uint32 EnableShadow();
    uint32 RemoveShadow();
    uint32 UpdateShadow();
    virtual uint32 Finish();
    bool IsFinished();
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 reason);
    void UpdateAnimationPose();
    static void ClearActivationFlags(ModelInstance *instance);
    static void UpdateList(ModelInstance *&head);
    static void RestoreNearbySuspended();
    virtual uint32 ReleaseResources();
    virtual uint32 UpdateLighting();
    virtual uint32 Update();
    static void ActivateByIdx(ModelInstance *instance, uint32 idx);
    sint32 TrackHealth(uint16 x, uint16 y, uint32 scale);
    static void ClearTrackedHealth();
    static void DrawTrackedHealth();
    bool DeactivateIfDistant(uint32 distance);
    static void ResetLists();
    virtual ~VisualObject();

private:
    void UpdateAnimationBehavior();
};

#endif
