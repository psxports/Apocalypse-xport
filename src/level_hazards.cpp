#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "level_session.h"
#include "record_scripts.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "fire_cycle.h"
#include "audio_session.h"
#include "vector_distance.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "effect_splash.h"
#include "model_resources.h"
#include "collision.h"
#include "object_damage.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "particle_debris.h"

#include "level_hazards.h"
#include "textured_quad.h"
#include "record_data.h"

FloatingHazard::FloatingHazard(const RecordCursor &record_data, uint32 index) : Enemy(), bob_phase(0), bob_offset(0)
{
    RecordDataBorrow record_borrow;

    uint32 animation, flags;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8005A630, "SLUS_003.73");
    SetModelBank(GetRecordModelBankName());
    animation = ::FindModelResourceIndex(0xB9E8D489, GetRecordModelBankSlot());
    flags = this->model.flags;
    this->model.model_idx = animation;
    this->model.flags = (flags & 0xFFFD) | 8;
    AddToList(visual_lists.actors);
    this->collision_radius = 64;
    this->health = (sint16)(uint16)(20);
    this->kind = 9;
    RecordCursor angles(ReadPosition(record_data));
    RecordCursor script(ReadAngles(angles));
    script_cursor = script;
    this->record_idx = index;
    motion.drag[0] = 2;
    motion.drag[1] = 2;
    motion.drag[2] = 2;
    motion.angular_drag[0] = 16;
    motion.angular_drag[1] = 16;
    motion.angular_drag[2] = 16;
    cursor = script_cursor;
    ai.parameters[0] = 12;
    ai.parameters[1] = 256;
    ai.parameters[2] = 20;
    this->control.script_active = 1;
    ExecuteScript(RecordCursor(cursor));
}

uint32 FloatingHazard::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005A74C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = Enemy::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(this);
    return result;
}

void FloatingHazard::Explode()
{
    uint32 variant;
    SpawnExplosionRing(model.pos.words, 96, 96, 128, 0, 4, 1, 100);
    variant = RandomRange(2);
    PlayPositionedSound((sint32)(variant + 1), model.pos.words, 0);
}

uint32 FloatingHazard::Damage(uint32 damage, const sint32 *, uint32)
{
    sint32 health;
    uint32 value;
    FUNCTION_MARKER(0x8005A7B0, "SLUS_003.73");
    health = this->health;
    value = (uint16)this->health - damage;
    if (health > 0)
    {
        this->health = (sint16)(uint16)(value);
        if ((sint32)(value << 16) <= 0)
        {
            Explode();
            Finish();
        }
    }
    return 1;
}

uint32 FloatingHazard::UpdateHeight()
{
    uint32 state, result, y, floor;
    FUNCTION_MARKER(0x8005AADC, "SLUS_003.73");
    if (ai.state)
        return 1;
    state = ai.phase;
    result = state < 2;
    if (state != 1)
    {
        if (!result || state)
            return result;
        result = ai.phase + 1;
        motion.acceleration.words[1] = 24576;
        ai.phase = result;
    }
    y = this->model.pos.words[1];
    floor = (uint32)level_environment.water_y;
    result = (sint32)floor < (sint32)y;
    if (result)
    {
        result = ai.phase;
        motion.acceleration.words[1] = 0;
        motion.velocity.words[1] = 0;
        this->model.pos.words[1] = floor;
        ++result;
        ai.phase = result;
    }
    return result;
}

uint32 FloatingHazard::Update()
{
    RecordDataBorrow record_borrow;
    uint32 flags, value, damage, distance, phase, height, y;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8005A860, "SLUS_003.73");
    if (!motion.velocity.words[1])
    {
        y = this->model.pos.words[1];
        height = bob_offset;
        this->model.pos.words[1] = y - height;
    }
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1)
    {
        value = (uint32)this->control.activation_count;
        this->activation_flags = flags & 0xFFFE;
        this->control.activation_count = value + 1;
    }
    if ((uint32)this->control.script_active)
    {
        value = (uint32)this->control.script_delay;
        if (value)
            this->control.script_delay = value - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateHeight();
    if (visual_lists.players)
    {
        value = ai.parameters[1];
        distance = player_distance;
        if (distance < value)
        {
            Explode();
            damage = ScaleDamageForDifficulty(ai.parameters[2]);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage(damage, zero_vector.components, 0);
            Finish();
        }
    }
    if (this->control.flags & 1)
    {
        distance = MeasurePositionDistance(this->model.pos.words, this->destination.words);
        if ((sint32)distance < 129)
            this->control.flags = this->control.flags & ~1;
        else
        {
            CalculateDirectionAngles(angles.words, this->model.pos.words, this->destination.words);
            BuildDirectionVector(motion.acceleration.words, ai.parameters[0], angles.words);
        }
    }
    if (!RandomRange(64))
    {
        value = RandomRange(256) - 128;
        motion.angular_velocity.words[0] = 0;
        motion.angular_velocity.words[1] = value;
        motion.angular_velocity.words[2] = 0;
    }
    if (!motion.velocity.words[1])
    {
        value = RandomRange(512);
        phase = (bob_phase + value) & 4095;
        bob_phase = phase;
        height = (uint32)rsin(phase);
        y = this->model.pos.words[1];
        height <<= 5;
        bob_offset = height;
        this->model.pos.words[1] = y + height;
    }
    return IntegrateMotion();
}

