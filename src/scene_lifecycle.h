#ifndef SCENE_LIFECYCLE_H
#define SCENE_LIFECYCLE_H

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

struct SceneFrameState
{
    uint32 paused;
    uint32 camera_paused;
    uint32 submit_pending;
    uint32 service_enabled;
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
void ResetSceneDisplayOffsets();
uint32 InitializeSceneDisplay();
uint32 WaitForGameFrames(uint32 frames);
uint32 RunLevelLoop();
void BeginSceneFrame();
void UpdateScenePacketLimit();
void SubmitSceneFrame();

uint32 ResetScene(uint32 retain_resources);

#endif
