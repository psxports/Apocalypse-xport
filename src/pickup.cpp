#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "pickup.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "game_random.h"
#include "model_resources.h"
#include "enemy_list.h"
#include "record_data.h"
#include "effect_lifecycle.h"
#include "pickup_effects.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "player.h"
#include "weapon_nodes.h"
#include "hud_text.h"
#include "game_panel.h"
#include "audio_session.h"
#include "ring_effect.h"
#include "ring_burst_effect.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static uint32 ReadPickupWord(const uint32 *input, uint32 axis)
{
    uint32 address = GuestMemoryAddress(input), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

Pickup::Pickup(uint32 resource, const uint32 *position, const uint32 *velocity, uint32 flags, uint32 yaw) : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, value, bank, key = 0;
    FUNCTION_MARKER(0x8001FD28, "SLUS_003.73");
    w_u32(object + 68, 0x800A11AC);
    w_u16(object + 58, resource);
    w_u16(object + 336, yaw);
    value = r_u8(0x800EC530);
    w_u8(object + 306, 1);
    w_u8(object + 309, value);
    x = ReadPickupWord(position, 0);
    y = ReadPickupWord(position, 1);
    z = ReadPickupWord(position, 2);
    w_u32(object + 316, x);
    w_u32(object + 320, y);
    w_u32(object + 324, z);
    x = ReadPickupWord(position, 0);
    y = ReadPickupWord(position, 1);
    z = ReadPickupWord(position, 2);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    if (flags & 8)
        w_u32(object + 108, (uint32)-245760);
    else
    {
        x = ReadPickupWord(velocity, 0);
        y = ReadPickupWord(velocity, 1);
        z = ReadPickupWord(velocity, 2);
        w_u32(object + 104, x);
        w_u32(object + 108, y);
        w_u32(object + 112, z);
    }
    w_u8(object + 129, 5);
    w_u8(object + 130, 5);
    w_u8(object + 131, 5);
    w_u16(object + 134, 100);
    w_u32(object + 120, 32768);
    value = RandomRange(20);
    w_u16(object + 334, value + 90);
    w_u16(object + 330, 50);
    w_u16(object + 332, 50);
    if (flags & 16)
    {
        w_u8(object + 307, 1);
        w_u8(object + 308, 1);
    }
    else if (flags & 4)
    {
        w_u8(object + 307, 1);
        value = sub_80067A18(object + 316, 0, 8000);
        w_u32(object + 312, value);
    }
    w_u16(object + 212, 300);
    EnableShadow();
    w_u16(object + 204, 300);
    SetModelBank((const char *)psx_addr(0x800FF208, 1));
    resource = r_u16(object + 58);
    value = resource - 1;
    value = value < 20 ? r_u32(0x800A11EC + value * 4) : 0x80020004;
    switch (value)
    {
    case 0x8001FF38:
        w_u8(object + 305, 0);
        break;
    case 0x8001FEDC: key = 0x350E968C; goto model;
    case 0x8001FEC0: key = 0xD40B155E; goto model;
    case 0x8001FEF8: key = 0x51E46AAF; goto model;
    case 0x8001FFD0: key = 0x4D2C5D7C; goto model;
    case 0x8001FF40: resource = 14; goto health;
    case 0x8001FF70: resource = 15; goto health;
    case 0x8001FFA0: resource = 16;
    health:
        value = r_u16(0x800EC656 + (resource - 14) * 2);
        w_u8(object + 305, 1);
        bank = r_u8(0x800FF645);
        w_u16(object + 134, 100 + (resource - 14) * 50);
        w_u16(object + 218, value);
        key = 0x7E74F3D4;
        goto resolve;
    model:
        w_u8(object + 305, 1);
        bank = r_u8(0x800FF645);
    resolve:
        value = FindModelResourceIndex(key, bank);
        w_u16(object + 22, value);
        break;
    case 0x8001FF14:
        SetModelBank((const char *)psx_addr(0x800A119C, 1));
        w_u8(object + 305, 1);
        w_u8(object + 309, 150);
        break;
    case 0x8001FFF8:
        w_u8(object + 305, 1);
        w_u16(object + 22, 0);
        break;
    case 0x80020004:
        break;
    default:
        // TODO Recover an unsupported original pickup jump target
        GameDiagnostic("Unsupported pickup branch: %08X\n", value);
        abort();
    }
    AddToList((uint32 *)psx_addr(0x800FF204, 4));
    if (!r_u8(object + 305))
    {
        value = r_u16(object);
        w_u16(object, value | 1);
    }
}

