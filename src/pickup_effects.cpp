extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "pickup_effects.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include "ribbon_segments.h"
#include "beam_segments.h"
#include "ring_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "game_random.h"

PickupEffect &PickupEffect::Attach(uint32 object)
{
    return *static_cast<PickupEffect *>(FindNativeObjectStorage(object));
}

uint32 PickupEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80032014, "SLUS_003.73");
    w_u32(object + 68, 0x800A1AA8);
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

extern "C" uint32 sub_80032014(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80032014, "SLUS_003.73");
    return PickupEffect::Attach(object).PickupEffect::Destroy(reason);
}

uint32 FlamethrowerPickupEffect::SetPosition(const uint32 *position)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, buffer, value, index, offset;
    uint32 step[4], current[4], scaled[4], multiplier = 2;
    uint16 angles[4];
    static const uint32 size_offsets[8] = {16, 18, 52, 54, 124, 126, 160, 162};
    static const uint32 sizes[8] = {10, 30, 10, 35, 10, 35, 10, 30};
    static const uint32 color_offsets[12] = {12, 13, 14, 20, 21, 22, 156, 157, 158, 164, 165, 166};
    FUNCTION_MARKER(0x80031588, "SLUS_003.73");
    value = r_u16(r_u32(0x800FF904) + 18);
    angles[0] = 0;
    angles[2] = 0;
    angles[1] = (uint16)(value + 1024);
    BuildDirectionVector(step, 70, angles);
    if (!r_u32(object + 72))
    {
        child = AllocateEffectStorage(108);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RibbonSegments));
            new (dram_addr) RibbonSegments(5, 1);
        }
        w_u32(object + 72, child);
        w_u8(child + 66, 1);
        RibbonSegments::Attach(r_u32(object + 72)).SetMaterial(0x68B7B136);
        RibbonSegments::Attach(r_u32(object + 72)).SetColors(255, 255, 255, 128, 128, 128);
        RibbonSegments::Attach(r_u32(object + 72)).SetSize(10, 40);
        for (index = 0; index < 8; ++index)
        {
            buffer = r_u32(r_u32(object + 72) + 104);
            w_u16(buffer + size_offsets[index], sizes[index]);
        }
        for (index = 0; index < 12; ++index)
        {
            buffer = r_u32(r_u32(object + 72) + 104);
            w_u8(buffer + color_offsets[index], 0);
        }
    }
    MultiplyVector(scaled, step, &multiplier);
    SubtractVectors(current, position, scaled);
    for (index = 0, offset = 0; index < 5; ++index, offset += 36)
    {
        buffer = r_u32(r_u32(object + 72) + 104) + offset;
        w_u32(buffer, current[0]);
        w_u32(buffer + 4, current[1]);
        w_u32(buffer + 8, current[2]);
        AddVectors(current, step);
    }
    child = r_u32(object + 72);
    value = r_u32(child + 88) + 4;
    w_u32(child + 88, value);
    return value;
}

uint32 RipLaserPickupEffect::SetPosition(const uint32 *position)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, buffer, value, index, offset;
    uint32 step[4], current[4], scaled[4], multiplier = 1, divisor = 2;
    uint16 angles[4];
    FUNCTION_MARKER(0x80031C64, "SLUS_003.73");
    if (!r_u32(object + 72))
    {
        child = AllocateEffectStorage(84);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
            new (dram_addr) BeamSegments(4, 1);
        }
        w_u32(object + 72, child);
        w_u8(child + 66, 1);
        BeamSegments::Attach(r_u32(object + 72)).SetColors(64, 32, 128);
        BeamSegments::Attach(r_u32(object + 72)).SetSize(25);
    }
    value = r_u16(r_u32(0x800FF904) + 18);
    angles[0] = 0;
    angles[2] = 0;
    angles[1] = (uint16)(value + 1024);
    BuildDirectionVector(step, 56, angles);
    MultiplyVector(scaled, step, &multiplier);
    SubtractVectors(current, position, scaled);
    DivideVector(scaled, step, &divisor);
    SubtractVectors(current, scaled);
    for (index = 0, offset = 0; index < 4; ++index, offset += 36)
    {
        buffer = r_u32(r_u32(object + 72) + 80) + offset;
        w_u32(buffer, current[0]);
        w_u32(buffer + 4, current[1]);
        w_u32(buffer + 8, current[2]);
        value = RandomRange(61);
        buffer = r_u32(r_u32(object + 72) + 80) + offset;
        w_u32(buffer + 4, r_u32(buffer + 4) + ((value - 30) << 12));
        AddVectors(current, step);
    }
    return 0;
}

