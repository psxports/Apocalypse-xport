extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "effect_sprite.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "object_lifecycle.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Native input bridge for the original position and velocity loads

// Native constructors and legacy bridges share the original field operations

// Field adapters preserve legacy construction without changing native type

// TODO Recover the original property name for halfword92

// Legacy entry also serves effects awaiting native construction

// Legacy entry also serves effects awaiting native construction

// Legacy entries also serve effects awaiting native construction

// Legacy entries preserve memory access before all derived effects have native construction

extern "C"
{
}
#include "effect_object.h"

// Native list-head accessors preserve guest-word storage when mapped

// Native methods and legacy entries share guest field operations

// Legacy entry also serves effects awaiting native construction

static uint32 InitializeEffectObjectData(uint32 object)
{
    FUNCTION_MARKER(0x80032F7C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1DB8);
    w_u8(object + 60, 1);
    w_u8(object + 61, 1);
    w_u8(object + 62, 1);
    uint32 count = r_u32(0x800FF3A8);
    w_u32(0x800FF3A8, count + 1);
    return object;
}

static void InitializeRenderableEffectFields(uint32 object)
{
    FUNCTION_MARKER(0x8003304C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D98);
}

static void InitializeEffectControllerEffectFields(uint32 object)
{
    FUNCTION_MARKER(0x800330F4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D80);
}

static void InitializeTexturedEffectFields(uint32 object)
{
    FUNCTION_MARKER(0x8003319C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D68);
    w_u16(object + 92, 128);
    w_u16(object + 94, 400);
    w_u32(object + 76, 0x2C808080);
}