uint32 Pickup::ReleaseEffects()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x80020168, "SLUS_003.73");
    result = RemoveShadow();
    child = r_u32(object + 340);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        result = apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    child = r_u32(object + 296);
    w_u32(object + 340, 0);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        result = apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    w_u32(object + 296, 0);
    return result;
}

uint32 Pickup::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x800201F8, "SLUS_003.73");
    w_u32(object + 68, 0x800A11AC);
    RemoveFromList((uint32 *)psx_addr(0x800FF204, 4));
    Pickup::ReleaseEffects();
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

Pickup &Pickup::Attach(uint32 object)
{
    return *static_cast<Pickup *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8001FD28(uint32 object, uint32 resource, uint32 position, uint32 velocity, uint32 flags, uint32 yaw)
{
    FUNCTION_MARKER(0x8001FD28, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Pickup));
    new (dram_addr) Pickup(resource, (const uint32 *)psx_addr(position, 12), flags & 8 ? 0 : (const uint32 *)psx_addr(velocity, 12), flags, yaw);
    return object;
}

extern "C" uint32 apocalypse_pickup_constructor_native(uint32 object, uint32 resource, const void *position, uint32 velocity, uint32 flags, uint32 yaw)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Pickup));
    new (dram_addr) Pickup(resource, (const uint32 *)position, flags & 8 ? 0 : (const uint32 *)psx_addr(velocity, 12), flags, yaw);
    return object;
}

extern "C" uint32 sub_800201F8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800201F8, "SLUS_003.73");
    return Pickup::Attach(object).Pickup::Destroy(reason);
}

extern "C" uint32 sub_80020168(uint32 object)
{
    FUNCTION_MARKER(0x80020168, "SLUS_003.73");
    return Pickup::Attach(object).Pickup::ReleaseEffects();
}

static void AttachPickupChildPosition(uint32 object, uint32 position, sint16 yaw)
{
    uint32 table = r_u32(object + 68);
    uint32 adjustment = (uint32)(sint32)(sint16)r_u16(table + 24);
    uint32 target = r_u32(table + 28);
    uint32 receiver = object + adjustment;
    switch (target)
    {
        case 0x8003200C:
            nullsub_29();
            return;
        case 0x80031974:
            ParticleBeamPickupEffect::Attach(receiver).ParticleBeamPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
            return;
        case 0x80031588:
            FlamethrowerPickupEffect::Attach(receiver).FlamethrowerPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
            return;
        case 0x80031C64:
            RipLaserPickupEffect::Attach(receiver).RipLaserPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
            return;
        case 0x80031ECC:
            PulseLaserPickupEffect::Attach(receiver).PulseLaserPickupEffect::SetPosition((const uint32 *)psx_addr(position, 12));
            return;
        default:
            break;
    }
    // TODO Recover remaining position callbacks and their yaw contract
    GameDiagnostic("Missing 209EC virtual24: target=%08X receiver=%08X position=%08X yaw=%d\n", target, receiver, position, yaw);
    abort();
}

