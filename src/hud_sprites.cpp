extern "C"
{
#include "draft_first_signatures.h"
}
#include "hud_sprites.h"
#include "native_object_storage.h"
#include <string.h>

static uint32 ReadHudByte(const uint8 *input, uint32 address, uint32 offset)
{
    return address ? r_u8(address + offset) : input[offset];
}

static uint32 ReadHudHalf(const uint8 *input, uint32 address, uint32 offset)
{
    uint16 value;
    if (address)
        return r_u16(address + offset);
    memcpy(&value, input + offset, 2);
    return value;
}

static uint32 ReadHudWord(const uint8 *input, uint32 address, uint32 offset)
{
    uint32 value;
    if (address)
        return r_u32(address + offset);
    memcpy(&value, input + offset, 4);
    return value;
}

static void WriteHudHalf(uint8 *output, uint32 address, uint32 offset, uint32 value)
{
    uint16 half = (uint16)value;
    if (address)
        w_u16(address + offset, half);
    else
        memcpy(output + offset, &half, 2);
}

uint8 *AllocateHudSprite(const uint8 *texture)
{
    uint32 packet, next, limit, data, uv0, uv1, uv2, tail, buffer, tag, head;
    uint32 address = GuestMemoryAddress(texture);
    FUNCTION_MARKER(0x8006D028, "SLUS_003.73");
    packet = r_u32(0x800FF668);
    limit = r_u32(0x800FF374);
    next = packet + 40;
    if (limit < next)
        return 0;
    w_u32(0x800FF668, next);
    w_u32(packet, 0x09000000);
    w_u32(packet + 4, 0x2C808080);
    data = ReadHudWord(texture, address, 4);
    uv0 = r_u32(data);
    uv1 = r_u32(data + 4);
    uv2 = r_u32(data + 8);
    tail = r_u16(data + 10);
    w_u32(packet + 20, uv1);
    w_u32(packet + 12, uv0);
    w_u32(packet + 28, uv2);
    w_u16(packet + 36, tail);
    buffer = r_u32(0x800FF660);
    tag = r_u32(packet);
    head = r_u32(buffer + 112);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(buffer + 112);
    w_u32(buffer + 112, (head & 0xFF000000) | (packet & 0xFFFFFF));
    return (uint8 *)psx_addr(packet, 40);
}

uint32 PlaceScaledHudSprite(uint32 x, uint32 y, uint8 *packet, const uint8 *texture)
{
    uint32 destination = GuestMemoryAddress(packet), source = GuestMemoryAddress(texture);
    uint32 offset, width, top, height, left, right, bottom;
    sint32 offset_high, width_high;
    FUNCTION_MARKER(0x8006D0D4, "SLUS_003.73");
    offset = (uint32)(sint32)(sint8)ReadHudByte(texture, source, 0) << 9;
    offset_high = (sint32)(((sint64)(sint32)offset * (sint64)0x66666667) >> 32);
    width = ReadHudByte(texture, source, 2) << 9;
    top = y + (uint32)(sint32)(sint8)ReadHudByte(texture, source, 1);
    width_high = (sint32)(((sint64)(sint32)width * (sint64)0x66666667) >> 32);
    height = ReadHudByte(texture, source, 3);
    WriteHudHalf(packet, destination, 10, top);
    WriteHudHalf(packet, destination, 26, top + height);
    left = x + (uint32)(offset_high >> 7) - (uint32)((sint32)offset >> 31);
    WriteHudHalf(packet, destination, 8, left);
    right = left + (uint32)(width_high >> 7);
    WriteHudHalf(packet, destination, 16, right);
    bottom = ReadHudHalf(packet, destination, 26);
    WriteHudHalf(packet, destination, 24, left);
    WriteHudHalf(packet, destination, 18, top);
    WriteHudHalf(packet, destination, 32, right);
    WriteHudHalf(packet, destination, 34, bottom);
    return bottom;
}

uint32 PlaceHudSprite(uint32 x, uint32 y, uint8 *packet, const uint8 *texture)
{
    uint32 destination = GuestMemoryAddress(packet), source = GuestMemoryAddress(texture);
    uint32 width, height, left, top, right, bottom;
    FUNCTION_MARKER(0x8006D164, "SLUS_003.73");
    width = ReadHudByte(texture, source, 2);
    height = ReadHudByte(texture, source, 3);
    left = x + (uint32)(sint32)(sint8)ReadHudByte(texture, source, 0);
    top = y + (uint32)(sint32)(sint8)ReadHudByte(texture, source, 1);
    WriteHudHalf(packet, destination, 8, left);
    WriteHudHalf(packet, destination, 10, top);
    WriteHudHalf(packet, destination, 16, left + width);
    WriteHudHalf(packet, destination, 26, top + height);
    right = ReadHudHalf(packet, destination, 16);
    bottom = ReadHudHalf(packet, destination, 26);
    WriteHudHalf(packet, destination, 18, top);
    WriteHudHalf(packet, destination, 24, left);
    WriteHudHalf(packet, destination, 32, right);
    WriteHudHalf(packet, destination, 34, bottom);
    return top;
}

