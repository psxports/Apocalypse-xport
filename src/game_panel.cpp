#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "game_panel.h"
#include "menu_window.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include "hud_text.h"
#include "hud_sprites.h"
#include "record_data.h"
#include "model_resources.h"
#include "game_heap.h"
#include "game_random.h"

uint32 LoadPanelResource()
{
    uint32 resource;
    FUNCTION_MARKER(0x8006CDA0, "SLUS_003.73");
    resource = FindModelTextureGroup(0x800FF760);
    w_u32(0x800FF75C, resource);
    return resource;
}

uint32 ResetPanelDisplayTimer()
{
    FUNCTION_MARKER(0x8006CDC4, "SLUS_003.73");
    w_u32(0x800FF758, 150);
    return 150;
}

extern "C" uint32 sub_8006CDA0(void)
{
    FUNCTION_MARKER(0x8006CDA0, "SLUS_003.73");
    return LoadPanelResource();
}
extern "C" uint32 sub_8006CDC4(void)
{
    FUNCTION_MARKER(0x8006CDC4, "SLUS_003.73");
    return ResetPanelDisplayTimer();
}

uint32 DrawDebugPanel()
{
    uint32 enabled = r_u32(0x800FF33C), text = 0x800FF768;
    uint32 trigger, player, numerator, denominator, percentage, position[3], end, start;
    sint32 high;
    FUNCTION_MARKER(0x8006CDD4, "SLUS_003.73");
    if (!enabled)
        return enabled;
    trigger = r_u32(0x800FF620);
    if (trigger != 0xFFFF && (sint16)r_u16(r_u32(r_u32(0x800FF624) + trigger * 4)) == 8)
        text = ReadRecordPosition(position, trigger) + 6;
    QueueHudText(24, 193, text, 32, 32, 114, 1);
    player = r_u32(0x800FF5A0);
    QueueHudNumber(23, 205, (uint32)((sint32)r_u32(player + 4) >> 12), 32, 32, 114, 1);
    player = r_u32(0x800FF5A0);
    QueueHudNumber(90, 205, (uint32)((sint32)r_u32(player + 8) >> 12), 32, 32, 114, 1);
    player = r_u32(0x800FF5A0);
    QueueHudNumber(152, 205, (uint32)((sint32)r_u32(player + 12) >> 12), 32, 32, 114, 1);
    numerator = r_u32(0x800FF34C) * 100;
    high = (sint32)(((sint64)(sint32)numerator * (sint64)0x4BDA12F7) >> 32);
    percentage = (uint32)(high >> 15) - (uint32)((sint32)numerator >> 31);
    QueueHudNumber(24, 217, percentage, 32, 32, 114, 1);
    numerator = r_u32(0x800FF748) * 100;
    end = r_u32(0x800E6C0C);
    start = r_u32(0x800E6C08);
    denominator = end - start;
    percentage = denominator ? numerator / denominator : 0xFFFFFFFF;
    QueueHudNumber(49, 217, percentage, 32, 32, 114, 1);
    player = r_u32(0x800FF5A0);
    if (player)
        QueueHudNumber(74, 217, r_u32(player + 644), 32, 32, 114, 1);
    QueueHudNumber(109, 217, r_u32(0x800FF700), 32, 32, 114, 1);
    return QueueHudNumber(127, 217, r_u32(0x800FF2B8), 32, 32, 114, 1);
}

extern "C" uint32 sub_8006CDD4(void)
{
    FUNCTION_MARKER(0x8006CDD4, "SLUS_003.73");
    return DrawDebugPanel();
}

// TODO Establish original receiver ownership before making this an instance method
uint32 TrackObjectHealth(uint32 object, uint32 x, uint32 y, uint32 scale)
{
    uint32 health;
    FUNCTION_MARKER(0x8006D390, "SLUS_003.73");
    health = (uint32)(sint32)(sint16)r_u16(object + 218);
    w_u32(0x800FF76C, object);
    w_u32(0x800FFC50, x);
    w_u32(0x800FFC54, y);
    w_u32(0x800FFC5C, scale);
    w_u32(0x800FFC58, health);
    w_u32(0x800FFC4C, health);
    return health;
}

void ClearTrackedObjectHealth()
{
    FUNCTION_MARKER(0x8006D3B4, "SLUS_003.73");
    w_u32(0x800FF76C, 0);
}

extern "C" uint32 sub_8006D390(uint32 object, uint32 first, uint32 second, uint32 scale)
{
    FUNCTION_MARKER(0x8006D390, "SLUS_003.73");
    return TrackObjectHealth(object, first, second, scale);
}
extern "C" void sub_8006D3B4(void)
{
    FUNCTION_MARKER(0x8006D3B4, "SLUS_003.73");
    ClearTrackedObjectHealth();
}

