#include "draft_first_adapters.h"
#include "xport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static uint32 native_vector_operation(void *destination, const void *left, const void *right, uint32 operation)
{
    uint32 values[3], a, b, index;
    for (index = 0u; index < 3u; ++index)
    {
        memcpy(&a, (const uint8 *)left + index * 4u, 4u);
        memcpy(&b, (const uint8 *)right + (operation < 2u ? index * 4u : 0u), 4u);
        if (operation == 0u)
            values[index] = a + b;
        else if (operation == 1u)
            values[index] = a - b;
        else if (operation == 2u)
            values[index] = a * b;
        else
            values[index] = (uint32)((sint32)a >> (b & 31u));
    }
    memcpy(destination, values, sizeof(values));
    return (uint32)(uintptr_t)destination;
}

static void native_vector_shift_left(void *destination, const void *shift)
{
    uint32 index, value, amount;
    for (index = 0u; index < 3u; ++index)
    {
        memcpy(&value, (uint8 *)destination + index * 4u, 4u);
        memcpy(&amount, shift, 4u);
        value <<= amount & 31u;
        memcpy((uint8 *)destination + index * 4u, &value, 4u);
    }
}

/* TODO Replace reached adapters with recovered service contracts */

uint32 xport_draft_host_sub_800981F4_p1(void *buffer, uint32 count)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800981F4_p1\n");
    abort();
}

uint32 xport_draft_host_sub_800981D4_p1(void *destination, uint32 count)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800981D4_p1\n");
    abort();
}

uint32 xport_draft_host_sub_80093ED8_p2(uint32 voice, const void *attributes)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80093ED8_p2\n");
    abort();
}

uint32 xport_draft_host_sub_800890BC_p2(uint32 packet, const void *rectangle)
{
    PSX_RECT bounds;
    uint32 words[3], index;
    const uint8 *bytes = (const uint8 *)rectangle;
    sint32 width = (sint16)r_u16(0x800FCEB0u);
    sint32 height = (sint16)r_u16(0x800FCEB2u);
    bounds.x = (sint16)xport_load_le16(bytes);
    bounds.y = (sint16)xport_load_le16(bytes + 2u);
    bounds.w = (sint16)xport_load_le16(bytes + 4u);
    bounds.h = (sint16)xport_load_le16(bytes + 6u);
    words[0] = r_u32(packet);
    words[1] = 0u;
    words[2] = 0u;
    SetDrawArea(words, &bounds);
    if (width != 1024 || height != 512)
    {
        for (index = 0u; index < 2u; ++index)
        {
            sint32 x = index ? (sint16)(bounds.x + bounds.w - 1) : bounds.x;
            sint32 y = index ? (sint16)(bounds.y + bounds.h - 1) : bounds.y;
            if (x < 0)
                x = 0;
            else if (x > width - 1)
                x = (sint16)(width - 1);
            if (y < 0)
                y = 0;
            else if (y > height - 1)
                y = (sint16)(height - 1);
            words[index + 1u] = (index ? 0xE4000000u : 0xE3000000u) |
                                  ((uint32)x & 0x3FFu) | (((uint32)y & 0x3FFu) << 10);
        }
    }
    w_u8(packet + 3u, 2u);
    w_u32(packet + 4u, words[1]);
    w_u32(packet + 8u, words[2]);
    return words[2];
}
void xport_draft_gte_control_write(uint32 index, uint32 value)
{
    xport_gte_write_control(index, value);
}

uint32 xport_draft_unknown_result_80010184(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_result_80010184\n");
    abort();
}

uint32 xport_draft_unknown_critical_argument_8009AD7C(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_critical_argument_8009AD7C\n");
    abort();
}

uint32 xport_draft_gte_leading_sign_count(uint32 value)
{
    return (uint32)Lzc((sint32)value);
}

void xport_draft_gte_square_vector(const sint32 input[3], uint32 output[3])
{
    sint32 ir[3];
    uint32 axis, flags = 0u;
    for (axis = 0u; axis < 3u; ++axis)
        xport_gte_write_data(9u + axis, (uint32)input[axis]);
    for (axis = 0u; axis < 3u; ++axis)
        ir[axis] = (sint32)xport_gte_read_data(9u + axis);
    /* Original SQR uses sf=0 and lm=1 */
    for (axis = 0u; axis < 3u; ++axis)
    {
        uint32 square = (uint32)((sint64)ir[axis] * ir[axis]);
        uint32 saturated = square;
        if (square > 32767u)
        {
            saturated = 32767u;
            flags |= 1u << (24u - axis);
        }
        xport_gte_write_data(25u + axis, square);
        xport_gte_write_data(9u + axis, saturated);
    }
    xport_gte_write_control(31u, flags);
    for (axis = 0u; axis < 3u; ++axis)
        output[axis] = xport_gte_read_data(25u + axis);
}
uint32 xport_draft_host_sub_80033900_p3(uint32 a1, uint32 a2, const void *a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80033900_p3\n");
    abort();
}

uint32 xport_draft_unknown_critical_argument_8008E9FC(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_critical_argument_8008E9FC\n");
    abort();
}

uint32 sub_80085B54(uint32 value);

uint32 xport_draft_host_sub_8006BF04_p1(const void *input)
{
    uint32 shifted[3], value, sum, axis;
    for (axis = 0u; axis < 3u; ++axis)
    {
        memcpy(&value, (const uint8 *)input + axis * 4u, 4u);
        shifted[axis] = (uint32)((sint32)value >> 12);
    }
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_data_write(9u + axis, shifted[axis]);
    xport_draft_gte_execute(0xA00428u);
    sum = xport_draft_gte_data_read(25u);
    sum += xport_draft_gte_data_read(26u);
    sum += xport_draft_gte_data_read(27u);
    return sub_80085B54(sum);
}

uint32 xport_draft_host_sub_8006C40C_p123(void *output, const void *matrix, const void *input)
{
    return native_vector_operation(output, matrix, input, 2u);
}

