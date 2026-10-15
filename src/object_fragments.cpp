#include "angle_math.h"
#include "game_random.h"
#include "vector_distance.h"
#include "effect_lifecycle.h"
#include "object_collision_events.h"
#include "vector_arithmetic.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "object_fragments.h"
#include "native_object_storage.h"
#include "textured_fragment.h"
#include "opaque_fragment.h"
#include "ring_burst_effect.h"
#include <new>
#include <string.h>

uint32 *TransformFragmentVertex(uint32 *output, uint32 geometry, uint32 indices, uint32 index)
{
    uint32 input = geometry + (r_u16(indices + index * 2) & 0xFFF8);
    uint32 axis, value, address = GuestMemoryAddress(output);
    uint32 shift = 12;
    FUNCTION_MARKER(0x80021ADC, "SLUS_003.73");
    xport_gte_write_data(0, r_u32(input));
    xport_gte_write_data(1, r_u32(input + 4));
    xport_gte_execute(0x480012);
    for (axis = 0; axis < 3; ++axis)
    {
        value = xport_gte_read_data(25 + axis);
        if (address)
            w_u32(address + axis * 4, value);
        else
            memcpy((uint8 *)output + axis * 4, &value, 4);
    }
    return ShiftVectorLeft(output, &shift);
}

uint32 SetFragmentColor(uint32 indexed, uint32 colors, uint32 count, uint32 palette)
{
    uint32 first, second, third, fourth, red, green, blue, result;
    FUNCTION_MARKER(0x80021B3C, "SLUS_003.73");
    if (indexed)
    {
        first = r_u32(palette + (colors & 255) * 4);
        second = r_u32(palette + ((colors >> 6) & 1020));
        third = r_u32(palette + ((colors >> 14) & 1020));
        red = (first & 255) + (second & 255) + (third & 255);
        green = ((first >> 8) & 255) + ((second >> 8) & 255) + ((third >> 8) & 255);
        blue = ((first >> 16) & 255) + ((second >> 16) & 255) + ((third >> 16) & 255);
        if (count == 4)
        {
            fourth = r_u32(palette + (colors >> 24) * 4);
            red += fourth & 255;
            green += (fourth >> 8) & 255;
            blue += (fourth >> 16) & 255;
            w_u8(0x800FFB80, red / 4);
            w_u8(0x800FFB81, green / 4);
            result = blue / 4;
        }
        else
        {
            // Byte-channel sums are nonnegative and at most 765
            w_u8(0x800FFB80, red / 3);
            w_u8(0x800FFB81, green / 3);
            result = blue / 3;
        }
    }
    else
    {
        w_u8(0x800FFB81, colors >> 8);
        result = colors >> 16;
        w_u8(0x800FFB80, colors);
    }
    w_u8(0x800FFB82, result);
    return result;
}

