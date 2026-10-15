#ifndef GAME_PANEL_H
#define GAME_PANEL_H

extern "C"
{
#include "psx.h"
}

class MenuWindow;
uint32 DrawMenuVolumeSlider(MenuWindow &menu);

const char *ReadMenuButtonLabel(uint32 button);
sint32 ReadMenuNavigationStep();
sint32 AssignMenuButton(uint32 button, sint16 *selected, sint16 *other1, sint16 *other2, sint16 *other3);
uint32 SetMenuRedText(void);
uint32 SetMenuGreenText(void);
uint32 DrawMenuLeftHint(void);
uint32 DrawMenuRightHint(void);
uint32 DrawMenuHints(void);
uint32 DrawMenuCenteredHint(void);
uint32 DrawPauseMenuHeading(void);
uint32 DrawPauseMenuDisplay(void);

uint32 TrackObjectHealth(uint32 object, uint32 x, uint32 y, uint32 scale);
void ClearTrackedObjectHealth();
void DrawTrackedObjectHealth();
void DrawWeaponAmmoIndicator(uint32 slot);
void DrawGamePanel();
uint32 DrawDebugPanel();
uint32 LoadPanelResource();
uint32 ResetPanelDisplayTimer();

uint32 SetMenuDarkRedText();
uint32 HasMenuBuffers();
uint32 CreateMenuBuffers(uint32 resource, uint32 width, uint32 height, uint32 mode);
uint32 BlurMenuBuffers();
uint32 SeedMenuFire(uint32 sparks);

#endif
