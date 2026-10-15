#include "game_sdk_bindings.h"
#include "model_render.h"
#include "model_projection.h"
#include "native_object_storage.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static uint32 ReadOffsetTranslationWord(const sint32 *translation, uint32 offset);

uint32 ProjectOffsetScratchVertices(uint32 vertices, uint32 count, const sint32 position[3])
{
    uint32 offset_x, offset_y, offset_z, offset_xy, first, second, screen, depth, result, output;
    FUNCTION_MARKER(0x800810F0, "SLUS_003.73");
    offset_x = ReadOffsetTranslationWord(position, 0);
    offset_y = ReadOffsetTranslationWord(position, 4);
    offset_xy = (offset_x & 65535) | (offset_y << 16);
    offset_z = ReadOffsetTranslationWord(position, 8);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    first = r_u32(vertices);
    second = r_u32(vertices + 4);
    xport_gte_write_data(0, first + offset_xy);
    xport_gte_write_data(1, second + offset_z);
    result = 0x60000000;
    if (!count)
        return result;
    xport_gte_execute(0x180001);
    vertices += 8;
    first = r_u32(vertices);
    second = r_u32(vertices + 4);
    first += offset_xy;
    second += offset_z;
    result = 0x60006000;
    output = 0x1F800000;
    do
    {
        screen = xport_gte_read_data(14);
        depth = xport_gte_read_data(11);
        xport_gte_write_data(0, first);
        xport_gte_write_data(1, second);
        vertices += 8;
        --count;
        xport_gte_execute(0x180001);
        first = r_u32(vertices);
        screen = (screen & 0xBFFFBFFF) | ~((screen - 0x00F00200) | 0xBFFFBFFF);
        w_u32(output, screen);
        second = r_u32(vertices + 4);
        w_u16(output + 6, depth);
        result &= screen;
        first += offset_xy;
        second += offset_z;
        output += 8;
    } while (count);
    return result;
}

extern "C" uint32 sub_800810F0(uint32 vertices, uint32 count, uint32 position)
{
    FUNCTION_MARKER(0x800810F0, "SLUS_003.73");
    return ProjectOffsetScratchVertices(vertices, count, (const sint32 *)psx_addr(position, 12));
}
extern "C" uint32 apocalypse_project_offset_scratch_native(uint32 vertices, uint32 count, const void *position) { return ProjectOffsetScratchVertices(vertices, count, (const sint32 *)position); }

uint32 ProjectModelScratchVertices(uint32 vertices, uint32 count)
{
    uint32 result = 0x60000000, output = 0x1F800000, saved, first, second, screen, depth, address;
    FUNCTION_MARKER(0x800811BC, "SLUS_003.73");
    if (!count)
        return result;
    result = 0x60006000;
    saved = r_u32(0x800FFB18);
    do
    {
        first = r_u32(vertices);
        second = r_u32(vertices + 4);
        xport_gte_write_data(0, first);
        xport_gte_write_data(1, second);
        xport_gte_execute(0x180001);
        if (second & 0x20000)
        {
            address = 0x1F8003F8 - first;
            screen = r_u32(address);
            depth = (uint32)(sint32)(sint16)r_u16(address + 6);
        }
        else
        {
            screen = xport_gte_read_data(14);
            depth = xport_gte_read_data(11);
            screen = (screen & 0xBFFFBFFF) | ~((screen - 0x00F00200) | 0xBFFFBFFF);
            if (second & 0x10000)
            {
                w_u32(saved, screen);
                w_u16(saved + 6, depth);
                saved -= 8;
            }
        }
        w_u32(output, screen);
        w_u16(output + 6, depth);
        result &= screen;
        vertices += 8;
        --count;
        output += 8;
    } while (count);
    w_u32(0x800FFB18, saved);
    return result;
}

uint32 ProjectModelClippedVertices(uint32 vertices, uint32 count)
{
    uint32 bounds, first_bound, second_bound, near_bound, output, first, second, screen, depth, flags, result;
    sint32 x, y;
    FUNCTION_MARKER(0x8008131C, "SLUS_003.73");
    if (!count)
        return 255;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    bounds = r_u32(0x800FFB08);
    first_bound = r_u32(bounds);
    second_bound = r_u32(bounds + 4);
    near_bound = r_u32(bounds + 8) & 65535;
    result = 65535;
    output = r_u32(0x800FFAC0);
    first = r_u32(vertices);
    second = r_u32(vertices + 4);
    do
    {
        xport_gte_write_data(0, first);
        xport_gte_write_data(1, second);
        vertices += 8;
        --count;
        xport_gte_execute(0x180001);
        first = r_u32(vertices);
        second = r_u32(vertices + 4);
        screen = xport_gte_read_data(14);
        depth = xport_gte_read_data(11);
        w_u32(output + 4, 32767);
        w_u32(output, screen);
        x = (sint16)(screen & 65535);
        y = (sint32)screen >> 16;
        flags = (sint32)(first_bound & 65535) < x;
        flags |= (x < (sint32)(second_bound & 65535)) << 1;
        flags |= ((sint32)(first_bound >> 16) < y) << 2;
        flags |= (y < (sint32)(second_bound >> 16)) << 3;
        flags |= ((sint32)depth < (sint32)near_bound) << 4;
        if ((sint32)depth < 0)
            flags ^= 15;
        flags |= (flags << 8) ^ 0xFF00;
        w_u16(output + 6, flags);
        result &= flags;
        output += 8;
    } while (count);
    return result;
}

extern "C" uint32 sub_800811BC(uint32 vertices, uint32 count)
{
    FUNCTION_MARKER(0x800811BC, "SLUS_003.73");
    return ProjectModelScratchVertices(vertices, count);
}
extern "C" uint32 sub_8008131C(uint32 vertices, uint32 count)
{
    FUNCTION_MARKER(0x8008131C, "SLUS_003.73");
    return ProjectModelClippedVertices(vertices, count);
}

uint32 UpdateProjectedVertexDepthFlags(uint32 count)
{
    uint32 camera, output, input, first, second, value;
    FUNCTION_MARKER(0x8008128C, "SLUS_003.73");
    camera = r_u32(0x800FFB0C);
    output = r_u32(0x800FFAC0);
    input = r_u32(0x800FFAC4);
    first = r_u32(camera + 56);
    second = r_u32(camera + 60);
    value = (uint32)(sint32)(sint16)r_u16(0x800FFB16);
    xport_gte_write_control(1, first);
    xport_gte_write_control(2, second);
    xport_gte_write_control(6, 0 - value);
    do
    {
        first = r_u32(input);
        second = r_u32(output + 4);
        xport_gte_write_data(0, first);
        xport_gte_write_data(1, second);
        xport_gte_execute(0x480012);
        value = xport_gte_read_data(10);
        value = ((value >> 16) ^ 65535) & 0x8080;
        w_u16(output + 6, (second >> 16) ^ value);
        output += 8;
        input += 8;
        --count;
    } while (count);
    return output;
}

static uint32 ReadReflectionWord(const uint32 *input, uint32 index)
{
    uint32 address = GuestMemoryAddress(input), value;
    if (address)
        return r_u32(address + index * 4);
    memcpy(&value, (const uint8 *)input + index * 4, 4);
    return value;
}

