extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "textured_quad.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TexturedQuad::TexturedQuad() : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033688, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D50);
    w_u32(object + 144, 0x2C808080);
    AddToList((uint32 *)psx_addr(0x800FF444, 4));
}

uint32 TexturedQuad::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800336D8, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D50);
    RemoveFromList((uint32 *)psx_addr(0x800FF444, 4));
    uint32 result = EffectObject::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

uint32 TexturedQuad::SelectResource(const char *name, uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033764, "SLUS_003.73");
    uint32 resource = FindModelTextureGroup(name);
    uint32 result = r_u32(resource + (index << 3) + 4);
    w_u32(object + 148, result);
    return result;
}

uint32 TexturedQuad::AddFlags(uint32 flags)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003384C, "SLUS_003.73");
    uint32 result = r_u8(object + 152) | flags;
    w_u8(object + 152, result);
    return result;
}

uint32 TexturedQuad::EnableSemiTransparency()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033874, "SLUS_003.73");
    uint32 result = r_u32(object + 144) | 0x2000000;
    w_u32(object + 144, result);
    return result;
}

uint32 TexturedQuad::SetBrightness(uint32 brightness)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800338A0, "SLUS_003.73");
    uint32 result = r_u32(object + 144) & 0xFF000000;
    brightness &= 255;
    result |= (brightness << 16) | (brightness << 8) | brightness;
    w_u32(object + 144, result);
    return result;
}

TexturedQuad &TexturedQuad::Attach(uint32 object)
{
    return *static_cast<TexturedQuad *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80033688(uint32 object)
{
    FUNCTION_MARKER(0x80033688, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TexturedQuad));
    new (dram_addr) TexturedQuad();
    return object;
}

extern "C" uint32 sub_800336D8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800336D8, "SLUS_003.73");
    return TexturedQuad::Attach(object).TexturedQuad::Destroy(reason);
}

extern "C" uint32 sub_80033764(uint32 object, uint32 name, uint32 index)
{
    FUNCTION_MARKER(0x80033764, "SLUS_003.73");
    return TexturedQuad::Attach(object).SelectResource((const char *)psx_addr(name, 1), index);
}

extern "C" uint32 sub_8003384C(uint32 object, uint32 flags)
{
    FUNCTION_MARKER(0x8003384C, "SLUS_003.73");
    return TexturedQuad::Attach(object).AddFlags(flags);
}

extern "C" uint32 sub_80033874(uint32 object)
{
    FUNCTION_MARKER(0x80033874, "SLUS_003.73");
    return TexturedQuad::Attach(object).EnableSemiTransparency();
}

extern "C" uint32 sub_800338A0(uint32 object, uint32 brightness)
{
    FUNCTION_MARKER(0x800338A0, "SLUS_003.73");
    return TexturedQuad::Attach(object).SetBrightness(brightness);
}

uint32 TexturedQuad::SelectResource(uint32 key)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800337B4, "SLUS_003.73");
    uint32 result = FindModelResourceEntry(key);
    w_u32(object + 148, result);
    return result;
}

uint32 TexturedQuad::SetColors(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800338CC, "SLUS_003.73");
    uint32 result = (r_u32(object + 144) & 0xFF000000) | ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    w_u32(object + 144, result);
    return result;
}

extern "C" uint32 sub_800337B4(uint32 object, uint32 key)
{
    FUNCTION_MARKER(0x800337B4, "SLUS_003.73");
    return TexturedQuad::Attach(object).SelectResource(key);
}

extern "C" uint32 sub_800338CC(uint32 object, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800338CC, "SLUS_003.73");
    return TexturedQuad::Attach(object).SetColors(red, green, blue);
}

// Native input adapter for original signed direction halfword loads
static uint32 ReadQuadDirection(const sint16 *direction, uint32 offset)
{
    uint32 address = GuestMemoryAddress(direction);
    if (address)
        return (uint32)(sint32)(sint16)r_u16(address + offset);
    sint16 value;
    memcpy(&value, (const uint8 *)direction + offset, 2);
    return (uint32)(sint32)value;
}

void TexturedQuad::SetPosition(const uint32 *position, const sint16 *direction, uint32 first_scale, uint32 second_scale)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80033900, "SLUS_003.73");
    uint32 first[3], second[3], out[3], length1, length2;
    uint32 buffer1[3], buffer2[3];
    uint32 x, y, z, product;
    first[0] = 0;
    z = ReadQuadDirection(direction, 4);
    first[1] = 0 - z;
    y = ReadQuadDirection(direction, 2);
    first[2] = y;
    y = ReadQuadDirection(direction, 2);
    product = y * y;
    z = ReadQuadDirection(direction, 4);
    second[0] = product + z * z;
    x = ReadQuadDirection(direction, 0);
    y = ReadQuadDirection(direction, 2);
    second[1] = (0 - x) * y;
    z = ReadQuadDirection(direction, 4);
    x = ReadQuadDirection(direction, 0);
    second[2] = (0 - z) * x;
    length1 = MeasureVectorLength(first);
    length2 = MeasureVectorLength(second);
    MultiplyVector(buffer1, first, &first_scale);
    DivideVector(out, buffer1, &length1);
    first[0] = out[0];
    first[1] = out[1];
    first[2] = out[2];
    MultiplyVector(buffer2, second, &second_scale);
    DivideVector(out, buffer2, &length2);
    second[0] = out[0];
    second[1] = out[1];
    second[2] = out[2];
    SubtractVectors(buffer1, position, first);
    SubtractVectors(out, buffer1, second);
    w_u32(object + 24, out[0]);
    w_u32(object + 28, out[1]);
    w_u32(object + 32, out[2]);
    AddVectors(buffer1, position, first);
    SubtractVectors(out, buffer1, second);
    w_u32(object + 72, out[0]);
    w_u32(object + 76, out[1]);
    w_u32(object + 80, out[2]);
    SubtractVectors(buffer1, position, first);
    AddVectors(out, buffer1, second);
    w_u32(object + 84, out[0]);
    w_u32(object + 88, out[1]);
    w_u32(object + 92, out[2]);
    AddVectors(buffer1, position, first);
    AddVectors(out, buffer1, second);
    w_u32(object + 96, out[0]);
    w_u32(object + 100, out[1]);
    w_u32(object + 104, out[2]);
}

