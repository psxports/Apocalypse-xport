extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "machine_gun.h"
#include "projectile_line.h"
#include "ring_burst_effect.h"
#include "bouncing_point_effect.h"
#include "smoke_emitter.h"
#include "audio_voice.h"
#include "angle_math.h"
#include "audio_session.h"
#include "object_damage.h"
#include "object_collision_events.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

MachineGun::MachineGun(uint32 list)
    : WeaponNode(list, 0)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80022B8C, "SLUS_003.73");
    w_u32(object, 0x800A1548);
    w_u32(object + 4, WPN_MACHINE_GUN);
    Configure(10, 32, 10, 8000, 1, 1, 11, 256);
}

uint32 MachineGun::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80022C04, "SLUS_003.73");
    w_u32(object, 0x800A1548);
    uint32 result = WeaponNode::Destroy(0);
    return (reason & 1) ? ReleaseWeaponStorage(object) : result;
}

// TODO Recover semantic names for the original configuration fields
uint32 MachineGun::Configure(uint32 field60, uint32 field64, uint32 field70, uint32 field72, uint32 field68, uint32 field69, uint32 field76, uint32 field80)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80022C58, "SLUS_003.73");
    w_u32(object + 60, field60);
    w_u32(object + 64, field64);
    w_u8(object + 70, field70);
    w_u32(object + 72, field72);
    w_u8(object + 68, field68);
    w_u8(object + 69, field69);
    w_u32(object + 76, field76);
    w_u32(object + 80, field80);
    return field72;
}

// Native extractions retain original scalar, vector and callback operations
static sint32 DivideMachineGunFire(uint32 n, uint32 d)
{
    if (!d)
        return (sint32)n < 0 ? 1 : -1;
    if (n == 0x80000000 && d == 0xFFFFFFFF)
        return (sint32)n;
    return (sint32)n / (sint32)d;
}

static void DamageMachineGunTarget(uint32 object, uint32 weapon, const sint32 vector[3], uint32 reason)
{
    uint32 table = r_u32(object + 68);
    uint32 damage = r_u32(weapon + 40);
    sint32 adjustment = (sint16)r_u16(table + 48);
    uint32 target = r_u32(table + 52);
    apocalypse_object_virtual52_native(target, object + (uint32)adjustment, damage, vector, reason);
}

static void CreateMachineGunTrace(uint32 object, const sint32 start[3], const sint32 end[3])
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ProjectileTracer));
    new (dram_addr) ProjectileTracer((const uint32 *)start, (const uint32 *)end);
}

static void CreateMachineGunImpact(uint32 effect, const sint32 position[3])
{
    apocalypse_projectile_impact_2289C(effect, position);
}

/* TODO Selected host-buffer callees and unknown class callbacks retain explicit boundaries */

