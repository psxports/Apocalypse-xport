extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "sprite_trail.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include "effect_sprite.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Native adapters preserve original pointer loads
static uint32 ReadTrailPosition(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

static uint32 DivideTrailColor(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

SpriteTrail::SpriteTrail(const uint32 *position, uint32 segments, uint32 spacing, const char *body_name, const char *head_name, uint32 width, uint32 enabled) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80036634, "SLUS_003.73");
    uint32 samples, index, offset, destination, history, x, y, z, table, slot, child, flags;
    w_u32(object + 72, segments);
    spacing -= 1;
    w_u32(object + 68, 0x800A1BC8);
    w_u32(object + 76, spacing);
    samples = segments * spacing + 1;
    w_u32(object + 80, samples);
    history = AllocateHeapBlock(samples * 12, 0, 1);
    samples = r_u32(object + 80);
    index = 0;
    w_u32(object + 88, history);
    if ((sint32)samples > 0)
    {
        offset = 0;
        do
        {
            history = r_u32(object + 88);
            destination = history + offset;
            ++index;
            x = ReadTrailPosition(position, 0);
            y = ReadTrailPosition(position, 1);
            z = ReadTrailPosition(position, 2);
            w_u32(destination, x);
            w_u32(destination + 4, y);
            w_u32(destination + 8, z);
            samples = r_u32(object + 80);
            offset += 12;
        } while ((sint32)index < (sint32)samples);
    }
    table = AllocateHeapBlock(segments * 4, 0, 1);
    index = r_u32(object + 72) - 1;
    w_u32(object + 92, table);
    if ((sint32)index >= 0)
    {
        do
        {
            table = r_u32(object + 92);
            offset = index * 4;
            slot = table + offset;
            child = AllocateEffectStorage(120);
            if (child)
            {
                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(LoopingEndpointSprite));
                new (dram_addr) LoopingEndpointSprite();
            }
            // TODO: Resolve original null allocation before endpoint attachment
            w_u32(slot, child);
            table = r_u32(object + 92);
            child = r_u32(table + offset);
            w_u8(child + 66, 1);
            table = r_u32(object + 92);
            child = r_u32(table + offset);
            LoopingEndpointSprite::Attach(child).SelectResource(body_name);
            table = r_u32(object + 92);
            child = r_u32(table + offset);
            LoopingEndpointSprite::Attach(child).SetSize(width & 0xFFFF);
            if (enabled)
            {
                table = r_u32(object + 92);
                child = r_u32(table + offset);
                LoopingEndpointSprite::Attach(child).EnableSemiTransparency();
            }
            --index;
        } while ((sint32)index >= 0);
    }
    table = r_u32(object + 92);
    child = r_u32(table);
    flags = r_u8(child + 88);
    w_u8(child + 88, flags | 16);
    table = r_u32(object + 92);
    child = r_u32(table);
    LoopingEndpointSprite::Attach(child).SelectResource(head_name);
}

uint32 SpriteTrail::Destroy(uint32 flags)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80036828, "SLUS_003.73");
    sint32 index = 0;
    sint32 count = (sint32)r_u32(object + 72);
    w_u32(object + 68, 0x800A1BC8);
    if (count > 0)
    {
        do
        {
            uint32 child = r_u32(r_u32(object + 92) + (uint32)index * 4);
            if (child)
            {
                uint32 table = r_u32(child + 68);
                sint32 adjustment = (sint16)r_u16(table + 8);
                uint32 target = r_u32(table + 12);
                (void)apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
            }
            index = (sint32)((uint32)index + 1);
        } while (index < (sint32)r_u32(object + 72));
    }
    ReleaseHeapBlock(r_u32(object + 92));
    ReleaseHeapBlock(r_u32(object + 88));
    EffectController::Destroy(0);
    if (flags & 1)
        return ReleaseEffectStorage(object);
    return flags & 1;
}

uint32 SpriteTrail::SetColors(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800368F8, "SLUS_003.73");
    uint32 index = 0, result = r_u32(object + 72), more;
    if ((sint32)result > 0)
    {
        do
        {
            uint32 table = r_u32(object + 92);
            uint32 child = r_u32(table + index * 4);
            LoopingEndpointSprite::Attach(child).SetColors(red & 255, green & 255, blue & 255);
            result = r_u32(object + 72);
            ++index;
            more = (sint32)index < (sint32)result;
            // The final branch delay slot also writes the offset carrier
            result = index * 4;
        } while (more);
    }
    return result;
}

uint32 SpriteTrail::SetWidth(uint32 width)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80036988, "SLUS_003.73");
    uint32 index = 0, result = r_u32(object + 72);
    if ((sint32)result > 0)
    {
        do
        {
            uint32 table = r_u32(object + 92);
            uint32 child = r_u32(table + index * 4);
            LoopingEndpointSprite::Attach(child).SetSize(width & 65535);
            result = r_u32(object + 72);
            ++index;
            result = (sint32)index < (sint32)result;
        } while (result);
    }
    return result;
}

