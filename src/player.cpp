#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "player.h"
#include "game_random.h"
#include "cd_voice_event.h"
#include "vector_arithmetic.h"
#include "level_interactions.h"
#include "model_deformation.h"
#include "expanding_ring.h"
#include "effect_lifecycle.h"
#include "pickup.h"
#include <new>
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_pad_poll.h"

#include "screen_flash.h"
#include "model_animation.h"
#include "machine_gun.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "audio_session.h"

#include "camera.h"
#include "camera.h"
#include "level_interactions.h"
#include "traversal_rail.h"

// TODO Recover the gameplay name of state 4096

// TODO Recover the gameplay names of record movement states 400 and 401

// TODO Recover the original names of the directional movement states

// TODO Recover original gameplay names for action states 256 and 2048

#include "weapon_targeting.h"

#include "flamethrower.h"
#include "weapon_nodes.h"
#include "spiral_ray_weapon.h"
#include "ring_attack.h"
#include "angle_math.h"
#include "model_attachment.h"

// Native extraction of the original selected-weapon virtual dispatch

// Native bridge for the original paired unaligned rotation load

#include "object_collision_events.h"
#include "vector_distance.h"

uint32 Player::Abs32(sint32 value)
{
    return value < 0 ? 0 - (uint32)value : (uint32)value;
}

static uint32 DividePlayerInput(uint32 numerator, uint32 denominator)
{
    sint32 value = (sint32)numerator;
    if (!denominator)
        return value < 0 ? 1 : 0xFFFFFFFF;
    return (uint32)(value / (sint32)denominator);
}

Player &Player::Attach(uint32 object)
{
    return *static_cast<Player *>(FindNativeObjectStorage(object));
}

void Player::StopSelectedWeapon(uint32 selected)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 weapon, table, receiver, target;
    weapon = r_u32(object + selected * 4 + 612);
    if (!weapon)
        return;
    table = r_u32(weapon);
    receiver = weapon + (uint32)(sint32)(sint16)r_u16(table + 24);
    target = r_u32(table + 28);
    switch (target)
    {
        case 0x8002BCA0: return;
        case 0x80024FDC: Flamethrower::Attach(receiver).Flamethrower::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x800243DC: SpiralRayWeapon::Attach(receiver).SpiralRayWeapon::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x80023380: MachineGun::Attach(receiver).MachineGun::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x80022C90: MachineGun::Attach(receiver).MachineGun::Fire((const uint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x800251E8: HomingMissiles::Attach(receiver).HomingMissiles::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x800254E8: PulseLaser::Attach(receiver).PulseLaser::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x800284A4: ParticleBeam::Attach(receiver).ParticleBeam::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x80028EE4: RipLaser::Attach(receiver).RipLaser::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x8002A798: Grenades::Attach(receiver).Grenades::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        case 0x8002B640: Rockets::Attach(receiver).Rockets::Update((const sint32 *)psx_addr(object + 4, 12), (const sint16 *)psx_addr(0x800FF5E4, 6)); return;
        default:
            GameDiagnostic("TODO Player weapon stop target %08X\n", target);
            abort();
    }
}

static uint32 DispatchPlayerWeapon(uint32 target, uint32 receiver, const sint32 position[3], const sint16 rotation[3])
{
    if (target == 0x8002BCA0)
    {
        // The empty original callback preserves its target in v0
        return target;
    }
    if (target == 0x8002BCA8)
        return WeaponNode::Attach(receiver).WeaponNode::Fire((const uint32 *)position, rotation);
    if (target == 0x800243DC)
        return SpiralRayWeapon::Attach(receiver).SpiralRayWeapon::Update(position, rotation);
    if (target == 0x80023C14)
        return SpiralRayWeapon::Attach(receiver).SpiralRayWeapon::Fire((const uint32 *)position, rotation);
    if (target == 0x800251E8)
        return HomingMissiles::Attach(receiver).HomingMissiles::Update(position, rotation);
    if (target == 0x800254E8)
    {
        PulseLaser::Attach(receiver).PulseLaser::Update(position, rotation);
        return 0;
    }
    if (target == 0x8002A798)
        return Grenades::Attach(receiver).Grenades::Update(position, rotation);
    if (target == 0x80028EE4)
    {
        RipLaser::Attach(receiver).RipLaser::Update(position, rotation);
        return 0;
    }
    if (target == 0x800284A4)
    {
        ParticleBeam::Attach(receiver).ParticleBeam::Update(position, rotation);
        return 0;
    }
    if (target == 0x8002B640)
        return Rockets::Attach(receiver).Rockets::Update(position, rotation);
    if (target == 0x8002A65C)
        return Grenades::Attach(receiver).Grenades::Fire((const uint32 *)position, rotation);
    if (target == 0x800286F4)
        return RipLaser::Attach(receiver).RipLaser::Fire((const uint32 *)position, rotation);
    if (target == 0x80027978)
        return ParticleBeam::Attach(receiver).ParticleBeam::Fire((const uint32 *)position, rotation);
    if (target == 0x80025380)
        return PulseLaser::Attach(receiver).PulseLaser::Fire((const uint32 *)position, rotation);
    if (target == 0x8002510C)
        return HomingMissiles::Attach(receiver).HomingMissiles::Fire((const uint32 *)position, rotation);
    if (target == 0x8002B588)
        return Rockets::Attach(receiver).Rockets::Fire((const uint32 *)position, rotation);
    if (target == 0x80022C90)
        return MachineGun::Attach(receiver).MachineGun::Fire((const uint32 *)position, rotation);
    if (target == 0x80024598)
        return Flamethrower::Attach(receiver).Flamethrower::Fire((const uint32 *)position, rotation);
    if (target == 0x80024FDC)
    {
        Flamethrower::Attach(receiver).Flamethrower::Update(position, rotation);
        return 0;
    }
    if (target == 0x80023380)
    {
        /* The observed tick uses position and ignores the rotation carrier */
        return MachineGun::Attach(receiver).MachineGun::Update(position, rotation);
    }
    GameDiagnostic("TODO 8005D360 weapon target %08X receiver %08X position %p rotation %p\n", target, receiver, (const void *)position, (const void *)rotation);
    abort();
}

static uint32 CallPlayerWeaponSlot(uint32 weapon, uint32 slot, const sint32 position[3], const sint16 rotation[3])
{
    uint32 table, receiver, target;
    table = r_u32(weapon);
    receiver = weapon + (uint32)(sint32)(sint16)r_u16(table + slot);
    target = r_u32(table + slot + 4);
    return DispatchPlayerWeapon(target, receiver, position, rotation);
}

uint32 SavePlayerTransform()
{
    FUNCTION_MARKER(0x8005C580, "SLUS_003.73");
    uint32 player = r_u32(0x800FF5A0);
    uint32 x = r_u32(player + 4);
    uint32 y = r_u32(player + 8);
    uint32 z = r_u32(player + 12);
    w_u32(player + 392, x);
    w_u32(player + 396, y);
    w_u32(player + 400, z);
    uint32 angles = r_u32(player + 16);
    uint32 last_angle = r_u16(player + 20);
    // Preserve the high then low halves written by the unaligned SWL/SWR pair
    w_u16(player + 412, angles >> 16);
    w_u16(player + 410, angles);
    w_u16(player + 414, last_angle);
    return player;
}

Player::Player() : Character()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005C5C0, "SLUS_003.73");
    uint32 count;
    short flags;
    short maximum_health;
    short object_flags;
    sint8 damping_shift;
    short animation_state;
    sint32 color_buffer;
    uint32 angles_buffer;
    uint32 weapon;
    sint32 ammo;
    sint32 difficulty;
    w_u32((((uint32)(object)) + (17) * 4), 0x800A32A4);
    w_u32(0x800FF5AC, 0);
    w_u32(0x800FF5A4, 0);
    w_u32(0x800FF5A8, 0);
    w_u16((object + (74) * 2), 2);
    AddToList((uint32 *)psx_addr(0x800FF5A0, 4));
    count = r_u32(0x800FF59C);
    flags = ((sint16)(r_u16((object + (39) * 2))));
    w_u16((object + (106) * 2), 100);
    w_u32(0x800FF59C, (count + 1));
    w_u16((object + (39) * 2), (flags & 0xFFFD));
    maximum_health = r_u16(0x800EC526);
    object_flags = ((sint16)(r_u16(object)));
    w_u32((((uint32)(object)) + (111) * 4), 0x800EC0F8);
    w_u16(object, (object_flags | 0x80));
    w_u16((object + (109) * 2), maximum_health);
    SetModelBank((const char *)psx_addr(0x800FF5B0, 1));
    w_u16((object + (29) * 2), 50);
    damping_shift = r_u8(0x800EC6D8);
    w_u8((((uint32)(object)) + (129) * 1), 1);
    w_u8((((uint32)(object)) + (131) * 1), 1);
    w_u8((((uint32)(object)) + (130) * 1), damping_shift);
    w_u8((((uint32)(object)) + (144) * 1), 5);
    w_u8((((uint32)(object)) + (145) * 1), 1);
    w_u8((((uint32)(object)) + (146) * 1), 5);
    w_u8((((uint32)(object)) + (582) * 1), r_u8(0x800EC4B4));
    animation_state = r_u16(0x800EC4B8);
    w_u16((object + (106) * 2), 100);
    w_u32((((uint32)(object)) + (112) * 4), 1);
    w_u8((((uint32)(object)) + (314) * 1), 1);
    w_u16((object + (292) * 2), animation_state);
    w_u32((((uint32)(object)) + (79) * 4), AllocateHeapBlock(32, 0, 1));
    color_buffer = AllocateHeapBlock(16, 0, 1);
    angles_buffer = ((uint32)(r_u32((((uint32)(object)) + (79) * 4))));
    w_u32((((uint32)(object)) + (80) * 4), color_buffer);
    w_u16(angles_buffer, 0);
    w_u16(((uint32)((r_u32((((uint32)(object)) + (79) * 4)) + 2))), -4096);
    w_u16(((uint32)((r_u32((((uint32)(object)) + (79) * 4)) + 4))), 0);
    w_u8(r_u32((((uint32)(object)) + (80) * 4)), 0x80);
    w_u8(((uint32)((r_u32((((uint32)(object)) + (80) * 4)) + 1))), 0x80);
    w_u8(((uint32)((r_u32((((uint32)(object)) + (80) * 4)) + 2))), 0x80);
    InitializeAnimation();
    w_u32(0x800FF840, 50);
    w_u32(((uint32)((r_u32(0x800FF904) + 564))), 3);
    weapon = AllocateZeroedHeapBlock(88);
    if (weapon)
    {
        void *dram_addr = AllocateNativeObjectStorage(weapon, sizeof(MachineGun));
        new (dram_addr) MachineGun(0x800FF4E8);
    }
    ammo = 5;
    difficulty = (r_u32(0x800FF384) != 0);
    w_u32((((uint32)(object)) + (153) * 4), weapon);
    if (!difficulty)
        ammo = 7;
    w_u32((weapon + (10) * 4), ammo);
    w_u32((((uint32)(object)) + (161) * 4), 0);
    w_u32((((uint32)(object)) + (162) * 4), 1);
    w_u32((((uint32)(object)) + (133) * 4), 3);
    SetModelBoneTable((const uint8 *)psx_addr(object, 28), (const uint16 *)psx_addr(0x800A6ECC, 4));
}

uint32 Player::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer;
    FUNCTION_MARKER(0x8005C7C0, "SLUS_003.73");
    buffer = r_u32(object + 316);
    w_u32(object + 68, 0x800A32A4);
    if (buffer)
        ReleaseHeapBlock(buffer);
    buffer = r_u32(object + 320);
    if (buffer)
        ReleaseHeapBlock(buffer);
    RemoveFromList((uint32 *)psx_addr(0x800FF5A0, 4));
    w_u32(0x800FF59C, r_u32(0x800FF59C) - 1);
    Player::ClearWeapons();
    Character::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

void Player::StopWeapon()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 selected;
    FUNCTION_MARKER(0x8005C868, "SLUS_003.73");
    selected = r_u32(object + 644);
    StopSelectedWeapon(selected);
}

uint32 TryPlayPlayerKillQuote(const uint32 *position)
{
    FUNCTION_MARKER(0x8005C8C4, "SLUS_003.73");
    uint32 count, total, random, now, previous_time, level, variant_count;
    uint32 kind, variant, selected, history[5];
    count = r_u32(0x800FF5AC);
    total = r_u32(0x800FF30C);
    w_u32(0x800FF5AC, count + 1);
    w_u32(0x800FF30C, total + 1);
    random = RandomRange(2);
    count = r_u32(0x800FF5AC);
    if (!((sint32)(random + 3) < (sint32)count))
        return 0;
    (void)r_u32(0x800FF64C);
    now = r_u32(0x800FF64C);
    previous_time = r_u32(0x800FF2CC);
    if (now - previous_time < 900)
        return 1;
    w_u32(0x800FF5AC, 0);
    for (;;)
    {
        level = r_u32(0x800FF378);
        if (level == 1)
        {
            random = RandomRange(4);
            kind = random ? 3 : 2;
            variant_count = random ? 6 : 8;
        }
        else if (level == 2)
        {
            random = RandomRange(4);
            if (random)
            {
                kind = 4;
                variant_count = 4;
            }
            else
            {
                random = RandomRange(2);
                kind = random ? 3 : 2;
                variant_count = random ? 6 : 8;
            }
        }
        else if (level == 0)
        {
            kind = 2;
            variant_count = 8;
        }
        else
        {
            random = RandomRange(4);
            if (random)
            {
                kind = 5;
                variant_count = 7;
            }
            else
            {
                random = RandomRange(3);
                if (!random)
                {
                    kind = 4;
                    variant_count = 4;
                }
                else
                {
                    random = RandomRange(3);
                    kind = random ? 2 : 3;
                    variant_count = random ? 8 : 6;
                }
            }
        }
        w_u32(0x800FF5B8, kind);
        variant = RandomRange(variant_count);
        w_u32(0x800FF5BC, variant);
        kind = r_u32(0x800FF5B8);
        variant = r_u32(0x800FF5BC);
        history[0] = r_u32(0x800A71B8);
        selected = (kind << 3) + variant;
        if (history[0] == selected)
            continue;
        history[1] = r_u32(0x800A71BC);
        if (history[1] == selected)
            continue;
        history[2] = r_u32(0x800A71C0);
        if (history[2] == selected)
            continue;
        history[3] = r_u32(0x800A71C4);
        if (history[3] == selected)
            continue;
        history[4] = r_u32(0x800A71C8);
        if (history[4] == selected)
            continue;
        w_u32(0x800A71B8, history[1]);
        w_u32(0x800A71BC, history[2]);
        w_u32(0x800A71C0, history[3]);
        w_u32(0x800A71C4, history[4]);
        w_u32(0x800A71C8, selected);
        kind = r_u32(0x800FF5B8);
        variant = r_u32(0x800FF5BC);
        return PlayCdVoiceEvent(kind, variant, position, 0);
    }
}

uint32 Player::ReadInput()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005CB1C, "SLUS_003.73");
    uint32 disabled, pad, pad_type, x, y, value, factor, divisor, quotient, result;
    disabled = r_u32(object + 452);
    w_u8(object + 468, 0);
    w_u8(object + 469, 0);
    w_u8(object + 471, 0);
    w_u8(object + 472, 0);
    w_u8(object + 470, 1);
    if (disabled)
    {
        w_u8(object + 473, 0);
        return 1;
    }
    pad = r_u32(object + 444);
    pad_type = r_u32(pad + 364);
    if (pad_type == 115 || pad_type == 83)
    {
        value = r_u8(pad + 360);
        pad = r_u32(object + 444);
        w_u8(object + 468, value);
        value = r_u8(pad + 361);
        w_u8(object + 469, value);
        value = r_u8(pad + 362);
        w_u8(object + 471, value);
        value = r_u8(pad + 363);
        w_u8(object + 472, value);
    }
    x = r_u8(object + 468);
    y = r_u8(object + 469);
    if (!(x | y))
    {
        pad = r_u32(object + 444);
        value = r_u8(pad + 160);
        x = 128;
        if (!value)
        {
            value = r_u8(pad + 176);
            x = 127;
        }
        if (value)
        {
            w_u8(object + 468, x);
            pad = r_u32(object + 444);
        }
        value = r_u8(pad + 144);
        y = 127;
        if (!value)
        {
            value = r_u8(pad + 128);
            y = 128;
        }
        if (value)
            w_u8(object + 469, y);
    }
    factor = r_u16(object + 574);
    if (factor)
    {
        x = (uint32)(sint32)(sint8)r_u8(object + 468);
        divisor = r_u16(object + 576);
        quotient = DividePlayerInput(factor * x, divisor);
        w_u8(object + 468, x - quotient);
        y = (uint32)(sint32)(sint8)r_u8(object + 469);
        factor = r_u16(object + 574);
        divisor = r_u16(object + 576);
        quotient = DividePlayerInput(factor * y, divisor);
        w_u8(object + 469, y - quotient);
    }
    x = r_u8(object + 471);
    y = r_u8(object + 472);
    if (x | y)
        w_u8(object + 473, 0);
    else
    {
        pad = r_u32(object + 444);
        w_u8(object + 470, 0);
        value = r_u8(pad);
        x = 128;
        if (!value)
        {
            value = r_u8(pad + 48);
            x = 127;
        }
        if (value)
            w_u8(object + 471, x);
        pad = r_u32(object + 444);
        value = r_u8(pad + 32);
        y = 127;
        if (!value)
        {
            value = r_u8(pad + 16);
            y = 128;
        }
        if (value)
        {
            w_u8(object + 472, y);
            pad = r_u32(object + 444);
        }
        if (r_u8(pad) || r_u8(pad + 48) || r_u8(pad + 32) || r_u8(pad + 16))
        {
            value = r_u8(object + 473) + 1;
            w_u8(object + 473, value);
            if ((value & 255) >= 2)
                w_u8(object + 473, 2);
        }
        else
            w_u8(object + 473, 0);
    }
    x = r_u8(object + 468);
    y = r_u8(object + 469);
    if (x | y)
    {
        result = ratan2(-(sint32)(sint8)x, (sint8)y);
        w_u16(object + 474, (1024 - result) & 4095);
    }
    x = r_u8(object + 471);
    y = r_u8(object + 472);
    result = x | y;
    if (result)
    {
        result = ratan2(-(sint32)(sint8)x, (sint8)y);
        w_u16(object + 476, (1024 - result) & 4095);
    }
    return result;
}