uint32 xport_draft_host_sub_8006C4EC_p123(void *output, const void *matrix, const void *input)
{
    uint32 values[3], numerator, denominator, index;
    for (index = 0u; index < 3u; ++index)
    {
        memcpy(&numerator, (const uint8 *)matrix + 4u * index, sizeof(numerator));
        memcpy(&denominator, input, sizeof(denominator));
        if (denominator == 0u)
            values[index] = (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
        else if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
            values[index] = numerator;
        else
            values[index] = (uint32)((long long)(sint32)numerator / (long long)(sint32)denominator);
    }
    memcpy(output, values, sizeof(values));
    return (uint32)(uintptr_t)output;
}

uint32 xport_draft_host_sub_8006C3AC_p13(void *output, uint32 matrix, const void *input)
{
    return native_vector_operation(output, psx_addr(matrix, 12u), input, 1u);
}

uint32 xport_draft_host_sub_8006C34C_p13(void *output, uint32 matrix, const void *input)
{
    return native_vector_operation(output, psx_addr(matrix, 12u), input, 0u);
}

uint32 xport_draft_host_sub_8006C3AC_p123(void *output, const void *matrix, const void *input)
{
    return native_vector_operation(output, matrix, input, 1u);
}

uint32 xport_draft_host_sub_8006C34C_p123(void *output, const void *matrix, const void *input)
{
    return native_vector_operation(output, matrix, input, 0u);
}

uint32 xport_draft_host_sub_80068450_p4(uint32 argument1, uint32 argument2, uint32 argument3, void *argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80068450_p4\n");
    abort();
}

uint32 xport_draft_host_sub_80068450_p459(uint32 argument1, uint32 argument2, uint32 argument3, void *argument4, void *argument5, uint32 argument6, uint32 argument7, uint32 argument8, void *argument9, uint32 argument10, uint32 argument11, uint32 argument12)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80068450_p459\n");
    abort();
}

uint32 xport_draft_host_sub_8006C4EC_p13(void *argument1, uint32 argument2, void *argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C4EC_p13\n");
    abort();
}

uint32 xport_draft_host_sub_8008F9FC_p4(uint32 argument1, uint32 argument2, uint32 argument3, void *argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8008F9FC_p4\n");
    abort();
}

uint32 xport_draft_host_sub_80094134_p2(uint32 argument1, void *argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80094134_p2\n");
    abort();
}

uint32 xport_draft_host_sub_80069D2C_p1(void *argument1, uint32 argument2)
{
    return apocalypse_load_sound_bank((const char *)argument1, argument2);
}

uint32 xport_draft_host_sub_8006B04C_p1(void *argument1)
{
    return apocalypse_resource_lookup((const char *)argument1);
}

uint32 xport_draft_host_sub_8006613C_p1(void *argument1, uint32 argument2)
{
    return apocalypse_trigger_position((uint32 *)argument1, argument2);
}

uint32 xport_draft_host_sub_8001C8E8_p2(uint32 argument1, void *argument2, uint32 argument3, uint32 argument4, uint32 argument5, uint32 argument6, uint32 argument7, uint32 argument8, uint32 argument9, uint32 argument10, uint32 argument11, uint32 argument12, uint32 argument13)
{
    return apocalypse_trigger_object_create(argument1, (const uint32 *)argument2, argument3, argument4, argument5, argument6, argument7, argument8, argument9, argument10, argument11, argument12, argument13);
}

uint32 xport_draft_host_sub_80020EF8_p2(uint32 object, void *position, uint32 a3, uint32 a4, uint32 a5)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80020EF8_p2\n");
    abort();
}

uint32 xport_draft_host_sub_800679A4_p1(void *argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800679A4_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C3AC_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), psx_addr(argument3, 12u), 1u);
}

uint32 xport_draft_host_sub_8006CB28_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    uint16 difference[3];
    uint32 index;
    for (index = 0u; index < 3u; ++index)
        difference[index] = (uint16)(r_u16(argument2 + 2u * index) - r_u16(argument3 + 2u * index));
    memcpy(argument1, difference, sizeof(difference));
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_8006C808_p2(uint32 argument1, void *argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C808_p2\n");
    abort();
}

uint32 xport_draft_host_sub_8006C564_p123(void *argument1, void *argument2, void *argument3)
{
    return native_vector_operation(argument1, argument2, argument3, 3u);
}

