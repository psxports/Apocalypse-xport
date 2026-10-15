extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "waving_beam.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "object_collision_events.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

WavingBeam::WavingBeam(const uint32 *position, const uint16 *angles, uint32 length, uint32 height, uint32 lifetime, uint32 red, uint32 green, uint32 blue, uint32 mode, uint32 material)
    : RibbonSegments(10, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 guest_angles = GuestMemoryAddress(angles);
    const uint16 *native_angles = guest_angles ? 0 : angles;
    uint32 upper, lower, word, tail, address, shift, index, buffer, value;
    uint32 colors[3], axis, product;
    FUNCTION_MARKER(0x80010D3C, "SLUS_003.73");
    w_u32(object + 68, 0x800A02F0);
    if (native_angles)
    {
        memcpy(&word, native_angles, 4);
        tail = native_angles[2];
    }
    else
    {
        upper = ReadMasked((guest_angles + 3) & ~3,
            (1 << (((guest_angles + 3) & 3) + 1)) - 1);
        lower = ReadMasked(guest_angles & ~3, (15 << (guest_angles & 3)) & 15);
        shift = 8 * (guest_angles & 3);
        word = shift ? (upper << (32 - shift)) | (lower >> shift) : lower;
        tail = r_u16(guest_angles + 4);
    }
    address = object + 357;
    shift = 8 * (3 - (address & 3));
    w_masked_u32(address & ~3, word >> shift, (1 << ((address & 3) + 1)) - 1);
    address = object + 354;
    shift = 8 * (address & 3);
    w_masked_u32(address & ~3, word << shift, (15 << (address & 3)) & 15);
    w_u16(object + 358, tail);
    w_u32(object + 360, length);
    SetPosition(position);
    w_u16(object + 350, lifetime);
    w_u16(object + 10, height);
    if ((lifetime & 0xFFFF) >= 1000)
    {
        value = r_u16(object + 350);
        w_u8(object + 349, 1);
        w_u16(object + 350, value - 1000);
    }
    w_u8(object + 348, mode);
    SetMaterial(material);
    for (index = 0; index < 10; ++index)
    {
        buffer = r_u32(object + 104) + index * 36;
        value = r_u32(buffer);
        w_u32(object + 188 + index * 4, value);
        value = RandomRange(4096);
        w_u32(object + 228 + index * 4, value);
        w_u32(object + 268 + index * 4, 80);
        w_u32(object + 308 + index * 4, index ? (uint32)((sint32)length / 80) : 0);
        value = RandomRange(16);
        w_u32(object + 108 + index * 8, value);
        if (index)
        {
            product = 148 * r_u32(object + 112 + (index - 1) * 8);
            value = (uint32)((sint32)product / 128);
        }
        else
            value = RandomRange(60) + 20;
        w_u32(object + 112 + index * 8, value);
        // Repeated buffer loads preserve aliases with the object
        buffer = r_u32(object + 104) + index * 36;
        value = r_u16(object + 112 + index * 8);
        w_u16(buffer + 16, value);
        buffer = r_u32(object + 104) + index * 36;
        value = r_u16(buffer + 16);
        w_u16(buffer + 18, value + 40);
        colors[0] = (uint32)((sint32)((9 - index) * red) / 9);
        colors[1] = (uint32)((sint32)((9 - index) * green) / 9);
        colors[2] = (uint32)((sint32)((9 - index) * blue) / 9);
        for (axis = 0; axis < 3; ++axis)
        {
            buffer = r_u32(object + 104) + index * 36;
            w_u8(buffer + 12 + axis, colors[axis]);
        }
        for (axis = 0; axis < 3; ++axis)
        {
            buffer = r_u32(object + 104) + index * 36;
            product = colors[axis] * 48;
            w_u8(buffer + 20 + axis, (uint32)((sint32)product / 256));
        }
    }
}

// Native helper keeps the original full-word LWL/LWR source read
uint32 WavingBeam::ReadMasked(uint32 address, uint32 mask)
{
    uint32 lane, lanes = 0, word = r_u32(address);
    for (lane = 0; lane < 4; ++lane)
        if (mask & (1 << lane))
            lanes |= (uint32)255 << (8 * lane);
    return word & lanes;
}

