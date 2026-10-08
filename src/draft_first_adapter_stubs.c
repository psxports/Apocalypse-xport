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
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800890BC_p2\n");
    abort();
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
    uint32 axis;
    for (axis = 0u; axis < 3u; ++axis)
        xport_gte_write_data(9u + axis, (uint32)input[axis]);
    xport_gte_execute(0x4AA00428u);
    for (axis = 0u; axis < 3u; ++axis)
        output[axis] = xport_gte_read_data(25u + axis);
}

uint32 xport_draft_host_sub_80033900_p3(uint32 a1, uint32 a2, const void *a3, uint32 a4, uint32 a5, uint32 a6)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80033900_p3\n");
    abort();
}

uint32 xport_draft_unknown_critical_argument_8008E9FC(void)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_unknown_critical_argument_8008E9FC\n");
    abort();
}

uint32 xport_draft_host_sub_8006BF04_p1(const void *input)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006BF04_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C40C_p123(void *output, const void *matrix, const void *input)
{
    return native_vector_operation(output, matrix, input, 2u);
}

uint32 xport_draft_host_sub_8006C4EC_p123(void *output, const void *matrix, const void *input)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C4EC_p123\n");
    abort();
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


uint32 xport_draft_host_sub_80068450_p4(uint32 argument1, uint32 argument2, uint32 argument3, void * argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80068450_p4\n");
    abort();
}

uint32 xport_draft_host_sub_80068450_p459(uint32 argument1, uint32 argument2, uint32 argument3, void * argument4, void * argument5, uint32 argument6, uint32 argument7, uint32 argument8, void * argument9, uint32 argument10, uint32 argument11, uint32 argument12)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80068450_p459\n");
    abort();
}

uint32 xport_draft_host_sub_8006C4EC_p13(void * argument1, uint32 argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C4EC_p13\n");
    abort();
}





uint32 xport_draft_host_sub_8008F9FC_p4(uint32 argument1, uint32 argument2, uint32 argument3, void * argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8008F9FC_p4\n");
    abort();
}

uint32 xport_draft_host_sub_80094134_p2(uint32 argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80094134_p2\n");
    abort();
}

uint32 xport_draft_host_sub_80069D2C_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80069D2C_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006B04C_p1(void * argument1)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006B04C_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006613C_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006613C_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8001C8E8_p2(uint32 argument1, void *argument2, uint32 argument3, uint32 argument4, uint32 argument5, uint32 argument6, uint32 argument7, uint32 argument8, uint32 argument9, uint32 argument10, uint32 argument11, uint32 argument12, uint32 argument13)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001C8E8_p2\n");
    abort();
}

uint32 xport_draft_host_sub_80020EF8_p2(uint32 object, void *position, uint32 a3, uint32 a4, uint32 a5)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80020EF8_p2\n");
    abort();
}

uint32 xport_draft_host_sub_800679A4_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800679A4_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C3AC_p1(void * argument1, uint32 argument2, uint32 argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), psx_addr(argument3, 12u), 1u);
}

uint32 xport_draft_host_sub_8006CB28_p1(void * argument1, uint32 argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006CB28_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C808_p2(uint32 argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C808_p2\n");
    abort();
}

uint32 xport_draft_host_sub_8006C564_p123(void * argument1, void * argument2, void * argument3)
{
    return native_vector_operation(argument1, argument2, argument3, 3u);
}

uint32 xport_draft_host_sub_8006C5C4_p13(void * argument1, uint32 argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C5C4_p13\n");
    abort();
}


uint32 xport_draft_host_sub_8006C22C_p2(uint32 argument1, void * argument2)
{
    native_vector_shift_left(psx_addr(argument1, 12u), argument2);
    return argument1;
}

uint32 xport_draft_host_sub_8007BB24_p1(void * argument1)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007BB24_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8007DD04_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007DD04_p1\n");
    abort();
}

