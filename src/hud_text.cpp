#include "game_random.h"
#include "game_heap.h"
#include "hud_text.h"
#include "native_object_storage.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

uint32 AppendHudNumber(uint32 value)
{
    uint32 emitted = 0, divisor = 1000000000, cursor, digit;
    FUNCTION_MARKER(0x8006A75C, "SLUS_003.73");
    if ((sint32)value < 0)
    {
        value = 0 - value;
        cursor = r_u32(0x800FF6F4);
        w_u8(cursor, 45);
        w_u32(0x800FF6F4, cursor + 1);
    }
    while (divisor)
    {
        if ((sint32)value >= (sint32)divisor)
        {
            digit = (uint32)((sint32)value / (sint32)divisor);
            emitted = 1;
            cursor = r_u32(0x800FF6F4);
            w_u8(cursor, digit + 48);
            w_u32(0x800FF6F4, cursor + 1);
            value -= digit * divisor;
        }
        else if (emitted || divisor == 1)
        {
            cursor = r_u32(0x800FF6F4);
            w_u8(cursor, 48);
            w_u32(0x800FF6F4, cursor + 1);
        }
        divisor /= 10;
    }
    cursor = r_u32(0x800FF6F4);
    w_u8(cursor, 0);
    w_u32(0x800FF6F4, cursor + 1);
    return cursor + 1;
}

uint32 InitializeHudFonts()
{
    uint32 small_font, font;
    FUNCTION_MARKER(0x8006A834, "SLUS_003.73");
    small_font = FindModelTextureGroup(0x800A3A3C);
    w_u32(0x800FF6DC, small_font);
    font = FindModelTextureGroup(0x800FF6E8);
    w_u32(0x800FF6E0, font);
    return font;
}

uint32 ResetHudTextQueue()
{
    FUNCTION_MARKER(0x8006A868, "SLUS_003.73");
    w_u32(0x800FF6F0, 0x800E5C08);
    w_u32(0x800E5C08, 1);
    w_u32(0x800FF6F4, 0x800E6A08);
    return 0x800E6A08;
}

uint32 QueueHudText(uint32 x, uint32 y, uint32 text, uint32 red, uint32 green, uint32 blue, uint32 flags)
{
    uint32 first, second, cursor, rgb;
    FUNCTION_MARKER(0x8006A890, "SLUS_003.73");
    first = r_u32(0x800FF6F0);
    w_u8(first + 16, 0);
    second = r_u32(0x800FF6F0);
    w_u32(first, 0);
    w_u16(first + 4, x);
    w_u16(first + 6, y);
    w_u8(second + 17, flags);
    rgb = r_u32(0x800FF6E4);
    cursor = r_u32(0x800FF6F0);
    rgb |= ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    w_u32(cursor + 12, rgb);
    w_u32(0x800FF6F0, cursor + 32);
    w_u32(cursor + 8, text);
    w_u32(cursor + 32, 1);
    return 1;
}

uint32 QueueHudNumber(uint32 x, uint32 y, uint32 value, uint32 red, uint32 green, uint32 blue, uint32 flags)
{
    uint32 first, second, cursor, text, rgb;
    FUNCTION_MARKER(0x8006A8FC, "SLUS_003.73");
    first = r_u32(0x800FF6F0);
    w_u8(first + 16, 0);
    second = r_u32(0x800FF6F0);
    w_u32(first, 0);
    w_u16(first + 4, x);
    w_u16(first + 6, y);
    w_u8(second + 17, flags);
    cursor = r_u32(0x800FF6F0);
    text = r_u32(0x800FF6F4);
    w_u32(cursor + 8, text);
    rgb = r_u32(0x800FF6E4);
    rgb |= ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    w_u32(cursor + 12, rgb);
    AppendHudNumber(value);
    cursor = r_u32(0x800FF6F0);
    w_u32(0x800FF6F0, cursor + 32);
    w_u32(cursor + 32, 1);
    return 1;
}