uint32 Player::RestoreHealth(uint32 amount)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 health, maximum, sound;
    sint32 signed_maximum;
    FUNCTION_MARKER(0x8005CE70, "SLUS_003.73");
    health = r_u16(object + 218) + amount;
    w_u16(object + 218, health);
    signed_maximum = (sint16)r_u16(0x800EC526);
    maximum = r_u16(0x800EC526);
    if (signed_maximum < (sint16)health)
        w_u16(object + 218, maximum);
    sound = RandomRange(2);
    PlayCdVoiceEvent(1, sound, (const uint32 *)psx_addr(object + 4, 12), 1);
    return 1;
}

uint32 UpdatePlayerLighting(uint32 object)
{
    FUNCTION_MARKER(0x8005CF0C, "SLUS_003.73");
    uint32 color;
    sint32 red;
    sint32 green;
    sint32 blue;
    signed int intensity;
    sint32 weapon;
    uint32 fallback_intensity;
    short fallback_color;
    sint32 heading;
    uint32 trig;
    short sine;
    short cosine;
    uint32 shifted[4];

    union
    {
        uint32 words[4];
        sint16 halves[8];
    } vector;

    unsigned char light_red;
    unsigned char light_green;
    char light_blue[2];
    uint32 shift;
    w_u16(0x800FFBB4, 0);
    w_u16(0x800FFBB6, -4096);
    w_u16(0x800FFBB8, 0);
    w_u16(0x800FFBBC, 0);
    w_u16(0x800FFBBE, 4096);
    w_u16(0x800FFBC0, 0);
    w_u16(0x800FFBC4, 0);
    w_u16(0x800FFBC6, 0);
    w_u16(0x800FFBC8, 0);
    w_u16(0x800FFBD0, 4096);
    w_u16(0x800FFBCE, 4096);
    w_u16(0x800FFBCC, 4096);
    color = r_u32(((uint32)((object + 32))));
    red = ((unsigned char)(color));
    green = ((color >> 8) & 255);
    blue = ((color >> 16) & 255);
    intensity = (color >> 24);
    if ((((uint32)((intensity - 1))) < 0x7F))
    {
        red = ((red * (0x80000 / intensity)) >> 12);
        green = ((green * (0x80000 / intensity)) >> 12);
        blue = ((blue * (0x80000 / intensity)) >> 12);
    }
    w_u16(0x800FFBD4, 16 * (uint32)red);
    w_u16(0x800FFBD6, 16 * (uint32)green);
    w_u16(0x800FFBD8, 16 * (uint32)blue);
    uint32 first_color = r_u32(0x800FFBD4);
    uint32 second_color = r_u32(0x800FFBD8);
    w_u32(0x800FFBDC, first_color);
    w_u32(0x800FFBE0, second_color);
    weapon = r_u32(((uint32)(((object + (4 * r_u32(((uint32)((object + 644)))))) + 612))));
    if (apocalypse_weapon_light_colors((uint32)weapon, &light_red, &light_green, (uint8 *)light_blue))
    {
        SubtractVectors(vector.words, (const uint32 *)psx_addr(object + 416, 12), (const uint32 *)psx_addr(object + 4, 12));
        shift = 12;
        ShiftVectorRight(shifted, vector.words, &shift);
        NormalizeFixedVector(vector.words, shifted);
        w_u16(0x800FFBCC, (16 * light_red));
        w_u16(0x800FFBB4, vector.halves[0]);
        w_u16(0x800FFBB6, 0);
        w_u16(0x800FFBB8, vector.halves[4]);
        w_u16(0x800FFBCE, (16 * light_green));
        w_u16(0x800FFBD0, (16 * ((unsigned char)(light_blue[0]))));
    }
    else
    {
        w_u16(0x800FFBB8, 0);
        w_u16(0x800FFBB4, 0);
        w_u16(0x800FFBB6, -4096);
        fallback_intensity = r_u8(((uint32)((object + 35))));
        fallback_color = (16 * fallback_intensity);
        if ((fallback_intensity < 0x40))
            fallback_color = 1024;
        w_u16(0x800FFBD0, fallback_color);
        w_u16(0x800FFBCE, fallback_color);
        w_u16(0x800FFBCC, fallback_color);
    }
    uint32 heading_object = r_u32(0x800FF904);
    heading = heading_object ? r_u16(heading_object + 494) & 4095 : 0;
    trig = 0x800F863C + heading * 4;
    sine = r_u16(trig);
    uint32 normal_x = r_u16(0x800FFBB4);
    uint32 normal_z = r_u16(0x800FFBB8);
    uint32 second_x = r_u16(0x800FFBBC);
    uint32 second_y = r_u16(0x800FFBBE);
    uint32 second_z = r_u16(0x800FFBC0);
    w_u16(0x800FFBC6, 0);
    w_u16(0x800FFBC4, sine);
    cosine = r_u16(trig + 2);
    w_u16(0x800EE6F0, normal_x);
    uint32 normal_y = r_u16(0x800FFBB6);
    w_u16(0x800EE6F4, normal_z);
    w_u16(0x800EE6F6, second_x);
    w_u16(0x800EE6F8, second_y);
    w_u16(0x800EE6FA, second_z);
    w_u16(0x800EE6FC, sine);
    w_u16(0x800EE6FE, 0);
    w_u16(0x800EE6F2, normal_y);
    uint32 light_x = r_u16(0x800FFBCC);
    uint32 color_x = r_u16(0x800FFBDC);
    uint32 light_y = r_u16(0x800FFBCE);
    uint32 color_y = r_u16(0x800FFBD6);
    uint32 next_color_y = r_u16(0x800FFBDE);
    uint32 light_z = r_u16(0x800FFBD0);
    w_u16(0x800EE710, light_x);
    uint32 first_color_x = r_u16(0x800FFBD4);
    w_u16(0x800EE714, color_x);
    w_u16(0x800EE716, light_y);
    w_u16(0x800EE718, color_y);
    w_u16(0x800EE71A, next_color_y);
    w_u16(0x800EE712, first_color_x);
    w_u16(0x800FFBC8, cosine);
    w_u16(0x800EE700, cosine);
    w_u16(0x800EE71C, light_z);
    uint32 first_color_z = r_u16(0x800FFBD8);
    uint32 next_color_z = r_u16(0x800FFBE0);
    w_u16(0x800FFA98, 64);
    w_u16(0x800FFA96, 64);
    w_u16(0x800FFA94, 64);
    w_u16(0x800EE71E, first_color_z);
    w_u16(0x800EE720, next_color_z);
    return 64;
}

uint32 Player::SetContactColor(uint32 polygon, uint32 surface)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, table, count, index, vertex, selected = 0, maximum = 0, color, component, result;
    bool found = false;
    FUNCTION_MARKER(0x8005D20C, "SLUS_003.73");
    flags = r_u16(polygon);
    if (flags & 2048)
    {
        table = r_u32(0x800EAEF8 + r_u8(surface + 27) * 64 + 32);
        count = (flags & 16) ? 3 : 4;
        for (index = 0; index < count; ++index)
        {
            vertex = r_u8(polygon + 12 + index);
            color = r_u32(table + vertex * 4) & 0xFFFFFF;
            if ((maximum & 255) < (color & 255))
            {
                maximum = color;
                selected = vertex;
                found = true;
            }
            component = color >> 8;
            if ((maximum & 255) < (component & 255))
            {
                maximum = component;
                selected = vertex;
                found = true;
            }
            component = color >> 16;
            if ((maximum & 255) < component)
            {
                maximum = component;
                selected = vertex;
                found = true;
            }
        }
        if (!found)
        {
            // Human-approved repair of the original undefined all-black index
            selected = r_u8(polygon + 12);
        }
        color = r_u32(table + selected * 4);
        result = color + (maximum << 24);
    }
    else
    {
        color = r_u32(polygon + 12) & 0xFFFFFF;
        if (color & 255)
            maximum = color;
        component = color >> 8;
        if ((maximum & 255) < (component & 255))
            maximum = component;
        component = color >> 16;
        if ((maximum & 255) < component)
            maximum = component;
        result = color + (maximum << 24);
    }
    w_u32(object + 32, result);
    return result;
}

uint32 Player::UpdateWeapons(const sint16 *a2, uint32 a3)
{
    const uint32 a1 = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005D360, "SLUS_003.73");
    sint32 result;
    sint32 v7;
    sint32 v8;
    uint32 v9;
    sint32 v10;
    uint32 v11;
    sint32 v12;
    uint32 v13;
    sint32 v14;
    uint32 v15;
    sint32 v16;
    uint32 v17;
    sint32 v18;
    uint32 v19;
    sint32 v20;
    uint32 v21;
    sint32 v22;
    uint32 v23;
    sint32 v24;
    uint32 v25;
    sint32 v26;
    sint32 v27;
    uint32 v28;
    uint32 v29;
    uint32 v30;
    sint32 v31;
    sint8 v32;
    short v33;
    uint32 cone, button1, button2;
    sint32 v34;
    sint32 v35;
    sint32 v36;
    const sint16 *v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    uint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    uint32 v48;
    sint32 v49;
    int v54[4];
    uint32 angles[2];
    result = r_u32(((uint32)((a1 + 360))));
    if (!result)
        return result;
    v7 = 0;
    if (((((r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 288)))) && (r_u32(((uint32)((a1 + 460)))) != 0x40000)) && !r_u32(0x800FF218)) && !(r_u32(((uint32)((a1 + 452)))))) && r_u32(((uint32)((a1 + 532))))))
    {
        v8 = AllocateEffectStorage(172);
        if (v8)
        {
            void *dram_addr = AllocateNativeObjectStorage((uint32)v8, sizeof(RingAttack));
            new (dram_addr) RingAttack(100, 10, 8192);
        }
        if ((r_u32(0x800FF2FC) == 2))
            SetGamePadActuator(0, 12, 1, 120);
        if (!r_u32(0x800FF318))
        {
            v9 = r_u32(((uint32)((a1 + 532))));
            v10 = (v9 == 0);
            v11 = (v9 - 1);
            if (!v10)
                w_u32(((uint32)((a1 + 532))), v11);
        }
        PlayCdVoiceEvent(0, 2, (const uint32 *)psx_addr(a1 + 4, 12), 1);
    }
    v12 = r_u32(((uint32)((a1 + 444))));
    if (r_u8(((uint32)((v12 + 305)))))
    {
        w_u8(((uint32)((v12 + 305))), 0);
        if (r_u32(0x800FF318))
        {
            PlaySoundEffect(26, 8192, 0);
            switch (r_u32(((uint32)((r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612)))) + 4)))))
            {
                case 1:
                    if (!(r_u32(((uint32)((a1 + 620))))))
                    {
                        v15 = AllocateZeroedHeapBlock(68);
                        if (v15)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(v15, sizeof(PulseLaser));
                            new (dram_addr) PulseLaser(0x800FF4E8);
                        }
                        v16 = 10;
                        v10 = (r_u32(0x800FF384) != 0);
                        w_u32(((uint32)((a1 + 620))), v15);
                        if (!v10)
                            v16 = 15;
                        w_u32((v15 + (10) * 4), v16);
                    }
                    w_u32(((uint32)((a1 + 648))), 1);
                    w_u32(((uint32)((a1 + 644))), 2);
                    break;

                case 2:
                    if (!(r_u32(((uint32)((a1 + 624))))))
                    {
                        v17 = AllocateZeroedHeapBlock(104);
                        if (v17)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(v17, sizeof(ParticleBeam));
                            new (dram_addr) ParticleBeam(0x800FF4E8, 3000, 0, 256, 9);
                        }
                        v18 = 10;
                        v10 = (r_u32(0x800FF384) != 0);
                        w_u32(((uint32)((a1 + 624))), v17);
                        if (!v10)
                            v18 = 15;
                        w_u32((v17 + (10) * 4), v18);
                    }
                    w_u32(((uint32)((a1 + 648))), 2);
                    w_u32(((uint32)((a1 + 644))), 3);
                    break;

                case 3:
                    if (!(r_u32(((uint32)((a1 + 628))))))
                    {
                        v19 = AllocateZeroedHeapBlock(100);
                        if (v19)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(v19, sizeof(RipLaser));
                            new (dram_addr) RipLaser(0x800FF4E8);
                        }
                        v20 = 2;
                        v10 = (r_u32(0x800FF384) != 0);
                        w_u32(((uint32)((a1 + 628))), v19);
                        if (!v10)
                            v20 = 3;
                        w_u32((v19 + (10) * 4), v20);
                    }
                    w_u32(((uint32)((a1 + 648))), 3);
                    w_u32(((uint32)((a1 + 644))), 4);
                    break;

                case 4:
                    if (!(r_u32(((uint32)((a1 + 632))))))
                    {
                        v21 = AllocateZeroedHeapBlock(68);
                        if (v21)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(v21, sizeof(Grenades));
                            new (dram_addr) Grenades(0x800FF4E8);
                        }
                        v22 = 120;
                        v10 = (r_u32(0x800FF384) != 0);
                        w_u32(((uint32)((a1 + 632))), v21);
                        if (!v10)
                            v22 = 180;
                        w_u32((v21 + (10) * 4), v22);
                    }
                    w_u32(((uint32)((a1 + 648))), 4);
                    w_u32(((uint32)((a1 + 644))), 5);
                    break;

                case 5:
                    if (!(r_u32(((uint32)((a1 + 636))))))
                    {
                        v23 = AllocateZeroedHeapBlock(68);
                        if (v23)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(v23, sizeof(HomingMissiles));
                            new (dram_addr) HomingMissiles(0x800FF4E8);
                        }
                        v24 = 15;
                        v10 = (r_u32(0x800FF384) != 0);
                        w_u32(((uint32)((a1 + 636))), v23);
                        if (!v10)
                            v24 = 22;
                        w_u32((v23 + (10) * 4), v24);
                    }
                    w_u32(((uint32)((a1 + 648))), 5);
                    w_u32(((uint32)((a1 + 644))), 6);
                    break;

                case 6:
                    if (!(r_u32(((uint32)((a1 + 640))))))
                    {
                        v25 = AllocateZeroedHeapBlock(68);
                        if (v25)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(v25, sizeof(Rockets));
                            new (dram_addr) Rockets(0x800FF4E8);
                        }
                        v26 = 40;
                        v10 = (r_u32(0x800FF384) != 0);
                        w_u32(((uint32)((a1 + 640))), v25);
                        if (!v10)
                            v26 = 60;
                        w_u32((v25 + (10) * 4), v26);
                    }
                    w_u32(((uint32)((a1 + 648))), 6);
                    w_u32(((uint32)((a1 + 644))), 7);
                    break;

                case 7:
                    w_u32(((uint32)((a1 + 648))), 7);
                    w_u32(((uint32)((a1 + 644))), 0);
                    break;

                case 8:
                    if (!(r_u32(((uint32)((a1 + 616))))))
                    {
                        v13 = AllocateZeroedHeapBlock(72);
                        if (v13)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(v13, sizeof(Flamethrower));
                            new (dram_addr) Flamethrower(0x800FF4E8, 0);
                        }
                        v14 = 10;
                        v10 = (r_u32(0x800FF384) != 0);
                        w_u32(((uint32)((a1 + 616))), v13);
                        if (!v10)
                            v14 = 15;
                        w_u32((v13 + (10) * 4), v14);
                    }
                    w_u32(((uint32)((a1 + 648))), 0);
                    w_u32(((uint32)((a1 + 644))), 1);
                    break;

                default:
                    break;
            }
        }
        else
        {
            v28 = r_u32(((uint32)((a1 + 644))));
            v29 = (v28 + 1);
            if ((sint32)(v28 + 1) < 8)
            {
                v30 = ((4 * (v28 + 1)) + a1);
                while (!(r_u32(((uint32)((v30 + 612))))))
                {
                    ++v29;
                    v30 += 4;
                    if ((sint32)v29 >= 8)
                        goto LABEL_73;
                }

                PlaySoundEffect(26, 8192, 0);
                v27 = r_u32(((uint32)((a1 + 644))));
                w_u32(((uint32)((a1 + 644))), v29);
                w_u32(((uint32)((a1 + 648))), v27);
            }
        LABEL_73:
            if (((v28 == r_u32(((uint32)((a1 + 644))))) && v28))
            {
                PlaySoundEffect(26, 8192, 0);
                v31 = r_u32(((uint32)((a1 + 644))));
                w_u32(((uint32)((a1 + 644))), 0);
                w_u32(((uint32)((a1 + 648))), v31);
            }
        }
    }
    GetModelAttachment((uint32 *)v54, a1, 0);
    button1 = r_u32(a1 + 460) & 0xF3800;
    if (!button1)
    {
        button1 = r_u8(a1 + 471);
        button2 = r_u8(a1 + 472);
        button1 = !(button1 | button2) || !a3;
    }
    if (button1)
    {
        v47 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
        if (v47)
            CallPlayerWeaponSlot(v47, 24, v54, a2);
        w_u8(((uint32)((a1 + 536))), 0);
        AddVectors((uint32 *)v54, (const uint32 *)psx_addr(a1 + 104, 12));
        goto LABEL_126;
    }
    v32 = r_u8(((uint32)((a1 + 536))));
    w_u16(((uint32)((a1 + 580))), 0);
    w_u8(((uint32)((a1 + 536))), (v32 + 1));
    CopyAngleVector((uint16 *)angles, (const uint16 *)a2);
    if (!(r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))))))
        goto LABEL_122;
    v34 = v54[1];
    v35 = v54[2];
    w_u32(((uint32)((a1 + 416))), v54[0]);
    w_u32(((uint32)((a1 + 420))), v34);
    w_u32(((uint32)((a1 + 424))), v35);
    v36 = r_u32(((uint32)((r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612)))) + 4))));
    if ((v36 == 5))
        goto LABEL_86;
    if ((v36 < 6))
    {
        if ((v36 != 2))
            goto LABEL_93;
    LABEL_86:
        cone = 768;
        button1 = r_u8(0x800EC0F8);
        if (!button1)
            button1 = r_u8(0x800EC128);
        if (button1)
        {
            button2 = r_u8(0x800EC108);
            if (!button2)
                button2 = r_u8(0x800EC118);
            if (button2)
                cone = 1536;
        }
        SelectAimTarget((sint16 *)angles, (const uint32 *)v54, a2, 4096, cone, 3072, 0, 0, r_u32(0x800FF4E8));
        goto LABEL_93;
    }
    if (((v36 < 9) && (v36 >= 7)))
        goto LABEL_86;