uint32 xport_draft_host_sub_800872BC_p12(void * argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800872BC_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8007CD74_p13(void * argument1, uint32 argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007CD74_p13\n");
    abort();
}

uint32 xport_draft_host_sub_80032EE4_p2(uint32 argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80032EE4_p2\n");
    abort();
}

uint32 xport_draft_host_sub_800762A8_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800762A8_p1\n");
    abort();
}

uint32 xport_draft_host_sub_80075F80_p12(void * argument1, void * argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80075F80_p12\n");
    abort();
}

uint32 xport_draft_host_sub_80076274_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80076274_p1\n");
    abort();
}

uint32 xport_draft_host_sub_80075F80_p123(void * argument1, void * argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80075F80_p123\n");
    abort();
}

uint32 xport_draft_host_sub_800762DC_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800762DC_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C34C_p12(void * argument1, void * argument2, uint32 argument3)
{
    return native_vector_operation(argument1, argument2, psx_addr(argument3, 12u), 0u);
}

uint32 xport_draft_host_sub_80076420_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80076420_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8001AA28_p3(uint32 argument1, uint32 argument2, void * argument3, uint32 argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8001AA28_p3\n");
    abort();
}

uint32 xport_draft_host_sub_8006C40C_p1(void * argument1, uint32 argument2, uint32 argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), psx_addr(argument3, 4u), 2u);
}

uint32 xport_draft_host_sub_80066B8C_p13(void * argument1, uint32 argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80066B8C_p13\n");
    abort();
}

uint32 xport_draft_host_sub_8006C1E8_p2(uint32 argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C1E8_p2\n");
    abort();
}

uint32 xport_draft_host_sub_800667CC_p3(uint32 argument1, uint32 argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800667CC_p3\n");
    abort();
}

uint32 xport_draft_host_sub_800665CC_p1(void * argument1, uint32 argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800665CC_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C34C_p1(void * argument1, uint32 argument2, uint32 argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), psx_addr(argument3, 12u), 0u);
}

uint32 xport_draft_host_sub_80021358_p3(uint32 argument1, uint32 argument2, void * argument3, uint32 argument4, uint32 argument5, uint32 argument6, uint32 argument7, uint32 argument8, uint32 argument9, uint32 argument10, uint32 argument11)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80021358_p3\n");
    abort();
}

uint32 xport_draft_host_sub_8006C564_p13(void * argument1, uint32 argument2, void * argument3)
{
    return native_vector_operation(argument1, psx_addr(argument2, 12u), argument3, 3u);
}

uint32 xport_draft_host_sub_8006CBF8_p12(void * argument1, void * argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006CBF8_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8006CCE0_p123(void * argument1, void * argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006CCE0_p123\n");
    abort();
}

uint32 xport_draft_host_sub_8006C730_p12(void * argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C730_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8006C22C_p12(void * argument1, void * argument2)
{
    native_vector_shift_left(argument1, argument2);
    return (uint32)(uintptr_t)argument1;
}

uint32 xport_draft_host_sub_80020FB4_p2345(uint32 argument1, void * argument2, void * argument3, void * argument4, void * argument5)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80020FB4_p2345\n");
    abort();
}

uint32 xport_draft_host_sub_800666DC_p12(void * argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800666DC_p12\n");
    abort();
}


uint32 xport_draft_host_sub_8005BBB0_p3(uint32 object, uint32 position, const void *velocity, uint32 kind, uint32 life, uint32 flags, uint32 parameter, uint32 name)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8005BBB0_p3\n");
    abort();
}

uint32 xport_draft_host_sub_8007CC10_p1(void * argument1, uint32 argument2, uint32 argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007CC10_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8005CEE0_p2(uint32 argument1, void * argument2, uint32 argument3)
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

uint32 xport_draft_host_sub_8007C398_p13(void * argument1, uint32 argument2, void * argument3, uint32 argument4)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007C398_p13\n");
    abort();
}

uint32 xport_draft_host_sub_800666DC_p1(void * argument1, uint32 argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800666DC_p1\n");
    abort();
}