uint32 MachineGun::Fire(const uint32 *input_position, const sint16 *input_rotation)
{
    const sint32 *position = (const sint32 *)input_position;
    const uint32 weapon = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    const uint32 guest_rotation = GuestMemoryAddress(input_rotation);
    FUNCTION_MARKER(0x80022C90, "SLUS_003.73");
    sint32 v5;
    sint32 v6;
    uint32 weapon_position;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    short v12;
    sint32 geometry_hit;
    sint32 object_hit;
    sint32 hit_object;
    sint32 v16;
    uint32 v17;
    sint32 v18;
    sint32 impact_kind;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 result;
    sint32 v28;

    union
    {
        uint32 words[2];
        sint16 halves[4];
    } rotation;

    sint32 endpoint[3];
    uint32 hit_direction[4];
    int collision_position[4];
    int near_ray_start[4];
    int ricochet_delta[3];
    uint32 ricochet_end[4];
    uint32 bounce_step[4];
    uint32 scaled_delta[4];
    sint32 bounce_distance;
    sint32 delta_length;
    uint32 position_x;
    if (guest_position)
    {
        position_x = r_u32(guest_position);
        v5 = r_u32(guest_position + 4);
        v6 = r_u32(guest_position + 8);
    }
    else
    {
        memcpy(&position_x, position, 4);
        memcpy(&v5, position + 1, 4);
        memcpy(&v6, position + 2, 4);
    }
    w_u32(((uint32)((weapon + 8))), position_x);
    w_u32(((uint32)((weapon + 12))), v5);
    w_u32(((uint32)((weapon + 16))), v6);
    weapon_position = ((uint32)((weapon + 8)));
    if (!(r_u32(((uint32)((weapon + 20))))))
    {
        ClearChild();
        v8 = r_u32(((uint32)((weapon + 76))));
        w_u32(((uint32)((weapon + 28))), r_u32(0x800FF2F0));
        w_u32(((uint32)((weapon + 52))), PlaySoundEffect(v8, 8192, 0));
    }
    if (r_u32(((uint32)((weapon + 80)))))
    {
        v9 = AllocateEffectStorage(124);
        if (v9)
        {
            uint32 extent = (sint32)(20 * r_u32(weapon + 80)) / 256;
            uint32 odd_min = (sint32)(14 * r_u32(weapon + 80)) / 256;
            uint32 even_min = (sint32)(10 * r_u32(weapon + 80)) / 256;
            uint32 even_range = (sint32)(20 * r_u32(weapon + 80)) / 256;
            void *dram_addr = AllocateNativeObjectStorage((uint32)v9, sizeof(RingBurstEffect));
            new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(weapon_position, 12), 5, 255, 235, 0, 2, 90, 10, 0, 20, extent, 0, 0, odd_min, 0, even_min, even_range, 0, 1);
        }
    }
    if (r_u32(weapon + 20))
    {
        sint32 fire_interval = DivideMachineGunFire(30, r_u8(weapon + 70));
        uint32 last_fire = r_u32(weapon + 24);
        uint32 now = r_u32(0x800FF2F0);
        if ((sint32)(now - last_fire) < fire_interval)
        {
            w_u32(weapon + 20, 1);
            return 0;
        }
    }
    v10 = r_u8(((uint32)((weapon + 68))));
    w_u32(((uint32)((weapon + 84))), 1);
    if (v10)
    {
        w_u32(0x800FF3AC, 0);
        v11 = AllocateEffectStorage(116);
        if (v11)
        {
            void *dram_addr = AllocateNativeObjectStorage((uint32)v11, sizeof(BouncingPointEffect));
            new (dram_addr) BouncingPointEffect((const uint32 *)psx_addr(weapon + 8, 12), (const uint16 *)input_rotation);
        }
        w_u32(0x800FF3AC, 1);
    }
    if (guest_rotation)
    {
        rotation.words[0] = ReadAnglesWord(guest_rotation);
        v12 = (sint16)r_u16(guest_rotation + 4);
    }
    else
    {
        memcpy(&rotation.words[0], input_rotation, 4);
        memcpy(&v12, input_rotation + 2, 2);
    }
    rotation.halves[2] = v12;
    {
        uint32 range = r_u32(weapon + 60);
        uint32 random = RandomRange((range << 1) | 1);
        uint32 spread = r_u16(weapon + 60);
        uint32 angle = (uint16)rotation.halves[0];
        rotation.halves[0] = (sint16)(angle + random - spread);
        range = r_u32(weapon + 64);
        random = RandomRange((range << 1) | 1);
        spread = r_u16(weapon + 64);
        angle = (uint16)rotation.halves[1];
        rotation.halves[1] = (sint16)(angle + random - spread);
    }
    NormalizeAngles((uint16 *)rotation.halves);
    BuildDirectionVector((uint32 *)endpoint, r_u32(weapon + 72), (const uint16 *)rotation.halves);
    DivideVector(hit_direction, (const uint32 *)endpoint, (const uint32 *)psx_addr(weapon + 72, 4));
    AddVectors((uint32 *)endpoint, (const uint32 *)psx_addr(weapon + 8, 12));
    rotation.halves[2] = 0;
    rotation.words[0] = ((rotation.words[0] & 0xFFFF0000) | (((0) & 0xFFFF) << 0));
    BuildDirectionVector((uint32 *)near_ray_start, (uint32)-150, (const uint16 *)rotation.halves);
    AddVectors((uint32 *)near_ray_start, (const uint32 *)psx_addr(weapon + 8, 12));
    w_u32(0x800ED638, near_ray_start[0]);
    w_u32(0x800ED640, near_ray_start[2]);
    w_u32(0x800ED63C, near_ray_start[1]);
    geometry_hit = 0;
    w_u32(0x800ED644, r_u32(((uint32)((weapon + 8)))));
    object_hit = 0;
    w_u32(0x800ED648, r_u32(((uint32)((weapon + 12)))));
    w_u32(0x800ED64C, r_u32(((uint32)((weapon + 16)))));
    InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
    w_u32(0x800FF974, 1);
    QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
    w_u32(0x800FF974, 0);
    hit_object = 0;
    if (r_u32(0x800ED6A0))
        goto LABEL_13;
    w_u32(0x800ED638, r_u32(((uint32)((weapon + 8)))));
    w_u32(0x800ED63C, r_u32(((uint32)((weapon + 12)))));
    v16 = r_u32(((uint32)((weapon + 16))));
    w_u32(0x800ED644, endpoint[0]);
    w_u32(0x800ED648, endpoint[1]);
    w_u32(0x800ED64C, endpoint[2]);
    w_u32(0x800ED640, v16);
    InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
    w_u32(0x800FF974, 1);
    QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 1);
    v17 = r_u32(((uint32)((weapon + 44))));
    w_u32(0x800FF974, 0);
    v18 = IntersectObjectSpheres((const uint32 *)psx_addr(weapon + 8, 12), (const uint32 *)endpoint, (uint32 *)collision_position, r_u32(v17), 0);
    hit_object = v18;
    if (!r_u32(0x800ED6A0))
    {
        if (!v18)
            goto LABEL_20;
        goto LABEL_19;
    }
    if (v18)
    {
        {
            uint32 object_distance = MeasureEuclideanDistance((const uint32 *)psx_addr((uint32)v18 + 4, 12), (const uint32 *)psx_addr(weapon + 8, 12));
            uint32 geometry_distance = r_u32(0x800ED678);
            if (object_distance >= geometry_distance)
            {
                geometry_hit = 1;
                goto LABEL_20;
            }
        }
    LABEL_19:
        object_hit = 1;

        goto LABEL_20;
    }