uint32 GenerateModelReflectionCoordinates(const uint32 *matrix, const uint32 *position, uint32 output, uint32 model)
{
    uint32 controls[6], rotation[5], index, first, second, third, product;
    uint32 vertices, normals, count, uv_base, translated_xy, translated_z, delta_xy, delta_z;
    uint32 normal_xy, normal_z, reflected_xy, reflected_z, screen, uv, combined_uv;
    FUNCTION_MARKER(0x80081458, "SLUS_003.73");
    for (index = 0; index < 3; ++index)
        controls[index] = xport_draft_gte_control_read(5 + index);
    for (index = 0; index < 3; ++index)
        controls[index + 3] = xport_draft_gte_control_read(24 + index);
    for (index = 0; index < 6; ++index)
        w_u32(0x1F800000 + index * 4, controls[index]);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, 0);
    xport_gte_write_control(24, 0);
    xport_gte_write_control(25, 0);
    xport_gte_write_control(26, 1024);
    for (index = 0; index < 5; ++index)
        controls[index] = xport_draft_gte_control_read(8 + index);
    for (index = 0; index < 5; ++index)
        w_u32(0x1F800018 + index * 4, controls[index]);
    for (index = 0; index < 5; ++index)
        rotation[index] = xport_draft_gte_control_read(index);
    for (index = 0; index < 5; ++index)
        w_u32(0x1F80002C + index * 4, rotation[index]);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, rotation[index]);
    product = (uint32)(sint32)(sint16)rotation[0] * 2560;
    first = ((uint32)((sint32)product >> 12) & 65535) | (rotation[1] & 0xFFFF0000);
    product = (uint32)((sint32)rotation[0] >> 16) * 2560;
    second = (rotation[3] & 65535) | ((uint32)((sint32)product >> 12) << 16);
    third = (rotation[2] & 65535) | (rotation[3] & 0xFFFF0000);
    product = (uint32)(sint32)(sint16)rotation[1] * 2560;
    controls[3] = ((uint32)((sint32)product >> 12) & 65535) | (rotation[2] & 0xFFFF0000);
    xport_gte_write_control(8, first);
    xport_gte_write_control(9, second);
    xport_gte_write_control(10, third);
    xport_gte_write_control(11, controls[3]);
    xport_gte_write_control(12, rotation[4]);
    for (index = 0; index < 5; ++index)
        rotation[index] = ReadReflectionWord(matrix, index);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, rotation[index]);
    first = 0 - ReadReflectionWord(position, 0);
    second = 0 - ReadReflectionWord(position, 1);
    third = 0 - ReadReflectionWord(position, 2);
    xport_gte_write_data(9, first);
    xport_gte_write_data(10, second);
    xport_gte_write_data(11, third);
    uv_base = r_u32(0x800FFB2C);
    xport_gte_execute(0x4BE012);
    count = r_u32(model + 4);
    vertices = model + 32;
    normals = vertices + count * 8;
    translated_xy = xport_gte_read_data(25) & 65535;
    translated_xy |= xport_gte_read_data(26) << 16;
    translated_z = xport_gte_read_data(27);
    xport_gte_write_control(10, 4096);
    xport_gte_write_control(11, 0);
    xport_gte_write_control(12, 4096);
    do
    {
        second = r_u32(vertices + 4);
        first = r_u32(vertices);
        normal_xy = r_u32(normals);
        normal_z = r_u32(normals + 4);
        delta_xy = translated_xy - first;
        delta_z = (translated_z - second) & 65535;
        xport_gte_write_control(8, delta_xy);
        xport_gte_write_control(9, delta_z);
        xport_gte_write_data(0, normal_xy);
        xport_gte_write_data(1, normal_z);
        xport_gte_execute(0x4A6012);
        product = xport_gte_read_data(9);
        xport_gte_write_data(9, normal_xy);
        xport_gte_write_data(8, product << 1);
        xport_draft_gte_execute(0x198003D);
        reflected_xy = xport_gte_read_data(9) & 65535;
        reflected_xy |= xport_gte_read_data(10) << 16;
        reflected_z = xport_gte_read_data(11);
        xport_gte_write_data(0, reflected_xy - delta_xy);
        xport_gte_write_data(1, reflected_z - delta_z);
        xport_gte_execute(0x180001);
        screen = xport_gte_read_data(14);
        uv = (((screen >> 5) + 32) & 63) | (((screen >> 13) + 0x2000) & 0x3F00);
        combined_uv = uv + uv_base;
        if ((uv ^ combined_uv) & (uv_base ^ combined_uv) & 0x80000000)
        {
            // Stop before the store as the original signed ADD traps
            GameDiagnostic("Original ADD overflow at 800816E8: uv=%08X base=%08X\n", uv, uv_base);
            abort();
        }
        w_u32(output + 4, combined_uv);
        vertices += 8;
        normals += 8;
        output += 8;
        --count;
    } while (count);
    for (index = 0; index < 6; ++index)
        controls[index] = r_u32(0x1F800000 + index * 4);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, controls[index]);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(24 + index, controls[index + 3]);
    for (index = 0; index < 5; ++index)
        controls[index] = r_u32(0x1F800018 + index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, controls[index]);
    for (index = 0; index < 5; ++index)
        controls[index] = r_u32(0x1F80002C + index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, controls[index]);
    return 0x1F800000;
}

extern "C" uint32 sub_8008128C(uint32 count)
{
    FUNCTION_MARKER(0x8008128C, "SLUS_003.73");
    return UpdateProjectedVertexDepthFlags(count);
}
extern "C" uint32 sub_80081458(uint32 matrix, uint32 position, uint32 output, uint32 model)
{
    FUNCTION_MARKER(0x80081458, "SLUS_003.73");
    return GenerateModelReflectionCoordinates((const uint32 *)psx_addr(matrix, 20), (const uint32 *)psx_addr(position, 12), output, model);
}
extern "C" uint32 xport_draft_host_sub_80081458_p12(const void *matrix, void *position, uint32 output, uint32 model)
{
    FUNCTION_MARKER(0x80081458, "SLUS_003.73");
    return GenerateModelReflectionCoordinates((const uint32 *)matrix, (const uint32 *)position, output, model);
}


static uint32 ReadOffsetTranslationWord(const sint32 *translation, uint32 offset)
{
    uint32 address = GuestMemoryAddress(translation);
    uint32 value;
    if (address)
        return r_u32(address + offset);
    memcpy(&value, (const uint8 *)translation + offset, 4);
    return value;
}

static uint32 CalculateOffsetVertexClip(sint32 x, sint32 y, sint32 z, const sint32 bounds[6])
{
    uint32 flags = (bounds[0] < x) | ((uint32)(x < bounds[1]) << 1) | ((uint32)(bounds[2] < y) << 2) | ((uint32)(y < bounds[3]) << 3) | ((uint32)(z < bounds[4]) << 4) | ((uint32)(bounds[5] < z) << 5);
    if (z < 0)
        flags ^= 15;
    return flags;
}

static uint32 DivideOffsetProjectionScalar(uint32 numerator, uint32 denominator)
{
    sint32 signed_numerator = (sint32)numerator;
    sint32 signed_denominator = (sint32)denominator;
    if (!denominator)
        return signed_numerator < 0 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)(signed_numerator / signed_denominator);
}