static uint32 DestroyRenderableEffectData(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80033090, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D98);
    sub_80032E7C(object, 0x800FF458);
    uint32 result = DestroyEffectObjectData(object, 0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

static uint32 DestroyEffectControllerData(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80033138, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D80);
    sub_80032E7C(object, 0x800FF434);
    uint32 result = DestroyEffectObjectData(object, 0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

static void SetEffectColorStepData(uint32 object, uint32 step)
{
    FUNCTION_MARKER(0x8003334C, "SLUS_003.73");
    w_u16(object + 72, step);
}

static uint32 SetEffectPositionData(uint32 object, const uint32 *position)
{
    uint32 address = GuestMemoryAddress(position), values[3], axis;
    FUNCTION_MARKER(0x80032EE4, "SLUS_003.73");
    for (axis = 0; axis < 3; ++axis)
    {
        if (address)
            values[axis] = r_u32(address + axis * 4);
        else
            memcpy(&values[axis], (const uint8 *)position + axis * 4, 4);
    }
    w_u32(object + 24, values[0]);
    w_u32(object + 28, values[1]);
    w_u32(object + 32, values[2]);
    return values[0];
}

static uint32 FadeEffectColorsData(uint32 object, uint32 finish_when_empty)
{
    FUNCTION_MARKER(0x80033590, "SLUS_003.73");
    uint32 packed, red, green, blue, result;
    sint32 decrement;
    packed = r_u32(object + 76);
    green = packed >> 8;
    if (!(packed & 0xFFFFFF))
    {
        if (finish_when_empty)
            sub_80032ED8(object);
        return 1;
    }
    red = r_u8(object + 76);
    decrement = (sint16)r_u16(object + 72);
    blue = packed >> 16;
    if ((sint32)red < decrement)
        red = 0;
    else
        red -= r_u8(object + 72);
    decrement = (sint16)r_u16(object + 72);
    if ((sint32)(green & 255) < decrement)
        green = 0;
    else
        green -= r_u8(object + 72);
    if ((sint32)(blue & 255) < decrement)
        blue = 0;
    else
        blue -= r_u8(object + 72);
    result = r_u32(object + 76) & 0xFF000000;
    result |= ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    w_u32(object + 76, result);
    return 0;
}

static void InitializeLineEffectFields(uint32 object)
{
    FUNCTION_MARKER(0x80032824, "SLUS_003.73");
    w_u32(object + 72, 0x50808080);
    w_u32(object + 68, 0x800A1B08);
    w_u32(object + 76, 0x55808080);
}

static uint32 DestroyLineEffectData(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80032880, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B08);
    sub_80032E7C(object, 0x800FF464);
    uint32 result = DestroyEffectObjectData(object, 0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

static uint32 ReadEffectHead(const uint32 *head)
{
    uint32 address = GuestMemoryAddress(head);
    if (address)
        return r_u32(address);
    uint32 value;
    memcpy(&value, head, 4);
    return value;
}

static void WriteEffectHead(uint32 *head, uint32 value)
{
    uint32 address = GuestMemoryAddress(head);
    if (address)
        w_u32(address, value);
    else
        memcpy(head, &value, 4);
}

static uint32 AddEffectListData(uint32 object, uint32 *head)
{
    FUNCTION_MARKER(0x80032E50, "SLUS_003.73");
    uint32 next = ReadEffectHead(head);
    w_u32(object, 0);
    w_u32(object + 4, next);
    WriteEffectHead(head, object);
    next = r_u32(object + 4);
    if (next)
        w_u32(next, object);
    return next;
}

static uint32 RemoveEffectListData(uint32 object, uint32 *head)
{
    FUNCTION_MARKER(0x80032E7C, "SLUS_003.73");
    uint32 next = r_u32(object + 4);
    if (next)
    {
        uint32 previous = r_u32(object);
        w_u32(next, previous);
    }
    uint32 previous = r_u32(object);
    if (previous)
    {
        next = r_u32(object + 4);
        w_u32(previous + 4, next);
    }
    uint32 result = ReadEffectHead(head);
    if (result == object)
    {
        result = r_u32(object + 4);
        WriteEffectHead(head, result);
    }
    return result;
}

EffectObject::EffectObject()
{
    InitializeEffectObjectData(NativeObjectGuestAddress(this));
}

uint32 EffectObject::Destroy(uint32 reason)
{
    return DestroyEffectObjectData(NativeObjectGuestAddress(this), reason);
}

RenderableEffect::RenderableEffect() : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    InitializeRenderableEffectFields(object);
    AddToList((uint32 *)psx_addr(0x800FF458, 4));
}

uint32 RenderableEffect::Destroy(uint32 reason)
{
    return DestroyRenderableEffectData(NativeObjectGuestAddress(this), reason);
}

EffectController::EffectController() : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    InitializeEffectControllerEffectFields(object);
    AddToList((uint32 *)psx_addr(0x800FF434, 4));
}

TexturedEffect::TexturedEffect() : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    InitializeTexturedEffectFields(object);
}

uint32 TexturedEffect::Destroy(uint32 reason)
{
    return DestroyTexturedEffectData(NativeObjectGuestAddress(this), reason);
}

void TexturedEffect::SetColorStep(uint32 step)
{
    SetEffectColorStepData(NativeObjectGuestAddress(this), step);
}

uint32 EffectObject::SetPosition(const uint32 *position)
{
    return SetEffectPositionData(NativeObjectGuestAddress(this), position);
}

uint32 TexturedEffect::FadeColors(uint32 finish_when_empty)
{
    return FadeEffectColorsData(NativeObjectGuestAddress(this), finish_when_empty);
}

uint32 PrimitiveParticle::Update()
{
    // The original empty callback has no defined return under the admitted native ABI
    return 0;
}

PrimitiveParticle &PrimitiveParticle::Attach(uint32 object)
{
    return *static_cast<PrimitiveParticle *>(FindNativeObjectStorage(object));
}

LineEffect::LineEffect() : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    InitializeLineEffectFields(object);
    AddToList((uint32 *)psx_addr(0x800FF464, 4));
}

void LineEffect::Update()
{
    // Shared original empty update at 8003A144
}

uint32 EffectObject::AddToList(uint32 *head)
{
    return AddEffectListData(NativeObjectGuestAddress(this), head);
}

