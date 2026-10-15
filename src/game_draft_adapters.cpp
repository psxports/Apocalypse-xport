#include "game_sdk_bindings.h"
extern uint32 CompleteCdReadDma(void);
extern "C"
{
#include "draft_first_adapters.h"
#include "draft_first_signatures.h"
#include "xport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* TODO Replace reached adapters with recovered service contracts */

uint32 xport_draft_host_sub_800981F4_p1(void *buffer, uint32 count)
{
    FUNCTION_MARKER(0x800981F4, "SLUS_003.73");
    uint32 success = CdGetSector(buffer, (sint32)count) != 0;
    if (success && r_u32(0x800F82BC) == 0x8009A71C)
        CompleteCdReadDma();
    return success;
}

uint32 xport_draft_host_sub_800981D4_p1(void *destination, uint32 count)
{
    FUNCTION_MARKER(0x800981D4, "SLUS_003.73");
    return CdGetSector(destination, (sint32)count) != 0;
}

void xport_draft_host_sub_80093ED8_p2(uint32 voice, const void *attributes)
{
    FUNCTION_MARKER(0x80093ED8, "SLUS_003.73");
    SsQueueRegisters((sint32)voice, (SndRegisterAttr *)attributes);
}

uint32 xport_draft_host_sub_800890BC_p2(uint32 packet, const void *rectangle)
{
    FUNCTION_MARKER(0x800890BC, "SLUS_003.73");
    PSX_RECT bounds;
    uint32 words[3], index;
    const uint8 *bytes = (const uint8 *)rectangle;
    sint32 width = (sint16)r_u16(0x800FCEB0);
    sint32 height = (sint16)r_u16(0x800FCEB2);
    bounds.x = (sint16)xport_load_le16(bytes);
    bounds.y = (sint16)xport_load_le16(bytes + 2);
    bounds.w = (sint16)xport_load_le16(bytes + 4);
    bounds.h = (sint16)xport_load_le16(bytes + 6);
    words[0] = r_u32(packet);
    words[1] = 0;
    words[2] = 0;
    SetDrawArea((DR_AREA *)words, &bounds);
    if (width != 1024 || height != 512)
    {
        for (index = 0; index < 2; ++index)
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
            words[index + 1] = (index ? 0xE4000000 : 0xE3000000) | ((uint32)x & 0x3FF) | (((uint32)y & 0x3FF) << 10);
        }
    }
    w_u8(packet + 3, 2);
    w_u32(packet + 4, words[1]);
    w_u32(packet + 8, words[2]);
    return words[2];
}

void xport_draft_gte_control_write(uint32 index, uint32 value)
{
    xport_gte_write_control(index, value);
}

uint32 xport_draft_gte_leading_sign_count(uint32 value)
{
    xport_gte_write_data(30, value);
    return xport_gte_read_data(31);
}

void xport_draft_gte_square_vector(const sint32 input[3], uint32 output[3])
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

uint32 xport_draft_host_sub_80033900_p3(uint32 a1, uint32 a2, const void *a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    FUNCTION_MARKER(0x80033900, "SLUS_003.73");
    return apocalypse_shadow_quad_native(a1, a2, a3, a4, a5, a6, a7);
}

uint32 sub_80085B54(uint32 value);

uint32 xport_draft_host_sub_8006B04C_p1(void *argument1)
{
    FUNCTION_MARKER(0x8006B04C, "SLUS_003.73");
    return apocalypse_resource_lookup((const char *)argument1);
}

uint32 xport_draft_host_sub_8006613C_p1(void *argument1, uint32 argument2)
{
    FUNCTION_MARKER(0x8006613C, "SLUS_003.73");
    return apocalypse_trigger_position((uint32 *)argument1, argument2);
}

