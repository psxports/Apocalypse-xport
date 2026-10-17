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
#include "vector_distance.h"
#include "activation_record.h"

// Original list-level operation has a list-head receiver, not an object receiver

// List traversal is a free function, not an object method

#include "textured_quad.h"
#include "effect_lifecycle.h"
#include <new>

VisualLists visual_lists;

void ResetVisualLists()
{
    memset(&visual_lists, 0, sizeof(visual_lists));
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
    model.owner = this;
    model.scale[0] = ONE;
    model.scale[1] = ONE;
    model.scale[2] = ONE;
}

uint32 InitializeScaledObjectBase(uint32 object)
{
    FUNCTION_MARKER(0x80062628, "SLUS_003.73");
    w_u32(object + 68, 0x800A33C0);
    ScaledObject *base = static_cast<ScaledObject *>(FindNativeObjectStorage(object));
    base->model.scale[0] = ONE;
    base->model.scale[1] = ONE;
    base->model.scale[2] = ONE;
    return object;
}

uint32 ScaledObject::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, index;
    FUNCTION_MARKER(0x80062650, "SLUS_003.73");
    flags = model.flags & 1024;
    w_u32(object + 68, 0x800A33C0);
    if (flags)
    {
        index = model.flash;
        ReleaseObjectFlash((uint8)index);
    }
    if (reason & 1)
        return ::ReleaseObjectStorage(object);
    return 0;
}

uint32 DestroyScaledObjectBase(uint32 object, uint32 reason)
{
    uint32 flags, index;
    FUNCTION_MARKER(0x80062650, "SLUS_003.73");
    ScaledObject *base = static_cast<ScaledObject *>(FindNativeObjectStorage(object));
    flags = base->model.flags & 1024;
    w_u32(object + 68, 0x800A33C0);
    if (flags)
    {
        index = base->model.flash;
        ReleaseObjectFlash((uint8)index);
    }
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 VisualObject::SetModelBank(const char *name)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800626C8, "SLUS_003.73");
    uint32 result = FindModelBankByName(name);
    this->model.bank = result;
    this->model.model_idx = 0;
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
        flags = this->model.flags;
        color_transition.timer = 0;
    }
    else if ((sint32)duration < 0)
    {
        divisor = (0 - (uint32)duration) & 65535;
        color_transition.timer = (uint16)divisor;
        dr = DivideVisualColor((sint32)red - 128, divisor);
        dg = DivideVisualColor((sint32)green - 128, color_transition.timer);
        db = DivideVisualColor((sint32)blue - 128, color_transition.timer);
        flags = this->model.flags;
        this->model.color = 0x808080;
        result = flags | 0x400;
        this->model.flags = (uint16)result;
        color_transition.step[0] = (uint16)dr;
        color_transition.step[1] = (uint16)dg;
        color_transition.step[2] = (uint16)db;
        return result;
    }
    else
    {
        dr = (128 - (sint32)red) / (sint32)duration;
        dg = (128 - (sint32)green) / (sint32)duration;
        db = (128 - (sint32)blue) / (sint32)duration;
        color_transition.step[2] = (uint16)db;
        flags = color_transition.saved;
        color_transition.timer = (uint16)((uint32)duration + 1);
        color_transition.step[1] = (uint16)dg;
        color_transition.step[0] = (uint16)dr;
        if (!(flags & 0x1000000))
        {
            uint32 old = this->model.color & 0xFFFFFF;
            flags = this->model.flags;
            color_transition.saved = old | 0x1000000;
            if (flags & 0x400)
                color_transition.saved = old | 0x3000000;
        }
        flags = this->model.flags;
    }
    this->model.color = color;
    result = flags | 0x400;
    this->model.flags = (uint16)result;
    return result;
}

uint32 VisualObject::AdvanceColor()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062858, "SLUS_003.73");
    uint32 timer = color_transition.timer;
    if (!timer)
        return timer;
    uint32 color = this->model.color;
    uint32 red_delta = (uint32)(sint32)(sint16)color_transition.step[0];
    uint32 green_delta = (uint32)(sint32)(sint16)color_transition.step[1];
    uint32 red = ((color & 255) + red_delta) & 255;
    uint32 green = ((((color >> 8) & 255) + green_delta) & 255) << 8;
    uint32 blue_delta = (uint32)(sint32)(sint16)color_transition.step[2];
    uint32 flags = this->model.flags;
    uint32 blue = ((((color >> 16) & 255) + blue_delta) & 255) << 16;
    this->model.flags = flags | 1024;
    timer = color_transition.timer;
    uint32 result = red | green | blue;
    this->model.color = result;
    timer = (timer - 1) & 65535;
    color_transition.timer = timer;
    if (!timer)
    {
        uint32 restore_flags = color_transition.saved;
        flags = this->model.flags;
        flags = (restore_flags & 0x2000000) ? flags | 1024 : flags & 0xFBFF;
        this->model.flags = flags;
        result = color_transition.saved;
        color_transition.saved = 0;
        this->model.color = result;
    }
    return result;
}