uint32 EffectObject::RemoveFromList(uint32 *head)
{
    return RemoveEffectListData(NativeObjectGuestAddress(this), head);
}

uint32 LineEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80032880, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B08);
    RemoveFromList((uint32 *)psx_addr(0x800FF464, 4));
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 EffectObject::Finish()
{
    FUNCTION_MARKER(0x80032ED8, "SLUS_003.73");
    w_u8(NativeObjectGuestAddress(this) + 63, 1);
    return 1;
}

uint32 DestroyEffectObjectData(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80032FB8, "SLUS_003.73");
    uint32 count = r_u32(0x800FF3A8);
    w_u32(object + 68, 0x800A1DB8);
    w_u32(0x800FF3A8, count - 1);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return 0x800A1DB8;
}

uint32 EffectObject::IntegrateMotion()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80032FFC, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    AddVectors((uint32 *)psx_addr(object + 36, 12), (const uint32 *)psx_addr(object + 48, 12));
    DampVector((uint32 *)psx_addr(object + 36, 12), (const uint8 *)psx_addr(object + 60, 3));
    return object + 36;
}

uint32 EffectController::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80033138, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D80);
    RemoveFromList((uint32 *)psx_addr(0x800FF434, 4));
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 DestroyTexturedEffectData(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800331EC, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D68);
    uint32 result = DestroyEffectObjectData(object, 0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 TexturedEffect::AddFlags(uint32 flags)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033240, "SLUS_003.73");
    uint32 result = r_u8(object + 88) | flags;
    w_u8(object + 88, result);
    return result;
}

void TexturedEffect::SetFrameStep(uint32 value)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033288, "SLUS_003.73");
    w_u16(object + 92, value);
}

void TexturedEffect::SetSize(uint32 size)
{
    FUNCTION_MARKER(0x80033290, "SLUS_003.73");
    w_u16(NativeObjectGuestAddress(this) + 94, size);
}

uint32 TexturedEffect::EnableSemiTransparency()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x800332A4, "SLUS_003.73");
    result = r_u32(object + 76) | 0x2000000;
    w_u32(object + 76, result);
    return result;
}

uint32 TexturedEffect::SetBrightness(uint32 intensity)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800332D0, "SLUS_003.73");
    uint32 color = r_u32(object + 76);
    intensity &= 255;
    color = (color & 0xFF000000) | (intensity << 16) | (intensity << 8) | intensity;
    w_u32(object + 76, color);
    return color;
}

uint32 TexturedEffect::SetColors(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x800332FC, "SLUS_003.73");
    blue &= 255;
    green &= 255;
    result = r_u32(object + 76) & 0xFF000000;
    red &= 255;
    result |= (blue << 16) | (green << 8) | red;
    w_u32(object + 76, result);
    return result;
}

uint32 TexturedEffect::SelectResource(const char *name)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 data, count, current;
    FUNCTION_MARKER(0x80033354, "SLUS_003.73");
    data = FindModelTextureGroup(name);
    w_u32(object + 80, data);
    count = r_u8(data - 4);
    current = r_u32(object + 80);
    w_u8(object + 90, 0);
    w_u8(object + 91, 0);
    w_u8(object + 89, count);
    w_u32(object + 84, current);
    return count;
}

uint32 TexturedEffect::SelectResource(uint32 resource)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 data, count, current;
    FUNCTION_MARKER(0x80033398, "SLUS_003.73");
    data = r_u32(0x800A6808 + resource * 4);
    w_u32(object + 80, data);
    count = r_u8(data - 4);
    current = r_u32(object + 80);
    w_u8(object + 90, 0);
    w_u8(object + 91, 0);
    w_u8(object + 89, count);
    w_u32(object + 84, current);
    return count;
}