uint32 Pickup::AdvanceEffects()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800209EC, "SLUS_003.73");
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    short v6;
    short v7;
    uint32 v8;
    sint32 v9;
    uint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    UpdateLifetime();
    if ((r_u32(((uint32)((object + 220)))) >= 0x1F41))
    {
        uint32 table = r_u32(object + 68);
        uint32 adjustment = (uint32)(sint32)(sint16)r_u16(table + 56);
        uint32 target = r_u32(table + 60);
        return xport_draft_guest_call1(target, object + adjustment);
    }
    UpdateMotion();
    if (!(r_u8(((uint32)((object + 305))))))
    {
        if (!(r_u32(((uint32)((object + 340))))))
            CreateChildEffect();
        AttachPickupChildPosition(r_u32(object + 340), object + 4, (sint16)r_u16(object + 18));
    }
    if (!(r_u32(((uint32)((object + 296))))))
    {
        if ((r_u16(((uint32)((object + 58)))) == 15))
        {
            v3 = AllocateEffectStorage(104);
            v4 = v3;
            if (!v3)
                goto LABEL_14;
            v5 = 3;
        }
        else
        {
            v3 = AllocateEffectStorage(104);
            v4 = v3;
            if (!v3)
                goto LABEL_14;
            v5 = 2;
        }
        {
            void *dram_addr = AllocateNativeObjectStorage((uint32)v4, sizeof(RingEffect));
            new (dram_addr) RingEffect(4, (uint32)v5);
            v3 = v4;
        }
    LABEL_14:
        w_u32(((uint32)((object + 296))), v3);

        w_u8(((uint32)((r_u32(((uint32)((object + 296)))) + 94))), 1);
        RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetCenterColor(0, 0, 0);
        RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRingColors(0, 0, 0);
        RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(1, 0, 0, 0);
        RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRadii(0);
        w_u8(((uint32)((r_u32(((uint32)((object + 296)))) + 66))), 1);
        switch (r_u16(((uint32)((object + 58)))))
        {
            case 1:

            case 0xE:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 64, 64, 64);
                v6 = 60;
                goto LABEL_32;

            case 2:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 160, 70, 0);
                v6 = 60;
                goto LABEL_32;

            case 3:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRadii(30);
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 0, 100, 0);
                v6 = 60;
                goto LABEL_32;

            case 4:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 16, 16, 64);
                w_u16(((uint32)((object + 300))), 60);
                v7 = 5;
                goto LABEL_33;

            case 5:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 128, 32, 0);
                w_u16(((uint32)((object + 300))), 50);
                v7 = 5;
                goto LABEL_33;

            case 6:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 100, 50, 0);
                w_u16(((uint32)((object + 300))), 30);
                v7 = 5;
                goto LABEL_33;

            case 7:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 90, 40, 0);
                v6 = 60;
                goto LABEL_32;

            case 0xA:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 255, 0, 0);
                v6 = 80;
                goto LABEL_32;

            case 0xF:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(2, 0, 0, 0);
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(1, 128, 0, 0);
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 64, 64, 64);
                v6 = 60;
                goto LABEL_32;

            case 0x10:
                v8 = 0;
                if (r_u32(((uint32)((r_u32(((uint32)((object + 296)))) + 80)))))
                {
                    v9 = 0;
                    do
                    {
                        v10 = v8;
                        if (v9)
                        {
                            v11 = 128;
                            v12 = r_u32(((uint32)((object + 296))));
                            v13 = 0;
                        }
                        else
                        {
                            v11 = 64;
                            v12 = r_u32(((uint32)((object + 296))));
                            v13 = 64;
                        }
                        ++v8;
                        RingEffect::Attach(v12).SetColumnColors(v10, v11, v13, v9 ? 0 : 64);
                        v9 = (v8 & 1);
                    } while ((v8 < r_u32(((uint32)((r_u32(((uint32)((object + 296)))) + 80))))));
                }
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRingColors(0, 0, 0);
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(1, 0, 0, 0);
                v6 = 60;
                goto LABEL_32;

            case 0x11:
                RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowColors(0, 64, 64, 64);
                v6 = 100;
            LABEL_32:
                w_u16(((uint32)((object + 300))), v6);

                v7 = 10;
            LABEL_33:
                w_u16(((uint32)((object + 302))), v7);

                break;

            default:
                break;
        }
    }
    if (r_u8(((uint32)((object + 306)))))
        w_u32(((uint32)((r_u32(((uint32)((object + 296)))) + 100))), -1);
    else
        w_u32(((uint32)((r_u32(((uint32)((object + 296)))) + 100))), 0);
    RingEffect::Attach(r_u32(object + 296)).SetPosition((const uint32 *)psx_addr(object + 4, 12));
    uint32 phase = (600 * r_u32(0x800FF2F0)) & 0x3FF8;
    uint32 amplitude = r_u16(object + 302);
    sint32 sine = (sint16)r_u16(0x800F863C + phase);
    uint32 product = amplitude * (uint32)sine;
    uint32 base = r_u16(object + 300);
    if ((sint32)product < 0)
        product += 4095;
    v14 = (sint32)(base + (uint32)((sint32)product >> 12));
    RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowExtent(0, v14);
    RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowExtent(1, v14);
    if ((r_u16(((uint32)((object + 58)))) == 15))
        RingEffect::Attach(r_u32(((uint32)((object + 296))))).SetRowExtent(2, v14);
    v15 = r_u32(((uint32)((object + 296))));
    result = (r_u16(((uint32)((v15 + 96)))) + 50);
    w_u16(((uint32)((v15 + 96))), result);
    return result;
}

extern "C" uint32 sub_800209EC(uint32 object)
{
    FUNCTION_MARKER(0x800209EC, "SLUS_003.73");
    return Pickup::Attach(object).AdvanceEffects();
}