uint32 xport_draft_host_sub_8006C5C4_p13(void *argument1, uint32 argument2, void *argument3)
{
    uint32 values[3], index, value, shift;
    for (index = 0u; index < 3u; ++index)
    {
        value = r_u32(argument2 + 4u * index);
        memcpy(&shift, argument3, sizeof(shift));
        values[index] = value << (shift & 31u);
    }
    memcpy(argument1, values, sizeof(values));
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_8006C22C_p2(uint32 argument1, void *argument2)
{
    native_vector_shift_left(psx_addr(argument1, 12u), argument2);
    return argument1;
}

uint32 xport_draft_host_sub_8007BB24_p1(void *argument1)
{
    return apocalypse_collision_init(argument1);
}

uint32 xport_draft_host_sub_8007DD04_p1(void *argument1, uint32 argument2)
{
    return apocalypse_collision_query(argument1, argument2);
}

uint32 xport_draft_host_sub_800872BC_p12(void *argument1, void *argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800872BC_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8007CD74_p13(void *argument1, uint32 argument2, void *argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007CD74_p13\n");
    abort();
}

uint32 xport_draft_host_sub_80032EE4_p2(uint32 argument1, void *argument2)
{
    uint32 position[3];
    memcpy(position, argument2, sizeof(position));
    w_u32(argument1 + 24u, position[0]);
    w_u32(argument1 + 28u, position[1]);
    w_u32(argument1 + 32u, position[2]);
    return position[0];
}

uint32 xport_draft_host_sub_800762A8_p1(void *argument1, uint32 argument2)
{
    uint32 table = 0x800F863Cu + ((argument2 << 1) & 0x3FFCu);
    uint32 cosine = (uint32)(sint32)(sint16)r_u16(table + 2u);
    uint32 sine = (uint32)(sint32)(sint16)r_u16(table);
    uint32 zero = 0u;
    memcpy((uint8 *)argument1, &zero, sizeof(zero));
    memcpy((uint8 *)argument1 + 8u, &zero, sizeof(zero));
    memcpy((uint8 *)argument1 + 4u, &sine, sizeof(sine));
    memcpy((uint8 *)argument1 + 12u, &cosine, sizeof(cosine));
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_80075F80_p12(void *argument1, void *argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80075F80_p12\n");
    abort();
}

uint32 xport_draft_host_sub_80076274_p1(void *argument1, uint32 argument2)
{
    uint32 table = 0x800F863Cu + ((argument2 << 1) & 0x3FFCu);
    uint32 values[4];
    values[3] = (uint32)(sint32)(sint16)r_u16(table + 2u);
    values[0] = (uint32)(sint32)(sint16)r_u16(table);
    values[1] = values[2] = 0u;
    memcpy(argument1, values, sizeof(values));
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_80075F80_p123(void *argument1, void *argument2, void *argument3)
{
    uint32 a[4], b[4], result[4];
    memcpy(b, argument3, sizeof(b));
    memcpy(a, argument2, sizeof(a));
    result[0] = (uint32)((sint32)(b[3]*a[0] + b[0]*a[3] + b[1]*a[2] - b[2]*a[1]) >> 12);
    result[1] = (uint32)((sint32)(b[3]*a[1] + b[1]*a[3] + b[2]*a[0] - b[0]*a[2]) >> 12);
    result[2] = (uint32)((sint32)(b[3]*a[2] + b[2]*a[3] + b[0]*a[1] - b[1]*a[0]) >> 12);
    result[3] = (uint32)((sint32)(b[3]*a[3] - b[0]*a[0] - b[1]*a[1] - b[2]*a[2]) >> 12);
    memcpy(argument1, result, sizeof(result));
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_800762DC_p1(void *argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800762DC_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C34C_p12(void *argument1, void *argument2, uint32 argument3)
{
    return native_vector_operation(argument1, argument2, psx_addr(argument3, 12u), 0u);
}

static sint32 native_quaternion_matrix_element(const void *matrix, uint32 index)
{
    sint16 element;
    memcpy(&element, (const uint8 *)matrix + 2u * index, sizeof(element));
    return element;
}

static uint32 native_quaternion_scaled_product(sint32 value, uint32 factor)
{
    return (uint32)((sint32)((uint32)value * factor) >> 12);
}

static uint32 native_quaternion_inverse_root(uint32 root)
{
    if (root == 0u)
        return 0xFFFFFFFFu;
    return (uint32)(0x800000LL / (long long)(sint32)root);
}

uint32 xport_draft_host_sub_80076420_p1(void *argument1, uint32 argument2)
{
    sint32 diagonal[3], trace, selected, next, last;
    uint32 root, factor, result;
    diagonal[0] = native_quaternion_matrix_element(argument1, 0u);
    diagonal[1] = native_quaternion_matrix_element(argument1, 4u);
    diagonal[2] = native_quaternion_matrix_element(argument1, 8u);
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    selected = diagonal[0] < diagonal[1];
    if (trace > 0)
    {
        root = SquareRoot0((sint32)(((uint32)trace + 4096u) << 12));
        w_u32(argument2 + 12u, (uint32)((sint32)root >> 1));
        factor = native_quaternion_inverse_root(root);
        w_u32(argument2, native_quaternion_scaled_product(native_quaternion_matrix_element(argument1, 5u)
            - native_quaternion_matrix_element(argument1, 7u), factor));
        w_u32(argument2 + 4u, native_quaternion_scaled_product(native_quaternion_matrix_element(argument1, 6u)
            - native_quaternion_matrix_element(argument1, 2u), factor));
        result = native_quaternion_scaled_product(native_quaternion_matrix_element(argument1, 1u)
            - native_quaternion_matrix_element(argument1, 3u), factor);
        w_u32(argument2 + 8u, result);
        return result;
    }
    if (diagonal[selected] < diagonal[2])
        selected = 2;
    next = (sint32)r_u32(0x800ED4C8u + 4u * (uint32)selected);
    last = (sint32)r_u32(0x800ED4C8u + 4u * (uint32)next);
    root = SquareRoot0((sint32)(((uint32)native_quaternion_matrix_element(argument1, 4u * selected)
        - (uint32)native_quaternion_matrix_element(argument1, 4u * next)
        - (uint32)native_quaternion_matrix_element(argument1, 4u * last) + 4096u) << 12));
    w_u32(argument2 + 4u * (uint32)selected, (uint32)((sint32)root >> 1));
    factor = native_quaternion_inverse_root(root);
    w_u32(argument2 + 12u, native_quaternion_scaled_product(native_quaternion_matrix_element(argument1, 3u * next + last)
        - native_quaternion_matrix_element(argument1, 3u * last + next), factor));
    if (next >= 0 && next < 3)
        w_u32(argument2 + 4u * (uint32)next, native_quaternion_scaled_product(native_quaternion_matrix_element(argument1, 3u * selected + next)
            + native_quaternion_matrix_element(argument1, 3u * next + selected), factor));
    result = (uint32)(last < 2);
    if (last >= 0 && last < 3)
    {
        result = native_quaternion_scaled_product(native_quaternion_matrix_element(argument1, 3u * selected + last)
            + native_quaternion_matrix_element(argument1, 3u * last + selected), factor);
        w_u32(argument2 + 4u * (uint32)last, result);
    }
    else if (last >= 2)
        result = 2u;
    return result;
}

uint32 xport_draft_host_sub_8001AA28_p3(uint32 argument1, uint32 argument2, void *argument3, uint32 argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001AA28_p3\n");
    abort();
}

uint32 xport_draft_host_sub_8006C40C_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), psx_addr(argument3, 4u), 2u);
}

uint32 xport_draft_host_sub_80066B8C_p13(void *argument1, uint32 argument2, void *argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80066B8C_p13\n");
    abort();
}

uint32 xport_draft_host_sub_8006C1E8_p2(uint32 argument1, void *argument2)
{
    xport_draft_host_sub_8006C1E8_p12(psx_addr(argument1, 12u), argument2);
    return argument1;
}

uint32 xport_draft_host_sub_800667CC_p3(uint32 argument1, uint32 argument2, void *argument3)
{
    return xport_draft_host_sub_800667CC_p13(psx_addr(argument1, 12u), argument2, argument3);
}

uint32 xport_draft_host_sub_800665CC_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    uint32 table = 0x800F863Cu + 4u * (argument3 & 4095u);
    uint32 left, right, value;
    sint32 component, coefficient;
    component = (sint32)r_u32(argument2) >> 3;
    coefficient = (sint16)r_u16(table + 2u);
    left = (uint32)component * (uint32)coefficient;
    component = (sint32)r_u32(argument2 + 8u) >> 3;
    coefficient = (sint16)r_u16(table);
    right = (uint32)component * (uint32)coefficient;
    value = (uint32)((sint32)(left + right) >> 9);
    memcpy(argument1, &value, sizeof(value));
    value = r_u32(argument2 + 4u);
    memcpy((uint8 *)argument1 + 4u, &value, sizeof(value));
    component = (sint32)r_u32(argument2 + 8u) >> 3;
    coefficient = (sint16)r_u16(table + 2u);
    left = (uint32)component * (uint32)coefficient;
    component = (sint32)r_u32(argument2) >> 3;
    coefficient = (sint16)r_u16(table);
    right = (uint32)component * (uint32)coefficient;
    value = (uint32)((sint32)(left - right) >> 9);
    memcpy((uint8 *)argument1 + 8u, &value, sizeof(value));
    return value;
}

uint32 xport_draft_host_sub_8006C34C_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), psx_addr(argument3, 12u), 0u);
}