// Native input bridge for the original three position loads
static uint32 ReadWavingBeamPositionWord(const uint32 *position, uint32 axis)
{
    uint32 guest = GuestMemoryAddress(position), value;
    if (guest)
        return r_u32(guest + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

uint32 WavingBeam::SetPosition(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 step[3], point[3], divisor = 9, index, buffer;
    FUNCTION_MARKER(0x800110AC, "SLUS_003.73");
    BuildDirectionVector(step, r_u32(object + 360), (const uint16 *)psx_addr(object + 354, 4));
    DivideVector(step, &divisor);
    point[0] = ReadWavingBeamPositionWord(position, 0);
    point[1] = ReadWavingBeamPositionWord(position, 1);
    point[2] = ReadWavingBeamPositionWord(position, 2);
    for (index = 0; index < 10; ++index)
    {
        buffer = r_u32(object + 104) + index * 36;
        w_u32(buffer, point[0]);
        w_u32(buffer + 4, point[1]);
        w_u32(buffer + 8, point[2]);
        AddVectors(point, step);
    }
    return 0;
}

WavingBeam &WavingBeam::Attach(uint32 object)
{
    return *static_cast<WavingBeam *>(FindNativeObjectStorage(object));
}

uint32 WavingBeam::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80011058, "SLUS_003.73");
    w_u32(object + 68, 0x800A02F0);
    result = RibbonSegments::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 WavingBeam::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 lifetime, buffer, player, x, z, distance, value, tick, index, offset, product, result, material, target, table, receiver;
    uint32 origin[4], endpoint[4], direction[4], hit[4];
    sint32 wave;
    FUNCTION_MARKER(0x8001116C, "SLUS_003.73");
    lifetime = r_u16(object + 10);
    if (lifetime == 65535)
    {
        buffer = r_u32(object + 104);
        player = r_u32(0x800FF5A0);
        x = (uint32)((sint32)(r_u32(buffer) - r_u32(player + 4)) >> 12);
        if ((sint32)x < 0)
            x = 0 - x;
        z = (uint32)((sint32)(r_u32(buffer + 8) - r_u32(player + 12)) >> 12);
        if ((sint32)z < 0)
            z = 0 - z;
        distance = x < z ? z + x / 2 : x + z / 2;
        if ((sint32)distance >= 10001)
        {
            w_u8(object + 80, 1);
            return 1;
        }
    }
    else
    {
        if (lifetime)
            w_u16(object + 10, lifetime - 1);
        if (!lifetime || lifetime == 1)
            Finish();
    }
    material = r_u32(object + 76);
    w_u8(object + 80, 0);
    if (r_u32(material + 20) != 0x68B7B136)
    {
        tick = r_u32(0x800FF2F0);
        wave = (sint16)r_u16(0x800F863C + ((tick << 6) & 0x3FC0));
        w_u32(object + 84, (uint32)(wave / 128));
    }
    value = r_u8(object + 348);
    result = r_u32(object + 88);
    tick = r_u32(0x800FF2F0);
    w_u32(object + 88, result + value);
    for (index = 0, offset = 0; index < 10; ++index, offset += 36)
    {
        value = r_u32(object + 268 + index * 4) * tick;
        value += r_u32(object + 228 + index * 4);
        wave = (sint16)r_u16(0x800F863C + (value & 4095) * 4);
        product = r_u32(object + 308 + index * 4) * (uint32)wave;
        buffer = r_u32(object + 104) + offset;
        value = r_u32(object + 188 + index * 4);
        value += (uint32)((sint32)product / 4096) << 12;
        w_u32(buffer, value);
    }
    w_u16(r_u32(object + 104) + 16, 20);
    w_u16(r_u32(object + 104) + 18, 60);
    value = RandomRange(60);
    w_u32(object + 112, value + 20);
    for (index = 1, offset = 36; index < 10; ++index, offset += 36)
    {
        buffer = r_u32(object + 104) + offset;
        value = r_u16(buffer + 16);
        product = r_u32(object + 112 + index * 8) - value;
        w_u16(buffer + 16, value + (uint32)((sint32)product / 8));
        buffer = r_u32(object + 104) + offset;
        value = r_u16(buffer + 16);
        w_u16(buffer + 18, value + 40);
        tick = r_u32(0x800FF2F0);
        value = r_u32(object + 108 + index * 8);
        if ((tick & 15) == value)
        {
            value = RandomRange(16);
            w_u32(object + 108 + index * 8, value);
            product = r_u32(object + 112 + (index - 1) * 8) * 148;
            w_u32(object + 112 + index * 8, (uint32)((sint32)product / 128));
        }
    }
    result = r_u16(object + 352);
    if (result)
    {
        w_u16(object + 352, result - 1);
        result = (result - 1) & 65535;
        if (result)
            return result;
    }
    result = r_u16(object + 350);
    if (!result)
        return result;
    buffer = r_u32(object + 104);
    origin[0] = r_u32(buffer);
    origin[1] = r_u32(buffer + 4);
    origin[2] = r_u32(buffer + 8);
    buffer = r_u32(object + 104);
    endpoint[0] = r_u32(buffer + 324);
    endpoint[1] = r_u32(buffer + 328);
    endpoint[2] = r_u32(buffer + 332);
    SubtractVectors(direction, endpoint, origin);
    NormalizeFixedVector(direction, direction);
    result = IntersectObjectSpheres(origin, endpoint, hit, r_u32(0x800FF5A0), 0);
    if (result)
    {
        table = r_u32(result + 68);
        value = r_u16(object + 350);
        receiver = result + (uint32)(sint32)(sint16)r_u16(table + 48);
        target = r_u32(table + 52);
        apocalypse_object_virtual52_native(target, receiver, value, (const sint32 *)direction, 31);
        w_u16(object + 352, 15);
    }
    result = r_u8(object + 349);
    if (result)
    {
        result = IntersectObjectSpheres(origin, endpoint, hit, r_u32(0x800FF4E8), 0);
        if (result)
        {
            table = r_u32(result + 68);
            value = r_u16(object + 350);
            receiver = result + (uint32)(sint32)(sint16)r_u16(table + 48);
            target = r_u32(table + 52);
            apocalypse_object_virtual52_native(target, receiver, value, (const sint32 *)direction, 31);
            result = 15;
            w_u16(object + 352, result);
        }
    }
    return result;
}

