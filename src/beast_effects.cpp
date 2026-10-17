#include "visual_object.h"
#include "object_damage.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "beast_effects.h"
#include "sprite_trail.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

// Native input bridge for the original three position-word loads
static uint32 ReadBeastEffectPosition(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

BeastOrbitEffect::BeastOrbitEffect(const uint32 *position) : EffectController()
{
    uint32 trig_angle;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, angle, choice, angular_speed, radius, sine, cosine, child, size;
    FUNCTION_MARKER(0x80040270, "SLUS_003.73");
    w_u32(object + 68, 0x800A2068);
    x = ReadBeastEffectPosition(position, 0);
    y = ReadBeastEffectPosition(position, 1);
    z = ReadBeastEffectPosition(position, 2);
    w_u32(object + 72, x);
    w_u32(object + 76, y);
    w_u32(object + 80, z);
    angle = RandomRange(4096);
    w_u16(object + 88, angle);
    choice = RandomRange(2);
    angular_speed = RandomRange(64);
    angular_speed = choice ? angular_speed + 64 : (uint32)-64 - angular_speed;
    w_u32(object + 52, angular_speed);
    radius = RandomRange(256) + 380;
    angle = r_u16(object + 88);
    w_u32(object + 48, radius);
    trig_angle = angle & 4095;
    sine = (uint32)rsin(trig_angle);
    x = sine * radius;
    angle = r_u16(object + 88);
    w_u32(object + 28, 0);
    w_u32(object + 24, x);
    trig_angle = angle & 4095;
    cosine = (uint32)rcos(trig_angle);
    radius = r_u32(object + 48);
    w_u32(object + 32, cosine * radius);
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 72, 12));
    child = AllocateEffectStorage(100);
    if (child)
    {
        size = RandomRange(21) + 16;
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RibbonTrail));
        new (dram_addr) RibbonTrail((const uint32 *)psx_addr(object + 24, 12), 6, 64, size, 16);
    }
    w_u32(object + 84, child);
    w_u8(child + 66, 1);
    child = r_u32(object + 84);
    RibbonTrail::Attach(child).SetWidth(512);
}

uint32 BeastOrbitEffect::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x800403E4, "SLUS_003.73");
    child = r_u32(object + 84);
    w_u32(object + 68, 0x800A2068);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

uint32 BeastOrbitEffect::Update()
{
    uint32 trig_angle;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 age, angle, angular_speed, sine, cosine, radius, x, z, origin_x, origin_z, delta_y, y, child;
    FUNCTION_MARKER(0x80040460, "SLUS_003.73");
    age = r_u16(object + 8) + 1;
    w_u16(object + 8, age);
    if ((sint16)age >= 180)
        return Finish();
    angle = r_u16(object + 88);
    angular_speed = r_u16(object + 52);
    angle = (angle + angular_speed) & 4095;
    w_u16(object + 88, angle);
    trig_angle = angle;
    sine = (uint32)rsin(trig_angle);
    radius = r_u32(object + 48);
    x = sine * radius;
    angle = r_u16(object + 88);
    trig_angle = angle & 4095;
    w_u32(object + 24, x);
    cosine = (uint32)rcos(trig_angle);
    radius = r_u32(object + 48);
    z = cosine * radius;
    origin_x = r_u32(object + 72);
    origin_z = r_u32(object + 80);
    x = r_u32(object + 24);
    w_u32(object + 24, x + origin_x);
    w_u32(object + 32, z);
    w_u32(object + 32, z + origin_z);
    delta_y = (RandomRange(64) - 32) << 12;
    y = r_u32(object + 28);
    child = r_u32(object + 84);
    w_u32(object + 28, y + delta_y);
    return child ? RibbonTrail::Attach(child).SetPosition((const uint32 *)psx_addr(object + 24, 12)) : delta_y;
}