uint32 xport_draft_host_sub_80021358_p3(uint32 argument1, uint32 argument2, void *argument3, uint32 argument4, uint32 argument5, uint32 argument6, uint32 argument7, uint32 argument8, uint32 argument9, uint32 argument10, uint32 argument11)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80021358_p3\n");
    abort();
}

uint32 xport_draft_host_sub_8006C564_p13(void *argument1, uint32 argument2, void *argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), argument3, 3u);
}

uint32 xport_draft_host_sub_8006CBF8_p12(void *argument1, void *argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006CBF8_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8006CCE0_p123(void *argument1, void *argument2, void *argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006CCE0_p123\n");
    abort();
}

uint32 xport_draft_host_sub_8006C730_p12(void *argument1, void *argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C730_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8006C22C_p12(void *argument1, void *argument2)
{
    native_vector_shift_left(argument1, argument2);
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_80020FB4_p2345(uint32 argument1, void *argument2, void *argument3, void *argument4, void *argument5)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80020FB4_p2345\n");
    abort();
}

uint32 xport_draft_host_sub_800666DC_p12(void *argument1, void *argument2)
{
    sint32 shifted[3], divisor;
    uint32 squared[3], component, result = 0u, axis;
    for (axis = 0u; axis < 3u; ++axis)
    {
        memcpy(&component, (const uint8 *)argument2 + axis * 4u, 4u);
        shifted[axis] = (sint32)component >> 12;
    }
    xport_draft_gte_square_vector(shifted, squared);
    divisor = (sint32)sub_80085B54(squared[0] + squared[1] + squared[2]);
    if (!divisor)
    {
        component = 0u;
        for (axis = 0u; axis < 3u; ++axis)
            memcpy((uint8 *)argument1 + axis * 4u, &component, 4u);
        return 0u;
    }
    for (axis = 0u; axis < 3u; ++axis)
    {
        memcpy(&component, (const uint8 *)argument2 + axis * 4u, 4u);
        result = component == 0x80000000u && divisor == -1
            ? component : (uint32)((sint32)component / divisor);
        memcpy((uint8 *)argument1 + axis * 4u, &result, 4u);
    }
    return result;
}

uint32 xport_draft_host_sub_8005BBB0_p3(uint32 object, uint32 position, const void *velocity, uint32 kind, uint32 life, uint32 flags, uint32 parameter, uint32 name)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8005BBB0_p3\n");
    abort();
}

uint32 sub_8007CF38(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6);
void sub_8008445C(uint32 a1, uint32 a2, uint32 a3);

uint32 xport_draft_host_sub_8007CC10_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    uint32 entry = 0x800EAEF8u + 64u * r_u8(argument2 + 27u);
    uint32 descriptor = r_u32(entry + 28u) + 8u * argument3;
    uint32 index = r_u16(descriptor + 6u);
    uint32 data = r_u32(entry + 24u), table, interval, frames, vertices;
    uint32 result[3];
    if (r_u16(argument2) & 4u)
        vertices = r_u32(argument2 + 356u) + 24u * index;
    else
    {
        table = data + 8u * r_u8(argument2 + 26u);
        interval = r_u16(table + 10u);
        frames = r_u32(r_u32(entry + 16u) - 4u);
        if (interval == 0u)
            vertices = data + r_u32(table + 4u) + 24u * (r_u8(argument2 + 24u) * frames + index);
        else
        {
            sub_8007CF38(4u, interval + 1u, data, argument2, index, frames);
            vertices = 0x800ED760u + 24u * index;
        }
    }
    sub_8008445C(descriptor, vertices, argument2 + 324u);
    result[0] = xport_draft_gte_data_read(25u) << 12;
    result[1] = xport_draft_gte_data_read(26u) << 12;
    result[2] = xport_draft_gte_data_read(27u) << 12;
    memcpy(argument1, result, sizeof(result));
    return result[1];
}

uint32 xport_draft_host_sub_8005CEE0_p2(uint32 argument1, void *argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8005CEE0_p2\n");
    abort();
}