LABEL_93:
    v38 = 0;

    if ((r_u8(((uint32)((a1 + 536)))) < 2))
    {
        v40 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
        CallPlayerWeaponSlot(v40, 24, v54, (const sint16 *)angles);
    }
    else
    {
        v39 = r_u32(((uint32)((a1 + 644))));
        w_u8(((uint32)((a1 + 536))), 1);
        {
            uint32 weapon = r_u32(a1 + 612 + 4 * (uint32)v39);
            v38 = CallPlayerWeaponSlot(weapon, 16, v54, (const sint16 *)angles);
        }
    }
    if (r_u32(0x800FF2FC))
    {
        switch (v36)
        {
            case 1:
                if (((r_u32(0x800FF2FC) == 2) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 4;
                    v42 = 1;
                    v43 = 60;
                    goto LABEL_116;
                }
                break;

            case 2:
                if (((v38 && (r_u32(0x800FF2FC) == 2)) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 2;
                    goto LABEL_115;
                }
                break;

            case 3:

            case 4:
                if (((r_u32(0x800FF2FC) == 2) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 2;
                    goto LABEL_115;
                }
                break;

            case 5:

            case 6:

            case 7:
                if (((v38 && (r_u32(0x800FF2FC) == 2)) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 4;
                LABEL_115:
                    v42 = 0;

                    v43 = 1;
                    goto LABEL_116;
                }
                break;

            case 8:
                if (((r_u32(0x800FF2FC) == 2) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 4;
                    v42 = 1;
                    v43 = 120;
                LABEL_116:
                    SetGamePadActuator(0, v41, v42, v43);
                }
                break;

            default:
                break;
        }
    }
    v44 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
    v7 = 1;
    if ((r_u32((v44 + (1) * 4)) == 8))
        goto LABEL_126;
    v45 = 0;
    if ((((sint32)(r_u32((v44 + (14) * 4)))) > 0))
        goto LABEL_127;
    if (v44)
    {
        uint32 table = r_u32(v44);
        uint32 receiver = v44 + (uint32)(sint32)(sint16)r_u16(table + 8);
        uint32 target = r_u32(table + 12);
        (void)apocalypse_object_cleanup(target, receiver, 3);
    }
    w_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))), 0);
    v46 = r_u32(((uint32)((a1 + 644))));
    w_u32(((uint32)((a1 + 644))), 0);
    w_u32(((uint32)((a1 + 648))), v46);
LABEL_122:
    v7 = 1;

LABEL_126:
    v45 = 0;

LABEL_127:
    v48 = a1;

    do
    {
        if ((v45 != r_u32(((uint32)((a1 + 644))))))
        {
            v49 = r_u32(((uint32)((v48 + 612))));
            if (v49)
                CallPlayerWeaponSlot(v49, 24, v54, a2);
        }
        ++v45;
        v48 += 4;
    } while ((v45 < 8));
    result = -2146500608;
    if (v7)
    {
        result = (sint16)r_u16(0x800EC666);
        w_u32(0x800FF5C4, (uint32)result);
    }
    return result;
}

uint32 Player::Finish()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, flags;
    FUNCTION_MARKER(0x8005DD40, "SLUS_003.73");
    result = IsFinished();
    if (!result)
    {
        result = r_u16(object + 78);
        flags = r_u16(object);
        result |= 0x40;
        flags |= 1;
        w_u16(object + 78, result);
        w_u16(object, flags);
    }
    return result;
}

uint32 Player::ClearWeapons()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005DD84, "SLUS_003.73");
    for (uint32 index = 0; index < 8; ++index)
    {
        uint32 slot = object + 612 + index * 4;
        uint32 child = r_u32(slot);
        if (child)
        {
            uint32 table = r_u32(child);
            uint32 receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
            uint32 target = r_u32(table + 12);
            apocalypse_object_cleanup(target, receiver, 3);
            w_u32(slot, 0);
        }
    }
    return 0;
}

uint32 Player::Damage(uint32 damage, const sint32 *vector, uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005DDF0, "SLUS_003.73");
    if (r_u32(0x800FF31C))
        return 0;
    if ((sint16)r_u16(object + 218) <= 0)
        return 0;
    if (VisualObject::IsFinished())
        return 0;
    if (r_u32(object + 452))
        return 0;
    if (r_u32(object + 456))
        return 0;
    uint32 impulse[3], shift = 1;
    // Preserve the existing ABI mapping of a zero guest vector address
    const uint32 *input = vector ? (const uint32 *)vector : (const uint32 *)psx_addr(0, 12);
    ShiftVectorRight(impulse, input, &shift);
    AddVectors((uint32 *)psx_addr(object + 104, 12), impulse);
    if ((sint32)damage >= 2)
    {
        sint32 maximum = (sint16)r_u16(0x800EC526);
        sint32 threshold = maximum / 5;
        sint32 health = (sint16)r_u16(object + 218);
        if (health < threshold)
            damage = (uint32)((sint32)damage >> 1);
        if (!r_u32(0x800FF384))
            damage = (uint32)((sint32)damage >> 1);
    }
    uint32 remaining = r_u16(object + 218) - damage;
    w_u16(object + 218, remaining);
    if ((sint16)remaining <= 0)
    {
        w_u16(object + 218, 0);
        EnterDeathState();
        sub_80070288(0, 0);
        sub_80070288(0, 1);
        return 1;
    }
    w_u8(object + 465, 6);
    if (reason == 5 || reason == 31)
    {
        w_u8(object + 466, 90);
        SetColor(90, 255, 0, 0);
        w_u16(object + 180, 10240);
        uint32 intensity = r_u8(object + 466);
        w_u16(object + 182, 10);
        w_u32(object + 176, (intensity << 1) | (intensity << 9) | 0x2000000 | (intensity << 17));
    }
    if (r_u32(0x800FF2FC))
    {
        SetGamePadActuator(0, 4, 0, 1);
        SetGamePadActuator(1, 4, 1, 255);
    }
    SetScreenFlash(128, 0, 0, 8, 0, 0);
    uint32 sound = RandomRange(4);
    PlaySoundEffect(sound + 4, 8192, 0);
    if (!r_u8(object + 26))
    {
        uint32 range = RandomRange(4);
        uint32 first_frame = r_u8(0x800FF5CC + range * 2);
        uint32 last_frame = r_u8(0x800FF5CC + range * 2 + 1);
        SetAnimationRange(6, first_frame, last_frame);
    }
    return 1;
}

uint32 Player::ApplyContactImpulse(uint32 object)
{
    const uint32 player = NativeObjectGuestAddress(this);
    uint32 difference[3], shifted[3], scaled[3], divided[3], impulse[3];
    uint32 distance, left_shift, right_shift;
    FUNCTION_MARKER(0x8005E07C, "SLUS_003.73");
    SubtractVectors(difference, (const uint32 *)psx_addr(player + 4, 12), (const uint32 *)psx_addr(object + 4, 12));
    difference[1] = 0;
    distance = MeasureVectorLength(difference);
    if (!distance)
        distance = 1;
    left_shift = 12;
    ShiftVectorRight(shifted, difference, &left_shift);
    MultiplyScalarVector(scaled, (const uint32 *)psx_addr(object + 208, 4), shifted);
    DivideVector(divided, scaled, &distance);
    right_shift = 12;
    ShiftVectorLeft(impulse, divided, &right_shift);
    AddVectors((uint32 *)psx_addr(player + 104, 12), impulse);
    return player + 104;
}

uint32 Player::ApplyBeamContactImpulse(uint32 object)
{
    const uint32 player = NativeObjectGuestAddress(this);
    uint32 left, right, x, z, beam_angle, player_angle, subtract, impulse[3];
    uint16 angles[4];
    FUNCTION_MARKER(0x8005E154, "SLUS_003.73");
    left = r_u32(object + 304);
    right = r_u32(object + 12);
    x = r_u32(object + 296);
    z = r_u32(object + 4);
    beam_angle = (1024 - (uint32)ratan2((sint32)(left - right), (sint32)(x - z))) & 4095;
    left = r_u32(player + 236);
    right = r_u32(object + 12);
    x = r_u32(player + 228);
    z = r_u32(object + 4);
    player_angle = (1024 - (uint32)ratan2((sint32)(left - right), (sint32)(x - z))) & 4095;
    subtract = beam_angle < player_angle ? player_angle - beam_angle < 2049 : beam_angle - player_angle >= 2049;
    angles[0] = 0;
    angles[1] = (uint16)((subtract ? beam_angle - 1024 : beam_angle + 1024) & 4095);
    angles[2] = 0;
    BuildDirectionVector(impulse, 512, angles);
    AddVectors((uint32 *)psx_addr(player + 104, 12), impulse);
    w_u16(player + 574, 60);
    w_u16(player + 576, 60);
    return 60;
}

uint32 Player::ReplaceWeapon(uint32 kind)
{
    const uint32 owner = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005E260, "SLUS_003.73");
    kind &= 65535;
    uint32 old, table, object, slot, lifetime, previous, receiver, target;
    if (kind < 1 || kind > 7)
        return 0;
    if (kind == 1)
        PlayCdVoiceEvent(0, 3, (const uint32 *)psx_addr(owner + 4, 12), 1);
    else
        PlayCdVoiceEvent(0, RandomRange(2), (const uint32 *)psx_addr(owner + 4, 12), 1);
    slot = owner + 0x264 + 4 * kind;
    old = r_u32(slot);
    if (old)
    {
        table = r_u32(old);
        receiver = old + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        (void)apocalypse_object_cleanup(target, receiver, 3);
    }
    switch (kind)
    {
        case WPN_FLAMETHROWER:
            object = AllocateZeroedHeapBlock(0x48);
            if (object)
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Flamethrower));
                new (dram_addr) Flamethrower(0x800FF4E8, 0);
            }
            lifetime = r_u32(0x800FF384) ? 10 : 15;
            break;
        case WPN_PULSE_LASER:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PulseLaser));
                new (dram_addr) PulseLaser(0x800FF4E8);
            }
            lifetime = r_u32(0x800FF384) ? 10 : 15;
            break;
        case WPN_PARTICLE_BEAM:
            object = AllocateZeroedHeapBlock(0x68);
            if (object)
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ParticleBeam));
                new (dram_addr) ParticleBeam(0x800FF4E8, 3000, 0, 256, 9);
            }
            lifetime = r_u32(0x800FF384) ? 10 : 15;
            break;
        case WPN_RIP_LASER:
            object = AllocateZeroedHeapBlock(0x64);
            if (object)
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RipLaser));
                new (dram_addr) RipLaser(0x800FF4E8);
            }
            lifetime = r_u32(0x800FF384) ? 2 : 3;
            break;
        case WPN_GRENADES:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Grenades));
                new (dram_addr) Grenades(0x800FF4E8);
            }
            lifetime = r_u32(0x800FF384) ? 120 : 180;
            break;
        case WPN_HOMING_MISSILES:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(HomingMissiles));
                new (dram_addr) HomingMissiles(0x800FF4E8);
            }
            lifetime = r_u32(0x800FF384) ? 15 : 22;
            break;
        default:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Rockets));
                new (dram_addr) Rockets(0x800FF4E8);
            }
            lifetime = r_u32(0x800FF384) ? 40 : 60;
            break;
    }
    w_u32(slot, object);
    // TODO The original writes the ammo field even when allocation returns zero
    w_u32(object + 40, lifetime);
    previous = r_u32(owner + 0x284);
    w_u32(owner + 0x284, kind);
    w_u32(owner + 0x288, previous);
    return 1;
}

uint32 Player::EnterDeathState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005E708, "SLUS_003.73");
    PlaySoundEffect(8, 0x2000, 0);
    PrepareDeathCamera();
    uint32 state = r_u32(object + 460), animation;
    w_u16(object + 218, 0);
    switch (state)
    {
        case 1:
        case 16:
        case 32:
        case 256:
        case 512:
        case 1024:
        case 2048:
            animation = 15;
            break;
        case 2:
        case 4:
        case 8:
        case 128:
        case 4096:
        case 8192:
        case 16384:
        case 32768:
        case 65536:
        case 131072:
        case 524288:
            animation = 16;
            break;
        case 262144:
            return 0x80000;
        default:
        {
            uint32 table = r_u32(object + 68);
            sint32 adjustment = (sint16)r_u16(table + 16);
            uint32 target = r_u32(table + 20);
            return xport_draft_guest_call1(target, object + (uint32)adjustment);
        }
    }
    w_u32(object + 112, 0);
    w_u32(object + 108, 0);
    w_u32(object + 104, 0);
    w_u32(object + 460, 0x40000);
    return SetAnimationRange(animation, 0, (uint32)-1);
}

void Player::PrepareDeathCamera()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 camera, mode, angle;
    sint32 distance;
    FUNCTION_MARKER(0x8005E90C, "SLUS_003.73");
    camera = r_u32(0x800FF904);
    if (!camera)
        return;
    if (!r_u32(object + 448))
        return;
    w_u16(object + 652, 0);
    SetCameraOrbitDistanceTransition(64, 32);
    mode = r_u32(0x800FF378);
    distance = mode == 10 ? -1400 : -700;
    camera = r_u32(0x800FF904);
    SetCameraOrbitHeightTransition((uint32)distance, 32);
    camera = r_u32(0x800FF904);
    SetCameraTargetLateralOffsetTransition(0, 32);
    camera = r_u32(0x800FF904);
    SetCameraTargetHeightOffsetTransition(0, 32);
    camera = r_u32(0x800FF904);
    SetCameraTargetForwardOffsetTransition(0, 32);
    camera = r_u32(0x800FF904);
    angle = r_u16(camera + 494);
    Camera::Attach(camera).SetYawTransition((angle + 1024) & 0xFFF, 34);
}

uint32 Player::InitializeAnimation()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005E9E4, "SLUS_003.73");
    SetAnimationRange(0, 0, (uint32)-1);
    w_u32(object + 460, 1);
    return 1;
}

uint32 Player::TryEnterMovementState16()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 animation;
    FUNCTION_MARKER(0x8005EA1C, "SLUS_003.73");
    if ((sint8)r_u8(object + 468) >= 0)
        return 0;
    if ((uint32)(r_u16(object + 474) - (uint32)768) < 2561)
        return 0;
    animation = r_u8(object + 26);
    w_u32(object + 460, 16);
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState32()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 animation;
    FUNCTION_MARKER(0x8005EA80, "SLUS_003.73");
    if ((sint8)r_u8(object + 468) <= 0)
        return 0;
    sint32 angle = (sint16)r_u16(object + 474);
    if (angle < 1281 || angle >= 2816)
        return 0;
    animation = r_u8(object + 26);
    w_u32(object + 460, 32);
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState512()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 animation;
    FUNCTION_MARKER(0x8005EAF0, "SLUS_003.73");
    if ((sint8)r_u8(object + 469) >= 0)
        return 0;
    sint32 angle = (sint16)r_u16(object + 474);
    if (angle < 2816 || angle >= 3329)
        return 0;
    animation = r_u8(object + 26);
    w_u32(object + 460, 512);
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState1024()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 animation;
    FUNCTION_MARKER(0x8005EB60, "SLUS_003.73");
    if ((sint8)r_u8(object + 469) <= 0)
        return 0;
    sint32 angle = (sint16)r_u16(object + 474);
    if (angle < 768 || angle >= 1281)
        return 0;
    animation = r_u8(object + 26);
    w_u32(object + 460, 1024);
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryActivateOneShotActivator()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 attached, first, second, angles, tail;
    FUNCTION_MARKER(0x8005EBD0, "SLUS_003.73");
    if (!r_u32(object + 516))
        return 0;
    first = r_u8(object + 471);
    second = r_u8(object + 472);
    if (!(first | second))
        return 0;
    w_u32(object + 112, 0);
    w_u32(object + 108, 0);
    w_u32(object + 104, 0);
    w_u32(object + 124, 0);
    w_u32(object + 120, 0);
    attached = r_u32(object + 516);
    w_u32(object + 116, 0);
    angles = r_u32(attached + 16);
    tail = r_u16(attached + 20);
    w_u32(object + 16, angles);
    w_u16(object + 20, tail);
    SetAnimationRange(7, 0, (uint32)-1);
    attached = r_u32(object + 516);
    w_u32(object + 460, 65536);
    OneShotActivator::Attach(attached).Activate();
    return 1;
}

