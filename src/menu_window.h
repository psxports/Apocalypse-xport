#ifndef MENU_WINDOW_H
#define MENU_WINDOW_H

extern "C"
{
#include "psx.h"
}

#include "object.h"

struct MenuItem
{
    const char *label;
    uint16 scale;
    uint16 target_scale;
    uint16 selected_scale;
    uint16 unselected_scale;
    uint8 height;
    uint8 enabled;
    uint8 color[2][3];
    uint8 brightness;
    uint16 pulse;
    uint16 glow;
};

void ResetMenuRepeat();

class MenuWindow : public Object
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
    const char *GetItemLabel(uint32 idx) const;
    const char *GetSelectedLabel() const;
    uint32 ResetSelection(uint32 selected);
    uint32 ResetSelection(void);

    uint8 alignment;
    uint8 selected;
    uint8 has_arrows;
    uint8 arrow_item;
    uint8 numbered;
    uint8 count;
    uint32 pos[2];
    uint32 spacing;
    MenuItem items[15];
};

#endif
