#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "level_session.h"
#include "game_tuning.h"
#include "game_tick.h"

#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"

#include "pickup.h"
#include "collision.h"
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

Pickup::Pickup(uint32 resource, const uint32 *position, const uint32 *velocity, uint32 flags, uint32 lifetime) : VisualObject(), ring(), ring_radius(0), ring_wave(0), spawn_linked(0), has_model(0), visible(0), moving(0), ignore_floor(0), floor_offset(0), spawn_record(0), floor_y(0), bob_phase(0), bob_size(0), bob_amplitude(0), bob_step(0), lifetime(lifetime), child()
{
    uint32 x, y, z, value, bank, key = 0;
    FUNCTION_MARKER(0x8001FD28, "SLUS_003.73");
    this->kind = resource;
    value = (uint8)GetGameTuning().pickup_floor_offset;
    this->visible = 1;
    this->floor_offset = value;
    x = position[0];
    y = position[1];
    z = position[2];
    this->base_pos.words[0] = x;
    this->base_pos.words[1] = y;
    this->base_pos.words[2] = z;
    x = position[0];
    y = position[1];
    z = position[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    if (flags & 8)
        motion.velocity.words[1] = (uint32)-245760;
    else
    {
        x = velocity[0];
        y = velocity[1];
        z = velocity[2];
        motion.velocity.words[0] = x;
        motion.velocity.words[1] = y;
        motion.velocity.words[2] = z;
    }
    motion.drag[0] = 5;
    motion.drag[1] = 5;
    motion.drag[2] = 5;
    motion.angular_velocity.words[1] = 100;
    motion.acceleration.words[1] = 32768;
    value = RandomRange(20);
    this->bob_step = value + 90;
    this->bob_size = 50;
    this->bob_amplitude = 50;
    if (flags & 16)
    {
        this->moving = 1;
        this->ignore_floor = 1;
    }
    else if (flags & 4)
    {
        this->moving = 1;
        value = FindGroundHeight(base_pos.words, 0, 8000);
        this->floor_y = value;
    }
    this->collision_radius = 300;
    EnableShadow();
    shadow.max_height = 300;
    SetModelBank("items");
    resource = this->kind;
    switch (resource)
    {
        case 1:
        case 2:
        case 3:
        case 7:
            this->has_model = 0;
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
            resource = 14;
            goto health;
        case 15:
            resource = 15;
            goto health;
        case 16:
            resource = 16;
        health:
            this->has_model = 1;
            bank = items_bank;
            motion.angular_velocity.words[1] = 100 + (resource - 14) * 50;
            this->health = GetGameTuning().health_pickup_amount[resource - 14];
            key = 0x7E74F3D4;
            goto resolve;
        model:
            this->has_model = 1;
            bank = items_bank;
        resolve:
            value = FindModelResourceIndex(key, bank);
            this->model.model_idx = value;
            break;
        case 17:
            SetModelBank("plus_one");
            this->has_model = 1;
            this->floor_offset = 150;
            break;
        case 19:
        case 20:
            this->has_model = 1;
            this->model.model_idx = 0;
            break;
        default:
            break;
    }
    AddToList(visual_lists.pickups);
    if (!this->has_model)
    {
        value = this->model.flags;
        this->model.flags = value | 1;
    }
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

Pickup::~Pickup()
{
    if (child)
        child->Destroy(3);
    if (ring)
        ring->Destroy(3);
}

uint32 Pickup::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x800201F8, "SLUS_003.73");
    RemoveFromList(visual_lists.pickups);
    Pickup::ReleaseResources();
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}