uint32 SpriteTrail::SetPosition(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800369F8, "SLUS_003.73");
    uint32 index, limit, history, destination, x, y, z, count, table, child, offset, result;
    index = r_u32(object + 84);
    limit = r_u32(object + 80);
    index += 1;
    w_u32(object + 84, index);
    if (index == limit)
        w_u32(object + 84, 0);
    index = r_u32(object + 84);
    history = r_u32(object + 88);
    destination = history + index * 12;
    x = ReadTrailPosition(position, 0);
    y = ReadTrailPosition(position, 1);
    z = ReadTrailPosition(position, 2);
    w_u32(destination, x);
    w_u32(destination + 4, y);
    w_u32(destination + 8, z);
    result = r_u32(object + 72);
    index = r_u32(object + 84);
    count = result - 1;
    while ((sint32)count >= 0)
    {
        offset = count * 4;
        table = r_u32(object + 92);
        history = r_u32(object + 88);
        child = r_u32(table + offset);
        destination = history + index * 12;
        x = r_u32(destination);
        y = r_u32(destination + 4);
        z = r_u32(destination + 8);
        w_u32(child + 108, x);
        w_u32(child + 112, y);
        w_u32(child + 116, z);
        index -= r_u32(object + 76);
        if ((sint32)index < 0)
            index += r_u32(object + 80);
        --count;
        table = r_u32(object + 92);
        history = r_u32(object + 88);
        child = r_u32(table + offset);
        result = history + index * 12;
        x = r_u32(result);
        y = r_u32(result + 4);
        z = r_u32(result + 8);
        w_u32(child + 96, x);
        w_u32(child + 100, y);
        w_u32(child + 104, z);
    }
    return result;
}

RibbonTrail::RibbonTrail(const uint32 *position, uint32 segments, uint32 red, uint32 green, uint32 blue) : SpriteTrail(position, segments, 2, (const char *)psx_addr(0x800FF3F8, 1), (const char *)psx_addr(0x800FF3F8, 1), 400, 1)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80036B00, "SLUS_003.73");
    uint32 count, red_step, green_step, blue_step, index = 0, next, weight, table, child, offset;
    count = r_u32(object + 72);
    red_step = DivideTrailColor(red, count);
    green_step = DivideTrailColor(green, count);
    w_u32(object + 68, 0x800A1BB0);
    blue_step = DivideTrailColor(blue, count);
    for (;;)
    {
        count = r_u32(object + 72);
        next = index + 1;
        if ((sint32)index >= (sint32)count)
            break;
        weight = count - next;
        offset = index * 4;
        table = r_u32(object + 92);
        child = r_u32(table + offset);
        LoopingEndpointSprite::Attach(child).SetColors((red - weight * red_step) & 255, (green - weight * green_step) & 255, (blue - weight * blue_step) & 255);
        table = r_u32(object + 92);
        child = r_u32(table + offset);
        LoopingEndpointSprite::Attach(child).SetColorStep(8);
        index = next;
    }
}

uint32 RibbonTrail::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80036C5C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1BB0);
    uint32 result = SpriteTrail::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 RibbonTrail::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80036CB0, "SLUS_003.73");
    uint32 result = r_u32(object + 96), all_empty = 1, index = 0;
    if (result)
    {
        result = r_u32(object + 72);
        if ((sint32)result > 0)
        {
            do
            {
                if (!LoopingEndpointSprite::Attach(r_u32(r_u32(object + 92) + 4 * index)).FadeColors(0))
                    all_empty = 0;
                ++index;
                result = (sint32)index < (sint32)r_u32(object + 72);
            } while (result);
        }
        if (all_empty)
            return Finish();
    }
    return result;
}

SpriteTrail &SpriteTrail::Attach(uint32 object)
{
    return *static_cast<SpriteTrail *>(FindNativeObjectStorage(object));
}

RibbonTrail &RibbonTrail::Attach(uint32 object)
{
    return *static_cast<RibbonTrail *>(FindNativeObjectStorage(object));
}





extern "C" uint32 sub_80036828(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80036828, "SLUS_003.73");
    return SpriteTrail::Attach(object).SpriteTrail::Destroy(reason);
}
extern "C" uint32 sub_80036C5C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80036C5C, "SLUS_003.73");
    return RibbonTrail::Attach(object).RibbonTrail::Destroy(reason);
}
extern "C" uint32 sub_80036CB0(uint32 object)
{
    FUNCTION_MARKER(0x80036CB0, "SLUS_003.73");
    return RibbonTrail::Attach(object).RibbonTrail::Update();
}