uint32 ProjectOffsetModelVertices(uint32 vertices, uint32 count, const sint32 translation[3])
{
    sint32 bounds[6], depth, x, y;
    uint32 bounds_source, bound_word, aggregate = 0xFFFF;
    uint32 translate_x, translate_y, translate_z, projected, transformed;
    uint32 packed_xy, input_depth, flags, screen, next_xy, next_depth;
    uint32 final_value, references, output, projected_base, pair, interpolation;
    uint32 first, second, first_screen, second_screen, length;
    uint32 delta_x, delta_y, normalized_x, normalized_y, generated_xy;
    sint32 origin_x, origin_y;
    FUNCTION_MARKER(0x800809B0, "SLUS_003.73");
    if (!count)
        return 0xFF;
    bounds_source = r_u32(0x800FFB08);
    bound_word = r_u32(bounds_source);
    bounds[0] = (sint32)(bound_word & 0xFFFF);
    bounds[2] = (sint32)(bound_word >> 16);
    bound_word = r_u32(bounds_source + 4);
    bounds[1] = (sint32)(bound_word & 0xFFFF);
    bounds[3] = (sint32)(bound_word >> 16);
    bound_word = r_u32(bounds_source + 8);
    bounds[4] = (sint32)(bound_word & 0xFFFF);
    bounds[5] = (sint32)(bound_word >> 16);
    translate_x = ReadOffsetTranslationWord(translation, 0);
    translate_y = ReadOffsetTranslationWord(translation, 4);
    translate_z = ReadOffsetTranslationWord(translation, 8);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    projected = r_u32(0x800FFAC0);
    transformed = r_u32(0x800FFAC4);
    packed_xy = r_u32(vertices);
    input_depth = r_u32(vertices + 4) + translate_z;
    packed_xy = ((packed_xy + translate_x) & 0xFFFF) | (((uint32)((sint32)packed_xy >> 16) + translate_y) << 16);
    for (;;)
    {
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, input_depth);
        flags = r_u16(vertices + 6);
        vertices += 8;
        --count;
        xport_gte_execute(0x180001);
        if (flags & 0x10)
            break;
        /* Original loop fetches the following vertex before storing this result */
        next_xy = r_u32(vertices);
        next_depth = r_u32(vertices + 4) + translate_z;
        next_xy = ((next_xy + translate_x) & 0xFFFF) | (((uint32)((sint32)next_xy >> 16) + translate_y) << 16);
        x = (sint32)xport_gte_read_data(9);
        y = (sint32)xport_gte_read_data(10);
        depth = (sint32)xport_gte_read_data(11);
        w_u32(transformed, ((uint32)x & 0xFFFF) | ((uint32)y << 16));
        screen = xport_gte_read_data(14);
        w_u16(projected + 4, (uint16)depth);
        w_u32(projected, screen);
        flags = CalculateOffsetVertexClip((sint16)screen, (sint32)screen >> 16, depth, bounds);
        if ((sint32)xport_gte_read_flag() < 0)
            flags |= 0x40;
        flags |= (flags << 8) ^ 0xFF00;
        w_u16(projected + 6, (uint16)flags);
        aggregate &= flags;
        projected += 8;
        transformed += 8;
        packed_xy = next_xy;
        input_depth = next_depth;
        if (!count)
        {
            w_u32(0x1F8001D4, projected);
            return aggregate;
        }
    }
    vertices -= 8;
    ++count;
    xport_gte_write_control(0, 0x28F5);
    xport_gte_write_control(1, 0);
    xport_gte_write_control(2, 4096);
    xport_gte_write_control(3, 0);
    xport_gte_write_control(4, 4096);
    xport_gte_write_control(24, 0);
    xport_gte_write_control(25, 0);
    references = vertices;
    output = projected;
    projected_base = r_u32(0x800FFAC0);
    do
    {
        pair = r_u32(references);
        interpolation = r_u32(references + 4);
        first = projected_base + (pair & 0xFFFF);
        first_screen = r_u32(first);
        depth = (sint16)r_u32(first + 4);
        second = projected_base + (pair >> 16);
        second_screen = r_u32(second);
        origin_y = (sint32)first_screen >> 16;
        origin_x = (sint16)first_screen;
        delta_x = (uint32)(sint32)(sint16)second_screen - (uint32)origin_x;
        delta_y = (uint32)origin_y - (uint32)((sint32)second_screen >> 16);
        xport_gte_write_data(9, delta_x);
        xport_gte_write_data(10, delta_y);
        xport_gte_execute(0xA00428);
        first = xport_gte_read_data(25);
        second = xport_gte_read_data(26);
        length = sub_80085B54(first + second);
        normalized_y = DivideOffsetProjectionScalar(delta_x << 12, length);
        normalized_x = DivideOffsetProjectionScalar(delta_y << 12, length);
        xport_gte_write_data(9, normalized_x);
        xport_gte_write_data(10, normalized_y);
        xport_gte_write_data(8, interpolation);
        xport_draft_gte_execute(0x198003D);
        first = xport_gte_read_data(25);
        second = xport_gte_read_data(26);
        generated_xy = (first & 0xFFFF) | (second << 16);
        xport_gte_write_data(0, generated_xy);
        xport_gte_write_data(1, (uint32)depth);
        xport_gte_execute(0x180001);
        screen = xport_gte_read_data(14);
        x = (sint32)((uint32)(sint32)(sint16)screen + (uint32)origin_x);
        y = (sint32)((uint32)((sint32)screen >> 16) + (uint32)origin_y);
        flags = CalculateOffsetVertexClip(x, y, depth, bounds);
        flags |= (flags << 8) ^ 0xFF00;
        aggregate &= flags;
        w_u32(output, ((uint32)x & 0xFFFF) | ((uint32)y << 16));
        w_u32(output + 4, (flags << 16) | ((uint32)depth & 0xFFFF));
        references += 8;
        output += 8;
        --count;
        final_value = length;
    } while (count);
    bounds_source = r_u32(0x800FFB08);
    bound_word = r_u32(bounds_source + 12);
    xport_gte_write_control(24, bound_word << 16);
    xport_gte_write_control(25, (bound_word >> 16) << 16);
    w_u32(0x1F8001D4, final_value);
    return aggregate;
}

extern "C" uint32 sub_800809B0(uint32 vertices, uint32 count, uint32 translation)
{
    FUNCTION_MARKER(0x800809B0, "SLUS_003.73");
    return ProjectOffsetModelVertices(vertices, count, count ? (const sint32 *)psx_addr(translation, 12) : 0);
}
extern "C" uint32 apocalypse_transform_geometry_vertices(uint32 vertices, uint32 count, const sint32 translation[3]) { return ProjectOffsetModelVertices(vertices, count, translation); }