void DrawTrackedObjectHealth()
{
    uint32 object, health, packet, cursor, next, limit, flash, scale, product;
    uint32 x, y, right, top, bottom, left, buffer, tag, head, opcode, initial_health;
    FUNCTION_MARKER(0x8006D3C0, "SLUS_003.73");
    object = r_u32(0x800FF76C);
    if (!object)
        return;
    if (r_u16(object + 78) & 64)
    {
        ClearTrackedObjectHealth();
        return;
    }
    health = (uint32)(sint32)(sint16)r_u16(object + 218);
    if ((sint32)health < 0)
        health = 0;
    if ((sint32)health < (sint32)r_u32(0x800FFC4C))
    {
        w_u32(0x800FFC4C, health);
        w_u32(0x800FF770, 8);
    }
    packet = r_u32(0x800FF668);
    limit = r_u32(0x800FF374);
    next = packet + 24;
    if (limit < next)
        return;
    w_u32(0x800FF668, next);
    w_u8(packet + 3, 5);
    w_u8(packet + 7, 40);
    w_u8(packet + 4, 128);
    flash = r_u8(0x800FF770);
    w_u8(packet + 5, flash << 4);
    flash = r_u8(0x800FF770);
    w_u8(packet + 6, flash << 4);
    scale = r_u32(0x800FFC5C);
    x = r_u16(0x800FFC50);
    product = (uint32)((sint32)health >> 2) * scale;
    opcode = r_u8(packet + 7);
    y = r_u16(0x800FFC54);
    w_u8(packet + 7, opcode | 2);
    buffer = r_u32(0x800FF660);
    w_u16(packet + 8, x);
    w_u16(packet + 16, x);
    w_u16(packet + 10, y);
    w_u16(packet + 14, y);
    y += 6;
    w_u16(packet + 18, y);
    w_u16(packet + 22, y);
    tag = r_u32(packet);
    cursor = r_u32(0x800FF668);
    next = cursor + 8;
    right = x + (uint32)((sint32)product >> 12);
    w_u16(packet + 12, right);
    w_u16(packet + 20, right);
    head = r_u32(buffer + 112);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(buffer + 112);
    limit = r_u32(0x800FF374);
    w_u32(buffer + 112, (head & 0xFF000000) | (packet & 0xFFFFFF));
    if (limit < next)
        return;
    w_u32(0x800FF668, next);
    w_u8(cursor + 3, 1);
    buffer = r_u32(0x800FF660);
    w_u32(cursor + 4, 0xE1000220);
    tag = r_u32(cursor);
    flash = r_u32(0x800FF770);
    head = r_u32(buffer + 112);
    w_u32(cursor, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(buffer + 112);
    w_u32(buffer + 112, (head & 0xFF000000) | (cursor & 0xFFFFFF));
    if (flash)
        w_u32(0x800FF770, flash - 1);
    packet = r_u32(0x800FF668);
    limit = r_u32(0x800FF374);
    next = packet + 28;
    if (limit < next)
        return;
    w_u32(0x800FF668, next);
    w_u8(packet + 3, 6);
    w_u8(packet + 7, 76);
    w_u8(packet + 4, 240);
    w_u8(packet + 5, 0);
    w_u8(packet + 6, 0);
    initial_health = r_u32(0x800FFC58);
    w_u32(packet + 24, 0x55555555);
    scale = r_u32(0x800FFC5C);
    y = r_u16(0x800FFC54);
    cursor = r_u32(0x800FF668);
    product = (uint32)((sint32)initial_health >> 2) * scale;
    top = y - 1;
    bottom = y + 6;
    x = r_u16(0x800FFC50);
    next = cursor + 16;
    w_u16(packet + 18, bottom);
    w_u16(packet + 22, bottom);
    w_u16(packet + 10, top);
    w_u16(packet + 14, top);
    left = x - 1;
    w_u16(packet + 8, left);
    w_u16(packet + 20, left);
    buffer = r_u32(0x800FF660);
    right = x + (uint32)((sint32)product >> 12) + 1;
    w_u16(packet + 12, right);
    w_u16(packet + 16, right);
    tag = r_u32(packet);
    head = r_u32(buffer + 112);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(buffer + 112);
    limit = r_u32(0x800FF374);
    w_u32(buffer + 112, (head & 0xFF000000) | (packet & 0xFFFFFF));
    if (limit < next)
        return;
    w_u32(0x800FF668, next);
    w_u8(cursor + 3, 3);
    w_u8(cursor + 7, 64);
    w_u8(cursor + 4, 240);
    w_u8(cursor + 5, 0);
    w_u8(cursor + 6, 0);
    y = r_u16(0x800FFC54);
    x = r_u16(0x800FFC50);
    top = y - 1;
    bottom = y + 6;
    w_u16(cursor + 14, bottom);
    buffer = r_u32(0x800FF660);
    left = x - 1;
    w_u16(cursor + 8, left);
    w_u16(cursor + 12, left);
    tag = r_u32(cursor);
    w_u16(cursor + 10, top);
    head = r_u32(buffer + 112);
    w_u32(cursor, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(buffer + 112);
    w_u32(buffer + 112, (head & 0xFF000000) | (cursor & 0xFFFFFF));
}

extern "C" void sub_8006D3C0(void)
{
    FUNCTION_MARKER(0x8006D3C0, "SLUS_003.73");
    DrawTrackedObjectHealth();
}

void DrawWeaponAmmoIndicator(uint32 slot)
{
    static const uint32 xs[8] = {0, 414, 424, 439, 449, 454, 454, 449};
    static const uint32 ys[8] = {0, 37, 32, 32, 37, 43, 49, 55};
    uint32 player, weapon, packet, bar, next, limit, ammo, extent, x, y, head, buffer, tag;
    sint32 high;
    FUNCTION_MARKER(0x8006D704, "SLUS_003.73");
    player = r_u32(0x800FF5A0);
    if (!player)
        return;
    // Original reads the slot before checking its range
    weapon = r_u32(player + slot * 4 + 612);
    if (!weapon || slot >= 8 || !slot)
        return;
    x = xs[slot];
    y = ys[slot];
    packet = r_u32(0x800FF668);
    limit = r_u32(0x800FF374);
    next = packet + 32;
    bar = packet + 16;
    if (limit < next)
        return;
    ammo = r_u32(weapon + 56);
    high = (sint32)(((sint64)(sint32)ammo * (sint64)0x10624DD3) >> 32);
    w_u32(0x800FF668, next);
    extent = (uint32)(high >> 3) - (uint32)((sint32)ammo >> 31);
    if (extent)
    {
        w_u8(bar + 3, 3);
        w_u8(bar + 7, 64);
        w_u8(bar + 5, 255);
        w_u8(bar + 4, 0);
        w_u8(bar + 6, 0);
        buffer = r_u32(0x800FF660);
        w_u16(bar + 8, x);
        w_u16(bar + 10, y);
        w_u16(bar + 12, x + extent - 1);
        w_u16(bar + 14, y);
        tag = r_u32(bar);
        head = r_u32(buffer + 112);
        w_u32(bar, (tag & 0xFF000000) | (head & 0xFFFFFF));
        head = r_u32(buffer + 112);
        w_u32(buffer + 112, (head & 0xFF000000) | (bar & 0xFFFFFF));
    }
    w_u8(packet + 3, 3);
    w_u8(packet + 7, 64);
    w_u8(packet + 5, 64);
    w_u8(packet + 4, 0);
    w_u8(packet + 6, 0);
    buffer = r_u32(0x800FF660);
    tag = r_u32(packet);
    w_u16(packet + 8, x);
    w_u16(packet + 10, y);
    w_u16(packet + 12, x + 7);
    w_u16(packet + 14, y);
    head = r_u32(buffer + 112);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(buffer + 112);
    w_u32(buffer + 112, (head & 0xFF000000) | (packet & 0xFFFFFF));
    if (r_u32(player + 644) == slot)
        AllocateHudRectangle(x - 3, y - 1, 13, 4, 128, 128, 128, 0);
}

extern "C" void sub_8006D704(uint32 slot)
{
    FUNCTION_MARKER(0x8006D704, "SLUS_003.73");
    DrawWeaponAmmoIndicator(slot);
}

void DrawGamePanel()
{
    uint32 timer, texture, text, value, quotient, flag, player, weapon, kind;
    uint32 x, y, percentage, width, product, height, green, index;
    sint32 count, maximum, numerator, sine;
    uint8 *packet;
    FUNCTION_MARKER(0x8006D91C, "SLUS_003.73");
    timer = r_u32(0x800FF758);
    if (timer)
    {
        texture = r_u32(0x800FF75C);
        w_u32(0x800FF758, timer - 1);
        packet = AllocateHudSprite((const uint8 *)psx_addr(texture + 8, 12));
        if (packet)
            PlaceHudSprite(40, 27, packet, (const uint8 *)psx_addr(r_u32(0x800FF75C) + 8, 12));
        if (r_u32(0x800FF758))
        {
            if (r_u32(0x800FF320))
                text = r_u32(0x800A558C);
            else
            {
                value = r_u32(0x800FF874) - 1;
                quotient = (uint32)((sint32)value / 10);
                text = 0x800FF774;
                w_u8(text, quotient ? quotient + 48 : 32);
                w_u8(text + 1, value - quotient * 10 + 48);
            }
            SetHudTextScale(256);
            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(1);
            DrawStyledHudText(65, 27, (const char *)psx_addr(text, 1), 0, 0, 256);
        }
    }
    if (r_u32(0x800FF008))
        return;
    flag = r_u32(0x800FF304);
    player = r_u32(0x800FF5A0);
    if (flag)
    {
        SetHudTextScale(256);
        SetHudTextColor(128, 128, 128, 0);
        DrawStyledHudText(58, 50, (const char *)psx_addr(r_u32(0x800A556C), 1), 0, 0, 256);
    }
    if (!r_u32(0x800FF324))
        return;
    count = (sint32)r_u32(player + 532);
    if (count >= 16)
        count = 15;
    x = 412;
    for (sint32 item = 0; item < count && item < 8; ++item)
    {
        packet = AllocateHudSprite((const uint8 *)psx_addr(r_u32(0x800FF75C) + 80, 12));
        if (packet)
            PlaceScaledHudSprite(x, 25, packet, (const uint8 *)psx_addr(r_u32(0x800FF75C) + 80, 12));
        x += 8;
    }
    if (count >= 9)
    {
        x -= 8;
        y = 30;
        count -= 8;
        for (sint32 item = 0; item < count; ++item)
        {
            packet = AllocateHudSprite((const uint8 *)psx_addr(r_u32(0x800FF75C) + 80, 12));
            if (packet)
                PlaceScaledHudSprite(x, y, packet, (const uint8 *)psx_addr(r_u32(0x800FF75C) + 80, 12));
            y += 5;
        }
    }
    DrawWeaponAmmoIndicator(1);
    DrawWeaponAmmoIndicator(2);
    DrawWeaponAmmoIndicator(3);
    DrawWeaponAmmoIndicator(4);
    DrawWeaponAmmoIndicator(5);
    DrawWeaponAmmoIndicator(6);
    DrawWeaponAmmoIndicator(7);
    packet = AllocateHudSprite((const uint8 *)psx_addr(r_u32(0x800FF75C), 12));
    if (packet)
    {
        PlaceScaledHudSprite(436, 46, packet, (const uint8 *)psx_addr(r_u32(0x800FF75C), 12));
        packet[6] = 90;
        packet[5] = 90;
        packet[4] = 90;
    }
    player = r_u32(0x800FF5A0);
    if (player)
    {
        weapon = r_u32(player + r_u32(player + 644) * 4 + 612);
        kind = r_u32(weapon + 4);
        texture = r_u32(0x800FF75C);
        switch (kind)
        {
            case 1: texture += 72; break;
            case 2: texture += 24; break;
            case 3: texture += 40; break;
            case 4: texture += 48; break;
            case 5: texture += 32; break;
            case 6: texture += 64; break;
            case 7: texture += 56; break;
            case 8:
                texture = 0;
                AllocateHudRectangle(425, 39, 22, 14, 40, 40, 40, 0);
                break;
            case 9: texture = 0; break;
            default: break;
        }
        if (texture)
        {
            packet = AllocateHudSprite((const uint8 *)psx_addr(texture, 12));
            if (packet)
                PlaceScaledHudSprite(436, 46, packet, (const uint8 *)psx_addr(texture, 12));
        }
    }
    // TODO Establish the original invariant requiring a player here
    maximum = (sint16)r_u16(0x800EC526);
    numerator = (sint32)(((uint32)maximum - (uint32)(sint32)(sint16)r_u16(player + 218)) * 100);
    if (!maximum)
        percentage = numerator < 0 ? 1 : 0xFFFFFFFF;
    else if (numerator == (sint32)0x80000000 && maximum == -1)
        percentage = (uint32)numerator;
    else
        percentage = (uint32)(numerator / maximum);
    product = percentage * 80;
    width = (uint32)((sint32)product / 100);
    AllocateHudRectangle(469 - width, 16, width, 6, 0, 0, 0, 0);
    if ((sint32)width < 41)
        QueueHudGradientRectangle(429, 16, 40, 6, 0, 255, 0, 255, 255, 0, 0);
    if ((sint32)percentage >= 61)
    {
        product = (percentage - 60) * 350;
        value = (uint32)((sint32)product / 40);
        index = ((value + 50) * r_u32(0x800FF2F0)) & 4095;
        sine = (sint16)r_u16(0x800F863C + index * 4);
        product = (uint32)sine * (uint32)sine;
        green = (uint32)((sint32)product >> 16);
        if ((sint32)green >= 256)
            green = 255;
        green &= 255;
        QueueHudGradientRectangle(389, 16, 40, 6, 255, green, 0, 255, green, 0, 0);
    }
    else
        QueueHudGradientRectangle(389, 16, 40, 6, 255, 255, 0, 255, 0, 0, 0);
    weapon = r_u32(player + r_u32(player + 644) * 4 + 612);
    if (weapon)
    {
        if (r_u32(weapon + 4) == 8)
        {
            y = 25;
            height = 50;
        }
        else
        {
            value = 1000 - r_u32(weapon + 56);
            height = (uint32)((sint32)value / 20);
            AllocateHudRectangle(474, 25, 10, height, 0, 0, 0, 0);
            y = height + 25;
            height = 50 - height;
        }
        AllocateHudRectangle(474, y, 10, height, 0, 100, 200, 0);
    }
    DrawTrackedObjectHealth();
}

extern "C" void sub_8006D91C(void)
{
    FUNCTION_MARKER(0x8006D91C, "SLUS_003.73");
    DrawGamePanel();
}

static uint32 DrawPauseMenuText(uint32 x, uint32 y, uint32 text, uint32 filter, uint32 color, uint32 scale)
{
    return DrawStyledHudText(x, y, (const char *)psx_addr(text, 1), filter, color, scale);
}

uint32 DrawPauseMenuHeading(void)
{
    FUNCTION_MARKER(0x8001664C, "SLUS_003.73");
    uint32 result;
    SetHudTextScale(512);
    SetHudTextColor(149, 20, 20, 0);
    result = DrawPauseMenuText(256, 34, r_u32(0x800A5548), 0, 0, 256);
    SetHudTextScale(256);
    return result;
}

uint32 DrawPauseMenuDisplay(void)
{
    FUNCTION_MARKER(0x80016884, "SLUS_003.73");
    sint32 result;
    uint32 player, label, text_y, amplitude, phase, countdown, text_count;
    sint32 v1;
    sint32 v2;
    sint32 v3;
    uint32 v4;
    sint32 v5;
    sint32 i;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    uint32 v12;
    uint32 v13;
    sint32 v14;
    char v15[104];
    if (r_u32(0x800FF33C))
        /* MIPS 800168B0..800168C0 stores zero for all four stack arguments */
        sub_8006D1C0(19, 190, 194, 42, 0, 0, 0, 0);
    if (r_u32(0x800FF008))
    {
        nullsub_19();
        sub_8001A8E8();
    }
    SetHudTextAlignment(0);
    if ((r_u32(0x800FF81C) && ((r_u32(0x800FF64C) & 0x20) != 0)))
    {
        SetHudTextColor(128, 128, 128, 0);
        SetHudTextScale(256);
        DrawPauseMenuText(256, 35, r_u32((0x800A556C + (0) * 4)), 0, 0, 256);
    }
    player = r_u32(0x800FF5A0);
    if (player && (sint16)r_u16(player + 218) <= 0 && r_u32(0x800FF874) == 1)
    {
        if (!sub_8002E13C())
            sub_8002E148(951706923, 64, 64, 127);
        SetHudTextColor(128, 128, 128, 0);
        SetHudTextScale(384);
        label = r_u32(0x800A54E8);
        text_y = (uint32)(sint32)(sint16)r_u16(0x800FF038);
        DrawPauseMenuText(256, text_y, label, 0, 128, 256);
        text_y = r_u16(0x800FF038) + 8;
        w_u16(0x800FF038, text_y);
        if ((sint16)text_y >= 121)
            w_u16(0x800FF038, 120);
    }
    SetHudTextScale(256);
    result = (sint32)0x800A0000;
    switch (r_u32(0x800FF080))
    {
        case 1:
            DrawPauseMenuHeading();
            amplitude = r_u32(0x800FF00C);
            v1 = (sint32)(amplitude - 2);
            if (amplitude)
            {
                phase = r_u32(0x800FF010);
                w_u32(0x800FF00C, (uint32)v1);
                w_u32(0x800FF010, phase + 690);
                if (v1 < 0)
                {
                    w_u32(0x800FF00C, 0);
                    w_u32(0x800FF010, 0);
                }
            }
            phase = r_u32(0x800FF010);
            {
                uint32 cosine = (uint32)(sint32)(sint16)r_u16(0x800F863E + (phase & 4095) * 4);
                uint32 product = r_u32(0x800FF00C) * cosine;
                v2 = r_u32(0x800FF084);
                if ((sint32)product < 0)
                    product += 4095;
                w_u32((uint32)v2 + 12, (uint32)((sint32)product >> 12) + 256);
            }
            MenuWindow::Attach((uint32)v2).Draw();
            if (r_u32(0x800FF33C))
                return DrawMenuRightHint();
            else
                return DrawMenuCenteredHint();

        case 2:
            v3 = r_u32(0x800FF088);
            if (v3)
                return MenuWindow::Attach((uint32)v3).Draw();
            return 0x80016B10;

        case 7:
        case 8:
            return 0x80017350;

        case 3:
            DrawPauseMenuHeading();
            DrawMenuHints();
            SetHudTextColor(149, 20, 20, 0);
            DrawPauseMenuText(256, 95, r_u32((0x800A559C + (0) * 4)), 0, 0, 256);
            v3 = r_u32(0x800FF024);
            w_u32((uint32)v3 + 16, 135);
            return MenuWindow::Attach((uint32)v3).Draw();

        case 4:
            DrawPauseMenuHeading();
            SetMenuRedText();
            result = ((((sint32)r_u32(0x800FF2F0)) / 10) & 1);
            if (!result)
                return result;
            DrawPauseMenuText(256, 171, r_u32((0x800A5564 + (0) * 4)), 0, 0, 256);
            v4 = r_u32((0x800A5568 + (0) * 4));
            v5 = 197;
            goto LABEL_57;

        case 5:
            // TODO: Original text buffer capacity is 104 bytes
            text_count = r_u32(0x800FF034);
            if ((sint32)text_count < 0 || text_count >= sizeof(v15))
            {
                GameDiagnostic("Menu text length exceeds original 104-byte buffer\n");
                abort();
            }
            i = 0;
            if ((sint32)text_count > 0)
            {
                do
                {
                    if ((uint32)i >= sizeof(v15) - 1)
                    {
                        GameDiagnostic("Menu text grew beyond original buffer\n");
                        abort();
                    }
                    v15[i] = (char)r_u8(r_u32(0x800FF030) + (uint32)i);
                    ++i;
                    text_count = r_u32(0x800FF034);
                } while (i < (sint32)text_count);
            }
            text_count = r_u32(0x800FF034);
            if (text_count >= sizeof(v15))
            {
                GameDiagnostic("Menu text terminator exceeds original buffer\n");
                abort();
            }
            v15[text_count] = 0;
            SetHudTextScale(192);
            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(1);
            v7 = MeasureStyledHudText((const char *)psx_addr(r_u32(0x800FF030), 1));
            return DrawStyledHudText(256 - (uint32)((sint32)v7 >> 1), 34, v15, 0, 0, 256);

        case 6:
            goto LABEL_31;

        case 9:
            SetHudTextColor(149, 20, 20, 0);
            DrawPauseMenuText(256, 103, r_u32((0x800A5550 + (0) * 4)), 0, 0, 256);
            v8 = r_u32(0x800FF024);
            w_u32((uint32)v8 + 16, 143);
            MenuWindow::Attach((uint32)v8).Draw();
            DrawMenuCenteredHint();
        LABEL_31:
            SetHudTextScale(192);

            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(0);
            DrawPauseMenuText(256, 34, r_u32(0x800FF030), 0, 0, 256);
            result = r_u32(0x800FF040);
            if (!result)
                return result;
            SetHudTextScale(256);
            SetHudTextAlignment(1);
            DrawPauseMenuText(166, 68, r_u32((0x800A5558 + (0) * 4)), 0, 0, 256);
            result = 100;
            v9 = 1;
            if ((r_u32(0x800FFB58) == 100))
            {
                countdown = r_u32(0x800FF044);
                if (countdown)
                    w_u32(0x800FF044, countdown - 1);
                result = ((r_u32(0x800FF044) >> 2) & 1);
                if (result)
                    v9 = 0;
            }
            if (!v9)
                return result;
            snprintf((char *)psx_addr(0x800FFD30, 1), PSX_DRAM_SIZE - 0xFFD30, (const char *)psx_addr(0x800FF08C, 1), r_u32(0x800FFB58));
            v10 = 270;
            v11 = 68;
            v12 = 0x800FFD30;
            goto LABEL_48;

        case 10:

        case 11:
            SetMenuGreenText();
            DrawPauseMenuText(256, 90, r_u32((0x800A56B4 + (0) * 4)), 0, 0, 256);
            SetMenuRedText();
            DrawPauseMenuText(256, 140, r_u32((0x800A56AC + (0) * 4)), 0, 0, 256);
            v4 = r_u32((0x800A56B0 + (0) * 4));
            v5 = 166;
            goto LABEL_57;

        case 12:
            DrawMenuHints();
            SetMenuRedText();
            DrawPauseMenuText(256, 25, r_u32((0x800A5554 + (0) * 4)), 0, 0, 256);
            v3 = r_u32(0x800FF028);
            return MenuWindow::Attach((uint32)v3).Draw();

        case 13:
            DrawMenuHints();
            SetMenuGreenText();
            DrawPauseMenuText(256, 40, r_u32((0x800A56DC + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 66, r_u32((0x800A56E0 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 92, r_u32((0x800A56E4 + (0) * 4)), 0, 0, 256);
            v3 = r_u32(0x800FF024);
            return MenuWindow::Attach((uint32)v3).Draw();

        case 14:
            SetMenuGreenText();
            DrawPauseMenuText(256, 110, r_u32((0x800A56E8 + (0) * 4)), 0, 0, 256);
            SetMenuRedText();
            DrawPauseMenuText(256, 140, r_u32((0x800A56AC + (0) * 4)), 0, 0, 256);
            v4 = r_u32((0x800A56B0 + (0) * 4));
            v5 = 166;
            goto LABEL_57;

        case 15:
            SetMenuRedText();
            DrawPauseMenuText(256, 110, r_u32((0x800A56EC + (0) * 4)), 0, 0, 256);
            v14 = 136;
            v13 = r_u32((0x800A56F0 + (0) * 4));
            goto LABEL_55;

        case 16:
            SetMenuGreenText();
            DrawPauseMenuText(256, 110, r_u32((0x800A5704 + (0) * 4)), 0, 0, 256);
            v5 = 140;
            goto LABEL_56;

        case 17:
            SetMenuRedText();
            SetHudTextScale(192);
            DrawPauseMenuText(256, 67, r_u32((0x800A56BC + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 85, r_u32((0x800A56C0 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 103, r_u32((0x800A56C4 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 121, r_u32((0x800A56C8 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 139, r_u32((0x800A56CC + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 157, r_u32((0x800A56D0 + (0) * 4)), 0, 0, 256);
            SetHudTextScale(256);
            v5 = 188;
            goto LABEL_56;

        case 18:
            SetMenuRedText();
            DrawPauseMenuText(256, 90, r_u32((0x800A5664 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 116, r_u32((0x800A5668 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 142, r_u32((0x800A566C + (0) * 4)), 0, 0, 256);
            v4 = r_u32((0x800A5670 + (0) * 4));
            v5 = 168;
            goto LABEL_57;

        case 19:
            DrawMenuHints();
            SetMenuRedText();
            DrawPauseMenuText(256, 35, r_u32((0x800A5688 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 61, r_u32((0x800A568C + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 87, r_u32((0x800A5690 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 113, r_u32(0x800A5694), 0, 0, 256);
            v3 = r_u32(0x800FF024);
            w_u32((uint32)v3 + 16, 148);
            return MenuWindow::Attach((uint32)v3).Draw();

        case 20:
            SetMenuGreenText();
            if ((((((sint32)r_u32(0x800FF2F0)) / 10) & 1) != 0))
                DrawPauseMenuText(256, 110, r_u32((0x800A56A8 + (0) * 4)), 0, 0, 256);
            SetMenuRedText();
            DrawPauseMenuText(256, 140, r_u32((0x800A56AC + (0) * 4)), 0, 0, 256);
            v4 = r_u32((0x800A56B0 + (0) * 4));
            v5 = 166;
            goto LABEL_57;

        case 21:
            SetMenuRedText();
            DrawPauseMenuText(256, 90, r_u32((0x800A56F4 + (0) * 4)), 0, 0, 256);
            DrawPauseMenuText(256, 116, r_u32((0x800A56F8 + (0) * 4)), 0, 0, 256);
            v13 = r_u32((0x800A56FC + (0) * 4));
            v14 = 142;
        LABEL_55:
            DrawPauseMenuText(256, v14, v13, 0, 0, 256);

            v5 = 170;
        LABEL_56:
            v4 = r_u32(0x800A5590);

        LABEL_57:
            result = DrawPauseMenuText(256, v5, v4, 0, 0, 256);

            break;

        case 22:
            SetMenuGreenText();
            if ((((((sint32)r_u32(0x800FF2F0)) / 10) & 1) != 0))
                DrawPauseMenuText(256, 110, r_u32((0x800A5700 + (0) * 4)), 0, 0, 256);
            v10 = 256;
            v11 = 140;
            v12 = ((uint32)(r_u32(0x800A5590)));
        LABEL_48:
            result = DrawPauseMenuText(v10, v11, v12, 0, 0, 256);

            break;

        default:
            return result;
    }

    return result;
}

extern "C" uint32 sub_8001664C(void)
{
    FUNCTION_MARKER(0x8001664C, "SLUS_003.73");
    return DrawPauseMenuHeading();
}
extern "C" uint32 sub_80016884(void)
{
    FUNCTION_MARKER(0x80016884, "SLUS_003.73");
    return DrawPauseMenuDisplay();
}

uint32 SetMenuRedText(void)
{
    FUNCTION_MARKER(0x800151D0, "SLUS_003.73");
    return SetHudTextColor(255, 0, 0, 0);
}

uint32 SetMenuGreenText(void)
{
    FUNCTION_MARKER(0x800151FC, "SLUS_003.73");
    return SetHudTextColor(0, 255, 0, 0);
}

uint32 DrawMenuLeftHint(void)
{
    FUNCTION_MARKER(0x800166AC, "SLUS_003.73");
    SetHudTextAlignment(1);
    SetHudTextScale(192);
    SetHudTextColor(149, 20, 20, 0);
    DrawPauseMenuText(92, 210, r_u32(0x800A5574), 0, 0, 256);
    SetHudTextAlignment(0);
    uint8 *packet = AllocateHudSprite((const uint8 *)psx_addr(r_u32(0x800FF01C) + 8, 8));
    uint32 result = packet ? GuestMemoryAddress(packet) : 0;
    if (packet)
        result = PlaceScaledHudSprite(64, 211, packet, (const uint8 *)psx_addr(r_u32(0x800FF01C) + 8, 8));
    SetHudTextScale(256);
    return result;
}

uint32 DrawMenuRightHint(void)
{
    FUNCTION_MARKER(0x80016744, "SLUS_003.73");
    SetHudTextAlignment(1);
    SetHudTextScale(192);
    SetHudTextColor(149, 20, 20, 0);
    DrawPauseMenuText(348, 210, r_u32(0x800A5578), 0, 0, 256);
    SetHudTextAlignment(0);
    uint8 *packet = AllocateHudSprite((const uint8 *)psx_addr(r_u32(0x800FF01C), 8));
    uint32 result = packet ? GuestMemoryAddress(packet) : 0;
    if (packet)
        result = PlaceScaledHudSprite(320, 211, packet, (const uint8 *)psx_addr(r_u32(0x800FF01C), 8));
    SetHudTextScale(256);
    return result;
}

uint32 DrawMenuHints(void)
{
    FUNCTION_MARKER(0x800167D8, "SLUS_003.73");
    DrawMenuLeftHint();
    return DrawMenuRightHint();
}

uint32 DrawMenuCenteredHint(void)
{
    FUNCTION_MARKER(0x80016800, "SLUS_003.73");
    SetHudTextAlignment(1);
    SetHudTextScale(256);
    SetHudTextColor(149, 20, 20, 0);
    DrawPauseMenuText(227, 213, r_u32(0x800A5578), 0, 0, 256);
    uint8 *packet = AllocateHudSprite((const uint8 *)psx_addr(r_u32(0x800FF01C), 8));
    if (packet)
        return PlaceScaledHudSprite(199, 214, packet, (const uint8 *)psx_addr(r_u32(0x800FF01C), 8));
    return 0;
}

extern "C" uint32 sub_800151D0(void)
{
    FUNCTION_MARKER(0x800151D0, "SLUS_003.73");
    return SetMenuRedText();
}
extern "C" uint32 sub_800151FC(void)
{
    FUNCTION_MARKER(0x800151FC, "SLUS_003.73");
    return SetMenuGreenText();
}
extern "C" uint32 sub_800166AC(void)
{
    FUNCTION_MARKER(0x800166AC, "SLUS_003.73");
    return DrawMenuLeftHint();
}
extern "C" uint32 sub_80016744(void)
{
    FUNCTION_MARKER(0x80016744, "SLUS_003.73");
    return DrawMenuRightHint();
}
extern "C" uint32 sub_800167D8(void)
{
    FUNCTION_MARKER(0x800167D8, "SLUS_003.73");
    return DrawMenuHints();
}
extern "C" uint32 sub_80016800(void)
{
    FUNCTION_MARKER(0x80016800, "SLUS_003.73");
    return DrawMenuCenteredHint();
}

uint32 BlurMenuBuffers(void)
{
    FUNCTION_MARKER(0x8002E2E8, "SLUS_003.73");
    uint32 selected, other, toggle;
    sint32 row;
    selected = r_u32(0x800FF230);
    if (!selected)
        return selected;
    toggle = r_u8(0x800FF238) == 0;
    w_u8(0x800FF238, toggle);
    selected = r_u32(0x800FF230 + 4 * toggle);
    other = r_u32(0x800FF230 + 4 * (toggle == 0));
    for (row = 1; row < (sint32)r_u16(0x800FF23C) - 1; ++row)
    {
        uint32 stride = r_u16(0x800FF23A);
        uint32 top = r_u32(selected + (uint32)(row - 1) * stride);
        uint32 bottom = r_u32(selected + (uint32)(row + 1) * stride);
        uint32 carry = 0;
        sint32 column = 1;
        for (;;)
        {
            uint32 top_second = (top >> 16) & 255;
            uint32 bottom_second = (bottom >> 16) & 255;
            uint32 top_last = top >> 24;
            uint32 bottom_last = bottom >> 24;
            uint32 output = carry;
            uint32 next_top, next_bottom;
            sint32 next_column = column + 2;
            uint32 top_offset;
            output |= (((top & 255) + top_second + (bottom & 255) + bottom_second) >> 2) << 8;
            output |= ((((top >> 8) & 255) + top_last + ((bottom >> 8) & 255) + bottom_last) >> 2) << 16;
            stride = r_u16(0x800FF23A);
            top_offset = (uint32)(row - 1) * stride;
            if (next_column >= (sint32)stride - 1)
                break;
            next_top = r_u32(selected + top_offset + (uint32)next_column + 1);
            next_bottom = r_u32(selected + (uint32)(row + 1) * stride + (uint32)next_column + 1);
            output |= ((top_second + (next_top & 255) + bottom_second + (next_bottom & 255)) >> 2) << 24;
            w_u32(selected + (uint32)row * stride + (uint32)next_column - 3, output);
            w_u32(other + top_offset + (uint32)next_column - 3, output);
            carry = (top_last + ((next_top >> 8) & 255) + bottom_last + (next_bottom >> 24)) >> 2;
            top = next_top;
            bottom = next_bottom;
            column = next_column + 2;
        }
    }
    SeedMenuFire(4);
    return xport_draft_host_sub_800885A4_p1((const sint16 *)psx_addr(0x800FF240, 8), selected);
}

extern "C" uint32 sub_8002E2E8(void)
{
    FUNCTION_MARKER(0x8002E2E8, "SLUS_003.73");
    return BlurMenuBuffers();
}

uint32 SeedMenuFire(uint32 sparks)
{
    FUNCTION_MARKER(0x8002E4D8, "SLUS_003.73");
    uint32 height = r_u16(0x800FF23C);
    uint32 width = r_u16(0x800FF23A);
    uint32 row_offset = (height - 1) * width;
    uint32 selected = r_u8(0x800FF238);
    uint32 buffer = r_u32(0x800FF230 + selected * 4);
    uint32 cursor = buffer + row_offset;
    uint32 words = width >> 2;
    if (words)
    {
        uint32 index = 0;
        do
        {
            w_u32(cursor, 0);
            width = r_u16(0x800FF23A);
            ++index;
            words = width >> 2;
            cursor += 4;
        } while (index < words);
    }
    uint32 result = RandomRange(r_u8(0x800FF23E) >> 1);
    uint32 intensity = (r_u8(0x800FF23E) >> 1) + result;
    while (sparks)
    {
        result = RandomRange(r_u16(0x800FF23A) >> 1);
        height = r_u16(0x800FF23C);
        width = r_u16(0x800FF23A);
        result += width >> 2;
        row_offset = (height - 1) * width;
        w_u8(buffer + row_offset + result, intensity);
        height = r_u16(0x800FF23C);
        width = r_u16(0x800FF23A);
        row_offset = (height - 1) * width;
        w_u8(buffer + row_offset + result + 1, intensity);
        height = r_u16(0x800FF23C);
        width = r_u16(0x800FF23A);
        row_offset = (height - 1) * width;
        --sparks;
        w_u8(buffer + row_offset + result - 1, intensity);
    }
    return result;
}

extern "C" uint32 sub_8002E4D8(uint32 sparks)
{
    FUNCTION_MARKER(0x8002E4D8, "SLUS_003.73");
    return SeedMenuFire(sparks);
}

uint32 HasMenuBuffers()
{
    FUNCTION_MARKER(0x8002E13C, "SLUS_003.73");
    return r_u32(0x800FF230) != 0;
}

uint32 CreateMenuBuffers(uint32 resource, uint32 width, uint32 height, uint32 mode)
{
    FUNCTION_MARKER(0x8002E148, "SLUS_003.73");
    uint32 result = HasMenuBuffers();
    if (result)
        return result;
    uint32 texture = FindModelResourceEntry(resource);
    width &= 65535;
    height &= 65535;
    uint32 page = r_u16(texture + 6);
    uint32 pixel_shift = (2 - ((page & 384) >> 7)) & 31;
    uint32 texture_x = r_u8(texture);
    w_u16(0x800FF240, ((page & 15) << 6) + (texture_x >> pixel_shift));
    uint32 texture_y = r_u8(texture + 1);
    w_u16(0x800FF244, width >> 1);
    w_u16(0x800FF23A, width);
    w_u16(0x800FF246, height);
    w_u16(0x800FF23C, height);
    w_u8(0x800FF23E, mode);
    w_u16(0x800FF242, texture_y + ((page << 4) & 256));
    uint32 bytes = width * height;
    result = AllocateHeapBlock(bytes << 1, 0, 1);
    if (!result)
        return result;
    w_u32(0x800FF230, result);
    w_u32(0x800FF234, result + bytes);
    for (uint32 row = 0; row < height; ++row)
    {
        for (uint32 column = 0; column < width; ++column)
        {
            uint32 stride = r_u16(0x800FF23A);
            uint32 buffer = r_u32(0x800FF230);
            w_u8(buffer + row * stride + column, 0);
            stride = r_u16(0x800FF23A);
            buffer = r_u32(0x800FF234);
            w_u8(buffer + row * stride + column, 0);
        }
    }
    return 0;
}

extern "C" uint32 sub_8002E13C(void)
{
    FUNCTION_MARKER(0x8002E13C, "SLUS_003.73");
    return HasMenuBuffers();
}
extern "C" uint32 sub_8002E148(uint32 resource, uint32 width, uint32 height, uint32 mode)
{
    FUNCTION_MARKER(0x8002E148, "SLUS_003.73");
    return CreateMenuBuffers(resource, width, height, mode);
}

uint32 SetMenuDarkRedText()
{
    FUNCTION_MARKER(0x80011594, "SLUS_003.73");
    return SetHudTextColor(149, 20, 20, 0);
}

extern "C" uint32 sub_80011594(void)
{
    FUNCTION_MARKER(0x80011594, "SLUS_003.73");
    return SetMenuDarkRedText();
}


const char *ReadMenuButtonLabel(uint32 button)
{
    uint32 label;
    FUNCTION_MARKER(0x800117D8, "SLUS_003.73");
    if (button == 2)
        label = r_u32(0x800A55C0);
    else if (button == 4)
        label = r_u32(0x800A55C4);
    else if (button == 8)
        label = r_u32(0x800A55C8);
    else if (button == 1)
        label = r_u32(0x800A55BC);
    else
        label = 0x800FEEFC;
    return label ? (const char *)psx_addr(label, 1) : 0;
}

extern "C" uint32 sub_800117D8(uint32 button)
{
    FUNCTION_MARKER(0x800117D8, "SLUS_003.73");
    return GuestMemoryAddress(ReadMenuButtonLabel(button));
}


sint32 ReadMenuNavigationStep()
{
    uint32 left, right, counter;
    sint32 direction = 0;
    FUNCTION_MARKER(0x8001173C, "SLUS_003.73");
    left = r_u8(0x800EC178);
    if (!left)
    {
        right = r_u8(0x800EC188);
        if (!right)
        {
            w_u32(0x800FF018, 0);
            return 0;
        }
    }
    counter = r_u32(0x800FF018);
    if (!counter || ((sint32)counter >= 11 && !(counter & 1)))
    {
        right = r_u8(0x800EC188);
        if (right)
            direction = 1;
        if (left)
            direction = -1;
    }
    counter = r_u32(0x800FF018);
    w_u32(0x800FF018, counter + 1);
    return direction;
}

extern "C" uint32 sub_8001173C(void)
{
    FUNCTION_MARKER(0x8001173C, "SLUS_003.73");
    return (uint32)ReadMenuNavigationStep();
}

static sint32 ReadMenuButtonBinding(const sint16 *binding)
{
    uint32 address = GuestMemoryAddress(binding);
    return address ? (sint16)r_u16(address) : (sint16)xport_load_le16(binding);
}

static void WriteMenuButtonBinding(sint16 *binding, uint32 button)
{
    uint32 address = GuestMemoryAddress(binding);
    if (address)
        w_u16(address, button);
    else
        xport_store_le16(binding, (uint16)button);
}

sint32 AssignMenuButton(uint32 button, sint16 *selected, sint16 *other1, sint16 *other2, sint16 *other3)
{
    FUNCTION_MARKER(0x80011860, "SLUS_003.73");
    WriteMenuButtonBinding(selected, button);
    sint32 first = ReadMenuButtonBinding(other1);
    sint32 second = ReadMenuButtonBinding(other2);
    sint32 third = ReadMenuButtonBinding(other3);
    uint32 available = ~(button | (uint32)first | (uint32)second | (uint32)third) & 15;
    if (available & 8)
        available = 8;
    if (available & 4)
        available = 4;
    if (available & 2)
        available = 2;
    if (available & 1)
        available = 1;
    if (button == (uint32)first)
        WriteMenuButtonBinding(other1, available);
    second = ReadMenuButtonBinding(other2);
    if (button == (uint32)second)
        WriteMenuButtonBinding(other2, available);
    third = ReadMenuButtonBinding(other3);
    if (button == (uint32)third)
        WriteMenuButtonBinding(other3, available);
    return third;
}

extern "C" uint32 sub_80011860(uint32 button, uint32 selected, uint32 other1, uint32 other2, uint32 other3)
{
    FUNCTION_MARKER(0x80011860, "SLUS_003.73");
    return (uint32)AssignMenuButton(button, (sint16 *)psx_addr(selected, 2), (sint16 *)psx_addr(other1, 2), (sint16 *)psx_addr(other2, 2), (sint16 *)psx_addr(other3, 2));
}

// Native extraction retains the signed multiply-high quotient sequence
static sint32 ReadMenuSliderWidth(uint32 address, uint32 multiplier, uint32 shift)
{
    sint32 level = (sint16)r_u16(address);
    uint32 product = (uint32)level * 296;
    sint64 wide = (sint64)(sint32)product * (sint64)(sint32)multiplier;
    uint32 high = (uint32)((uint64)wide >> 32);
    sint32 sum = (sint32)(high + product);
    return (sint32)((uint32)(sum >> shift) - (uint32)((sint32)product >> 31));
}

static const char *ReadMenuSliderLabel(uint32 address)
{
    uint32 label = r_u32(address);
    return label ? (const char *)psx_addr(label, 1) : 0;
}

uint32 DrawMenuVolumeSlider(MenuWindow &menu)
{
    FUNCTION_MARKER(0x800115C0, "SLUS_003.73");
    uint32 result = menu.IsSelected(ReadMenuSliderLabel(0x800A55A4));
    if (result)
        return result;
    sint32 width = 0;
    bool width_known = false;
    if (menu.IsSelected(ReadMenuSliderLabel(0x800A55A8)))
    {
        width = ReadMenuSliderWidth(0x800ECC7A, 0x80020009, 13);
        width_known = true;
    }
    if (menu.IsSelected(ReadMenuSliderLabel(0x800A55AC)))
    {
        width = ReadMenuSliderWidth(0x800ECC78, 0x81020409, 6);
        width_known = true;
    }
    if (menu.IsSelected(ReadMenuSliderLabel(0x800A55B0)))
    {
        width = ReadMenuSliderWidth(0x800ECC7C, 0x80808081, 7);
        width_known = true;
    }
    if (!width_known)
    {
        // TODO Original unmatched-item branch reads an undefined saved width
        GameDiagnostic("%s", "Undefined original menu volume slider selection\n");
        abort();
    }
    AllocateHudRectangle((uint32)width + 108, 41, 296 - (uint32)width, 10, 0, 0, 0, 0);
    uint8 *packet = AllocateHudRectangle(106, 40, 300, 12, 100, 0, 0, 0);
    return packet ? GuestMemoryAddress(packet) : 0;
}

extern "C" uint32 sub_800115C0(uint32 menu)
{
    FUNCTION_MARKER(0x800115C0, "SLUS_003.73");
    return DrawMenuVolumeSlider(MenuWindow::Attach(menu));
}