uint32 Player::TryActivateResettingActivator()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 attached, first, second, angles, tail;
    FUNCTION_MARKER(0x8005EC7C, "SLUS_003.73");
    if (!r_u32(object + 520))
        return 0;
    first = r_u8(object + 471);
    second = r_u8(object + 472);
    if (!(first | second))
        return 0;
    w_u32(object + 112, 0);
    w_u32(object + 108, 0);
    w_u32(object + 104, 0);
    w_u32(object + 124, 0);
    w_u32(object + 120, 0);
    attached = r_u32(object + 520);
    w_u32(object + 116, 0);
    angles = r_u32(attached + 16);
    tail = r_u16(attached + 20);
    w_u32(object + 16, angles);
    w_u16(object + 20, tail);
    SetAnimationRange(18, 0, (uint32)-1);
    attached = r_u32(object + 520);
    w_u32(object + 460, 131072);
    ResettingActivator::Attach(attached).BeginActivation();
    return 1;
}

extern "C" uint32 sub_8005ED28(void)
{
    FUNCTION_MARKER(0x8005ED28, "SLUS_003.73");
    // TODO Recover the original name of this zero-result entry
    return 0;
}

uint32 Player::TryEnterActionState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 input, state, x, z, angle, camera, heading, flags, sound;
    FUNCTION_MARKER(0x8005ED30, "SLUS_003.73");
    input = r_u32(object + 444);
    if (!r_u8(input + 256))
        return 0;
    state = r_u32(object + 460);
    if (state == 256)
    {
        x = r_u8(object + 468);
        z = r_u8(object + 469);
        if (!(x | z))
            return 0;
        angle = r_u16(object + 474);
        camera = r_u32(0x800FF904);
        w_u16(object + 606, angle);
        heading = camera ? r_u16(camera + 494) + angle : angle;
        w_u16(object + 18, heading & 4095);
        flags = r_u16(object);
        w_u32(object + 460, 2048);
        sound = (flags & 8) ? 55 : 0;
        PlaySoundEffect(sound, 8192, 0);
        SetAnimationRange(12, 6, (uint32)-1);
        return 1;
    }
    if (!(state & 1584))
    {
        flags = r_u16(object) & 8;
        w_u32(object + 460, 256);
        if (flags)
            PlaySoundEffect(72, 8192, 0);
        SetAnimationRange(5, 0, (uint32)-1);
        return 1;
    }
    x = r_u8(object + 468);
    z = r_u8(object + 469);
    if (!(x | z))
        return 0;
    angle = r_u16(object + 474);
    camera = r_u32(0x800FF904);
    w_u16(object + 606, angle);
    heading = camera ? r_u16(camera + 494) + angle : angle;
    w_u16(object + 18, heading & 4095);
    w_u32(object + 460, 2048);
    SetAnimationRange(12, 0, (uint32)-1);
    sound = (r_u16(object) & 8) ? 55 : 0;
    PlaySoundEffect(sound, 8192, 0);
    return 1;
}

uint32 Player::TryEnterRecordMovementState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 record, x, y, z, height, kind, yaw, input;
    FUNCTION_MARKER(0x8005EEDC, "SLUS_003.73");
    record = r_u32(object + 512);
    if (!record || r_u32(0x800FF5A4))
        return 0;
    if (!ProjectMovementConstraint(record, object + 4, object + 496, 0))
        return 0;
    x = r_u32(object + 496);
    y = r_u32(object + 500);
    z = r_u32(object + 504);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    height = r_u32(object + 8);
    record = r_u32(object + 512);
    w_u32(object + 8, height + 524288);
    kind = r_u16(record + 58);
    if (kind != 400 && kind != 401)
        return 0;
    record = r_u32(object + 512);
    w_u32(object + 460, kind == 400 ? 32768 : 16384);
    w_u32(object + 108, 0);
    yaw = r_u16(record + 320);
    input = r_u32(object + 444);
    w_u16(object + 18, (yaw + 2048) & 4095);
    w_u8(input + 273, 0);
    w_u32(object + 508, 1);
    w_u32(0x800FF5A4, 1);
    if (kind == 400)
        w_u32(object + 492, PlaySoundEffect(100, 8192, 0));
    return 1;
}

uint32 Player::TryEnterJumpState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 input, latch, impulse, linked, velocity, first, last, flags, delay;
    FUNCTION_MARKER(0x8005F00C, "SLUS_003.73");
    input = r_u32(object + 444);
    if (!r_u8(input + 273))
        return 0;
    if (r_u8(object + 569))
        return 0;
    PlaySoundEffect(5, 8192, 0);
    if (r_u16(object) & 8)
        PlaySoundEffect(63, 8192, 0);
    latch = r_u8(object + 569);
    w_u8(object + 569, latch + 1);
    impulse = (uint32)(sint32)(sint16)r_u16(0x800EC4DA);
    linked = r_u32(object + 428);
    impulse = (uint32)0 - (impulse << 12);
    w_u32(object + 564, impulse);
    if (linked)
    {
        velocity = r_u32(linked + 108);
        if ((sint32)velocity < -262144)
            velocity <<= 1;
        w_u32(object + 564, impulse + velocity);
    }
    w_u32(object + 460, 2);
    first = (uint32)(sint32)(sint16)r_u16(0x800EC4B6);
    last = (uint32)(sint32)(sint16)r_u16(0x800EC4D4);
    SetAnimationRange(2, first + 1, last);
    flags = r_u16(object + 216) & 1;
    delay = r_u8(0x800EC4D8);
    w_u8(object + 568, delay);
    w_u32(0x800FF5A8, 0);
    if (flags)
        w_u32(object + 608, 0);
    return 1;
}

uint32 Player::TryEnterAirborneState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, first, last;
    bool check_timer = true;
    FUNCTION_MARKER(0x8005F124, "SLUS_003.73");
    if (!(r_u16(object + 216) & 2))
    {
        timer = r_u8(object + 578);
        if (timer)
            w_u8(object + 578, timer - 1);
        else
            check_timer = false;
    }
    if (check_timer && r_u8(object + 578))
        return 0;
    w_u32(object + 480, r_u32(object + 8));
    first = (uint32)(sint32)(sint16)r_u16(0x800EC4D4);
    last = (uint32)(sint32)(sint16)r_u16(0x800EC4D6);
    SetAnimationRange(2, first + 1, last);
    w_u32(object + 460, 4);
    return 1;
}

uint32 Player::TryEnterLandingState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 threshold, distance = 0, numerator, denominator, damage, table, input, sound, receiver, target;
    FUNCTION_MARKER(0x8005F1B0, "SLUS_003.73");
    if (!(r_u16(object + 216) & 2))
        return 0;
    threshold = r_u32(object + 484);
    w_u8(object + 578, 4);
    if (threshold)
        distance = (uint32)((sint32)(r_u32(object + 8) - r_u32(object + 480)) >> 12);
    if (threshold && (sint32)threshold < (sint32)distance)
    {
        numerator = (distance - threshold) * (uint32)(sint32)(sint16)r_u16(0x800EC526);
        denominator = r_u32(object + 488) - threshold;
        if (!denominator)
            damage = (sint32)numerator < 0 ? 1 : (uint32)-1;
        else if (numerator == 0x80000000 && denominator == (uint32)-1)
            damage = numerator;
        else
            damage = (uint32)((sint32)numerator / (sint32)denominator);
        table = r_u32(object + 68);
        receiver = object + (uint32)(sint32)(sint16)r_u16(table + 48);
        target = r_u32(table + 52);
        apocalypse_object_virtual52_native(target, receiver, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
        if ((sint16)r_u16(object + 218) <= 0)
            return 1;
    }
    else if (r_u32(0x800FF2FC))
        SetGamePadActuator(0, 4, 0, 1);
    SetAnimationRange(2, (uint32)(sint32)(sint16)r_u16(0x800EC4D6) + 1, (uint32)-1);
    sound = (r_u16(object) & 8) ? 56 : 19;
    PlaySoundEffect(sound, 8192, 0);
    input = r_u32(object + 444);
    w_u32(object + 460, 8);
    w_u8(input + 273, 0);
    w_u8(object + 538, 0);
    w_u8(object + 537, 0);
    w_u32(0x800FF5A4, 0);
    return 1;
}

uint32 Player::TryEnterAnimation10State()
{
    const uint32 object = NativeObjectGuestAddress(this);
    sint32 scaled[3], position[3], offset[3], angle_z, angle_x;
    uint32 scale = 64, offset_scale = 128, flags, height;
    FUNCTION_MARKER(0x8005F300, "SLUS_003.73");
    if (!(r_u16(object + 216) & 64))
        return 0;
    SetAnimationRange(10, 1, (uint32)-1);
    flags = r_u16(object + 216) & 128;
    w_u32(object + 460, 4096);
    if (!flags)
    {
        w_u32(object + 8, r_u32(object + 544) + 524288);
        return 1;
    }
    MultiplyVector((uint32 *)scaled, (const uint32 *)psx_addr(object + 552, 12), &scale);
    AddVectors((uint32 *)position, (const uint32 *)psx_addr(object + 540, 12), (const uint32 *)scaled);
    w_u32(object + 4, (uint32)position[0]);
    w_u32(object + 8, (uint32)position[1]);
    w_u32(object + 12, (uint32)position[2]);
    angle_z = (sint32)r_u32(object + 560);
    height = r_u32(object + 8);
    angle_x = (sint32)r_u32(object + 552);
    w_u32(object + 8, height + 524288);
    w_u16(object + 18, (1024 - (uint32)ratan2(angle_z, angle_x)) & 4095);
    MultiplyScalarVector((uint32 *)offset, &offset_scale, (const uint32 *)psx_addr(object + 552, 12));
    SubtractVectors((uint32 *)position, (const uint32 *)psx_addr(object + 4, 12), (const uint32 *)offset);
    position[1] = (sint32)((uint32)position[1] - 884736);
    if (!r_u32(object + 428))
    {
        uint32 camera = r_u32(0x800FF904);
        Camera::Attach(camera).SetTargetTransition(position, 36);
    }
    return 1;
}

uint32 Player::UpdateGroundContact()
{
    const uint32 a1 = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005F470, "SLUS_003.73");
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;

    union
    {
        uint32 words[35];

        struct
        {
            sint32 origin[3], target[3];
            uint8 workspace[0x68 - 6 * sizeof(sint32)];
            uint32 hit;
            sint32 position[3];
            uint16 rotation[3];
        } fields;
    } ray;

    result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(460))))) & 0x3000);
    if (result)
        goto LABEL_8;
    if (((r_u16(((uint32)(a1))) & 8) != 0))
    {
        v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 1);
        v4 = r_u32(0x800FF37C);
        v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
        result = -4096;
        w_u16(((uint32)(((uint32)(a1) + (uint32)(164)))), 0);
        w_u16(a1 + 166, (uint32)-4096);
        w_u16(a1 + 168, 0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(184)))), v3);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))), v4);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(192)))), v5);
        return result;
    }
    if (((r_u16(((uint32)(((uint32)(a1) + (uint32)(216))))) & 2) != 0))
    {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 1);
        result = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(582)))))) << (uint32)(12));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))), ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))) + (uint32)(result)));
        return result;
    }
    ray.fields.origin[0] = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));
    ray.fields.origin[1] = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))) - (uint32)(0x40000));
    v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
    ray.fields.target[0] = ray.fields.origin[0];
    ray.fields.target[1] = ((uint32)(ray.fields.origin[1]) + (uint32)(0x800000));
    ray.fields.origin[2] = v6;
    ray.fields.target[2] = v6;
    InitializeCollisionRay((uint8 *)&ray);
    QueryCollisionRay((uint8 *)&ray, 1);
    result = 1;
    if (!ray.fields.hit)
    {
    LABEL_8:
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 0);
    }
    else
    {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 1);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(184)))), ray.fields.position[0]);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))), ray.fields.position[1]);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(192)))), ray.fields.position[2]);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(164)))), ray.fields.rotation[0]);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(166)))), ray.fields.rotation[1]);
        result = ray.fields.rotation[2];
        w_u16(((uint32)(((uint32)(a1) + (uint32)(168)))), ray.fields.rotation[2]);
    }
    return result;
}

void Player::SetWeaponStop(uint32 duration)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 selected;
    FUNCTION_MARKER(0x8005F62C, "SLUS_003.73");
    selected = r_u32(object + 644);
    w_u32(object + 456, duration);
    StopSelectedWeapon(selected);
}

