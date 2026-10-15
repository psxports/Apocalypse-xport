#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

#include "character.h"
#include "object_lifecycle.h"
#include "object_damage.h"
#include "game_heap.h"
#include "model_resources.h"
#include "game_object_allocation.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Native lookup for the mapped single-inheritance visual hierarchy

// Native binding for the original addressable list-head word

// Native adapter for the original adjusted suspend callback

// Native input adapters for original record word and halfword loads

// Native extraction of original signed color DIV corner behavior

// Native and legacy destruction share only the original buffer field operations

#include "visual_object.h"
#include "vector_arithmetic.h"
#include "animation.h"
#include "vector_distance.h"
#include "activation_record.h"

// Original list-level operation has a list-head receiver, not an object receiver

// List traversal is a free function, not an object method

#include "textured_quad.h"
#include "effect_lifecycle.h"
#include <new>

static uint32 VisualListAddress(uint32 *head)
{
    uint32 address = GuestMemoryAddress(head);
    if (!address)
    {
        // TODO Bind native list heads to the original linked-list layout
        GameDiagnostic("TODO VisualObject native list head\n");
        abort();
    }
    return address;
}

static void InvokeVisualSuspendCallback(uint32 target, uint32 receiver)
{
    if (target == 0x80063994)
    {
        /* Original method contains only JR RA and NOP */
        return;
    }
    if (target == 0x8005843C)
    {
        sub_8005843C(receiver);
        return;
    }
    GameDiagnostic("TODO 62B7C virtual60 target %08X receiver %08X\n", target, receiver);
    abort();
}

static uint32 ReadVisualRecordWord(const uint8 *data)
{
    uint32 address = GuestMemoryAddress(data);
    if (address)
        return r_u32(address);
    uint32 value;
    memcpy(&value, data, 4);
    return value;
}

static uint32 ReadVisualRecordHalf(const uint8 *data)
{
    uint32 address = GuestMemoryAddress(data);
    if (address)
        return r_u16(address);
    uint16 value;
    memcpy(&value, data, 2);
    return value;
}

static sint32 DivideVisualColor(sint32 numerator, uint32 divisor)
{
    if (!divisor)
        return numerator < 0 ? 1 : -1;
    return numerator / (sint32)divisor;
}

VisualObject &VisualObject::Attach(uint32 object)
{
    return *static_cast<VisualObject *>(FindNativeObjectStorage(object));
}

ScaledObject::ScaledObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062628, "SLUS_003.73");
    w_u32(object + 68, 0x800A33C0);
    w_u16(object + 36, 4096);
    w_u16(object + 38, 4096);
    w_u16(object + 40, 4096);
}

uint32 InitializeScaledObjectBase(uint32 object)
{
    FUNCTION_MARKER(0x80062628, "SLUS_003.73");
    w_u32(object + 68, 0x800A33C0);
    w_u16(object + 36, 4096);
    w_u16(object + 38, 4096);
    w_u16(object + 40, 4096);
    return object;
}

uint32 ScaledObject::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, index;
    FUNCTION_MARKER(0x80062650, "SLUS_003.73");
    flags = r_u16(object) & 1024;
    w_u32(object + 68, 0x800A33C0);
    if (flags)
    {
        index = r_u8(object + 25);
        ReleaseObjectFlash(0x800F2610 + index * 24);
    }
    if (reason & 1)
        return sub_80062608(object);
    return 0;
}

uint32 DestroyScaledObjectBase(uint32 object, uint32 reason)
{
    uint32 flags, index;
    FUNCTION_MARKER(0x80062650, "SLUS_003.73");
    flags = r_u16(object) & 1024;
    w_u32(object + 68, 0x800A33C0);
    if (flags)
    {
        index = r_u8(object + 25);
        ReleaseObjectFlash(0x800F2610 + index * 24);
    }
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 VisualObject::SetModelBank(const char *name)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800626C8, "SLUS_003.73");
    uint32 result = FindModelBankByName(name);
    w_u8(object + 27, result);
    w_u16(object + 22, 0);
    return result;
}