uint32 PulseLaserPickupEffect::SetPosition(const uint32 *position)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, value;
    FUNCTION_MARKER(0x80031ECC, "SLUS_003.73");
    child = r_u32(object + 72);
    if (child)
        RingEffect::Attach(child).SetPosition(position);
    else
    {
        child = AllocateEffectStorage(104);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
            new (dram_addr) RingEffect(4, 2);
        }
        w_u32(object + 72, child);
        RingEffect::Attach(child).SetCenterColor(255, 255, 255);
        RingEffect::Attach(r_u32(object + 72)).SetRingColors(0, 130, 0);
        RingEffect::Attach(r_u32(object + 72)).SetRowColors(0, 0, 40, 0);
        RingEffect::Attach(r_u32(object + 72)).SetRowColors(1, 0, 0, 0);
        RingEffect::Attach(r_u32(object + 72)).SetOddRadii(21);
        RingEffect::Attach(r_u32(object + 72)).SetEvenRadii(12);
        RingEffect::Attach(r_u32(object + 72)).SetRowExtent(0, 8);
        RingEffect::Attach(r_u32(object + 72)).SetRowExtent(1, 8);
        RingEffect::Attach(r_u32(object + 72)).SetPosition(position);
        value = RandomRange(1024);
        child = r_u32(object + 72);
        w_u16(child + 96, value);
        w_u8(r_u32(object + 72) + 66, 1);
    }
    child = r_u32(object + 72);
    value = r_u16(child + 96) + 150;
    w_u16(child + 96, value);
    return value;
}

extern "C" uint32 sub_80031588(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80031588, "SLUS_003.73");
    return FlamethrowerPickupEffect::Attach(object).FlamethrowerPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
}

extern "C" uint32 sub_80031C64(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80031C64, "SLUS_003.73");
    return RipLaserPickupEffect::Attach(object).RipLaserPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
}

extern "C" uint32 sub_80031ECC(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80031ECC, "SLUS_003.73");
    return PulseLaserPickupEffect::Attach(object).PulseLaserPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
}

FlamethrowerPickupEffect::FlamethrowerPickupEffect() : PickupEffect()
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800314CC, "SLUS_003.73");
    w_u32(object + 68, 0x800A1A88);
}

RipLaserPickupEffect::RipLaserPickupEffect() : PickupEffect()
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80031BA8, "SLUS_003.73");
    w_u32(object + 68, 0x800A1A48);
}

PulseLaserPickupEffect::PulseLaserPickupEffect() : PickupEffect()
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80031E10, "SLUS_003.73");
    w_u32(object + 68, 0x800A1A28);
}

extern "C" uint32 sub_800314CC(uint32 object)
{
    FUNCTION_MARKER(0x800314CC, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FlamethrowerPickupEffect));
    new (dram_addr) FlamethrowerPickupEffect();
    return object;
}
extern "C" uint32 sub_80031BA8(uint32 object)
{
    FUNCTION_MARKER(0x80031BA8, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RipLaserPickupEffect));
    new (dram_addr) RipLaserPickupEffect();
    return object;
}
extern "C" uint32 sub_80031E10(uint32 object)
{
    FUNCTION_MARKER(0x80031E10, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PulseLaserPickupEffect));
    new (dram_addr) PulseLaserPickupEffect();
    return object;
}

FlamethrowerPickupEffect &FlamethrowerPickupEffect::Attach(uint32 object)
{
    return *static_cast<FlamethrowerPickupEffect *>(FindNativeObjectStorage(object));
}

uint32 FlamethrowerPickupEffect::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, receiver;
    FUNCTION_MARKER(0x80031500, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A1A88);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    PickupEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

extern "C" uint32 sub_80031500(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80031500, "SLUS_003.73");
    return FlamethrowerPickupEffect::Attach(object).FlamethrowerPickupEffect::Destroy(reason);
}

RipLaserPickupEffect &RipLaserPickupEffect::Attach(uint32 object)
{
    return *static_cast<RipLaserPickupEffect *>(FindNativeObjectStorage(object));
}

uint32 RipLaserPickupEffect::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, receiver;
    FUNCTION_MARKER(0x80031BDC, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A1A48);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    PickupEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

extern "C" uint32 sub_80031BDC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80031BDC, "SLUS_003.73");
    return RipLaserPickupEffect::Attach(object).RipLaserPickupEffect::Destroy(reason);
}

PulseLaserPickupEffect &PulseLaserPickupEffect::Attach(uint32 object)
{
    return *static_cast<PulseLaserPickupEffect *>(FindNativeObjectStorage(object));
}

uint32 PulseLaserPickupEffect::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, receiver;
    FUNCTION_MARKER(0x80031E44, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A1A28);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    PickupEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

