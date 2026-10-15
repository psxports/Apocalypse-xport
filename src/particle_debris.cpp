#include "game_sdk_bindings.h"
#include "particle_settings.h"
#include "fading_particle.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "game_random.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>

#include "particle_debris.h"
#include "native_object_storage.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

// Native input bridge for the original three-word pointer loads
static uint32 ReadDebrisInput(const uint32 *input, uint32 axis)
{
    uint32 value, address = GuestMemoryAddress(input);
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

Debris::Debris(const uint32 *position, const uint32 *velocity, uint32 kind, uint32 floor_height, uint32 flags, uint32 lifetime, const void *model_data) : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 name = GuestMemoryAddress(model_data);
    if (!name)
    {
        // TODO Bind native model data to the existing resource contract
        GameDiagnostic("TODO Debris native model data\n");
        abort();
    }
    FUNCTION_MARKER(0x8005BBB0, "SLUS_003.73");
    uint32 values[3], object_flags, x_rotation, y_rotation, z_rotation, axis;
    w_u32(object + 68, 0x800A27D8);
    SetModelBank((const char *)model_data);
    w_u16(object + 22, kind);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    object_flags = r_u16(object + 78);
    w_u16(object + 212, 0);
    w_u16(object + 78, object_flags & 0xFFEF);
    for (axis = 0; axis < 3; ++axis)
        values[axis] = ReadDebrisInput(position, axis);
    w_u32(object + 4, values[0]);
    w_u32(object + 8, values[1]);
    w_u32(object + 12, values[2]);
    for (axis = 0; axis < 3; ++axis)
        values[axis] = ReadDebrisInput(velocity, axis);
    w_u32(object + 104, values[0]);
    w_u32(object + 108, values[1]);
    w_u32(object + 112, values[2]);
    if (!(flags & 32))
    {
        x_rotation = RandomRange(256) - 128;
        y_rotation = RandomRange(256) - 128;
        z_rotation = RandomRange(256) - 128;
        w_u16(object + 132, x_rotation);
        w_u16(object + 134, y_rotation);
        w_u16(object + 136, z_rotation);
    }
    w_u16(object + 298, lifetime);
    w_u16(object + 300, flags);
    w_u32(object + 304, floor_height);
    w_u8(object + 296, 3);
}

// Native extraction of the original adjusted finish callback
uint32 Debris::FinishFromTable()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    return xport_draft_guest_call1(target, receiver);
}

// Native extraction of the original6CCE0/6C774 scale operations
void Debris::Shrink()
{
    const uint32 object = NativeObjectGuestAddress(this);
    sint16 delta[3];
    uint32 axis, value;
    w_u16(object, r_u16(object) | 512);
    for (axis = 0; axis < 3; ++axis)
        delta[axis] = (sint16)((sint32)(sint16)r_u16(object + 36 + axis * 2) >> 4);
    for (axis = 0; axis < 3; ++axis)
    {
        value = r_u16(object + 36 + axis * 2);
        w_u16(object + 36 + axis * 2, value - (uint32)(sint32)delta[axis]);
    }
}