uint32 xport_draft_host_sub_80066B8C_p1(void *output, uint32 origin, uint32 target)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80066B8C_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8001C158_p23(uint32 a1, const void *a2, const void *a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001C158_p23\n");
    abort();
}

uint32 xport_draft_host_sub_8007C398_p13(void *argument1, uint32 argument2, void *argument3, uint32 argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007C398_p13\n");
    abort();
}

uint32 xport_draft_host_sub_800666DC_p1(void *argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800666DC_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C0B8_p2(uint32 argument1, void *argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C0B8_p2\n");
    abort();
}

uint32 xport_draft_host_sub_8006C47C_p13(void *argument1, uint32 argument2, void *argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C47C_p13\n");
    abort();
}

uint32 xport_draft_host_sub_8006C5C4_p123(void *argument1, void *argument2, void *argument3)
{
    uint32 values[3], index, value, shift;
    for (index = 0u; index < 3u; ++index)
    {
        memcpy(&value, (const uint8 *)argument2 + 4u * index, sizeof(value));
        memcpy(&shift, argument3, sizeof(shift));
        values[index] = value << (shift & 31u);
    }
    memcpy(argument1, values, sizeof(values));
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_8006C40C_p12(void *argument1, void *argument2, uint32 argument3)
{
    return native_vector_operation(argument1, argument2, psx_addr(argument3, 4u), 2u);
}

uint32 xport_draft_host_sub_8006C0FC_p2(uint32 argument1, void *argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C0FC_p2\n");
    abort();
}

uint32 xport_draft_gte_data_read(uint32 index)
{
    return xport_gte_read_data(index);
}

void xport_draft_gte_data_write(uint32 index, uint32 value)
{
    xport_gte_write_data(index, value);
}

static sint64 draft_dcpl_wrap44(sint64 value, uint32 axis, uint32 *flags)
{
    uint64 bits;
    if (value > 0x7FFFFFFFFFFLL)
        *flags |= 1u << (30u - axis);
    if (value < -0x80000000000LL)
        *flags |= 1u << (27u - axis);
    bits = (uint64)value & 0xFFFFFFFFFFFULL;
    return (sint64)(bits & 0x7FFFFFFFFFFULL) - ((bits & 0x80000000000ULL) ? 0x80000000000LL : 0);
}

static sint64 draft_dcpl_sra(sint64 value, uint32 shift)
{
    sint64 divisor = (sint64)(1ULL << shift);
    return value >= 0 ? value / divisor : -1 - ((-1 - value) / divisor);
}

static sint32 draft_dcpl_ir(sint32 value, uint32 axis, uint32 *flags)
{
    if (value < -32768)
    {
        *flags |= 1u << (24u - axis);
        return -32768;
    }
    if (value > 32767)
    {
        *flags |= 1u << (24u - axis);
        return 32767;
    }
    return value;
}

static void draft_dcpl_680029(void)
{
    PsxGteSnapshot state;
    uint32 axis, flags = 0u, color;
    psx_gte_snapshot(&state);
    color = state.rgbc & 0xFF000000u;
    for (axis = 0u; axis < 3u; ++axis)
    {
        sint64 base = (sint64)((state.rgbc >> (axis * 8u)) & 255u) * state.ir[axis] * 16;
        sint64 difference = draft_dcpl_wrap44((sint64)state.far_color[axis] * 4096 - base, axis, &flags);
        sint32 delta = draft_dcpl_ir((sint32)draft_dcpl_sra(difference, 12u), axis, &flags);
        sint64 value = draft_dcpl_wrap44(base + (sint64)state.ir0 * delta, axis, &flags);
        sint32 mac = (sint32)(uint32)draft_dcpl_sra(value, 12u);
        sint32 ir = draft_dcpl_ir(mac, axis, &flags);
        sint32 channel = (sint32)draft_dcpl_sra(mac, 4u);
        if (channel < 0)
        {
            flags |= 1u << (21u - axis);
            channel = 0;
        }
        else if (channel > 255)
        {
            flags |= 1u << (21u - axis);
            channel = 255;
        }
        xport_gte_write_data(25u + axis, (uint32)mac);
        xport_gte_write_data(9u + axis, (uint32)ir);
        color |= (uint32)channel << (axis * 8u);
    }
    xport_gte_write_data(20u, state.rgb[1]);
    xport_gte_write_data(21u, state.rgb[2]);
    xport_gte_write_data(22u, color);
    xport_gte_write_control(31u, flags);
}

void xport_draft_gte_execute(uint32 opcode)
{
    if (opcode == 0x680029u)
    {
        draft_dcpl_680029();
        return;
    }
    if ((opcode & 0x3Fu) == 0x3Eu && !(opcode & 0x80000u) && !(opcode & 0x400u))
    {
        SVECTOR input;
        VECTOR output;
        sint32 scale = (sint16)xport_gte_read_data(8u);
        /* GPL sf0 lm0 preserves unshifted MAC accumulation */
        input.vx = (sint16)xport_gte_read_data(9u);
        input.vy = (sint16)xport_gte_read_data(10u);
        input.vz = (sint16)xport_gte_read_data(11u);
        input.pad = 0;
        gte_gpl0(&input, scale, &output);
        return;
    }
    if ((opcode & 0x3Fu) == 0x3Eu && (opcode & 0x80000u) && !(opcode & 0x400u))
    {
        SVECTOR input;
        VECTOR output;
        sint32 scale = (sint16)xport_gte_read_data(8u);
        /* GPL sf1 lm0 accumulates the existing MAC values */
        input.vx = (sint16)xport_gte_read_data(9u);
        input.vy = (sint16)xport_gte_read_data(10u);
        input.vz = (sint16)xport_gte_read_data(11u);
        input.pad = 0;
        gte_gpl12(&input, scale, &output);
        return;
    }
    if ((opcode & 0x3Fu) == 0x28u && !(opcode & 0x80000u) && (opcode & 0x400u))
    {
        sint32 input[3];
        uint32 output[3], axis;
        /* SQR sf0 lm1 consumes the current signed IR operands */
        for (axis = 0u; axis < 3u; ++axis)
            input[axis] = (sint32)xport_gte_read_data(9u + axis);
        xport_draft_gte_square_vector(input, output);
        return;
    }
    if ((opcode & 0x3Fu) == 0x3Du && !(opcode & 0x400u))
    {
        SVECTOR input;
        VECTOR output;
        sint32 scale = (sint16)xport_gte_read_data(8u);
        input.vx = (sint16)xport_gte_read_data(9u);
        input.vy = (sint16)xport_gte_read_data(10u);
        input.vz = (sint16)xport_gte_read_data(11u);
        input.pad = 0;
        if (opcode & 0x80000u)
            gte_gpf12(&input, scale, &output);
        else
            gte_gpf0(&input, scale, &output);
        return;
    }
    xport_gte_execute(opcode);
}

uint32 xport_draft_guest_call2(uint32 target, uint32 argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_guest_call2\n");
    abort();
}

uint32 xport_draft_missing_sub_80024428(uint32 object, uint32 list, uint32 mode)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_missing_sub_80024428\n");
    abort();
}

uint32 xport_draft_missing_sub_800252CC(uint32 object, uint32 list)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_missing_sub_800252CC\n");
    abort();
}

