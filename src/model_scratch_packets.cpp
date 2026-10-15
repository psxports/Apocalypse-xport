extern "C"
{
#include "draft_first_signatures.h"
}

void EmitScratchModelPackets(uint32 descriptor, uint32 count)
{
    uint32 ordering, colors, packet, limit, indices0, indices1, addresses[4], screen[4];
    uint32 flags, index, depth, value, fog, shift, color_indices, color, opcode, length;
    uint32 uv0, uv1, uv2, destination, previous, extra, next;
    sint32 winding, current_depth;
    FUNCTION_MARKER(0x80083F38, "SLUS_003.73");
    if (!count)
        return;
    ordering = r_u32(0x800FFB30);
    packet = r_u32(0x800FF668) & 0xFFFFFF;
    colors = r_u32(0x800FFB04);
    limit = r_u32(0x800FF374) - 100;
    do
    {
        indices0 = r_u32(descriptor + 4);
        indices1 = r_u32(descriptor + 8);
        addresses[0] = 0x1F800000 + (indices0 & 65535);
        addresses[1] = 0x1F800000 + (indices0 >> 16);
        addresses[2] = 0x1F800000 + (indices1 & 65535);
        addresses[3] = 0x1F800000 + (indices1 >> 16);
        screen[0] = r_u32(addresses[0]);
        screen[1] = r_u32(addresses[1]);
        screen[2] = r_u32(addresses[2]);
        xport_gte_write_data(12, screen[0]);
        xport_gte_write_data(13, screen[1]);
        xport_gte_write_data(14, screen[2]);
        xport_gte_execute(0x1400006);
        screen[3] = r_u32(addresses[3]);
        flags = r_u32(descriptor);
        if (!(screen[0] & screen[1] & screen[2] & screen[3] & 0x60006000))
        {
            winding = (sint32)xport_gte_read_data(24);
            xport_gte_write_data(15, screen[3]);
            xport_gte_execute(0x1400006);
            if (flags & 0xC0)
            {
                if (winding <= 0)
                    winding = (sint32)xport_gte_read_data(24);
                else
                    goto Visible;
                if (winding < 0)
                {
Visible:
                    w_u32(packet + 8, screen[0]);
                    if (packet >= limit)
                    {
                        // The original exits through a neighboring private-frame epilogue
                        w_u32(0x800FF668, packet);
                        return;
                    }
                    depth = (uint32)(sint32)(sint16)r_u16(addresses[0] + 6);
                    for (index = 1; index < 4; ++index)
                    {
                        value = (uint32)(sint32)(sint16)r_u16(addresses[index] + 6);
                        if ((sint32)(depth - value) < 0)
                            depth = value;
                    }
                    value = (uint32)(sint32)(sint16)r_u16(0x1F800164);
                    shift = (uint32)(sint32)(sint16)r_u16(0x1F800174);
                    current_depth = (sint32)(depth - value);
                    fog = 0;
                    if (current_depth > 0)
                    {
                        if ((sint32)shift < 0)
                            fog = (uint32)(current_depth >> ((0 - shift) & 31));
                        else
                            fog = (uint32)current_depth << (shift & 31);
                    }
                    xport_gte_write_data(8, fog);
                    if (flags & 0x800)
                    {
                        color_indices = r_u32(descriptor + 12);
                        xport_gte_write_data(20, r_u32(colors + ((color_indices << 2) & 0x3FC)));
                        xport_gte_write_data(21, r_u32(colors + ((color_indices >> 6) & 0x3FC)));
                        xport_gte_write_data(22, r_u32(colors + ((color_indices >> 14) & 0x3FC)));
                        xport_gte_execute(0xF8002A);
                        if (flags & 1)
                        {
                            w_u32(packet + 20, screen[1]);
                            w_u32(packet + 32, screen[2]);
                            uv0 = r_u32(descriptor + 20);
                            uv1 = r_u32(descriptor + 24);
                            uv2 = r_u32(descriptor + 28);
                            uv1 |= (flags & 0x180) << 14;
                            w_u32(packet + 12, uv0);
                            w_u32(packet + 24, uv1);
                            w_u32(packet + 36, uv2);
                            xport_gte_write_data(6, r_u32(colors + ((color_indices >> 22) & 0x3FC)));
                            color = xport_gte_read_data(20);
                            w_u32(packet + 16, xport_gte_read_data(21));
                            w_u32(packet + 28, xport_gte_read_data(22));
                            xport_gte_execute(0x780010);
                            if (flags & 16)
                            {
                                opcode = 0x34000000;
                                length = 0x09000000;
                            }
                            else
                            {
                                w_u32(packet + 44, screen[3]);
                                w_u32(packet + 48, uv2 >> 16);
                                opcode = 0x3C000000;
                                w_u32(packet + 40, xport_gte_read_data(22));
                                length = 0x0C000000;
                            }
                        }
                        else
                        {
                            w_u32(packet + 16, screen[1]);
                            w_u32(packet + 24, screen[2]);
                            xport_gte_write_data(6, r_u32(colors + ((color_indices >> 22) & 0x3FC)));
                            color = xport_gte_read_data(20);
                            w_u32(packet + 12, xport_gte_read_data(21));
                            w_u32(packet + 20, xport_gte_read_data(22));
                            xport_gte_execute(0x780010);
                            if (flags & 16)
                            {
                                opcode = 0x30000000;
                                length = 0x06000000;
                            }
                            else
                            {
                                w_u32(packet + 32, screen[3]);
                                opcode = 0x38000000;
                                w_u32(packet + 28, xport_gte_read_data(22));
                                length = 0x08000000;
                            }
                        }
                    }
                    else
                    {
                        xport_gte_write_data(6, r_u32(descriptor + 12));
                        xport_gte_execute(0x780010);
                        if (flags & 1)
                        {
                            uv0 = r_u32(descriptor + 20);
                            uv1 = r_u32(descriptor + 24);
                            uv2 = r_u32(descriptor + 28);
                            w_u32(packet + 16, screen[1]);
                            w_u32(packet + 24, screen[2]);
                            w_u32(packet + 12, uv0);
                            w_u32(packet + 20, uv1 | ((flags & 0x180) << 14));
                            w_u32(packet + 28, uv2);
                            color = xport_gte_read_data(22);
                            if (flags & 16)
                            {
                                opcode = 0x24000000;
                                length = 0x07000000;
                            }
                            else
                            {
                                w_u32(packet + 32, screen[3]);
                                w_u32(packet + 36, uv2 >> 16);
                                opcode = 0x2C000000;
                                length = 0x09000000;
                            }
                        }
                        else
                        {
                            w_u32(packet + 12, screen[1]);
                            w_u32(packet + 16, screen[2]);
                            color = xport_gte_read_data(22);
                            if (flags & 16)
                            {
                                opcode = 0x20000000;
                                length = 0x04000000;
                            }
                            else
                            {
                                w_u32(packet + 20, screen[3]);
                                opcode = 0x28000000;
                                length = 0x05000000;
                            }
                        }
                    }
                    w_u32(packet + 4, color | opcode | ((flags & 64) << 19));
                    if (flags & 0x6000)
                    {
                        extra = (uint32)(sint32)(sint16)r_u16(0x1F800204);
                        if (!(flags & 0x2000))
                            extra = (uint32)(sint32)(sint16)r_u16(0x1F800214);
                        depth += extra;
                    }
                    destination = ordering + (depth < 0x4000 ? depth & 0xFFFC : 0x3FFC);
                    previous = r_u32(destination);
                    if (flags & 32)
                    {
                        length += 0x03000000;
                        extra = r_u32(descriptor + 32);
                        w_u32(packet, previous | length);
                        next = packet + (length >> 22) + 4;
                        w_u32(destination, next);
                        w_u32(next - 12, 0);
                        w_u32(next - 8, 0xE2000000);
                        w_u32(next - 4, 0);
                        w_u32(next, packet | 0x02000000);
                        w_u32(next + 4, extra);
                        w_u32(next + 8, 0);
                        packet = next + 12;
                    }
                    else
                    {
                        w_u32(destination, packet);
                        w_u32(packet, previous | length);
                        next = packet + (length >> 22) + 4;
                        if ((flags & 0x41) == 0x40)
                        {
                            w_u32(next + 4, ((flags & 0x180) >> 2) | 0xE1000200);
                            w_u32(next, packet | 0x01000000);
                            w_u32(destination, next);
                            next += 8;
                        }
                        packet = next;
                    }
                }
            }
        }
        descriptor += flags >> 16;
        --count;
    } while (count);
    w_u32(0x800FF668, packet);
}

extern "C" void sub_80083F38(uint32 descriptor, uint32 unused_attributes, uint32 count)
{
    FUNCTION_MARKER(0x80083F38, "SLUS_003.73");
    EmitScratchModelPackets(descriptor, count);
}
