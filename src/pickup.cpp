#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "pickup.h"
#include "game_settings.h"
#include "game_heap.h"
#include "level_session.h"
#include "game_tuning.h"
#include "game_tick.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "object_initializers.h"
#include "collision.h"
#include "game_object_allocation.h"
#include "global.h"
#include "model_bank.h"
#include "enemy_list.h"
#include "record_data.h"
#include "effect_lifecycle.h"
#include "flamethrower_pickup_effect.h"
#include "particle_beam_pickup_effect.h"
#include "rip_laser_pickup_effect.h"
#include "pulse_laser_pickup_effect.h"
#include "angle_math.h"
#include "player.h"
#include "weapon_type.h"
#include "hud_text.h"
#include "game_panel.h"
#include "audio_session.h"
#include "ring_effect.h"
#include "ring_burst_effect.h"

static const char *MissingCollectedPickupKind(uint32 kind)
{
    // TODO Mutated kind requires an uninitialized original MIPS value
    GameDiagnostic(("TODO Pickup kind changed during health collection: %u\n", kind));
    abort();
}

Pickup::Pickup(uint16 resource, const Vector3 *pos, const Vector3 *velocity, uint32 flags, uint16 lifetime) : VisualObject(), ring(), ring_radius(0), ring_wave(0), spawn_linked(0), has_model(false), visible(false), moving(false), ignore_floor(false), floor_offset(0), spawn_record(0), floor_y(0), bob_phase(0), bob_size(0), bob_amplitude(0), bob_step(0), lifetime(lifetime), child()
{
    uint8 bank;
    uint32 key = 0;
    FUNCTION_MARKER(0x8001FD28, "SLUS_003.73");
    kind = resource;
    visible = true;
    floor_offset = GetGameTuning().pickup_floor_offset;
    base_pos = *pos;
    model.pos = *pos;
    if (flags & 8)
        motion.velocity.y = -245760;
    else
        motion.velocity = *velocity;
    motion.drag[0] = 5;
    motion.drag[1] = 5;
    motion.drag[2] = 5;
    motion.angular_velocity.y = 100;
    motion.acceleration.y = 32768;
    bob_step = RandomRange(20) + 90;
    bob_size = 50;
    bob_amplitude = 50;
    if (flags & 16)
    {
        moving = true;
        ignore_floor = true;
    }
    else if (flags & 4)
    {
        moving = true;
        floor_y = CollisionRay::FindGroundHeight(base_pos, 0, 8000);
    }
    col_radius = 300;
    EnableShadow();
    shadow.max_height = 300;
    SetModelBank("items");
    resource = kind;
    switch (resource)
    {
        case 1:
        case 2:
        case 3:
        case 7:
            has_model = false;
            break;
        case 4:
            key = 0x350E968C;
            goto model;
        case 5:
            key = 0xD40B155E;
            goto model;
        case 6:
            key = 0x51E46AAF;
            goto model;
        case 10:
            key = 0x4D2C5D7C;
            goto model;
        case 14:
        case 15:
        case 16:
            has_model = true;
            bank = items_bank;
            motion.angular_velocity.y = 100 + (resource - 14) * 50;
            health = GetGameTuning().health_pickup_amount[resource - 14];
            key = 0x7E74F3D4;
            goto resolve;
        model:
            has_model = true;
            bank = items_bank;
        resolve:
            model.model_idx = ModelBank::FindResourceIdx(key, bank);
            break;
        case 17:
            SetModelBank("plus_one");
            has_model = true;
            floor_offset = 150;
            break;
        case 19:
        case 20:
            has_model = true;
            model.model_idx = 0;
            break;
        default:
            break;
    }
    AddToList(visual_lists.pickups);
    if (!has_model)
        model.flags |= 1;
}

void Pickup::SetMotionParameters(sint32 gravity, uint8 damping_shift)
{
    FUNCTION_MARKER(0x8002005C, "SLUS_003.73");
    motion.acceleration.y = gravity;
    motion.drag[1] = damping_shift;
}