uint32 xport_draft_missing_sub_80028578(uint32 object, uint32 list)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_missing_sub_80028578\n");
    abort();
}

uint32 xport_draft_missing_sub_8002A5C0(uint32 object, uint32 list)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_missing_sub_8002A5C0\n");
    abort();
}

uint32 xport_draft_missing_sub_80025070(uint32 object, uint32 list)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_missing_sub_80025070\n");
    abort();
}

uint32 xport_draft_missing_sub_8002B4EC(uint32 object, uint32 list)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_missing_sub_8002B4EC\n");
    abort();
}

uint32 xport_draft_unknown_result_8001E650(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_result_8001E650\n");
    abort();
}

uint32 sub_8005CF0C(uint32 player);

uint32 xport_draft_guest_call1(uint32 target, uint32 argument1)
{
    if (target == 0x8005CF0Cu)
        return sub_8005CF0C(argument1);
    fprintf(stderr, "Unimplemented adapter: xport_draft_guest_call1 target=%08X argument=%08X\n", target, argument1);
    abort();
}

uint32 xport_draft_host_sub_80084D4C_p4(uint32 geometry, uint32 output, uint32 flags, const void *translation)
{
    return apocalypse_collision_transform(geometry, output, flags, (const sint32 *)translation);
}

uint32 xport_draft_gte_control_read(uint32 index)
{
    PsxGteSnapshot state;
    const MATRIX *matrix;
    uint32 lane;
    uint32 first;
    uint32 second;
    psx_gte_snapshot(&state);
    if (index < 5u || (index >= 8u && index < 13u) || (index >= 16u && index < 21u))
    {
        matrix = index < 5u ? &state.rotation : (index < 13u ? &state.light : &state.color);
        lane = (index & 7u) * 2u;
        first = (uint16)matrix->m[lane / 3u][lane % 3u];
        if (lane == 8u)
            return (uint32)(sint32)(sint16)first;
        ++lane;
        second = (uint16)matrix->m[lane / 3u][lane % 3u];
        return first | (second << 16);
    }
    if (index >= 5u && index <= 7u)
        return (uint32)state.translation[index - 5u];
    if (index >= 13u && index <= 15u)
        return (uint32)state.back_color[index - 13u];
    if (index >= 21u && index <= 23u)
        return (uint32)state.far_color[index - 21u];
    switch (index)
    {
        case 24u:
            return (uint32)state.ofx;
        case 25u:
            return (uint32)state.ofy;
        case 26u:
            return (uint32)(sint32)(sint16)state.h;
        case 27u:
            return (uint32)(sint32)(sint16)state.dqa;
        case 28u:
            return (uint32)state.dqb;
        case 29u:
            return (uint32)(sint32)(sint16)state.zsf3;
        case 30u:
            return (uint32)(sint32)(sint16)state.zsf4;
        case 31u:
            return (uint32)state.flag;
        default:
            abort();
    }
}

void xport_draft_inputs_80082638(uint32 *vertices, uint32 *stride, uint32 *flags, uint32 *clip_bias, uint32 *clip_mask, uint32 vertex_offsets[3], uint32 *offset)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_inputs_80082638\n");
    abort();
}

void xport_draft_outputs_80082508(uint32 *subdivisions, uint32 *step)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_outputs_80082508\n");
    abort();
}

uint32 xport_draft_continue_80082638(uint32 row_start, uint32 row_end)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_continue_80082638\n");
    abort();
}

void xport_draft_call_80084778(uint32 object, uint32 geometry, sint32 lower[3], sint32 upper[3])
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_call_80084778\n");
    abort();
}

void xport_draft_call_80084814(uint32 object, uint32 geometry, const sint32 lower[3], const sint32 upper[3], sint32 first[3], sint32 second[3])
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_call_80084814\n");
    abort();
}

uint32 xport_draft_call_800848D0(uint32 object, uint32 geometry, const sint32 first[3], const sint32 second[3])
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_call_800848D0\n");
    abort();
}

void xport_draft_gte_transform_local_vector(uint32 opcode, void *vector)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_gte_transform_local_vector\n");
    abort();
}

uint32 xport_draft_unknown_result_8003736C(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_result_8003736C\n");
    abort();
}

void xport_draft_gte_transform_local_vector_to(uint32 opcode, const void *input, void *output)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_gte_transform_local_vector_to\n");
    abort();
}

uint32 xport_draft_host_sub_800858FC_p2(uint32 input, void *output)
{
    apocalypse_rotation_matrix(input, output);
    return 0u;
}
uint32 xport_draft_host_sub_800854D8_p1(void *output)
{
    uint32 index;
    for (index = 0u; index < 5u; ++index)
        xport_store_le32((uint8 *)output + index * 4u, (index & 1u) ? 0u : 4096u);
    return 0u;
}