BeastGroundEffect::BeastGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed, uint32 lifetime) : EffectController()
{
    uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, child, first_angle, second_angle;
    FUNCTION_MARKER(0x80040568, "SLUS_003.73");
    w_u32(object + 68, 0x800A2050);
    w_u8(object + 67, 3);
    x = native_position ? native_position[0] : r_u32(guest_position);
    y = native_position ? native_position[1] : r_u32(guest_position + 4);
    z = native_position ? native_position[2] : r_u32(guest_position + 8);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u32(object + 96, damage);
    w_u32(object + 104, radial_speed);
    w_u32(object + 92, lifetime);
    child = AllocateEffectStorage(116);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MeshRingEffect));
        new (dram_addr) MeshRingEffect(segments, 0xCCE0187C);
    }
    w_u32(object + 72, child);
    w_u8(child + 66, 1);
    child = r_u32(object + 72);
    w_u16(child + 114, 0);
    child = r_u32(object + 72);
    MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), 0, 0);
    first_angle = RandomRange(64);
    second_angle = RandomRange(64);
    child = r_u32(object + 72);
    MeshRingEffect::Attach(child).SetTextureOffsets(first_angle, second_angle, 2);
    w_u32(object + 80, red);
    child = r_u32(object + 72);
    w_u32(object + 88, blue);
    w_u32(object + 84, green);
    MeshRingEffect::Attach(child).SetColors(red, green, blue);
    w_u32(object + 100, 128);
}

uint32 BeastGroundEffect::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x800406B8, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A2050);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

uint32 BeastGroundEffect::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 age, lifetime, old_radius, speed, player, distance, radius, width, y, height, player_y, damage, child, angle, other_angle;
    FUNCTION_MARKER(0x80040734, "SLUS_003.73");
    age = r_u16(object + 8);
    lifetime = r_u32(object + 92);
    age += 1;
    w_u16(object + 8, age);
    if ((sint32)lifetime < (sint16)age)
        return Finish();
    old_radius = r_u32(object + 76);
    speed = r_u32(object + 104);
    player = visual_lists.players;
    w_u32(object + 76, old_radius + speed);
    if (player && !r_u32(object + 108))
    {
        distance = MeasureHorizontalDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(player + 4, 12));
        if ((sint32)distance >= (sint32)old_radius)
        {
            radius = r_u32(object + 76);
            width = r_u32(object + 100);
            if ((sint32)(radius + width) >= (sint32)distance)
            {
                player = visual_lists.players;
                y = r_u32(object + 28);
                height = r_u8(player + 582);
                player_y = r_u32(player + 8);
                if ((sint32)y < (sint32)(player_y + (height << 12)))
                {
                    damage = r_u32(object + 96);
                    ApplyObjectDamage(player, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
                    // Preserve post-call reads whose private vector is discarded by the original
                    player = visual_lists.players;
                    (void)r_u32(player + 4);
                    (void)r_u32(player + 8);
                    (void)r_u32(player + 12);
                    (void)r_u32(object + 28);
                    w_u32(object + 108, 1);
                }
            }
        }
    }
    child = r_u32(object + 72);
    radius = r_u32(object + 76);
    width = r_u32(object + 100);
    MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius, width);
    child = r_u32(object + 72);
    other_angle = r_u32(child + 104);
    angle = r_u32(child + 100);
    return MeshRingEffect::Attach(child).SetTextureOffsets(angle, other_angle - 1, 2);
}

BeastOrbitEffect &BeastOrbitEffect::Attach(uint32 object)
{
    return *static_cast<BeastOrbitEffect *>(FindNativeObjectStorage(object));
}

BeastGroundEffect &BeastGroundEffect::Attach(uint32 object)
{
    return *static_cast<BeastGroundEffect *>(FindNativeObjectStorage(object));
}



extern "C" uint32 sub_800403E4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800403E4, "SLUS_003.73");
    return BeastOrbitEffect::Attach(object).BeastOrbitEffect::Destroy(reason);
}

extern "C" uint32 sub_80040460(uint32 object)
{
    FUNCTION_MARKER(0x80040460, "SLUS_003.73");
    return BeastOrbitEffect::Attach(object).BeastOrbitEffect::Update();
}



extern "C" uint32 apocalypse_beast_ground_effect_native(uint32 object, const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed, uint32 lifetime)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(BeastGroundEffect));
    new (dram_addr) BeastGroundEffect(position, damage, red, green, blue, segments, radial_speed, lifetime);
    return object;
}

extern "C" uint32 sub_800406B8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800406B8, "SLUS_003.73");
    return BeastGroundEffect::Attach(object).BeastGroundEffect::Destroy(reason);
}

extern "C" uint32 sub_80040734(uint32 object)
{
    FUNCTION_MARKER(0x80040734, "SLUS_003.73");
    return BeastGroundEffect::Attach(object).BeastGroundEffect::Update();
}