uint32 xport_draft_host_sub_8006C0B8_p2(uint32 argument1, void * argument2)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C0B8_p2\n");
    abort();
}

uint32 xport_draft_host_sub_8006C47C_p13(void * argument1, uint32 argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C47C_p13\n");
    abort();
}

uint32 xport_draft_host_sub_8006C5C4_p123(void * argument1, void * argument2, void * argument3)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C5C4_p123\n");
    abort();
}



uint32 xport_draft_host_sub_8006C40C_p12(void * argument1, void * argument2, uint32 argument3)
{
    return native_vector_operation(argument1, argument2, psx_addr(argument3, 4u), 2u);
}

uint32 xport_draft_host_sub_8006C0FC_p2(uint32 argument1, void * argument2)
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

void xport_draft_gte_execute(uint32 opcode)
{
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

uint32 xport_draft_guest_call1(uint32 target, uint32 argument1)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_guest_call1\n");
    abort();
}

uint32 xport_draft_host_sub_80084D4C_p4(uint32 geometry, uint32 output, uint32 flags, const void *translation)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80084D4C_p4\n");
    abort();
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
    case 24u: return (uint32)state.ofx;
    case 25u: return (uint32)state.ofy;
    case 26u: return (uint32)(sint32)(sint16)state.h;
    case 27u: return (uint32)(sint32)(sint16)state.dqa;
    case 28u: return (uint32)state.dqb;
    case 29u: return (uint32)(sint32)(sint16)state.zsf3;
    case 30u: return (uint32)(sint32)(sint16)state.zsf4;
    case 31u: return (uint32)state.flag;
    default: abort();
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
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800858FC_p2\n");
    abort();
}

uint32 xport_draft_host_sub_800854D8_p1(void *output)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800854D8_p1\n");
    abort();
}

uint32 xport_draft_host_sub_800854F4_p12(const void *input, void *output)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800854F4_p12\n");
    abort();
}

uint32 xport_draft_host_sub_800878DC_p1(const void *matrix)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_800878DC_p1\n");
    abort();
}

uint32 xport_draft_host_sub_80085A08_p2(uint32 input, void *output)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80085A08_p2\n");
    abort();
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

uint32 xport_draft_host_sub_8007FC60_p2(uint32 model, const void *vector)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007FC60_p2\n");
    abort();
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
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80069EF4_p2\n");
    abort();
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
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80066B8C_p123\n");
    abort();
}
uint32 xport_draft_817FC_color_inputs(uint32 index_shift, sint32 vector[3])
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_817FC_color_inputs\n");
    abort();
}
uint32 xport_draft_host_sub_80035478_p2(uint32 object, const void *position, uint32 config, uint32 flags, uint32 initialize, uint32 auxiliary, uint32 index)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80035478_p2\n");
    abort();
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
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007C398_p3\n");
    abort();
}

uint32 xport_draft_host_sub_8007C398_p123(const void *start, const void *end, void *output, uint32 filter, uint32 flags)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007C398_p123\n");
    abort();
}

uint32 xport_draft_host_sub_8007C398_p23(uint32 position, const void *endpoint, void *output, uint32 collision, uint32 flags)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8007C398_p23\n");
    abort();
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
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80066F38_p12\n");
    abort();
}

uint32 xport_draft_unknown_cleanup_result_80017914(void)
{
    fprintf(stderr, "Unknown cleanup result at 80017914\n");
    abort();
}

uint32 xport_draft_host_sub_80035638_p3(uint32 object, uint32 position, const void *velocity, uint32 name, uint32 field5E, uint32 field4A, uint32 field48)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_80035638_p3\n");
    abort();
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
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C1E8_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8006C0B8_p12(void *destination, const void *source)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C0B8_p12\n");
    abort();
}

uint32 xport_draft_host_sub_8006C0B8_p1(void *destination, uint32 source)
{
    fprintf(stderr, "Unimplemented adapter: xport_draft_host_sub_8006C0B8_p1\n");
    abort();
}