uint32 Player::Update()
{
    const uint32 a1 = NativeObjectGuestAddress(this);
    uint32 callback_table, callback_receiver, callback_target;
    uint32 movement_x, movement_z;
    uint32 camera_address;
    sint16 camera_angles[3];
    sint16 weapon_angles[4];
    uint32 first_aim, second_aim, yaw_model, body_heading, model_heading;
    FUNCTION_MARKER(0x8005F688, "SLUS_003.73");
    /* TODO Recover GTE register carriers and missing host GTE adapters */

    sint32 v1;
    sint32 v3;
    sint32 v4;
    sint32 result;
    sint32 v6;
    sint32 v7;
    sint8 v8;
    sint32 v9;
    sint8 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    short v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    uint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    uint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 v35;
    uint32 v36;
    sint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    sint32 v58;
    sint32 v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    sint32 v63;
    sint32 v64;
    sint32 v65;
    sint32 v66;
    sint32 v67;
    short v68;
    short v69;
    sint32 v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    sint32 v75;
    sint32 v76;
    sint32 v77;
    sint32 v78;
    sint32 v79;
    sint32 v80;
    short v81;
    sint32 v82;
    sint32 v83;
    sint32 v84;
    sint32 v85;
    sint32 v86;
    sint32 v87;
    sint32 v88;
    sint32 v89;
    sint32 v90;
    sint32 v91;
    sint32 v92;
    sint32 v93;
    sint32 v94;
    sint32 v95;
    sint32 v96;
    sint32 v97;
    sint32 v98;
    sint32 v99;
    sint32 v100;
    sint32 v101;
    sint32 v102;
    sint32 v103;
    sint32 v104;
    sint32 v105;
    sint32 v106;
    sint32 v107;
    sint32 v108;
    sint32 v109;
    sint32 v110;
    sint32 v111;
    sint32 v112;
    sint32 v113;
    sint32 v114;
    sint32 v115;
    sint32 v116;
    sint32 v117;
    sint32 v118;
    sint32 v119;
    sint32 v120;
    sint32 v121;
    sint32 v122;
    sint32 v123;
    short v124;
    sint32 v125;
    sint32 v126;
    sint32 v127;
    sint32 v128;
    sint32 v129;
    sint32 v130;
    sint32 v131;
    sint32 v132;
    sint32 v133;
    sint32 v134;
    sint32 v135;
    uint32 v136;
    sint32 v137;
    sint32 v138;
    uint32 v139;
    sint32 v140;
    sint32 v141;
    uint32 v142;
    uint32 v143;
    uint32 v144;
    uint32 v145;
    uint32 v146;
    uint32 v147;
    sint32 v148;
    sint32 v149;
    sint32 v150;
    sint32 v151;
    short v152;
    short v153;
    sint32 v154;
    sint32 v156;
    short v157;
    short v158;
    short v159;
    sint32 v160;
    signed int v162;
    sint32 v164;
    sint32 v165;
    sint32 v166;
    sint32 v167;
    sint32 v168;
    sint32 v169;
    sint32 v170;
    sint32 v171;
    sint32 v172;
    sint32 v173;
    sint32 v174;
    sint32 v175;
    sint32 v176;
    sint32 v177;
    sint32 v178;
    sint32 v179;
    uint32 v180;
    sint32 v181;
    sint32 v182;
    sint32 v183;
    sint32 v184;
    sint32 v185;
    sint32 v186;
    sint32 v187;
    sint32 v188;
    sint32 v189;
    sint32 v190;
    sint32 v191;
    sint32 v192;
    sint32 v193;
    sint32 v194;
    sint32 v195;
    sint32 v196;
    sint32 v197;
    sint32 v198;
    sint32 v199;
    sint32 v200;
    sint32 v201;
    sint32 v202;
    sint8 v203;
    sint32 v204;
    sint32 v205;
    short v206;
    sint32 v207;
    sint32 v208;
    sint32 v209;
    sint32 v210;
    sint32 v211;
    sint32 v212;
    sint32 v213;
    sint32 v214;
    sint32 v215;
    sint32 position[4];
#define v216 position[0]
#define v217 position[1]
#define v218 position[2]
#define v219 position[3]
    MATRIX v220;
    MATRIX v221;
    sint32 vectors[7];
#define v222 vectors[0]
#define v223 vectors[1]
#define v224 (vectors + 2)
#define v225 vectors[4]
    sint32 collision_record[35];
#define v227 collision_record
#define v228 collision_record[2]
#define v229 collision_record[3]
#define v230 collision_record[4]
#define v231 collision_record[5]
#define v232 ((char *)(collision_record + 6))
#define v233 collision_record[26]
    sint32 v234[35];
    sint32 v236;
    sint32 v237;
    sint32 v238;
    sint32 v239;
    sint32 v240;
    sint32 v241;
    (v3 = r_u32(((uint32)((a1 + 456)))));
    (v4 = (v3 == 0));
    (result = (sint32)((uint32)v3 - 1));
    if (!v4)
    {
        w_u32(((uint32)((a1 + 456))), result);
        return result;
    }
    (v6 = r_u32(((uint32)((a1 + 428)))));
    if ((v6 && ((r_u16(((uint32)((v6 + 78)))) & 0x40) != 0)))
        w_u32(((uint32)((a1 + 428))), 0);
    w_u32(0x800FF970u, 0);
    (v7 = r_u8(((uint32)((a1 + 465)))));
    (v4 = (v7 == 0));
    (v8 = (v7 - 1));
    if (!v4)
        w_u8(((uint32)((a1 + 465))), v8);
    (v9 = r_u8(((uint32)((a1 + 466)))));
    (v4 = (v9 == 0));
    (v10 = (v9 - 1));
    if (!v4)
    {
        w_u8(((uint32)((a1 + 466))), v10);
        if (v10)
        {
            uint32 intensity = r_u8(a1 + 466);
            w_u32(a1 + 176, intensity | (intensity << 8) | 0x2000000 | (intensity << 16));
        }
        else
            w_u32(((uint32)((a1 + 176))), 0);
    }
    (v11 = r_u32(((uint32)((a1 + 452)))));
    (v4 = (v11 == 0));
    (v12 = (sint32)((uint32)v11 - 1));
    if (!v4)
    {
        (v13 = r_u32(((uint32)((a1 + 444)))));
        w_u32(((uint32)((a1 + 452))), v12);
        w_u8(((uint32)((v13 + 128))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 144))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 160))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 176))), 0);
        w_u8(r_u32(((uint32)((a1 + 444)))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 32))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 48))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 16))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 240))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 64))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 80))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 96))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 112))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 273))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 81))), 0);
    }
    (v14 = r_u8(((uint32)((a1 + 26)))));
    w_u32(((uint32)((a1 + 304))), 0x10000);
    if (((v14 == 5) && (r_u8(((uint32)((a1 + 297)))) == 1)))
        w_u32(((uint32)((a1 + 304))), 0x20000);
    if (r_u32(0x800FF328u))
    {
        (v15 = r_u32(((uint32)((a1 + 444)))));
        if (r_u8(((uint32)((v15 + 81)))))
        {
            w_u8(((uint32)((v15 + 81))), 0);
            sub_8005C338();
        }
    }
    (result = r_u8(((uint32)((a1 + 464)))));
    if (!result)
    {
        w_u16(((uint32)(a1)), (r_u16(((uint32)(a1))) | (4u)));
        uint32 saved_x = r_u32(a1 + 4);
        (v16 = r_u32(((uint32)((a1 + 8)))));
        (v17 = r_u32(((uint32)((a1 + 12)))));
        w_u32(a1 + 228, saved_x);
        w_u32(((uint32)((a1 + 232))), v16);
        w_u32(((uint32)((a1 + 236))), v17);
        (v19 = (r_u32(((uint32)((a1 + 460)))) & 0xF000));
        (v18 = r_u32(((uint32)((a1 + 108)))));
        w_u32(((uint32)((a1 + 32))), 8421504);
        if (!v19)
            ResolvePlayerCollisionMotion(a1);
        (v21 = (r_u16(((uint32)(a1))) & 0xFFF7));
        (v20 = r_u32(0x800FF378u));
        (v4 = (v20 == 1));
        w_u16(((uint32)(a1)), v21);
        uint32 water_contact = 0;
        if (v4 || v20 == 5)
        {
            uint32 offset = r_u8(a1 + 582);
            uint32 height = r_u32(a1 + 8);
            uint32 surface = r_u32(0x800FF37C);
            water_contact = (sint32)surface < (sint32)(height + (offset << 12));
        }
        if (water_contact)
        {
            (v22 = r_u32(0x800FF380u));
            (v4 = (v22 == 0));
            w_u16(((uint32)(a1)), (v21 | 8));
            if (v4)
            {
                if (((r_u32(0x800FF2F0u) & 3) == 0))
                {
                    uint32 surface = r_u32(0x800FF37C);
                    v216 = r_u32(a1 + 4);
                    v217 = r_u32(a1 + 8);
                    v218 = r_u32(a1 + 12);
                    v217 = surface;
                    (v24 = (sub_80066570(32) + 32));
                    (v25 = (0x800F863Cu + ((sub_80066570(4096) & 0xFFF)) * 4u));
                    v216 = (sint32)((uint32)v216 + (uint32)v24 * (uint32)(sint32)(sint16)r_u16(v25));
                    v218 = (sint32)((uint32)v218 + (uint32)v24 * (uint32)(sint32)(sint16)r_u16(v25 + 2));
                    (v26 = AllocateEffectStorage(116));
                    if (v26)
                    {
                        (v28 = (sub_80066570(32) + 32));
                        (v27 = sub_80066570(4));
                        void *dram_addr = AllocateNativeObjectStorage(v26, sizeof(ExpandingRing));
                        new (dram_addr) ExpandingRing(position, v28, 16, 64, 16, 12, (uint32)v27 + 4);
                    }
                }
            }
            else
            {
                callback_table = r_u32(a1 + 68);
                callback_receiver = a1 + (uint32)(sint32)(sint16)r_u16(callback_table + 48);
                callback_target = r_u32(callback_table + 52);
                apocalypse_object_virtual52(callback_target, callback_receiver, v22, 0x800A71CC, 0);
            }
        }
        if (((r_u32(0x800FF32Cu) && r_u8(0x800EC138u)) && r_u8(0x800EC158u)))
        {
            (v29 = r_u32(((uint32)((a1 + 232)))));
            w_u32(((uint32)((a1 + 8))), v29);
            if (r_u8(0x800EC0F8u))
                w_u32(((uint32)((a1 + 8))), (v29 - 393216));
            if (r_u8(0x800EC128u))
                w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + (393216)));
            (v30 = (r_u32(((uint32)((a1 + 12)))) + (5 * (r_u32(((uint32)((a1 + 12)))) - r_u32(((uint32)((a1 + 236))))))));
            w_u32(((uint32)((a1 + 4))), (r_u32(((uint32)((a1 + 4)))) + ((5 * (r_u32(((uint32)((a1 + 4)))) - r_u32(((uint32)((a1 + 228)))))))));
            w_u32(((uint32)((a1 + 12))), v30);
        }
        (v31 = r_u16(((uint32)((a1 + 216)))));
        w_u16(((uint32)((a1 + 212))), 100);
        w_u16(((uint32)((a1 + 20))), 0);
        w_u16(((uint32)((a1 + 16))), 0);
        if ((((((unsigned char)((v31 >> 1))) ^ 1) & 1) != 0))
        {
            w_u16(a1 + 570, r_u16(a1 + 570) + 1);
        }
        else
        {
            w_u16(((uint32)((a1 + 570))), 0);
            w_u8(((uint32)((a1 + 569))), 0);
        }
        if ((((!r_u32(0x800FF32Cu) && (r_u8(((uint32)((a1 + 26)))) == 17)) && r_u8(((uint32)((a1 + 303))))) && !(r_u32(((uint32)((a1 + 484)))))))
            sub_8005E708(a1);
        ReadInput();
        if ((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))))
        {
            (v33 = r_u32(((uint32)((a1 + 608)))));
            if ((v33 < 8))
                w_u32(((uint32)((a1 + 608))), (v33 + 1));
        }
        else
        {
            (v34 = r_u32(((uint32)((a1 + 608)))));
            (v4 = (v34 == 0));
            (v35 = (sint32)((uint32)v34 - 1));
            if (!v4)
                w_u32(((uint32)((a1 + 608))), v35);
        }
        (v36 = r_u32(((uint32)((a1 + 460)))));
        w_u8(((uint32)((a1 + 572))), 1);
        if ((v36 == 512))
        {
            w_u16(((uint32)((a1 + 580))), 0);
            (v106 = TryEnterMovementState16());
            (v37 = 0);
            if (v106)
                goto LABEL_232;
            (v107 = TryEnterMovementState32());
            (v37 = 0);
            if (v107)
                goto LABEL_232;
            (v108 = TryEnterMovementState1024());
            goto LABEL_213;
        }
        if ((v36 < 0x201))
        {
            if ((v36 == 8))
            {
                w_u16(((uint32)((a1 + 580))), 0);
                (v83 = TryEnterMovementState16());
                (v37 = 0);
                if (v83)
                    goto LABEL_232;
                (v84 = TryEnterMovementState32());
                (v37 = 0);
                if (v84)
                    goto LABEL_232;
                (v85 = TryEnterMovementState512());
                (v37 = 0);
                if (v85)
                    goto LABEL_232;
                (v86 = TryEnterMovementState1024());
                (v37 = 0);
                if (v86)
                    goto LABEL_232;
                (v87 = TryEnterJumpState());
                (v37 = 0);
                if (v87)
                    goto LABEL_232;
                (v88 = TryEnterAirborneState());
                (v37 = 0);
                if (v88)
                    goto LABEL_232;
                (v89 = TryEnterActionState());
                (v37 = 0);
                if (v89)
                    goto LABEL_232;
                (v90 = sub_8005ED28());
                (v37 = 0);
                if ((v90 || !(r_u8(((uint32)((a1 + 303)))))))
                    goto LABEL_232;
            LABEL_221:
                sub_8005E9E4(a1);

                (v37 = 0);
                goto LABEL_232;
            }
            if ((v36 < 9))
            {
                if ((v36 != 2))
                {
                    if ((v36 >= 3))
                    {
                        if ((v36 == 4))
                        {
                            (v4 = (r_u16(((uint32)((a1 + 570)))) < 0x31u));
                            w_u16(((uint32)((a1 + 580))), 0);
                            if ((v4 || (r_u8(((uint32)((a1 + 26)))) != 2)))
                            {
                                (v74 = r_u8(((uint32)((a1 + 26)))));
                                if ((v74 != 17))
                                {
                                    (v75 = 0);
                                    if ((((sint32)(r_u32(((uint32)((a1 + 108)))) ^ (uint32)v18) < 0) || (v74 != 2)))
                                        (v75 = 1);
                                    if (v75)
                                    {
                                        w_u32(((uint32)((a1 + 480))), r_u32(((uint32)((a1 + 8)))));
                                        SetAnimationRange(2, ((sint32)(sint16)r_u16(0x800EC4D4u) + 1), (sint32)(sint16)r_u16(0x800EC4D6u));
                                        w_u8(((uint32)((a1 + 130))), r_u8(0x800EC6DAu));
                                    }
                                }
                            }
                            else
                            {
                                SetAnimationRange(17, 0, -1);
                            }
                            (v76 = TryEnterLandingState());
                            (v37 = 0);
                            if (!v76)
                            {
                                (v77 = TryEnterRecordMovementState());
                                (v37 = 0);
                                if (!v77)
                                {
                                    TryEnterAnimation10State();
                                    (v37 = 0);
                                }
                            }
                        }
                        else
                        {
                            (v37 = 0);
                        }
                    }
                    else
                    {
                        (v37 = 0);
                        if ((v36 == 1))
                        {
                            (v38 = TryEnterMovementState16());
                            (v37 = 0);
                            if (!v38)
                            {
                                (v39 = TryEnterMovementState32());
                                (v37 = 0);
                                if (!v39)
                                {
                                    (v40 = TryEnterMovementState512());
                                    (v37 = 0);
                                    if (!v40)
                                    {
                                        (v41 = TryEnterMovementState1024());
                                        (v37 = 0);
                                        if (!v41)
                                        {
                                            (v42 = TryEnterJumpState());
                                            (v37 = 0);
                                            if (!v42)
                                            {
                                                (v43 = TryEnterAirborneState());
                                                (v37 = 0);
                                                if (!v43)
                                                {
                                                    (v44 = TryEnterActionState());
                                                    (v37 = 0);
                                                    if (!v44)
                                                    {
                                                        (v45 = TryActivateOneShotActivator());
                                                        (v37 = 0);
                                                        if (!v45)
                                                        {
                                                            (v46 = TryActivateResettingActivator());
                                                            (v37 = 0);
                                                            if (!v46)
                                                            {
                                                                (v47 = sub_8005ED28());
                                                                (v37 = 0);
                                                                if (!v47)
                                                                {
                                                                    if (r_u8(((uint32)((a1 + 303)))))
                                                                        SetAnimationRange(0, 0, -1);
                                                                    v48 = (r_u16(a1 + 580) + 1) & 65535;
                                                                    w_u16(a1 + 580, v48);
                                                                    if ((v48 == 1350))
                                                                    {
                                                                        (v49 = sub_80066570(8));
                                                                        sub_8002FD2C(8, v49);
                                                                        (v37 = 0);
                                                                        goto LABEL_232;
                                                                    }
                                                                    (v37 = 0);
                                                                    if ((v48 == 4050))
                                                                    {
                                                                        (v50 = sub_80066570(8));
                                                                        sub_8002FC64(8, v50, (a1 + 4), 0);
                                                                        w_u16(((uint32)((a1 + 580))), 1350);
                                                                    LABEL_231:
                                                                        (v37 = 0);

                                                                        goto LABEL_232;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    goto LABEL_232;
                }
                w_u16(((uint32)((a1 + 580))), 0);
                (v57 = TryEnterRecordMovementState());
                (v37 = 0);
                if (v57)
                    goto LABEL_232;
                if (TryEnterAnimation10State())
                    goto LABEL_231;
                if (!(r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 272))))))
                    w_u32(0x800FF5A8u, 1);
                (v58 = 4);
                if (r_u8(((uint32)((a1 + 568)))))
                {
                    (v37 = 0);
                    if (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 272)))))
                        goto LABEL_232;
                    (v58 = 4);
                }
            LABEL_230:
                w_u32(((uint32)((a1 + 460))), v58);

                goto LABEL_231;
            }
            if ((v36 == 32))
            {
                w_u16(((uint32)((a1 + 580))), 0);
                (v99 = TryEnterMovementState512());
                (v37 = 0);
                if (v99)
                    goto LABEL_232;
                (v100 = TryEnterMovementState1024());
                (v37 = 0);
                if (v100)
                    goto LABEL_232;
                (v101 = TryEnterJumpState());
                (v37 = 0);
                if (v101)
                    goto LABEL_232;
                (v102 = TryEnterAirborneState());
                (v37 = 0);
                if (v102)
                    goto LABEL_232;
                (v103 = TryEnterActionState());
                (v37 = 0);
                if (v103)
                    goto LABEL_232;
                (v104 = TryActivateOneShotActivator());
                (v37 = 0);
                if (v104)
                    goto LABEL_232;
                (v105 = TryActivateResettingActivator());
                (v37 = 0);
                if (v105)
                    goto LABEL_232;
                (v98 = ((sint8)(r_u8(((uint32)((a1 + 468)))))));
                goto LABEL_220;
            }
            if ((v36 < 0x21))
            {
                (v37 = 0);
                if ((v36 != 16))
                    goto LABEL_232;
                w_u16(((uint32)((a1 + 580))), 0);
                (v91 = TryEnterMovementState512());
                (v37 = 0);
                if (v91)
                    goto LABEL_232;
                (v92 = TryEnterMovementState1024());
                (v37 = 0);
                if (v92)
                    goto LABEL_232;
                (v93 = TryEnterJumpState());
                (v37 = 0);
                if (v93)
                    goto LABEL_232;
                (v94 = TryEnterAirborneState());
                (v37 = 0);
                if (v94)
                    goto LABEL_232;
                (v95 = TryEnterActionState());
                (v37 = 0);
                if (v95)
                    goto LABEL_232;
                (v96 = TryActivateOneShotActivator());
                (v37 = 0);
                if (v96)
                    goto LABEL_232;
                (v97 = TryActivateResettingActivator());
                (v37 = 0);
                if (v97)
                    goto LABEL_232;
                (v98 = ((sint8)(r_u8(((uint32)((a1 + 468)))))));
            LABEL_220:
                (v37 = 0);

                if (v98)
                    goto LABEL_232;
                goto LABEL_221;
            }
            if ((v36 == 128))
            {
                (v54 = r_u8(((uint32)((a1 + 303)))));
                w_u16(((uint32)((a1 + 580))), 0);
                if (v54)
                {
                    (v55 = r_u32(((uint32)((a1 + 428)))));
                    (v56 = (sint32)(0u - ((uint32)(sint32)(sint16)r_u16(0x800EC4DAu) << 12)));
                    w_u32(((uint32)((a1 + 564))), v56);
                    if (v55)
                        w_u32(((uint32)((a1 + 564))), (v56 + r_u32(((uint32)((v55 + 108))))));
                    w_u32(((uint32)((a1 + 460))), 2);
                    w_u8(((uint32)((a1 + 568))), r_u8(0x800EC4D8u));
                }
                goto LABEL_231;
            }
            (v37 = 0);
            if ((v36 != 256))
                goto LABEL_232;
            (v51 = TryEnterJumpState());
            (v37 = 0);
            if (v51)
                goto LABEL_232;
            w_u16(((uint32)((a1 + 580))), 0);
            w_u16(((uint32)((a1 + 212))), 50);
            (v52 = TryEnterActionState());
            (v37 = 0);
            if (v52)
                goto LABEL_232;
            (v32 = 21);
            if (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 256)))))
                goto LABEL_232;
            (v53 = a1);
            if ((((sint8)(r_u8(((uint32)((a1 + 24)))))) < 21))
                (v32 = ((sint8)(r_u8(((uint32)((a1 + 24)))))));
        LABEL_229:
            Player::Attach(v53).SetAnimationRange(5, v32, 0);

            (v58 = 1);
            goto LABEL_230;
        }
        if ((v36 == 0x4000))
        {
            (v70 = r_u8(((uint32)((a1 + 26)))));
            w_u16(((uint32)((a1 + 580))), 0);
            if ((v70 != 13))
                goto LABEL_231;
            (v71 = DidAnimationCrossFrame(13, 3));
            if ((UpdateEventLatch(v71, 1) || ((v72 = DidAnimationCrossFrame(13, 33)), (v73 = UpdateEventLatch(v72, 1)), (v37 = 0), v73)))
            {
                ApplyRandomModelOffset(r_u32(a1 + 512), 64);
                (v37 = 0);
            }
            goto LABEL_232;
        }
        if ((v36 < 0x4001))
        {
            if ((v36 == 2048))
            {
                (v116 = TryEnterJumpState());
                (v37 = 0);
                if (v116)
                    goto LABEL_232;
                w_u16(((uint32)((a1 + 580))), 0);
                w_u16(((uint32)((a1 + 212))), 50);
                (v117 = TryEnterAirborneState());
                (v37 = 0);
                if (v117)
                    goto LABEL_232;
                (v118 = ((sint8)(r_u8(((uint32)((a1 + 24)))))));
                if (((v118 < 21) && (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 256)))) || (v118 < 11))))
                    goto LABEL_232;
                (v58 = 256);
                if (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 256)))))
                    goto LABEL_230;
                (v53 = a1);
                (v32 = 21);
                goto LABEL_229;
            }
            if ((v36 >= 0x801))
            {
                if ((v36 == 4096))
                {
                    w_u16(((uint32)((a1 + 580))), 0);
                    w_u32(((uint32)((a1 + 460))), 0x2000);
                    w_u32(((uint32)((a1 + 304))), 111408);
                    SetAnimationRange(11, 0, -1);
                    (v37 = 0);
                    goto LABEL_232;
                }
                (v37 = 0);
                if ((v36 == 0x2000))
                {
                    (v78 = r_u8(((uint32)((a1 + 303)))));
                    w_u16(((uint32)((a1 + 580))), 0);
                    w_u32(((uint32)((a1 + 304))), 111408);
                    if (!v78)
                        goto LABEL_231;
                    SetAnimationRange(0, 0, -1);
                    (v236 = 128);
                    xport_draft_host_sub_8006C47C_p12(position, &v236, a1 + 552);
                    xport_draft_host_sub_8006C0FC_p2(a1 + 4, position);
                    (v79 = r_u32(((uint32)((a1 + 444)))));
                    w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) - (884736)));
                    w_u8(((uint32)((v79 + 273))), 0);
                    (v80 = sub_80067A18((a1 + 4), 0, 2048));
                    if ((v80 != -1))
                    {
                        (v81 = (r_u16(((uint32)((a1 + 216)))) | 2));
                        w_u32(((uint32)((a1 + 8))), (v80 - (r_u8(((uint32)((a1 + 582)))) << 12)));
                        w_u16(((uint32)((a1 + 216))), v81);
                    }
                    w_u8(((uint32)((a1 + 578))), 4);
                    (v82 = r_u32(0x800FF904u));
                    w_u32(((uint32)((a1 + 460))), 1);
                    w_u32(((uint32)((a1 + 112))), 0);
                    w_u32(((uint32)((a1 + 104))), 0);
                    sub_8007823C(v82);
                    (v37 = 0);
                }
                goto LABEL_232;
            }
            (v37 = 0);
            if ((v36 != 1024))
                goto LABEL_232;
            w_u16(((uint32)((a1 + 580))), 0);
            (v109 = TryEnterMovementState16());
            (v37 = 0);
            if (v109)
                goto LABEL_232;
            (v110 = TryEnterMovementState32());
            (v37 = 0);
            if (v110)
                goto LABEL_232;
            (v108 = TryEnterMovementState512());
        LABEL_213:
            (v37 = 0);

            if (v108)
                goto LABEL_232;
            (v111 = TryEnterJumpState());
            (v37 = 0);
            if (v111)
                goto LABEL_232;
            (v112 = TryEnterAirborneState());
            (v37 = 0);
            if (v112)
                goto LABEL_232;
            (v113 = TryEnterActionState());
            (v37 = 0);
            if (v113)
                goto LABEL_232;
            (v114 = TryActivateOneShotActivator());
            (v37 = 0);
            if (v114)
                goto LABEL_232;
            (v115 = TryActivateResettingActivator());
            (v37 = 0);
            if (v115)
                goto LABEL_232;
            (v98 = ((sint8)(r_u8(((uint32)((a1 + 469)))))));
            goto LABEL_220;
        }
        if ((v36 == 0x10000))
        {
            (v59 = r_u32(((uint32)((a1 + 516)))));
            (v60 = r_u32(((uint32)((a1 + 4)))));
            w_u16(((uint32)((a1 + 580))), 0);
            (v61 = r_u32(((uint32)((v59 + 296)))));
            if ((sint32)Abs32((sint32)((uint32)v61 - (uint32)v60) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 4))), v61);
            else
                w_u32(((uint32)((a1 + 4))), ((uint32)v60 + ((sint32)((uint32)v61 - (uint32)v60) >> 3)));
            (v62 = r_u32(((uint32)((a1 + 12)))));
            (v32 = r_u32(((uint32)((r_u32(((uint32)((a1 + 516)))) + 304)))));
            if ((sint32)Abs32((sint32)((uint32)v32 - (uint32)v62) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 12))), v32);
            else
                w_u32(((uint32)((a1 + 12))), ((uint32)v62 + ((sint32)((uint32)v32 - (uint32)v62) >> 3)));
            if (((((uint32)(r_u8(((uint32)((a1 + 24)))))) - 25) < 0x19))
            {
                (v63 = r_u32(((uint32)((a1 + 516)))));
                if (v63)
                {
                    w_u16(((uint32)((v63 + 20))), ((r_u16(((uint32)((v63 + 20)))) - 28) & 0xFFF));
                    if ((r_u8(((uint32)((a1 + 24)))) == 32))
                        OneShotActivator::Attach(r_u32(a1 + 516u)).SpawnObjects();
                }
            }
            if (r_u8(((uint32)((a1 + 303)))))
                w_u32(((uint32)((a1 + 460))), 1);
            goto LABEL_231;
        }
        if ((v36 <= 0x10000))
        {
            (v37 = 0);
            if ((v36 == 0x8000))
            {
                w_u16(((uint32)((a1 + 580))), 0);
                goto LABEL_231;
            }
        LABEL_232:
            (v119 = 0);

            if ((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))))
            {
                (v120 = r_u16(((uint32)((a1 + 474)))));
                (v121 = ((uint32)v120 << 16));
                if ((((uint32)((v120 - 257))) < 0xDFF))
                {
                    (v120 = ((short)(v120)));
                    (v122 = ((v121 >> 16) - 256));
                    (v4 = (v122 >= 0));
                    (v123 = (v122 >> 9));
                    if (!v4)
                        (v123 = ((v120 + 255) >> 9));
                    (v37 = (v123 + 2));
                }
                else
                {
                    (v37 = 1);
                }
            }
            (v124 = 0);
            if ((r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472))))))
            {
                (v125 = r_u16(((uint32)((a1 + 476)))));
                (v126 = ((uint32)v125 << 16));
                if ((((uint32)((v125 - 257))) < 0xDFF))
                {
                    (v125 = ((short)(v125)));
                    (v127 = ((v126 >> 16) - 256));
                    (v4 = (v127 >= 0));
                    (v128 = (v127 >> 9));
                    if (!v4)
                        (v128 = ((v125 + 255) >> 9));
                    (v119 = (v128 + 2));
                    (v124 = 0);
                }
                else
                {
                    (v119 = 1);
                }
            }
            (v129 = (v37 - 1));
            (v130 = (v119 - 1));
            camera_address = r_u32(0x800FF904);
            if (camera_address)
                v124 = r_u16(camera_address + 494);
            (v131 = r_u32(((uint32)((a1 + 460)))));
            if (((v131 & 0x630) != 0))
            {
                if (v119)
                {
                    (v136 = (((uint32)((0x800A6FD8u + ((12 * v129)) * 4u))) + (v130) * 1u));
                    (v137 = r_u8(v136));
                    (v138 = ((sint8)(r_u8((v136 + (8) * 1u)))));
                    if (((v137 != r_u8(((uint32)((a1 + 26))))) || ((v132 = a1), (v138 != ((sint8)(r_u8(((uint32)((a1 + 297))))))))))
                    {
                        StartAnimation(v137, v138);
                        (v132 = a1);
                    }
                    (v134 = 0);
                    (v139 = (((uint32)((0x800A6FD8u + ((12 * v129)) * 4u))) + ((2 * v130)) * 1u));
                    (v133 = ((sint16)(r_u16((((uint32)(v139)) + (16) * 2u)))));
                    uint32 model_offset = r_u16(v139 + 16);
                    uint32 movement_heading = r_u16(a1 + 474);
                    v135 = (sint16)((uint32)v124 + model_offset + movement_heading - ((uint32)v129 << 9));
                }
                else
                {
                    (v132 = a1);
                    if (((r_u8(((uint32)((a1 + 26)))) != 1) || ((v133 = 0), (r_u8(((uint32)((a1 + 297)))) != 1))))
                    {
                        StartAnimation(1, 1);
                        (v132 = a1);
                        (v133 = 0);
                    }
                    (v134 = 0);
                    (v135 = ((short)((v124 + r_u16(((uint32)((a1 + 474))))))));
                }
                goto LABEL_296;
            }
            if ((v131 != 1))
            {
                if ((v131 == 256))
                {
                    (v132 = a1);
                    if (v119)
                    {
                        (v143 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 0);
                        (v133 = (((r_u16((((uint32)(v143)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v143)) + (224) * 2u))))));
                    }
                    else
                    {
                        (v133 = 0);
                        (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                        (v134 = 0);
                    }
                    goto LABEL_296;
                }
                if (((v131 & 0x8E) != 0))
                {
                    (v132 = a1);
                    if (v119)
                    {
                        (v144 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 0);
                        (v133 = (((r_u16((((uint32)(v144)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v144)) + (224) * 2u))))));
                    }
                    else
                    {
                        (v133 = 0);
                        (v134 = 0);
                        (v135 = ((short)((v124 + r_u16(((uint32)((a1 + 474))))))));
                    }
                    goto LABEL_296;
                }
                if ((v131 == 0x4000))
                {
                    if ((v119 || !((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))))))
                    {
                        if ((r_u8(((uint32)((a1 + 26)))) != 14))
                            SetAnimationRange(14, 0, -1);
                        (v132 = a1);
                        if (!v119)
                        {
                        LABEL_297:
                            if (r_u32(((uint32)((a1 + 440)))))
                            {
                                uint32 pose = r_u32(a1 + 360);
                                uint32 step = r_u16(a1 + 436);
                                uint32 yaw = r_u16(pose + 86);
                                w_u16(pose + 86, (yaw + step) & 4095);
                                uint32 remaining = r_u32(a1 + 440) - 1;
                                w_u32(a1 + 440, remaining);
                                v4 = remaining != 0;
                                if (!v4)
                                {
                                    pose = r_u32(a1 + 360);
                                    uint32 target = r_u16(a1 + 432);
                                    w_u16(pose + 86, target);
                                }
                            }

                            (v147 = r_u32(((uint32)((a1 + 460)))));
                            w_u32(((uint32)((a1 + 124))), 0);
                            w_u32(((uint32)((a1 + 116))), 0);
                            if ((v147 != 0x4000))
                            {
                                if ((v147 >= 0x4001))
                                {
                                    if ((v147 != 0x8000))
                                    {
                                    LABEL_306:
                                        w_u32(((uint32)((a1 + 120))), ((uint32)(sint32)(sint16)r_u16(0x800EC4B0u) << 12));

                                    LABEL_307:
                                        if (((r_u32(((uint32)((a1 + 460)))) & 0x80000) == 0))
                                        {
                                            (v152 = r_u16(0x800FF5E8u));
                                            w_u32(a1 + 132u, r_u32(0x800FF5E4u));
                                            w_u16(((uint32)((a1 + 136))), v152);
                                            (v153 = r_u16(0x800FF5E8u));
                                            w_u32(a1 + 138u, r_u32(0x800FF5E4u));
                                            w_u16(((uint32)((a1 + 142))), v153);
                                        }

                                        (v154 = 0);
                                        camera_address = r_u32(0x800FF904);
                                        if (camera_address)
                                        {
                                            camera_angles[0] = r_u16(camera_address + 492);
                                            camera_angles[1] = r_u16(camera_address + 494);
                                            camera_angles[2] = r_u16(camera_address + 496);
                                            RotMatrix((SVECTOR *)camera_angles, &v221);
                                        }
                                        else
                                        {
                                            // Use the approved identity transform when the camera is absent
                                            memset(&v221, 0, sizeof(v221));
                                            v221.m[0][0] = 4096;
                                            v221.m[1][1] = 4096;
                                            v221.m[2][2] = 4096;
                                        }
                                        {
                                            uint32 r, c;
                                            for (r = 0; r < 3u; ++r)
                                                for (c = 0; c < 3u; ++c)
                                                    v220.m[r][c] = v221.m[c][r];
                                        }
                                        SetRotMatrix(&v220);
                                        (v237 = 6);
                                        xport_draft_host_sub_8006C564_p13(v224, a1 + 104u, &v237);

                                        (v216 = v224[0]);
                                        (v217 = v224[1]);
                                        (v218 = v225);
                                        xport_gte_write_data(9u, (uint32)v216);
                                        xport_gte_write_data(10u, (uint32)v217);
                                        xport_gte_write_data(11u, (uint32)v218);
                                        xport_gte_execute(0x49E012u);
                                        v216 = (sint32)xport_gte_read_data(25u);
                                        v217 = (sint32)xport_gte_read_data(26u);
                                        v218 = (sint32)xport_gte_read_data(27u);
                                        (v238 = 6);
                                        xport_draft_host_sub_8006C22C_p12(&v216, &v238);
                                        (v156 = r_u32(((uint32)((a1 + 460)))));
                                        if (((v156 & 0x71100) == 0))
                                        {
                                            switch (v156)
                                            {
                                                case 0x800:
                                                    if ((((sint8)(r_u8(((uint32)((a1 + 24)))))) < 20))
                                                    {
                                                        v216 = (sint32)(0u - ((uint32)(sint32)(sint16)r_u16(0x800EC4C0u) * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 606u) & 0xFFFu))));
                                                        (v154 = 1);
                                                        v218 = (sint32)(0u - ((uint32)(sint32)(sint16)r_u16(0x800EC4C0u) * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 606u) & 0xFFFu) + 2u)));
                                                    }
                                                    break;

                                                case 0x4000:
                                                    if ((!((r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472)))))) && r_u16(((uint32)((a1 + 468))))))
                                                    {
                                                        if (((((r_u16(((uint32)((r_u32(((uint32)((a1 + 512)))) + 320)))) - (v124 + r_u16(((uint32)((a1 + 474)))))) & 0xFFFu) - 1025) < 0x7FF))
                                                        {
                                                            (v159 = r_u16(0x800EC4C0u));
                                                            (v160 = r_u32(((uint32)((a1 + 512)))));
                                                            w_u32(((uint32)((a1 + 304))), 0x20000);
                                                            (v1 = (v159 / 4));
                                                            (v158 = ((r_u16(((uint32)((v160 + 320)))) + 2048) & 0xFFF));
                                                        }
                                                        else
                                                        {
                                                            (v157 = r_u16(0x800EC4C0u));
                                                            w_u32(((uint32)((a1 + 304))), 0x20000);
                                                            (v158 = r_u16(((uint32)((r_u32(((uint32)((a1 + 512)))) + 320)))));
                                                            (v1 = (v157 / -4));
                                                        }
                                                        w_u16(((uint32)((a1 + 18))), v158);
                                                        (v154 = 1);
                                                    }
                                                    break;

                                                case 0x8000:
                                                    v216 = ((sint32)(sint16)r_u16(0x800EC4C0u) / 4) * 8u * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * ((r_u16(r_u32(a1 + 512u) + 320u) - v124) & 0xFFFu));
                                                    (v154 = 1);
                                                    v218 = ((sint32)(sint16)r_u16(0x800EC4C0u) / 4) * 8u * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * ((r_u16(r_u32(a1 + 512u) + 320u) - v124) & 0xFFFu) + 2u);
                                                    break;

                                                default:
                                                    movement_x = r_u8(a1 + 468);
                                                    movement_z = r_u8(a1 + 469);

                                                    if (movement_x | movement_z)
                                                    {
                                                        v162 = Abs32(((sint32)(sint8)movement_x * 4096) / 96) + Abs32(((sint32)(sint8)movement_z * 4096) / 96);
                                                        if ((v162 >= 4097))
                                                            (v162 = 4096);
                                                        if (movement_x)
                                                        {
                                                            v218 = (sint32)(0u - ((uint32)((sint32)((uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 474u) & 0xFFFu) + 2u) * (uint32)v162) >> 12) * (uint32)(sint32)(sint16)r_u16(0x800EC4C0u)));
                                                            (v218 = ((sint32)((uint32)v218 * r_u32(((uint32)((a1 + 608))))) / 8));
                                                            (v154 = 1);
                                                        }
                                                        if (r_u8(((uint32)((a1 + 469)))))
                                                        {
                                                            v216 = (sint32)(0u - ((uint32)((sint32)((uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 474u) & 0xFFFu)) * (uint32)v162) >> 12) * (uint32)(sint32)(sint16)r_u16(0x800EC4C0u)));
                                                            (v216 = ((sint32)((uint32)v216 * r_u32(((uint32)((a1 + 608))))) / 8));
                                                            (v154 = 1);
                                                        }
                                                    }
                                                    else if (!(r_u32(((uint32)((a1 + 528))))))
                                                    {
                                                        (v154 = 1);
                                                        (v216 = 0);
                                                        (v218 = 0);
                                                    }
                                                    break;
                                            }
                                        }
                                        SetRotMatrix(&v221);
                                        if (v154)
                                        {
                                            (v239 = 6);
                                            xport_draft_host_sub_8006C564_p123(&v222, &v216, &v239);

                                            (v164 = v223);
                                            (v165 = v224[0]);
                                            w_u32(((uint32)((a1 + 104))), v222);
                                            w_u32(((uint32)((a1 + 108))), v164);
                                            w_u32(((uint32)((a1 + 112))), v165);
                                            xport_gte_write_data(9u, r_u32(a1 + 104u));
                                            xport_gte_write_data(10u, r_u32(a1 + 108u));
                                            xport_gte_write_data(11u, r_u32(a1 + 112u));
                                            xport_gte_execute(0x49E012u);
                                            w_u32(a1 + 104u, xport_gte_read_data(25u));
                                            w_u32(a1 + 108u, xport_gte_read_data(26u));
                                            w_u32(a1 + 112u, xport_gte_read_data(27u));
                                            (v240 = 6);
                                            xport_draft_host_sub_8006C22C_p2(a1 + 104u, &v240);
                                            (v166 = r_u32(((uint32)((a1 + 460)))));
                                            if (((v166 & 0x8000) != 0))
                                            {
                                                sub_8006C0B8((a1 + 4), (a1 + 104));
                                                sub_8006C0B8((a1 + 496), (a1 + 104));
                                                if (!apocalypse_movement_19318_native(r_u32(a1 + 512), a1 + 496, (uint32 *)vectors) || r_u8(r_u32(a1 + 444u) + 273u))
                                                {
                                                    w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 273))), 0);
                                                    (v167 = r_u32(((uint32)((a1 + 492)))));
                                                    w_u32(((uint32)((a1 + 460))), 4);
                                                    w_u32(((uint32)((a1 + 508))), 0);
                                                    sub_8006A294(v167);
                                                    goto LABEL_347;
                                                }
                                            LABEL_342:
                                                (v168 = v223);

                                                (v169 = v224[0]);
                                                w_u32(((uint32)((a1 + 4))), v222);
                                                w_u32(((uint32)((a1 + 8))), v168);
                                                w_u32(((uint32)((a1 + 12))), v169);
                                                {
                                                    uint32 snapshot_x = r_u32(a1 + 4);
                                                    v170 = r_u32(a1 + 8);
                                                    v171 = r_u32(a1 + 12);
                                                    w_u32(a1 + 496, snapshot_x);
                                                    w_u32(a1 + 500, v170);
                                                    w_u32(a1 + 504, v171);
                                                }
                                                w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + (0x80000)));
                                                goto LABEL_347;
                                            }
                                            if (((v166 & 0x4000) == 0))
                                                goto LABEL_347;
                                            if (apocalypse_movement_1922C_native(r_u32(a1 + 512), a1 + 496, v1, (uint32 *)vectors) && !r_u8(r_u32(a1 + 444u) + 273u))
                                            {
                                                goto LABEL_342;
                                            }
                                            w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 273))), 0);
                                        }
                                        else
                                        {
                                            if ((((r_u32(((uint32)((a1 + 460)))) & 0xC000) == 0) || ((v172 = r_u32(((uint32)((a1 + 444))))), !(r_u8(((uint32)((v172 + 273))))))))
                                            {
                                            LABEL_347:
                                                if (!(r_u32(((uint32)((a1 + 428))))))
                                                    goto LABEL_360;

                                                (v173 = 0);
                                                {
                                                    uint32 snapshot_x = r_u32(a1 + 4);
                                                    v174 = r_u32(a1 + 8);
                                                    v175 = r_u32(a1 + 12);
                                                    v222 = snapshot_x;
                                                    v223 = v174;
                                                    v224[0] = v175;
                                                }
                                                if (((r_u32(((uint32)((a1 + 460)))) & 0x3000) != 0))
                                                {
                                                    sub_8006C0B8((a1 + 4), (r_u32(((uint32)((a1 + 428)))) + 104));
                                                    (v173 = 1);
                                                    goto LABEL_357;
                                                }
                                                if (r_u16(((uint32)((a1 + 570)))))
                                                {
                                                    if (((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))) || ((v176 = r_u32(((uint32)((a1 + 428))))), (v177 = r_u32(((uint32)((v176 + 104))))), !((v177 | r_u32(((uint32)((v176 + 112)))))))))
                                                    {
                                                    LABEL_357:
                                                        if (v173)
                                                        {
                                                            (v227[0] = v222);
                                                            (v227[1] = v223);
                                                            (v228 = v224[0]);
                                                            (v229 = r_u32(((uint32)((a1 + 4)))));
                                                            (v230 = r_u32(((uint32)((a1 + 8)))));
                                                            (v231 = r_u32(((uint32)((a1 + 12)))));
                                                            xport_draft_host_sub_8007BB24_p1(v227);
                                                            xport_draft_host_sub_8007DD04_p1(v227, 1u);
                                                            if (v233)
                                                            {
                                                                (v184 = v223);
                                                                (v185 = v224[0]);
                                                                w_u32(((uint32)((a1 + 4))), v222);
                                                                w_u32(((uint32)((a1 + 8))), v184);
                                                                w_u32(((uint32)((a1 + 12))), v185);
                                                            }
                                                        }

                                                    LABEL_360:
                                                        if (((r_u16(((uint32)((a1 + 216)))) & 0x100) != 0))
                                                            w_u32(0x800FF5A8u, 1);

                                                        if (((r_u8(((uint32)((a1 + 568)))) && r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 272))))) && !r_u32(0x800FF5A8u)))
                                                            w_u32(((uint32)((a1 + 108))), r_u32(((uint32)((a1 + 564)))));
                                                        (v186 = 74);
                                                        if (((r_u16(((uint32)(a1))) & 8) != 0))
                                                        {
                                                            if ((r_u32(0x800FF5D4u) == 74))
                                                                (v186 = 75);
                                                        }
                                                        else
                                                        {
                                                            do
                                                                (v186 = (sub_80066570(4) + 31));
                                                            while ((v186 == r_u32(0x800FF5D4u)));
                                                        }
                                                        (v187 = r_u8(((uint32)((a1 + 26)))));
                                                        switch (v187)
                                                        {
                                                            case 1:
                                                                (v188 = DidAnimationCrossFrame(1, (sint32)(sint16)r_u16(0x800EC5E4u)));
                                                                (v189 = UpdateEventLatch(v188, 1));
                                                                (v190 = v186);
                                                                if (!v189)
                                                                {
                                                                    (v191 = a1);
                                                                    (v192 = (sint32)(sint16)r_u16(0x800EC5E6u));
                                                                    (v193 = 1);
                                                                    break;
                                                                }
                                                            LABEL_383:
                                                                sub_80069DF0(v190, 0x3FFF, 0);

                                                                w_u32(0x800FF5D4u, v186);
                                                            LABEL_384:
                                                                (v202 = r_u8(((uint32)((a1 + 568)))));

                                                                (v4 = (v202 == 0));
                                                                (v203 = (v202 - 1));
                                                                if (!v4)
                                                                    w_u8(((uint32)((a1 + 568))), v203);
                                                                (v204 = r_u16(((uint32)((a1 + 574)))));
                                                                if (v204)
                                                                {
                                                                    (v205 = (v204 << 8));
                                                                    if (v204 >= 129)
                                                                    {
                                                                        (v204 = 128);
                                                                        (v205 = 0x8000);
                                                                    }
                                                                    w_u32(((uint32)((a1 + 176))), (((v204 | v205) | 0x6000000) | (v204 << 17)));
                                                                    w_u16(((uint32)((a1 + 180))), 0x2000);
                                                                    (v206 = r_u16(((uint32)((a1 + 574)))));
                                                                    w_u16(((uint32)((a1 + 182))), 10);
                                                                    w_u16(((uint32)((a1 + 574))), --v206);
                                                                    (v207 = (a1 + 228));
                                                                    if (v206)
                                                                    {
                                                                    LABEL_392:
                                                                        if (!sub_8006C304(v207, a1 + 4u))
                                                                        {
                                                                        LABEL_399:
                                                                            if (!r_u32(0x800FF5D8u))
                                                                                w_u32(0x800FF5D8u, 1);

                                                                            xport_draft_host_sub_8006C3AC_p1(vectors, a1 + 4, a1 + 228);
                                                                            (v211 = a1);
                                                                            if ((r_u32(((uint32)((a1 + 460)))) != 0x40000))
                                                                            {
                                                                                if (((v222 | v223) | v224[0]))
                                                                                {
                                                                                    w_u32(0x800FFE90u, v222);
                                                                                    w_u32(0x800FFE94u, v223);
                                                                                    w_u32(0x800FFE98u, v224[0]);
                                                                                }
                                                                                xport_draft_host_sub_800666DC_p1(&v225, 0x800FFE90);
                                                                                (v241 = 16);
                                                                                xport_draft_host_sub_8006C40C_p123(v232, &v225, &v241);
                                                                                xport_draft_host_sub_8006C3AC_p13(&v228, a1 + 228, v232);
                                                                                (v234[0] = v228);
                                                                                (v234[1] = v229);
                                                                                (v234[2] = v230);
                                                                                (v234[3] = r_u32(((uint32)((a1 + 4)))));
                                                                                (v234[4] = r_u32(((uint32)((a1 + 8)))));
                                                                                (v234[5] = r_u32(((uint32)((a1 + 12)))));
                                                                                xport_draft_host_sub_8007BB24_p1(v234);
                                                                                w_u32(0x800FF978u, 0);
                                                                                ((uint8 *)v234)[136] = 1u;
                                                                                xport_draft_host_sub_8007DD04_p1(v234, 1u);
                                                                                w_u32(0x800FF978u, 0);
                                                                                (v211 = a1);
                                                                            }
                                                                            UpdateAnimationModel(0x800A6F20);
                                                                            weapon_angles[0] = 0;
                                                                            weapon_angles[1] = (sint16)v124;
                                                                            weapon_angles[2] = 0;
                                                                            weapon_angles[3] = 0;
                                                                            first_aim = 0;
                                                                            second_aim = 0;
                                                                            if (r_u8(a1 + 470))
                                                                            {
                                                                                first_aim = r_u8(a1 + 471);
                                                                                second_aim = r_u8(a1 + 472);
                                                                            }
                                                                            if (first_aim | second_aim)
                                                                            {
                                                                                weapon_angles[1] = (sint16)(((uint32)v124 + r_u16(a1 + 476)) & 4095);
                                                                            }
                                                                            else
                                                                            {
                                                                                yaw_model = r_u32(a1 + 360);
                                                                                body_heading = r_u16(a1 + 18);
                                                                                model_heading = r_u16(yaw_model + 86);
                                                                                weapon_angles[1] = (sint16)(body_heading + model_heading);
                                                                            }
                                                                            UpdateWeapons(weapon_angles, r_u8(a1 + 572));
                                                                            UpdateGroundContact();
                                                                            if ((((sint16)(r_u16(((uint32)((a1 + 218)))))) < 0))
                                                                                w_u16(((uint32)((a1 + 218))), 0);
                                                                            if (!(r_u32(((uint32)((a1 + 508))))))
                                                                                w_u32(((uint32)((a1 + 512))), 0);
                                                                            if (((r_u32(((uint32)((a1 + 460)))) & 0x180000) == 0))
                                                                                w_u32(((uint32)((a1 + 524))), 0);
                                                                            (v212 = r_u32(((uint32)((a1 + 460)))));
                                                                            (result = 0x20000);
                                                                            if ((v212 != 0x10000))
                                                                            {
                                                                                w_u32(((uint32)((a1 + 516))), 0);
                                                                                (v212 = r_u32(((uint32)((a1 + 460)))));
                                                                            }
                                                                            if ((v212 != 0x20000))
                                                                                w_u32(((uint32)((a1 + 520))), 0);
                                                                            w_u32(((uint32)((a1 + 528))), 0);
                                                                            return result;
                                                                        }

                                                                        (v208 = sub_80067508(v207, (a1 + 4), r_u32(0x800FF4E8u)));
                                                                        if (v208)
                                                                        {
                                                                            (v209 = (a1 + 228));
                                                                            if ((r_u16(((uint32)((v208 + 58)))) == 201))
                                                                                goto LABEL_397;
                                                                            ApplyContactImpulse(v208);
                                                                        }
                                                                        (v209 = (a1 + 228));
                                                                    LABEL_397:
                                                                        (v210 = sub_80067508(v209, (a1 + 4), r_u32(0x800FF204u)));

                                                                        if (v210)
                                                                            Pickup::Attach(v210).Collect(*this);
                                                                        goto LABEL_399;
                                                                    }
                                                                    w_u32(((uint32)((a1 + 176))), 0);
                                                                }
                                                                (v207 = (a1 + 228));
                                                                goto LABEL_392;

                                                            case 3:
                                                                (v194 = DidAnimationCrossFrame(3, (sint32)(sint16)r_u16(0x800EC5E4u)));
                                                                (v195 = UpdateEventLatch(v194, 1));
                                                                (v190 = v186);
                                                                if (v195)
                                                                    goto LABEL_383;
                                                                (v191 = a1);
                                                                (v192 = (sint32)(sint16)r_u16(0x800EC5E6u));
                                                                (v193 = 3);
                                                                break;

                                                            case 4:
                                                                (v196 = DidAnimationCrossFrame(4, (sint32)(sint16)r_u16(0x800EC5E4u)));
                                                                (v197 = UpdateEventLatch(v196, 1));
                                                                (v190 = v186);
                                                                if (v197)
                                                                    goto LABEL_383;
                                                                (v191 = a1);
                                                                (v192 = (sint32)(sint16)r_u16(0x800EC5E6u));
                                                                (v193 = 4);
                                                                break;

                                                            case 8:
                                                                (v198 = DidAnimationCrossFrame(8, 7));
                                                                (v199 = UpdateEventLatch(v198, 1));
                                                                (v190 = v186);
                                                                if (v199)
                                                                    goto LABEL_383;
                                                                (v191 = a1);
                                                                (v193 = 8);
                                                                (v192 = 18);
                                                                break;

                                                            default:
                                                                goto LABEL_384;
                                                        }

                                                        (v200 = Player::Attach(v191).DidAnimationCrossFrame(v193, v192));
                                                        (v201 = UpdateEventLatch(v200, 2));
                                                        (v190 = v186);
                                                        if (!v201)
                                                            goto LABEL_384;
                                                        goto LABEL_383;
                                                    }
                                                    (v178 = (r_u32(((uint32)((a1 + 4)))) + v177));
                                                    (v179 = r_u32(((uint32)((a1 + 428)))));
                                                    w_u32(((uint32)((a1 + 4))), v178);
                                                }
                                                else
                                                {
                                                    (v180 = r_u32(((uint32)((a1 + 428)))));
                                                    (v181 = r_u32((v180 + (26) * 4u)));
                                                    uint32 platform_y = r_u32(v180 + 108);
                                                    uint32 platform_z = r_u32(v180 + 112);
                                                    if (!(v181 | platform_y | platform_z))
                                                        goto LABEL_357;
                                                    (v182 = r_u32(((uint32)((a1 + 428)))));
                                                    w_u32(((uint32)((a1 + 4))), (r_u32(((uint32)((a1 + 4)))) + (v181)));
                                                    (v183 = (r_u32(((uint32)((a1 + 8)))) + r_u32(((uint32)((v182 + 108))))));
                                                    (v179 = r_u32(((uint32)((a1 + 428)))));
                                                    w_u32(((uint32)((a1 + 8))), v183);
                                                }
                                                (v173 = 1);
                                                w_u32(((uint32)((a1 + 12))), (r_u32(((uint32)((a1 + 12)))) + (r_u32(((uint32)((v179 + 112)))))));
                                                goto LABEL_357;
                                            }
                                            w_u8(((uint32)((v172 + 273))), 0);
                                        }
                                        w_u32(((uint32)((a1 + 460))), 4);
                                        w_u32(((uint32)((a1 + 508))), 0);
                                        goto LABEL_347;
                                    }
                                }
                                else if ((v147 != 4096))
                                {
                                    goto LABEL_306;
                                }
                            }
                            v148 = r_u32(0x800A71CC);
                            v149 = r_u32(0x800A71D0);
                            v150 = r_u32(0x800A71D4);
                            w_u32(a1 + 104, v148);
                            w_u32(a1 + 108, v149);
                            w_u32(a1 + 112, v150);
                            v148 = r_u32(0x800A71CC);
                            v149 = r_u32(0x800A71D0);
                            v150 = r_u32(0x800A71D4);
                            w_u32(a1 + 116, v148);
                            w_u32(a1 + 120, v149);
                            w_u32(a1 + 124, v150);
                            goto LABEL_307;
                        }
                        (v145 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 1);
                        (v133 = (((r_u16((((uint32)(v145)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v145)) + (224) * 2u))))));
                    LABEL_296:
                        Player::Attach(v132).SetFacingTransition(v133, v135, v134);

                        goto LABEL_297;
                    }
                    if ((r_u8(((uint32)((a1 + 26)))) != 13))
                        StartAnimation(13, 1);
                    (v132 = a1);
                    (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                    (v133 = 0);
                }
                else if ((v131 == 0x8000))
                {
                    if ((r_u8(((uint32)((a1 + 26)))) != 14))
                        SetAnimationRange(14, 0, -1);
                    (v132 = a1);
                    if (v119)
                    {
                        (v146 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 1);
                        (v133 = ((short)(((r_u16((((uint32)(v146)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)))));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v146)) + (224) * 2u))))));
                        goto LABEL_296;
                    }
                    (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                    (v133 = 0);
                }
                else
                {
                    (v132 = a1);
                    if (((v131 & 0x1F3800) != 0))
                    {
                        (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                        (v133 = 0);
                    }
                    else
                    {
                        (v133 = 0);
                        (v135 = v124);
                    }
                }
                (v134 = 1);
                goto LABEL_296;
            }
            if (v119)
            {
                if ((r_u8(((uint32)((a1 + 26)))) != 9))
                    StartAnimation(9, 1);
                (v132 = a1);
                (v142 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                (v134 = 0);
                (v133 = (((r_u16((((uint32)(v142)) + (208) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                (v135 = ((short)((v124 + r_u16((((uint32)(v142)) + (200) * 2u))))));
                goto LABEL_296;
            }
            (v140 = r_u8(((uint32)((a1 + 26)))));
            if ((v140 == 6))
            {
                (v141 = a1);
                if (!(r_u8(((uint32)((a1 + 303))))))
                {
                LABEL_264:
                    (v132 = a1);

                    (v133 = 0);
                    (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                    (v134 = 0);
                    goto LABEL_296;
                }
            }
            else
            {
                if (!(r_u8(((uint32)((a1 + 26))))))
                    goto LABEL_264;
                (v141 = a1);
                if ((v140 == 5))
                    goto LABEL_264;
            }
            Player::Attach(v141).SetAnimationRange(0, 0, -1);
            goto LABEL_264;
        }
        if ((v36 == 0x20000))
        {
            (v64 = r_u32(((uint32)((a1 + 520)))));
            (v65 = r_u32(((uint32)((a1 + 4)))));
            w_u16(((uint32)((a1 + 580))), 0);
            (v66 = r_u32(((uint32)((v64 + 308)))));
            if ((sint32)Abs32((sint32)((uint32)v66 - (uint32)v65) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 4))), v66);
            else
                w_u32(((uint32)((a1 + 4))), ((uint32)v65 + ((sint32)((uint32)v66 - (uint32)v65) >> 3)));
            (v67 = r_u32(((uint32)((a1 + 12)))));
            (v32 = r_u32(((uint32)((r_u32(((uint32)((a1 + 520)))) + 316)))));
            if ((sint32)Abs32((sint32)((uint32)v32 - (uint32)v67) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 12))), v32);
            else
                w_u32(((uint32)((a1 + 12))), ((uint32)v67 + ((sint32)((uint32)v32 - (uint32)v67) >> 3)));
            if (((r_u8(((uint32)((a1 + 24)))) == 14) && (r_u8(((uint32)((a1 + 297)))) == 1)))
                sub_80062044(r_u32(((uint32)((a1 + 520)))));
            if (r_u8(((uint32)((a1 + 303)))))
            {
                if ((r_u8(((uint32)((a1 + 297)))) == 1))
                {
                    SetAnimationRange(18, ((sint8)(r_u8(((uint32)((a1 + 24)))))), 0);
                    (v37 = 0);
                    goto LABEL_232;
                }
                w_u32(((uint32)((a1 + 460))), 1);
            }
            goto LABEL_231;
        }
        (v37 = 0);
        if ((v36 != 0x40000))
            goto LABEL_232;
        (v68 = r_u16(((uint32)((a1 + 652)))));
        w_u16(((uint32)((a1 + 580))), 0);
        (v69 = (v68 + 1));
        w_u16(((uint32)((a1 + 652))), v69);
        if ((((v69 == 32) || (v69 == 64)) || (v69 == 96)))
        {
            if (r_u32(0x800FF904u))
            {
                (v32 = 34);
                if (r_u32(((uint32)((a1 + 448)))))
                    sub_8007851C(r_u32(0x800FF904u), ((r_u16(((uint32)((r_u32(0x800FF904u) + 494)))) + 1024) & 0xFFF), 34);
            }
            if ((r_u16(((uint32)((a1 + 652)))) == 64))
                sub_8001BEF8(128, 1);
        }
        if (!(r_u8(((uint32)((a1 + 303))))))
        {
            w_u32(0x800FFBE4u, 0);
            (v37 = 0);
            goto LABEL_232;
        }
        if ((r_u32(0x800FF874u) == 1))
        {
            if ((r_u32(0x800FFBE4u) < 128))
                goto LABEL_157;
        }
        else if ((r_u32(0x800FFBE4u) < 64))
        {
        LABEL_157:
            w_u32(0x800FFBE4, r_u32(0x800FFBE4) + 1);

            (v37 = 0);
            goto LABEL_232;
        }
        callback_table = r_u32(a1 + 68);
        callback_receiver = a1 + (uint32)(sint32)(sint16)r_u16(callback_table + 16);
        callback_target = r_u32(callback_table + 20);
        apocalypse_object_virtual20(callback_target, callback_receiver);
        sub_8001BE54();
        goto LABEL_157;
    }
    return result;
}

#undef v216
#undef v217
#undef v218
#undef v219
#undef v222
#undef v223
#undef v224
#undef v225
#undef v227
#undef v228
#undef v229
#undef v230
#undef v231
#undef v232
#undef v233

uint32 Player::SetFacingTransition(uint32 target_angle, uint32 heading, uint32 immediate)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80061A78, "SLUS_003.73");
    uint32 target = target_angle & 4095, desired = heading & 4095;
    uint32 link, yaw_distance, angle_distance, angle_word;
    sint32 old_heading, old_angle, delta;
    if (r_u8(object + 470) || r_u8(object + 473) == 1)
        immediate = 1;
    if (immediate)
    {
        link = r_u32(object + 360);
        w_u16(object + 18, desired);
        w_u16(link + 86, target);
        w_u32(object + 436, 0);
        w_u32(object + 440, 0);
        return link;
    }
    old_heading = (sint16)r_u16(object + 18);
    delta = (sint32)desired - old_heading;
    yaw_distance = delta < 0 ? 0 - (uint32)delta : (uint32)delta;
    if (yaw_distance - 1025 < 2047)
    {
        link = r_u32(object + 360);
        w_u16(object + 18, desired);
        w_u16(link + 86, target);
        w_u32(object + 436, 0);
        w_u32(object + 440, 0);
        return link;
    }
    link = r_u32(object + 360);
    old_angle = (sint16)r_u16(link + 86);
    delta = (sint32)target - old_angle;
    angle_distance = delta < 0 ? 0 - (uint32)delta : (uint32)delta;
    angle_word = r_u16(link + 86);
    if (angle_distance - 1025 < 2047)
    {
        w_u16(object + 18, desired);
        w_u16(link + 86, target);
        w_u32(object + 436, 0);
        w_u32(object + 440, 0);
        return link;
    }
    if (yaw_distance && angle_distance)
    {
        uint32 angle = angle_word;
        angle = old_heading < (sint32)desired ? angle - yaw_distance : angle + yaw_distance;
        w_u16(link + 86, angle & 4095);
    }
    w_u32(object + 440, 8);
    w_u16(object + 18, desired);
    link = r_u32(object + 360);
    w_u32(object + 432, target);
    old_angle = (sint16)r_u16(link + 86);
    angle_word = r_u16(link + 86);
    if (old_angle < (sint32)target)
        delta = (sint32)target - old_angle < 1024 ? (sint32)target - old_angle : (sint16)(target - 4096) - old_angle;
    else
        delta = old_angle - (sint32)target < 1024 ? (sint32)target - old_angle : (sint32)target - (sint16)(angle_word - 4096);
    delta /= 8;
    w_u32(object + 436, (uint32)delta);
    return (uint32)delta;
}

