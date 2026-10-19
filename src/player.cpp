#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "level_session.h"
#include "game_lighting.h"
#include "game_tuning.h"
#include "game_settings.h"

#include "game_tick.h"

#include "object_initializers.h"

#include "audio_voice.h"
#include "enemy_list.h"
#include "cheat_input.h"
#include "visual_object.h"
#include "object_damage.h"
#include "model_resources.h"
#include "game_diagnostics.h"

#include "player.h"
#include "object_force.h"
#include "ring_attack.h"
#include "beam_barrier.h"
#include "game_random.h"
#include "cd_voice_event.h"
#include "vector_arithmetic.h"
#include "level_interactions.h"
#include "expanding_ring.h"
#include "effect_lifecycle.h"
#include "pickup.h"

#include "game_pad_poll.h"

#include "screen_flash.h"
#include "model_animation.h"
#include "model_matrix.h"
#include "machine_gun.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "audio_session.h"

#include "camera.h"
#include "traversal_rail.h"

// TODO Recover the gameplay name of state 4096

// TODO Recover the gameplay names of record movement states 400 and 401

// TODO Recover the original names of the directional movement states

// TODO Recover original gameplay names for action states 256 and 2048

#include "weapon_targeting.h"

#include "flamethrower.h"
#include "weapon_nodes.h"
#include "spiral_ray_weapon.h"
#include "angle_math.h"
#include "model_attachment.h"

#include "collision.h"
#include "vector_distance.h"

struct PlayerRuntime
{
    uint32 count;
    uint32 rail_entered;
    uint32 jump_released;
    uint32 kills_since_quote;
    uint32 quote_kind;
    uint32 quote_variant;
    uint32 last_footstep_sound;
    uint32 motion_update_seen;
    Vector3 trace_direction;
    uint32 death_wait;
    uint32 quote_history[5];
};

static PlayerRuntime runtime;
static const uint8 hurt_frames[4][2] = {{0, 22}, {23, 36}, {37, 52}, {53, 65}};

struct DirectionAnimation
{
    uint8 animation[8];
    sint8 direction[8];
    uint16 body_heading[8];
    sint16 facing[8];
};

static const DirectionAnimation direction_animations[10] = {
    {{1, 1, 3, 8, 8, 8, 4, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},         {{1, 1, 1, 3, 8, 8, 8, 4}, {1, 1, 1, 1, 1, 1, 1, 1}, {512, 512, 512, 1536, 64000, 64000, 64000, 65024}, {-512, 0, 512, 0, -512, 0, 512, 0}},
    {{4, 1, 1, 1, 3, 8, 8, 8}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 1024, 1024, 1024, 2048, 64512, 64512, 64512}, {0, -512, 0, 512, 0, -512, 0, 512}}, {{8, 3, 1, 1, 1, 4, 8, 8}, {1, -1, 1, 1, 1, -1, 1, 1}, {65024, 512, 1536, 1536, 1536, 64000, 65024, 65024}, {512, 0, -512, 0, 512, 0, -512, 0}},
    {{8, 8, 4, 1, 1, 1, 3, 8}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},         {{8, 8, 8, 3, 1, 1, 1, 4}, {1, 1, 1, -1, 1, 1, 1, -1}, {512, 512, 512, 1536, 64000, 64000, 64000, 65024}, {-512, 0, 512, 0, -512, 0, 512, 0}},
    {{3, 8, 8, 8, 4, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 1024, 1024, 1024, 2048, 64512, 64512, 64512}, {0, -512, 0, 512, 0, -512, 0, 512}}, {{1, 3, 8, 8, 8, 4, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {65024, 512, 1536, 1536, 1536, 64000, 65024, 65024}, {512, 0, -512, 0, 512, 0, -512, 0}},
    {{0, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},         {{5, 5, 5, 5, 5, 5, 5, 5}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},
};

void ResetPlayerRuntime()
{
    runtime = PlayerRuntime();
}

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

void Player::StopSelectedWeapon(uint32 selected)
{
    WeaponNode *weapon = weapons[selected];
    if (weapon)
        weapon->Update(model.pos.words, zero_angles.components);
}

void SavePlayerTransform()
{
    FUNCTION_MARKER(0x8005C580, "SLUS_003.73");
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        player.saved_pos.words[axis] = player.model.pos.words[axis];
        player.saved_angles.words[axis] = player.model.angles.words[axis];
    }
}

Player::Player() : Character(), facing_target(0), facing_step(0), facing_ticks(0), pad(0), death_camera_enabled(0), disable_timer(0), weapon_stop_ticks(0), movement_state(0), update_disabled(0), hit_reaction_ticks(0), damage_glow_ticks(0), analog_aim(0), aim_press_ticks(0), move_heading(0), aim_heading(0), fall_start_y(0), fall_damage_threshold(0), fatal_fall_distance(0), rail_voice(0), on_rail(0), smartbombs(0), fire_ticks(0), jump_velocity(0), jump_hold_ticks(0), jump_latch(0), airborne_ticks(0), weapons_enabled(0), slowdown_ticks(0), slowdown_duration(0), ground_grace_ticks(0), idle_ticks(0), ground_offset(0), collision_probe_size(0), action_heading(0), move_ramp(0), sel_weapon(0), prev_weapon(0), death_ticks(0)
{
    FUNCTION_MARKER(0x8005C5C0, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        muzzle_pos.words[axis] = 0;
        saved_surface_pos.words[axis] = 0;
        rail_pos.words[axis] = 0;
        saved_pos.words[axis] = 0;
        saved_angles.words[axis] = 0;
        ledge_pos[axis] = 0;
        ledge_normal[axis] = 0;
    }
    move_input[0] = move_input[1] = 0;
    aim_input[0] = aim_input[1] = 0;
    WeaponNode *new_weapon = 0;
    for (uint32 slot = 0; slot < 8; ++slot)
        weapons[slot] = 0;
    uint32 count;
    sint16 flags;
    sint16 maximum_health;
    sint16 object_flags;
    sint8 damping_shift;
    uint16 probe_size;
    GameHeapBlock *weapon;
    sint32 weapon_damage;
    sint32 difficulty;
    runtime.kills_since_quote = 0;
    runtime.rail_entered = 0;
    runtime.jump_released = 0;
    AddToList(visual_lists.players);
    count = runtime.count;
    flags = ((sint16)(this->lifecycle_flags));
    this->collision_radius = 100;
    runtime.count = (count + 1);
    this->lifecycle_flags = (flags & 0xFFFD);
    maximum_health = GetGameTuning().player.maximum_health;
    object_flags = ((sint16)(this->model.flags));
    this->pad = &game_pads[0];
    this->model.flags = (object_flags | 0x80);
    this->health = (sint16)(uint16)(maximum_health);
    SetModelBank("bruce");
    this->kind = 50;
    damping_shift = GetGameTuning().player.vertical_drag;
    motion.drag[0] = 1;
    motion.drag[2] = 1;
    motion.drag[1] = damping_shift;
    motion.angular_drag[0] = 5;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 5;
    ground_offset = GetGameTuning().player.ground_offset;
    probe_size = GetGameTuning().player.collision_probe_size;
    this->collision_radius = 100;
    this->death_camera_enabled = 1;
    projection.enabled = true;
    this->collision_probe_size = probe_size;
    projection.dir.m[0][0] = 0;
    projection.dir.m[0][1] = -4096;
    projection.dir.m[0][2] = 0;
    projection.color = 0x808080;
    InitializeAnimation();
    if (visual_lists.cameras)
        static_cast<Camera *>(visual_lists.cameras->owner)->mode = 3;
    weapon = AllocateZeroedHeapBlock(88);
    if (weapon)
    {
        new_weapon = new (weapon) MachineGun(&visual_lists.actors);
    }
    weapon_damage = 5;
    difficulty = (game_settings.difficulty != 0);
    this->weapons[0] = new_weapon;
    if (!difficulty)
        weapon_damage = 7;
    new_weapon->damage = weapon_damage;
    this->sel_weapon = 0;
    this->prev_weapon = 1;
    this->smartbombs = 3;
    SetModelBoneTable(this->model.bank, PlayerAttachments);
    InitializeModelAnimation(PlayerHierarchy);
}

uint32 Player::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8005C7C0, "SLUS_003.73");
    projection.enabled = false;
    RemoveFromList(visual_lists.players);
    runtime.count = runtime.count - 1;
    Player::ReleaseResources();
    Character::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

void Player::StopWeapon()
{
    uint32 selected;
    FUNCTION_MARKER(0x8005C868, "SLUS_003.73");
    selected = this->sel_weapon;
    StopSelectedWeapon(selected);
}

uint32 TryPlayPlayerKillQuote(const uint32 *position)
{
    FUNCTION_MARKER(0x8005C8C4, "SLUS_003.73");
    uint32 count, total, random, now, previous_time, level, variant_count;
    uint32 kind, variant, selected, history[5];
    count = runtime.kills_since_quote;
    total = game_progress.level.enemy_kills;
    runtime.kills_since_quote = count + 1;
    game_progress.level.enemy_kills = total + 1;
    random = RandomRange(2);
    count = runtime.kills_since_quote;
    if (!((sint32)(random + 3) < (sint32)count))
        return 0;
    (void)game_clock.vblank_frame;
    now = game_clock.vblank_frame;
    previous_time = cd_voice.last_frame;
    if (now - previous_time < 900)
        return 1;
    runtime.kills_since_quote = 0;
    for (;;)
    {
        level = level_environment.area_kind;
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
        runtime.quote_kind = kind;
        variant = RandomRange(variant_count);
        runtime.quote_variant = variant;
        kind = runtime.quote_kind;
        variant = runtime.quote_variant;
        history[0] = runtime.quote_history[0];
        selected = (kind << 3) + variant;
        if (history[0] == selected)
            continue;
        history[1] = runtime.quote_history[1];
        if (history[1] == selected)
            continue;
        history[2] = runtime.quote_history[2];
        if (history[2] == selected)
            continue;
        history[3] = runtime.quote_history[3];
        if (history[3] == selected)
            continue;
        history[4] = runtime.quote_history[4];
        if (history[4] == selected)
            continue;
        runtime.quote_history[0] = history[1];
        runtime.quote_history[1] = history[2];
        runtime.quote_history[2] = history[3];
        runtime.quote_history[3] = history[4];
        runtime.quote_history[4] = selected;
        kind = runtime.quote_kind;
        variant = runtime.quote_variant;
        return PlayCdVoiceEvent(kind, variant, position, 0);
    }
}

uint32 Player::ReadInput()
{
    GamePadInput *pad;
    FUNCTION_MARKER(0x8005CB1C, "SLUS_003.73");
    uint32 disabled, pad_type, x, y, value, factor, divisor, quotient, result;
    disabled = this->disable_timer;
    this->move_input[0] = 0;
    this->move_input[1] = 0;
    this->aim_input[0] = 0;
    this->aim_input[1] = 0;
    this->analog_aim = 1;
    if (disabled)
    {
        this->aim_press_ticks = 0;
        return 1;
    }
    pad = this->pad;
    pad_type = pad->type;
    if (pad_type == 115 || pad_type == 83)
    {
        value = pad->axis[0];
        pad = this->pad;
        this->move_input[0] = value;
        value = pad->axis[1];
        this->move_input[1] = value;
        value = pad->axis[2];
        this->aim_input[0] = value;
        value = pad->axis[3];
        this->aim_input[1] = value;
    }
    x = this->move_input[0];
    y = this->move_input[1];
    if (!(x | y))
    {
        pad = this->pad;
        value = pad->buttons[10].down;
        x = 128;
        if (!value)
        {
            value = pad->buttons[11].down;
            x = 127;
        }
        if (value)
        {
            this->move_input[0] = x;
            pad = this->pad;
        }
        value = pad->buttons[9].down;
        y = 127;
        if (!value)
        {
            value = pad->buttons[8].down;
            y = 128;
        }
        if (value)
            this->move_input[1] = y;
    }
    factor = this->slowdown_ticks;
    if (factor)
    {
        x = (uint32)(sint32)(sint8)this->move_input[0];
        divisor = this->slowdown_duration;
        quotient = DividePlayerInput(factor * x, divisor);
        this->move_input[0] = x - quotient;
        y = (uint32)(sint32)(sint8)this->move_input[1];
        factor = this->slowdown_ticks;
        divisor = this->slowdown_duration;
        quotient = DividePlayerInput(factor * y, divisor);
        this->move_input[1] = y - quotient;
    }
    x = this->aim_input[0];
    y = this->aim_input[1];
    if (x | y)
        this->aim_press_ticks = 0;
    else
    {
        pad = this->pad;
        this->analog_aim = 0;
        value = pad->buttons[0].down;
        x = 128;
        if (!value)
        {
            value = pad->buttons[3].down;
            x = 127;
        }
        if (value)
            this->aim_input[0] = x;
        pad = this->pad;
        value = pad->buttons[2].down;
        y = 127;
        if (!value)
        {
            value = pad->buttons[1].down;
            y = 128;
        }
        if (value)
        {
            this->aim_input[1] = y;
            pad = this->pad;
        }
        if (pad->buttons[0].down || pad->buttons[3].down || pad->buttons[2].down || pad->buttons[1].down)
        {
            value = this->aim_press_ticks + 1;
            this->aim_press_ticks = value;
            if ((value & 255) >= 2)
                this->aim_press_ticks = 2;
        }
        else
            this->aim_press_ticks = 0;
    }
    x = this->move_input[0];
    y = this->move_input[1];
    if (x | y)
    {
        result = ratan2(-(sint32)(sint8)x, (sint8)y);
        this->move_heading = (1024 - result) & 4095;
    }
    x = this->aim_input[0];
    y = this->aim_input[1];
    result = x | y;
    if (result)
    {
        result = ratan2(-(sint32)(sint8)x, (sint8)y);
        this->aim_heading = (1024 - result) & 4095;
    }
    return result;
}