uint32 ProjectWorldPoint(const uint32 *position, uint32 *screen)
{
    uint32 raw, offset, x, y, z, output;
    FUNCTION_MARKER(0x80025248, "SLUS_003.73");
    raw = ReadOffsetTranslationWord((const sint32 *)position, 0);
    offset = r_u32(0x800ED520);
    x = (uint32)((sint32)raw >> 12) - offset;
    raw = ReadOffsetTranslationWord((const sint32 *)position, 4);
    offset = r_u32(0x800ED524);
    y = (uint32)((sint32)raw >> 12) - offset;
    raw = ReadOffsetTranslationWord((const sint32 *)position, 8);
    offset = r_u32(0x800ED528);
    z = (uint32)((sint32)raw >> 12) - offset;
    xport_gte_write_data(0, (x & 0xFFFF) | (y << 16));
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    raw = xport_gte_read_data(14);
    output = GuestMemoryAddress(screen);
    if (output)
        w_u32(output, raw);
    else
        memcpy(screen, &raw, 4);
    return xport_gte_read_data(27);
}

extern "C" uint32 sub_80025248(uint32 position, uint32 screen_output)
{
    FUNCTION_MARKER(0x80025248, "SLUS_003.73");
    return ProjectWorldPoint((const uint32 *)psx_addr(position, 12), (uint32 *)psx_addr(screen_output, 4));
}

void GenerateModelNormalColors(uint32 vertices, uint32 count)
{
    FUNCTION_MARKER(0x80081794, "SLUS_003.73");
    uint32 output = 0x800F3E70, first, second, x, y, z;
    xport_gte_write_data(6, 0xFFFFFF);
    first = r_u32(vertices);
    second = r_u32(vertices + 4);
    do
    {
        xport_gte_write_data(0, first);
        xport_gte_write_data(1, second);
        vertices += 8;
        --count;
        xport_gte_execute(0x108041B);
        first = r_u32(vertices);
        second = r_u32(vertices + 4);
        x = xport_gte_read_data(9);
        y = xport_gte_read_data(10);
        z = xport_gte_read_data(11);
        w_u32(output, (x & 0xFFFF) | (y << 16));
        w_u32(output + 4, z);
        output += 8;
    } while ((sint32)count > 0);
}

extern "C" void sub_80081794(uint32 vertices, uint32 count)
{
    FUNCTION_MARKER(0x80081794, "SLUS_003.73");
    GenerateModelNormalColors(vertices, count);
}

static void LoadProjectionEffectIr(const sint16 v[3])
{
    uint32 i;
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, (uint16)v[i]);
}

static void StoreProjectionEffectIr(sint16 v[3])
{
    uint32 i;
    for (i = 0; i < 3; ++i)
        v[i] = (sint16)xport_gte_read_data(9 + i);
}

static void TransformProjectionEffectMatrix(const MATRIX *m, uint32 opcode)
{
    uint32 i;
    const sint16 *v = &m->m[0][0];
    (void)xport_gte_read_data(9);
    for (i = 0; i < 4; ++i)
        xport_gte_write_control(i, (uint16)v[2 * i] | ((uint32)(uint16)v[2 * i + 1] << 16));
    xport_gte_write_control(4, (uint16)v[8]);
    xport_gte_execute(opcode);
}

static void CrossProjectionEffectVector(const sint16 v[3])
{
    (void)xport_gte_read_data(9);
    xport_gte_write_control(0, (uint16)v[0]);
    xport_gte_write_control(2, (uint16)v[1]);
    xport_gte_write_control(4, (uint16)v[2]);
    xport_gte_execute(0x178000C);
}

static sint32 DivideProjectionEffectScalar(sint32 a, sint32 b)
{
    /* Raw MIPS DIV has defined zero and overflow results */
    if (!b)
        return a < 0 ? 1 : -1;
    if ((uint32)a == 0x80000000 && b == -1)
        return a;
    return a / b;
}

static void ScaleProjectionEffectVector(const sint16 v[3], uint32 factor)
{
    uint32 i;
    (void)xport_gte_read_data(9);
    xport_gte_write_data(8, factor);
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(25 + i, (uint32)(sint32)v[i]);
    xport_draft_gte_execute(0x1A8003E);
}

static void LoadProjectionEffectVertex(uint32 slot, const sint16 c[3], const sint16 a[3], const sint16 b[3], sint32 sa, sint32 sb)
{
    sint16 x = (sint16)((uint32)(sint32)c[0] + (uint32)(sa * a[0]) + (uint32)(sb * b[0]));
    sint16 y = (sint16)((uint32)(sint32)c[1] + (uint32)(sa * a[1]) + (uint32)(sb * b[1]));
    sint16 z = (sint16)((uint32)(sint32)c[2] + (uint32)(sa * a[2]) + (uint32)(sb * b[2]));
    xport_gte_write_data(slot, (uint16)x | ((uint32)(uint16)y << 16));
    xport_gte_write_data(slot + 1, (uint16)z);
}