uint32 xport_draft_host_sub_800854F4_p12(const void *input, void *output)
{
    const uint8 *source = (const uint8 *)input;
    uint8 *destination = (uint8 *)output;
    uint32 first = xport_load_le32(source), second = xport_load_le32(source + 4u);
    uint32 fourth = xport_load_le32(source + 12u), third, fifth, values[3][3], axis;
    xport_gte_write_data(9u, first);
    xport_gte_write_data(10u, second >> 16);
    xport_gte_write_data(11u, fourth);
    xport_gte_execute(0x49E012u);
    third = xport_load_le32(source + 8u);
    for (axis = 0u; axis < 3u; ++axis)
        values[0][axis] = xport_gte_read_data(25u + axis);
    xport_gte_write_data(9u, first >> 16);
    xport_gte_write_data(10u, third);
    xport_gte_write_data(11u, fourth >> 16);
    xport_gte_execute(0x49E012u);
    fifth = xport_load_le32(source + 16u);
    for (axis = 0u; axis < 3u; ++axis)
        values[1][axis] = xport_gte_read_data(25u + axis);
    xport_gte_write_data(9u, second);
    xport_gte_write_data(10u, third >> 16);
    xport_gte_write_data(11u, fifth);
    xport_gte_execute(0x49E012u);
    xport_store_le32(destination, (values[0][0] & 0xFFFFu) | (values[1][0] << 16));
    xport_store_le32(destination + 12u, (values[0][2] & 0xFFFFu) | (values[1][2] << 16));
    for (axis = 0u; axis < 3u; ++axis)
        values[2][axis] = xport_gte_read_data(25u + axis);
    xport_store_le32(destination + 4u, (values[2][0] & 0xFFFFu) | (values[0][1] << 16));
    xport_store_le32(destination + 8u, (values[1][1] & 0xFFFFu) | (values[2][1] << 16));
    xport_store_le32(destination + 16u, values[2][2]);
    return 0u;
}

uint32 xport_draft_host_sub_800878DC_p1(const void *matrix)
{
    uint32 words[5], index;
    for (index = 0u; index < 5u; ++index)
        words[index] = xport_load_le32((const uint8 *)matrix + index * 4u);
    for (index = 0u; index < 5u; ++index)
        xport_gte_write_control(index, words[index]);
    return 0u;
}

uint32 xport_draft_host_sub_80085A08_p2(uint32 input, void *output)
{
    uint8 *matrix = (uint8 *)output;
    uint32 column, row;
    for (column = 0u; column < 3u; ++column)
    {
        SVECTOR vector = {0};
        VECTOR scaled;
        sint32 scale = (sint16)r_u16(input + 36u + column * 2u);
        vector.vx = (sint16)xport_load_le16(matrix + column * 2u);
        vector.vy = (sint16)xport_load_le16(matrix + 6u + column * 2u);
        vector.vz = (sint16)xport_load_le16(matrix + 12u + column * 2u);
        gte_gpf12(&vector, scale, &scaled);
        for (row = 0u; row < 3u; ++row)
            xport_store_le16(matrix + row * 6u + column * 2u, (uint16)xport_gte_read_data(9u + row));
    }
    return 0u;
}

uint32 xport_draft_host_sub_80081458_p12(const void *matrix, void *output, uint32 camera, uint32 model)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80081458_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8007F9E0_p2(uint32 model, const void *vector)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007F9E0_p2\n");
    abort();
}

void xport_draft_host_sub_8007FC60_p2(uint32 model, const void *vector)
{
    { (void)(apocalypse_render_geometry(model, (const sint32 *)vector)); return; }
}

uint32 xport_draft_unknown_result_800817FC(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_result_800817FC\n");
    abort();
}

uint32 xport_draft_unknown_result_8006EE94(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_result_8006EE94\n");
    abort();
}

uint32 xport_draft_host_sub_80067388_p1(const void *position, uint32 a2, uint32 a3, uint32 a4, uint32 priority)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80067388_p1\n");
    abort();
}

uint32 xport_draft_host_sub_80069EF4_p2(uint32 sound, const void *position, uint32 parameter)
{
    uint32 origin[3], balance, gain, left, right;
    if ((sint32)sound < 0) return 0u;
    memcpy(origin, position, sizeof(origin));
    balance = apocalypse_sound_balance_native(origin, 256u, 12000u);
    gain = (uint32)(sint32)(sint16)r_u16(0x800ECC7Au);
    left = ((balance & 0xFFFu) * gain) >> 12;
    right = (((balance >> 16) & 0xFFFu) * gain) >> 12;
    if ((sint32)balance < 0) left = 0u - left;
    return sub_8006A4C4(r_u32(0x800E53A8u + sound * 8u),
        r_u8(0x800E53A8u + ((sound * 8u) | 4u)),
        (sint16)left, (sint16)right, parameter);
}

uint32 xport_draft_host_sub_8001E4C8_p1(const void *position, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001E4C8_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8001CF9C_p2(uint32 a1, const void *position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13, uint32 a14, uint32 a15, uint32 a16, uint32 a17, uint32 a18, uint32 a19, uint32 a20)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001CF9C_p2\n");
    abort();
}

uint32 xport_draft_host_sub_8001EB64_p2(uint32 a1, const void *position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001EB64_p2\n");
    abort();
}

uint32 xport_draft_host_sub_80066B8C_p123(void *output, const void *origin, const void *target)
{
    return apocalypse_rotation_between((sint16 *)output, (const uint32 *)origin, (const uint32 *)target);
}

uint32 xport_draft_817FC_color_inputs(uint32 index_shift, sint32 vector[3])
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_817FC_color_inputs\n");
    abort();
}

uint32 xport_draft_host_sub_80035478_p2(uint32 object, const void *position, uint32 config, uint32 flags, uint32 initialize, uint32 auxiliary, uint32 index)
{
    return apocalypse_effect_construct_35478(object, position, config, flags, initialize, auxiliary, index);
}