uint32 Pickup::Update()
{
    FUNCTION_MARKER(0x800209EC, "SLUS_003.73");
    ObjectBorrow object_borrow;
    sint32 result;
    uint16 radius;
    uint16 wave;
    uint32 column_cursor;
    sint32 odd_column;
    uint32 column;
    sint32 red;
    sint32 green;
    sint32 extent;
    UpdateLifetime();
    if (player_distance >= 8001)
    {
        return ReleaseResources();
    }
    UpdateMotion();
    if (!this->has_model)
    {
        if (!child)
            CreateChildEffect();
        if (!child)
        {
            GameDiagnostic("Pickup child effect allocation failed\n");
            abort();
        }
        child->SetPosition(model.pos.words);
    }
    if (!ring)
    {
        GameHeapBlock *allocation = AllocateEffectStorage(104);
        if (!allocation)
        {
            GameDiagnostic("Pickup ring allocation failed\n");
            abort();
        }
        ring = new (allocation) RingEffect(4, this->kind == 15 ? 3 : 2);
        ring->hide_center = 1;
        ring->SetCenterColor(0, 0, 0);
        ring->SetRingColors(0, 0, 0);
        ring->SetRowColors(1, 0, 0, 0);
        ring->SetRadii(0);
        ring->persistent = 1;
        switch (this->kind)
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
                this->ring_radius = 60;
                wave = 5;
                goto ApplyRingWave;

            case 5:
                ring->SetRowColors(0, 128, 32, 0);
                this->ring_radius = 50;
                wave = 5;
                goto ApplyRingWave;

            case 6:
                ring->SetRowColors(0, 100, 50, 0);
                this->ring_radius = 30;
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
                column_cursor = 0;
                if (ring->point_count)
                {
                    odd_column = 0;
                    do
                    {
                        column = column_cursor;
                        if (odd_column)
                        {
                            red = 128;

                            green = 0;
                        }
                        else
                        {
                            red = 64;

                            green = 64;
                        }
                        ++column_cursor;
                        ring->SetColumnColors(column, red, green, odd_column ? 0 : 64);
                        odd_column = (column_cursor & 1);
                    } while (column_cursor < ring->point_count);
                }
                ring->SetRingColors(0, 0, 0);
                ring->SetRowColors(1, 0, 0, 0);
                radius = 60;
                goto ApplyRingRadius;

            case 0x11:
                ring->SetRowColors(0, 64, 64, 64);
                radius = 100;
            ApplyRingRadius:
                this->ring_radius = radius;

                wave = 10;
            ApplyRingWave:
                this->ring_wave = wave;

                break;

            default:
                break;
        }
    }
    if (this->visible)
        ring->seg_mask = -1;
    else
        ring->seg_mask = 0;
    ring->SetPosition(this->model.pos.words);
    uint32 phase = (600 * game_clock.tick) & 0x3FF8;
    uint32 amplitude = this->ring_wave;
    sint32 sine = rsin(phase >> 2);
    uint32 product = amplitude * (uint32)sine;
    uint32 base = this->ring_radius;
    if ((sint32)product < 0)
        product += 4095;
    extent = (sint32)(base + (uint32)((sint32)product >> 12));
    ring->SetRowExtent(0, extent);
    ring->SetRowExtent(1, extent);
    if (this->kind == 15)
        ring->SetRowExtent(2, extent);

    result = ring->angle + 50;
    ring->angle = result;
    return result;
}

uint32 Pickup::Finish()
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80020264, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        if (this->spawn_linked)
        {
            SpawnEnemyIndices(GetRecordPayload(this->spawn_record));
        }
        result = this->lifecycle_flags | 64;
        uint32 flags = this->model.flags | 1;
        this->lifecycle_flags = result;
        this->model.flags = flags;
    }
    return result;
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
            GameHeapBlock *allocation = AllocateEffectStorage(kind == 2 || kind == 3 ? 80 : 76);
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
        child->persistent = 1;
    use_small_effect_pool = true;
    return 1;
}