void RenderModelProjectionEffect(uint32 model, const uint32 *matrix32, uint32 vector, uint32 position, uint32 direction, uint32 color)
{
    FUNCTION_MARKER(0x8007A8F8, "SLUS_003.73");
    MATRIX forward, inverse, diagonal, projection;
    sint16 center[3], extent[3], normal[3], cross[3], axis[3] = {0, 0, 0};
    sint16 basis1[3], basis2[3], coefficients[3], projected[3], edge1[3], edge2[3], dir[3];
    uint32 i, packed, squares[3], scale, packet, depth, screens, texture, bucket, header;
    sint32 average, numerator, distance;
    sint16 length;
    uint32 bounds[3];
    bounds[0] = r_u32(model + 20);
    bounds[1] = r_u32(model + 24);
    bounds[2] = r_u32(model + 28);
    for (i = 0; i < 3; ++i)
    {
        sint32 low = (sint32)(bounds[i] & 65535);
        sint32 high = (sint16)(bounds[i] >> 16);
        sint32 half = (low - high) >> 1;
        extent[i] = (sint16)((half * (i == 1 ? 11 : 5)) >> (i == 1 ? 3 : 2));
    }
    center[0] = (sint16)(((sint32)(bounds[0] & 65535) + (sint16)(bounds[0] >> 16)) >> 1);
    packed = r_u32(model + 24);
    center[1] = (sint16)(((sint32)(packed & 65535) + (sint16)(packed >> 16)) >> 1);
    packed = r_u32(model + 28);
    center[2] = (sint16)(((sint32)(packed & 65535) + (sint16)(packed >> 16)) >> 1);
    LoadProjectionEffectIr(center);
    {
        uint32 words[5];
        for (i = 0; i < 5; ++i)
            words[i] = ReadReflectionWord(matrix32, i);
        (void)xport_gte_read_data(9);
        for (i = 0; i < 5; ++i)
            xport_gte_write_control(i, words[i]);
        xport_gte_execute(0x49E012);
    }
    StoreProjectionEffectIr(center);
    for (i = 0; i < 3; ++i)
    {
        uint16 offset;
        uint32 matrix_address = GuestMemoryAddress(matrix32);
        offset = matrix_address ? r_u16(matrix_address + 20 + i * 4) : xport_load_le16((const uint8 *)matrix32 + 20 + i * 4);
        center[i] = (sint16)((uint32)(sint32)center[i] + offset);
    }
    average = ((sint32)extent[0] + extent[1] + extent[2]) / 3;
    numerator = (sint32)((uint32)average << 16) >> 4;
    /* Original 85C04 copies only the nine rotation halfwords */
    CopyModelRotationMatrix(matrix32, (uint32 *)&forward);
    TransposeModelMatrix((const uint32 *)&forward, (uint32 *)&inverse);
    ClearModelRotationMatrix((uint32 *)&diagonal);
    sint16 diagonal_x = (sint16)DivideProjectionEffectScalar(numerator, extent[0]);
    sint16 diagonal_y = (sint16)DivideProjectionEffectScalar(numerator, extent[1]);
    sint16 diagonal_z = (sint16)DivideProjectionEffectScalar(numerator, extent[2]);
    diagonal.m[0][0] = diagonal_x;
    diagonal.m[1][1] = diagonal_y;
    diagonal.m[2][2] = diagonal_z;
    for (i = 0; i < 3; ++i)
        dir[i] = (sint16)r_u16(direction + 2 * i);
    LoadProjectionEffectIr(dir);
    TransformProjectionEffectMatrix(&inverse, 0x49E012);
    (void)NormalizeProjectionIr();
    TransformProjectionEffectMatrix(&diagonal, 0x49E012);
    StoreProjectionEffectIr(normal);
    length = (sint16)MeasureProjectionIrLength();
    for (i = 0; i < 3; ++i)
        squares[i] = xport_gte_read_data(25 + i);
    if ((sint32)squares[1] >= (sint32)squares[0] && (sint32)squares[2] >= (sint32)squares[0])
        axis[0] = 4096;
    else if ((sint32)squares[0] >= (sint32)squares[1] && (sint32)squares[2] >= (sint32)squares[1])
        axis[1] = 4096;
    else
        axis[2] = 4096;
    LoadProjectionEffectIr(normal);
    CrossProjectionEffectVector(axis);
    StoreProjectionEffectIr(cross);
    (void)NormalizeProjectionIr();
    scale = (uint32)((sint32)extent[0] * extent[1]);
    scale = (uint32)DivideProjectionEffectScalar((sint32)scale, (sint16)average);
    scale *= (uint32)(sint32)extent[2];
    scale *= (uint32)(sint32)length;
    scale = (uint32)(DivideProjectionEffectScalar((sint32)scale, (sint16)average) >> 12);
    TransformProjectionEffectMatrix(&diagonal, 0x49E012);
    TransformProjectionEffectMatrix(&forward, 0x49E012);
    sub_80085D30(direction);
    ScaleProjectionIr(scale);
    StoreProjectionEffectIr(basis1);
    LoadProjectionEffectIr(normal);
    CrossProjectionEffectVector(cross);
    (void)NormalizeProjectionIr();
    TransformProjectionEffectMatrix(&diagonal, 0x49E012);
    TransformProjectionEffectMatrix(&forward, 0x49E012);
    sub_80085D30(direction);
    ScaleProjectionIr(scale);
    StoreProjectionEffectIr(basis2);
    xport_gte_write_data(0, r_u32(vector));
    xport_gte_write_data(1, r_u32(vector + 4));
    TransformProjectionIr((const uint32 *)psx_addr(direction, 20));
    distance = (sint32)xport_gte_read_data(25);
    if (distance < (sint32)r_u32(0x800FF964))
        return;
    for (i = 0; i < 3; ++i)
    {
        projection.m[0][i] = (sint16)((uint32)((sint32)r_u32(position + 4 * i) >> 12) - (uint32)(sint32)center[i]);
        projection.m[1][i] = basis1[i];
        projection.m[2][i] = basis2[i];
    }
    TransformProjectionEffectMatrix(&projection, 0x486012);
    StoreProjectionEffectIr(coefficients);
    if (coefficients[0] > 0)
        return;
    uint32 camera = r_u32(0x800FFB0C);
    for (i = 0; i < 3; ++i)
        center[i] = (sint16)((uint32)(sint32)center[i] - r_u16(camera + 4 + 4 * i));
    LoadProjectionEffectIr(dir);
    ScaleProjectionEffectVector(center, (uint32)DivideProjectionEffectScalar((sint32)((uint32)(sint32)coefficients[0] << 12), distance));
    StoreProjectionEffectIr(projected);
    LoadProjectionEffectIr(dir);
    ScaleProjectionEffectVector(basis1, (uint32)DivideProjectionEffectScalar((sint32)(0 - ((uint32)(sint32)coefficients[1] << 12)), distance));
    StoreProjectionEffectIr(edge1);
    LoadProjectionEffectIr(dir);
    ScaleProjectionEffectVector(basis2, (uint32)DivideProjectionEffectScalar((sint32)(0 - ((uint32)(sint32)coefficients[2] << 12)), distance));
    StoreProjectionEffectIr(edge2);
    SetRotMatrix((MATRIX *)psx_addr(r_u32(0x800FFB0C) + 116, 20));
    for (i = 0; i < 3; ++i)
        xport_gte_write_control(5 + i, 0);
    LoadProjectionEffectVertex(0, projected, edge1, edge2, -1, -1);
    LoadProjectionEffectVertex(2, projected, edge1, edge2, 1, -1);
    LoadProjectionEffectVertex(4, projected, edge1, edge2, -1, 1);
    xport_gte_execute(0x280030);
    for (i = 0; i < 3; ++i)
    {
        screens = r_u32(0x800FFAC0);
        w_u32(screens + 8 * i, xport_gte_read_data(12 + i));
    }
    LoadProjectionEffectVertex(0, projected, edge1, edge2, 1, 1);
    xport_gte_execute(0x180001);
    screens = r_u32(0x800FFAC0);
    w_u32(screens + 24, xport_gte_read_data(14));
    xport_gte_execute(0x168002E);
    depth = xport_gte_read_data(7);
    if (depth >= 4096)
        depth = 4095;
    packet = r_u32(0x800FF668);
    uint32 limit = r_u32(0x800FF374);
    uint32 next_packet = packet + 40;
    w_u32(0x800FF668, next_packet);
    if (limit < next_packet)
    {
        w_u32(0x800FF668, packet);
        return;
    }
    (void)sub_8008AFFC(packet);
    w_u8(packet + 7, r_u8(packet + 7) | 2);
    screens = r_u32(0x800FFAC0);
    for (i = 0; i < 4; ++i)
    {
        w_u16(packet + 8 + i * 8, r_u16(screens + i * 8));
        w_u16(packet + 10 + i * 8, r_u16(screens + i * 8 + 2));
    }
    w_u8(packet + 4, color);
    w_u8(packet + 5, color >> 8);
    w_u8(packet + 6, color >> 16);
    texture = r_u32(0x800FF408);
    w_u8(packet + 12, r_u8(texture));
    texture = r_u32(0x800FF408);
    w_u8(packet + 13, r_u8(texture + 1));
    texture = r_u32(0x800FF408);
    w_u8(packet + 20, r_u8(texture + 4) - 1);
    texture = r_u32(0x800FF408);
    w_u8(packet + 21, r_u8(texture + 5));
    texture = r_u32(0x800FF408);
    w_u8(packet + 28, r_u8(texture + 8));
    texture = r_u32(0x800FF408);
    w_u8(packet + 29, r_u8(texture + 9) - 1);
    texture = r_u32(0x800FF408);
    w_u8(packet + 36, r_u8(texture + 10) - 1);
    texture = r_u32(0x800FF408);
    w_u8(packet + 37, r_u8(texture + 11) - 1);
    texture = r_u32(0x800FF408);
    w_u16(packet + 14, r_u16(texture + 2));
    uint32 page = r_u16(texture + 6);
    uint32 packet_header = r_u32(packet);
    w_u16(packet + 22, (page & 0xFF9F) | 0x40);
    bucket = r_u32(0x800FF660) + 4 * depth + 112;
    header = r_u32(bucket);
    w_u32(packet, (packet_header & 0xFF000000) | (header & 0xFFFFFF));
    w_u32(bucket, (r_u32(bucket) & 0xFF000000) | (packet & 0xFFFFFF));
}