uint32 Pickup::Finish()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80020264, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        if (r_u8(object + 304))
        {
            uint32 indices = GetRecordPayload(r_u16(object + 310));
            SpawnEnemyIndices((const uint16 *)psx_addr(indices, 2));
        }
        result = r_u16(object + 78) | 64;
        uint32 flags = r_u16(object) | 1;
        w_u16(object + 78, result);
        w_u16(object, flags);
    }
    return result;
}

extern "C" uint32 sub_80020264(uint32 object)
{
    FUNCTION_MARKER(0x80020264, "SLUS_003.73");
    return Pickup::Attach(object).Pickup::Finish();
}

uint32 Pickup::CreateChildEffect()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80020068, "SLUS_003.73");
    uint32 kind = r_u16(object + 58);
    uint32 child;
    w_u32(0x800FF3AC, 0);
    switch (kind)
    {
        case 1:
        case 2:
        case 3:
        case 7:
            child = AllocateEffectStorage(kind == 2 || kind == 3 ? 80 : 76);
            if (child)
            {
                if (kind == 2)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ParticleBeamPickupEffect));
                    new (dram_addr) ParticleBeamPickupEffect();
                }
                else if (kind == 1)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RipLaserPickupEffect));
                    new (dram_addr) RipLaserPickupEffect();
                }
                else if (kind == 3)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(PulseLaserPickupEffect));
                    new (dram_addr) PulseLaserPickupEffect();
                }
                else
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(FlamethrowerPickupEffect));
                    new (dram_addr) FlamethrowerPickupEffect();
                }
            }
            w_u32(object + 340, child);
            break;
        default:
            break;
    }
    child = r_u32(object + 340);
    if (child)
        w_u8(child + 66, 1);
    w_u32(0x800FF3AC, 1);
    return 1;
}

extern "C" uint32 sub_80020068(uint32 object)
{
    FUNCTION_MARKER(0x80020068, "SLUS_003.73");
    return Pickup::Attach(object).CreateChildEffect();
}

uint32 Pickup::UpdateMotion()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800202CC, "SLUS_003.73");
    uint32 x, y, z, phase, amplitude, product, velocity, gravity, shift;
    uint32 unrestricted, damped, offset, floor, result;
    sint32 height, ground;
    AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 132, 6));
    NormalizeAngles((uint16 *)psx_addr(object + 16, 6));
    if (!r_u8(object + 307))
    {
        x = r_u32(object + 316);
        y = r_u32(object + 320);
        z = r_u32(object + 324);
        w_u32(object + 4, x);
        w_u32(object + 8, y);
        w_u32(object + 12, z);
        phase = r_u16(object + 328) & 4095;
        amplitude = r_u16(object + 332);
        product = amplitude * (uint32)(sint32)(sint16)r_u16(0x800F863C + phase * 4);
        if ((sint32)product < 0)
            product += 4095;
        offset = (uint32)((sint32)product >> 12) << 12;
        y = r_u32(object + 8);
        phase = r_u16(object + 328);
        amplitude = r_u16(object + 334);
        w_u32(object + 8, y + offset);
        w_u16(object + 328, phase + amplitude);
        goto UpdateShadowHeight;
    }
    if (!r_u8(object + 308))
    {
        ground = (sint32)r_u32(object + 312);
        if (ground < 0 && ground >= -4)
        {
            if (r_u32(0x800FF2F0) & 1)
            {
                floor = sub_80067A18(object + 316, 0, 8000);
                if (floor == 0xFFFFFFFF)
                {
                    floor = r_u32(object + 312) - 1;
                    w_u32(object + 312, floor);
                    if ((sint32)floor < -4)
                        w_u8(object + 308, 1);
                }
                else
                    w_u32(object + 312, floor);
            }
            goto UpdateShadowHeight;
        }
    }
    AddVectors((uint32 *)psx_addr(object + 316, 12), (const uint32 *)psx_addr(object + 104, 12));
    velocity = r_u32(object + 108);
    gravity = r_u32(object + 120);
    shift = r_u8(object + 130) & 31;
    velocity += gravity;
    damped = velocity - (uint32)((sint32)velocity >> shift);
    unrestricted = r_u8(object + 308);
    w_u32(object + 108, velocity);
    w_u32(object + 108, damped);
    if (!unrestricted)
    {
        offset = r_u8(object + 309) << 12;
        floor = r_u32(object + 312);
        y = r_u32(object + 320);
        floor -= offset;
        if ((sint32)floor < (sint32)y && (sint32)damped > 0)
        {
            w_u32(object + 320, floor);
            w_u32(object + 104, 0);
            w_u32(object + 108, 0);
            w_u32(object + 112, 0);
            w_u8(object + 307, 0);
        }
    }
    x = r_u32(object + 316);
    y = r_u32(object + 320);
    z = r_u32(object + 324);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
