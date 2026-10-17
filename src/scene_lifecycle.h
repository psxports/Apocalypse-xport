#ifndef SCENE_LIFECYCLE_H
#define SCENE_LIFECYCLE_H

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

uint32 ServiceGameFrame();
uint32 AdvanceSceneVBlank();
uint32 ResetSceneDisplayOffsets();
uint32 InitializeSceneDisplay();
uint32 WaitForGameFrames(uint32 frames);
uint32 RunLevelLoop();
uint32 DrawSceneFrame();
uint32 BeginSceneFrame();
uint32 UpdateScenePacketLimit();
void SubmitSceneFrame();

void DestroyTransientSceneObjects(ModelInstance *instance);
uint32 ClearScene();
uint32 ResetScene(uint32 retain_resources);

#endif