uint32 FloatingHazard::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8005AB58, "SLUS_003.73");
    command &= 65535;
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = script_cursor;
    value = cursor.Read16();
    script_cursor = cursor + 2;
    if (value & 0x2000)
    {
        value = ResolveParameter(value & 65535);
    }
    ReadRecordPosition(this->destination.words, value & 65535);
    value = this->control.flags;
    this->control.flags = value | 1;
    return 1;
}

BallisticHazard::BallisticHazard(const uint32 *position, uint32 kind, uint32 flags, uint32 parameter, sint32 vertical_speed) : Enemy(), impact_flags(0), impact_radius(0)
{
    uint32 trig_angle;
    uint32 x, y, z, value, radius, coefficient, floor;
    ModelInstance *player;
    FUNCTION_MARKER(0x8005ABF8, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    pos_history.saved.words[0] = x;
    pos_history.saved.words[1] = y;
    pos_history.saved.words[2] = z;
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    if (kind == 2)
    {
        SetModelBank("items");
        value = FindModelResourceIndex(0x717FB6B2, items_bank);
        this->model.model_idx = value;
        motion.acceleration.words[1] = (uint32)vertical_speed << 12;
        shadow.radius = 128;
    }
    else if ((sint32)kind >= 0 && (sint32)kind <= 3)
    {
        SetModelBank(GetRecordModelBankName());
        value = FindModelResourceIndex(0x2D990A1E, GetRecordModelBankSlot());
        this->model.model_idx = value;
        motion.acceleration.words[1] = (uint32)vertical_speed << 12;
        this->model.angles.words[1] = RandomRange(4096);
        value = 256 - RandomRange(128);
        motion.angular_drag[0] = 16;
        shadow.radius = 128;
        motion.angular_velocity.words[0] = value;
        ai.parameters[0] = kind == 3 ? 48 : 32;
        ai.parameters[1] = 256;
        ai.parameters[2] = kind == 3 ? 4 : 8;
        if (kind == 1)
        {
            radius = RandomRange(512);
            trig_angle = RandomRange(4096) & 4095;
            coefficient = (uint32)rsin(trig_angle);
            player = visual_lists.players;
            value = player->pos.words[0];
            this->model.pos.words[0] = value + radius * coefficient;
            coefficient = (uint32)rcos(trig_angle);
            value = player->pos.words[2];
            this->model.pos.words[2] = value + radius * coefficient;
            x = this->model.pos.words[0];
            y = this->model.pos.words[1];
            z = this->model.pos.words[2];
            pos_history.saved.words[0] = x;
            pos_history.saved.words[1] = y;
            pos_history.saved.words[2] = z;
        }
    }
    this->model.flags = this->model.flags & 0xFFFD;
    AddToList(visual_lists.actors);
    motion.drag[0] = 2;
    motion.drag[1] = 5;
    motion.drag[2] = 2;
    this->collision_radius = 0;
    this->kind = 410;
    impact_flags = flags;
    impact_radius = parameter;
    this->lifecycle_flags = this->lifecycle_flags & 0xFFEF;
    if (flags & 5)
    {
        floor = FindGroundHeight(this->model.pos.words, 0, 8192);
        if (floor == (uint32)-1)
        {
            Enemy::Finish();
            return;
        }
        this->destination.words[1] = floor;
        EnableShadow();
        value = this->model.pos.words[1];
        x = this->model.pos.words[0];
        value = floor - value;
        surface.pos.words[0] = x;
        z = this->model.pos.words[2];
        shadow.max_height = (uint32)((sint32)value >> 12);
        surface.pos.words[1] = floor;
        surface.pos.words[2] = z;
    }
    else if (flags & 2)
    {
        EnableShadow();
        floor = (uint32)level_environment.water_y;
        value = this->model.pos.words[1];
        x = this->model.pos.words[0];
        shadow.max_height = (uint32)((sint32)(floor - value) >> 12);
        surface.pos.words[0] = x;
        z = this->model.pos.words[2];
        surface.pos.words[1] = floor - 65536;
        surface.pos.words[2] = z;
    }
}

uint32 BallisticHazard::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005AF34, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}

