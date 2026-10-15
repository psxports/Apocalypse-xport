#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
}
#include "menu_window.h"
#include "game_string.h"
#include "hud_text.h"
#include "audio_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "native_object_storage.h"
#include "game_heap.h"
#include <new>

MenuWindow::MenuWindow(uint32 x, uint32 y, uint32 mode, uint32 selected_scale, uint32 unselected_scale, uint32 spacing)
{
    FUNCTION_MARKER(0x80015228, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    w_u32(object, 0x800A05C4);
    w_u32(object + 12, x);
    w_u32(object + 16, y);
    w_u8(object + 4, mode);
    w_u32(object + 20, spacing);
    for (uint32 index = 0; index < 15; ++index)
    {
        uint32 row = object + index * 28;
        w_u16(row + 46, 0);
        w_u16(row + 48, 0);
        w_u8(row + 38, 200);
        w_u8(row + 39, 200);
        w_u8(row + 40, 200);
        w_u8(row + 41, 128);
        w_u8(row + 42, 128);
        w_u8(row + 43, 128);
        w_u8(row + 44, 128);
        // TODO Confirm the original units of these selection scale values
        w_u16(row + 32, selected_scale);
        w_u16(row + 34, unselected_scale);
    }
    ResetSelection();
}

MenuWindow &MenuWindow::Attach(uint32 object)
{
    return *static_cast<MenuWindow *>(FindNativeObjectStorage(object));
}

uint32 MenuWindow::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800152C4, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    w_u32(object, 0x800A05D4);
    if (reason & 1)
        return ReleaseHeapBlock(object);
    return 0x800A05D4;
}

uint32 MenuWindow::CenterVertical(void)
{
    FUNCTION_MARKER(0x800152F8, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    uint32 rows = r_u8(object + 10);
    uint32 enabled = 0, height = 0;
    for (uint32 index = 0; index < rows; ++index)
    {
        uint32 row = object + index * 28;
        if (r_u8(row + 37))
        {
            ++enabled;
            height += r_u8(row + 36);
        }
    }
    uint32 spacing = r_u32(object + 20);
    uint32 span = (enabled - 1) * spacing + height;
    uint32 result = (uint32)((sint32)(240 - span) / 2);
    w_u32(object + 16, result);
    return result;
}

uint32 MenuWindow::ResetSelection(uint32 selected)
{
    FUNCTION_MARKER(0x80015374, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    w_u8(object + 6, selected);
    for (uint32 index = 0; index < 15; ++index)
    {
        uint32 row = object + index * 28;
        uint32 selection = r_u8(object + 6);
        uint32 scale = r_u16(row + (index == selection ? 32 : 34));
        w_u16(row + 30, scale);
        scale = r_u16(row + 30);
        w_u16(row + 46, 0);
        w_u16(row + 48, 0);
        w_u8(row + 44, 128);
        w_u16(row + 28, scale);
    }
    return 0;
}

uint32 MenuWindow::ResetSelection(void)
{
    FUNCTION_MARKER(0x800153D8, "SLUS_003.73");
    return ResetSelection(0);
}

extern "C" uint32 sub_80015228(uint32 object, uint32 x, uint32 y, uint32 mode, uint32 selected_scale, uint32 unselected_scale, uint32 spacing)
{
    FUNCTION_MARKER(0x80015228, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(MenuWindow));
    new (dram_addr) MenuWindow(x, y, mode, selected_scale, unselected_scale, spacing);
    return object;
}

extern "C" uint32 sub_800152C4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800152C4, "SLUS_003.73");
    return MenuWindow::Attach(object).MenuWindow::Destroy(reason);
}

extern "C" uint32 sub_800152F8(uint32 object)
{
    FUNCTION_MARKER(0x800152F8, "SLUS_003.73");
    return MenuWindow::Attach(object).CenterVertical();
}