uint32 Pickup::CreateChildEffect()
{
    FUNCTION_MARKER(0x80020068, "SLUS_003.73");
    use_small_effect_pool = false;
    switch (kind)
    {
        case 1:
        case 2:
        case 3:
        case 7:
        {
            GameHeapBlock *allocation = EffectObject::AllocateStorage(kind == 2 || kind == 3 ? 80 : 76);
            if (allocation)
            {
                if (kind == 2)
                {
                    child = new (allocation) ParticleBeamPickupEffect();
                }
                else if (kind == 1)
                {
                    child = new (allocation) RipLaserPickupEffect();
                }
                else if (kind == 3)
                {
                    child = new (allocation) PulseLaserPickupEffect();
                }
                else
                {
                    child = new (allocation) FlamethrowerPickupEffect();
                }
            }
            else
                child = 0;
            break;
        }
        default:
            break;
    }
    if (child)
        child->persistent = true;
    use_small_effect_pool = true;
    return 1;
}

uint32 Pickup::ReleaseResources()
{
    FUNCTION_MARKER(0x80020168, "SLUS_003.73");
    uint32 result = RemoveShadow();
    if (child)
        result = child->Destroy(3);
    child = 0;
    if (ring)
        result = ring->Destroy(3);
    ring = 0;
    return result;
}

uint32 Pickup::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x800201F8, "SLUS_003.73");
    RemoveFromList(visual_lists.pickups);
    Pickup::ReleaseResources();
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

uint32 Pickup::Finish()
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80020264, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        if (spawn_linked)
        {
            SpawnEnemyIndices(GetRecPayload(spawn_record));
        }
        result = lifecycle_flags | 64;
        uint16 flags = model.flags | 1;
        lifecycle_flags = result;
        model.flags = flags;
    }
    return result;
}

uint32 Pickup::UpdateMotion()
{
    FUNCTION_MARKER(0x800202CC, "SLUS_003.73");
    model.angles.AddAngles(motion.angular_velocity);
    model.angles.NormalizeAngles();
    if (!moving)
    {
        model.pos = base_pos;
        sint32 sine = rsin(bob_phase & 4095);
        // Truncate to whole world units before restoring Q12 coordinates
        model.pos.y = (uint32)model.pos.y + (bob_amplitude * sine / ONE * ONE);
        bob_phase += bob_step;
    }
    else if (!ignore_floor && floor_y < 0 && floor_y >= -4)
    {
        if (game_clock.tick & 1)
        {
            uint32 floor = CollisionRay::FindGroundHeight(base_pos, 0, 8000);
            if (floor == 0xFFFFFFFF)
            {
                floor_y = (uint32)floor_y - 1;
                if (floor_y < -4)
                    ignore_floor = true;
            }
            else
                floor_y = floor;
        }
    }
    else
    {
        base_pos.Add(motion.velocity);
        uint32 velocity = (uint32)motion.velocity.y + motion.acceleration.y;
        motion.velocity.y = velocity - ((sint32)velocity >> (motion.drag[1] & 31));
        if (!ignore_floor)
        {
            sint32 floor = (uint32)floor_y - (floor_offset << 12);
            if (floor < base_pos.y && motion.velocity.y > 0)
            {
                base_pos.y = floor;
                motion.velocity = zero_vector;
                moving = false;
            }
        }
        model.pos = base_pos;
    }
    surface.pos = base_pos;
    shadow.height = (sint32)((uint32)floor_y - (uint32)model.pos.y) >> 12;
    surface.pos.y = floor_y;
    if (shadow.height)
    {
        sint32 radius = 140 - 60 * shadow.height / 128;
        shadow.radius = radius;
        return radius;
    }
    shadow.radius = 0;
    return 0;
}

void Pickup::UpdateLifetime()
{
    FUNCTION_MARKER(0x80020518, "SLUS_003.73");
    if (lifetime == 65535)
        return;
    if (lifetime)
        --lifetime;
    if (lifetime < 60)
        bob_amplitude = bob_size * lifetime / 60;
    if (lifetime >= 31 && lifetime < 60)
        visible = (game_clock.tick & 2) != 0;
    if (lifetime < 31)
        visible = (game_clock.tick & 1) != 0;
    if (has_model)
        model.flags = visible ? model.flags & 65534 : model.flags | 1;
    if (!lifetime)
    {
        if (player_distance < 8001)
            RingBurstEffect::SpawnExplosion(&model.pos, 20, 255, 255, 255, 4, 1, 20);
        Finish();
    }
}