uint32 VisualObject::SetColor(sint32 duration, uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800626F8, "SLUS_003.73");
    uint32 flags, color, divisor, result;
    sint32 dr, dg, db;
    red &= 255;
    green &= 255;
    blue &= 255;
    color = red | (green << 8) | (blue << 16);
    if (!duration)
    {
        flags = r_u16(object);
        w_u16(object + 60, 0);
    }
    else if ((sint32)duration < 0)
    {
        divisor = (0 - (uint32)duration) & 65535;
        w_u16(object + 60, (uint16)divisor);
        dr = DivideVisualColor((sint32)red - 128, divisor);
        dg = DivideVisualColor((sint32)green - 128, r_u16(object + 60));
        db = DivideVisualColor((sint32)blue - 128, r_u16(object + 60));
        flags = r_u16(object);
        w_u32(object + 32, 0x808080);
        result = flags | 0x400;
        w_u16(object, (uint16)result);
        w_u16(object + 62, (uint16)dr);
        w_u16(object + 64, (uint16)dg);
        w_u16(object + 66, (uint16)db);
        return result;
    }
    else
    {
        dr = (128 - (sint32)red) / (sint32)duration;
        dg = (128 - (sint32)green) / (sint32)duration;
        db = (128 - (sint32)blue) / (sint32)duration;
        w_u16(object + 66, (uint16)db);
        flags = r_u32(object + 52);
        w_u16(object + 60, (uint16)((uint32)duration + 1));
        w_u16(object + 64, (uint16)dg);
        w_u16(object + 62, (uint16)dr);
        if (!(flags & 0x1000000))
        {
            uint32 old = r_u32(object + 32) & 0xFFFFFF;
            flags = r_u16(object);
            w_u32(object + 52, old | 0x1000000);
            if (flags & 0x400)
                w_u32(object + 52, old | 0x3000000);
        }
        flags = r_u16(object);
    }
    w_u32(object + 32, color);
    result = flags | 0x400;
    w_u16(object, (uint16)result);
    return result;
}

uint32 VisualObject::AdvanceColor()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062858, "SLUS_003.73");
    uint32 timer = r_u16(object + 60);
    if (!timer)
        return timer;
    uint32 color = r_u32(object + 32);
    uint32 red_delta = (uint32)(sint32)(sint16)r_u16(object + 62);
    uint32 green_delta = (uint32)(sint32)(sint16)r_u16(object + 64);
    uint32 red = ((color & 255) + red_delta) & 255;
    uint32 green = ((((color >> 8) & 255) + green_delta) & 255) << 8;
    uint32 blue_delta = (uint32)(sint32)(sint16)r_u16(object + 66);
    uint32 flags = r_u16(object);
    uint32 blue = ((((color >> 16) & 255) + blue_delta) & 255) << 16;
    w_u16(object, flags | 1024);
    timer = r_u16(object + 60);
    uint32 result = red | green | blue;
    w_u32(object + 32, result);
    timer = (timer - 1) & 65535;
    w_u16(object + 60, timer);
    if (!timer)
    {
        uint32 restore_flags = r_u32(object + 52);
        flags = r_u16(object);
        flags = (restore_flags & 0x2000000) ? flags | 1024 : flags & 0xFBFF;
        w_u16(object, flags);
        result = r_u32(object + 52);
        w_u32(object + 52, 0);
        w_u32(object + 32, result);
    }
    return result;
}

VisualObject::VisualObject() : ScaledObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, value;
    FUNCTION_MARKER(0x80062924, "SLUS_003.73");
    w_u32(object + 68, 0x800A3380);
    w_u8(object + 129, 1);
    w_u8(object + 130, 1);
    w_u8(object + 131, 1);
    w_u8(object + 144, 1);
    w_u8(object + 145, 1);
    w_u8(object + 146, 1);
    w_u32(object + 208, 10);
    w_u16(object + 148, 0);
    w_u16(object + 214, 65535);
    value = r_u16(0x800EC682);
    w_u16(object + 200, value);
    flags = r_u16(object + 78);
    value = r_u16(0x800EC680);
    w_u16(object + 78, flags | 18);
    w_u16(object + 212, 50);
    w_u16(object + 204, value);
}

uint32 InitializeVisualObjectBase(uint32 object)
{
    uint32 flags, value;
    FUNCTION_MARKER(0x80062924, "SLUS_003.73");
    InitializeScaledObjectBase(object);
    w_u32(object + 68, 0x800A3380);
    w_u8(object + 129, 1);
    w_u8(object + 130, 1);
    w_u8(object + 131, 1);
    w_u8(object + 144, 1);
    w_u8(object + 145, 1);
    w_u8(object + 146, 1);
    w_u32(object + 208, 10);
    w_u16(object + 148, 0);
    w_u16(object + 214, 65535);
    value = r_u16(0x800EC682);
    w_u16(object + 200, value);
    flags = r_u16(object + 78);
    value = r_u16(0x800EC680);
    w_u16(object + 78, flags | 18);
    w_u16(object + 212, 50);
    w_u16(object + 204, value);
    return object;
}

uint32 VisualObject::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x800629BC, "SLUS_003.73");
    child = r_u32(object + 196);
    w_u32(object + 68, 0x800A3380);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    ScaledObject::Destroy(0);
    if (reason & 1)
        return sub_80062608(object);
    return 0;
}