uint32 Debris::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, flags, previous[3], value, limit, y, bounce, index;
    uint32 red, green, blue, player, table, receiver, target, damage;
    sint16 angles[4];
    sint32 distance;
    FUNCTION_MARKER(0x8005BD48, "SLUS_003.73");
    timer = r_u16(object + 298);
    if (!timer)
        return FinishFromTable();
    flags = r_u16(object + 300);
    w_u16(object + 298, timer - 1);
    if (flags & 256)
    {
        CalculateDirectionAngles((uint16 *)angles, (const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 308, 12));
        BuildDirectionVector((uint32 *)psx_addr(object + 116, 12), 128, (const uint16 *)angles);
        AddVectors((uint32 *)psx_addr(object + 104, 12), (const uint32 *)psx_addr(object + 116, 12));
        DampVector((uint32 *)psx_addr(object + 104, 12), (const uint8 *)psx_addr(object + 129, 3));
        SnapSmallVector((uint32 *)psx_addr(object + 104, 12));
    }
    previous[0] = r_u32(object + 4);
    previous[1] = r_u32(object + 8);
    previous[2] = r_u32(object + 12);
    AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
    if (r_u16(object + 300) & 1)
    {
        y = r_u32(object + 8);
        limit = r_u32(object + 304);
        if ((sint32)limit < (sint32)y)
        {
            bounce = r_u8(object + 296) - 1;
            w_u8(object + 296, bounce);
            if (!(bounce & 255))
                return FinishFromTable();
            value = r_u32(object + 108);
            limit = r_u32(object + 304);
            w_u32(object + 108, 0 - value);
            w_u32(object + 8, limit);
        }
    }
    if (!(r_u16(object + 300) & 16))
        w_u32(object + 108, r_u32(object + 108) + 0x4000);
    AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 132, 6));
    if (r_u16(object + 300) & 128)
        Shrink();
    if (!r_u32(0x800FF738) && (r_u16(object + 300) & 14) && (sint32)r_u32(0x800FF3A8) < 200)
    {
        angles[0] = 512;
        angles[1] = 4096;
        angles[2] = 0;
        SetParticleSpread(angles);
        CalculateDirectionAngles((uint16 *)angles, previous, (const uint32 *)psx_addr(object + 4, 12));
        SetParticleDirection((const sint16 *)angles);
        player = r_u32(0x800FF904);
        distance = (sint32)MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(player + 4, 12));
        SetParticleSize(distance < 8193 ? (distance < 1025 ? 4 : 2) : 1);
        flags = r_u16(object + 300);
        if (flags & 2)
        {
            red = r_u8(0x800ECC94);
            green = r_u8(0x800ECC96);
            blue = r_u8(0x800ECC98);
            SetParticleColor(red, green, blue);
            red = (r_u16(0x800ECC94) >> 5) & 255;
            green = (r_u16(0x800ECC96) >> 5) & 255;
            blue = (r_u16(0x800ECC98) >> 5) & 255;
            SetParticleColorStep(red, green, blue);
        }
        else if (flags & 4)
        {
            SetParticleColor(128, 128, 128);
            SetParticleColorStep(6, 6, 6);
        }
        else if (flags & 8)
        {
            value = RandomRange(128);
            SetParticleColor(value & 255, 128, 0);
            SetParticleColorStep((uint32)((sint32)value >> 5) & 255, 6, 0);
        }
        w_u32(0x800FF3AC, 0);
        for (index = 0; index < 4; ++index)
        {
            value = AllocateEffectStorage(88);
            if (value)
            {
                void *dram_addr = AllocateNativeObjectStorage(value, sizeof(FadingParticle));
                new (dram_addr) FadingParticle((const uint32 *)psx_addr(object + 4, 12), 16, 8192, 16);
            }
        }
        w_u32(0x800FF3AC, 1);
    }
    value = r_u16(object + 300) & 64;
    if (value)
    {
        player = r_u32(0x800FF5A0);
        if (player)
        {
            value = (sint32)MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(player + 4, 12)) < 128;
            if (value)
            {
                player = r_u32(0x800FF5A0);
                table = r_u32(player + 68);
                damage = r_u8(object + 297);
                receiver = player + (uint32)(sint32)(sint16)r_u16(table + 48);
                target = r_u32(table + 52);
                apocalypse_object_virtual52(target, receiver, damage, 0x800A71CC, 0);
                return FinishFromTable();
            }
        }
    }
    return value;
}

uint32 Debris::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8005BCE4, "SLUS_003.73");
    w_u32(object + 68, 0x800A27D8);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

Debris &Debris::Attach(uint32 object)
{
    return *static_cast<Debris *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8005BBB0(uint32 object, uint32 position, uint32 velocity, uint32 kind, uint32 floor_height, uint32 flags, uint32 lifetime, uint32 name)
{
    FUNCTION_MARKER(0x8005BBB0, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Debris));
    new (dram_addr) Debris((const uint32 *)psx_addr(position, 12), (const uint32 *)psx_addr(velocity, 12), kind, floor_height, flags, lifetime, psx_addr(name, 1));
    return object;
}

extern "C" uint32 apocalypse_debris_construct_5BBB0(uint32 object, uint32 position, const void *velocity, uint32 kind, uint32 floor_height, uint32 flags, uint32 lifetime, uint32 name)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Debris));
    new (dram_addr) Debris((const uint32 *)psx_addr(position, 12), (const uint32 *)velocity, kind, floor_height, flags, lifetime, psx_addr(name, 1));
    return object;
}

extern "C" uint32 apocalypse_debris_native(uint32 object, const uint32 position[3], uint32 velocity, uint32 kind, uint32 floor_height, uint32 flags, uint32 lifetime, uint32 name)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Debris));
    new (dram_addr) Debris(position, (const uint32 *)psx_addr(velocity, 12), kind, floor_height, flags, lifetime, psx_addr(name, 1));
    return object;
}

extern "C" uint32 sub_8005BD48(uint32 object)
{
    FUNCTION_MARKER(0x8005BD48, "SLUS_003.73");
    return Debris::Attach(object).Debris::Update();
}

extern "C" uint32 sub_8005BCE4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005BCE4, "SLUS_003.73");
    return Debris::Attach(object).Debris::Destroy(reason);
}