extern "C" uint32 sub_8006D028(uint32 texture)
{
    FUNCTION_MARKER(0x8006D028, "SLUS_003.73");
    uint8 *packet = AllocateHudSprite((const uint8 *)psx_addr(texture, 8));
    return packet ? GuestMemoryAddress(packet) : 0;
}

extern "C" uint32 sub_8006D0D4(uint32 x, uint32 y, uint32 packet, uint32 texture)
{
    FUNCTION_MARKER(0x8006D0D4, "SLUS_003.73");
    return PlaceScaledHudSprite(x, y, (uint8 *)psx_addr(packet, 40), (const uint8 *)psx_addr(texture, 4));
}

extern "C" uint32 sub_8006D164(uint32 x, uint32 y, uint32 packet, uint32 texture)
{
    FUNCTION_MARKER(0x8006D164, "SLUS_003.73");
    return PlaceHudSprite(x, y, (uint8 *)psx_addr(packet, 40), (const uint8 *)psx_addr(texture, 4));
}

uint8 *AllocateHudRectangle(uint32 x, uint32 y, uint32 width, uint32 height, uint32 red, uint32 green, uint32 blue, uint32 depth)
{
    uint32 packet, limit, next, bottom, right, buffer, slot, tag, head;
    FUNCTION_MARKER(0x8006D1C0, "SLUS_003.73");
    packet = r_u32(0x800FF668);
    limit = r_u32(0x800FF374);
    next = packet + 24;
    if (limit < next)
        return 0;
    w_u32(0x800FF668, next);
    w_u8(packet + 3, 5);
    w_u8(packet + 7, 40);
    bottom = y + height;
    w_u8(packet + 4, red);
    w_u8(packet + 5, green);
    w_u8(packet + 6, blue);
    w_u16(packet + 18, bottom);
    w_u16(packet + 22, bottom);
    buffer = r_u32(0x800FF660);
    right = x + width;
    w_u16(packet + 8, x);
    w_u16(packet + 16, x);
    w_u16(packet + 10, y);
    w_u16(packet + 12, right);
    w_u16(packet + 14, y);
    w_u16(packet + 20, right);
    tag = r_u32(packet);
    slot = buffer + depth * 4;
    head = r_u32(slot + 112);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(slot + 112);
    w_u32(slot + 112, (head & 0xFF000000) | (packet & 0xFFFFFF));
    return (uint8 *)psx_addr(packet, 24);
}

extern "C" uint32 sub_8006D1C0(uint32 x, uint32 y, uint32 width, uint32 height, uint32 red, uint32 green, uint32 blue, uint32 depth)
{
    FUNCTION_MARKER(0x8006D1C0, "SLUS_003.73");
    uint8 *packet = AllocateHudRectangle(x, y, width, height, red, green, blue, depth);
    return packet ? GuestMemoryAddress(packet) : 0;
}

uint32 QueueHudGradientRectangle(uint32 x, uint32 y, uint32 width, uint32 height, uint32 right_red, uint32 right_green, uint32 right_blue, uint32 left_red, uint32 left_green, uint32 left_blue, uint32 depth)
{
    uint32 packet, limit, next, right, bottom, buffer, tag, slot, head, result;
    FUNCTION_MARKER(0x8006D294, "SLUS_003.73");
    packet = r_u32(0x800FF668);
    limit = r_u32(0x800FF374);
    next = packet + 36;
    if (limit < next)
        return 1;
    w_u32(0x800FF668, next);
    w_u8(packet + 3, 8);
    w_u8(packet + 7, 56);
    right = x + width;
    w_u16(packet + 16, right);
    w_u16(packet + 32, right);
    bottom = y + height;
    w_u8(packet + 4, left_red);
    w_u8(packet + 12, right_red);
    w_u8(packet + 5, left_green);
    w_u8(packet + 13, right_green);
    w_u8(packet + 6, left_blue);
    w_u8(packet + 14, right_blue);
    w_u8(packet + 20, left_red);
    w_u8(packet + 28, right_red);
    w_u8(packet + 21, left_green);
    w_u8(packet + 29, right_green);
    w_u8(packet + 22, left_blue);
    w_u8(packet + 30, right_blue);
    w_u16(packet + 26, bottom);
    w_u16(packet + 34, bottom);
    buffer = r_u32(0x800FF660);
    w_u16(packet + 10, y);
    w_u16(packet + 18, y);
    w_u16(packet + 8, x);
    w_u16(packet + 24, x);
    tag = r_u32(packet);
    slot = buffer + depth * 4;
    head = r_u32(slot + 112);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    head = r_u32(slot + 112);
    result = (head & 0xFF000000) | (packet & 0xFFFFFF);
    w_u32(slot + 112, result);
    return result;
}

extern "C" uint32 sub_8006D294(uint32 x, uint32 y, uint32 width, uint32 height, uint32 right_red, uint32 right_green, uint32 right_blue, uint32 left_red, uint32 left_green, uint32 left_blue, uint32 depth)
{
    FUNCTION_MARKER(0x8006D294, "SLUS_003.73");
    return QueueHudGradientRectangle(x, y, width, height, right_red, right_green, right_blue, left_red, left_green, left_blue, depth);
}

