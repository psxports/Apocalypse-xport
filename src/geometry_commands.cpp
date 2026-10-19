#include <stdlib.h>

#include "geometry_commands.h"
#include "xport.h"

uint32 CountGeometryLeadingBits(uint32 value)
{
    xport_gte_write_data(30, value);
    return xport_gte_read_data(31);
}

static void SquareGeometryVector(const sint32 input[3], uint32 output[3])
{
    sint32 ir[3];
    uint32 axis, flags = 0;
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, (uint32)input[axis]);
    for (axis = 0; axis < 3; ++axis)
        ir[axis] = (sint32)xport_gte_read_data(9 + axis);
    /* Original SQR uses sf=0 and lm=1 */
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 square = (uint32)((sint64)ir[axis] * ir[axis]);
        uint32 saturated = square;
        if (square > 32767)
        {
            saturated = 32767;
            flags |= 1 << (24 - axis);
        }
        xport_gte_write_data(25 + axis, square);
        xport_gte_write_data(9 + axis, saturated);
    }
    xport_gte_write_control(31, flags);
    for (axis = 0; axis < 3; ++axis)
        output[axis] = xport_gte_read_data(25 + axis);
}

static sint64 WrapDcpl44(sint64 value, uint32 axis, uint32 *flags)
{
    uint64 bits;
    if (value > 0x7FFFFFFFFFFLL)
        *flags |= 1 << (30 - axis);
    if (value < -0x80000000000LL)
        *flags |= 1 << (27 - axis);
    bits = (uint64)value & 0xFFFFFFFFFFFULL;
    return (sint64)(bits & 0x7FFFFFFFFFFULL) - ((bits & 0x80000000000ULL) ? 0x80000000000LL : 0);
}

static sint64 ShiftDcplArithmetic(sint64 value, uint32 shift)
{
    sint64 divisor = (sint64)(1ULL << shift);
    return value >= 0 ? value / divisor : -1 - ((-1 - value) / divisor);
}

static sint32 SaturateDcplIr(sint32 value, uint32 axis, uint32 *flags)
{
    if (value < -32768)
    {
        *flags |= 1 << (24 - axis);
        return -32768;
    }
    if (value > 32767)
    {
        *flags |= 1 << (24 - axis);
        return 32767;
    }
    return value;
}

static void ExecuteUnshiftedDcpl(void)
{
    PsxGteSnapshot state;
    uint32 axis, flags = 0, color;
    psx_gte_snapshot(&state);
    color = state.rgbc & 0xFF000000;
    for (axis = 0; axis < 3; ++axis)
    {
        sint64 base = (sint64)((state.rgbc >> (axis * 8)) & 255) * state.ir[axis] * 16;
        sint64 difference = WrapDcpl44((sint64)state.far_color[axis] * 4096 - base, axis, &flags);
        sint32 delta = SaturateDcplIr((sint32)ShiftDcplArithmetic(difference, 12), axis, &flags);
        sint64 value = WrapDcpl44(base + (sint64)state.ir0 * delta, axis, &flags);
        sint32 mac = (sint32)(uint32)ShiftDcplArithmetic(value, 12);
        sint32 ir = SaturateDcplIr(mac, axis, &flags);
        sint32 channel = (sint32)ShiftDcplArithmetic(mac, 4);
        if (channel < 0)
        {
            flags |= 1 << (21 - axis);
            channel = 0;
        }
        else if (channel > 255)
        {
            flags |= 1 << (21 - axis);
            channel = 255;
        }
        xport_gte_write_data(25 + axis, (uint32)mac);
        xport_gte_write_data(9 + axis, (uint32)ir);
        color |= (uint32)channel << (axis * 8);
    }
    xport_gte_write_data(20, state.rgb[1]);
    xport_gte_write_data(21, state.rgb[2]);
    xport_gte_write_data(22, color);
    xport_gte_write_control(31, flags);
}

