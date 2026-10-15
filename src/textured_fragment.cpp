extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "textured_fragment.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "sprite_trail.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

// Native boundary preserves mapped halfword accesses
static sint16 ReadFragmentHalf(const sint16 *input, uint32 axis)
{
    uint32 address = GuestMemoryAddress(input);
    sint16 value;
    if (address)
        return (sint16)r_u16(address + axis * 2);
    memcpy(&value, (const uint8 *)input + axis * 2, 2);
    return value;
}

TexturedTriangle::TexturedTriangle(const sint16 *vertex0, const sint16 *vertex1, const sint16 *vertex2) : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const sint16 *vertices[3] = {vertex0, vertex1, vertex2};
    uint32 axis, index, first[3], second[3], cross[3], normal[3], factor;
    FUNCTION_MARKER(0x800340A4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D20);
    w_u8(object + 67, 6);
    for (index = 0; index < 3; ++index)
        for (axis = 0; axis < 3; ++axis)
            w_u16(object + 72 + index * 8 + axis * 2, ReadFragmentHalf(vertices[index], axis));
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 a = (uint32)(sint32)ReadFragmentHalf(vertex2, axis);
        uint32 b = (uint32)(sint32)ReadFragmentHalf(vertex0, axis);
        first[axis] = a - b;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 a = (uint32)(sint32)ReadFragmentHalf(vertex1, axis);
        uint32 b = (uint32)(sint32)ReadFragmentHalf(vertex0, axis);
        second[axis] = a - b;
    }
    cross[0] = first[1] * second[2] - first[2] * second[1];
    cross[1] = first[2] * second[0] - first[0] * second[2];
    cross[2] = first[0] * second[1] - first[1] * second[0];
    NormalizeFixedVector(normal, cross);
    factor = RandomRange(96) + 128;
    for (axis = 0; axis < 3; ++axis)
        w_u16(object + 96 + axis * 2, (sint32)(normal[axis] * factor) >> 12);
    AddToList((uint32 *)psx_addr(0x800FF44C, 4));
}

uint32 TexturedTriangle::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800342B0, "SLUS_003.73");
    w_u32(object + 68, 0x800A1D20);
    RemoveFromList((uint32 *)psx_addr(0x800FF44C, 4));
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

void TexturedTriangle::Update()
{
    FUNCTION_MARKER(0x8003A144, "SLUS_003.73");
}

TexturedTriangle &TexturedTriangle::Attach(uint32 object)
{
    return *static_cast<TexturedTriangle *>(FindNativeObjectStorage(object));
}

uint32 TexturedTriangle::TransformVertices()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 matrix[8], vertex, axis, input, output;
    FUNCTION_MARKER(0x800344BC, "SLUS_003.73");
    BuildModelRotationMatrix((const uint32 *)psx_addr(object + 152, 8), (uint16 *)matrix);
    SetRotMatrix((MATRIX *)matrix);
    LoadFixedGteTranslation((const uint32 *)psx_addr(object + 24, 12));
    // Original fourth input is the normal, not another triangle corner
    for (vertex = 0; vertex < 4; ++vertex)
    {
        input = object + 72 + vertex * 8;
        output = object + 104 + vertex * 12;
        xport_gte_write_data(0, r_u32(input));
        xport_gte_write_data(1, r_u32(input + 4));
        xport_gte_execute(0x480012);
        for (axis = 0; axis < 3; ++axis)
            w_u32(output + axis * 4, xport_gte_read_data(25 + axis));
    }
    return object + 96;
}

uint32 TexturedTriangle::SetColors(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index, factor, r, g, b, result = 0;
    FUNCTION_MARKER(0x80034314, "SLUS_003.73");
    red &= 255;
    green &= 255;
    blue &= 255;
    w_u32(object + 172, (blue << 16) | (green << 8) | red);
    for (index = 0; index < 3; ++index)
    {
        factor = RandomRange(4096);
        b = (uint32)((sint32)(blue * factor) >> 12);
        g = (uint32)((sint32)(green * factor) >> 12);
        r = (uint32)((sint32)(red * factor) >> 12);
        w_u32(object + 176 + index * 4, (b << 16) | (g << 8) | r);
        result = r;
    }
    return result;
}

uint32 TexturedTriangle::SetTextureCoordinates(uint32 clut, uint32 tpage, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8003445C, "SLUS_003.73");
    w_u32(object + 160, (clut << 16) | ((v0 & 255) << 8) | (u0 & 255));
    w_u32(object + 164, (tpage << 16) | ((v1 & 255) << 8) | (u1 & 255));
    result = (u2 & 255) | ((v2 & 255) << 8);
    w_u16(object + 168, result);
    return result;
}

TexturedFragment::TexturedFragment(const sint16 *vertex0, const sint16 *vertex1, const sint16 *vertex2, const uint32 *position) : TexturedTriangle(vertex0, vertex1, vertex2)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, angle;
    FUNCTION_MARKER(0x80020FB4, "SLUS_003.73");
    w_u32(object + 68, 0x800A12C8);
    if (RandomRange(2))
    {
        angle = RandomRange(160) + 80;
        w_u16(object + 192, angle);
        w_u16(object + 194, 0);
        w_u16(object + 196, 0);
    }
    else
    {
        angle = RandomRange(160) + 80;
        w_u16(object + 192, 0);
        w_u16(object + 194, angle);
        w_u16(object + 196, 0);
    }
    w_u16(object + 10, RandomRange(30) + 45);
    InitializeVelocity(position);
    TransformVertices();
    if (RandomRange(2) && !r_u32(0x800FF738))
    {
        child = AllocateEffectStorage(100);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)psx_addr(object + 24, 12), 4, 128, 128, 128);
        }
        // TODO: Resolve original null allocation writes before native trail attachment
        w_u32(object + 200, child);
        w_u8(child + 66, 1);
        RibbonTrail::Attach(r_u32(object + 200)).SetWidth(1200);
        RibbonTrail::Attach(r_u32(object + 200)).SetColors(32, 32, 32);
    }
}