extern "C" uint32 sub_8005F688(uint32 object)
{
    FUNCTION_MARKER(0x8005F688, "SLUS_003.73");
    return Player::Attach(object).Player::Update();
}

extern "C" uint32 sub_8005DDF0(uint32 object, uint32 damage, uint32 vector, uint32 reason)
{
    FUNCTION_MARKER(0x8005DDF0, "SLUS_003.73");
    return Player::Attach(object).Player::Damage(damage, vector ? (const sint32 *)psx_addr(vector, 12) : 0, reason);
}

extern "C" uint32 apocalypse_player_damage_native(uint32 object, uint32 damage, const sint32 vector[3], uint32 reason)
{
    return Player::Attach(object).Player::Damage(damage, vector, reason);
}

extern "C" uint32 sub_8005DD40(uint32 object)
{
    FUNCTION_MARKER(0x8005DD40, "SLUS_003.73");
    return Player::Attach(object).Player::Finish();
}

extern "C" uint32 sub_8005C580(void)
{
    FUNCTION_MARKER(0x8005C580, "SLUS_003.73");
    return SavePlayerTransform();
}

extern "C" uint32 sub_8005CB1C(uint32 object)
{
    FUNCTION_MARKER(0x8005CB1C, "SLUS_003.73");
    return Player::Attach(object).ReadInput();
}

