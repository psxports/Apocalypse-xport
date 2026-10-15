#ifndef MENU_WINDOW_H
#define MENU_WINDOW_H

extern "C"
{
#include "psx.h"
}

class MenuWindow
{
public:
    MenuWindow(uint32 x, uint32 y, uint32 mode, uint32 selected_scale, uint32 unselected_scale, uint32 spacing);
    virtual ~MenuWindow() {}
    static MenuWindow &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 CenterVertical(void);
    uint32 Draw(void);
    uint32 Update(void);
    uint32 FindItem(const char *label);
    uint32 AddItem(const char *label);
    uint32 EnableItem(const char *label);
    uint32 DisableItem(const char *label);
    uint32 SetItemHeight(const char *label, uint32 height);
    uint32 IsSelected(const char *label);
    uint32 GetItemPosition(const char *label, uint32 *x, uint32 *y);
    uint32 ResetSelection(uint32 selected);
    uint32 ResetSelection(void);
};

#endif
