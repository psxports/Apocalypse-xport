#include <string.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tuning.h"
#include "object_initializers.h"

#include "visual_object.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "particle_settings.h"
#include "fading_particle.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "game_random.h"

#include "particle_debris.h"

Debris::Debris(const uint32 *position, const uint32 *velocity, uint32 kind, uint32 floor_height, uint32 flags, uint32 lifetime, const char *name) : VisualObject(), bounce_count(3), damage(0), lifetime(lifetime), flags(flags), floor_y(floor_height)
{
    FUNCTION_MARKER(0x8005BBB0, "SLUS_003.73");
    uint32 object_flags, x_rotation, y_rotation, z_rotation, axis;
    Vector3 values;
    for (uint32 axis = 0; axis < 3; ++axis)
        target_pos.words[axis] = 0;
    SetModelBank(name);
    this->model.model_idx = kind;
    AddToList(visual_lists.actors);
    object_flags = this->lifecycle_flags;
    this->collision_radius = 0;
    this->lifecycle_flags = object_flags & 0xFFEF;
    for (axis = 0; axis < 3; ++axis)
        values.words[axis] = position[axis];
    this->model.pos = values;
    for (axis = 0; axis < 3; ++axis)
        values.words[axis] = velocity[axis];
    motion.velocity = values;
    if (!(flags & 32))
    {
        x_rotation = RandomRange(256) - 128;
        y_rotation = RandomRange(256) - 128;
        z_rotation = RandomRange(256) - 128;
        motion.angular_velocity.words[0] = x_rotation;
        motion.angular_velocity.words[1] = y_rotation;
        motion.angular_velocity.words[2] = z_rotation;
    }
}

// Native extraction of the original6CCE0/6C774 scale operations
void Debris::Shrink()
{
    ShortVector3 delta;
    model.flags |= 512;
    for (uint32 axis = 0; axis < 3; ++axis)
        delta.components[axis] = (sint16)model.scale.words[axis] >> 4;
    for (uint32 axis = 0; axis < 3; ++axis)
        model.scale.words[axis] -= delta.components[axis];
}

uint32 Debris::Update()
{
    GameHeapBlock *reservation;
    uint32 timer, flags, value, limit, y, bounce, index;
    Vector3 previous;
    uint32 red, green, blue, damage;
    ModelInstance *player;
    ShortVector3 angles;
    sint32 distance;
    FUNCTION_MARKER(0x8005BD48, "SLUS_003.73");
    ObjectBorrow object_borrow;
    timer = this->lifetime;
    if (!timer)
        return VisualObject::Finish();
    flags = this->flags;
    this->lifetime = timer - 1;
    if (flags & 256)
    {
        CalculateDirectionAngles(angles.words, this->model.pos.words, target_pos.words);
        BuildDirectionVector(motion.acceleration.words, 128, angles.words);
        AddVectors(motion.velocity.words, motion.acceleration.words);
        DampVector(motion.velocity.words, (const uint8 *)motion.drag);
        SnapSmallVector(motion.velocity.words);
    }
    previous = this->model.pos;
    AddVectors(this->model.pos.words, motion.velocity.words);
    if (this->flags & 1)
    {
        y = this->model.pos.words[1];
        limit = this->floor_y;
        if ((sint32)limit < (sint32)y)
        {
            bounce = this->bounce_count - 1;
            this->bounce_count = bounce;
            if (!(bounce & 255))
                return VisualObject::Finish();
            value = motion.velocity.words[1];
            limit = this->floor_y;
            motion.velocity.words[1] = 0 - value;
            this->model.pos.words[1] = limit;
        }
    }
    if (!(this->flags & 16))
        motion.velocity.words[1] = motion.velocity.words[1] + 0x4000;
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    if (this->flags & 128)
        Shrink();
    if (!IsGameHeapUnderPressure() && (this->flags & 14) && (sint32)effect_count < 200)
    {
        angles.components[0] = 512;
        angles.components[1] = 4096;
        angles.components[2] = 0;
        SetParticleSpread(angles.components);
        CalculateDirectionAngles(angles.words, previous.words, this->model.pos.words);
        SetParticleAngles(angles.components);
        distance = (sint32)MeasurePositionDistance(model.pos.words, visual_lists.cameras->pos.words);
        SetParticleSize(distance < 8193 ? (distance < 1025 ? 4 : 2) : 1);
        flags = this->flags;
        if (flags & 2)
        {
            red = (uint8)GetGameTuning().death_particle_color[0];
            green = (uint8)GetGameTuning().death_particle_color[1];
            blue = (uint8)GetGameTuning().death_particle_color[2];
            SetParticleColor(red, green, blue);
            red = (GetGameTuning().death_particle_color[0] >> 5) & 255;
            green = (GetGameTuning().death_particle_color[1] >> 5) & 255;
            blue = (GetGameTuning().death_particle_color[2] >> 5) & 255;
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
        use_small_effect_pool = false;
        for (index = 0; index < 4; ++index)
        {
            reservation = AllocateEffectStorage(88);
            if (reservation)
            {
                new (reservation) FadingParticle(this->model.pos.words, 16, 8192, 16);
            }
        }
        use_small_effect_pool = true;
    }
    value = this->flags & 64;
    if (value)
    {
        player = visual_lists.players;
        if (player)
        {
            value = (sint32)MeasurePositionDistance(this->model.pos.words, player->pos.words) < 128;
            if (value)
            {
                player = visual_lists.players;
                damage = this->damage;
                static_cast<VisualObject *>(player->owner)->Damage(damage, zero_vector.components, 0);
                return VisualObject::Finish();
            }
        }
    }
    return value;
}

uint32 Debris::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005BCE4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}