extern "C" uint32 sub_8006A75C(uint32 value)
{
    FUNCTION_MARKER(0x8006A75C, "SLUS_003.73");
    return AppendHudNumber(value);
}
extern "C" uint32 sub_8006A890(uint32 x, uint32 y, uint32 text, uint32 red, uint32 green, uint32 blue, uint32 flags)
{
    FUNCTION_MARKER(0x8006A890, "SLUS_003.73");
    return QueueHudText(x, y, text, red, green, blue, flags);
}
extern "C" uint32 sub_8006A8FC(uint32 x, uint32 y, uint32 value, uint32 red, uint32 green, uint32 blue, uint32 flags)
{
    FUNCTION_MARKER(0x8006A8FC, "SLUS_003.73");
    return QueueHudNumber(x, y, value, red, green, blue, flags);
}

extern "C" uint32 sub_8006A834(void)
{
    FUNCTION_MARKER(0x8006A834, "SLUS_003.73");
    return InitializeHudFonts();
}
extern "C" uint32 sub_8006A868(void)
{
    FUNCTION_MARKER(0x8006A868, "SLUS_003.73");
    return ResetHudTextQueue();
}

uint32 DrawHudTextQueue()
{
    uint32 row = 0x800E5C08, camera[3], delta[3], scaled[3], shift;
    uint32 matrix[5], sentinel, projected, text, command, x, y, depth, screen;
    uint32 character, glyph, metrics, metric_width, uv, packet, next, limit;
    uint32 uv_word, u_right, u_left, v_bottom, v_top, uv_tail, width, height;
    uint32 byte, top, bottom, right, buffer, bucket, tag, head;
    FUNCTION_MARKER(0x8006AAB8, "SLUS_003.73");
    matrix[0] = r_u32(0x800ED590);
    matrix[1] = r_u32(0x800ED594);
    xport_draft_gte_control_write(0, matrix[0]);
    xport_draft_gte_control_write(1, matrix[1]);
    matrix[2] = r_u32(0x800ED598);
    matrix[3] = r_u32(0x800ED59C);
    matrix[4] = r_u32(0x800ED5A0);
    xport_draft_gte_control_write(2, matrix[2]);
    xport_draft_gte_control_write(3, matrix[3]);
    xport_draft_gte_control_write(4, matrix[4]);
    xport_draft_gte_control_write(5, 0);
    xport_draft_gte_control_write(6, 0);
    xport_draft_gte_control_write(7, 0);
    camera[0] = r_u32(0x800ED520) << 12;
    camera[1] = r_u32(0x800ED524) << 12;
    camera[2] = r_u32(0x800ED528) << 12;
    for (;; row += 32)
    {
        sentinel = r_u32(row);
        if (sentinel)
            return sentinel;
        projected = r_u8(row + 16);
        text = r_u32(row + 8);
        // TODO Original depth is defined only when this row is projected
        depth = 0;
        if (projected)
        {
            SubtractVectors(delta, (const uint32 *)psx_addr(row + 20, 12), camera);
            shift = 12;
            ShiftVectorRight(scaled, delta, &shift);
            xport_draft_gte_data_write(0, (scaled[0] & 65535) | (scaled[1] << 16));
            xport_draft_gte_data_write(1, scaled[2]);
            xport_draft_gte_execute(0x180001);
            command = r_u32(row + 12);
            depth = xport_draft_gte_data_read(27);
            if ((sint32)depth < (sint32)r_u32(0x800FFAE8))
                continue;
            if ((sint32)r_u32(0x800FFAD8) < (sint32)depth)
                continue;
            screen = xport_draft_gte_data_read(14);
            x = screen & 65535;
            y = screen >> 16;
        }
        else
        {
            x = r_u16(row + 4);
            y = r_u16(row + 6);
            command = r_u32(row + 12);
        }
        for (;;)
        {
            character = r_u8(text);
            if (!character)
                break;
            glyph = r_u8(0x800E6888 + (character & 127));
            ++text;
            if (character == 95)
                glyph = r_u8(0x800E6888 + 45);
            if (glyph == 255)
            {
                x += 5;
                continue;
            }
            metrics = r_u32(0x800FF6DC) + (glyph << 3);
            metric_width = r_u8(metrics + 2);
            uv = r_u32(metrics + 4);
            if (metric_width < 4)
                ++x;
            packet = r_u32(0x800FF668);
            limit = r_u32(0x800FF374);
            next = packet + 40;
            if (limit < next)
                return 0x09000000;
            w_u32(0x800FF668, next);
            w_u32(packet, 0x09000000);
            w_u32(packet + 4, command);
            w_u32(packet + 12, r_u32(uv));
            w_u32(packet + 20, r_u32(uv + 4));
            uv_word = r_u32(uv + 8);
            u_right = r_u8(packet + 20);
            w_u32(packet + 28, uv_word);
            u_left = r_u8(packet + 12);
            uv_tail = r_u16(uv + 10);
            v_bottom = r_u8(packet + 29);
            width = u_right - u_left;
            v_top = r_u8(packet + 13);
            w_u16(packet + 36, uv_tail);
            projected = r_u8(row + 17);
            height = v_bottom - v_top;
            if (projected)
            {
                width = (uint32)((sint32)(width * 55) >> 5);
                height = (uint32)((sint32)(height * 55) >> 5);
                byte = r_u8(packet + 36);
                w_u8(packet + 20, u_right - 1);
                v_bottom = r_u8(packet + 29);
                w_u8(packet + 36, byte - 1);
                byte = r_u8(packet + 37);
                w_u8(packet + 29, v_bottom - 1);
                w_u8(packet + 37, byte - 1);
            }
            right = x + width;
            top = y;
            if (glyph == 30)
                top += character == 95 ? 5 : 2;
            bottom = top + height;
            w_u16(packet + 8, x);
            w_u16(packet + 10, top);
            w_u16(packet + 16, right);
            w_u16(packet + 18, top);
            w_u16(packet + 24, x);
            w_u16(packet + 26, bottom);
            w_u16(packet + 32, right);
            w_u16(packet + 34, bottom);
            projected = r_u8(row + 16);
            buffer = r_u32(0x800FF660);
            bucket = buffer + (projected ? depth & 0x3FFC : 0) + 112;
            tag = r_u32(packet);
            head = r_u32(bucket);
            w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
            head = r_u32(bucket);
            w_u32(bucket, (head & 0xFF000000) | (packet & 0xFFFFFF));
            x += 3 + width;
            if ((sint32)width < 4)
                ++x;
        }
    }
}