TexturedFragment &TexturedFragment::Attach(uint32 object)
{
    return *static_cast<TexturedFragment *>(FindNativeObjectStorage(object));
}

uint32 TexturedFragment::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800210D8, "SLUS_003.73");
    uint32 child = r_u32(object + 200), table;
    w_u32(object + 68, 0x800A12C8);
    if (child)
    {
        table = r_u32(child + 68);
        sint32 adjustment = (sint16)r_u16(table + 8);
        uint32 target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    TexturedTriangle::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

uint32 TexturedFragment::InitializeVelocity(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 difference[3], scaled[3], velocity[3], distance, factor, normal, axis, value, result, child;
    uint32 shift = 1;
    FUNCTION_MARKER(0x80021154, "SLUS_003.73");
    SetPosition(position);
    SubtractVectors(difference, position, (const uint32 *)psx_addr(0x800FFD58, 12));
    distance = MeasureVectorLength(difference);
    if (distance)
    {
        factor = RandomRange(48);
        MultiplyScalarVector(scaled, &factor, difference);
        DivideVector(velocity, scaled, &distance);
        for (axis = 0; axis < 3; ++axis)
            w_u32(object + 36 + axis * 4, velocity[axis]);
    }
    else
        Finish();
    factor = RandomRange(45);
    normal = r_u32(0x800FF210);
    for (axis = 0; axis < 3; ++axis)
    {
        value = (uint32)(sint32)(sint16)r_u16(normal + axis * 2);
        value = r_u32(object + 36 + axis * 4) + factor * value;
        w_u32(object + 36 + axis * 4, value);
    }
    value = RandomRange(175);
    w_u32(object + 40, r_u32(object + 40) - (value << 12));
    if (!RandomRange(20))
        ShiftVectorLeft((uint32 *)psx_addr(object + 36, 12), &shift);
    result = r_u32(object + 40);
    if ((sint32)result > 0)
        w_u32(object + 40, 0);
    child = r_u32(object + 200);
    if (child)
        return RibbonTrail::Attach(child).SetPosition((const uint32 *)psx_addr(object + 24, 12));
    return result;
}

void TexturedFragment::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, lifetime;
    FUNCTION_MARKER(0x800212C8, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    w_u32(object + 40, r_u32(object + 40) + 20500);
    AddAngleVector((uint16 *)psx_addr(object + 152, 6), (const uint16 *)psx_addr(object + 192, 6));
    child = r_u32(object + 200);
    if (child)
        RibbonTrail::Attach(child).SetPosition((const uint32 *)psx_addr(object + 24, 12));
    TransformVertices();
    lifetime = r_u16(object + 10);
    if (!lifetime)
    {
        Finish();
        return;
    }
    w_u16(object + 10, lifetime - 1);
}

extern "C" uint32 xport_draft_host_sub_80020FB4_p2345(uint32 object, void *vertex0, void *vertex1, void *vertex2, void *position)
{
    FUNCTION_MARKER(0x80020FB4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TexturedFragment));
    new (dram_addr) TexturedFragment((const sint16 *)vertex0, (const sint16 *)vertex1, (const sint16 *)vertex2, (const uint32 *)position);
    return object;
}

extern "C" uint32 sub_80020FB4(uint32 object, uint32 vertex0, uint32 vertex1, uint32 vertex2, uint32 position)
{
    FUNCTION_MARKER(0x80020FB4, "SLUS_003.73");
    return xport_draft_host_sub_80020FB4_p2345(object, psx_addr(vertex0, 6), psx_addr(vertex1, 6), psx_addr(vertex2, 6), psx_addr(position, 12));
}

extern "C" uint32 sub_80021154(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80021154, "SLUS_003.73");
    return TexturedFragment::Attach(object).InitializeVelocity((const uint32 *)psx_addr(position, 12));
}

extern "C" uint32 sub_800210D8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800210D8, "SLUS_003.73");
    return TexturedFragment::Attach(object).TexturedFragment::Destroy(reason);
}

extern "C" void sub_800212C8(uint32 object)
{
    FUNCTION_MARKER(0x800212C8, "SLUS_003.73");
    TexturedFragment::Attach(object).TexturedFragment::Update();
}

extern "C" uint32 sub_800344BC(uint32 object)
{
    FUNCTION_MARKER(0x800344BC, "SLUS_003.73");
    return TexturedTriangle::Attach(object).TransformVertices();
}

extern "C" uint32 sub_80034314(uint32 object, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x80034314, "SLUS_003.73");
    return TexturedTriangle::Attach(object).SetColors(red, green, blue);
}

extern "C" uint32 sub_8003445C(uint32 object, uint32 clut, uint32 tpage, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2)
{
    FUNCTION_MARKER(0x8003445C, "SLUS_003.73");
    return TexturedTriangle::Attach(object).SetTextureCoordinates(clut, tpage, u0, v0, u1, v1, u2, v2);
}

extern "C" uint32 sub_800340A4(uint32 object, uint32 vertex0, uint32 vertex1, uint32 vertex2)
{
    FUNCTION_MARKER(0x800340A4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TexturedTriangle));
    new (dram_addr) TexturedTriangle((const sint16 *)psx_addr(vertex0, 6), (const sint16 *)psx_addr(vertex1, 6), (const sint16 *)psx_addr(vertex2, 6));
    return object;
}

extern "C" uint32 sub_800342B0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800342B0, "SLUS_003.73");
    return TexturedTriangle::Attach(object).TexturedTriangle::Destroy(reason);
}