extern "C" void sub_8007A8F8(uint32 model, uint32 matrix, uint32 vector, uint32 position, uint32 direction, uint32 color)
{
    FUNCTION_MARKER(0x8007A8F8, "SLUS_003.73");
    RenderModelProjectionEffect(model, (const uint32 *)psx_addr(matrix, 32), vector, position, direction, color);
}

extern "C" void apocalypse_model_effect_native(uint32 model, const void *matrix, uint32 vector, uint32 position, uint32 direction, uint32 color)
{
    RenderModelProjectionEffect(model, (const uint32 *)matrix, vector, position, direction, color);
}

uint32 MeasureProjectionIrLength()
{
    FUNCTION_MARKER(0x80085D14, "SLUS_003.73");
    xport_gte_execute(0xA00428);
    uint32 x = xport_gte_read_data(25);
    uint32 y = xport_gte_read_data(26);
    uint32 z = xport_gte_read_data(27);
    return sub_80085B54(x + y + z);
}

uint32 NormalizeProjectionIr()
{
    FUNCTION_MARKER(0x80085D64, "SLUS_003.73");
    uint32 saved[3], normalized[3];
    for (uint32 axis = 0; axis < 3; ++axis)
        saved[axis] = xport_gte_read_data(9 + axis);
    uint32 length = MeasureProjectionIrLength();
    for (uint32 axis = 0; axis < 3; ++axis)
        normalized[axis] = (uint32)DivideProjectionEffectScalar((sint32)(saved[axis] << 12), (sint32)length);
    for (uint32 axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, normalized[axis]);
    return length;
}

void CrossProjectionIr(const uint32 *vector)
{
    FUNCTION_MARKER(0x80085D30, "SLUS_003.73");
    uint32 packed = ReadReflectionWord(vector, 0);
    uint32 last = ReadReflectionWord(vector, 1);
    (void)xport_gte_read_data(9);
    xport_gte_write_control(0, packed & 65535);
    xport_gte_write_control(2, packed >> 16);
    xport_gte_write_control(4, last);
    xport_gte_execute(0x178000C);
}

void TransformProjectionIr(const uint32 *matrix)
{
    FUNCTION_MARKER(0x80085CD4, "SLUS_003.73");
    uint32 words[5];
    for (uint32 index = 0; index < 5; ++index)
        words[index] = ReadReflectionWord(matrix, index);
    (void)xport_gte_read_data(9);
    for (uint32 index = 0; index < 5; ++index)
        xport_gte_write_control(index, words[index]);
    xport_gte_execute(0x486012);
}

extern "C" uint32 sub_80085D14(void)
{
    FUNCTION_MARKER(0x80085D14, "SLUS_003.73");
    return MeasureProjectionIrLength();
}
extern "C" uint32 sub_80085D64(void)
{
    FUNCTION_MARKER(0x80085D64, "SLUS_003.73");
    return NormalizeProjectionIr();
}
extern "C" void sub_80085D30(uint32 input)
{
    FUNCTION_MARKER(0x80085D30, "SLUS_003.73");
    CrossProjectionIr((const uint32 *)psx_addr(input, 8));
}
extern "C" void sub_80085CD4(uint32 input)
{
    FUNCTION_MARKER(0x80085CD4, "SLUS_003.73");
    TransformProjectionIr((const uint32 *)psx_addr(input, 20));
}

uint32 ProjectModelReferenceVertices(uint32 input, uint32 count)
{
    FUNCTION_MARKER(0x80080EF4, "SLUS_003.73");
    uint32 bounds, bx, by, bz, output, vectors, reference, cache, packed, tagged;
    uint32 common = 0xFFFF;
    if (!count)
        return 255;
    bounds = r_u32(0x800FFB08);
    bx = r_u32(bounds);
    by = r_u32(bounds + 4);
    bz = r_u32(bounds + 8);
    output = r_u32(0x800FFAC0);
    vectors = r_u32(0x800FFAC4);
    reference = output + 7992;
    cache = r_u32(0x800FFB18);
    packed = r_u32(input);
    tagged = r_u32(input + 4);
    do
    {
        uint32 tag = tagged >> 16;
        xport_gte_write_data(0, packed);
        xport_gte_write_data(1, tagged);
        input += 8;
        --count;
        xport_gte_execute(0x180001);
        if (tag & 2)
        {
            uint32 previous = reference - packed;
            uint32 xy = r_u32(previous), depth_flags = r_u32(previous + 4);
            uint32 irxy = r_u32(previous + vectors - output);
            w_u32(output, xy);
            w_u32(output + 4, depth_flags);
            w_u32(vectors, irxy);
            common &= depth_flags >> 16;
            packed = r_u32(input);
            tagged = r_u32(input + 4);
        }
        else
        {
            uint32 irxy, xy, flags, depth_flags;
            sint32 x, y, depth;
            packed = r_u32(input);
            tagged = r_u32(input + 4);
            uint32 ir_x = xport_gte_read_data(9);
            uint32 ir_y = xport_gte_read_data(10);
            depth = (sint32)xport_gte_read_data(11);
            irxy = (ir_x & 65535) | (ir_y << 16);
            xy = xport_gte_read_data(14);
            x = (sint16)xy;
            y = (sint32)xy >> 16;
            flags = ((sint32)(bx & 0xFFFF) < x) | ((uint32)(x < (sint32)(by & 0xFFFF)) << 1) | ((uint32)((sint32)(bx >> 16) < y) << 2) | ((uint32)(y < (sint32)(by >> 16)) << 3) | ((uint32)(depth < (sint32)(bz & 0xFFFF)) << 4) | ((uint32)((sint32)(bz >> 16) < depth) << 5);
            if (depth < 0)
                flags ^= 15;
            flags |= (flags << 8) ^ 0xFF00;
            common &= flags;
            depth_flags = (flags << 16) | ((uint32)depth & 0xFFFF);
            w_u32(output, xy);
            w_u32(output + 4, depth_flags);
            w_u32(vectors, irxy);
            if (tag & 1)
            {
                w_u32(cache, xy);
                w_u32(cache + 4, depth_flags);
                w_u32(cache + vectors - output, irxy);
                cache -= 8;
            }
        }
        output += 8;
        vectors += 8;
    } while (count);
    w_u32(0x800FFB18, cache);
    w_u32(0x1F8001D4, output);
    return common;
}

void LoadPackedProjectionIr(uint32 geometry)
{
    FUNCTION_MARKER(0x800821E0, "SLUS_003.73");
    uint32 packed = r_u32(geometry);
    uint32 depth = r_u32(geometry + 4);
    xport_gte_write_data(9, packed);
    xport_gte_write_data(10, (uint32)((sint32)packed >> 16));
    xport_gte_write_data(11, depth);
}