uint32 TexturedEffect::SetFrame(uint32 frame)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800333D0, "SLUS_003.73");
    uint32 index, data, offset;
    w_u8(object + 90, frame);
    index = (uint32)(sint32)(sint8)r_u8(object + 90);
    data = r_u32(object + 80);
    w_u8(object + 91, 0);
    offset = index << 3;
    w_u32(object + 84, data + offset);
    return offset;
}

uint32 TexturedEffect::AdvanceFrame()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800333F0, "SLUS_003.73");
    uint32 frame, fraction, speed, phase, data, offset;
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    fraction = r_u8(object + 91);
    speed = r_u16(object + 92);
    phase = (frame << 8) | fraction;
    phase += speed;
    w_u8(object + 90, phase >> 8);
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    w_u8(object + 91, phase);
    data = r_u32(object + 80);
    offset = frame << 3;
    w_u32(object + 84, data + offset);
    return offset;
}

uint32 TexturedEffect::AdvanceLoopingFrame()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033430, "SLUS_003.73");
    uint32 frame, fraction, speed, phase, count, data, offset;
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    fraction = r_u8(object + 91);
    speed = r_u16(object + 92);
    phase = (frame << 8) | fraction;
    phase += speed;
    w_u8(object + 90, phase >> 8);
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    count = r_u8(object + 89);
    w_u8(object + 91, phase);
    if ((sint32)frame >= (sint32)count)
        w_u8(object + 90, 0);
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    data = r_u32(object + 80);
    offset = frame << 3;
    w_u32(object + 84, data + offset);
    return offset;
}

uint32 TexturedEffect::RandomizeFrame()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800334EC, "SLUS_003.73");
    uint32 frame = RandomRange(r_u8(object + 89));
    w_u8(object + 90, frame);
    uint32 offset = (uint32)((sint32)(frame << 24) >> 21);
    uint32 data = r_u32(object + 80);
    w_u32(object + 84, data + offset);
    return offset;
}

uint32 TexturedEffect::Shrink(uint32 finish_when_empty)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033530, "SLUS_003.73");
    sint32 signed_size;
    uint32 size, step;
    signed_size = (sint16)r_u16(object + 94);
    size = r_u16(object + 94);
    if (!signed_size)
    {
        if (finish_when_empty)
            Finish();
        return 1;
    }
    step = r_u16(object + 74);
    size -= step;
    w_u16(object + 94, size);
    if (size & 32768)
        w_u16(object + 94, 0);
    return 0;
}

PrimitiveParticle::PrimitiveParticle() : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80034EF4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1CA8);
    AddToList((uint32 *)psx_addr(0x800FF440, 4));
}

uint32 PrimitiveParticle::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80034F38, "SLUS_003.73");
    w_u32(object + 68, 0x800A1CA8);
    RemoveFromList((uint32 *)psx_addr(0x800FF440, 4));
    result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

extern "C" uint32 sub_80033090(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80033090, "SLUS_003.73");
    return DestroyRenderableEffectData(object, reason);
}

extern "C" uint32 sub_80033138(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80033138, "SLUS_003.73");
    return DestroyEffectControllerData(object, reason);
}

extern "C" void sub_8003334C(uint32 object, uint32 step)
{
    FUNCTION_MARKER(0x8003334C, "SLUS_003.73");
    SetEffectColorStepData(object, step);
}

extern "C" uint32 sub_80033590(uint32 object, uint32 finish_when_empty)
{
    FUNCTION_MARKER(0x80033590, "SLUS_003.73");
    return FadeEffectColorsData(object, finish_when_empty);
}

extern "C" uint32 sub_800332D0(uint32 object, uint32 intensity)
{
    FUNCTION_MARKER(0x800332D0, "SLUS_003.73");
    uint32 color = r_u32(object + 76);
    intensity &= 255;
    color = (color & 0xFF000000) | (intensity << 16) | (intensity << 8) | intensity;
    w_u32(object + 76, color);
    return color;
}

extern "C" uint32 sub_80032ED8(uint32 object)
{
    FUNCTION_MARKER(0x80032ED8, "SLUS_003.73");
    w_u8(object + 63, 1);
    return 1;
}