extern "C" uint32 sub_80011058(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80011058, "SLUS_003.73");
    return WavingBeam::Attach(object).WavingBeam::Destroy(reason);
}

extern "C" uint32 sub_8001116C(uint32 object)
{
    FUNCTION_MARKER(0x8001116C, "SLUS_003.73");
    return WavingBeam::Attach(object).WavingBeam::Update();
}

// Compatibility entry points retain the guest call contract
uint32 sub_80010D3C(uint32 object, uint32 position, uint32 angles, uint32 length, uint32 height, uint32 lifetime, uint32 red, uint32 green, uint32 blue, uint32 mode, uint32 material)
{
    FUNCTION_MARKER(0x80010D3C, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(WavingBeam));
    new (storage) WavingBeam((const uint32 *)psx_addr(position, 12), (const uint16 *)psx_addr(angles, 6), length, height, lifetime, red, green, blue, mode, material);
    return object;
}

uint32 apocalypse_effect10d_native(uint32 object, uint32 position, const uint16 angles[3], uint32 length, uint32 height, uint32 lifetime, uint32 red, uint32 green, uint32 blue, uint32 mode, uint32 material)
{
    void *storage = AllocateNativeObjectStorage(object, sizeof(WavingBeam));
    new (storage) WavingBeam((const uint32 *)psx_addr(position, 12), angles, length, height, lifetime, red, green, blue, mode, material);
    return object;
}

uint32 sub_800110AC(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x800110AC, "SLUS_003.73");
    return WavingBeam::Attach(object).SetPosition((const uint32 *)psx_addr(position, 12));
}