extern "C" uint32 sub_8005CF0C(uint32 object)
{
    FUNCTION_MARKER(0x8005CF0C, "SLUS_003.73");
    return UpdatePlayerLighting(object);
}

extern "C" uint32 sub_8005C8C4(uint32 object)
{
    FUNCTION_MARKER(0x8005C8C4, "SLUS_003.73");
    return TryPlayPlayerKillQuote((const uint32 *)psx_addr(object + 4, 12));
}

extern "C" uint32 sub_8005CE70(uint32 object, uint32 amount)
{
    FUNCTION_MARKER(0x8005CE70, "SLUS_003.73");
    return Player::Attach(object).Player::RestoreHealth(amount);
}

extern "C" uint32 sub_8005C5C0(uint32 object)
{
    FUNCTION_MARKER(0x8005C5C0, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Player));
    new (dram_addr) Player();
    return object;
}

extern "C" uint32 sub_8005C7C0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005C7C0, "SLUS_003.73");
    return Player::Attach(object).Player::Destroy(reason);
}

extern "C" uint32 sub_8005DD84(uint32 object)
{
    FUNCTION_MARKER(0x8005DD84, "SLUS_003.73");
    return Player::Attach(object).Player::ClearWeapons();
}

extern "C" uint32 sub_8005E708(uint32 object)
{
    FUNCTION_MARKER(0x8005E708, "SLUS_003.73");
    return Player::Attach(object).EnterDeathState();
}