UpdateShadowHeight:
    x = r_u32(object + 316);
    y = r_u32(object + 320);
    z = r_u32(object + 324);
    w_u32(object + 184, x);
    w_u32(object + 188, y);
    w_u32(object + 192, z);
    floor = r_u32(object + 312);
    y = r_u32(object + 8);
    result = (uint32)((sint32)(floor - y) >> 12);
    w_u16(object + 202, result);
    result <<= 16;
    height = (sint32)result >> 16;
    w_u32(object + 188, floor);
    if (height)
    {
        product = 60 * (uint32)height;
        if ((sint32)product < 0)
            product += 127;
        result = 140 - (uint32)((sint32)product >> 7);
        w_u16(object + 200, result);
    }
    else
        w_u16(object + 200, 0);
    return result;
}

extern "C" uint32 sub_800202CC(uint32 object)
{
    FUNCTION_MARKER(0x800202CC, "SLUS_003.73");
    return Pickup::Attach(object).UpdateMotion();
}

void Pickup::UpdateLifetime()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80020518, "SLUS_003.73");
    uint32 count = r_u16(object + 336);
    if (count == 65535)
        return;
    if (count)
        w_u16(object + 336, count - 1);
    count = r_u16(object + 336);
    if (count < 60)
    {
        uint32 amplitude = r_u16(object + 330);
        uint32 remaining = r_u16(object + 336);
        uint32 product = amplitude * remaining;
        w_u16(object + 332, (sint32)product / 60);
    }
    count = r_u16(object + 336);
    if (count - 31 < 29)
        w_u8(object + 306, (r_u32(0x800FF2F0) & 2) ? 1 : 0);
    if (r_u16(object + 336) < 31)
        w_u8(object + 306, (r_u32(0x800FF2F0) & 1) ? 1 : 0);
    if (r_u8(object + 305))
    {
        uint32 blinking = r_u8(object + 306);
        uint32 flags = r_u16(object);
        w_u16(object, blinking ? flags & 65534 : flags | 1);
    }
    if (!r_u16(object + 336))
    {
        if (r_u32(object + 220) < 8001)
            SpawnExplosionRing((const uint32 *)psx_addr(object + 4, 12), 20, 255, 255, 255, 4, 1, 20);
        uint32 table = r_u32(object + 68);
        uint32 adjustment = (uint32)(sint32)(sint16)r_u16(table + 16);
        uint32 target = r_u32(table + 20);
        xport_draft_guest_call1(target, object + adjustment);
    }
}

extern "C" uint32 sub_80020518(uint32 object)
{
    FUNCTION_MARKER(0x80020518, "SLUS_003.73");
    Pickup::Attach(object).UpdateLifetime();
    // The original caller does not consume the incidental V0 carrier
    return 0;
}

static void MissingCollectedPickupKind(uint32 kind)
{
    // TODO Mutated kind requires an uninitialized original MIPS value
    GameDiagnostic("TODO Pickup kind changed during health collection: %u\n", kind);
    abort();
}