extern "C" uint32 sub_8006AAB8(void)
{
    FUNCTION_MARKER(0x8006AAB8, "SLUS_003.73");
    return DrawHudTextQueue();
}

uint32 AllocateAnimatedHudTextNode()
{
    uint32 node, previous;
    FUNCTION_MARKER(0x8001B654, "SLUS_003.73");
    node = AllocateHeapBlock(20, 0, 1);
    previous = r_u32(0x800FF1AC);
    w_u32(node + 16, 0);
    w_u32(0x800FF1AC, node);
    w_u32(node + 12, previous);
    if (previous)
        w_u32(previous + 16, node);
    return node;
}

uint32 RemoveAnimatedHudTextNode(uint32 node)
{
    uint32 next, previous, head;
    FUNCTION_MARKER(0x8001B694, "SLUS_003.73");
    next = r_u32(node + 12);
    if (next)
    {
        previous = r_u32(node + 16);
        w_u32(next + 16, previous);
    }
    previous = r_u32(node + 16);
    if (previous)
    {
        next = r_u32(node + 12);
        w_u32(previous + 12, next);
    }
    head = r_u32(0x800FF1AC);
    if (node == head)
    {
        next = r_u32(node + 12);
        w_u32(0x800FF1AC, next);
    }
    return ReleaseHeapBlock(node);
}

void ClearAnimatedHudText()
{
    uint32 current, next;
    FUNCTION_MARKER(0x8001B708, "SLUS_003.73");
    current = r_u32(0x800FF1AC);
    if (current)
    {
        next = r_u32(current + 12);
        for (;;)
        {
            ReleaseHeapBlock(current);
            current = next;
            if (!next)
                break;
            next = r_u32(next + 12);
        }
    }
    w_u32(0x800FF1AC, 0);
}