uint32 Pickup::Collect(Player &player)
{
    FUNCTION_MARKER(0x800206C4, "SLUS_003.73");
    ObjectBorrow object_borrow;
    sint32 result;
    bool collected;
    const char *message;
    const HudTextAnimation *text_animation;
    uint16 collected_kind;
    uint32 smartbombs_before;
    result = kind;
    collected = false;
    switch (kind)
    {
        case 1:
            result = player.ReplaceWeapon(WPN_RIP_LASER);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::rip_laser;
                text_animation = hud_weapon_animation;
                goto ShowPickupMessage;
            }
            break;

        case 2:
            result = player.ReplaceWeapon(WPN_PARTICLE_BEAM);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::particle_beam;
                text_animation = hud_weapon_animation;
                goto ShowPickupMessage;
            }
            break;

        case 3:
            result = player.ReplaceWeapon(WPN_PULSE_LASER);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::pulse_laser;
                text_animation = hud_pulse_laser_animation;
                goto ShowPickupMessage;
            }
            break;

        case 4:
            result = player.ReplaceWeapon(WPN_ROCKETS);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::rockets;
                text_animation = hud_weapon_animation;
                goto ShowPickupMessage;
            }
            break;

        case 5:
            result = player.ReplaceWeapon(WPN_HOMING_MISSILES);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::homing_missiles;
                text_animation = hud_weapon_animation;
                goto ShowPickupMessage;
            }
            break;

        case 6:
            result = player.ReplaceWeapon(WPN_GRENADES);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::grenades;
                text_animation = hud_weapon_animation;
                goto ShowPickupMessage;
            }
            break;

        case 7:
            result = player.ReplaceWeapon(WPN_FLAMETHROWER);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::flamethrower;
                text_animation = hud_flamethrower_animation;
                goto ShowPickupMessage;
            }
            break;

        case 0xA:
            smartbombs_before = player.smartbombs;
            result = smartbombs_before + 1;
            if ((sint32)smartbombs_before >= 15)
            {
                collected = false;
            }
            else
            {
                player.smartbombs = result;
                collected = true;
            }

            if (collected)
            {
                AnimatedHudText::ClearAll();
                message = HudMessage::smart_bomb;
                goto UsePickupAnimation;
            }
            break;

        case 0xE:

        case 0xF:

        case 0x10:
            result = player.RestoreHealth(health);
            collected = result != 0;

            if (collected)
            {
                AnimatedHudText::ClearAll();
                collected_kind = kind;
                if (collected_kind == 15)
                {
                    message = HudMessage::health50;
                }
                else if (collected_kind >= 16)
                {
                    /* TODO Unexpected kind would retain an inherited assembly value */
                    message = collected_kind == 16 ? HudMessage::health100 : MissingCollectedPickupKind(collected_kind);
                }
                else
                {
                    /* TODO Unexpected kind would retain an inherited assembly value */
                    message = collected_kind == 14 ? HudMessage::health20 : MissingCollectedPickupKind(collected_kind);
                }
            UsePickupAnimation:
                text_animation = hud_pickup_animation;

            ShowPickupMessage:
                AnimatedHudText::Add(message, text_animation);
            }
            break;

        case 0x11:
            result = game_progress.lives + 1;
            game_progress.lives = result;
            if (result >= 101)
                game_progress.lives = 100;
            collected = true;
            ResetPanelDispTimer();

            AnimatedHudText::ClearAll();
            AnimatedHudText::Add(HudMessage::extra_life, hud_extra_life_animation);
            break;

        default:
            break;
    }

    if (collected)
    {
        Finish();
        return PlaySoundEffect(26, 0x2000, 0);
    }
    return result;
}

