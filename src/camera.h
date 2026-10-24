#ifndef CAMERA_H
#define CAMERA_H

struct CameraTransition;
struct CollisionRay;
struct PadButton;
struct HD_CAMERA;
struct ModelViewport;

#include "vector3.h"
#include "visual_object.h"
#include "quaternion.h"

extern ModelReference camera_subject;

class Camera : public VisualObject
{
public:
    struct TargetTracking
    {
        ModelReference target;
        uint32 flags;
        Vector3 pos;
        Vector3 step;
        uint32 timer;

        TargetTracking() : flags(0), timer(0)
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                pos.comp[axis] = 0;
                step.comp[axis] = 0;
            }
        }
    };

    TargetTracking follow;
    TargetTracking aim;
    uint32 proj_distance;
    uint16 proj_timer;
    uint32 proj_step;
    uint32 clamp_enabled;
    Vector3 clamp_start;
    Vector3 clamp_end;
    sint16 vertical_pitch;
    sint16 vertical_offset_gain;

private:
    Vector3 view_up;

public:
    Quaternion rotation;
    Quaternion col_rotation;
    Quaternion blend_rotation;
    Quaternion saved_rotation;
    ShortVector3 angles;

private:
    Vector3 view_from;
    Vector3 view_to;

public:
    sint32 col_lateral_dist;
    sint32 col_rear_dist;
    sint16 col_yaw_strength;
    sint16 col_pitch_strength;

    struct Shake
    {
        ShortVector3 amplitude;
        uint8 decay[3];
        ShortVector3 frequency;
    } shake;

    uint32 mode;
    uint16 turn_step;
    uint32 update_ticks;

    virtual ~Camera()
    {
    }

private:
    // Native helpers extracted from the original tracking and update bodies

public:
    static sint16 ShortestAngleDelta(sint16 src_angle, sint16 target_angle);
    Camera(VisualObject &target);
    virtual uint32 Destroy(uint32 reason);
    void SetColDistances(sint32 val);
    void SetLateralColDistance(sint32 val);
    void SetRearColDistance(sint32 val);
    void SetColYawStrength(sint16 val);
    void SetColPitchStrength(sint16 val);
    static void MarkShakeList(ModelInstance *list);
    void ApplyExplosionShake(uint32 kind);
    uint32 SetModePreset(uint16 mode);

private:
    MATRIX *PrepareView();
    void ResolveCollisions();

public:
    void SetTargetTransition(const Vector3 &pos, uint32 frames);
    void ClearTargetTransition();

private:
    void ClampTargetToSeg();

public:
    void SetYawTransition(uint16 target, uint16 frames);
    void SetOrbitDistanceTransition(uint16 target, uint16 frames);
    void SetOrbitHeightTransition(sint16 target, uint16 frames);
    void SetTargetLateralOffsetTransition(sint16 target, uint16 frames);
    void SetTargetHeightOffsetTransition(sint16 target, uint16 frames);
    void SetTargetForwardOffsetTransition(sint16 target, uint16 frames);
    void SetProjDistanceTransition(uint16 target, uint16 frames);
    uint32 GetProjDistance();
    void CaptureHd(HD_CAMERA &camera, const ModelViewport &bounds) const;
    virtual uint32 Update();

private:
    uint32 BuildView();
    uint32 UpdatePlayerRelativeTracking();
    uint32 UpdateFixedOriginTracking();

public:
    static void ResetDefaults();

private:
    static void AdvanceGlobalTransition(CameraTransition &transition);
    static void AdjustDebugVal(const PadButton &button, uint32 &val, bool subtract);
    static sint16 ReadColAngle(CollisionRay &rec, sint16 strength, bool reverse);
    static void RotateVector(const MATRIX &matrix, const Vector3 &input, Vector3 &output);
    void TurnTracking(sint16 delta, sint32 distance, bool fine_turn);
    sint16 ReadOscillation(uint32 axis) const;
};

#endif