uint32 xport_draft_host_sub_8001C8E8_p2(uint32 argument1, void *argument2, uint32 argument3, uint32 argument4, uint32 argument5, uint32 argument6, uint32 argument7, uint32 argument8, uint32 argument9, uint32 argument10, uint32 argument11, uint32 argument12, uint32 argument13)
{
    FUNCTION_MARKER(0x8001C8E8, "SLUS_003.73");
    return apocalypse_trigger_object_create(argument1, (const uint32 *)argument2, argument3, argument4, argument5, argument6, argument7, argument8, argument9, argument10, argument11, argument12, argument13);
}

uint32 xport_draft_host_sub_80020EF8_p2(uint32 resource, void *position, uint32 flags, uint32 yaw, uint32 velocity)
{
    FUNCTION_MARKER(0x80020EF8, "SLUS_003.73");
    return apocalypse_pickup_create_native(resource, position, flags, yaw, velocity);
}

uint32 xport_draft_host_sub_800679A4_p1(void *argument1, uint32 argument2)
{
    FUNCTION_MARKER(0x800679A4, "SLUS_003.73");
    return apocalypse_visibility_native(argument1, argument2);
}

uint32 xport_draft_host_sub_8007BB24_p1(void *argument1)
{
    FUNCTION_MARKER(0x8007BB24, "SLUS_003.73");
    return apocalypse_collision_init(argument1);
}

uint32 xport_draft_host_sub_8007DD04_p1(void *argument1, uint32 argument2)
{
    FUNCTION_MARKER(0x8007DD04, "SLUS_003.73");
    return apocalypse_collision_query(argument1, argument2);
}

uint32 xport_draft_host_sub_800872BC_p12(void *argument1, void *argument2)
{
    FUNCTION_MARKER(0x800872BC, "SLUS_003.73");
    uint32 input[3], output[3], first, second, third, partial, sum, leading, normalized, shift, coefficient;
    memcpy(input, argument1, 12);
    xport_draft_gte_data_write(9, input[0]);
    xport_draft_gte_data_write(10, input[1]);
    xport_draft_gte_data_write(11, input[2]);
    xport_draft_gte_execute(0xA00428);
    first = xport_draft_gte_data_read(25);
    second = xport_draft_gte_data_read(26);
    third = xport_draft_gte_data_read(27);
    partial = first + second;
    sum = partial + third;
    if (((first ^ partial) & (second ^ partial) & 0x80000000) || ((partial ^ sum) & (third ^ sum) & 0x80000000))
    {
        /* TODO Original ADD overflow traps require a native exception contract */
        GameDiagnostic("TODO VectorNormal original ADD overflow\n");
        abort();
    }
    xport_draft_gte_data_write(30, sum);
    leading = xport_draft_gte_data_read(31) & 0xFFFFFFFE;
    shift = (uint32)((sint32)(31 - leading) >> 1) & 31;
    if ((sint32)(leading - 24) < 0)
        normalized = (uint32)((sint32)sum >> ((24 - leading) & 31));
    else
        normalized = sum << ((leading - 24) & 31);
    coefficient = (uint32)(sint32)(sint16)r_u16(0x800F84B0 + (normalized - 64) * 2);
    xport_draft_gte_data_write(8, coefficient);
    xport_draft_gte_data_write(9, input[0]);
    xport_draft_gte_data_write(10, input[1]);
    xport_draft_gte_data_write(11, input[2]);
    xport_draft_gte_execute(0x190003D);
    first = xport_draft_gte_data_read(25);
    second = xport_draft_gte_data_read(26);
    third = xport_draft_gte_data_read(27);
    output[0] = (uint32)((sint32)first >> shift);
    output[1] = (uint32)((sint32)second >> shift);
    output[2] = (uint32)((sint32)third >> shift);
    memcpy(argument2, output, 12);
    return sum;
}