uint32 AddAnimatedHudText(uint32 text, uint32 animation)
{
    uint32 node;
    FUNCTION_MARKER(0x8001B788, "SLUS_003.73");
    // The text queue retains the original string and animation addresses
    node = AllocateAnimatedHudTextNode();
    w_u32(node, text);
    w_u32(node + 8, animation);
    w_u16(node + 4, 0);
    return node;
}

extern "C" uint32 sub_8001B654(void)
{
    FUNCTION_MARKER(0x8001B654, "SLUS_003.73");
    return AllocateAnimatedHudTextNode();
}

extern "C" uint32 sub_8001B694(uint32 node, uint32)
{
    FUNCTION_MARKER(0x8001B694, "SLUS_003.73");
    return RemoveAnimatedHudTextNode(node);
}

extern "C" void sub_8001B708(void)
{
    FUNCTION_MARKER(0x8001B708, "SLUS_003.73");
    ClearAnimatedHudText();
}

extern "C" uint32 sub_8001B788(uint32 resource, uint32 animation)
{
    FUNCTION_MARKER(0x8001B788, "SLUS_003.73");
    return AddAnimatedHudText(resource, animation);
}

uint32 UpdateAnimatedHudText()
{
    uint32 node, next, animation, age, duration, timer;
    sint32 step;
    FUNCTION_MARKER(0x8001B7C4, "SLUS_003.73");
    node = r_u32(0x800FF1AC);
    if (node)
    {
        next = r_u32(node + 12);
        for (;;)
        {
            animation = r_u32(node + 8);
            age = r_u16(node + 4);
            duration = r_u16(animation);
            if (age < duration)
                w_u16(node + 4, age + 1);
            else
            {
                step = (sint8)r_u8(animation + 27);
                if (!step)
                    RemoveAnimatedHudTextNode(node);
                else if (step != 127)
                {
                    w_u32(node + 8, animation + (uint32)step * 28);
                    w_u16(node + 4, 0);
                }
            }
            node = next;
            if (!next)
                break;
            next = r_u32(next + 12);
        }
    }
    timer = r_u32(0x800FF1C8);
    if (timer)
    {
        timer -= 1;
        w_u32(0x800FF1C8, timer);
    }
    else
    {
        timer = r_u32(0x800FF1CC) ? RandomRange(16) + 16 : RandomRange(24);
        w_u32(0x800FF1C8, timer);
        w_u32(0x800FF1CC, 0);
    }
    return timer;
}

static uint32 InterpolateHudText(uint32 first, uint32 last, uint32 age, uint32 duration)
{
    uint32 product = (last - first) * age;
    sint32 quotient;
    if (!duration)
        quotient = (sint32)product < 0 ? 1 : -1;
    else if (product == 0x80000000 && duration == (uint32)-1)
        quotient = (sint32)product;
    else
        quotient = (sint32)product / (sint32)duration;
    return first + (uint32)quotient;
}