void ExecuteGeometryCommand(uint32 opcode)
{
    if (opcode == 0x680029)
    {
        ExecuteUnshiftedDcpl();
        return;
    }
    if ((opcode & 0x3F) == 0x3E && !(opcode & 0x80000) && !(opcode & 0x400))
    {
        SVECTOR input;
        VECTOR output;
        sint32 scale = (sint16)xport_gte_read_data(8);
        /* GPL sf0 lm0 preserves unshifted MAC accumulation */
        input.vx = (sint16)xport_gte_read_data(9);
        input.vy = (sint16)xport_gte_read_data(10);
        input.vz = (sint16)xport_gte_read_data(11);
        input.pad = 0;
        gte_gpl0(&input, scale, &output);
        return;
    }
    if ((opcode & 0x3F) == 0x3E && (opcode & 0x80000) && !(opcode & 0x400))
    {
        SVECTOR input;
        VECTOR output;
        sint32 scale = (sint16)xport_gte_read_data(8);
        /* GPL sf1 lm0 accumulates the existing MAC values */
        input.vx = (sint16)xport_gte_read_data(9);
        input.vy = (sint16)xport_gte_read_data(10);
        input.vz = (sint16)xport_gte_read_data(11);
        input.pad = 0;
        gte_gpl12(&input, scale, &output);
        return;
    }
    if ((opcode & 0x3F) == 0x28 && !(opcode & 0x80000) && (opcode & 0x400))
    {
        sint32 input[3];
        uint32 output[3], axis;
        /* SQR sf0 lm1 consumes the current signed IR operands */
        for (axis = 0; axis < 3; ++axis)
            input[axis] = (sint32)xport_gte_read_data(9 + axis);
        SquareGeometryVector(input, output);
        return;
    }
    if ((opcode & 0x3F) == 0x3D && !(opcode & 0x400))
    {
        SVECTOR input;
        VECTOR output;
        sint32 scale = (sint16)xport_gte_read_data(8);
        input.vx = (sint16)xport_gte_read_data(9);
        input.vy = (sint16)xport_gte_read_data(10);
        input.vz = (sint16)xport_gte_read_data(11);
        input.pad = 0;
        if (opcode & 0x80000)
            gte_gpf12(&input, scale, &output);
        else
            gte_gpf0(&input, scale, &output);
        return;
    }
    xport_gte_execute(opcode);
}

uint32 ReadGeometryControl(uint32 index)
{
    PsxGteSnapshot state;
    const MATRIX *matrix;
    uint32 lane;
    uint32 first;
    uint32 second;
    psx_gte_snapshot(&state);
    if (index < 5 || (index >= 8 && index < 13) || (index >= 16 && index < 21))
    {
        matrix = index < 5 ? &state.rotation : (index < 13 ? &state.light : &state.color);
        lane = (index & 7) * 2;
        first = (uint16)matrix->m[lane / 3][lane % 3];
        if (lane == 8)
            return (uint32)(sint32)(sint16)first;
        ++lane;
        second = (uint16)matrix->m[lane / 3][lane % 3];
        return first | (second << 16);
    }
    if (index >= 5 && index <= 7)
        return (uint32)state.translation[index - 5];
    if (index >= 13 && index <= 15)
        return (uint32)state.back_color[index - 13];
    if (index >= 21 && index <= 23)
        return (uint32)state.far_color[index - 21];
    switch (index)
    {
        case 24:
            return (uint32)state.ofx;
        case 25:
            return (uint32)state.ofy;
        case 26:
            return (uint32)(sint32)(sint16)state.h;
        case 27:
            return (uint32)(sint32)(sint16)state.dqa;
        case 28:
            return (uint32)state.dqb;
        case 29:
            return (uint32)(sint32)(sint16)state.zsf3;
        case 30:
            return (uint32)(sint32)(sint16)state.zsf4;
        case 31:
            return (uint32)state.flag;
        default:
            abort();
    }
}