uint32 xport_draft_host_sub_80032EE4_p2(uint32 argument1, void *argument2)
{
    FUNCTION_MARKER(0x80032EE4, "SLUS_003.73");
    uint32 position[3];
    memcpy(position, argument2, sizeof(position));
    w_u32(argument1 + 24, position[0]);
    w_u32(argument1 + 28, position[1]);
    w_u32(argument1 + 32, position[2]);
    return position[0];
}

uint32 xport_draft_host_sub_80066B8C_p13(void *argument1, uint32 argument2, void *argument3)
{
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    return apocalypse_rotation_between((sint16 *)argument1, (const uint32 *)psx_addr(argument2, 12), (const uint32 *)argument3);
}

uint32 xport_draft_host_sub_8005BBB0_p3(uint32 object, uint32 position, const void *velocity, uint32 kind, uint32 life, uint32 flags, uint32 parameter, uint32 name)
{
    FUNCTION_MARKER(0x8005BBB0, "SLUS_003.73");
    return apocalypse_debris_construct_5BBB0(object, position, velocity, kind, life, flags, parameter, name);
}

uint32 sub_8007CF38(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6);
void sub_8008445C(uint32 a1, uint32 a2, uint32 a3);

uint32 xport_draft_host_sub_80066B8C_p1(void *output, uint32 origin, uint32 target)
{
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    return apocalypse_rotation_between((sint16 *)output, (const uint32 *)psx_addr(origin, 12), (const uint32 *)psx_addr(target, 12));
}