uint32 Pickup::Collect(Player &player)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 player_object = NativeObjectGuestAddress(&player);
    FUNCTION_MARKER(0x800206C4, "SLUS_003.73");
    sint32 v2;
    sint32 result;
    sint32 v6;
    uint32 v9;
    uint32 v10;
    sint32 v25;
    sint32 v26;
    result = r_u16(((uint32)((object + 58))));
    v6 = 0;
    switch (r_u16(((uint32)((object + 58)))))
    {
        case 1:
            result = player.ReplaceWeapon(WPN_RIP_LASER);
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v9 = r_u32((0x800A5744 + (0) * 4));
                v10 = 0x800A5A30;
                goto LABEL_32;
            }
            break;

        case 2:
            result = player.ReplaceWeapon(WPN_PARTICLE_BEAM);
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v9 = r_u32((0x800A5740 + (0) * 4));
                v10 = 0x800A5A30;
                goto LABEL_32;
            }
            break;

        case 3:
            result = player.ReplaceWeapon(WPN_PULSE_LASER);
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v9 = r_u32((0x800A573C + (0) * 4));
                v10 = 0x800A5A84;
                goto LABEL_32;
            }
            break;

        case 4:
            result = player.ReplaceWeapon(WPN_ROCKETS);
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v9 = r_u32((0x800A574C + (0) * 4));
                v10 = 0x800A5A30;
                goto LABEL_32;
            }
            break;

        case 5:
            result = player.ReplaceWeapon(WPN_HOMING_MISSILES);
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v9 = r_u32((0x800A5748 + (0) * 4));
                v10 = 0x800A5A30;
                goto LABEL_32;
            }
            break;

        case 6:
            result = player.ReplaceWeapon(WPN_GRENADES);
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v9 = r_u32(0x800A5750);
                v10 = 0x800A5A30;
                goto LABEL_32;
            }
            break;

        case 7:
            result = player.ReplaceWeapon(WPN_FLAMETHROWER);
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v9 = r_u32((0x800A5738 + (0) * 4));
                v10 = 0x800A5AD8;
                goto LABEL_32;
            }
            break;

        case 0xA:
            v26 = r_u32(((uint32)((player_object + 532))));
            result = (sint32)((uint32)v26 + 1);
            if (((sint32)v26 >= 15))
            {
                v6 = 0;
            }
            else
            {
                w_u32(((uint32)((player_object + 532))), result);
                v6 = 1;
            }
            v2 = 26;
            if (v6)
            {
                ClearAnimatedHudText();
                v9 = r_u32((0x800A5730 + (0) * 4));
                goto LABEL_31;
            }
            break;

        case 0xE:

        case 0xF:

        case 0x10:
            result = player.RestoreHealth( ((sint16)(r_u16(((uint32)((object + 218)))))));
            v6 = result;
            v2 = 26;
            if (result)
            {
                ClearAnimatedHudText();
                v25 = r_u16(((uint32)((object + 58))));
                if ((v25 == 15))
                {
                    v9 = r_u32((0x800A5728 + (0) * 4));
                }
                else if (v25 >= 16)
                {
                    /* TODO Unexpected kind would retain an inherited assembly value */
                    if (v25 != 16)
                        MissingCollectedPickupKind(v25);
                    if ((v25 == 16))
                        v9 = r_u32((0x800A572C + (0) * 4));
                }
                else
                {
                    /* TODO Unexpected kind would retain an inherited assembly value */
                    if (v25 != 14)
                        MissingCollectedPickupKind(v25);
                    if ((v25 == 14))
                        v9 = r_u32((0x800A5724 + (0) * 4));
                }
            LABEL_31:
                v10 = 0x800A57A8;

            LABEL_32:
                result = AddAnimatedHudText(((sint32)(v9)), ((sint32)(v10)));

                v2 = 26;
            }
            break;

        case 0x11:
            result = (sint32)(r_u32(0x800FF874) + 1);
            w_u32(0x800FF874, (uint32)result);
            if (result >= 101)
                w_u32(0x800FF874, 100);
            v6 = 1;
            ResetPanelDisplayTimer();
            v2 = 26;
            ClearAnimatedHudText();
            result = AddAnimatedHudText(((sint32)(r_u32((0x800A5734 + (0) * 4)))), ((sint32)(0x800A59DC)));
            break;

        default:
            break;
    }

    if (v6)
    {
        uint32 table = r_u32(object + 68);
        uint32 adjustment = (uint32)(sint32)(sint16)r_u16(table + 16);
        uint32 target = r_u32(table + 20);
        apocalypse_object_virtual20(target, object + adjustment);
        return PlaySoundEffect(v2, 0x2000, 0);
    }
    return result;
}

extern "C" uint32 sub_800206C4(uint32 object, uint32 player)
{
    FUNCTION_MARKER(0x800206C4, "SLUS_003.73");
    return Pickup::Attach(object).Collect(Player::Attach(player));
}

void Pickup::SetMotionParameters(uint32 gravity, uint32 damping_shift)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002005C, "SLUS_003.73");
    w_u32(object + 120, gravity);
    w_u8(object + 130, damping_shift);
}

extern "C" void sub_8002005C(uint32 object, uint32 gravity, uint32 damping_shift)
{
    FUNCTION_MARKER(0x8002005C, "SLUS_003.73");
    Pickup::Attach(object).SetMotionParameters(gravity, damping_shift);
}