uint32 Player::RestoreHealth(uint32 amount)
{
    uint32 health, maximum, sound;
    sint32 signed_maximum;
    FUNCTION_MARKER(0x8005CE70, "SLUS_003.73");
    health = (uint16)this->health + amount;
    this->health = (sint16)(uint16)(health);
    signed_maximum = (sint16)GetGameTuning().player.maximum_health;
    maximum = GetGameTuning().player.maximum_health;
    if (signed_maximum < (sint16)health)
        this->health = (sint16)(uint16)(maximum);
    sound = RandomRange(2);
    PlayCdVoiceEvent(1, sound, this->model.pos.words, 1);
    return 1;
}

uint32 Player::UpdateLighting()
{
    FUNCTION_MARKER(0x8005CF0C, "SLUS_003.73");
    ShortVector3 direction = {{{0, -ONE, 0}}};
    sint16 light_color[3] = {ONE, ONE, ONE};
    sint16 surface_color[3];
    uint32 color = model.color;
    sint32 red = color & 255;
    sint32 green = (color >> 8) & 255;
    sint32 blue = (color >> 16) & 255;
    uint32 intensity = color >> 24;
    if (intensity >= 1 && intensity <= 127)
    {
        red = (red * (0x80000 / intensity)) >> 12;
        green = (green * (0x80000 / intensity)) >> 12;
        blue = (blue * (0x80000 / intensity)) >> 12;
    }
    surface_color[0] = red * 16;
    surface_color[1] = green * 16;
    surface_color[2] = blue * 16;
    WeaponNode *weapon = weapons[sel_weapon];
    uint8 light_red, light_green, light_blue;
    if (weapon && weapon->ReadLight(&light_red, &light_green, &light_blue))
    {
        Vector3 difference, shifted, normal;
        uint32 shift = 12;
        SubtractVectors(difference.words, muzzle_pos.words, model.pos.words);
        ShiftVectorRight(shifted.words, difference.words, shift);
        NormalizeFixedVector(normal.words, shifted.words);
        direction.components[0] = (sint16)normal.words[0];
        direction.components[1] = 0;
        direction.components[2] = (sint16)normal.words[2];
        light_color[0] = light_red * 16;
        light_color[1] = light_green * 16;
        light_color[2] = light_blue * 16;
    }
    else
    {
        uint32 fallback_intensity = model.color >> 24;
        sint16 fallback_color = fallback_intensity < 64 ? 1024 : fallback_intensity * 16;
        light_color[0] = fallback_color;
        light_color[1] = fallback_color;
        light_color[2] = fallback_color;
    }
    Camera *camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    uint32 heading = camera ? camera->angles.words[1] & 4095 : 0;
    sint16 sine = rsin(heading);
    sint16 cosine = rcos(heading);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        game_lighting.directions.m[0][axis] = direction.components[axis];
        game_lighting.directions.m[1][axis] = axis == 1 ? ONE : 0;
        game_lighting.directions.m[2][axis] = axis == 0 ? sine : axis == 2 ? cosine : 0;
        game_lighting.colors.m[axis][0] = light_color[axis];
        game_lighting.colors.m[axis][1] = surface_color[axis];
        game_lighting.colors.m[axis][2] = surface_color[axis];
    }
    game_lighting.background_color.r = 64;
    game_lighting.background_color.g = 64;
    game_lighting.background_color.b = 64;
    return 64;
}

uint32 Player::SetContactColor(const ModelPolygon &polygon, const ModelInstance &surface)
{
    uint32 flags, count, index, vertex, selected = 0, maximum = 0, color, component, result;
    bool found = false;
    FUNCTION_MARKER(0x8005D20C, "SLUS_003.73");
    flags = polygon.flags;
    if (flags & 2048)
    {
        const ModelPalette *table = &GetModelBank(surface.bank).colors;
        count = (flags & 16) ? 3 : 4;
        for (index = 0; index < count; ++index)
        {
            vertex = ((polygon.color >> (index * 8)) & 255);
            color = ReadModelPaletteColor(table, vertex) & 0xFFFFFF;
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
            selected = (polygon.color & 255);
        }
        color = ReadModelPaletteColor(table, selected);
        result = color + (maximum << 24);
    }
    else
    {
        color = polygon.color & 0xFFFFFF;
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
    this->model.color = result;
    return result;
}

uint32 Player::UpdateWeapons(const ShortVector3 &rotation, uint32 enabled)
{
    FUNCTION_MARKER(0x8005D360, "SLUS_003.73");
    // TODO Original cheat allocation failure writes through a null weapon
    WeaponNode *new_weapon = 0;
    sint32 result;
    bool weapon_activity;
    GameHeapBlock *ring_storage;
    GameHeapBlock *weapon_storage;
    uint32 weapon_damage;
    GamePadInput *input;
    uint32 smartbombs_before;
    bool not_easy;
    bool smartbombs_empty;
    uint32 smartbombs_after;
    sint32 prev_slot;
    uint32 start_slot;
    uint32 next_slot;
    sint32 wrapped_slot;
    uint8 prev_fire_ticks;
    uint32 cone, button1, button2;
    uint32 weapon_type;
    bool firing_blocked;
    uint32 fire_result;
    sint32 fire_slot;
    WeaponNode *active_weapon;
    uint32 actuator_val;
    uint32 actuator;
    uint32 actuator_enabled;
    WeaponNode *fired_weapon;
    sint32 slot;
    sint32 depleted_slot;
    WeaponNode *idle_weapon;
    WeaponNode *other_weapon;
    Vector3 weapon_pos;
    ShortVector3 aim_angles;
    if (!HasModelJointAngles())
        return 0;
    weapon_activity = false;
    if (this->pad->buttons[18].down && this->movement_state != 0x40000 && !active_ring_attack && !this->disable_timer && this->smartbombs)
    {
        ring_storage = AllocateEffectStorage(172);
        if (ring_storage)
        {
            new (ring_storage) RingAttack(100, 10, 8192);
        }
        if (game_settings.vibration == VibrationFull)
            SetGamePadActuator(0, 12, 1, 120);
        if (!GetCheatOptions().weapon_cheat)
        {
            smartbombs_before = this->smartbombs;
            smartbombs_empty = smartbombs_before == 0;
            smartbombs_after = smartbombs_before - 1;
            if (!smartbombs_empty)
                this->smartbombs = smartbombs_after;
        }
        PlayCdVoiceEvent(0, 2, this->model.pos.words, 1);
    }
    input = this->pad;
    if (input->buttons[19].pressed)
    {
        input->buttons[19].pressed = 0;
        if (GetCheatOptions().weapon_cheat)
        {
            PlaySoundEffect(26, 8192, 0);
            switch (this->weapons[this->sel_weapon]->type)
            {
                case WPN_FLAMETHROWER:
                    if (!this->weapons[2])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) PulseLaser(&visual_lists.actors);
                        }
                        weapon_damage = 10;
                        not_easy = game_settings.difficulty != 0;
                        this->weapons[2] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 15;
                        new_weapon->damage = weapon_damage;
                    }
                    this->prev_weapon = 1;
                    this->sel_weapon = 2;
                    break;

                case WPN_PULSE_LASER:
                    if (!this->weapons[3])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(104);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) ParticleBeam(&visual_lists.actors, 3000, 0, 256, 9);
                        }
                        weapon_damage = 10;
                        not_easy = game_settings.difficulty != 0;
                        this->weapons[3] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 15;
                        new_weapon->damage = weapon_damage;
                    }
                    this->prev_weapon = 2;
                    this->sel_weapon = 3;
                    break;

                case WPN_PARTICLE_BEAM:
                    if (!this->weapons[4])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(100);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) RipLaser(&visual_lists.actors);
                        }
                        weapon_damage = 2;
                        not_easy = game_settings.difficulty != 0;
                        this->weapons[4] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 3;
                        new_weapon->damage = weapon_damage;
                    }
                    this->prev_weapon = 3;
                    this->sel_weapon = 4;
                    break;

                case WPN_RIP_LASER:
                    if (!this->weapons[5])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) Grenades(&visual_lists.actors);
                        }
                        weapon_damage = 120;
                        not_easy = game_settings.difficulty != 0;
                        this->weapons[5] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 180;
                        new_weapon->damage = weapon_damage;
                    }
                    this->prev_weapon = 4;
                    this->sel_weapon = 5;
                    break;

                case WPN_GRENADES:
                    if (!this->weapons[6])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) HomingMissiles(&visual_lists.actors);
                        }
                        weapon_damage = 15;
                        not_easy = game_settings.difficulty != 0;
                        this->weapons[6] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 22;
                        new_weapon->damage = weapon_damage;
                    }
                    this->prev_weapon = 5;
                    this->sel_weapon = 6;
                    break;

                case WPN_HOMING_MISSILES:
                    if (!this->weapons[7])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) Rockets(&visual_lists.actors);
                        }
                        weapon_damage = 40;
                        not_easy = game_settings.difficulty != 0;
                        this->weapons[7] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 60;
                        new_weapon->damage = weapon_damage;
                    }
                    this->prev_weapon = 6;
                    this->sel_weapon = 7;
                    break;

                case WPN_ROCKETS:
                    this->prev_weapon = 7;
                    this->sel_weapon = 0;
                    break;

                case WPN_MACHINE_GUN:
                    if (!this->weapons[1])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(72);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) Flamethrower(&visual_lists.actors, 0);
                        }
                        weapon_damage = 10;
                        not_easy = game_settings.difficulty != 0;
                        this->weapons[1] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 15;
                        new_weapon->damage = weapon_damage;
                    }
                    this->prev_weapon = 0;
                    this->sel_weapon = 1;
                    break;

                default:
                    break;
            }
        }
        else
        {
            start_slot = this->sel_weapon;
            next_slot = start_slot + 1;
            if ((sint32)(start_slot + 1) < 8)
            {
                while (!this->weapons[next_slot])
                {
                    ++next_slot;
                    if ((sint32)next_slot >= 8)
                        goto WrapWeaponSelection;
                }

                PlaySoundEffect(26, 8192, 0);
                prev_slot = this->sel_weapon;
                this->sel_weapon = next_slot;
                this->prev_weapon = prev_slot;
            }
        WrapWeaponSelection:
            if (start_slot == this->sel_weapon && start_slot)
            {
                PlaySoundEffect(26, 8192, 0);
                wrapped_slot = this->sel_weapon;
                this->sel_weapon = 0;
                this->prev_weapon = wrapped_slot;
            }
        }
    }
    GetModelAttachment(weapon_pos.words, *this, 0);
    firing_blocked = (this->movement_state & 0xF3800) || !(this->aim_input[0] | this->aim_input[1]) || !enabled;
    if (firing_blocked)
    {
        idle_weapon = this->weapons[this->sel_weapon];
        if (idle_weapon)
            idle_weapon->Update(weapon_pos.words, rotation.components);
        this->fire_ticks = 0;
        AddVectors(weapon_pos.words, motion.velocity.words);
        goto BeginOtherWeapons;
    }
    prev_fire_ticks = this->fire_ticks;
    this->idle_ticks = 0;
    this->fire_ticks = prev_fire_ticks + 1;
    aim_angles = rotation;
    if (!this->weapons[this->sel_weapon])
        goto MarkWeaponActivity;
    muzzle_pos = weapon_pos;
    weapon_type = this->weapons[this->sel_weapon]->type;
    if (weapon_type == WPN_GRENADES || weapon_type == WPN_PULSE_LASER || weapon_type == WPN_ROCKETS || weapon_type == WPN_MACHINE_GUN)
    {
        cone = 768;
        button1 = game_pads[0].buttons[0].down;
        if (!button1)
            button1 = game_pads[0].buttons[3].down;
        if (button1)
        {
            button2 = game_pads[0].buttons[1].down;
            if (!button2)
                button2 = game_pads[0].buttons[2].down;
            if (button2)
                cone = 1536;
        }
        SelectAimTarget(aim_angles, weapon_pos, rotation, 4096, cone, 3072, 0, 0, visual_lists.actors);
    }
    fire_result = 0;

    if (this->fire_ticks < 2)
    {
        active_weapon = this->weapons[this->sel_weapon];
        active_weapon->Update(weapon_pos.words, aim_angles.components);
    }
    else
    {
        fire_slot = this->sel_weapon;
        this->fire_ticks = 1;
        active_weapon = this->weapons[fire_slot];
        fire_result = active_weapon->Fire(weapon_pos.words, aim_angles.components);
    }
    if (game_settings.vibration)
    {
        switch (weapon_type)
        {
            case WPN_FLAMETHROWER:
                if (game_settings.vibration == VibrationFull && !this->hit_reaction_ticks)
                {
                    actuator_val = 4;
                    actuator = 1;
                    actuator_enabled = 60;
                    goto ApplyWeaponVibration;
                }
                break;

            case WPN_PULSE_LASER:
                if (fire_result && game_settings.vibration == VibrationFull && !this->hit_reaction_ticks)
                {
                    actuator_val = 2;
                    goto SetImpulseActuator;
                }
                break;

            case WPN_PARTICLE_BEAM:

            case WPN_RIP_LASER:
                if (game_settings.vibration == VibrationFull && !this->hit_reaction_ticks)
                {
                    actuator_val = 2;
                    goto SetImpulseActuator;
                }
                break;

            case WPN_GRENADES:

            case WPN_HOMING_MISSILES:

            case WPN_ROCKETS:
                if (fire_result && game_settings.vibration == VibrationFull && !this->hit_reaction_ticks)
                {
                    actuator_val = 4;
                SetImpulseActuator:
                    actuator = 0;

                    actuator_enabled = 1;
                    goto ApplyWeaponVibration;
                }
                break;

            case WPN_MACHINE_GUN:
                if (game_settings.vibration == VibrationFull && !this->hit_reaction_ticks)
                {
                    actuator_val = 4;
                    actuator = 1;
                    actuator_enabled = 120;
                ApplyWeaponVibration:
                    SetGamePadActuator(0, actuator_val, actuator, actuator_enabled);
                }
                break;

            default:
                break;
        }
    }
    fired_weapon = this->weapons[this->sel_weapon];
    weapon_activity = true;
    if (fired_weapon && fired_weapon->type == WPN_MACHINE_GUN)
        goto BeginOtherWeapons;
    slot = 0;
    if (fired_weapon && (sint32)fired_weapon->ammo > 0)
        goto UpdateOtherWeapons;
    if (fired_weapon)
    {
        fired_weapon->Destroy(3);
    }
    this->weapons[this->sel_weapon] = 0;
    depleted_slot = this->sel_weapon;
    this->sel_weapon = 0;
    this->prev_weapon = depleted_slot;