uint32 Pickup::Update()
{
    FUNCTION_MARKER(0x800209EC, "SLUS_003.73");
    ObjectBorrow object_borrow;
    uint32 result;
    uint16 radius;
    uint16 wave;
    sint32 extent;
    UpdateLifetime();
    if (player_distance >= 8001)
    {
        return ReleaseResources();
    }
    UpdateMotion();
    if (!has_model)
    {
        if (!child)
            CreateChildEffect();
        if (!child)
        {
            GameDiagnostic(("Pickup child effect allocation failed\n"));
            abort();
        }
        child->SetPos(&model.pos);
    }
    if (!ring)
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(104);
        if (!allocation)
        {
            GameDiagnostic(("Pickup ring allocation failed\n"));
            abort();
        }
        ring = new (allocation) RingEffect(4, kind == 15 ? 3 : 2);
        ring->hide_center = true;
        ring->SetCenterColor(0, 0, 0);
        ring->SetRingColors(0, 0, 0);
        ring->SetRowColors(1, 0, 0, 0);
        ring->SetRadii(0);
        ring->persistent = true;
        switch (kind)
        {
            case 1:

            case 0xE:
                ring->SetRowColors(0, 64, 64, 64);
                radius = 60;
                goto ApplyRingRadius;

            case 2:
                ring->SetRowColors(0, 160, 70, 0);
                radius = 60;
                goto ApplyRingRadius;

            case 3:
                ring->SetRadii(30);
                ring->SetRowColors(0, 0, 100, 0);
                radius = 60;
                goto ApplyRingRadius;

            case 4:
                ring->SetRowColors(0, 16, 16, 64);
                ring_radius = 60;
                wave = 5;
                goto ApplyRingWave;

            case 5:
                ring->SetRowColors(0, 128, 32, 0);
                ring_radius = 50;
                wave = 5;
                goto ApplyRingWave;

            case 6:
                ring->SetRowColors(0, 100, 50, 0);
                ring_radius = 30;
                wave = 5;
                goto ApplyRingWave;

            case 7:
                ring->SetRowColors(0, 90, 40, 0);
                radius = 60;
                goto ApplyRingRadius;

            case 0xA:
                ring->SetRowColors(0, 255, 0, 0);
                radius = 80;
                goto ApplyRingRadius;

            case 0xF:
                ring->SetRowColors(2, 0, 0, 0);
                ring->SetRowColors(1, 128, 0, 0);
                ring->SetRowColors(0, 64, 64, 64);
                radius = 60;
                goto ApplyRingRadius;

            case 0x10:
                for (uint32 column = 0; column < ring->point_count; ++column)
                {
                    if (column & 1)
                        ring->SetColumnColors(column, 128, 0, 0);
                    else
                        ring->SetColumnColors(column, 64, 64, 64);
                }
                ring->SetRingColors(0, 0, 0);
                ring->SetRowColors(1, 0, 0, 0);
                radius = 60;
                goto ApplyRingRadius;

            case 0x11:
                ring->SetRowColors(0, 64, 64, 64);
                radius = 100;
            ApplyRingRadius:
                ring_radius = radius;

                wave = 10;
            ApplyRingWave:
                ring_wave = wave;

                break;

            default:
                break;
        }
    }
    if (visible)
        ring->seg_mask = -1;
    else
        ring->seg_mask = 0;
    ring->SetPos(&model.pos);
    uint32 phase = (600 * game_clock.tick) & 0x3FF8;
    sint32 sine = rsin(phase >> 2);
    extent = ring_radius + ring_wave * sine / ONE;
    ring->SetRowExtent(0, extent);
    ring->SetRowExtent(1, extent);
    if (kind == 15)
        ring->SetRowExtent(2, extent);

    result = ring->angle + 50;
    ring->angle = result;
    return result;
}

Pickup::~Pickup()
{
    if (child)
        child->Destroy(3);
    if (ring)
        ring->Destroy(3);
}

Pickup *Pickup::Spawn(uint16 resource, const Vector3 *pos, uint32 flags, uint16 lifetime, const Vector3 *velocity)
{
    uint32 difficulty;
    GameHeapBlock *object;
    FUNCTION_MARKER(0x80020EF8, "SLUS_003.73");
    difficulty = game_settings.difficulty;
    if (difficulty == 2)
    {
        if (resource == 14)
            return 0;
        if (resource == 15)
            resource = 14;
        else if (resource == 16)
            resource = 15;
    }
    else if (!difficulty && resource == 14)
        resource = 15;
    object = AllocateZeroedObject(344);
    if (!object)
        return 0;
    return new (object) Pickup(resource, pos, velocity, flags, lifetime);
}