uint32 SpawnTexturedFragments(const uint32 *vertex0, const uint32 *vertex1, const uint32 *vertex2, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2, uint32 texture, uint32 subdivisions)
{
    FUNCTION_MARKER(0x80021358, "SLUS_003.73");
    uint32 vector[3], difference[3], scaled[3], midpoint[3];
    uint32 points[3][3], center[3];
    sint16 vertices[3][4], jitter[4], shifted_jitter[4];
    sint32 length, candidate;
    uint32 factor, shift;
    uint32 edge = 0, object, i, component;
    uint32 u, v, result = r_u32(0x800FF738);
    if (result != 0)
        return result;
    if (subdivisions != 0)
    {
        SubtractVectors(vector, vertex1, vertex2);
        length = (sint32)MeasureVectorLength(vector);
        SubtractVectors(vector, vertex0, vertex2);
        candidate = (sint32)MeasureVectorLength(vector);
        if (length < candidate)
        {
            length = candidate;
            edge = 1;
        }
        SubtractVectors(vector, vertex0, vertex1);
        if (length < (sint32)MeasureVectorLength(vector))
            edge = 2;
        factor = RandomRange(9);
        if (edge == 1)
            SubtractVectors(difference, vertex2, vertex0);
        else if (edge == 2)
            SubtractVectors(difference, vertex1, vertex0);
        else
            SubtractVectors(difference, vertex2, vertex1);
        MultiplyVector(scaled, difference, &factor);
        shift = 3;
        ShiftVectorRight(midpoint, scaled, &shift);
        AddVectors(vector, edge == 0 ? vertex1 : vertex0, midpoint);
        if (edge == 1)
        {
            u = u0 + (uint32)((sint32)((u2 - u0) * (uint32)factor) >> 3);
            v = v0 + (uint32)((sint32)((v2 - v0) * (uint32)factor) >> 3);
            SpawnTexturedFragments(vertex1, vertex0, vector, u1, v1, u0, v0, u, v, texture, subdivisions - 1);
            return SpawnTexturedFragments(vertex1, vertex2, vector, u1, v1, u2, v2, u, v, texture, subdivisions - 1);
        }
        if (edge == 2)
        {
            u = u0 + (uint32)((sint32)((u1 - u0) * (uint32)factor) >> 3);
            v = v0 + (uint32)((sint32)((v1 - v0) * (uint32)factor) >> 3);
            SpawnTexturedFragments(vertex2, vertex0, vector, u2, v2, u0, v0, u, v, texture, subdivisions - 1);
            return SpawnTexturedFragments(vertex2, vertex1, vector, u2, v2, u1, v1, u, v, texture, subdivisions - 1);
        }
        u = u1 + (uint32)((sint32)((u2 - u1) * (uint32)factor) >> 3);
        v = v1 + (uint32)((sint32)((v2 - v1) * (uint32)factor) >> 3);
        SpawnTexturedFragments(vertex0, vertex1, vector, u0, v0, u1, v1, u, v, texture, subdivisions - 1);
        return SpawnTexturedFragments(vertex0, vertex2, vector, u0, v0, u2, v2, u, v, texture, subdivisions - 1);
    }
    shift = 12;
    ShiftVectorRight(points[0], vertex0, &shift);
    ShiftVectorRight(points[1], vertex1, &shift);
    ShiftVectorRight(points[2], vertex2, &shift);
    for (component = 0; component < 3; ++component)
    {
        center[component] = (sint32)((uint32)points[0][component] + (uint32)points[1][component] + (uint32)points[2][component]) / 3;
        for (i = 0; i < 3; ++i)
            vertices[i][component] = (sint16)((uint32)points[i][component] - (uint32)center[component]);
    }
    for (i = 0; i < 3; ++i)
    {
        factor = RandomRange(100);
        MultiplyScalarShortVector((uint16 *)jitter, &factor, (const uint16 *)psx_addr(r_u32(0x800FF210), 6));
        ShiftShortVectorRight((uint16 *)shifted_jitter, (const uint16 *)jitter, &shift);
        AddAngleVector((uint16 *)vertices[i], (const uint16 *)shifted_jitter);
    }
    ShiftVectorLeft(center, &shift);
    w_u32(0x800FF3AC, 0);
    object = AllocateEffectStorage(204);
    if (object != 0)
    {
        void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TexturedFragment));
        new (dram_addr) TexturedFragment(vertices[0], vertices[1], vertices[2], center);
    }
    uint32 red = r_u8(0x800FFB80);
    uint32 green = r_u8(0x800FFB81);
    uint32 blue = r_u8(0x800FFB82);
    w_u32(0x800FF3AC, 1);
    TexturedFragment::Attach(object).SetColors(red, green, blue);
    uint32 clut = r_u16(0x800FFB78);
    uint32 tpage = r_u16(0x800FFB7C);
    result = TexturedFragment::Attach(object).SetTextureCoordinates(clut, tpage, u0, v0, u1, v1, u2, v2);
    w_u32(object + 188, texture);
    return result;
}