MarkWeaponActivity:
    weapon_activity = true;

BeginOtherWeapons:
    slot = 0;

UpdateOtherWeapons:

    do
    {
        if (slot != this->sel_weapon)
        {
            other_weapon = this->weapons[slot];
            if (other_weapon)
                other_weapon->Update(weapon_pos.words, rotation.components);
        }
        ++slot;
    } while (slot < 8);
    // Historical unconsumed V0 carrier from the original data-page load
    result = -2146500608;
    if (weapon_activity)
    {
        result = GetGameTuning().weapon_activity_value;
    }
    return result;
}

uint32 Player::Finish()
{
    uint32 result, flags;
    FUNCTION_MARKER(0x8005DD40, "SLUS_003.73");
    result = IsFinished();
    if (!result)
    {
        result = this->lifecycle_flags;
        flags = this->model.flags;
        result |= 0x40;
        flags |= 1;
        this->lifecycle_flags = result;
        this->model.flags = flags;
    }
    return result;
}

uint32 Player::ReleaseResources()
{
    FUNCTION_MARKER(0x8005DD84, "SLUS_003.73");
    for (uint32 index = 0; index < 8; ++index)
        if (weapons[index])
        {
            weapons[index]->Destroy(3);
            weapons[index] = 0;
        }
    return 0;
}

uint32 Player::Damage(uint32 damage, const sint32 *vector, uint32 reason)
{
    FUNCTION_MARKER(0x8005DDF0, "SLUS_003.73");
    if (GetCheatOptions().invulnerable)
        return 0;
    if (this->health <= 0)
        return 0;
    if (VisualObject::IsFinished())
        return 0;
    if (this->disable_timer)
        return 0;
    if (this->weapon_stop_ticks)
        return 0;
    Vector3 impulse;
    uint32 shift = 1;
    const Vector3 zero_impulse = {0};
    const uint32 *input = vector ? (const uint32 *)vector : zero_impulse.words;
    ShiftVectorRight(impulse.words, input, shift);
    AddVectors(motion.velocity.words, impulse.words);
    if ((sint32)damage >= 2)
    {
        sint32 maximum = (sint16)GetGameTuning().player.maximum_health;
        sint32 threshold = maximum / 5;
        sint32 health = this->health;
        if (health < threshold)
            damage = (uint32)((sint32)damage >> 1);
        if (!game_settings.difficulty)
            damage = (uint32)((sint32)damage >> 1);
    }
    uint32 remaining = (uint16)this->health - damage;
    this->health = (sint16)(uint16)(remaining);
    if ((sint16)remaining <= 0)
    {
        this->health = (sint16)(uint16)(0);
        EnterDeathState();
        ClearGamePadActuator(0, 0);
        ClearGamePadActuator(0, 1);
        return 1;
    }
    this->hit_reaction_ticks = 6;
    if (reason == 5 || reason == 31)
    {
        this->damage_glow_ticks = 90;
        SetColor(90, 255, 0, 0);
        model_effect.scale = 10240;
        uint32 intensity = this->damage_glow_ticks;
        model_effect.depth_bias = 10;
        model_effect.color_mode = (intensity << 1) | (intensity << 9) | 0x2000000 | (intensity << 17);
    }
    if (game_settings.vibration)
    {
        SetGamePadActuator(0, 4, 0, 1);
        SetGamePadActuator(1, 4, 1, 255);
    }
    SetScreenFlash(128, 0, 0, 8, 0, 0);
    uint32 sound = RandomRange(4);
    PlaySoundEffect(sound + 4, 8192, 0);
    if (!this->model.animation)
    {
        uint32 range = RandomRange(4);
        uint32 first_frame = hurt_frames[range][0];
        uint32 last_frame = hurt_frames[range][1];
        SetAnimationRange(6, first_frame, last_frame);
    }
    return 1;
}

uint32 *Player::ApplyContactImpulse(const VisualObject &object)
{
    Vector3 difference, shifted, scaled, divided, impulse;
    uint32 distance, left_shift, right_shift;
    FUNCTION_MARKER(0x8005E07C, "SLUS_003.73");
    SubtractVectors(difference.words, model.pos.words, object.model.pos.words);
    difference.words[1] = 0;
    distance = MeasureVectorLength(difference.words);
    if (!distance)
        distance = 1;
    left_shift = 12;
    ShiftVectorRight(shifted.words, difference.words, left_shift);
    MultiplyScalarVector(scaled.words, object.contact_strength, shifted.words);
    DivideVector(divided.words, scaled.words, distance);
    right_shift = 12;
    ShiftVectorLeft(impulse.words, divided.words, right_shift);
    AddVectors(motion.velocity.words, impulse.words);
    return motion.velocity.words;
}

uint32 Player::ApplyBeamContactImpulse(const BeamBarrier &barrier)
{
    uint32 left, right, x, z, beam_angle, player_angle, subtract;
    Vector3 impulse;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8005E154, "SLUS_003.73");
    left = barrier.target.words[2];
    right = barrier.model.pos.words[2];
    x = barrier.target.words[0];
    z = barrier.model.pos.words[0];
    beam_angle = (1024 - (uint32)ratan2((sint32)(left - right), (sint32)(x - z))) & 4095;
    left = pos_history.prev.words[2];
    right = barrier.model.pos.words[2];
    x = pos_history.prev.words[0];
    z = barrier.model.pos.words[0];
    player_angle = (1024 - (uint32)ratan2((sint32)(left - right), (sint32)(x - z))) & 4095;
    subtract = beam_angle < player_angle ? player_angle - beam_angle < 2049 : beam_angle - player_angle >= 2049;
    angles.words[0] = 0;
    angles.words[1] = (uint16)((subtract ? beam_angle - 1024 : beam_angle + 1024) & 4095);
    angles.words[2] = 0;
    BuildDirectionVector(impulse.words, 512, angles.words);
    AddVectors(motion.velocity.words, impulse.words);
    slowdown_ticks = 60;
    slowdown_duration = 60;
    return 60;
}

uint32 Player::ReplaceWeapon(uint32 kind)
{
    FUNCTION_MARKER(0x8005E260, "SLUS_003.73");
    kind &= 65535;
    GameHeapBlock *object;
    uint32 weapon_damage;
    uint32 previous;
    WeaponNode *new_weapon = 0;
    WeaponNode **slot;
    if (kind < 1 || kind > 7)
        return 0;
    if (kind == 1)
        PlayCdVoiceEvent(0, 3, this->model.pos.words, 1);
    else
        PlayCdVoiceEvent(0, RandomRange(2), this->model.pos.words, 1);
    slot = &weapons[kind];
    if (*slot)
        (*slot)->Destroy(3);
    switch (kind)
    {
        case WPN_FLAMETHROWER:
            object = AllocateZeroedHeapBlock(0x48);
            if (object)
            {
                new_weapon = new (object) Flamethrower(&visual_lists.actors, 0);
            }
            weapon_damage = game_settings.difficulty ? 10 : 15;
            break;
        case WPN_PULSE_LASER:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) PulseLaser(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 10 : 15;
            break;
        case WPN_PARTICLE_BEAM:
            object = AllocateZeroedHeapBlock(0x68);
            if (object)
            {
                new_weapon = new (object) ParticleBeam(&visual_lists.actors, 3000, 0, 256, 9);
            }
            weapon_damage = game_settings.difficulty ? 10 : 15;
            break;
        case WPN_RIP_LASER:
            object = AllocateZeroedHeapBlock(0x64);
            if (object)
            {
                new_weapon = new (object) RipLaser(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 2 : 3;
            break;
        case WPN_GRENADES:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) Grenades(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 120 : 180;
            break;
        case WPN_HOMING_MISSILES:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) HomingMissiles(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 15 : 22;
            break;
        default:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) Rockets(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 40 : 60;
            break;
    }
    *slot = new_weapon;
    // TODO Original allocation failure leaves no weapon for the damage store
    new_weapon->damage = weapon_damage;
    previous = this->sel_weapon;
    this->sel_weapon = kind;
    this->prev_weapon = previous;
    return 1;
}

uint32 Player::EnterDeathState()
{
    FUNCTION_MARKER(0x8005E708, "SLUS_003.73");
    PlaySoundEffect(8, 0x2000, 0);
    PrepareDeathCamera();
    uint32 state = this->movement_state, animation;
    this->health = (sint16)(uint16)(0);
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
            return Finish();
        }
    }
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
    this->movement_state = 0x40000;
    return SetAnimationRange(animation, 0, (uint32)-1);
}