uint32 Pickup::UpdateMotion()
{
    FUNCTION_MARKER(0x800202CC, "SLUS_003.73");
    uint32 x, y, z, phase, amplitude, product, velocity, gravity, shift;
    uint32 unrestricted, damped, offset, floor, result;
    sint32 height, ground;
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    NormalizeAngles(this->model.angles.words);
    if (!this->moving)
    {
        x = this->base_pos.words[0];
        y = this->base_pos.words[1];
        z = this->base_pos.words[2];
        this->model.pos.words[0] = x;
        this->model.pos.words[1] = y;
        this->model.pos.words[2] = z;
        phase = this->bob_phase & 4095;
        amplitude = this->bob_amplitude;
        product = amplitude * (uint32)rsin(phase);
        if ((sint32)product < 0)
            product += 4095;
        offset = (uint32)((sint32)product >> 12) << 12;
        y = this->model.pos.words[1];
        phase = this->bob_phase;
        amplitude = this->bob_step;
        this->model.pos.words[1] = y + offset;
        this->bob_phase = phase + amplitude;
        goto UpdateShadowHeight;
    }
    if (!this->ignore_floor)
    {
        ground = (sint32)this->floor_y;
        if (ground < 0 && ground >= -4)
        {
            if (game_clock.tick & 1)
            {
                floor = FindGroundHeight(base_pos.words, 0, 8000);
                if (floor == 0xFFFFFFFF)
                {
                    floor = this->floor_y - 1;
                    this->floor_y = floor;
                    if ((sint32)floor < -4)
                        this->ignore_floor = 1;
                }
                else
                    this->floor_y = floor;
            }
            goto UpdateShadowHeight;
        }
    }
    AddVectors(base_pos.words, motion.velocity.words);
    velocity = motion.velocity.words[1];
    gravity = motion.acceleration.words[1];
    shift = motion.drag[1] & 31;
    velocity += gravity;
    damped = velocity - (uint32)((sint32)velocity >> shift);
    unrestricted = this->ignore_floor;
    motion.velocity.words[1] = velocity;
    motion.velocity.words[1] = damped;
    if (!unrestricted)
    {
        offset = this->floor_offset << 12;
        floor = this->floor_y;
        y = this->base_pos.words[1];
        floor -= offset;
        if ((sint32)floor < (sint32)y && (sint32)damped > 0)
        {
            this->base_pos.words[1] = floor;
            motion.velocity.words[0] = 0;
            motion.velocity.words[1] = 0;
            motion.velocity.words[2] = 0;
            this->moving = 0;
        }
    }
    x = this->base_pos.words[0];
    y = this->base_pos.words[1];
    z = this->base_pos.words[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
UpdateShadowHeight:
    x = this->base_pos.words[0];
    y = this->base_pos.words[1];
    z = this->base_pos.words[2];
    surface.pos.words[0] = x;
    surface.pos.words[1] = y;
    surface.pos.words[2] = z;
    floor = this->floor_y;
    y = this->model.pos.words[1];
    result = (uint32)((sint32)(floor - y) >> 12);
    shadow.height = result;
    result <<= 16;
    height = (sint32)result >> 16;
    surface.pos.words[1] = floor;
    if (height)
    {
        product = 60 * (uint32)height;
        if ((sint32)product < 0)
            product += 127;
        result = 140 - (uint32)((sint32)product >> 7);
        shadow.radius = result;
    }
    else
        shadow.radius = 0;
    return result;
}

void Pickup::UpdateLifetime()
{
    FUNCTION_MARKER(0x80020518, "SLUS_003.73");
    uint32 count = this->lifetime;
    if (count == 65535)
        return;
    if (count)
        this->lifetime = count - 1;
    count = this->lifetime;
    if (count < 60)
    {
        uint32 amplitude = this->bob_size;
        uint32 remaining = this->lifetime;
        uint32 product = amplitude * remaining;
        this->bob_amplitude = (sint32)product / 60;
    }
    count = this->lifetime;
    if (count - 31 < 29)
        this->visible = (game_clock.tick & 2) ? 1 : 0;
    if (this->lifetime < 31)
        this->visible = (game_clock.tick & 1) ? 1 : 0;
    if (this->has_model)
    {
        uint32 blinking = this->visible;
        uint32 flags = this->model.flags;
        this->model.flags = blinking ? flags & 65534 : flags | 1;
    }
    if (!this->lifetime)
    {
        if (player_distance < 8001)
            SpawnExplosionRing(this->model.pos.words, 20, 255, 255, 255, 4, 1, 20);
        Finish();
    }
}

static const char *MissingCollectedPickupKind(uint32 kind)
{
    // TODO Mutated kind requires an uninitialized original MIPS value
    GameDiagnostic("TODO Pickup kind changed during health collection: %u\n", kind);
    abort();
}

uint32 Pickup::Collect(Player &player)
{
    FUNCTION_MARKER(0x800206C4, "SLUS_003.73");
    ObjectBorrow object_borrow;
    const uint32 sound_id = 26;
    sint32 result;
    bool collected;
    const char *message;
    const HudTextAnimation *text_animation;
    sint32 collected_kind;
    sint32 smartbombs_before;
    result = this->kind;
    collected = false;
    switch (this->kind)
    {
        case 1:
            result = player.ReplaceWeapon(WPN_RIP_LASER);
            collected = result != 0;

            if (collected)
            {
                ClearAnimatedHudText();
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
                ClearAnimatedHudText();
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
                ClearAnimatedHudText();
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
                ClearAnimatedHudText();
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
                ClearAnimatedHudText();
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
                ClearAnimatedHudText();
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
                ClearAnimatedHudText();
                message = HudMessage::flamethrower;
                text_animation = hud_flamethrower_animation;
                goto ShowPickupMessage;
            }
            break;

        case 0xA:
            smartbombs_before = player.smartbombs;
            result = (sint32)((uint32)smartbombs_before + 1);
            if (((sint32)smartbombs_before >= 15))
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
                ClearAnimatedHudText();
                message = HudMessage::smart_bomb;
                goto UsePickupAnimation;
            }
            break;

        case 0xE:

        case 0xF:

        case 0x10:
            result = player.RestoreHealth(((sint16)((uint16)this->health)));
            collected = result != 0;

            if (collected)
            {
                ClearAnimatedHudText();
                collected_kind = this->kind;
                if ((collected_kind == 15))
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
                AddAnimatedHudText(message, text_animation);
            }
            break;

        case 0x11:
            result = (sint32)(game_progress.lives + 1);
            game_progress.lives = (uint32)result;
            if (result >= 101)
                game_progress.lives = 100;
            collected = true;
            ResetPanelDisplayTimer();

            ClearAnimatedHudText();
            AddAnimatedHudText(HudMessage::extra_life, hud_extra_life_animation);
            break;

        default:
            break;
    }

    if (collected)
    {
        Finish();
        return PlaySoundEffect(sound_id, 0x2000, 0);
    }
    return result;
}

void Pickup::SetMotionParameters(uint32 gravity, uint32 damping_shift)
{
    FUNCTION_MARKER(0x8002005C, "SLUS_003.73");
    motion.acceleration.words[1] = gravity;
    motion.drag[1] = damping_shift;
}
