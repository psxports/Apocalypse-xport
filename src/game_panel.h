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
class MenuWindow;
class VisualObject;
void DrawMenuVolumeSlider(MenuWindow &menu);

const char *ReadMenuButtonLabel(uint32 button);
sint32 ReadMenuNavigationStep();
sint32 AssignMenuButton(uint32 button, sint16 *sel, sint16 *binding0, sint16 *binding1, sint16 *binding2);
uint32 SetMenuRedText(void);
uint32 SetMenuGreenText(void);
uint32 DrawMenuLeftHint(void);
uint32 DrawMenuHints(void);
uint32 DrawMenuCenteredHint(void);
void DrawPauseMenuDisplay(void);

uint32 TrackObjectHealth(VisualObject &object, uint32 x, uint32 y, uint32 scale);
void ClearTrackedObjectHealth();
void DrawGamePanel();
uint32 DrawDebugPanel();
void LoadPanelResource();
void ForgetPanelTexture(const TextureGroup *grp);
void ClearPanelTextureBinding();
uint32 ResetPanelDisplayTimer();

uint32 SetMenuDarkRedText();
void ResetMenuFireBuffers();
void ReleaseMenuBuffers();
uint32 HasMenuBuffers();
uint32 CreateMenuBuffers(uint32 resource, uint32 width, uint32 height, uint32 mode);
uint32 BlurMenuBuffers();

#endif