extern "C" uint32 sub_80034EF4(uint32 object)
{
    FUNCTION_MARKER(0x80034EF4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PrimitiveParticle));
    new (dram_addr) PrimitiveParticle();
    return object;
}

extern "C" uint32 sub_80034F38(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80034F38, "SLUS_003.73");
    return PrimitiveParticle::Attach(object).PrimitiveParticle::Destroy(reason);
}

extern "C" uint32 sub_80032FB8(uint32 object, uint32 flags)
{
    FUNCTION_MARKER(0x80032FB8, "SLUS_003.73");
    return DestroyEffectObjectData(object, flags);
}

extern "C" uint32 sub_80032F7C(uint32 object)
{
    FUNCTION_MARKER(0x80032F7C, "SLUS_003.73");
    return InitializeEffectObjectData(object);
}

extern "C" uint32 sub_80033398(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80033398, "SLUS_003.73");
    uint32 data = r_u32(0x800A6808 + 4 * a2), result, current;
    w_u32(a1 + 80, data);
    result = r_u8(data - 4);
    current = r_u32(a1 + 80);
    w_u8(a1 + 90, 0);
    w_u8(a1 + 91, 0);
    w_u8(a1 + 89, result);
    w_u32(a1 + 84, current);
    return result;
}

extern "C" uint32 sub_80033530(uint32 object, uint32 finish_when_empty)
{
    FUNCTION_MARKER(0x80033530, "SLUS_003.73");
    sint32 signed_size;
    uint32 size, step;
    signed_size = (sint16)r_u16(object + 94);
    size = r_u16(object + 94);
    if (!signed_size)
    {
        if (finish_when_empty)
            sub_80032ED8(object);
        return 1;
    }
    step = r_u16(object + 74);
    size -= step;
    w_u16(object + 94, size);
    if (size & 32768)
        w_u16(object + 94, 0);
    return 0;
}

extern "C" uint32 sub_80033240(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80033240, "SLUS_003.73");
    uint32 result = r_u8(a1 + 88) | a2;
    w_u8(a1 + 88, result);
    return result;
}

extern "C" void sub_80033288(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80033288, "SLUS_003.73");
    w_u16(a1 + 92, a2);
}

extern "C" void sub_80033290(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80033290, "SLUS_003.73");
    w_u16(((uint32)((a1 + 94))), a2);
}

extern "C" uint32 sub_800332A4(uint32 a1)
{
    FUNCTION_MARKER(0x800332A4, "SLUS_003.73");
    sint32 result;
    result = (r_u32(((uint32)((a1 + 76)))) | 0x2000000);
    w_u32(((uint32)((a1 + 76))), result);
    return result;
}

extern "C" uint32 sub_800332FC(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    FUNCTION_MARKER(0x800332FC, "SLUS_003.73");
    uint32 result;
    result = (r_u32(a1 + 76) & 0xFF000000) | ((a4 & 255) << 16) | ((a3 & 255) << 8) | (a2 & 255);
    w_u32(((uint32)((a1 + 76))), result);
    return result;
}

extern "C" uint32 sub_80033354(uint32 object, uint32 name)
{
    FUNCTION_MARKER(0x80033354, "SLUS_003.73");
    uint32 data, count, current;
    data = FindModelTextureGroup(name);
    w_u32(object + 80, data);
    count = r_u8(data - 4);
    current = r_u32(object + 80);
    w_u8(object + 90, 0);
    w_u8(object + 91, 0);
    w_u8(object + 89, count);
    w_u32(object + 84, current);
    return count;
}

extern "C" uint32 sub_800333D0(uint32 object, uint32 frame)
{
    FUNCTION_MARKER(0x800333D0, "SLUS_003.73");
    uint32 index, data, offset;
    w_u8(object + 90, frame);
    index = (uint32)(sint32)(sint8)r_u8(object + 90);
    data = r_u32(object + 80);
    w_u8(object + 91, 0);
    offset = index << 3;
    w_u32(object + 84, data + offset);
    return offset;
}