uint32 SpawnOpaqueFragments(uint32 count, const uint32 *vertex0, const uint32 *vertex1, const uint32 *vertex2, const uint32 *normal, uint32 red, uint32 green, uint32 blue)
{
    uint32 midpoint[3], sum[3], collision[35], edge0[3], edge1[3], temporary[3], position[3], velocity[3], first[3], second[3], player[3];
    uint32 axis, index, value, floor = 0x7FFFFFFF, factor0, factor1, object, distance, result;
    uint32 shift = 1;
    FUNCTION_MARKER(0x80022454, "SLUS_003.73");
    red &= 255;
    green &= 255;
    blue &= 255;
    AddVectors(sum, vertex1, vertex2);
    ShiftVectorRight(midpoint, sum, &shift);
    for (axis = 0; axis < 3; ++axis)
    {
        memcpy(&value, (const uint8 *)normal + axis * 4, 4);
        collision[axis] = midpoint[axis] + value * 500;
    }
    collision[3] = collision[0];
    collision[4] = collision[1] + 12288000;
    collision[5] = collision[2];
    InitializeCollisionRay((uint8 *)collision);
    QueryCollisionRay((uint8 *)collision, 1);
    if (collision[26])
        floor = collision[28];
    shift = 12;
    SubtractVectors(temporary, vertex2, vertex0);
    ShiftVectorRight(edge0, temporary, &shift);
    SubtractVectors(temporary, vertex1, vertex0);
    ShiftVectorRight(edge1, temporary, &shift);
    for (index = 0; (sint32)index < (sint32)count; ++index)
    {
        factor0 = RandomRange(4096);
        factor1 = RandomRange(4096);
        MultiplyScalarVector(first, &factor0, edge0);
        MultiplyScalarVector(second, &factor1, edge1);
        AddVectors(temporary, first, second);
        ShiftVectorRight(position, temporary, &shift);
        ShiftVectorLeft(first, position, &shift);
        AddVectors(temporary, first, vertex0);
        memcpy(position, temporary, 12);
        SubtractVectors(velocity, position, midpoint);
        NormalizeFixedVector(velocity, velocity);
        factor0 = RandomRange(40);
        MultiplyVector(first, velocity, &factor0);
        factor1 = RandomRange(40);
        MultiplyVector(second, normal, &factor1);
        AddVectors(temporary, first, second);
        memcpy(velocity, temporary, 12);
        object = AllocateEffectStorage(120);
        if (object)
        {
            void *dram_addr = AllocateNativeObjectStorage(object, sizeof(OpaqueFragment));
            new (dram_addr) OpaqueFragment(position, velocity, floor, red, green, blue, 100, 100, 100);
        }
    }
    if (!r_u32(0x800FF214))
    {
        w_u32(0x800FFD48, midpoint[0]);
        w_u32(0x800FFD4C, midpoint[1]);
        w_u32(0x800FFD50, midpoint[2]);
        w_u32(0x800FF214, 1);
        return 1;
    }
    player[0] = r_u32(0x800ED520);
    player[1] = r_u32(0x800ED524);
    player[2] = r_u32(0x800ED528);
    ShiftVectorLeft(player, &shift);
    distance = MeasurePositionDistance(player, midpoint);
    result = MeasurePositionDistance(player, (const uint32 *)psx_addr(0x800FFD48, 12));
    if (distance < result)
    {
        w_u32(0x800FFD48, midpoint[0]);
        w_u32(0x800FFD4C, midpoint[1]);
        w_u32(0x800FFD50, midpoint[2]);
    }
    return result;
}

extern "C" uint32 sub_80022454(uint32 count, uint32 vertex0, uint32 vertex1, uint32 vertex2, uint32 normal, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x80022454, "SLUS_003.73");
    return SpawnOpaqueFragments(count, (const uint32 *)psx_addr(vertex0, 12), (const uint32 *)psx_addr(vertex1, 12), (const uint32 *)psx_addr(vertex2, 12), (const uint32 *)psx_addr(normal, 12), red, green, blue);
}

extern "C" uint32 xport_draft_host_sub_80022454_p2345(uint32 count, const void *vertex0, const void *vertex1, const void *vertex2, const void *normal, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x80022454, "SLUS_003.73");
    return SpawnOpaqueFragments(count, (const uint32 *)vertex0, (const uint32 *)vertex1, (const uint32 *)vertex2, (const uint32 *)normal, red, green, blue);
}