extern "C" uint32 sub_80031E44(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80031E44, "SLUS_003.73");
    return PulseLaserPickupEffect::Attach(object).PulseLaserPickupEffect::Destroy(reason);
}

ParticleBeamPickupEffect::ParticleBeamPickupEffect() : PickupEffect()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003188C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1A68);
}

ParticleBeamPickupEffect &ParticleBeamPickupEffect::Attach(uint32 object)
{
    return *static_cast<ParticleBeamPickupEffect *>(FindNativeObjectStorage(object));
}

uint32 ParticleBeamPickupEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800318C0, "SLUS_003.73");
    uint32 child = r_u32(object + 72);
    w_u32(object + 68, 0x800A1A68);
    if (child)
    {
        uint32 table = r_u32(child + 68);
        uint32 adjustment = (uint32)(sint32)(sint16)r_u16(table + 8);
        uint32 target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + adjustment, 3);
        child = r_u32(object + 76);
        if (child)
        {
            table = r_u32(child + 68);
            adjustment = (uint32)(sint32)(sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            apocalypse_object_cleanup(target, child + adjustment, 3);
        }
    }
    w_u32(object + 68, 0x800A1AA8);
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

extern "C" uint32 sub_8003188C(uint32 object)
{
    FUNCTION_MARKER(0x8003188C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ParticleBeamPickupEffect));
    new (dram_addr) ParticleBeamPickupEffect();
    return object;
}

extern "C" uint32 sub_800318C0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800318C0, "SLUS_003.73");
    return ParticleBeamPickupEffect::Attach(object).ParticleBeamPickupEffect::Destroy(reason);
}

uint32 ParticleBeamPickupEffect::SetPosition(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80031974, "SLUS_003.73");
    uint32 child;
    if (!r_u32(object + 72))
    {
        child = AllocateEffectStorage(84);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
            new (dram_addr) BeamSegments(3, 1);
        }
        w_u32(object + 72, child);
        w_u8(child + 66, 1);
        BeamSegments::Attach(r_u32(object + 72)).SetColors(255, 128, 0);
        child = AllocateEffectStorage(84);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
            new (dram_addr) BeamSegments(3, 0);
        }
        w_u32(object + 76, child);
        w_u8(child + 66, 1);
        BeamSegments::Attach(r_u32(object + 76)).SetColors(185, 185, 185);
    }
    uint16 angles[4];
    uint32 yaw = r_u16(r_u32(0x800FF904) + 18);
    angles[0] = 0;
    angles[2] = 0;
    angles[1] = (uint16)(yaw + 1024);
    uint32 step[3], left[3], right[3];
    BuildDirectionVector(step, 100, angles);
    SubtractVectors(left, position, step);
    AddVectors(right, position, step);
    uint32 point;
    for (point = 0; point < 3; ++point)
    {
        uint32 second_child = r_u32(object + 76);
        uint32 first_child = r_u32(object + 72);
        uint32 second_buffer = r_u32(second_child + 80);
        uint32 first_buffer = r_u32(first_child + 80);
        uint32 x, y, z;
        if (point == 1)
        {
            uint32 address = GuestMemoryAddress(position);
            if (address)
            {
                x = r_u32(address);
                y = r_u32(address + 4);
                z = r_u32(address + 8);
            }
            else
            {
                x = position[0];
                y = position[1];
                z = position[2];
            }
        }
        else
        {
            const uint32 *source = point ? right : left;
            x = source[0];
            y = source[1];
            z = source[2];
        }
        uint32 offset = point * 36;
        w_u32(second_buffer + offset, x);
        w_u32(second_buffer + offset + 4, y);
        w_u32(second_buffer + offset + 8, z);
        x = r_u32(second_buffer + offset);
        y = r_u32(second_buffer + offset + 4);
        z = r_u32(second_buffer + offset + 8);
        w_u32(first_buffer + offset, x);
        w_u32(first_buffer + offset + 4, y);
        w_u32(first_buffer + offset + 8, z);
    }
    BeamSegments::Attach(r_u32(object + 72)).SetSize(15);
    uint32 phase = (1200 * r_u32(0x800FF2F0)) & 0x3FF0;
    sint32 sine = (sint16)r_u16(0x800F863C + phase);
    child = r_u32(object + 72);
    uint32 product = 5 * (uint32)sine;
    uint32 buffer = r_u32(child + 80);
    if ((sint32)product < 0)
        product += 4095;
    w_u16(buffer + 52, (uint32)((sint32)product >> 12) + 20);
    return BeamSegments::Attach(r_u32(object + 76)).SetSize(10);
}

extern "C" uint32 sub_80031974(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80031974, "SLUS_003.73");
    return ParticleBeamPickupEffect::Attach(object).ParticleBeamPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
}