uint32 xport_draft_unknown_result_8001BB68(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_result_8001BB68\n");
    abort();
}

uint32 xport_draft_unknown_result_800653F4(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_result_800653F4\n");
    abort();
}

uint32 xport_draft_host_sub_80036D48_p23(uint32 object, const void *position, const void *velocity, uint32 kind, uint32 red, uint32 green, uint32 blue, uint32 a8, uint32 a9, uint32 a10)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80036D48_p23\n");
    abort();
}

void xport_draft_host_sub_80085C04_p2(uint32 source, void *destination)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80085C04_p2\n");
    abort();
}

uint32 xport_draft_host_sub_8007C398_p3(uint32 start, uint32 end, void *output, uint32 filter, uint32 flags)
{
    return apocalypse_collision_sphere_segment(start, end, output, filter, flags);
}

uint32 xport_draft_host_sub_8007C398_p123(const void *start, const void *end, void *output, uint32 filter, uint32 flags)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007C398_p123\n");
    abort();
}

uint32 xport_draft_host_sub_8007C398_p23(uint32 position, const void *endpoint, void *output, uint32 collision, uint32 flags)
{
    return apocalypse_collision_sphere_segment_native_end(position, endpoint, output, collision, flags);
}

uint32 xport_draft_unknown_alignment_offset_8001AA28(void)
{
    fprintf(stderr, "Unimplemented alignment offset at 8001AA28\n");
    abort();
}

uint32 xport_draft_host_sub_8001D320_p1(const void *position, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001D320_p1\n");
    abort();
}

uint32 xport_draft_host_sub_80066B8C_p12(void *output, const void *input, uint32 mode)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80066B8C_p12\n");
    abort();
}

uint32 xport_draft_host_sub_80066F38_p12(void *angles, const void *position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
    return apocalypse_select_aim_target((sint16 *)angles, (const uint32 *)position, (const sint16 *)psx_addr(a3, 6u), a4, a5, a6, a7, a8, a9);
}

uint32 xport_draft_unknown_cleanup_result_80017914(void)
{
    fprintf(stderr, "Unknown cleanup result at 80017914\n");
    abort();
}

uint32 xport_draft_host_sub_80035638_p3(uint32 object, uint32 position, const void *velocity, uint32 name, uint32 field5E, uint32 field4A, uint32 field48)
{
    uint32 words[3];
    memcpy(words, velocity, sizeof(words));
    sub_80034D88(object);
    w_u32(object + 68u, 0x800A1C60u);
    w_u32(object + 24u, r_u32(position));
    w_u32(object + 28u, r_u32(position + 4u));
    w_u32(object + 32u, r_u32(position + 8u));
    w_u32(object + 36u, words[0]);
    w_u32(object + 40u, words[1]);
    w_u32(object + 44u, words[2]);
    sub_80033354(object, name);
    sub_800332A4(object);
    w_u16(object + 94u, (uint16)field5E);
    w_u16(object + 74u, (uint16)field4A);
    w_u16(object + 72u, (uint16)field48);
    return object;
}

uint32 xport_draft_host_sub_80022454_p2345(uint32 kind, const void *vertex0, const void *vertex1, const void *vertex2, const void *offset, uint32 red, uint32 green, uint32 blue)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80022454_p2345\n");
    abort();
}

uint32 xport_draft_unknown_result_800278D0(void)
{
    fprintf(stderr, "Unknown empty cleanup return at 800278D0\n");
    abort();
}

uint32 xport_draft_continue_8008267C(uint32 row_start, uint32 row_end)
{
    fprintf(stderr, "Unknown reverse polygon continuation at 8008267C\n");
    abort();
}

xport_draft_rotation_inputs xport_draft_8267C_rotation_inputs(void)
{
    fprintf(stderr, "Unknown rotation source addresses at 8008267C\n");
    abort();
}

xport_draft_reverse_polygon_inputs xport_draft_8267C_polygon_inputs(void)
{
    fprintf(stderr, "Unknown reverse polygon inputs at 8008267C\n");
    abort();
}

uint32 xport_draft_host_sub_80035124_p2(uint32 object, const void *position, uint32 speed, uint32 lifetime, uint32 random_range)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80035124_p2\n");
    abort();
}

uint32 xport_draft_host_sub_80021358_p123(const void *v0, const void *v1, const void *v2, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80021358_p123\n");
    abort();
}

uint32 xport_draft_host_sub_8006C40C_p13(void *output, uint32 vector, const void *scale)
{
    return native_vector_operation(output, psx_addr(vector, 12u), scale, 2u);
}

uint32 xport_draft_host_sub_8006C47C_p12(void *output, const void *scale, uint32 vector)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C47C_p12\n");
    abort();
}

uint32 xport_draft_host_sub_80078178_p2(uint32 object, const void *position, uint32 mode)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80078178_p2\n");
    abort();
}

uint32 xport_draft_host_sub_8006C1E8_p12(void *vector, const void *scale)
{
    uint32 value, shift, index;
    for (index = 0u; index < 3u; ++index)
    {
        memcpy(&value, (uint8 *)vector + 4u * index, sizeof(value));
        memcpy(&shift, scale, sizeof(shift));
        value = (uint32)((sint32)value >> (shift & 31u));
        memcpy((uint8 *)vector + 4u * index, &value, sizeof(value));
    }
    return (uint32)(uintptr_t)vector;
}

uint32 xport_draft_host_sub_8006C0B8_p12(void *destination, const void *source)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C0B8_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8006C0B8_p1(void *destination, uint32 source)
{
    uint32 index, value;
    for (index = 0u; index < 3u; ++index)
    {
        memcpy(&value, (uint8 *)destination + 4u * index, sizeof(value));
        value += r_u32(source + 4u * index);
        memcpy((uint8 *)destination + 4u * index, &value, sizeof(value));
    }
    return (uint32)(uintptr_t)destination;
}