uint32 FragmentLevelPolygon(uint32 object, uint32 polygon, uint32 parameter, uint32 effect, uint32 test, uint32 mark, uint32 render)
{
    uint32 bank, index, table, model, geometry, normals, payload = 0, flags, touched, count, axis, vertex;
    uint32 result = 1, first_word, second_word, texture, color, red, green, blue, u0, v0, u1, v1, u2, v2;
    uint32 matrix[8], vertices[4][3], offset[3];
    FUNCTION_MARKER(0x80021CA8, "SLUS_003.73");
    bank = r_u8(object + 27);
    index = r_u16(object + 22);
    table = r_u32(0x800EAEF8 + bank * 64 + 16);
    model = r_u32(table + index * 4);
    first_word = r_u32(model + 4);
    geometry = model + 32;
    normals = geometry + first_word * 8;
    if (test)
    {
        touched = r_u16(polygon + 18);
        if (!(touched & 8) || (touched & 1))
            return 0;
        if (mark)
        {
            flags = r_u16(polygon);
            w_u16(polygon + 18, touched | 1);
            w_u16(polygon, flags & 0xFE7F);
            if (!render && ((flags & 64) || !(flags & 1)))
                w_u16(polygon, flags & 0xFE3F);
        }
    }
    if (!render)
        return 1;
    flags = r_u16(polygon);
    if (flags & 1)
    {
        payload = (flags & 2) ? polygon + 20 : r_u32(polygon + 20);
        first_word = r_u16(payload + 2);
        second_word = r_u16(payload + 6);
        w_u32(0x800FFB78, first_word);
        w_u32(0x800FFB7C, second_word);
    }
    first_word = r_u16(polygon + 16);
    w_u32(0x800FF210, normals + (first_word & 0xFFF8));
    BuildModelRotationMatrix((const uint32 *)psx_addr(object + 16, 8), (uint16 *)matrix);
    SetRotMatrix((MATRIX *)matrix);
    LoadFixedGteTranslation((const uint32 *)psx_addr(object + 4, 12));
    for (vertex = 0; vertex < 3; ++vertex)
        TransformFragmentVertex(vertices[vertex], geometry, polygon + 4, vertex);
    flags = r_u16(polygon);
    if (flags & 16)
    {
        count = 3;
        bank = r_u8(object + 27);
        color = r_u32(polygon + 12);
        table = r_u32(0x800EAEF8 + bank * 64 + 32);
    }
    else
    {
        count = 4;
        TransformFragmentVertex(vertices[3], geometry, polygon + 4, 3);
        color = r_u32(polygon + 12);
        bank = r_u8(object + 27);
        flags = r_u16(polygon);
        table = r_u32(0x800EAEF8 + bank * 64 + 32);
    }
    SetFragmentColor(flags & 2048, color, count, table);
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 sum = 0;
        sint32 center;
        for (vertex = 0; vertex < count; ++vertex)
            sum += (uint32)((sint32)vertices[vertex][axis] >> 12);
        center = count == 3 ? (sint32)sum / 3 : (sint32)sum >> 2;
        w_u32(0x800FFD58 + axis * 4, (uint32)center << 12);
    }
    flags = r_u16(polygon);
    if (!(flags & 64) && (flags & 1))
    {
        texture = (flags & 32) ? r_u32(payload + 12) : 0;
        if (!r_u32(0x800FF738))
        {
            v0 = r_u8(payload + 1);
            u0 = r_u8(payload);
            u1 = r_u8(payload + 4);
            v1 = r_u8(payload + 5);
            u2 = r_u8(payload + 8);
            v2 = r_u8(payload + 9);
            SpawnTexturedFragments(vertices[0], vertices[1], vertices[2], u0, v0, u1, v1, u2, v2, texture, parameter);
            if (count == 4)
            {
                v0 = r_u8(payload + 5);
                u0 = r_u8(payload + 4);
                u1 = r_u8(payload + 8);
                v1 = r_u8(payload + 9);
                u2 = r_u8(payload + 10);
                v2 = r_u8(payload + 11);
                SpawnTexturedFragments(vertices[1], vertices[2], vertices[3], u0, v0, u1, v1, u2, v2, texture, parameter);
            }
        }
    }
    else
    {
        if (!r_u32(0x800FF738))
        {
            uint32 address = r_u32(0x800FF210);
            if (count == 3)
                blue = r_u8(0x800FFB82);
            offset[0] = (uint32)(sint32)(sint16)r_u16(address);
            offset[1] = (uint32)(sint32)(sint16)r_u16(address + 2);
            offset[2] = (uint32)(sint32)(sint16)r_u16(address + 4);
            red = r_u8(0x800FFB80);
            if (count == 3)
                green = r_u8(0x800FFB81);
            else
            {
                blue = r_u8(0x800FFB82);
                green = blue;
            }
            result = 2;
            SpawnOpaqueFragments(count == 3 ? 15 : 30, vertices[0], vertices[1], vertices[2], offset, red, green, blue);
        }
        if (mark)
        {
            flags = r_u16(polygon);
            w_u16(polygon, flags & 0xFFBF);
        }
    }
    if (effect && !r_u32(0x800FF738))
    {
        red = r_u8(0x800FFB80);
        green = r_u8(0x800FFB81);
        blue = r_u8(0x800FFB82);
        SpawnExplosionRing((const uint32 *)psx_addr(0x800FFD58, 12), 100, red, green, blue, 4, 1, 100);
    }
    return result;
}