LABEL_13:
    geometry_hit = 1;

LABEL_20:
    impact_kind = 0;

    if (!geometry_hit)
        goto LABEL_37;
    {
        uint32 level_object = r_u32(0x800ED6A0);
        uint32 polygon = r_u32(0x800ED6B8);
        sint32 damage = (sint32)r_u32(weapon + 40);
        DamageLevelObject(level_object, polygon, damage);
    }
    {
        uint32 geometry_object = r_u32(0x800ED6A0);
        if (r_u16(geometry_object) & 16)
            DamageMachineGunTarget(geometry_object, weapon, (const sint32 *)hit_direction, 28);
    }
    impact_kind = 12;
    endpoint[0] = r_u32(0x800ED6A4);
    endpoint[1] = r_u32(0x800ED6A8);
    endpoint[2] = r_u32(0x800ED6AC);
    if (!RandomRange(4))
    {
        SubtractVectors((uint32 *)ricochet_delta, (const uint32 *)endpoint, (const uint32 *)psx_addr(weapon + 8, 12));
        sint32 normal_x = (sint16)r_u16(0x800ED6B0);
        sint32 normal_y = (sint16)r_u16(0x800ED6B2);
        sint32 normal_z = (sint16)r_u16(0x800ED6B4);
        v20 = (sint32)((uint32)(ricochet_delta[0] >> 6) * (uint32)normal_x + (uint32)(ricochet_delta[1] >> 6) * (uint32)normal_y + (uint32)(ricochet_delta[2] >> 6) * (uint32)normal_z);
        v21 = (v20 > 0);
        v22 = (v20 >> 12);
        if (!v21)
        {
            ricochet_delta[0] = (sint32)((uint32)ricochet_delta[0] - (uint32)((sint32)((uint32)v22 * (uint32)normal_x) >> 6));
            ricochet_delta[2] = (sint32)((uint32)ricochet_delta[2] - (uint32)((sint32)((uint32)v22 * (uint32)normal_z) >> 6));
            delta_length = MeasureVectorLength((const uint32 *)ricochet_delta);
            if ((delta_length >= 201))
            {
                bounce_distance = 400;
                MultiplyVector(scaled_delta, (const uint32 *)ricochet_delta, (const uint32 *)&bounce_distance);
                DivideVector(bounce_step, scaled_delta, (const uint32 *)&delta_length);
                AddVectors(ricochet_end, (const uint32 *)endpoint, bounce_step);
                v23 = AllocateEffectStorage(108);
                if (v23)
                    CreateMachineGunTrace(v23, endpoint, (const sint32 *)ricochet_end);
            }
        }
    }
    if (RandomRange(4))
        goto LABEL_37;
    v24 = RandomRange(3);
    if ((v24 == 1))
    {
        v25 = 28;
    }
    else if ((v24 >= 2))
    {
        v25 = 29;
        if ((v24 != 2))
            goto LABEL_37;
    }
    else
    {
        v25 = 27;
        if (v24)
            goto LABEL_37;
    }
    PlayPositionedSound(v25, (const uint32 *)endpoint, 0);
