#ifndef SCENE_LIFECYCLE_H
#define SCENE_LIFECYCLE_H

struct ModelInstance;
struct HD_FRAME;

extern "C"
{
#include "psx.h"
}

struct SceneFrameState
{
    bool paused;
    bool camera_paused;
    uint32 submit_pending;
    bool service_enabled;
};

struct SceneRenderBuffer
{
    DRAWENV draw;
    DISPENV disp;
    uint32 ot[4096];
    uint32 packets[0x1AF00 / sizeof(uint32)];
};

struct SceneRenderState
{
    SceneRenderBuffer buffers[2];
    SceneRenderBuffer *cur;
    SceneRenderBuffer *published;
    uint8 *cursor;
    uint8 *limit;
    uint32 used;
};

extern SceneRenderState scene_render;
void ResetSceneRenderState();

extern SceneFrameState scene_frame;
void ResetSceneFrameState();
void ResetSceneSmoothing();
uint32 ServiceGameFrame();
void SceneVBlankCallback();
uint32 AdvanceSceneVBlank();
void ResetSceneDispOffsets();
uint32 InitializeSceneDisp();
bool WaitForGameFrames(uint32 frames);
bool WaitForRenderFrames(uint32 frames, HD_FRAME *frame, float (*phase)());
uint32 RunLevelLoop();
void CaptureHdLogicState();
void ResetHdCamera(const ModelInstance *model);
void BeginSceneFrame();
void UpdateScenePacketLimit();
void SubmitSceneFrame();

sint32 ResetScene(bool retain_resources);

#endif