uint32 FragmentLevelObject(uint32 object, uint32 test, uint32 mark)
{
    uint32 bank, index, table, model, vertices, polygons, normals, polygon, drawn = 0, any = 0, result, random, stride, flags;
    FUNCTION_MARKER(0x80022318, "SLUS_003.73");
    bank = r_u8(object + 27);
    index = r_u16(object + 22);
    table = r_u32(0x800EAEF8 + bank * 64 + 16);
    model = r_u32(table + index * 4);
    vertices = r_u32(model + 4);
    polygons = r_u32(model + 12);
    normals = r_u32(model + 8);
    polygon = model + 32 + vertices * 8 + normals * 8;
    index = 0;
    while ((sint32)index < (sint32)polygons)
    {
        random = RandomRange(5);
        result = FragmentLevelPolygon(object, polygon, random == 0, 0, test, mark, (sint32)drawn < 6);
        if (result)
        {
            any = 1;
            if (result == 1)
                ++drawn;
        }
        stride = r_u16(polygon + 2);
        ++index;
        polygon += stride & 0xFFFC;
    }
    if (mark && any)
    {
        flags = r_u16(object);
        w_u16(object, flags | 1);
    }
    return any;
}

extern "C" uint32 sub_80021CA8(uint32 object, uint32 polygon, uint32 parameter, uint32 effect, uint32 test, uint32 mark, uint32 render)
{
    FUNCTION_MARKER(0x80021CA8, "SLUS_003.73");
    return FragmentLevelPolygon(object, polygon, parameter, effect, test, mark, render);
}

extern "C" uint32 sub_80022318(uint32 object, uint32 test, uint32 mark)
{
    FUNCTION_MARKER(0x80022318, "SLUS_003.73");
    return FragmentLevelObject(object, test, mark);
}

extern "C" uint32 sub_80021B3C(uint32 indexed, uint32 colors, uint32 count, uint32 palette)
{
    FUNCTION_MARKER(0x80021B3C, "SLUS_003.73");
    return SetFragmentColor(indexed, colors, count, palette);
}

extern "C" uint32 sub_80021ADC(uint32 output, uint32 geometry, uint32 indices, uint32 index)
{
    FUNCTION_MARKER(0x80021ADC, "SLUS_003.73");
    TransformFragmentVertex((uint32 *)psx_addr(output, 12), geometry, indices, index);
    return output;
}

extern "C" uint32 sub_80021358(uint32 vertex0, uint32 vertex1, uint32 vertex2, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2, uint32 texture, uint32 subdivisions)
{
    FUNCTION_MARKER(0x80021358, "SLUS_003.73");
    return SpawnTexturedFragments((const uint32 *)psx_addr(vertex0, 12), (const uint32 *)psx_addr(vertex1, 12), (const uint32 *)psx_addr(vertex2, 12), u0, v0, u1, v1, u2, v2, texture, subdivisions);
}