extern "C" uint32 sub_800333F0(uint32 a1)
{
    FUNCTION_MARKER(0x800333F0, "SLUS_003.73");
    uint32 high = (uint32)(sint32)(signed char)r_u8(a1 + 90);
    uint32 sum = (high << 8) | r_u8(a1 + 91);
    uint32 result;
    sum += r_u16(a1 + 92);
    w_u8(a1 + 90, sum >> 8);
    result = (uint32)(sint32)(signed char)r_u8(a1 + 90) * 8;
    w_u8(a1 + 91, sum);
    w_u32(a1 + 84, r_u32(a1 + 80) + result);
    return result;
}

extern "C" uint32 sub_80033430(uint32 object)
{
    FUNCTION_MARKER(0x80033430, "SLUS_003.73");
    uint32 frame, fraction, speed, phase, count, data, offset;
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    fraction = r_u8(object + 91);
    speed = r_u16(object + 92);
    phase = (frame << 8) | fraction;
    phase += speed;
    w_u8(object + 90, phase >> 8);
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    count = r_u8(object + 89);
    w_u8(object + 91, phase);
    if ((sint32)frame >= (sint32)count)
        w_u8(object + 90, 0);
    frame = (uint32)(sint32)(sint8)r_u8(object + 90);
    data = r_u32(object + 80);
    offset = frame << 3;
    w_u32(object + 84, data + offset);
    return offset;
}

extern "C" uint32 sub_8003304C(uint32 object)
{
    FUNCTION_MARKER(0x8003304C, "SLUS_003.73");
    InitializeEffectObjectData(object);
    InitializeRenderableEffectFields(object);
    sub_80032E50(object, 0x800FF458);
    return object;
}

extern "C" uint32 sub_800330F4(uint32 object)
{
    FUNCTION_MARKER(0x800330F4, "SLUS_003.73");
    InitializeEffectObjectData(object);
    InitializeEffectControllerEffectFields(object);
    sub_80032E50(object, 0x800FF434);
    return object;
}

extern "C" uint32 sub_8003319C(uint32 object)
{
    FUNCTION_MARKER(0x8003319C, "SLUS_003.73");
    InitializeEffectObjectData(object);
    InitializeTexturedEffectFields(object);
    return object;
}

extern "C" uint32 sub_800331EC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800331EC, "SLUS_003.73");
    return DestroyTexturedEffectData(object, reason);
}

extern "C" uint32 sub_80032824(uint32 object)
{
    FUNCTION_MARKER(0x80032824, "SLUS_003.73");
    InitializeEffectObjectData(object);
    InitializeLineEffectFields(object);
    sub_80032E50(object, 0x800FF464);
    return object;
}

extern "C" uint32 sub_80032880(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80032880, "SLUS_003.73");
    return DestroyLineEffectData(object, reason);
}

extern "C" uint32 sub_80032EE4(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80032EE4, "SLUS_003.73");
    return SetEffectPositionData(object, (const uint32 *)psx_addr(position, 12));
}

extern "C" uint32 sub_80032E50(uint32 object, uint32 head)
{
    FUNCTION_MARKER(0x80032E50, "SLUS_003.73");
    return AddEffectListData(object, (uint32 *)psx_addr(head, 4));
}

extern "C" uint32 sub_80032E7C(uint32 object, uint32 head)
{
    FUNCTION_MARKER(0x80032E7C, "SLUS_003.73");
    return RemoveEffectListData(object, (uint32 *)psx_addr(head, 4));
}

extern "C" uint32 sub_80032FFC(uint32 object)
{
    FUNCTION_MARKER(0x80032FFC, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    AddVectors((uint32 *)psx_addr(object + 36, 12), (const uint32 *)psx_addr(object + 48, 12));
    DampVector((uint32 *)psx_addr(object + 36, 12), (const uint8 *)psx_addr(object + 60, 3));
    return object + 36;
}