extern "C" uint32 sub_80033900(uint32 object, uint32 position, uint32 direction, uint32 first_scale, uint32 second_scale, uint32, uint32)
{
    FUNCTION_MARKER(0x80033900, "SLUS_003.73");
    TexturedQuad::Attach(object).SetPosition((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(direction, 6), first_scale, second_scale);
    // Native ABI excludes the original local-stack pointer carrier
    return 0;
}

extern "C" uint32 apocalypse_shadow_quad_native(uint32 object, uint32 position, const void *direction, uint32 first_scale, uint32 second_scale, uint32, uint32)
{
    TexturedQuad::Attach(object).SetPosition((const uint32 *)psx_addr(position, 12), (const sint16 *)direction, first_scale, second_scale);
    // Native ABI excludes the original local-stack pointer carrier
    return 0;
}

extern "C" uint32 apocalypse_shadow_quad_position_native(uint32 object, const void *position, const void *direction, uint32 first_scale, uint32 second_scale, uint32, uint32)
{
    TexturedQuad::Attach(object).SetPosition((const uint32 *)position, (const sint16 *)direction, first_scale, second_scale);
    // Native ABI excludes the original local-stack pointer carrier
    return 0;
}

static void CopyQuadPosition(uint32 destination, uint32 source)
{
    uint32 x = r_u32(source);
    uint32 y = r_u32(source + 4);
    uint32 z = r_u32(source + 8);
    w_u32(destination, x);
    w_u32(destination + 4, y);
    w_u32(destination + 8, z);
}

static void SmoothQuadPosition(uint32 position, uint32 history)
{
    uint32 current[3], previous_quarter[3], current_quarter[3], weighted[3], output[3];
    uint32 shift = 2, weight = 3, axis;
    for (axis = 0; axis < 3; ++axis)
        current[axis] = r_u32(position + axis * 4);
    ShiftVectorRight(previous_quarter, (const uint32 *)psx_addr(history, 12), &shift);
    ShiftVectorRight(current_quarter, current, &shift);
    MultiplyScalarVector(weighted, &weight, current_quarter);
    AddVectors(output, previous_quarter, weighted);
    for (axis = 0; axis < 3; ++axis)
        w_u32(position + axis * 4, output[axis]);
    for (axis = 0; axis < 3; ++axis)
        w_u32(history + axis * 4, current[axis]);
}

void SmoothTexturedQuadPositions(void)
{
    FUNCTION_MARKER(0x800303F4, "SLUS_003.73");
    uint32 object = r_u32(0x800FF444);
    while (object)
    {
        SmoothQuadPosition(object + 24, object + 12);
        SmoothQuadPosition(object + 72, object + 108);
        SmoothQuadPosition(object + 84, object + 120);
        SmoothQuadPosition(object + 96, object + 132);
        object = r_u32(object + 4);
    }
}

uint32 SaveTexturedQuadPositions(void)
{
    FUNCTION_MARKER(0x800306D8, "SLUS_003.73");
    uint32 object = r_u32(0x800FF444);
    while (object)
    {
        CopyQuadPosition(object + 12, object + 24);
        CopyQuadPosition(object + 108, object + 72);
        CopyQuadPosition(object + 120, object + 84);
        CopyQuadPosition(object + 132, object + 96);
        object = r_u32(object + 4);
    }
    return object;
}

uint32 RestoreTexturedQuadPositions(void)
{
    FUNCTION_MARKER(0x80030764, "SLUS_003.73");
    uint32 object = r_u32(0x800FF444);
    while (object)
    {
        CopyQuadPosition(object + 24, object + 12);
        CopyQuadPosition(object + 72, object + 108);
        CopyQuadPosition(object + 84, object + 120);
        CopyQuadPosition(object + 96, object + 132);
        object = r_u32(object + 4);
    }
    return object;
}

extern "C" void sub_800303F4(void)
{
    FUNCTION_MARKER(0x800303F4, "SLUS_003.73");
    SmoothTexturedQuadPositions();
}
extern "C" uint32 sub_800306D8(void)
{
    FUNCTION_MARKER(0x800306D8, "SLUS_003.73");
    return SaveTexturedQuadPositions();
}
extern "C" uint32 sub_80030764(void)
{
    FUNCTION_MARKER(0x80030764, "SLUS_003.73");
    return RestoreTexturedQuadPositions();
}