uint32 DestroyVisualObjectBase(uint32 object, uint32 reason)
{
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x800629BC, "SLUS_003.73");
    child = r_u32(object + 196);
    w_u32(object + 68, 0x800A3380);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    DestroyScaledObjectBase(object, 0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 VisualObject::AddToList(uint32 *head)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 list = VisualListAddress(head);
    FUNCTION_MARKER(0x80062A38, "SLUS_003.73");
    uint32 next = r_u32(list);
    w_u32(object + 48, 0);
    w_u32(object + 28, next);
    w_u32(list, object);
    next = r_u32(object + 28);
    if (next)
        w_u32(next + 48, object);
    return next;
}

uint32 VisualObject::RemoveFromList(uint32 *head)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 list = VisualListAddress(head);
    FUNCTION_MARKER(0x80062A64, "SLUS_003.73");
    if ((r_u16(object + 78) & 1) && list != 0x800FF5E0)
        RestoreList();
    uint32 next = r_u32(object + 28);
    if (next)
    {
        uint32 previous = r_u32(object + 48);
        w_u32(next + 48, previous);
    }
    uint32 previous = r_u32(object + 48);
    if (previous)
    {
        next = r_u32(object + 28);
        w_u32(previous + 28, next);
    }
    uint32 result = r_u32(list);
    if (result == object)
    {
        result = r_u32(object + 28);
        w_u32(list, result);
    }
    return result;
}

const uint8 *VisualObject::ReadPosition(const uint8 *data)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062B0C, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 value = ReadVisualRecordWord(data + axis * 4) << 12;
        w_u32(object + 240 + axis * 4, value);
        w_u32(object + 4 + axis * 4, value);
    }
    return data + 12;
}

const uint8 *VisualObject::ReadAngles(const uint8 *data)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062B50, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 value = ReadVisualRecordHalf(data + axis * 2);
        w_u16(object + 16 + axis * 2, value);
    }
    return data + 6;
}

uint32 VisualObject::MoveToTemporaryList(uint32 *head)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 list = VisualListAddress(head);
    FUNCTION_MARKER(0x80062B7C, "SLUS_003.73");
    uint32 table = r_u32(object + 68), receiver, target, result;
    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 56);
    target = r_u32(table + 60);
    InvokeVisualSuspendCallback(target, receiver);
    w_u32(object + 72, list);
    RemoveFromList(head);
    AddToList((uint32 *)psx_addr(0x800FF5E0, 4));
    result = r_u16(object + 78) | 1;
    w_u16(object + 78, result);
    return result;
}

uint32 VisualObject::RestoreList()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062BF0, "SLUS_003.73");
    uint32 result = r_u16(object + 78) & 1;
    if (result)
    {
        RemoveFromList((uint32 *)psx_addr(0x800FF5E0, 4));
        uint32 previous_list = r_u32(object + 72);
        AddToList((uint32 *)psx_addr(previous_list, 4));
        result = r_u16(object + 78) & 0xFFFE;
        w_u16(object + 78, result);
    }
    return result;
}

uint32 VisualObject::UpdateEventLatch(uint32 active, uint32 mask)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80062D24, "SLUS_003.73");
    if (active == 1)
    {
        flags = r_u8(object + 172);
        if (flags & mask)
            return 0;
        w_u8(object + 172, flags | mask);
        return 1;
    }
    flags = r_u8(object + 172);
    w_u8(object + 172, flags & ~mask);
    return 0;
}

uint32 VisualObject::EnableShadow()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062D70, "SLUS_003.73");
    uint32 result = r_u16(object + 78) | 8;
    w_u16(object + 78, result);
    return result;
}

uint32 VisualObject::RemoveShadow()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062D84, "SLUS_003.73");
    uint32 result = r_u16(object + 78) & 0xFFF7;
    uint32 child = r_u32(object + 196);
    w_u16(object + 78, result);
    if (child)
    {
        uint32 table = r_u32(child + 68);
        sint32 adjustment = (sint16)r_u16(table + 8);
        uint32 target = r_u32(table + 12);
        result = apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
        w_u32(object + 196, 0);
    }
    return result;
}