void DrawAnimatedHudText()
{
    uint32 node, animation, period, age, duration, first, last, value, red, green, blue, rgb, x, y, size, text;
    FUNCTION_MARKER(0x8001B8BC, "SLUS_003.73");
    node = r_u32(0x800FF1AC);
    while (node)
    {
        animation = r_u32(node + 8);
        period = r_u8(animation + 26);
        if (period)
        {
            age = r_u16(node + 4);
            if (!((age / period) & 1))
            {
                node = r_u32(node + 12);
                continue;
            }
        }
        last = r_u16(animation + 6);
        first = r_u16(animation + 4);
        age = r_u16(node + 4);
        duration = r_u16(animation);
        value = InterpolateHudText(first, last, age, duration);
        value &= 65535;
        w_u8(0x800FF1B0, r_u8(animation + 3));
        w_u16(0x800FF1B8, value);
        last = r_u8(animation + 11);
        first = r_u8(animation + 8);
        age = r_u16(node + 4);
        red = InterpolateHudText(first, last, age, duration) & 255;
        first = r_u8(animation + 9);
        last = r_u8(animation + 12);
        green = InterpolateHudText(first, last, age, duration) & 255;
        last = r_u8(animation + 13);
        first = r_u8(animation + 10);
        blue = InterpolateHudText(first, last, age, duration) & 255;
        rgb = red | (green << 8) | (blue << 16);
        w_u32(0x800FF1B4, rgb);
        value = r_u8(animation + 2);
        w_u32(0x800FF1B4, rgb | (value ? 0x2E000000 : 0x2C000000));
        first = (uint32)(sint32)(sint16)r_u16(animation + 18);
        last = (uint32)(sint32)(sint16)r_u16(animation + 22);
        age = r_u16(node + 4);
        x = InterpolateHudText(first, last, age, duration);
        first = (uint32)(sint32)(sint16)r_u16(animation + 20);
        last = (uint32)(sint32)(sint16)r_u16(animation + 24);
        y = InterpolateHudText(first, last, age, duration);
        first = r_u16(animation + 14);
        last = r_u16(animation + 16);
        size = InterpolateHudText(first, last, age, duration);
        text = r_u32(node);
        DrawStyledHudText(x, y, (const char *)psx_addr(text, 1), size, 0, 256);
        node = r_u32(node + 12);
    }
}

extern "C" uint32 sub_8001B7C4(void)
{
    FUNCTION_MARKER(0x8001B7C4, "SLUS_003.73");
    return UpdateAnimatedHudText();
}

extern "C" void sub_8001B8BC(void)
{
    FUNCTION_MARKER(0x8001B8BC, "SLUS_003.73");
    DrawAnimatedHudText();
}

sint32 GetHudGlyphIndex(uint32 character)
{
    uint32 index;
    FUNCTION_MARKER(0x8001A784, "SLUS_003.73");
    index = r_u8(0x800E6908 + (character & 255));
    return index == 255 ? -1 : (sint32)index;
}

void SetHudTextAlignment(uint32 alignment)
{
    FUNCTION_MARKER(0x8001A7B0, "SLUS_003.73");
    w_u8(0x800FF1B0, alignment);
}

void SetHudTextScale(uint32 scale)
{
    FUNCTION_MARKER(0x8001A7BC, "SLUS_003.73");
    w_u16(0x800FF1B8, scale);
}

void SetHudTextDepth(uint32 depth)
{
    FUNCTION_MARKER(0x8001A7C8, "SLUS_003.73");
    w_u16(0x800FF1BA, depth);
}

uint32 SetHudTextColor(uint32 red, uint32 green, uint32 blue, uint32 semi_transparent)
{
    uint32 rgb, result;
    FUNCTION_MARKER(0x8001A7D4, "SLUS_003.73");
    rgb = ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    w_u32(0x800FF1B4, rgb);
    result = rgb | (semi_transparent ? 0x2E000000 : 0x2C000000);
    w_u32(0x800FF1B4, result);
    return result;
}

extern "C" uint32 sub_8001A784(uint32 character)
{
    FUNCTION_MARKER(0x8001A784, "SLUS_003.73");
    return (uint32)GetHudGlyphIndex(character);
}

extern "C" void sub_8001A7B0(uint32 alignment)
{
    FUNCTION_MARKER(0x8001A7B0, "SLUS_003.73");
    SetHudTextAlignment(alignment);
}

extern "C" void sub_8001A7BC(uint32 scale)
{
    FUNCTION_MARKER(0x8001A7BC, "SLUS_003.73");
    SetHudTextScale(scale);
}

extern "C" void sub_8001A7C8(uint32 depth)
{
    FUNCTION_MARKER(0x8001A7C8, "SLUS_003.73");
    SetHudTextDepth(depth);
}

extern "C" uint32 sub_8001A7D4(uint32 red, uint32 green, uint32 blue, uint32 semi_transparent)
{
    FUNCTION_MARKER(0x8001A7D4, "SLUS_003.73");
    return SetHudTextColor(red, green, blue, semi_transparent);
}