uint32 xport_draft_host_sub_8001C158_p23(uint32 a1, const void *a2, const void *a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13)
{
    FUNCTION_MARKER(0x8001C158, "SLUS_003.73");
    return apocalypse_projectile_construct_1C158(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}

uint32 xport_draft_gte_data_read(uint32 index)
{
    return xport_gte_read_data(index);
}

void xport_draft_gte_data_write(uint32 index, uint32 value)
{
    xport_gte_write_data(index, value);
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

static void ExecuteDcpl680029(void)
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

void xport_draft_gte_execute(uint32 opcode)
{
    if (opcode == 0x680029)
    {
        ExecuteDcpl680029();
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
        xport_draft_gte_square_vector(input, output);
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

uint32 sub_8004CFDC(uint32 object, uint32 command);
uint32 sub_800474F0(uint32 object, uint32 command);
uint32 sub_800476D0(uint32 object, uint32 command);
uint32 sub_80059D1C(uint32 object, uint32 command);
uint32 sub_800529AC(uint32 object, uint32 command);
uint32 sub_8004D2B4(uint32 object, uint32 opcode);
uint32 sub_8003C7B4(uint32 object, uint32 command);
uint32 sub_8003CF3C(uint32 object, uint32 command);
uint32 sub_8003D0F0(uint32 object, uint32 command);
uint32 sub_8004DC40(uint32 object, uint32 command);
void sub_8004DB90(uint32 object, uint32 command);

uint32 sub_80044474(uint32 object, uint32 command);
uint32 sub_80044608(uint32 object, uint32 command);
uint32 sub_80049B38(uint32 object, uint32 command);
uint32 sub_8003FCEC(uint32 object, uint32 command);
uint32 sub_8003FF2C(uint32 object, uint32 command);
uint32 sub_80049C48(uint32 object, uint32 command);

uint32 xport_draft_guest_call2(uint32 target, uint32 argument1, uint32 argument2)
{
    uint32 result;
    if (target == 0x80044474)
        return sub_80044474(argument1, argument2);
    if (target == 0x8005AB58)
        return sub_8005AB58(argument1, argument2);
    if (target == 0x80049B38)
        return sub_80049B38(argument1, argument2);
    if (target == 0x8003FCEC)
        return sub_8003FCEC(argument1, argument2);
    if (target == 0x8003FF2C)
        return sub_8003FF2C(argument1, argument2);
    if (target == 0x80049C48)
        return sub_80049C48(argument1, argument2);
    if (target == 0x80044608)
        return sub_80044608(argument1, argument2);
    if (apocalypse_enemy_script_cpp(target, argument1, argument2, &result))
        return result;
    if (target == 0x8005568C)
        return sub_8005568C(argument1, argument2);
    if (target == 0x80072320)
        return sub_80072320(argument1, argument2);
    if (target == 0x80072204)
    {
        /* The script loader discards the parameter callback return register */
        sub_80072204(argument1, argument2);
        return 0;
    }
    if (target == 0x80053F48)
    {
        /* The parameter setter caller discards the return register */
        sub_80053F48(argument1, argument2);
        return 0;
    }
    if (target == 0x800474F0)
        return sub_800474F0(argument1, argument2);
    if (target == 0x800476D0)
        return sub_800476D0(argument1, argument2);
    if (target == 0x8004CFDC)
        return sub_8004CFDC(argument1, argument2);
    if (target == 0x80059D1C)
        return sub_80059D1C(argument1, argument2);
    if (target == 0x800529AC)
        return sub_800529AC(argument1, argument2);
    if (target == 0x80057840)
        return sub_80057840(argument1, argument2);
    if (target == 0x80058DB8)
        return sub_80058DB8(argument1, argument2);
    if (target == 0x80073C78)
        return sub_80073C78(argument1, argument2);
    if (target == 0x8004A690)
        return sub_8004A690(argument1, argument2);
    if (target == 0x8004D2B4)
        return sub_8004D2B4(argument1, argument2);
    if (target == 0x8003C7B4)
        return sub_8003C7B4(argument1, argument2);
    if (target == 0x8003CF3C)
        return sub_8003CF3C(argument1, argument2);
    if (target == 0x8003D0F0)
        return sub_8003D0F0(argument1, argument2);
    if (target == 0x8004DC40)
        return sub_8004DC40(argument1, argument2);
    if (target == 0x8004DB90)
    {
        /* The script setter caller ignores the return register */
        sub_8004DB90(argument1, argument2);
        return 0;
    }
    GameDiagnostic("Unimplemented adapter: xport_draft_guest_call2 target=%08X object=%08X command=%08X\n", target, argument1, argument2);
    abort();
}

uint32 xport_draft_missing_sub_80024428(uint32 object, uint32 list, uint32 mode)
{
    FUNCTION_MARKER(0x80024428, "SLUS_003.73");
    return sub_80024428(object, list, mode);
}

uint32 sub_8005CF0C(uint32 player);
uint32 sub_8005DD40(uint32 player);
uint32 sub_8004B8C8(uint32 object);
uint32 sub_80062F0C(uint32 object);

uint32 sub_80025BEC(uint32 object);
uint32 sub_80025E2C(uint32 object);

uint32 xport_draft_guest_call1(uint32 target, uint32 argument1)
{
    if (target == 0x8006399C || target == 0x8003A13C)
    {
        // Native ABI carrier for the original void no-op
        return 0;
    }
    if (target == 0x80020264)
        return sub_80020264(argument1);
    if (target == 0x80020168)
        return sub_80020168(argument1);
    if (target == 0x80026C68)
        return sub_80026C68(argument1);
    if (target == 0x8002948C)
        return sub_8002948C(argument1);
    if (target == 0x80026488)
        return sub_80026488(argument1);
    if (target == 0x80042038)
        return sub_80042038(argument1);
    if (target == 0x8002BFFC)
        return sub_8002BFFC(argument1);
    if (target == 0x8002DB9C)
        return sub_8002DB9C(argument1);
    if (target == 0x80025BEC)
        return sub_80025BEC(argument1);
    if (target == 0x80025E2C)
        return sub_80025E2C(argument1);
    if (target == 0x8005DD40)
        return sub_8005DD40(argument1);
    if (target == 0x8005CF0C)
        return sub_8005CF0C(argument1);
    if (target == 0x8004B8C8)
        return sub_8004B8C8(argument1);
    if (target == 0x80062F0C)
        return sub_80062F0C(argument1);
    GameDiagnostic("Unimplemented adapter: xport_draft_guest_call1 target=%08X argument=%08X\n", target, argument1);
    abort();
}

uint32 xport_draft_host_sub_80084D4C_p4(uint32 geometry, uint32 output, uint32 flags, const void *translation)
{
    FUNCTION_MARKER(0x80084D4C, "SLUS_003.73");
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

uint32 xport_draft_host_sub_800878DC_p1(const void *matrix)
{
    FUNCTION_MARKER(0x800878DC, "SLUS_003.73");
    uint32 words[5], index;
    for (index = 0; index < 5; ++index)
        words[index] = xport_load_le32((const uint8 *)matrix + index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, words[index]);
    return 0;
}

void xport_draft_host_sub_8007FC60_p2(uint32 model, const void *vector)
{
    FUNCTION_MARKER(0x8007FC60, "SLUS_003.73");
    {
        (void)(apocalypse_render_geometry(model, (const sint32 *)vector));
        return;
    }
}

uint32 xport_draft_host_sub_8001EB64_p2(uint32 object, const void *position, uint32 red, uint32 green, uint32 blue, uint32 size, uint32 duration)
{
    FUNCTION_MARKER(0x8001EB64, "SLUS_003.73");
    return apocalypse_expanding_glow_native(object, position, red, green, blue, size, duration);
}

uint32 xport_draft_host_sub_80066B8C_p123(void *output, const void *origin, const void *target)
{
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    return apocalypse_rotation_between((sint16 *)output, (const uint32 *)origin, (const uint32 *)target);
}

uint32 xport_draft_host_sub_80035478_p2(uint32 object, const void *position, uint32 config, uint32 flags, uint32 initialize, uint32 auxiliary, uint32 index)
{
    FUNCTION_MARKER(0x80035478, "SLUS_003.73");
    return apocalypse_effect_construct_35478(object, position, config, flags, initialize, auxiliary, index);
}

uint32 xport_draft_host_sub_8007C398_p3(uint32 start, uint32 end, void *output, uint32 filter, uint32 flags)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    return apocalypse_collision_sphere_segment(start, end, output, filter, flags);
}

uint32 xport_draft_host_sub_8007C398_p123(const void *start, const void *end, void *output, uint32 filter, uint32 flags)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    return apocalypse_collision_sphere_segment_native(start, end, output, filter, flags);
}

uint32 xport_draft_host_sub_8007C398_p23(uint32 position, const void *endpoint, void *output, uint32 collision, uint32 flags)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    return apocalypse_collision_sphere_segment_native_end(position, endpoint, output, collision, flags);
}

uint32 xport_draft_unknown_alignment_offset_8001AA28(void)
{
    GameDiagnostic("Unimplemented alignment offset at 8001AA28\n");
    abort();
}

uint32 xport_draft_host_sub_80066B8C_p12(void *output, const void *origin, uint32 target)
{
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    return apocalypse_rotation_between((sint16 *)output, (const uint32 *)origin, (const uint32 *)psx_addr(target, 12));
}

uint32 xport_draft_host_sub_80066F38_p12(void *angles, const void *position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
    FUNCTION_MARKER(0x80066F38, "SLUS_003.73");
    return apocalypse_select_aim_target((sint16 *)angles, (const uint32 *)position, (const sint16 *)psx_addr(a3, 6), a4, a5, a6, a7, a8, a9);
}

void apocalypse_orbit_fire_native(uint32 target, uint32 receiver, const uint32 position[3], const sint16 angles[3])
{
    if (target == 0x80027978)
    {
        apocalypse_beam_fire_27978(receiver, position, angles);
        return;
    }
    /* TODO Audit other orbit attack targets */
    GameDiagnostic("Unsupported native orbit attack: %08X\n", target);
    abort();
}
}