uint32 VisualObject::UpdateShadow()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062DD8, "SLUS_003.73");
    if (!(r_u16(object + 78) & 8))
        return RemoveShadow();
    uint32 child = r_u32(object + 196);
    if (!child)
    {
        w_u32(0x800FF3AC, 0);
        child = AllocateEffectStorage(156);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(TexturedQuad));
            new (dram_addr) TexturedQuad();
        }
        // TODO: Resolve original null allocation before native quad attachment
        w_u32(object + 196, child);
        w_u32(0x800FF3AC, 1);
        TexturedQuad::Attach(child).SelectResource((const char *)psx_addr(0x800FF5F0, 1), 0);
        TexturedQuad::Attach(r_u32(object + 196)).AddFlags(4);
        TexturedQuad::Attach(r_u32(object + 196)).EnableSemiTransparency();
        w_u16(r_u32(object + 196) + 64, 32);
        w_u8(r_u32(object + 196) + 66, 1);
    }
    sint16 direction[3] = {0, -4096, 0};
    uint32 radius = r_u16(object + 200);
    child = r_u32(object + 196);
    TexturedQuad::Attach(child).SetPosition((const uint32 *)psx_addr(object + 184, 12), direction, radius, radius);
    sint32 total = (sint16)r_u16(object + 204);
    sint32 current = (sint16)r_u16(object + 202);
    uint32 numerator = ((uint32)total - (uint32)current) << 7;
    sint32 alpha = total ? (sint32)numerator / total : ((sint32)numerator < 0 ? 1 : -1);
    if (alpha < 0)
        alpha = 0;
    return TexturedQuad::Attach(r_u32(object + 196)).SetBrightness((uint32)alpha & 255);
}

uint32 VisualObject::Finish()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062F0C, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        result = r_u16(object + 78) | 64;
        w_u16(object + 78, result);
    }
    return result;
}

uint32 VisualObject::IsFinished()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062F48, "SLUS_003.73");
    return (r_u16(object + 78) & 64) != 0;
}

extern "C" uint32 sub_80062F48(uint32 object)
{
    // ABI data query also supports original callers before native construction
    FUNCTION_MARKER(0x80062F48, "SLUS_003.73");
    return (r_u16(object + 78) & 64) != 0;
}

extern "C" uint32 sub_80062B7C(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x80062B7C, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::MoveToTemporaryList((uint32 *)psx_addr(list, 4));
}

extern "C" uint32 sub_80062A38(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x80062A38, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::AddToList((uint32 *)psx_addr(list, 4));
}

extern "C" uint32 sub_80062A64(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x80062A64, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::RemoveFromList((uint32 *)psx_addr(list, 4));
}

extern "C" uint32 sub_80062BF0(uint32 object)
{
    FUNCTION_MARKER(0x80062BF0, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::RestoreList();
}

extern "C" uint32 sub_80062B0C(uint32 object, uint32 data)
{
    FUNCTION_MARKER(0x80062B0C, "SLUS_003.73");
    const uint8 *next = VisualObject::Attach(object).VisualObject::ReadPosition((const uint8 *)psx_addr(data, 12));
    return GuestMemoryAddress(next);
}

extern "C" uint32 sub_80062B50(uint32 object, uint32 data)
{
    FUNCTION_MARKER(0x80062B50, "SLUS_003.73");
    const uint8 *next = VisualObject::Attach(object).VisualObject::ReadAngles((const uint8 *)psx_addr(data, 6));
    return GuestMemoryAddress(next);
}

extern "C" uint32 sub_800626C8(uint32 object, uint32 name)
{
    FUNCTION_MARKER(0x800626C8, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::SetModelBank((const char *)psx_addr(name, 1));
}

extern "C" uint32 sub_800626F8(uint32 object, uint32 duration, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800626F8, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::SetColor((sint32)duration, red, green, blue);
}

extern "C" uint32 sub_80062628(uint32 object)
{
    FUNCTION_MARKER(0x80062628, "SLUS_003.73");
    return InitializeScaledObjectBase(object);
}

extern "C" uint32 sub_80062924(uint32 object)
{
    FUNCTION_MARKER(0x80062924, "SLUS_003.73");
    return InitializeVisualObjectBase(object);
}

extern "C" uint32 sub_80062F0C(uint32 object)
{
    FUNCTION_MARKER(0x80062F0C, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::Finish();
}

extern "C" uint32 sub_80062650(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80062650, "SLUS_003.73");
    return DestroyScaledObjectBase(object, reason);
}

extern "C" uint32 sub_800629BC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800629BC, "SLUS_003.73");
    return DestroyVisualObjectBase(object, reason);
}

extern "C" uint32 sub_80062858(uint32 object)
{
    FUNCTION_MARKER(0x80062858, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::AdvanceColor();
}

extern "C" uint32 sub_80062D24(uint32 object, uint32 active, uint32 mask)
{
    FUNCTION_MARKER(0x80062D24, "SLUS_003.73");
    return VisualObject::Attach(object).UpdateEventLatch(active, mask);
}

extern "C" uint32 sub_80062D70(uint32 object)
{
    FUNCTION_MARKER(0x80062D70, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::EnableShadow();
}

extern "C" uint32 sub_80062D84(uint32 object)
{
    FUNCTION_MARKER(0x80062D84, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::RemoveShadow();
}

extern "C" uint32 sub_80062DD8(uint32 object)
{
    FUNCTION_MARKER(0x80062DD8, "SLUS_003.73");
    return VisualObject::Attach(object).VisualObject::UpdateShadow();
}