extern "C" uint32 sub_80080EF4(uint32 input, uint32 count)
{
    FUNCTION_MARKER(0x80080EF4, "SLUS_003.73");
    return ProjectModelReferenceVertices(input, count);
}
extern "C" void sub_800821E0(uint32 geometry)
{
    FUNCTION_MARKER(0x800821E0, "SLUS_003.73");
    LoadPackedProjectionIr(geometry);
}

uint32 ProjectAndClipWorldLine(uint32 line)
{
    uint32 camera = r_u32(0x800FFB0C), camera_position[3], first[3], second[3];
    uint32 first_xy, second_xy, first_flags, second_flags, packed, z, carrier, result, axis;
    uint32 planes[6], rotation[5], low = 0, high = 4096, mask, cursor;
    sint32 dx, dy;
    FUNCTION_MARKER(0x800850A4, "SLUS_003.73");
    camera_position[0] = r_u32(camera + 4);
    camera_position[1] = r_u32(camera + 8);
    camera_position[2] = r_u32(camera + 12);
    packed = ((uint32)((sint32)r_u32(line) >> 12) - camera_position[0]) & 0xFFFF;
    packed |= ((uint32)((sint32)r_u32(line + 4) >> 12) - camera_position[1]) << 16;
    z = (uint32)((sint32)r_u32(line + 8) >> 12) - camera_position[2];
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    packed = ((uint32)((sint32)r_u32(line + 12) >> 12) - camera_position[0]) & 0xFFFF;
    packed |= ((uint32)((sint32)r_u32(line + 16) >> 12) - camera_position[1]) << 16;
    z = (uint32)((sint32)r_u32(line + 20) >> 12) - camera_position[2];
    first_flags = xport_draft_gte_control_read(31);
    first_xy = xport_gte_read_data(14);
    for (axis = 0; axis < 3; ++axis)
        first[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    w_u32(line + 24, first_xy);
    w_u32(line + 40, first[2]);
    w_u32(line + 32, 0);
    w_u32(line + 36, 4096);
    second_flags = xport_draft_gte_control_read(31);
    second_xy = xport_gte_read_data(14);
    for (axis = 0; axis < 3; ++axis)
        second[axis] = xport_gte_read_data(25 + axis);
    carrier = first_flags | second_flags;
    if ((sint32)carrier >= 0)
    {
        dx = (sint16)(second_xy - first_xy);
        carrier = (uint32)dx;
        if (dx < 1024)
        {
            // The lower X bound branch also updates the clipping carrier
            carrier = (uint32)((sint32)first_xy >> 16);
            if (dx >= -1023)
            {
                dy = ((sint32)second_xy >> 16) - (sint32)carrier;
                carrier = (uint32)dy;
                if (dy < 512 && dy >= -511)
                {
                    w_u32(line + 28, second_xy);
                    w_u32(line + 44, second[2]);
                    result = (sint32)first[2] < (sint32)second[2] ? first[2] >> 2 : second[2] >> 2;
                    return (sint32)(result - 4096) < 0 ? result : 4095;
                }
            }
        }
    }
    if (((carrier >> 22) & 7) || (sint32)(first[2] & second[2]) < 0)
        return 0xFFFFFFFF;
    xport_gte_write_data(0, (first[0] & 0xFFFF) | (first[1] << 16));
    xport_gte_write_data(1, first[2]);
    xport_gte_write_data(2, (second[0] & 0xFFFF) | (second[1] << 16));
    xport_gte_write_data(3, second[2]);
    for (axis = 0; axis < 5; ++axis)
        planes[axis] = r_u32(0x800F25D0 + axis * 4);
    planes[5] = r_u16(0x800F25E4);
    xport_gte_write_control(8, planes[0]);
    xport_gte_write_control(9, (planes[2] << 16) | (planes[1] & 0xFFFF));
    xport_gte_write_control(10, (planes[2] >> 16) | (planes[3] << 16));
    xport_gte_write_control(11, planes[4]);
    xport_gte_write_control(12, planes[5]);
    xport_gte_write_control(13, (uint32)((sint32)planes[1] >> 16));
    xport_gte_write_control(14, (uint32)((sint32)planes[3] >> 16));
    xport_gte_write_control(15, 0);
    planes[0] = r_u32(0x800F25E8);
    planes[1] = r_u16(0x800F25EC);
    planes[2] = r_u32(0x800F2600);
    planes[3] = r_u16(0x800F2604);
    planes[4] = r_u32(0x800F2608);
    planes[5] = r_u16(0x800F260C);
    xport_gte_write_control(16, planes[0]);
    xport_gte_write_control(17, (planes[2] << 16) | planes[1]);
    xport_gte_write_control(18, (planes[2] >> 16) | (planes[3] << 16));
    xport_gte_write_control(19, planes[4]);
    xport_gte_write_control(20, planes[5]);
    xport_gte_execute(0x4A2412);
    first_flags = (xport_draft_gte_control_read(31) >> 19) & 0x38;
    for (axis = 0; axis < 3; ++axis)
        w_u32(line + 48 + axis * 4, xport_gte_read_data(25 + axis));
    xport_gte_execute(0x4C6412);
    first_flags |= (xport_draft_gte_control_read(31) >> 22) & 7;
    for (axis = 0; axis < 3; ++axis)
        w_u32(line + 60 + axis * 4, xport_gte_read_data(25 + axis));
    xport_gte_execute(0x4AA412);
    second_flags = (xport_draft_gte_control_read(31) >> 19) & 0x38;
    for (axis = 0; axis < 3; ++axis)
        w_u32(line + 72 + axis * 4, xport_gte_read_data(25 + axis));
    xport_gte_execute(0x4CE412);
    second_flags |= (xport_draft_gte_control_read(31) >> 22) & 7;
    for (axis = 0; axis < 3; ++axis)
        w_u32(line + 84 + axis * 4, xport_gte_read_data(25 + axis));
    if ((first_flags & second_flags) || ((first_flags | second_flags) & 0x20))
        return 0xFFFFFFFF;
    cursor = line;
    for (mask = 32; mask; mask >>= 1, cursor += 4)
    {
        if ((first_flags ^ second_flags) & mask)
        {
            uint32 distance = r_u32(cursor + 48), other = r_u32(cursor + 72);
            uint32 parameter = DivideOffsetProjectionScalar(distance << 12, distance - other);
            if (first_flags & mask)
            {
                if ((sint32)(parameter - low) > 0)
                    low = parameter;
            }
            else if ((sint32)(parameter - high) < 0)
                high = parameter;
        }
    }
    if ((sint32)(low - high) >= 0)
        return 0xFFFFFFFF;
    w_u32(line + 32, low);
    w_u32(line + 36, high);
    for (axis = 0; axis < 5; ++axis)
        rotation[axis] = xport_draft_gte_control_read(axis);
    packed = (xport_gte_read_data(0) << 3) & 0xFFF8FFF8;
    first[2] = xport_gte_read_data(1) << 3;
    z = (xport_gte_read_data(2) << 3) & 0xFFF8FFF8;
    second[2] = xport_gte_read_data(3) << 3;
    xport_gte_write_control(0, packed);
    xport_gte_write_control(2, z);
    xport_gte_write_control(1, packed ^ ((packed ^ z) & 0xFFFF));
    xport_gte_write_control(3, first[2]);
    xport_gte_write_control(4, second[2]);
    xport_gte_write_data(0, 4096 - low);
    xport_gte_write_data(1, low);
    xport_gte_execute(0x180001);
    first_xy = xport_gte_read_data(14);
    first[2] = xport_gte_read_data(11);
    xport_gte_write_data(0, 4096 - high);
    xport_gte_write_data(1, high);
    first[2] >>= 3;
    xport_gte_execute(0x180001);
    w_u32(line + 24, first_xy);
    w_u32(line + 40, first[2]);
    second_xy = xport_gte_read_data(14);
    second[2] = xport_gte_read_data(11);
    w_u32(line + 28, second_xy);
    second[2] >>= 3;
    w_u32(line + 44, second[2]);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, rotation[axis]);
    result = (sint32)first[2] < (sint32)second[2] ? first[2] >> 2 : second[2] >> 2;
    return (sint32)(result - 4096) < 0 ? result : 4095;
}

