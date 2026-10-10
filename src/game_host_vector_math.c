#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <string.h>

static uint32 math_word(const void *input, unsigned index)
{
    uint32 value;
    memcpy(&value, (const unsigned char *)input + index * 4u, 4u);
    return value;
}

static void math_store(void *output, unsigned index, uint32 value)
{
    memcpy((unsigned char *)output + index * 4u, &value, 4u);
}

static uint32 direction_vector(void *output, uint32 scale, const void *angles)
{
    uint16 pitch, yaw;
    uint32 product, value;
    memcpy(&pitch, angles, 2u);
    memcpy(&yaw, (const unsigned char *)angles + 2u, 2u);
    product = scale * (uint32)(sint32)(sint16)r_u16(0x800F863Eu + 4u * (pitch & 4095u));
    value = 0u - ((uint32)((sint32)product >> 12) * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (yaw & 4095u)));
    math_store(output, 0u, value);
    memcpy(&pitch, angles, 2u);
    value = scale * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (pitch & 4095u));
    math_store(output, 1u, value);
    memcpy(&pitch, angles, 2u);
    product = scale * (uint32)(sint32)(sint16)r_u16(0x800F863Eu + 4u * (pitch & 4095u));
    memcpy(&yaw, (const unsigned char *)angles + 2u, 2u);
    value = 0u - ((uint32)((sint32)product >> 12) * (uint32)(sint32)(sint16)r_u16(0x800F863Eu + 4u * (yaw & 4095u)));
    math_store(output, 2u, value);
    return value;
}

uint32 xport_draft_host_sub_800667CC_p1(void *output, uint32 scale, uint32 angles)
{
    return direction_vector(output, scale, psx_addr(angles, 4u));
}

uint32 xport_draft_host_sub_800667CC_p13(void *output, uint32 scale, const void *angles)
{
    return direction_vector(output, scale, angles);
}

static uint32 signed_quotient(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

void xport_draft_host_sub_8006C4EC_p12(void *output, void *input, uint32 divisor)
{
    uint32 result[3];
    unsigned index;
    for (index = 0u; index < 3u; ++index)
        result[index] = signed_quotient(math_word(input, index), r_u32(divisor));
    for (index = 0u; index < 3u; ++index)
        math_store(output, index, result[index]);
}

void xport_draft_host_sub_8006C47C_p123(void *output, const void *scale, const void *input)
{
    uint32 result[3];
    unsigned index;
    for (index = 0u; index < 3u; ++index)
        result[index] = math_word(input, index) * math_word(scale, 0u);
    for (index = 0u; index < 3u; ++index)
        math_store(output, index, result[index]);
}

uint32 xport_draft_host_sub_8006689C_p2(uint32 origin, const void *target)
{
    uint32 x = (uint32)((sint32)(r_u32(origin) - math_word(target, 0u)) >> 12);
    uint32 y = (uint32)((sint32)(r_u32(origin + 4u) - math_word(target, 1u)) >> 12);
    uint32 z = (uint32)((sint32)(r_u32(origin + 8u) - math_word(target, 2u)) >> 12);
    return sub_80085B54(x * x + y * y + z * z);
}

uint32 xport_draft_host_sub_8006696C_p2(uint32 origin, void *target)
{
    uint32 delta[3], value;
    sint32 x, y, z;
    unsigned index;
    for (index = 0u; index < 3u; ++index)
    {
        delta[index] = r_u32(origin + index * 4u) - math_word(target, index);
        if ((sint32)delta[index] < 0)
            delta[index] = 0u - delta[index];
    }
    x = (sint32)delta[0];
    y = (sint32)delta[1];
    z = (sint32)delta[2];
    if (x < y)
    {
        if (y < z)
            value = (uint32)(x >> 2) + (uint32)(y >> 1) + (uint32)z;
        else if (z < x)
            value = (uint32)(z >> 2) + (uint32)(x >> 1) + (uint32)y;
        else
            value = (uint32)(x >> 2) + (uint32)(z >> 1) + (uint32)y;
    }
    else
    {
        if (x < z)
            value = (uint32)(y >> 2) + (uint32)(x >> 1) + (uint32)z;
        else if (z < y)
            value = (uint32)(z >> 2) + (uint32)(y >> 1) + (uint32)x;
        else
            value = (uint32)(y >> 2) + (uint32)(z >> 1) + (uint32)x;
    }
    return (uint32)((sint32)value >> 12);
}

void xport_draft_host_sub_80022B54_p2(uint32 object, void *position, uint32 angles)
{
    uint32 direction[3];
    unsigned index;
    (void)object;
    direction_vector(direction, 100u, psx_addr(angles, 4u));
    for (index = 0u; index < 3u; ++index)
        math_store(position, index, math_word(position, index) - direction[index]);
}
