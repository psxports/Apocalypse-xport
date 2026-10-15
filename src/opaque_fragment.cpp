extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "opaque_fragment.h"
#include "native_object_storage.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include <new>
#include <string.h>

// Native boundary preserves live mapped vector reads
static uint32 ReadOpaqueWord(const uint32 *input, uint32 axis)
{
    uint32 address = GuestMemoryAddress(input), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

OpaqueFragment::OpaqueFragment(const uint32 *position, const uint32 *velocity, uint32 floor, uint32 red, uint32 green, uint32 blue, uint32 extent_x, uint32 extent_y, uint32 extent_z) : EffectObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 extents[3] = {extent_x, extent_y, extent_z};
    uint32 vertex, axis, random, value, x, y, z;
    FUNCTION_MARKER(0x80036D48, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B98);
    AddToList((uint32 *)psx_addr(0x800FF454, 4));
    SetPosition(position);
    for (vertex = 0; vertex < 3; ++vertex)
        for (axis = 0; axis < 3; ++axis)
        {
            random = RandomRange(extents[axis] * 2 + 1);
            value = ReadOpaqueWord(position, axis) + ((random - extents[axis]) << 12);
            w_u32(object + 72 + vertex * 12 + axis * 4, value);
        }
    x = ReadOpaqueWord(velocity, 0);
    y = ReadOpaqueWord(velocity, 1);
    z = ReadOpaqueWord(velocity, 2);
    w_u32(object + 36, x);
    w_u32(object + 40, y);
    w_u32(object + 44, z);
    w_u32(object + 108, floor);
    red = (red & 255) >> 2;
    green = (green & 255) >> 2;
    blue = (blue & 255) >> 2;
    w_u8(object + 112, red);
    w_u8(object + 113, green);
    w_u8(object + 115, red);
    w_u8(object + 116, green);
    w_u8(object + 114, blue);
    w_u8(object + 117, blue);
    w_u16(object + 10, RandomRange(30) + 30);
    w_u8(object + 118, 4);
}

OpaqueFragment &OpaqueFragment::Attach(uint32 object)
{
    return *static_cast<OpaqueFragment *>(FindNativeObjectStorage(object));
}

uint32 OpaqueFragment::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80036F98, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B98);
    RemoveFromList((uint32 *)psx_addr(0x800FF454, 4));
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 OpaqueFragment::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 velocity_y, position_y, floor, random, last_y, bounce, x, z, red, green, blue, lifetime, step, result;
    FUNCTION_MARKER(0x80036FFC, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(object + 72, 12), (const uint32 *)psx_addr(object + 36, 12));
    AddVectors((uint32 *)psx_addr(object + 84, 12), (const uint32 *)psx_addr(object + 36, 12));
    AddVectors((uint32 *)psx_addr(object + 96, 12), (const uint32 *)psx_addr(object + 36, 12));
    velocity_y = r_u32(object + 40);
    position_y = r_u32(object + 76);
    floor = r_u32(object + 108);
    w_u32(object + 40, velocity_y + 10000);
    if ((sint32)floor < (sint32)position_y)
    {
        if ((sint16)r_u16(object + 8) < 2)
        {
            random = RandomRange(50);
            w_u32(object + 76, r_u32(object + 108) - (random << 12));
            random = RandomRange(50);
            w_u32(object + 88, r_u32(object + 108) - (random << 12));
            random = RandomRange(50);
            floor = r_u32(object + 108);
            velocity_y = r_u32(object + 40);
            last_y = floor - (random << 12);
            bounce = r_u16(object + 8);
            velocity_y = (uint32)((sint32)((uint32)0 - velocity_y) >> 2);
            w_u32(object + 100, last_y);
            w_u32(object + 40, velocity_y);
            w_u16(object + 8, bounce + 1);
        }
        else
            w_u32(object + 40, 0);
        x = r_u32(object + 36);
        z = r_u32(object + 44);
        w_u32(object + 36, (sint32)x >> 1);
        w_u32(object + 44, (sint32)z >> 1);
    }
    if ((sint16)r_u16(object + 8) >= 2 || RandomRange(5))
    {
        red = r_u8(object + 112);
        green = r_u8(object + 113);
        blue = r_u8(object + 114);
        w_u8(object + 115, red);
        w_u8(object + 116, green);
        w_u8(object + 117, blue);
    }
    else
    {
        w_u8(object + 115, 48);
        w_u8(object + 116, 48);
        w_u8(object + 117, 64);
    }
    lifetime = r_u16(object + 10);
    if (lifetime)
    {
        lifetime = (lifetime - 1) & 65535;
        w_u16(object + 10, lifetime);
    }
    if (!lifetime)
    {
        red = r_u8(object + 112);
        step = r_u8(object + 118);
        red = step < red ? red - step : 0;
        green = r_u8(object + 113);
        step = r_u8(object + 118);
        w_u8(object + 112, red);
        green = step < green ? green - step : 0;
        blue = r_u8(object + 114);
        step = r_u8(object + 118);
        w_u8(object + 113, green);
        blue = step < blue ? blue - step : 0;
        w_u8(object + 114, blue);
    }
    red = r_u8(object + 112);
    green = r_u8(object + 113);
    blue = r_u8(object + 114);
    result = red | green;
    return (blue | result) ? result : Finish();
}

extern "C" uint32 xport_draft_host_sub_80036D48_p23(uint32 object, const void *position, const void *velocity, uint32 floor, uint32 red, uint32 green, uint32 blue, uint32 extent_x, uint32 extent_y, uint32 extent_z)
{
    FUNCTION_MARKER(0x80036D48, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(OpaqueFragment));
    new (dram_addr) OpaqueFragment((const uint32 *)position, (const uint32 *)velocity, floor, red, green, blue, extent_x, extent_y, extent_z);
    return object;
}

extern "C" uint32 sub_80036D48(uint32 object, uint32 position, uint32 velocity, uint32 floor, uint32 red, uint32 green, uint32 blue, uint32 extent_x, uint32 extent_y, uint32 extent_z)
{
    FUNCTION_MARKER(0x80036D48, "SLUS_003.73");
    return xport_draft_host_sub_80036D48_p23(object, psx_addr(position, 12), psx_addr(velocity, 12), floor, red, green, blue, extent_x, extent_y, extent_z);
}

extern "C" uint32 sub_80036F98(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80036F98, "SLUS_003.73");
    return OpaqueFragment::Attach(object).OpaqueFragment::Destroy(reason);
}

extern "C" uint32 sub_80036FFC(uint32 object)
{
    FUNCTION_MARKER(0x80036FFC, "SLUS_003.73");
    return OpaqueFragment::Attach(object).OpaqueFragment::Update();
}