void Player::PrepareDeathCamera()
{
    uint32 mode, angle;
    Camera *camera;
    sint32 distance;
    FUNCTION_MARKER(0x8005E90C, "SLUS_003.73");
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    if (!camera)
        return;
    if (!this->death_camera_enabled)
        return;
    this->death_ticks = 0;
    SetCameraOrbitDistanceTransition(64, 32);
    mode = level_environment.area_kind;
    distance = mode == 10 ? -1400 : -700;
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    SetCameraOrbitHeightTransition((uint32)distance, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    SetCameraTargetLateralOffsetTransition(0, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    SetCameraTargetHeightOffsetTransition(0, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    SetCameraTargetForwardOffsetTransition(0, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    angle = camera->angles.words[1];
    camera->SetYawTransition((angle + 1024) & 0xFFF, 34);
}

uint32 Player::InitializeAnimation()
{
    FUNCTION_MARKER(0x8005E9E4, "SLUS_003.73");
    SetAnimationRange(0, 0, (uint32)-1);
    this->movement_state = 1;
    return 1;
}

uint32 Player::TryEnterMovementState16()
{
    uint32 animation;
    FUNCTION_MARKER(0x8005EA1C, "SLUS_003.73");
    if ((sint8)this->move_input[0] >= 0)
        return 0;
    if ((uint32)(this->move_heading - (uint32)768) < 2561)
        return 0;
    animation = this->model.animation;
    this->movement_state = 16;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState32()
{
    uint32 animation;
    FUNCTION_MARKER(0x8005EA80, "SLUS_003.73");
    if ((sint8)this->move_input[0] <= 0)
        return 0;
    sint32 angle = (sint16)this->move_heading;
    if (angle < 1281 || angle >= 2816)
        return 0;
    animation = this->model.animation;
    this->movement_state = 32;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState512()
{
    uint32 animation;
    FUNCTION_MARKER(0x8005EAF0, "SLUS_003.73");
    if ((sint8)this->move_input[1] >= 0)
        return 0;
    sint32 angle = (sint16)this->move_heading;
    if (angle < 2816 || angle >= 3329)
        return 0;
    animation = this->model.animation;
    this->movement_state = 512;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState1024()
{
    uint32 animation;
    FUNCTION_MARKER(0x8005EB60, "SLUS_003.73");
    if ((sint8)this->move_input[1] <= 0)
        return 0;
    sint32 angle = (sint16)this->move_heading;
    if (angle < 768 || angle >= 1281)
        return 0;
    animation = this->model.animation;
    this->movement_state = 1024;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryActivateOneShotActivator()
{
    ObjectBorrow borrow;
    FUNCTION_MARKER(0x8005EBD0, "SLUS_003.73");
    if (!one_shot_activator)
        return 0;
    if (!(this->aim_input[0] | this->aim_input[1]))
        return 0;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        motion.velocity.words[axis] = 0;
        motion.acceleration.words[axis] = 0;
        model.angles.words[axis] = one_shot_activator->model.angles.words[axis];
    }
    SetAnimationRange(7, 0, (uint32)-1);
    this->movement_state = 65536;
    if (!one_shot_activator)
    {
        GameDiagnostic("TODO Interaction expired during player activation\n");
        abort();
    }
    one_shot_activator->Activate();
    return 1;
}

uint32 Player::TryActivateResettingActivator()
{
    ObjectBorrow borrow;
    FUNCTION_MARKER(0x8005EC7C, "SLUS_003.73");
    if (!resetting_activator)
        return 0;
    if (!(this->aim_input[0] | this->aim_input[1]))
        return 0;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        motion.velocity.words[axis] = 0;
        motion.acceleration.words[axis] = 0;
        model.angles.words[axis] = resetting_activator->model.angles.words[axis];
    }
    SetAnimationRange(18, 0, (uint32)-1);
    this->movement_state = 131072;
    if (!resetting_activator)
    {
        GameDiagnostic("TODO Interaction expired during player activation\n");
        abort();
    }
    resetting_activator->BeginActivation();
    return 1;
}

uint32 Player::TryEnterActionState()
{
    GamePadInput *input;
    uint32 state, x, z, angle, heading, flags, sound;
    Camera *camera;
    FUNCTION_MARKER(0x8005ED30, "SLUS_003.73");
    input = this->pad;
    if (!input->buttons[16].down)
        return 0;
    state = this->movement_state;
    if (state == 256)
    {
        x = this->move_input[0];
        z = this->move_input[1];
        if (!(x | z))
            return 0;
        angle = this->move_heading;
        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
        this->action_heading = angle;
        heading = camera ? camera->angles.words[1] + angle : angle;
        this->model.angles.words[1] = heading & 4095;
        flags = this->model.flags;
        this->movement_state = 2048;
        sound = (flags & 8) ? 55 : 0;
        PlaySoundEffect(sound, 8192, 0);
        SetAnimationRange(12, 6, (uint32)-1);
        return 1;
    }
    if (!(state & 1584))
    {
        flags = this->model.flags & 8;
        this->movement_state = 256;
        if (flags)
            PlaySoundEffect(72, 8192, 0);
        SetAnimationRange(5, 0, (uint32)-1);
        return 1;
    }
    x = this->move_input[0];
    z = this->move_input[1];
    if (!(x | z))
        return 0;
    angle = this->move_heading;
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    this->action_heading = angle;
    heading = camera ? camera->angles.words[1] + angle : angle;
    this->model.angles.words[1] = heading & 4095;
    this->movement_state = 2048;
    SetAnimationRange(12, 0, (uint32)-1);
    sound = (this->model.flags & 8) ? 55 : 0;
    PlaySoundEffect(sound, 8192, 0);
    return 1;
}

uint32 Player::TryEnterRecordMovementState()
{
    FUNCTION_MARKER(0x8005EEDC, "SLUS_003.73");
    if (!rail || runtime.rail_entered)
        return 0;
    if (!rail->Project(model.pos, rail_pos))
        return 0;
    for (uint32 axis = 0; axis < 3; ++axis)
        model.pos.words[axis] = rail_pos.words[axis];
    model.pos.words[1] += 524288;
    uint32 kind = rail->kind;
    if (kind != 400 && kind != 401)
        return 0;
    this->movement_state = kind == 400 ? 32768 : 16384;
    motion.velocity.words[1] = 0;
    model.angles.words[1] = (rail->heading + 2048) & 4095;
    this->pad->buttons[17].pressed = 0;
    on_rail = 1;
    runtime.rail_entered = 1;
    if (kind == 400)
        this->rail_voice = PlaySoundEffect(100, 8192, 0);
    return 1;
}

uint32 Player::TryEnterJumpState()
{
    GamePadInput *input;
    uint32 latch, impulse, velocity, first, last, flags, delay;
    ModelInstance *linked;
    FUNCTION_MARKER(0x8005F00C, "SLUS_003.73");
    input = this->pad;
    if (!input->buttons[17].pressed)
        return 0;
    if (this->jump_latch)
        return 0;
    PlaySoundEffect(5, 8192, 0);
    if (this->model.flags & 8)
        PlaySoundEffect(63, 8192, 0);
    latch = this->jump_latch;
    this->jump_latch = latch + 1;
    impulse = (uint32)GetGameTuning().player.jump_impulse;
    linked = ground.GetObject();
    impulse = (uint32)0 - (impulse << 12);
    this->jump_velocity = impulse;
    if (linked)
    {
        velocity = static_cast<VisualObject *>(linked->owner)->motion.velocity.words[1];
        if ((sint32)velocity < -262144)
            velocity <<= 1;
        this->jump_velocity = impulse + velocity;
    }
    this->movement_state = 2;
    first = (uint32)GetGameTuning().player.jump_first_frame;
    last = (uint32)GetGameTuning().player.jump_last_frame;
    SetAnimationRange(2, first + 1, last);
    flags = this->contact_flags & 1;
    delay = GetGameTuning().player.jump_hold_ticks;
    this->jump_hold_ticks = delay;
    runtime.jump_released = 0;
    if (flags)
        this->move_ramp = 0;
    return 1;
}

uint32 Player::TryEnterAirborneState()
{
    uint32 timer, first, last;
    bool check_timer = true;
    FUNCTION_MARKER(0x8005F124, "SLUS_003.73");
    if (!(this->contact_flags & 2))
    {
        timer = this->ground_grace_ticks;
        if (timer)
            this->ground_grace_ticks = timer - 1;
        else
            check_timer = false;
    }
    if (check_timer && this->ground_grace_ticks)
        return 0;
    this->fall_start_y = this->model.pos.words[1];
    first = (uint32)GetGameTuning().player.jump_last_frame;
    last = (uint32)GetGameTuning().player.airborne_last_frame;
    SetAnimationRange(2, first + 1, last);
    this->movement_state = 4;
    return 1;
}

uint32 Player::TryEnterLandingState()
{
    GamePadInput *input;
    uint32 threshold, distance = 0, numerator, denominator, damage, sound;
    FUNCTION_MARKER(0x8005F1B0, "SLUS_003.73");
    if (!(this->contact_flags & 2))
        return 0;
    threshold = this->fall_damage_threshold;
    this->ground_grace_ticks = 4;
    if (threshold)
        distance = (uint32)((sint32)(this->model.pos.words[1] - this->fall_start_y) >> 12);
    if (threshold && (sint32)threshold < (sint32)distance)
    {
        numerator = (distance - threshold) * (uint32)(sint32)(sint16)GetGameTuning().player.maximum_health;
        denominator = this->fatal_fall_distance - threshold;
        if (!denominator)
            damage = (sint32)numerator < 0 ? 1 : (uint32)-1;
        else if (numerator == 0x80000000 && denominator == (uint32)-1)
            damage = numerator;
        else
            damage = (uint32)((sint32)numerator / (sint32)denominator);
        Damage(damage, zero_vector.components, 0);
        if (this->health <= 0)
            return 1;
    }
    else if (game_settings.vibration)
        SetGamePadActuator(0, 4, 0, 1);
    SetAnimationRange(2, (uint32)GetGameTuning().player.airborne_last_frame + 1, (uint32)-1);
    sound = (this->model.flags & 8) ? 56 : 19;
    PlaySoundEffect(sound, 8192, 0);
    input = this->pad;
    this->movement_state = 8;
    (input->buttons[17].pressed = 0);
    runtime.rail_entered = 0;
    return 1;
}

uint32 Player::TryEnterAnimation10State()
{
    Vector3 scaled, position, offset;
    sint32 angle_z, angle_x;
    uint32 scale = 64, offset_scale = 128, flags, height;
    FUNCTION_MARKER(0x8005F300, "SLUS_003.73");
    if (!(this->contact_flags & 64))
        return 0;
    SetAnimationRange(10, 1, (uint32)-1);
    flags = this->contact_flags & 128;
    this->movement_state = 4096;
    if (!flags)
    {
        this->model.pos.words[1] = (uint32)this->ledge_pos[1] + 524288;
        return 1;
    }
    MultiplyVector(scaled.words, ledge_normal.words, scale);
    AddVectors(position.words, ledge_pos.words, scaled.words);
    this->model.pos = position;
    angle_z = (sint32)(uint32)this->ledge_normal[2];
    height = this->model.pos.words[1];
    angle_x = (sint32)(uint32)this->ledge_normal[0];
    this->model.pos.words[1] = height + 524288;
    this->model.angles.words[1] = (1024 - (uint32)ratan2(angle_z, angle_x)) & 4095;
    MultiplyScalarVector(offset.words, offset_scale, ledge_normal.words);
    SubtractVectors(position.words, this->model.pos.words, offset.words);
    position.components[1] = (sint32)((uint32)position.components[1] - 884736);
    if (!ground.GetObject())
    {
        Camera *camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
        camera->SetTargetTransition(position.words, 36);
    }
    return 1;
}

uint32 Player::UpdateGroundContact()
{
    FUNCTION_MARKER(0x8005F470, "SLUS_003.73");
    sint32 result;
    uint32 water_x;
    uint32 water_y;
    uint32 water_z;
    uint32 ray_z;

    CollisionRay ray;

    result = (movement_state & 0x3000);
    if (result)
        goto DisableProjection;
    if (model.flags & 8)
    {
        water_x = model.pos.words[0];
        projection.enabled = true;
        water_y = (uint32)level_environment.water_y;
        water_z = model.pos.words[2];
        result = -ONE;
        surface.n.vx = 0;
        surface.n.vy = -ONE;
        surface.n.vz = 0;
        surface.pos.words[0] = water_x;
        surface.pos.words[1] = water_y;
        surface.pos.words[2] = water_z;
        return result;
    }
    if (contact_flags & 2)
    {
        projection.enabled = true;
        result = (uint32)ground_offset << 12;
        surface.pos.words[1] = model.pos.words[1] + (uint32)result;
        return result;
    }
    ray.endpoints[0].words[0] = model.pos.words[0];
    ray.endpoints[0].words[1] = model.pos.words[1] - 0x40000;
    ray_z = model.pos.words[2];
    ray.endpoints[1].words[0] = ray.endpoints[0].words[0];
    ray.endpoints[1].words[1] = ray.endpoints[0].words[1] + 0x800000;
    ray.endpoints[0].words[2] = ray_z;
    ray.endpoints[1].words[2] = ray_z;
    InitializeCollisionRay(ray);
    QueryCollisionRay(ray, 1);
    result = 1;
    if (!ray.hit.GetObject())
    {
    DisableProjection:
        projection.enabled = false;
    }
    else
    {
        projection.enabled = true;
        surface.pos.words[0] = ray.hit_pos.words[0];
        surface.pos.words[1] = ray.hit_pos.words[1];
        surface.pos.words[2] = ray.hit_pos.words[2];
        surface.n.vx = ray.n.vx;
        surface.n.vy = ray.n.vy;
        result = (uint16)ray.n.vz;
        surface.n.vz = ray.n.vz;
    }
    return result;
}

void Player::SetWeaponStop(uint32 duration)
{
    uint32 selected;
    FUNCTION_MARKER(0x8005F62C, "SLUS_003.73");
    selected = this->sel_weapon;
    this->weapon_stop_ticks = duration;
    StopSelectedWeapon(selected);
}

uint32 Player::Update()
{
    GamePadInput *cheat_pad;
    GamePadInput *rail_pad;
    GamePadInput *disabled_pad;
    GamePadInput *climb_pad;
    ObjectBorrow borrow;
    Camera *transition_camera;
    uint32 trig_angle;
    uint32 movement_x, movement_z;
    Camera *movement_camera;
    ShortVector3 camera_angles, weapon_angles;
    uint32 aim_axis0, aim_axis1, body_heading, model_heading;
    FUNCTION_MARKER(0x8005F688, "SLUS_003.73");

    sint32 rail_step;
    sint32 weapon_stop_before;
    sint32 result;
    uint32 transition_result;
    ModelInstance *prev_ground;
    sint32 hit_ticks_before;
    sint8 hit_ticks_after;
    sint32 glow_ticks_before;
    sint8 glow_ticks_after;
    sint32 disable_ticks_before;
    sint32 disable_ticks_after;
    sint32 initial_animation;
    sint32 saved_y;
    sint32 saved_z;
    sint32 prev_vertical_velocity;
    sint32 script_motion_mask;
    sint32 area_kind;
    sint16 dry_model_flags;
    sint32 water_damage;
    sint32 ripple_radius;
    GameHeapBlock *ripple_storage;
    sint32 ripple_steps;
    sint32 ripple_size;
    sint32 free_move_y;
    sint32 free_move_z;
    uint32 motion_contacts;
    sint32 animation_start;
    sint32 activation_target_z;
    sint32 ramp_up;
    sint32 ramp_before;
    sint32 ramp_after;
    uint32 movement_state;
    sint32 move_octant;
    sint32 idle_ticks;
    sint32 idle_quote0;
    sint32 idle_quote1;
    sint32 jump_ready;
    ModelInstance *jump_platform;
    sint32 jump_impulse;
    sint32 next_state;
    sint32 activator_pos_x;
    sint32 activator_target_x;
    sint32 activator_pos_z;
    sint32 reset_pos_x;
    sint32 reset_target_x;
    sint32 reset_pos_z;
    sint16 death_tick_before;
    sint16 death_tick_after;
    sint32 rail_animation;
    sint32 rail_frame3;
    sint32 rail_frame33;
    sint32 rail_frame33_event;
    sint32 air_animation;
    bool restart_air_animation;
    sint32 climb_complete;
    sint32 ledge_ground_y;
    sint16 ledge_contact_flags;
    sint32 move_axis;
    sint32 crouch_frame;
    sint32 aim_octant;
    sint32 move_angle;
    sint32 move_angle_word;
    sint32 move_angle_offset;
    sint32 move_angle_bin;
    sint16 camera_yaw;
    sint32 aim_angle;
    sint32 aim_angle_word;
    sint32 aim_angle_offset;
    sint32 aim_angle_bin;
    sint32 move_idx;
    sint32 aim_idx;
    sint32 facing_state;
    sint32 facing_angle;
    bool immediate_facing;
    sint32 desired_heading;
    sint32 direction_animation;
    sint32 animation_dir;
    sint32 idle_animation;
    uint32 acceleration_state;
    bool velocity_changed;
    sint32 velocity_state;
    sint16 rail_forward_speed;
    sint16 rail_heading;
    sint16 rail_backward_speed;
    sint32 move_strength;
    sint32 rail_state;
    sint32 rail_sound;
    bool platform_moved;
    ModelInstance *air_platform;
    sint32 air_platform_velocity_x;
    sint32 platform_carried_x;
    ModelInstance *platform_z_owner;
    ModelInstance *ground_platform;
    sint32 ground_velocity_x;
    ModelInstance *platform_y_owner;
    sint32 platform_carried_y;
    sint32 footstep_sound;
    sint32 footstep_animation;
    sint32 footstep_crossed;
    sint32 footstep_event;
    sint32 played_footstep_sound;
    sint32 footstep_frame;
    sint32 footstep_animation_id;
    sint32 jump_hold_before;
    sint8 jump_hold_after;
    sint32 slowdown_intensity;
    sint32 slowdown_green;
    sint16 slowdown_remaining;
    ModelInstance *contact;
    ModelInstance *pickup;
    sint32 cleanup_state;
    Vector3 move_work;
    MATRIX inverse_camera_matrix;
    MATRIX camera_matrix;
    Vector3 camera_velocity, world_velocity, rail_point;
    Vector3 platform_pos, frame_delta, trace_dir;
    CollisionRay platform_ray, motion_ray;
    Vector3 trace_offset, trace_start;
    const uint32 ledge_offset_scale = 128;
    const uint32 velocity_shift = 6;
    const uint32 trace_offset_scale = 16;
    weapon_stop_before = this->weapon_stop_ticks;
    result = (sint32)((uint32)weapon_stop_before - 1);
    if (weapon_stop_before)
    {
        this->weapon_stop_ticks = result;
        return result;
    }
    prev_ground = ground.GetObject();
    if (prev_ground && ((static_cast<VisualObject *>(prev_ground->owner)->lifecycle_flags & 0x40) != 0))
        ground.Reset();
    collision_query.allow_surface20 = false;
    hit_ticks_before = this->hit_reaction_ticks;
    hit_ticks_after = hit_ticks_before - 1;
    if (hit_ticks_before)
        this->hit_reaction_ticks = hit_ticks_after;
    glow_ticks_before = this->damage_glow_ticks;
    glow_ticks_after = glow_ticks_before - 1;
    if (glow_ticks_before)
    {
        this->damage_glow_ticks = glow_ticks_after;
        if (glow_ticks_after)
        {
            uint32 intensity = this->damage_glow_ticks;
            model_effect.color_mode = intensity | (intensity << 8) | 0x2000000 | (intensity << 16);
        }
        else
            model_effect.color_mode = 0;
    }
    disable_ticks_before = this->disable_timer;
    disable_ticks_after = (sint32)((uint32)disable_ticks_before - 1);
    if (disable_ticks_before)
    {
        disabled_pad = this->pad;
        this->disable_timer = disable_ticks_after;
        (disabled_pad->buttons[8].down = 0);
        this->pad->buttons[9].down = 0;
        this->pad->buttons[10].down = 0;
        this->pad->buttons[11].down = 0;
        this->pad->buttons[0].down = 0;
        this->pad->buttons[2].down = 0;
        this->pad->buttons[3].down = 0;
        this->pad->buttons[1].down = 0;
        this->pad->buttons[15].down = 0;
        this->pad->buttons[4].down = 0;
        this->pad->buttons[5].down = 0;
        this->pad->buttons[6].down = 0;
        this->pad->buttons[7].down = 0;
        this->pad->buttons[17].pressed = 0;
        this->pad->buttons[5].pressed = 0;
    }
    initial_animation = this->model.animation;
    this->playback.speed = 0x10000;
    if ((initial_animation == 5) && ((uint8)this->playback.dir == 1))
        this->playback.speed = 0x20000;
    if (GetCheatOptions().enemy_finish_cheat)
    {
        cheat_pad = this->pad;
        if (cheat_pad->buttons[5].pressed)
        {
            (cheat_pad->buttons[5].pressed = 0);
            ::FinishSelectedEnemies();
        }
    }
    result = this->update_disabled;
    if (!result)
    {
        this->model.flags = (this->model.flags | 4);
        uint32 saved_x = this->model.pos.words[0];
        saved_y = this->model.pos.words[1];
        saved_z = this->model.pos.words[2];
        pos_history.prev.words[0] = saved_x;
        pos_history.prev.words[1] = saved_y;
        pos_history.prev.words[2] = saved_z;
        script_motion_mask = this->movement_state & 0xF000;
        prev_vertical_velocity = motion.velocity.words[1];
        this->model.color = 8421504;
        if (!script_motion_mask)
            ResolvePlayerCollisionMotion(*this);
        dry_model_flags = this->model.flags & 0xFFF7;
        area_kind = level_environment.area_kind;
        this->model.flags = dry_model_flags;
        uint32 water_contact = 0;
        if (area_kind == 1 || area_kind == 5)
        {
            uint32 offset = ground_offset;
            uint32 height = this->model.pos.words[1];
            uint32 surface = (uint32)level_environment.water_y;
            water_contact = (sint32)surface < (sint32)(height + (offset << 12));
        }
        if (water_contact)
        {
            water_damage = level_environment.water_damage;
            this->model.flags = (dry_model_flags | 8);
            if (!water_damage)
            {
                if ((game_clock.tick & 3) == 0)
                {
                    uint32 surface = (uint32)level_environment.water_y;
                    move_work.components[0] = this->model.pos.words[0];
                    move_work.components[1] = this->model.pos.words[1];
                    move_work.components[2] = this->model.pos.words[2];
                    move_work.components[1] = surface;
                    ripple_radius = RandomRange(32) + 32;
                    trig_angle = RandomRange(4096) & 0xFFF;
                    move_work.components[0] = (sint32)((uint32)move_work.components[0] + (uint32)ripple_radius * (uint32)rsin(trig_angle));
                    move_work.components[2] = (sint32)((uint32)move_work.components[2] + (uint32)ripple_radius * (uint32)rcos(trig_angle));
                    ripple_storage = AllocateEffectStorage(116);
                    if (ripple_storage)
                    {
                        ripple_size = RandomRange(32) + 32;
                        ripple_steps = RandomRange(4);
                        new (ripple_storage) ExpandingRing(move_work.components, ripple_size, 16, 64, 16, 12, (uint32)ripple_steps + 4);
                    }
                }
            }
            else
            {
                Damage(water_damage, zero_vector.components, 0);
            }
        }
        if ((GetCheatOptions().free_movement && game_pads[0].buttons[4].down) && game_pads[0].buttons[6].down)
        {
            free_move_y = pos_history.prev.words[1];
            this->model.pos.words[1] = free_move_y;
            if (game_pads[0].buttons[0].down)
                this->model.pos.words[1] = (free_move_y - 393216);
            if (game_pads[0].buttons[3].down)
                this->model.pos.words[1] = (this->model.pos.words[1] + (393216));
            free_move_z = this->model.pos.words[2] + (5 * (this->model.pos.words[2] - pos_history.prev.words[2]));
            this->model.pos.words[0] = (this->model.pos.words[0] + ((5 * (this->model.pos.words[0] - pos_history.prev.words[0]))));
            this->model.pos.words[2] = free_move_z;
        }
        motion_contacts = this->contact_flags;
        this->collision_radius = 100;
        this->model.angles.words[2] = 0;
        this->model.angles.words[0] = 0;
        if (((((uint8)((motion_contacts >> 1))) ^ 1) & 1) != 0)
        {
            this->airborne_ticks = this->airborne_ticks + 1;
        }
        else
        {
            this->airborne_ticks = 0;
            this->jump_latch = 0;
        }
        if (((!GetCheatOptions().free_movement && (this->model.animation == 17)) && this->playback.complete) && !(this->fall_damage_threshold))
            EnterDeathState();
        ReadInput();
        if (this->move_input[0] | this->move_input[1])
        {
            ramp_up = this->move_ramp;
            if (ramp_up < 8)
                this->move_ramp = (ramp_up + 1);
        }
        else
        {
            ramp_before = this->move_ramp;
            ramp_after = (sint32)((uint32)ramp_before - 1);
            if (ramp_before)
                this->move_ramp = ramp_after;
        }
        movement_state = this->movement_state;
        this->weapons_enabled = 1;
        if (movement_state == 512)
        {
            this->idle_ticks = 0;
            transition_result = TryEnterMovementState16();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState32();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState1024();
            goto CheckActionTransitions;
        }
        if (movement_state < 0x201)
        {
            if (movement_state == 8)
            {
                this->idle_ticks = 0;
                transition_result = TryEnterMovementState16();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState32();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState512();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState1024();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterActionState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                move_octant = 0;
                if (!this->playback.complete)
                    goto SelectDirectionalAnimation;
            ReturnToIdleAnimation:
                InitializeAnimation();

                move_octant = 0;
                goto SelectDirectionalAnimation;
            }
            if (movement_state < 9)
            {
                if (movement_state != 2)
                {
                    if (movement_state >= 3)
                    {
                        if (movement_state == 4)
                        {
                            this->idle_ticks = 0;
                            if (this->airborne_ticks < 49 || this->model.animation != 2)
                            {
                                air_animation = this->model.animation;
                                if (air_animation != 17)
                                {
                                    restart_air_animation = false;
                                    if (((sint32)(motion.velocity.words[1] ^ (uint32)prev_vertical_velocity) < 0) || (air_animation != 2))
                                        restart_air_animation = true;
                                    if (restart_air_animation)
                                    {
                                        this->fall_start_y = this->model.pos.words[1];
                                        SetAnimationRange(2, ((sint32)GetGameTuning().player.jump_last_frame + 1), (sint32)GetGameTuning().player.airborne_last_frame);
                                        motion.drag[1] = GetGameTuning().player.airborne_drag;
                                    }
                                }
                            }
                            else
                            {
                                SetAnimationRange(17, 0, -1);
                            }
                            transition_result = TryEnterLandingState();
                            move_octant = 0;
                            if (!transition_result)
                            {
                                transition_result = TryEnterRecordMovementState();
                                move_octant = 0;
                                if (!transition_result)
                                {
                                    TryEnterAnimation10State();
                                    move_octant = 0;
                                }
                            }
                        }
                        else
                        {
                            move_octant = 0;
                        }
                    }
                    else
                    {
                        move_octant = 0;
                        if (movement_state == 1)
                        {
                            transition_result = TryEnterMovementState16();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterMovementState32();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterMovementState512();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterMovementState1024();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterJumpState();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterAirborneState();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterActionState();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryActivateOneShotActivator();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryActivateResettingActivator();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            if (this->playback.complete)
                                SetAnimationRange(0, 0, -1);
                            idle_ticks = (this->idle_ticks + 1) & 65535;
                            this->idle_ticks = idle_ticks;
                            if (idle_ticks == 1350)
                            {
                                idle_quote0 = RandomRange(8);
                                ::SelectCdVoice(8, idle_quote0);
                                move_octant = 0;
                                goto SelectDirectionalAnimation;
                            }
                            move_octant = 0;
                            if (idle_ticks == 4050)
                            {
                                idle_quote1 = RandomRange(8);
                                PlayCdVoiceEvent(8, idle_quote1, model.pos.words, 0);
                                this->idle_ticks = 1350;
                            ClearMovementOctant:
                                move_octant = 0;
                                goto SelectDirectionalAnimation;
                            }
                        }
                    }
                    goto SelectDirectionalAnimation;
                }
                this->idle_ticks = 0;
                transition_result = TryEnterRecordMovementState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                if (TryEnterAnimation10State())
                    goto ClearMovementOctant;
                if (!(this->pad->buttons[17].down))
                    runtime.jump_released = 1;
                next_state = 4;
                if (this->jump_hold_ticks)
                {
                    move_octant = 0;
                    if (this->pad->buttons[17].down)
                        goto SelectDirectionalAnimation;
                    next_state = 4;
                }
            ApplyMovementState:
                this->movement_state = next_state;

                goto ClearMovementOctant;
            }
            if (movement_state == 32)
            {
                this->idle_ticks = 0;
                transition_result = TryEnterMovementState512();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState1024();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterActionState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateOneShotActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateResettingActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                move_axis = (sint8)(this->move_input[0]);
                goto CheckMovementInput;
            }
            if (movement_state < 0x21)
            {
                move_octant = 0;
                if (movement_state != 16)
                    goto SelectDirectionalAnimation;
                this->idle_ticks = 0;
                transition_result = TryEnterMovementState512();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState1024();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterActionState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateOneShotActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateResettingActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                move_axis = (sint8)(this->move_input[0]);
            CheckMovementInput:
                move_octant = 0;

                if (move_axis)
                    goto SelectDirectionalAnimation;
                goto ReturnToIdleAnimation;
            }
            if (movement_state == 128)
            {
                jump_ready = this->playback.complete;
                this->idle_ticks = 0;
                if (jump_ready)
                {
                    jump_platform = ground.GetObject();
                    jump_impulse = (sint32)(0u - ((uint32)GetGameTuning().player.jump_impulse << 12));
                    this->jump_velocity = jump_impulse;
                    if (jump_platform)
                        this->jump_velocity = (jump_impulse + static_cast<VisualObject *>(jump_platform->owner)->motion.velocity.words[1]);
                    this->movement_state = 2;
                    this->jump_hold_ticks = GetGameTuning().player.jump_hold_ticks;
                }
                goto ClearMovementOctant;
            }
            move_octant = 0;
            if (movement_state != 256)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterJumpState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            this->idle_ticks = 0;
            this->collision_radius = 50;
            transition_result = TryEnterActionState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            animation_start = 21;
            if (this->pad->buttons[16].down)
                goto SelectDirectionalAnimation;
            if (((sint8)(this->model.frame)) < 21)
                animation_start = (sint8)(this->model.frame);
        ReverseCrouchAnimation:
            SetAnimationRange(5, animation_start, 0);

            next_state = 1;
            goto ApplyMovementState;
        }
        if (movement_state == 0x4000)
        {
            rail_animation = this->model.animation;
            this->idle_ticks = 0;
            if (rail_animation != 13)
                goto ClearMovementOctant;
            rail_frame3 = DidAnimationCrossFrame(13, 3);
            if ((UpdateEventLatch(rail_frame3, 1) || ((rail_frame33 = DidAnimationCrossFrame(13, 33)), (rail_frame33_event = UpdateEventLatch(rail_frame33, 1)), (move_octant = 0), rail_frame33_event)))
            {
                static_cast<FlexibleTraversalRail *>(rail.Get())->ApplyRandomOffset(64);
                move_octant = 0;
            }
            goto SelectDirectionalAnimation;
        }
        if (movement_state < 0x4001)
        {
            if (movement_state == 2048)
            {
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                this->idle_ticks = 0;
                this->collision_radius = 50;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                crouch_frame = (sint8)(this->model.frame);
                if ((crouch_frame < 21) && (this->pad->buttons[16].down || (crouch_frame < 11)))
                    goto SelectDirectionalAnimation;
                next_state = 256;
                if (this->pad->buttons[16].down)
                    goto ApplyMovementState;
                animation_start = 21;
                goto ReverseCrouchAnimation;
            }
            if (movement_state >= 0x801)
            {
                if (movement_state == 4096)
                {
                    this->idle_ticks = 0;
                    this->movement_state = 0x2000;
                    this->playback.speed = 111408;
                    SetAnimationRange(11, 0, -1);
                    move_octant = 0;
                    goto SelectDirectionalAnimation;
                }
                move_octant = 0;
                if (movement_state == 0x2000)
                {
                    climb_complete = this->playback.complete;
                    this->idle_ticks = 0;
                    this->playback.speed = 111408;
                    if (!climb_complete)
                        goto ClearMovementOctant;
                    SetAnimationRange(0, 0, -1);

                    MultiplyScalarVector(move_work.words, ledge_offset_scale, ledge_normal.words);
                    SubtractVectors(model.pos.words, move_work.words);
                    climb_pad = this->pad;
                    this->model.pos.words[1] = (this->model.pos.words[1] - (884736));
                    (climb_pad->buttons[17].pressed = 0);
                    ledge_ground_y = FindGroundHeight(model.pos.words, 0, 2048);
                    if (ledge_ground_y != -1)
                    {
                        ledge_contact_flags = this->contact_flags | 2;
                        this->model.pos.words[1] = (ledge_ground_y - (ground_offset << 12));
                        this->contact_flags = ledge_contact_flags;
                    }
                    this->ground_grace_ticks = 4;
                    transition_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    this->movement_state = 1;
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[0] = 0;
                    transition_camera->ClearTargetTransition();
                    move_octant = 0;
                }
                goto SelectDirectionalAnimation;
            }
            move_octant = 0;
            if (movement_state != 1024)
                goto SelectDirectionalAnimation;
            this->idle_ticks = 0;
            transition_result = TryEnterMovementState16();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState32();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState512();
        CheckActionTransitions:
            move_octant = 0;

            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterJumpState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterAirborneState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterActionState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryActivateOneShotActivator();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryActivateResettingActivator();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            move_axis = (sint8)(this->move_input[1]);
            goto CheckMovementInput;
        }
        if (movement_state == 0x10000)
        {
            OneShotActivator *one_shot = one_shot_activator.Get();
            if (!one_shot)
            {
                GameDiagnostic("TODO Missing active one-shot activator\n");
                abort();
            }
            activator_pos_x = this->model.pos.words[0];
            this->idle_ticks = 0;
            activator_target_x = one_shot->activation_pos.words[0];
            if ((sint32)Abs32((sint32)((uint32)activator_target_x - (uint32)activator_pos_x) >> 3) <= 0x7FFF)
                this->model.pos.words[0] = activator_target_x;
            else
                this->model.pos.words[0] = ((uint32)activator_pos_x + ((sint32)((uint32)activator_target_x - (uint32)activator_pos_x) >> 3));
            activator_pos_z = this->model.pos.words[2];
            activation_target_z = one_shot_activator->activation_pos.words[2];
            if ((sint32)Abs32((sint32)((uint32)activation_target_z - (uint32)activator_pos_z) >> 3) <= 0x7FFF)
                this->model.pos.words[2] = activation_target_z;
            else
                this->model.pos.words[2] = ((uint32)activator_pos_z + ((sint32)((uint32)activation_target_z - (uint32)activator_pos_z) >> 3));
            if ((((uint32)(this->model.frame)) - 25) < 0x19)
            {
                if (one_shot_activator)
                {
                    one_shot_activator->model.angles.words[2] = (one_shot_activator->model.angles.words[2] - 28) & 4095;
                    if (this->model.frame == 32)
                        one_shot_activator->SpawnObjects();
                }
            }
            if (this->playback.complete)
                this->movement_state = 1;
            goto ClearMovementOctant;
        }
        if (movement_state <= 0x10000)
        {
            move_octant = 0;
            if (movement_state == 0x8000)
            {
                this->idle_ticks = 0;
                goto ClearMovementOctant;
            }
        SelectDirectionalAnimation:
            aim_octant = 0;

            if (this->move_input[0] | this->move_input[1])
            {
                move_angle = this->move_heading;
                move_angle_word = (uint32)move_angle << 16;
                if (((uint32)((move_angle - 257))) < 0xDFF)
                {
                    move_angle = (sint16)(move_angle);
                    move_angle_offset = (move_angle_word >> 16) - 256;
                    move_angle_bin = move_angle_offset >> 9;
                    if (move_angle_offset < 0)
                        move_angle_bin = (move_angle + 255) >> 9;
                    move_octant = move_angle_bin + 2;
                }
                else
                {
                    move_octant = 1;
                }
            }
            camera_yaw = 0;
            if (this->aim_input[0] | this->aim_input[1])
            {
                aim_angle = this->aim_heading;
                aim_angle_word = (uint32)aim_angle << 16;
                if (((uint32)((aim_angle - 257))) < 0xDFF)
                {
                    aim_angle = (sint16)(aim_angle);
                    aim_angle_offset = (aim_angle_word >> 16) - 256;
                    aim_angle_bin = aim_angle_offset >> 9;
                    if (aim_angle_offset < 0)
                        aim_angle_bin = (aim_angle + 255) >> 9;
                    aim_octant = aim_angle_bin + 2;
                    camera_yaw = 0;
                }
                else
                {
                    aim_octant = 1;
                }
            }
            move_idx = move_octant - 1;
            aim_idx = aim_octant - 1;
            movement_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
            if (movement_camera)
                camera_yaw = movement_camera->angles.words[1];
            facing_state = this->movement_state;
            if ((facing_state & 0x630) != 0)
            {
                if (aim_octant)
                {
                    const DirectionAnimation &entry = direction_animations[move_idx];
                    direction_animation = entry.animation[aim_idx];
                    animation_dir = entry.direction[aim_idx];
                    if ((direction_animation != this->model.animation) || (animation_dir != this->playback.dir))
                    {
                        StartAnimation(direction_animation, animation_dir);
                    }
                    immediate_facing = false;
                    facing_angle = entry.facing[aim_idx];
                    uint32 model_offset = entry.body_heading[aim_idx];
                    uint32 movement_heading = this->move_heading;
                    desired_heading = (sint16)((uint32)camera_yaw + model_offset + movement_heading - ((uint32)move_idx << 9));
                }
                else
                {
                    if (((this->model.animation != 1) || ((facing_angle = 0), ((uint8)this->playback.dir != 1))))
                    {
                        StartAnimation(1, 1);
                        facing_angle = 0;
                    }
                    immediate_facing = false;
                    desired_heading = (sint16)((camera_yaw + this->move_heading));
                }
                goto ApplyFacingTransition;
            }
            if (facing_state != 1)
            {
                if (facing_state == 256)
                {
                    if (aim_octant)
                    {
                        immediate_facing = false;
                        facing_angle = (((uint16)direction_animations[9].facing[aim_idx] + this->aim_heading) - (((uint16)(aim_idx)) << 9)) & 0xFFF;
                        desired_heading = (sint16)((camera_yaw + direction_animations[9].body_heading[aim_idx]));
                    }
                    else
                    {
                        facing_angle = 0;
                        desired_heading = (sint16)(this->model.angles.words[1]);
                        immediate_facing = false;
                    }
                    goto ApplyFacingTransition;
                }
                if ((facing_state & 0x8E) != 0)
                {
                    if (aim_octant)
                    {
                        immediate_facing = false;
                        facing_angle = (((uint16)direction_animations[9].facing[aim_idx] + this->aim_heading) - (((uint16)(aim_idx)) << 9)) & 0xFFF;
                        desired_heading = (sint16)((camera_yaw + direction_animations[9].body_heading[aim_idx]));
                    }
                    else
                    {
                        facing_angle = 0;
                        immediate_facing = false;
                        desired_heading = (sint16)((camera_yaw + this->move_heading));
                    }
                    goto ApplyFacingTransition;
                }
                if (facing_state == 0x4000)
                {
                    if (aim_octant || !((this->move_input[0] | this->move_input[1])))
                    {
                        if (this->model.animation != 14)
                            SetAnimationRange(14, 0, -1);
                        if (!aim_octant)
                        {
                        AdvanceFacingTransition:
                            if (this->facing_ticks)
                            {
                                ModelJointAngles &facing = GetModelJointAngles(7);
                                uint32 step = (uint16)this->facing_step;
                                uint32 yaw = facing.rotation.words[1];
                                facing.rotation.words[1] = (uint16)((yaw + step) & 4095);
                                uint32 remaining = this->facing_ticks - 1;
                                this->facing_ticks = remaining;
                                if (!remaining)
                                {
                                    uint32 target = (uint16)this->facing_target;
                                    facing.rotation.words[1] = (uint16)target;
                                }
                            }

                            acceleration_state = this->movement_state;
                            motion.acceleration.words[2] = 0;
                            motion.acceleration.words[0] = 0;
                            if (acceleration_state != 0x4000)
                            {
                                if (acceleration_state >= 0x4001)
                                {
                                    if (acceleration_state != 0x8000)
                                    {
                                    ApplyGravity:
                                        motion.acceleration.words[1] = ((uint32)GetGameTuning().player.gravity << 12);

                                    TransformMovement:
                                        if ((this->movement_state & 0x80000) == 0)
                                        {
                                            motion.angular_velocity = zero_angles;
                                            motion.angular_acceleration = zero_angles;
                                        }

                                        velocity_changed = false;
                                        movement_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                                        if (movement_camera)
                                        {
                                            camera_angles = movement_camera->angles;
                                            BuildModelRotationMatrix(camera_angles.words, camera_matrix);
                                        }
                                        else
                                        {
                                            // Use the approved identity transform when the camera is absent
                                            memset(&camera_matrix, 0, sizeof(camera_matrix));
                                            camera_matrix.m[0][0] = ONE;
                                            camera_matrix.m[1][1] = ONE;
                                            camera_matrix.m[2][2] = ONE;
                                        }
                                        TransposeRotationMatrix(camera_matrix, inverse_camera_matrix);
                                        SetRotMatrix(&inverse_camera_matrix);

                                        ShiftVectorRight(camera_velocity.words, motion.velocity.words, velocity_shift);

                                        move_work = camera_velocity;
                                        xport_gte_write_data(9, (uint32)move_work.components[0]);
                                        xport_gte_write_data(10, (uint32)move_work.components[1]);
                                        xport_gte_write_data(11, (uint32)move_work.components[2]);
                                        xport_gte_execute(0x49E012);
                                        move_work.components[0] = (sint32)xport_gte_read_data(25);
                                        move_work.components[1] = (sint32)xport_gte_read_data(26);
                                        move_work.components[2] = (sint32)xport_gte_read_data(27);

                                        ShiftVectorLeft(move_work.words, velocity_shift);
                                        velocity_state = this->movement_state;
                                        if ((velocity_state & 0x71100) == 0)
                                        {
                                            switch (velocity_state)
                                            {
                                                case 0x800:
                                                    if (((sint8)(this->model.frame)) < 20)
                                                    {
                                                        move_work.components[0] = (sint32)(0u - ((uint32)(sint32)(sint16)GetGameTuning().player.move_speed * (uint32)rsin(this->action_heading & 0xFFFu)));
                                                        velocity_changed = true;
                                                        move_work.components[2] = (sint32)(0u - ((uint32)(sint32)(sint16)GetGameTuning().player.move_speed * (uint32)rcos(this->action_heading & 0xFFFu)));
                                                    }
                                                    break;

                                                case 0x4000:
                                                    if (!((this->aim_input[0] | this->aim_input[1])) && this->move_input[0])
                                                    {
                                                        if ((((rail->heading - (camera_yaw + this->move_heading)) & 0xFFFu) - 1025) < 0x7FF)
                                                        {
                                                            rail_backward_speed = GetGameTuning().player.move_speed;
                                                            this->playback.speed = 0x20000;
                                                            rail_step = rail_backward_speed / 4;
                                                            rail_heading = (rail->heading + 2048) & 0xFFF;
                                                        }
                                                        else
                                                        {
                                                            rail_forward_speed = GetGameTuning().player.move_speed;
                                                            this->playback.speed = 0x20000;
                                                            rail_heading = rail->heading;
                                                            rail_step = rail_forward_speed / -4;
                                                        }
                                                        this->model.angles.words[1] = rail_heading;
                                                        velocity_changed = true;
                                                    }
                                                    break;

                                                case 0x8000:
                                                    move_work.components[0] = ((sint32)(sint16)GetGameTuning().player.move_speed / 4) * 8u * (uint32)rsin((rail->heading - camera_yaw) & 0xFFFu);
                                                    velocity_changed = true;
                                                    move_work.components[2] = ((sint32)(sint16)GetGameTuning().player.move_speed / 4) * 8u * (uint32)rcos((rail->heading - camera_yaw) & 0xFFFu);
                                                    break;

                                                default:
                                                    movement_x = this->move_input[0];
                                                    movement_z = this->move_input[1];

                                                    if (movement_x | movement_z)
                                                    {
                                                        move_strength = Abs32(((sint32)(sint8)movement_x * 4096) / 96) + Abs32(((sint32)(sint8)movement_z * 4096) / 96);
                                                        if (move_strength >= 4097)
                                                            move_strength = 4096;
                                                        if (movement_x)
                                                        {
                                                            move_work.components[2] = (sint32)(0u - ((uint32)((sint32)((uint32)rcos(this->move_heading & 0xFFFu) * (uint32)move_strength) >> 12) * (uint32)(sint32)(sint16)GetGameTuning().player.move_speed));
                                                            move_work.components[2] = (sint32)((uint32)move_work.components[2] * this->move_ramp) / 8;
                                                            velocity_changed = true;
                                                        }
                                                        if (this->move_input[1])
                                                        {
                                                            move_work.components[0] = (sint32)(0u - ((uint32)((sint32)((uint32)rsin(this->move_heading & 0xFFFu) * (uint32)move_strength) >> 12) * (uint32)(sint32)(sint16)GetGameTuning().player.move_speed));
                                                            move_work.components[0] = (sint32)((uint32)move_work.components[0] * this->move_ramp) / 8;
                                                            velocity_changed = true;
                                                        }
                                                    }
                                                    else if (!active_force)
                                                    {
                                                        velocity_changed = true;
                                                        move_work.components[0] = 0;
                                                        move_work.components[2] = 0;
                                                    }
                                                    break;
                                            }
                                        }
                                        SetRotMatrix(&camera_matrix);
                                        if (velocity_changed)
                                        {
                                            ShiftVectorRight(world_velocity.words, move_work.words, velocity_shift);

                                            motion.velocity = world_velocity;
                                            xport_gte_write_data(9, motion.velocity.words[0]);
                                            xport_gte_write_data(10, motion.velocity.words[1]);
                                            xport_gte_write_data(11, motion.velocity.words[2]);
                                            xport_gte_execute(0x49E012);
                                            motion.velocity.words[0] = xport_gte_read_data(25);
                                            motion.velocity.words[1] = xport_gte_read_data(26);
                                            motion.velocity.words[2] = xport_gte_read_data(27);

                                            ShiftVectorLeft(motion.velocity.words, velocity_shift);
                                            rail_state = this->movement_state;
                                            if ((rail_state & 0x8000) != 0)
                                            {
                                                AddVectors(this->model.pos.words, motion.velocity.words);
                                                AddVectors(rail_pos.words, motion.velocity.words);
                                                if (!rail->Project(rail_pos, rail_point) || this->pad->buttons[17].pressed)
                                                {
                                                    this->pad->buttons[17].pressed = 0;
                                                    rail_sound = this->rail_voice;
                                                    this->movement_state = 4;
                                                    on_rail = 0;
                                                    ::StopSoundVoices(rail_sound);
                                                    goto ApplyPlatformMotion;
                                                }
                                            ApplyRailPoint:
                                                this->model.pos = rail_point;
                                                rail_pos.words[0] = this->model.pos.words[0];
                                                rail_pos.words[1] = this->model.pos.words[1];
                                                rail_pos.words[2] = this->model.pos.words[2];
                                                this->model.pos.words[1] = (this->model.pos.words[1] + (0x80000));
                                                goto ApplyPlatformMotion;
                                            }
                                            if ((rail_state & 0x4000) == 0)
                                                goto ApplyPlatformMotion;
                                            if (rail->Advance(rail_pos, rail_step, rail_point) && !this->pad->buttons[17].pressed)
                                            {
                                                goto ApplyRailPoint;
                                            }
                                            this->pad->buttons[17].pressed = 0;
                                        }
                                        else
                                        {
                                            if ((((this->movement_state & 0xC000) == 0) || ((rail_pad = this->pad), !(rail_pad->buttons[17].pressed))))
                                            {
                                            ApplyPlatformMotion:
                                                if (!(ground.GetObject()))
                                                    goto UpdateFootsteps;

                                                platform_moved = false;
                                                platform_pos = this->model.pos;
                                                if ((this->movement_state & 0x3000) != 0)
                                                {
                                                    AddVectors(this->model.pos.words, static_cast<VisualObject *>(ground.GetObject()->owner)->motion.velocity.words);
                                                    platform_moved = true;
                                                    goto CheckPlatformMotion;
                                                }
                                                if (this->airborne_ticks)
                                                {
                                                    if (((this->move_input[0] | this->move_input[1]) || ((air_platform = ground.GetObject()), (air_platform_velocity_x = static_cast<VisualObject *>(air_platform->owner)->motion.velocity.words[0]), !((air_platform_velocity_x | static_cast<VisualObject *>(air_platform->owner)->motion.velocity.words[2])))))
                                                    {
                                                    CheckPlatformMotion:
                                                        if (platform_moved)
                                                        {
                                                            (platform_ray.endpoints[0].words[0] = platform_pos.words[0]);
                                                            (platform_ray.endpoints[0].words[1] = platform_pos.words[1]);
                                                            (platform_ray.endpoints[0].words[2] = platform_pos.words[2]);
                                                            (platform_ray.endpoints[1].words[0] = this->model.pos.words[0]);
                                                            (platform_ray.endpoints[1].words[1] = this->model.pos.words[1]);
                                                            (platform_ray.endpoints[1].words[2] = this->model.pos.words[2]);
                                                            InitializeCollisionRay(platform_ray);
                                                            QueryCollisionRay(platform_ray, 1);
                                                            if (platform_ray.hit.GetObject())
                                                            {
                                                                this->model.pos = platform_pos;
                                                            }
                                                        }

                                                    UpdateFootsteps:
                                                        if ((this->contact_flags & 0x100) != 0)
                                                            runtime.jump_released = 1;

                                                        if ((this->jump_hold_ticks && this->pad->buttons[17].down) && !runtime.jump_released)
                                                            motion.velocity.words[1] = this->jump_velocity;
                                                        footstep_sound = 74;
                                                        if ((this->model.flags & 8) != 0)
                                                        {
                                                            if (runtime.last_footstep_sound == 74)
                                                                footstep_sound = 75;
                                                        }
                                                        else
                                                        {
                                                            do
                                                                footstep_sound = RandomRange(4) + 31;
                                                            while ((footstep_sound == runtime.last_footstep_sound));
                                                        }
                                                        footstep_animation = this->model.animation;
                                                        switch (footstep_animation)
                                                        {
                                                            case 1:
                                                                footstep_crossed = DidAnimationCrossFrame(1, (sint32)GetGameTuning().player.footstep_frames[0]);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (!footstep_event)
                                                                {
                                                                    footstep_frame = (sint32)GetGameTuning().player.footstep_frames[1];
                                                                    footstep_animation_id = 1;
                                                                    break;
                                                                }
                                                            PlayFootstep:
                                                                PlaySoundEffect(played_footstep_sound, 0x3FFF, 0);

                                                                runtime.last_footstep_sound = footstep_sound;
                                                            AdvanceMovementTimers:
                                                                jump_hold_before = this->jump_hold_ticks;

                                                                jump_hold_after = jump_hold_before - 1;
                                                                if (jump_hold_before)
                                                                    this->jump_hold_ticks = jump_hold_after;
                                                                slowdown_intensity = this->slowdown_ticks;
                                                                if (slowdown_intensity)
                                                                {
                                                                    slowdown_green = slowdown_intensity << 8;
                                                                    if (slowdown_intensity >= 129)
                                                                    {
                                                                        slowdown_intensity = 128;
                                                                        slowdown_green = 0x8000;
                                                                    }
                                                                    model_effect.color_mode = (((slowdown_intensity | slowdown_green) | 0x6000000) | (slowdown_intensity << 17));
                                                                    model_effect.scale = 0x2000;
                                                                    slowdown_remaining = this->slowdown_ticks;
                                                                    model_effect.depth_bias = 10;
                                                                    this->slowdown_ticks = --slowdown_remaining;
                                                                    if (slowdown_remaining)
                                                                    {
                                                                    CheckActorContact:
                                                                        if (!VectorsDiffer(pos_history.prev.words, this->model.pos.words))
                                                                        {
                                                                        UpdateMovementTrace:
                                                                            if (!runtime.motion_update_seen)
                                                                                runtime.motion_update_seen = 1;

                                                                            SubtractVectors(frame_delta.words, this->model.pos.words, pos_history.prev.words);
                                                                            if (this->movement_state != 0x40000)
                                                                            {
                                                                                if ((frame_delta.words[0] | frame_delta.words[1]) | frame_delta.words[2])
                                                                                {
                                                                                    runtime.trace_direction = frame_delta;
                                                                                }
                                                                                NormalizeFixedVector(trace_dir.words, runtime.trace_direction.words);

                                                                                MultiplyVector(trace_offset.words, trace_dir.words, trace_offset_scale);
                                                                                SubtractVectors(trace_start.words, pos_history.prev.words, trace_offset.words);
                                                                                (motion_ray.endpoints[0].words[0] = trace_start.words[0]);
                                                                                (motion_ray.endpoints[0].words[1] = trace_start.words[1]);
                                                                                (motion_ray.endpoints[0].words[2] = trace_start.words[2]);
                                                                                (motion_ray.endpoints[1].words[0] = this->model.pos.words[0]);
                                                                                (motion_ray.endpoints[1].words[1] = this->model.pos.words[1]);
                                                                                (motion_ray.endpoints[1].words[2] = this->model.pos.words[2]);
                                                                                InitializeCollisionRay(motion_ray);
                                                                                collision_query.require_surface02 = false;
                                                                                motion_ray.triggers = 1;
                                                                                QueryCollisionRay(motion_ray, 1);
                                                                                collision_query.require_surface02 = false;
                                                                            }
                                                                            UpdateAnimationModel(PlayerHierarchy);
                                                                            weapon_angles.components[0] = 0;
                                                                            weapon_angles.components[1] = (sint16)camera_yaw;
                                                                            weapon_angles.components[2] = 0;
                                                                            aim_axis0 = 0;
                                                                            aim_axis1 = 0;
                                                                            if (this->analog_aim)
                                                                            {
                                                                                aim_axis0 = this->aim_input[0];
                                                                                aim_axis1 = this->aim_input[1];
                                                                            }
                                                                            if (aim_axis0 | aim_axis1)
                                                                            {
                                                                                weapon_angles.components[1] = (sint16)(((uint32)camera_yaw + this->aim_heading) & 4095);
                                                                            }
                                                                            else
                                                                            {
                                                                                body_heading = this->model.angles.words[1];
                                                                                model_heading = GetModelJointAngles(7).rotation.words[1];
                                                                                weapon_angles.components[1] = (sint16)(body_heading + model_heading);
                                                                            }
                                                                            UpdateWeapons(weapon_angles, this->weapons_enabled);
                                                                            UpdateGroundContact();
                                                                            if (((sint16)((uint16)this->health)) < 0)
                                                                                this->health = (sint16)(uint16)(0);
                                                                            if (!(on_rail))
                                                                                rail = 0;
                                                                            if ((this->movement_state & 0x180000) == 0)
                                                                                traversal_region = 0;
                                                                            cleanup_state = this->movement_state;
                                                                            result = 0x20000;
                                                                            if (cleanup_state != 0x10000)
                                                                            {
                                                                                one_shot_activator = 0;
                                                                                cleanup_state = this->movement_state;
                                                                            }
                                                                            if (cleanup_state != 0x20000)
                                                                                resetting_activator = 0;
                                                                            active_force = 0;
                                                                            return result;
                                                                        }

                                                                        contact = TraceSegmentCollision(pos_history.prev.words, this->model.pos.words, visual_lists.actors);
                                                                        if (contact)
                                                                        {
                                                                            if (static_cast<VisualObject *>(contact->owner)->kind == 201)
                                                                                goto CheckPickupContact;
                                                                            ApplyContactImpulse(*static_cast<VisualObject *>(contact->owner));
                                                                        }
                                                                    CheckPickupContact:
                                                                        pickup = TraceSegmentCollision(pos_history.prev.words, this->model.pos.words, visual_lists.pickups);

                                                                        if (pickup)
                                                                            static_cast<Pickup *>(pickup->owner)->Collect(*this);
                                                                        goto UpdateMovementTrace;
                                                                    }
                                                                    model_effect.color_mode = 0;
                                                                }
                                                                goto CheckActorContact;

                                                            case 3:
                                                                footstep_crossed = DidAnimationCrossFrame(3, (sint32)GetGameTuning().player.footstep_frames[0]);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (footstep_event)
                                                                    goto PlayFootstep;
                                                                footstep_frame = (sint32)GetGameTuning().player.footstep_frames[1];
                                                                footstep_animation_id = 3;
                                                                break;

                                                            case 4:
                                                                footstep_crossed = DidAnimationCrossFrame(4, (sint32)GetGameTuning().player.footstep_frames[0]);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (footstep_event)
                                                                    goto PlayFootstep;
                                                                footstep_frame = (sint32)GetGameTuning().player.footstep_frames[1];
                                                                footstep_animation_id = 4;
                                                                break;

                                                            case 8:
                                                                footstep_crossed = DidAnimationCrossFrame(8, 7);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (footstep_event)
                                                                    goto PlayFootstep;
                                                                footstep_animation_id = 8;
                                                                footstep_frame = 18;
                                                                break;

                                                            default:
                                                                goto AdvanceMovementTimers;
                                                        }

                                                        footstep_crossed = DidAnimationCrossFrame(footstep_animation_id, footstep_frame);
                                                        footstep_event = UpdateEventLatch(footstep_crossed, 2);
                                                        played_footstep_sound = footstep_sound;
                                                        if (!footstep_event)
                                                            goto AdvanceMovementTimers;
                                                        goto PlayFootstep;
                                                    }
                                                    platform_carried_x = this->model.pos.words[0] + air_platform_velocity_x;
                                                    platform_z_owner = ground.GetObject();
                                                    this->model.pos.words[0] = platform_carried_x;
                                                }
                                                else
                                                {
                                                    ground_platform = ground.GetObject();
                                                    ground_velocity_x = static_cast<VisualObject *>(ground_platform->owner)->motion.velocity.words[0];
                                                    uint32 platform_y = static_cast<VisualObject *>(ground_platform->owner)->motion.velocity.words[1];
                                                    uint32 platform_z = static_cast<VisualObject *>(ground_platform->owner)->motion.velocity.words[2];
                                                    if (!(ground_velocity_x | platform_y | platform_z))
                                                        goto CheckPlatformMotion;
                                                    platform_y_owner = ground.GetObject();
                                                    this->model.pos.words[0] = (this->model.pos.words[0] + (ground_velocity_x));
                                                    platform_carried_y = this->model.pos.words[1] + static_cast<VisualObject *>(platform_y_owner->owner)->motion.velocity.words[1];
                                                    platform_z_owner = ground.GetObject();
                                                    this->model.pos.words[1] = platform_carried_y;
                                                }
                                                platform_moved = true;
                                                this->model.pos.words[2] = (this->model.pos.words[2] + (static_cast<VisualObject *>(platform_z_owner->owner)->motion.velocity.words[2]));
                                                goto CheckPlatformMotion;
                                            }
                                            (rail_pad->buttons[17].pressed = 0);
                                        }
                                        this->movement_state = 4;
                                        on_rail = 0;
                                        goto ApplyPlatformMotion;
                                    }
                                }
                                else if ((acceleration_state != 4096))
                                {
                                    goto ApplyGravity;
                                }
                            }
                            motion.velocity = zero_vector;
                            motion.acceleration = zero_vector;
                            goto TransformMovement;
                        }

                        immediate_facing = true;
                        facing_angle = (((uint16)direction_animations[9].facing[aim_idx] + this->aim_heading) - (((uint16)(aim_idx)) << 9)) & 0xFFF;
                        desired_heading = (sint16)((camera_yaw + direction_animations[9].body_heading[aim_idx]));
                    ApplyFacingTransition:
                        SetFacingTransition(facing_angle, desired_heading, immediate_facing);

                        goto AdvanceFacingTransition;
                    }
                    if (this->model.animation != 13)
                        StartAnimation(13, 1);
                    desired_heading = (sint16)(this->model.angles.words[1]);
                    facing_angle = 0;
                }
                else if ((facing_state == 0x8000))
                {
                    if (this->model.animation != 14)
                        SetAnimationRange(14, 0, -1);
                    if (aim_octant)
                    {
                        immediate_facing = true;
                        facing_angle = (sint16)((((uint16)direction_animations[9].facing[aim_idx] + this->aim_heading) - (((uint16)(aim_idx)) << 9)));
                        desired_heading = (sint16)((camera_yaw + direction_animations[9].body_heading[aim_idx]));
                        goto ApplyFacingTransition;
                    }
                    desired_heading = (sint16)(this->model.angles.words[1]);
                    facing_angle = 0;
                }
                else
                {
                    if ((facing_state & 0x1F3800) != 0)
                    {
                        desired_heading = (sint16)(this->model.angles.words[1]);
                        facing_angle = 0;
                    }
                    else
                    {
                        facing_angle = 0;
                        desired_heading = camera_yaw;
                    }
                }
                immediate_facing = true;
                goto ApplyFacingTransition;
            }
            if (aim_octant)
            {
                if (this->model.animation != 9)
                    StartAnimation(9, 1);

                immediate_facing = false;
                facing_angle = (((uint16)direction_animations[8].facing[aim_idx] + this->aim_heading) - (((uint16)(aim_idx)) << 9)) & 0xFFF;
                desired_heading = (sint16)((camera_yaw + direction_animations[8].body_heading[aim_idx]));
                goto ApplyFacingTransition;
            }
            idle_animation = this->model.animation;
            if (idle_animation == 6)
            {
                if (!(this->playback.complete))
                {
                KeepCurrentFacing:

                    facing_angle = 0;
                    desired_heading = (sint16)(this->model.angles.words[1]);
                    immediate_facing = false;
                    goto ApplyFacingTransition;
                }
            }
            else
            {
                if (!(this->model.animation))
                    goto KeepCurrentFacing;
                if (idle_animation == 5)
                    goto KeepCurrentFacing;
            }
            SetAnimationRange(0, 0, -1);
            goto KeepCurrentFacing;
        }
        if (movement_state == 0x20000)
        {
            ResettingActivator *resetting = resetting_activator.Get();
            if (!resetting)
            {
                GameDiagnostic("TODO Missing active resetting activator\n");
                abort();
            }
            reset_pos_x = this->model.pos.words[0];
            this->idle_ticks = 0;
            reset_target_x = resetting->activation_pos.words[0];
            if ((sint32)Abs32((sint32)((uint32)reset_target_x - (uint32)reset_pos_x) >> 3) <= 0x7FFF)
                this->model.pos.words[0] = reset_target_x;
            else
                this->model.pos.words[0] = ((uint32)reset_pos_x + ((sint32)((uint32)reset_target_x - (uint32)reset_pos_x) >> 3));
            reset_pos_z = this->model.pos.words[2];
            activation_target_z = resetting_activator->activation_pos.words[2];
            if ((sint32)Abs32((sint32)((uint32)activation_target_z - (uint32)reset_pos_z) >> 3) <= 0x7FFF)
                this->model.pos.words[2] = activation_target_z;
            else
                this->model.pos.words[2] = ((uint32)reset_pos_z + ((sint32)((uint32)activation_target_z - (uint32)reset_pos_z) >> 3));
            if ((this->model.frame == 14) && ((uint8)this->playback.dir == 1))
                resetting_activator->Activate();
            if (this->playback.complete)
            {
                if ((uint8)this->playback.dir == 1)
                {
                    SetAnimationRange(18, ((sint8)(this->model.frame)), 0);
                    move_octant = 0;
                    goto SelectDirectionalAnimation;
                }
                this->movement_state = 1;
            }
            goto ClearMovementOctant;
        }
        move_octant = 0;
        if (movement_state != 0x40000)
            goto SelectDirectionalAnimation;
        death_tick_before = this->death_ticks;
        this->idle_ticks = 0;
        death_tick_after = death_tick_before + 1;
        this->death_ticks = death_tick_after;
        if (((death_tick_after == 32) || (death_tick_after == 64)) || (death_tick_after == 96))
        {
            if (visual_lists.cameras)
            {
                if (this->death_camera_enabled)
                    static_cast<Camera *>(visual_lists.cameras->owner)->SetYawTransition((static_cast<Camera *>(visual_lists.cameras->owner)->angles.words[1] + 1024) & 0xFFF, 34);
            }
            if (this->death_ticks == 64)
                ::FadeScreenToWhite(128, 1);
        }
        if (!(this->playback.complete))
        {
            runtime.death_wait = 0;
            move_octant = 0;
            goto SelectDirectionalAnimation;
        }
        if (game_progress.lives == 1)
        {
            if (runtime.death_wait < 128)
                goto AdvanceDeathWait;
        }
        else if ((runtime.death_wait < 64))
        {
        AdvanceDeathWait:
            ++runtime.death_wait;

            move_octant = 0;
            goto SelectDirectionalAnimation;
        }
        Finish();
        ::ResetScreenFlash();
        goto AdvanceDeathWait;
    }
    return result;
}

void Player::SetFacingTransition(uint32 target_angle, uint32 heading, uint32 immediate)
{
    FUNCTION_MARKER(0x80061A78, "SLUS_003.73");
    uint32 target = target_angle & 4095, desired = heading & 4095;
    ModelJointAngles &facing = GetModelJointAngles(7);
    uint32 yaw_distance, angle_distance, angle_word;
    sint32 old_heading, old_angle, delta;
    if (this->analog_aim || this->aim_press_ticks == 1)
        immediate = 1;
    if (immediate)
    {
        this->model.angles.words[1] = desired;
        facing.rotation.words[1] = (uint16)(target);
        this->facing_step = 0;
        this->facing_ticks = 0;
        return;
    }
    old_heading = (sint16)this->model.angles.words[1];
    delta = (sint32)desired - old_heading;
    yaw_distance = delta < 0 ? 0 - (uint32)delta : (uint32)delta;
    if (yaw_distance - 1025 < 2047)
    {
        this->model.angles.words[1] = desired;
        facing.rotation.words[1] = (uint16)(target);
        this->facing_step = 0;
        this->facing_ticks = 0;
        return;
    }
    old_angle = (sint16)facing.rotation.words[1];
    delta = (sint32)target - old_angle;
    angle_distance = delta < 0 ? 0 - (uint32)delta : (uint32)delta;
    angle_word = facing.rotation.words[1];
    if (angle_distance - 1025 < 2047)
    {
        this->model.angles.words[1] = desired;
        facing.rotation.words[1] = (uint16)(target);
        this->facing_step = 0;
        this->facing_ticks = 0;
        return;
    }
    if (yaw_distance && angle_distance)
    {
        uint32 angle = angle_word;
        angle = old_heading < (sint32)desired ? angle - yaw_distance : angle + yaw_distance;
        facing.rotation.words[1] = (uint16)(angle & 4095);
    }
    this->facing_ticks = 8;
    this->model.angles.words[1] = desired;
    this->facing_target = target;
    old_angle = (sint16)facing.rotation.words[1];
    angle_word = facing.rotation.words[1];
    if (old_angle < (sint32)target)
        delta = (sint32)target - old_angle < 1024 ? (sint32)target - old_angle : (sint16)(target - 4096) - old_angle;
    else
        delta = old_angle - (sint32)target < 1024 ? (sint32)target - old_angle : (sint32)target - (sint16)(angle_word - 4096);
    delta /= 8;
    this->facing_step = (uint32)delta;
    return;
}
