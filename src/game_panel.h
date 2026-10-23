#ifndef GAME_PANEL_H
#define GAME_PANEL_H

extern "C"
{
#include "psx.h"
}

struct PanelText
{
    const char *caption;
    uint32 visible;
    sint16 game_over_y;
    uint32 lives_timer;
    char lives[3];
    uint32 reveal_frame;
};

extern PanelText panel_text;
void ResetPanelText();

struct TextureGroup;

uint32 DrawMenuLeftHint(void);
uint32 DrawMenuHints(void);
uint32 DrawMenuCenteredHint(void);
void DrawPauseMenuDisp();

void DrawGamePanel();
uint32 DrawDebugPanel();
void LoadPanelResource();
void ForgetPanelTex(const TextureGroup *grp);
uint32 ResetPanelDispTimer();

void ResetMenuFireBuffers();
void ReleaseMenuBuffers();
uint32 HasMenuBuffers();
// Byte-pixel rows require width >= 4, divisible by 4, and a nonzero height
uint32 CreateMenuBuffers(uint32 resource, uint16 width, uint16 height, uint8 intensity);
uint32 BlurMenuBuffers();

#endif