uint32 BallisticHazard::Damage(uint32, const sint32 *, uint32)
{
    FUNCTION_MARKER(0x8005AF98, "SLUS_003.73");
    return 0;
}

uint32 BallisticHazard::Update()
{
    uint32 trig_angle;
    GameHeapBlock *child;
    uint32 flags;
    uint32 result;
    uint32 y;
    uint32 floor;
    uint32 parameter;
    uint32 distance;
    uint32 threshold;
    uint32 damage;
    uint32 value, random, product, kind, index;
    Vector3 point, velocity;
    sint32 quotient;
    FUNCTION_MARKER(0x8005AFA0, "SLUS_003.73");
    if (shadow.effect)
        shadow.effect->depth_bias = 128;
    Enemy::IntegrateMotion();
    flags = impact_flags;
    result = flags & 2;
    if (flags & 5)
    {
        y = this->model.pos.words[1];
        floor = this->destination.words[1];
        if ((sint32)y >= (sint32)floor)
        {
            if (flags == 4)
            {
                this->model.pos.words[1] = floor;
                SpawnFireCycle(this->model.pos.words, 180, 1, 2);
                parameter = impact_radius;
                SpawnExplosionRing(this->model.pos.words, (uint32)((sint32)parameter >> 1), 255, 128, 0, 4, 1, 100);
                distance = player_distance;
                parameter = impact_radius;
                if (distance < parameter)
                {
                    damage = ScaleDamageForDifficulty(20);
                    VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                    damage_target->Damage(damage, zero_vector.components, 0);
                }
                child = AllocateEffectStorage(120);
                if (child)
                {
                    parameter = impact_radius;
                    velocity = zero_vector;
                    ImpactRing *ring = new (child) ImpactRing(velocity.words);
                    ring->SetGeometry(this->model.pos.words, 0, 0);
                    value = (uint32)((sint32)parameter / 16);
                    ring->lifetime = 16;
                    ring->radius_step = value;
                    quotient = (sint16)value;
                    ring->width_step = (uint32)(quotient / 2);
                }
            }
            else if (flags == 1)
            {
                if (level_environment.area_kind == 1)
                    PlayPositionedSound(59, this->model.pos.words, 0);
                for (index = 0; index < ai.parameters[2]; ++index)
                {
                    random = RandomRange(4096);
                    floor = this->destination.words[1];
                    this->model.pos.words[1] = floor - 0x20000;
                    trig_angle = random & 4095;
                    value = (uint32)rsin(trig_angle);
                    parameter = impact_radius;
                    product = value * parameter;
                    velocity.words[0] = (uint32)((sint32)product / 8);
                    random = RandomRange(32);
                    value = (uint32)rcos(trig_angle);
                    parameter = impact_radius;
                    product = value * parameter;
                    velocity.words[1] = (0 - 16 - random) << 12;
                    velocity.words[2] = (uint32)((sint32)product / 8);
                    Debris *debris = 0;
                    child = AllocateZeroedObject(320);
                    if (child)
                    {
                        kind = FindModelResourceIndex(0x51FBD441, GetRecordModelBankSlot()) & 65535;
                        floor = this->destination.words[1];
                        debris = new (child) Debris(this->model.pos.words, velocity.words, kind, floor, 193, 24, GetRecordModelBankName());
                    }
                    // Skip the missing child under the approved allocation-failure contract
                    value = ai.parameters[0] >> 3;
                    if (debris)
                        debris->damage = value;
                }
            }
            return Finish();
        }
        value = surface.pos.words[1];
        distance = player_distance;
        threshold = ai.parameters[1];
        result = (uint32)((sint32)(value - y) >> 12);
    }
    else
    {
        if (!result)
            return result;
        y = this->model.pos.words[1];
        floor = (uint32)level_environment.water_y;
        if ((sint32)y >= (sint32)floor)
        {
            point = this->model.pos;
            point.words[1] -= 286720;
            CreateSplash(point.words);
            PlayPositionedSound(57, this->model.pos.words, 0);
            return Finish();
        }
        value = surface.pos.words[1];
        threshold = ai.parameters[1];
        distance = player_distance;
        result = (uint32)((sint32)(value - y) >> 12);
    }
    shadow.height = result;
    if (distance < threshold)
    {
        ModelInstance *player = visual_lists.players;
        if (player)
        {
            damage = ScaleDamageForDifficulty(ai.parameters[0]);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            return damage_target->Damage(damage, zero_vector.components, 0);
        }
        return 0;
    }
    return result;
}