VisualObject::VisualObject() : kind(0), activation_flags(0), lifecycle_flags(0), target_mark(0), event_flags(0), record_idx(0), saved_head(0) : ScaledObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, value;
    FUNCTION_MARKER(0x80062924, "SLUS_003.73");
    w_u32(object + 68, 0x800A3380);
    motion.drag[0] = 1;
    motion.drag[1] = 1;
    motion.drag[2] = 1;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    w_u32(object + 208, 10);
    w_u16(object + 148, 0);
    this->record_idx = 65535;
    value = r_u16(0x800EC682);
    w_u16(object + 200, value);
    flags = this->lifecycle_flags;
    value = r_u16(0x800EC680);
    this->lifecycle_flags = flags | 18;
    w_u16(object + 212, 50);
    w_u16(object + 204, value);
}

uint32 InitializeVisualObjectBase(uint32 object)
{
    uint32 flags, value;
    FUNCTION_MARKER(0x80062924, "SLUS_003.73");
    InitializeScaledObjectBase(object);
    w_u32(object + 68, 0x800A3380);
    VisualObject::Attach(object).motion.drag[0] = 1;
    VisualObject::Attach(object).motion.drag[1] = 1;
    VisualObject::Attach(object).motion.drag[2] = 1;
    VisualObject::Attach(object).motion.angular_drag[0] = 1;
    VisualObject::Attach(object).motion.angular_drag[1] = 1;
    VisualObject::Attach(object).motion.angular_drag[2] = 1;
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

uint32 VisualObject::Update()
{
    FUNCTION_MARKER(0x800639AC, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::UpdateLighting()
{
    FUNCTION_MARKER(0x8006399C, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::ReleaseResources()
{
    FUNCTION_MARKER(0x80063994, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::Damage(uint32, const sint32 *, uint32)
{
    FUNCTION_MARKER(0x80062F5C, "SLUS_003.73");
    return 1;
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
        return ::ReleaseObjectStorage(object);
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

ModelInstance *VisualObject::AddToList(ModelInstance *&head)
{
    FUNCTION_MARKER(0x80062A38, "SLUS_003.73");
    model.prev = 0;
    model.next = head;
    head = &model;
    if (model.next)
        model.next->prev = &model;
    return model.next;
}

ModelInstance *VisualObject::RemoveFromList(ModelInstance *&head)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062A64, "SLUS_003.73");
    if ((this->lifecycle_flags & 1) && &head != &visual_lists.suspended)
        RestoreList();
    if (model.next)
        model.next->prev = model.prev;
    if (model.prev)
        model.prev->next = model.next;
    ModelInstance *result = head;
    if (result == &model)
    {
        result = model.next;
        head = result;
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
        pos_history.saved[axis] = value;
        this->model.pos[axis] = value;
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
        this->model.angles[axis] = value;
    }
    return data + 6;
}

uint32 VisualObject::MoveToTemporaryList(ModelInstance *&head)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062B7C, "SLUS_003.73");
    ReleaseResources();
    saved_head = &head;
    RemoveFromList(head);
    AddToList(visual_lists.suspended);
    uint32 result = this->lifecycle_flags | 1;
    this->lifecycle_flags = result;
    return result;
}

uint32 VisualObject::RestoreList()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062BF0, "SLUS_003.73");
    uint32 result = this->lifecycle_flags & 1;
    if (result)
    {
        RemoveFromList(visual_lists.suspended);
        AddToList(*saved_head);
        result = this->lifecycle_flags & 0xFFFE;
        this->lifecycle_flags = result;
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
        flags = this->event_flags;
        if (flags & mask)
            return 0;
        this->event_flags = flags | mask;
        return 1;
    }
    flags = this->event_flags;
    this->event_flags = flags & ~mask;
    return 0;
}

uint32 VisualObject::EnableShadow()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062D70, "SLUS_003.73");
    uint32 result = this->lifecycle_flags | 8;
    this->lifecycle_flags = result;
    return result;
}

uint32 VisualObject::RemoveShadow()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062D84, "SLUS_003.73");
    uint32 result = this->lifecycle_flags & 0xFFF7;
    uint32 child = r_u32(object + 196);
    this->lifecycle_flags = result;
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
    if (!(this->lifecycle_flags & 8))
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
        result = this->lifecycle_flags | 64;
        this->lifecycle_flags = result;
    }
    return result;
}

uint32 VisualObject::IsFinished()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062F48, "SLUS_003.73");
    return (this->lifecycle_flags & 64) != 0;
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