LABEL_37:
    if (object_hit)
    {
        impact_kind = 4;
        DamageMachineGunTarget((uint32)hit_object, weapon, (const sint32 *)hit_direction, 28);
        if (RandomRange(2))
            impact_kind = 20;
        endpoint[0] = collision_position[0];
        endpoint[1] = collision_position[1];
        endpoint[2] = collision_position[2];
    }

    if (impact_kind)
        CreateMachineGunImpact(impact_kind, endpoint);
    v26 = AllocateEffectStorage(108);
    if (v26)
        CreateMachineGunTrace(v26, (const sint32 *)psx_addr(weapon + 8, 12), endpoint);
    result = 1;
    v28 = r_u32(0x800FF2F0);
    w_u32(((uint32)((weapon + 20))), 1);
    w_u32(((uint32)((weapon + 24))), v28);
    return result;
}

static void StoreMachineGunLight(uint8 *output, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u8(address, value);
    else
        *output = (uint8)value;
}


uint32 MachineGun::Update(const sint32 *position, const sint16 *)
{
    const uint32 weapon = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80023380, "SLUS_003.73");
    uint32 child = r_u32(weapon + 32), result, sound;
    if (child)
    {
        static_cast<EffectObject *>(FindNativeObjectStorage(child))->SetPosition((const uint32 *)position);
        uint32 now = r_u32(0x800FF2F0);
        uint32 created_at = r_u32(weapon + 36);
        if (now - created_at >= 31)
            ClearChild();
    }
    result = r_u32(weapon + 20);
    if (result)
    {
        sound = r_u32(weapon + 52);
        w_u32(weapon + 20, 0);
        if (sound)
            StopSoundVoices(sound);
        uint32 now = r_u32(0x800FF2F0);
        uint32 started_at = r_u32(weapon + 28);
        result = now - started_at < 46;
        if (!result)
        {
            child = AllocateEffectStorage(96);
            if (child)
            {
                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(SmokeEmitter));
                new (dram_addr) SmokeEmitter((const uint32 *)position, (uint32)-1, 1, 100, 100, 100, 3, 8, 188, 188);
            }
            w_u32(weapon + 32, child);
            w_u8(child + 66, 1);
            result = r_u32(0x800FF2F0);
            w_u32(weapon + 36, result);
        }
    }
    return result;
}

uint32 MachineGun::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002349C, "SLUS_003.73");
    if (!r_u32(object + 84))
        return 0;
    StoreMachineGunLight(red, 255);
    StoreMachineGunLight(green, 0);
    StoreMachineGunLight(blue, 0);
    w_u32(object + 84, 0);
    return 1;
}







MachineGun &MachineGun::Attach(uint32 object)
{
    return *static_cast<MachineGun *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80022B8C(uint32 object, uint32 list)
{
    FUNCTION_MARKER(0x80022B8C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(MachineGun));
    new (dram_addr) MachineGun(list);
    return object;
}

extern "C" uint32 sub_80022C04(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80022C04, "SLUS_003.73");
    return MachineGun::Attach(object).MachineGun::Destroy(reason);
}

extern "C" uint32 sub_80022C58(uint32 object, uint32 field60, uint32 field64, uint32 field70, uint32 field72, uint32 field68, uint32 field69, uint32 field76, uint32 field80)
{
    FUNCTION_MARKER(0x80022C58, "SLUS_003.73");
    return MachineGun::Attach(object).Configure(field60, field64, field70, field72, field68, field69, field76, field80);
}

extern "C" uint32 apocalypse_weapon_tick_23380(uint32 weapon, const sint32 position[3])
{
    return MachineGun::Attach(weapon).MachineGun::Update(position, 0);
}

extern "C" uint32 sub_80023380(uint32 weapon, uint32 position)
{
    FUNCTION_MARKER(0x80023380, "SLUS_003.73");
    return MachineGun::Attach(weapon).MachineGun::Update((const sint32 *)psx_addr(position, 12), 0);
}

extern "C" uint32 sub_80022C90(uint32 object, uint32 position, uint32 rotation)
{
    FUNCTION_MARKER(0x80022C90, "SLUS_003.73");
    return MachineGun::Attach(object).MachineGun::Fire((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(rotation, 6));
}

extern "C" uint32 apocalypse_weapon_fire_22C90(uint32 object, const sint32 position[3], const sint16 rotation[3])
{
    return MachineGun::Attach(object).MachineGun::Fire((const uint32 *)position, rotation);
}