extern "C" void sub_8005E90C(uint32 object)
{
    FUNCTION_MARKER(0x8005E90C, "SLUS_003.73");
    Player::Attach(object).PrepareDeathCamera();
}

extern "C" uint32 sub_8005E9E4(uint32 object)
{
    FUNCTION_MARKER(0x8005E9E4, "SLUS_003.73");
    return Player::Attach(object).InitializeAnimation();
}

extern "C" uint32 sub_8005F300(uint32 object)
{
    FUNCTION_MARKER(0x8005F300, "SLUS_003.73");
    return Player::Attach(object).TryEnterAnimation10State();
}

extern "C" uint32 sub_8005EEDC(uint32 object)
{
    FUNCTION_MARKER(0x8005EEDC, "SLUS_003.73");
    return Player::Attach(object).TryEnterRecordMovementState();
}

extern "C" uint32 sub_8005F1B0(uint32 object)
{
    FUNCTION_MARKER(0x8005F1B0, "SLUS_003.73");
    return Player::Attach(object).TryEnterLandingState();
}

extern "C" uint32 sub_8005F00C(uint32 object)
{
    FUNCTION_MARKER(0x8005F00C, "SLUS_003.73");
    return Player::Attach(object).TryEnterJumpState();
}

extern "C" uint32 sub_8005F124(uint32 object)
{
    FUNCTION_MARKER(0x8005F124, "SLUS_003.73");
    return Player::Attach(object).TryEnterAirborneState();
}

extern "C" uint32 sub_80061A78(uint32 object, uint32 target_angle, uint32 heading, uint32 immediate)
{
    FUNCTION_MARKER(0x80061A78, "SLUS_003.73");
    return Player::Attach(object).SetFacingTransition(target_angle, heading, immediate);
}

extern "C" uint32 sub_8005EA1C(uint32 object)
{
    FUNCTION_MARKER(0x8005EA1C, "SLUS_003.73");
    return Player::Attach(object).TryEnterMovementState16();
}

extern "C" uint32 sub_8005EA80(uint32 object)
{
    FUNCTION_MARKER(0x8005EA80, "SLUS_003.73");
    return Player::Attach(object).TryEnterMovementState32();
}

extern "C" uint32 sub_8005EAF0(uint32 object)
{
    FUNCTION_MARKER(0x8005EAF0, "SLUS_003.73");
    return Player::Attach(object).TryEnterMovementState512();
}

extern "C" uint32 sub_8005EB60(uint32 object)
{
    FUNCTION_MARKER(0x8005EB60, "SLUS_003.73");
    return Player::Attach(object).TryEnterMovementState1024();
}

extern "C" uint32 sub_8005EBD0(uint32 object)
{
    FUNCTION_MARKER(0x8005EBD0, "SLUS_003.73");
    return Player::Attach(object).TryActivateOneShotActivator();
}

extern "C" uint32 sub_8005EC7C(uint32 object)
{
    FUNCTION_MARKER(0x8005EC7C, "SLUS_003.73");
    return Player::Attach(object).TryActivateResettingActivator();
}

extern "C" uint32 sub_8005ED30(uint32 object)
{
    FUNCTION_MARKER(0x8005ED30, "SLUS_003.73");
    return Player::Attach(object).TryEnterActionState();
}

extern "C" void sub_8005C868(uint32 object)
{
    FUNCTION_MARKER(0x8005C868, "SLUS_003.73");
    Player::Attach(object).StopWeapon();
}

extern "C" void sub_8005F62C(uint32 object, uint32 duration)
{
    FUNCTION_MARKER(0x8005F62C, "SLUS_003.73");
    Player::Attach(object).SetWeaponStop(duration);
}

extern "C" uint32 sub_8005D360(uint32 object, uint32 rotation, uint32 enabled)
{
    FUNCTION_MARKER(0x8005D360, "SLUS_003.73");
    return Player::Attach(object).UpdateWeapons((const sint16 *)psx_addr(rotation, 6), enabled);
}

extern "C" uint32 apocalypse_player_action(uint32 object, const sint16 *rotation, uint32 enabled)
{
    return Player::Attach(object).UpdateWeapons(rotation, enabled);
}

extern "C" uint32 sub_8005E260(uint32 object, uint32 kind)
{
    FUNCTION_MARKER(0x8005E260, "SLUS_003.73");
    return Player::Attach(object).ReplaceWeapon(kind);
}

extern "C" uint32 sub_8005F470(uint32 object)
{
    FUNCTION_MARKER(0x8005F470, "SLUS_003.73");
    return Player::Attach(object).UpdateGroundContact();
}

extern "C" uint32 sub_8005E07C(uint32 object, uint32 other)
{
    FUNCTION_MARKER(0x8005E07C, "SLUS_003.73");
    return Player::Attach(object).ApplyContactImpulse(other);
}

extern "C" uint32 sub_8005E154(uint32 player, uint32 object)
{
    FUNCTION_MARKER(0x8005E154, "SLUS_003.73");
    return Player::Attach(player).ApplyBeamContactImpulse(object);
}

extern "C" uint32 sub_8005D20C(uint32 object, uint32 polygon, uint32 surface)
{
    FUNCTION_MARKER(0x8005D20C, "SLUS_003.73");
    return Player::Attach(object).SetContactColor(polygon, surface);
}
