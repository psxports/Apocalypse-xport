#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"

#include "visual_object.h"
#include "quaternion_math.h"

void ResetCameraDefaults();
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
                pos.words[axis] = 0;
                step.words[axis] = 0;
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
    uint16 vertical_pitch;
    uint16 vertical_offset_gain;
    Quaternion rotation;
    Quaternion collision_rotation;
    Quaternion blend_rotation;
    Quaternion saved_rotation;
    ShortVector3 angles;
    uint32 col_lateral_dist;
    uint32 col_rear_dist;
    uint16 col_yaw_strength;
    uint16 col_pitch_strength;

    struct Shake
    {
        uint16 amplitude[3];
        uint8 decay[3];
        uint16 frequency[3];
    } shake;

    uint32 mode;
    uint32 turn_step;

    virtual ~Camera()
    {
    }

    Camera(VisualObject &target);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void ClearTargetTransition();
    void SetColDistances(uint32 value);
    void SetLateralColDistance(uint32 value);
    void SetRearColDistance(uint32 value);
    void SetColYawStrength(uint32 value);
    void SetColPitchStrength(uint32 value);
    uint32 SetModePreset(uint32 mode);
    uint32 ApplyExplosionShake(uint32 kind);
    void SetTargetTransition(const uint32 *position, uint32 frames);
    uint32 ClampTargetToSegment();
    uint32 SetProjDistanceTransition(uint32 target, uint32 frames);
    uint32 GetProjDistance();
    uint32 SetYawTransition(uint32 target, uint32 frames);
    MATRIX *PrepareView();
    uint32 BuildView();
    void ResolveCollisions();
    uint32 UpdatePlayerRelativeTracking();
    uint32 UpdateFixedOriginTracking();
};

// Shared camera tuning transitions have no instance receiver
uint32 SetCameraOrbitDistanceTransition(uint32 target, uint32 frames);
uint32 SetCameraOrbitHeightTransition(uint32 target, uint32 frames);
uint32 SetCameraTargetLateralOffsetTransition(uint32 target, uint32 frames);
uint32 SetCameraTargetHeightOffsetTransition(uint32 target, uint32 frames);
uint32 SetCameraTargetForwardOffsetTransition(uint32 target, uint32 frames);

#endif