extern "C" uint32 sub_80015374(uint32 object, uint32 selected)
{
    FUNCTION_MARKER(0x80015374, "SLUS_003.73");
    return MenuWindow::Attach(object).ResetSelection(selected);
}

extern "C" uint32 sub_800153D8(uint32 object)
{
    FUNCTION_MARKER(0x800153D8, "SLUS_003.73");
    return MenuWindow::Attach(object).ResetSelection();
}

// Native adapter for nullable original label pointers
static const char *MapMenuItemLabel(uint32 label)
{
    return label ? (const char *)psx_addr(label, 1) : 0;
}

uint32 MenuWindow::FindItem(const char *label)
{
    FUNCTION_MARKER(0x800153F8, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    if (!r_u8(object + 10))
        return 0;
    uint32 index = 0;
    uint32 row = object;
    for (;;)
    {
        uint32 stored_label = r_u32(row + 24);
        if (NamesEqualIgnoringCase(label, MapMenuItemLabel(stored_label)))
            return index;
        uint32 count = r_u8(object + 10);
        ++index;
        if (index >= count)
            return 0;
        row += 28;
    }
}

uint32 MenuWindow::AddItem(const char *label)
{
    FUNCTION_MARKER(0x80015474, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    uint32 label_address = label ? GuestMemoryAddress(label) : 0;
    // TODO Persistent menu labels must remain in mapped game memory
    if (label && !label_address)
    {
        GameDiagnostic("TODO Unmapped persistent menu label\n");
        abort();
    }
    uint32 count = r_u8(object + 10);
    w_u32(object + count * 28 + 24, label_address);
    count = r_u8(object + 10);
    w_u8(object + count * 28 + 37, 1);
    count = r_u8(object + 10);
    w_u8(object + count * 28 + 36, 0);
    uint32 result = r_u8(object + 10) + 1;
    w_u8(object + 10, result);
    return result;
}

uint32 MenuWindow::EnableItem(const char *label)
{
    FUNCTION_MARKER(0x800154E0, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index = FindItem(label);
    w_u8(object + index * 28 + 37, 1);
    return 1;
}

uint32 MenuWindow::DisableItem(const char *label)
{
    FUNCTION_MARKER(0x8001551C, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index = FindItem(label);
    w_u8(object + index * 28 + 37, 0);
    return index;
}

uint32 MenuWindow::SetItemHeight(const char *label, uint32 height)
{
    FUNCTION_MARKER(0x80015590, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index = FindItem(label);
    w_u8(object + index * 28 + 36, height);
    return index;
}

uint32 MenuWindow::IsSelected(const char *label)
{
    FUNCTION_MARKER(0x800155D4, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    uint32 selected = r_u8(object + 6);
    uint32 stored_label = r_u32(object + selected * 28 + 24);
    return NamesEqualIgnoringCase(label, MapMenuItemLabel(stored_label));
}

extern "C" uint32 sub_800153F8(uint32 object, uint32 label)
{
    FUNCTION_MARKER(0x800153F8, "SLUS_003.73");
    return MenuWindow::Attach(object).FindItem(MapMenuItemLabel(label));
}

extern "C" uint32 sub_80015474(uint32 object, uint32 label)
{
    FUNCTION_MARKER(0x80015474, "SLUS_003.73");
    return MenuWindow::Attach(object).AddItem(MapMenuItemLabel(label));
}

extern "C" uint32 sub_800154E0(uint32 object, uint32 label)
{
    FUNCTION_MARKER(0x800154E0, "SLUS_003.73");
    return MenuWindow::Attach(object).EnableItem(MapMenuItemLabel(label));
}

extern "C" uint32 sub_8001551C(uint32 object, uint32 label)
{
    FUNCTION_MARKER(0x8001551C, "SLUS_003.73");
    return MenuWindow::Attach(object).DisableItem(MapMenuItemLabel(label));
}

extern "C" uint32 sub_80015590(uint32 object, uint32 label, uint32 height)
{
    FUNCTION_MARKER(0x80015590, "SLUS_003.73");
    return MenuWindow::Attach(object).SetItemHeight(MapMenuItemLabel(label), height);
}

extern "C" uint32 sub_800155D4(uint32 object, uint32 label)
{
    FUNCTION_MARKER(0x800155D4, "SLUS_003.73");
    return MenuWindow::Attach(object).IsSelected(MapMenuItemLabel(label));
}

static uint32 ReadMenuPositionOutput(const uint32 *output)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        return r_u32(address);
    uint32 value;
    memcpy(&value, output, 4);
    return value;
}

static void StoreMenuPositionOutput(uint32 *output, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u32(address, value);
    else
        memcpy(output, &value, 4);
}

uint32 MenuWindow::GetItemPosition(const char *label, uint32 *x, uint32 *y)
{
    FUNCTION_MARKER(0x80015614, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    StoreMenuPositionOutput(x, r_u32(object + 12));
    StoreMenuPositionOutput(y, r_u32(object + 16));
    uint32 result = r_u8(object + 10);
    if (!result)
        return result;
    uint32 index = 0;
    uint32 row = object;
    do
    {
        if (r_u8(row + 37))
        {
            uint32 height = r_u8(row + 36);
            uint32 position = ReadMenuPositionOutput(y);
            StoreMenuPositionOutput(y, position + height);
            uint32 stored_label = r_u32(row + 24);
            result = NamesEqualIgnoringCase(label, MapMenuItemLabel(stored_label));
            if (result)
                return result;
            position = ReadMenuPositionOutput(y);
            uint32 spacing = r_u32(object + 20);
            StoreMenuPositionOutput(y, position + spacing);
        }
        uint32 count = r_u8(object + 10);
        ++index;
        result = index < count;
        row += 28;
    } while (result);
    return result;
}

extern "C" uint32 sub_80015614(uint32 object, uint32 label, uint32 x, uint32 y)
{
    FUNCTION_MARKER(0x80015614, "SLUS_003.73");
    return MenuWindow::Attach(object).GetItemPosition(MapMenuItemLabel(label), (uint32 *)psx_addr(x, 4), (uint32 *)psx_addr(y, 4));
}

extern "C" uint32 xport_draft_host_sub_80015614_p34(uint32 object, uint32 label, void *x, void *y)
{
    FUNCTION_MARKER(0x80015614, "SLUS_003.73");
    return MenuWindow::Attach(object).GetItemPosition(MapMenuItemLabel(label), (uint32 *)x, (uint32 *)y);
}

uint32 MenuWindow::Draw(void)
{
    FUNCTION_MARKER(0x800156E0, "SLUS_003.73");
    uint32 object = NativeObjectGuestAddress(this);
    uint32 row_height, enabled, red, green, blue, brightness, label_address, numbered, draw_color, draw_glow;
    sint32 current_pulse, current_glow, current_scale, unselected_scale;
    uint32 pulse_bits, glow_bits;
    sint32 index;
    uint32 row;
    uint32 y;
    sint32 result;
    short pulse;
    short glow;
    unsigned char fade;
    const char *text;
    uint32 name;
    sint32 length_cursor;
    sint32 length;
    char *suffix;
    sint32 text_scale;
    sint32 width;
    uint32 arrow_offset;
    uint32 left_arrow;
    uint32 right_arrow;
    short left_center;
    short left_edge;
    short right_center;
    short right_edge;
    uint32 ordering_table;
    uint32 link;
    char numbered_label[104];
    index = 0;
    SetHudTextAlignment(r_u8(((uint32)((object + 4)))));
    row = object;
    y = r_u32(((uint32)((object + 16))));
    while (1)
    {
        result = (index < r_u8(((uint32)((object + 10)))));
        if (!result)
            return result;
        row_height = r_u8(row + 36);
        enabled = r_u8(row + 37);
        y += row_height;
        if (!enabled)
            goto LABEL_37;
        if (((sint16)r_u16(((uint32)((row + 28)))) > 0))
        {
            if ((index == r_u8(((uint32)((object + 6))))))
            {
                red = r_u8(row + 38);
                green = r_u8(row + 39);
                blue = r_u8(row + 40);
                SetHudTextColor(red, green, blue, 0);
                current_pulse = (sint16)r_u16(row + 46);
                pulse_bits = r_u16(row + 46);
                pulse = (sint16)(pulse_bits - 16);
                if (current_pulse < 129)
                {
                LABEL_9:
                    if (((sint16)r_u16(((uint32)((row + 46)))) < 128))
                        w_u16(((uint32)((row + 46))), 255);
                }
                else
                {
                    w_u16(((uint32)((row + 46))), pulse);
                    if ((pulse < 128))
                    {
                        w_u16(((uint32)((row + 46))), 128);
                        goto LABEL_9;
                    }
                }
                current_glow = (sint16)r_u16(row + 48);
                glow_bits = r_u16(row + 48);
                glow = (sint16)(glow_bits + 32);
                if (current_glow < 256)
                {
                    w_u16(((uint32)((row + 48))), glow);
                    if ((glow >= 257))
                        w_u16(((uint32)((row + 48))), 256);
                }
                w_u8(((uint32)((row + 44))), 0);
            }
            else
            {
                red = r_u8(row + 41);
                brightness = r_u8(row + 44);
                green = r_u8(row + 42);
                blue = r_u8(row + 43);
                SetHudTextColor((red * brightness >> 7) & 255, (green * brightness >> 7) & 255, (blue * brightness >> 7) & 255, 0);
                fade = (r_u8(((uint32)((row + 44)))) + 6);
                w_u8(((uint32)((row + 44))), fade);
                if ((fade >= 0x81))
                    w_u8(((uint32)((row + 44))), 0x80);
                current_pulse = (sint16)r_u16(row + 46);
                pulse_bits = r_u16(row + 46);
                if (current_pulse < 16)
                    w_u16(((uint32)((row + 46))), 0);
                else
                    w_u16(row + 46, pulse_bits - 16);
                if ((sint16)r_u16(((uint32)((row + 46)))))
                    w_u16(((uint32)((row + 48))), 256);
                else
                    w_u16(((uint32)((row + 48))), 0);
            }
            current_scale = (sint16)r_u16(row + 28);
            unselected_scale = (sint16)r_u16(row + 34);
            SetHudTextDepth(unselected_scale >= current_scale);
            numbered = r_u8(object + 9);
            label_address = r_u32(row + 24);
            text = MapMenuItemLabel(label_address);
            if (numbered && !NamesEqualIgnoringCase(text, MapMenuItemLabel(r_u32(0x800A5660))))
            {
                name = r_u32(((uint32)((row + 24))));
                length_cursor = 1;
                if (r_u8(name))
                {
                    while (r_u8((name + (length_cursor++) * 1)))
                        ;
                }
                length = (length_cursor - 1);
                text = numbered_label;
                // TODO Original numbered-label local buffer can overflow
                if ((uint32)length + 3 > sizeof(numbered_label))
                {
                    GameDiagnostic("TODO Numbered menu label exceeds original local buffer\n");
                    abort();
                }
                CopyGameString(MapMenuItemLabel(r_u32(row + 24)), numbered_label);
                numbered_label[length] = 32;
                suffix = &numbered_label[(length + 1)];
                *suffix = (char)(index + 49);
                suffix[1] = 0;
            }
            SetHudTextScale((sint16)r_u16(((uint32)((row + 32)))));
            if ((MeasureStyledHudText(text) < 491))
                text_scale = (sint16)r_u16(((uint32)((row + 28))));
            else
                text_scale = (sint16)r_u16(((uint32)((row + 34))));
            SetHudTextScale(text_scale);
            draw_color = (uint32)(sint32)(sint16)r_u16(row + 46);
            draw_glow = (uint32)(sint32)(sint16)r_u16(row + 48);
            width = (sint32)DrawStyledHudText(r_u32(object + 12), y, text, 0, draw_color, draw_glow);
            if (((r_u8(((uint32)((object + 7)))) && (index == r_u8(((uint32)((object + 8)))))) && !(r_u8(((uint32)((object + 4)))))))
            {
                current_scale = (sint16)r_u16(row + 28);
                arrow_offset = (uint32)((sint32)((uint32)width * (uint32)current_scale) / 512) + (uint32)((sint32)((uint32)current_scale * 14) / 256);
                left_arrow = r_u32(0x800FF668);
                right_arrow = left_arrow + 20;
                if (r_u32(0x800FF374) >= left_arrow + 40)
                {
                    w_u32(0x800FF668, left_arrow + 40);
                    w_u8(((uint32)((left_arrow + 3))), 4);
                    w_u8(((uint32)((left_arrow + 7))), 32);
                    w_u8(((uint32)((right_arrow + 3))), 4);
                    w_u8(((uint32)((right_arrow + 7))), 32);
                    w_u8(left_arrow + 4, 255);
                    w_u8(left_arrow + 5, 0);
                    w_u8(((uint32)((left_arrow + 6))), 0);
                    left_center = r_u16(((uint32)((object + 12))));
                    w_u16(((uint32)((left_arrow + 10))), y);
                    w_u16(((uint32)((left_arrow + 8))), (left_center - arrow_offset));
                    w_u16(((uint32)((left_arrow + 14))), (y - 6));
                    left_edge = r_u16(((uint32)((left_arrow + 8))));
                    w_u16(((uint32)((left_arrow + 18))), (y + 6));
                    left_edge -= 20;
                    w_u16(((uint32)((left_arrow + 12))), left_edge);
                    w_u16(((uint32)((left_arrow + 16))), left_edge);
                    w_u8(right_arrow + 4, 255);
                    w_u8(right_arrow + 5, 0);
                    w_u8(((uint32)((right_arrow + 6))), 0);
                    right_center = r_u16(((uint32)((object + 12))));
                    w_u16(((uint32)((right_arrow + 10))), y);
                    w_u16(((uint32)((right_arrow + 8))), (right_center + arrow_offset));
                    w_u16(((uint32)((right_arrow + 14))), (y - 6));
                    right_edge = r_u16(((uint32)((right_arrow + 8))));
                    ordering_table = r_u32(0x800FF660);
                    w_u16(((uint32)((right_arrow + 18))), (y + 6));
                    right_edge += 20;
                    w_u16(((uint32)((right_arrow + 12))), right_edge);
                    w_u16(((uint32)((right_arrow + 16))), right_edge);
                    w_u32(((uint32)(left_arrow)), ((r_u32(((uint32)(left_arrow))) & 0xFF000000) | (r_u32(((uint32)((ordering_table + 112)))) & 0xFFFFFF)));
                    link = ((r_u32(((uint32)((ordering_table + 112)))) & 0xFF000000) | (left_arrow & 0xFFFFFF));
                    w_u32(((uint32)((ordering_table + 112))), link);
                    w_u32(((uint32)((left_arrow + 20))), ((r_u32(((uint32)((left_arrow + 20)))) & 0xFF000000) | (link & 0xFFFFFF)));
                    w_u32(((uint32)((ordering_table + 112))), ((r_u32(((uint32)((ordering_table + 112)))) & 0xFF000000) | (right_arrow & 0xFFFFFF)));
                }
            }
        }
        y += r_u32(((uint32)((object + 20))));
    LABEL_37:
        row += 28;

        ++index;
    }
}

extern "C" uint32 sub_800156E0(uint32 object)
{
    FUNCTION_MARKER(0x800156E0, "SLUS_003.73");
    return MenuWindow::Attach(object).Draw();
}

uint32 MenuWindow::Update(void)
{
    uint32 menu = NativeObjectGuestAddress(this);
    uint32 current_word, target_word;
    uint32 row, selection, count, repeat, previous, next, index;
    sint32 current, target;
    sint16 stepped;
    FUNCTION_MARKER(0x80015BA4, "SLUS_003.73");
    // TODO Original selection traversal requires at least one enabled row
    for (;;)
    {
        row = menu + 28 * r_u8(menu + 6);
        if (r_u8(row + 37))
            break;
        w_u16(row + 30, r_u16(row + 34));
        row = menu + 28 * r_u8(menu + 6);
        w_u16(row + 28, r_u16(row + 34));
        selection = r_u8(menu + 6) + 1;
        count = r_u8(menu + 10);
        w_u8(menu + 6, selection);
        if ((uint8)selection >= count)
            w_u8(menu + 6, 0);
        row = menu + 28 * r_u8(menu + 6);
        w_u16(row + 30, r_u16(row + 32));
        row = menu + 28 * r_u8(menu + 6);
        w_u16(row + 28, r_u16(row + 34));
    }
    previous = r_u8(0x800EC198);
    next = previous ? 0 : r_u8(0x800EC1A8);
    if (previous || next)
    {
        repeat = r_u32(0x800FF014);
        if (!repeat || ((sint32)repeat >= 11 && !(repeat & 1)))
        {
            if (previous)
            {
                row = menu + 28 * r_u8(menu + 6);
                w_u16(row + 30, r_u16(row + 34));
                do
                {
                    selection = r_u8(menu + 6);
                    selection = selection ? selection - 1 : r_u8(menu + 10) - 1;
                    w_u8(menu + 6, selection);
                    row = menu + 28 * r_u8(menu + 6);
                } while (!r_u8(row + 37));
                w_u16(row + 30, r_u16(row + 32));
                PlaySoundEffect(26, 0x2000, 0);
            }
            if (r_u8(0x800EC1A8))
            {
                row = menu + 28 * r_u8(menu + 6);
                w_u16(row + 30, r_u16(row + 34));
                count = r_u8(menu + 10);
                do
                {
                    selection = r_u8(menu + 6) + 1;
                    w_u8(menu + 6, selection);
                    if ((uint8)selection >= count)
                        w_u8(menu + 6, 0);
                    row = menu + 28 * r_u8(menu + 6);
                } while (!r_u8(row + 37));
                w_u16(row + 30, r_u16(row + 32));
                PlaySoundEffect(26, 0x2000, 0);
            }
        }
        repeat = r_u32(0x800FF014);
        w_u32(0x800FF014, repeat + 1);
    }
    else
        w_u32(0x800FF014, 0);
    count = r_u8(menu + 10);
    if (!count)
        return 0;
    row = menu;
    index = 0;
    for (;;)
    {
        current = (sint16)r_u16(row + 28);
        target = (sint16)r_u16(row + 30);
        current_word = r_u16(row + 28);
        if (current < target)
        {
            stepped = (sint16)(current_word + 40);
            w_u16(row + 28, (uint16)stepped);
            target = (sint16)r_u16(row + 30);
            target_word = r_u16(row + 30);
            if (target >= stepped)
                goto next_row;
            w_u16(row + 28, target_word);
        }
        current = (sint16)r_u16(row + 28);
        target = (sint16)r_u16(row + 30);
        current_word = r_u16(row + 28);
        if (target < current)
        {
            stepped = (sint16)(current_word - 40);
            w_u16(row + 28, (uint16)stepped);
            target = (sint16)r_u16(row + 30);
            target_word = r_u16(row + 30);
            if (stepped < target)
                w_u16(row + 28, target_word);
        }
    next_row:
        count = r_u8(menu + 10);
        ++index;
        row += 28;
        if ((sint32)index >= (sint32)count)
            return 0;
    }
}

extern "C" uint32 sub_80015BA4(uint32 object)
{
    FUNCTION_MARKER(0x80015BA4, "SLUS_003.73");
    return MenuWindow::Attach(object).Update();
}
