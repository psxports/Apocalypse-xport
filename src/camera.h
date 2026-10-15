#ifndef CAMERA_H
#define CAMERA_H

#include "visual_object.h"

uint32 ResetCameraPresetData();


uint32 ClearCameraPresetFlags();

class Camera : public VisualObject
{
public:
    virtual ~Camera() {}
    Camera(const VisualObject &target);
    static Camera &Attach(uint32 object);
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
    uint32 SetTargetTransition(const void *position, uint32 frames);
    uint32 ClampTargetToSegment();
    uint32 SetProjDistanceTransition(uint32 target, uint32 frames);
    uint32 GetProjDistance();
    uint32 SetYawTransition(uint32 target, uint32 frames);
    uint32 PrepareView();
    uint32 BuildView();
    uint32 ResolveCollisions();
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