uint32 LoadHudTextGlowResource()
{
    uint32 resource;
    FUNCTION_MARKER(0x8001A760, "SLUS_003.73");
    resource = FindModelResourceEntry(0x38B9E52B);
    w_u32(0x800FF1D0, resource);
    return resource;
}

uint32 InitializeHudTextGlowCurve()
{
    uint32 index, value;
    FUNCTION_MARKER(0x8001A858, "SLUS_003.73");
    for (index = 0; index < 40; ++index)
    {
        value = RandomRange(10);
        w_u32(0x800A57FC + index * 4, value + 10);
        w_u32(0x800A593C + index * 4, 40);
        value = RandomRange(30);
        w_u32(0x800A589C + index * 4, value + 30);
    }
    return 0;
}

uint32 UpdateHudTextGlowCurve()
{
    uint32 index, rate, tick, phase, amplitude, product;
    sint32 sine;
    FUNCTION_MARKER(0x8001A8E8, "SLUS_003.73");
    for (index = 0; index < 40; ++index)
    {
        rate = r_u32(0x800A589C + index * 4);
        tick = r_u32(0x800FF64C);
        phase = (rate * tick) & 4095;
        sine = (sint16)r_u16(0x800F863C + phase * 4);
        amplitude = r_u32(0x800A57FC + index * 4);
        product = amplitude * (uint32)sine;
        if ((sint32)product < 0)
            product += 4095;
        w_u32(0x800A593C + index * 4, (uint32)((sint32)product >> 12) + 40);
    }
    // Original updates40 entries while the packet owner cycles through41 indices
    return 0;
}

uint32 MeasureStyledHudText(const char *text)
{
    uint32 mapped = GuestMemoryAddress(text), cursor = 0, character, width = 0, count = 0;
    uint32 font, glyph, advance, bearing, scale;
    sint32 index;
    FUNCTION_MARKER(0x8001A97C, "SLUS_003.73");
    for (;;)
    {
        character = mapped ? r_u8(mapped + cursor) : (uint8)text[cursor];
        if (!character)
            break;
        index = GetHudGlyphIndex(character);
        if (index < 0)
            width += 10;
        else
        {
            font = r_u32(0x800FF6E0);
            glyph = font + (uint32)index * 8;
            advance = r_u8(glyph + 2);
            bearing = (uint32)(sint32)(sint8)r_u8(glyph);
            count += 1;
            width += advance + bearing;
        }
        cursor += 1;
    }
    scale = r_u16(0x800FF1B8);
    return (uint32)((sint32)(width * scale) >> 8) + count * 3;
}

extern "C" uint32 sub_8001A760(void)
{
    FUNCTION_MARKER(0x8001A760, "SLUS_003.73");
    return LoadHudTextGlowResource();
}

extern "C" uint32 sub_8001A858(void)
{
    FUNCTION_MARKER(0x8001A858, "SLUS_003.73");
    return InitializeHudTextGlowCurve();
}

extern "C" uint32 sub_8001A8E8(void)
{
    FUNCTION_MARKER(0x8001A8E8, "SLUS_003.73");
    return UpdateHudTextGlowCurve();
}

extern "C" uint32 sub_8001A97C(uint32 text)
{
    FUNCTION_MARKER(0x8001A97C, "SLUS_003.73");
    return MeasureStyledHudText((const char *)psx_addr(text, 1));
}

extern "C" uint32 apocalypse_text_width(const char *text)
{
    return MeasureStyledHudText(text);
}

void SetHudTextShadowColor(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8001A838, "SLUS_003.73");
    w_u32(0x800FF1BC, red & 255);
    w_u32(0x800FF1C0, green & 255);
    w_u32(0x800FF1C4, blue & 255);
}

void ResetHudTextShadowColor()
{
    FUNCTION_MARKER(0x8001A810, "SLUS_003.73");
    SetHudTextShadowColor(255, 200, 0);
}