extern "C" uint32 sub_800850A4(uint32 line)
{
    FUNCTION_MARKER(0x800850A4, "SLUS_003.73");
    return ProjectAndClipWorldLine(line);
}

void TransformPackedMatrixForProjection(uint32 input, uint32 output)
{
    uint32 first = r_u32(input), second = r_u32(input + 4);
    uint32 fourth = r_u32(input + 12), third, fifth, values[3][3], axis;
    FUNCTION_MARKER(0x800855B4, "SLUS_003.73");
    xport_gte_write_data(9, first);
    xport_gte_write_data(10, second >> 16);
    xport_gte_write_data(11, fourth);
    xport_gte_execute(0x4DE012);
    third = r_u32(input + 8);
    for (axis = 0; axis < 3; ++axis)
        values[0][axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(9, first >> 16);
    xport_gte_write_data(10, third);
    xport_gte_write_data(11, fourth >> 16);
    xport_gte_execute(0x4DE012);
    fifth = r_u32(input + 16);
    for (axis = 0; axis < 3; ++axis)
        values[1][axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(9, second);
    xport_gte_write_data(10, third >> 16);
    xport_gte_write_data(11, fifth);
    xport_gte_execute(0x4DE012);
    w_u32(output, (values[0][0] & 0xFFFF) | (values[1][0] << 16));
    w_u32(output + 12, (values[0][2] & 0xFFFF) | (values[1][2] << 16));
    for (axis = 0; axis < 3; ++axis)
        values[2][axis] = xport_gte_read_data(25 + axis);
    w_u32(output + 4, (values[2][0] & 0xFFFF) | (values[0][1] << 16));
    w_u32(output + 8, (values[1][1] & 0xFFFF) | (values[2][1] << 16));
    w_u32(output + 16, values[2][2]);
}

extern "C" void sub_800855B4(uint32 input, uint32 output)
{
    FUNCTION_MARKER(0x800855B4, "SLUS_003.73");
    TransformPackedMatrixForProjection(input, output);
}

void ScaleProjectionIr(uint32 scale)
{
    FUNCTION_MARKER(0x80085DBC, "SLUS_003.73");
    (void)xport_gte_read_data(9);
    xport_gte_write_data(8, scale);
    xport_draft_gte_execute(0x198003D);
}

void AccumulateProjectionVector(uint32 vector, uint32 scale)
{
    FUNCTION_MARKER(0x80085DD8, "SLUS_003.73");
    uint32 x = (uint32)(sint32)(sint16)r_u16(vector);
    uint32 y = (uint32)(sint32)(sint16)r_u16(vector + 2);
    uint32 z = (uint32)(sint32)(sint16)r_u16(vector + 4);
    (void)xport_gte_read_data(9);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(25, x);
    xport_gte_write_data(26, y);
    xport_gte_write_data(27, z);
    xport_draft_gte_execute(0x1A8003E);
}

extern "C" void sub_80085DBC(uint32 scale)
{
    FUNCTION_MARKER(0x80085DBC, "SLUS_003.73");
    ScaleProjectionIr(scale);
}
extern "C" void sub_80085DD8(uint32 vector, uint32 scale)
{
    FUNCTION_MARKER(0x80085DD8, "SLUS_003.73");
    AccumulateProjectionVector(vector, scale);
}

uint32 ConfigureProjectionDepth(uint32 limit, uint32 base, uint32 extent)
{
    FUNCTION_MARKER(0x8007D76C, "SLUS_003.73");
    uint32 x = r_u32(0x800FFADC);
    uint32 square_x = x * x;
    uint32 y = r_u32(0x800FFAE0);
    uint32 square_y = y * y;
    uint32 z = r_u32(0x800FFAE4);
    uint32 square_z = z * z;
    w_u32(0x800FFAE8, limit);
    w_u32(0x800FFACC, base);
    uint32 end = base + extent;
    w_u32(0x800FFAC8, end);
    w_u32(0x800FFAEC, end);
    uint32 root = SquareRoot0(square_x + square_y + square_z);
    end = r_u32(0x800FFAC8);
    uint32 product = end * root;
    uint32 divisor = r_u32(0x800FFAE4);
    uint32 quotient;
    if (!divisor)
        quotient = (sint32)product < 0 ? 1 : 0xFFFFFFFF;
    else if (product == 0x80000000 && divisor == 0xFFFFFFFF)
        quotient = product;
    else
        quotient = (uint32)((sint32)product / (sint32)divisor);
    w_u32(0x800FFAD8, quotient);
    uint32 result = extent < 4096;
    w_u32(0x800FFAD4, 0);
    if (extent < 4096)
    {
        do
        {
            uint32 shift = r_u32(0x800FFAD4) + 1;
            w_u32(0x800FFAD4, shift);
            result = extent < (4096 >> (shift & 31));
        } while (result);
    }
    else if (extent > 4096)
    {
        // TODO Original shift cycle cannot reach extents above 80000000
        do
        {
            uint32 shift = r_u32(0x800FFAD4) - 1;
            w_u32(0x800FFAD4, shift);
            result = ((uint32)4096 << ((0 - shift) & 31)) < extent;
        } while (result);
    }
    return result;
}

void SetProjectionScreenParameters(uint32 x, uint32 y, uint32 depth)
{
    FUNCTION_MARKER(0x8007D8C0, "SLUS_003.73");
    w_u32(0x800FFADC, x);
    w_u32(0x800FFAE0, y);
    w_u32(0x800FFAE4, depth);
    w_u32(0x800FFAFC, 0);
}

extern "C" uint32 sub_8007D76C(uint32 limit, uint32 base, uint32 extent)
{
    FUNCTION_MARKER(0x8007D76C, "SLUS_003.73");
    return ConfigureProjectionDepth(limit, base, extent);
}

extern "C" void sub_8007D8C0(uint32 x, uint32 y, uint32 depth)
{
    FUNCTION_MARKER(0x8007D8C0, "SLUS_003.73");
    SetProjectionScreenParameters(x, y, depth);
}
