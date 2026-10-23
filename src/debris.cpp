#include <string.h>
#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tuning.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "fading_particle.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "debris.h"

Debris::Debris(const Vector3 *pos, const Vector3 *velocity, uint16 model_idx, sint32 floor_height, uint16 flags, uint16 lifetime, const char *name) : VisualObject(), bounce_count(3), damage(0), lifetime(lifetime), flags(flags), floor_y(floor_height)
{
    FUNCTION_MARKER(0x8005BBB0, "SLUS_003.73");
    sint16 x_rotation, y_rotation, z_rotation;
    for (uint32 axis = 0; axis < 3; ++axis)
        target_pos.comp[axis] = 0;
    SetModelBank(name);
    model.model_idx = model_idx;
    AddToList(visual_lists.actors);
    col_radius = 0;
    lifecycle_flags &= 0xFFEF;
    model.pos = *pos;
    motion.velocity = *velocity;
    if (!(flags & 32))
    {
        x_rotation = RandomRange(256) - 128;
        y_rotation = RandomRange(256) - 128;
        z_rotation = RandomRange(256) - 128;
        motion.angular_velocity.x = x_rotation;
        motion.angular_velocity.y = y_rotation;
        motion.angular_velocity.z = z_rotation;
    }
}

uint32 Debris::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005BCE4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

uint32 Debris::Update()
{
    GameHeapBlock *reservation;
    uint32 val, idx;
    uint16 timer, flags;
    Vector3 prev;
    uint8 r, g, b, damage;
    ModelInstance *player;
    ShortVector3 angles;
    sint32 distance;
    FUNCTION_MARKER(0x8005BD48, "SLUS_003.73");
    ObjectBorrow object_borrow;
    timer = lifetime;
    if (!timer)
        return VisualObject::Finish();
    flags = this->flags;
    lifetime = timer - 1;
    if (flags & 256)
    {
        angles.SetDir(model.pos, target_pos);
        motion.acceleration.SetDir(128, angles);
        motion.velocity.Add(motion.acceleration);
        motion.velocity.Damp(motion.drag);
        motion.velocity.SnapSmall();
    }
    prev = model.pos;
    model.pos.Add(motion.velocity);
    if (this->flags & 1)
    {
        if (floor_y < model.pos.y)
        {
            --bounce_count;
            if (!bounce_count)
                return VisualObject::Finish();
            motion.velocity.y = -(uint32)motion.velocity.y;
            model.pos.y = floor_y;
        }
    }
    if (!(this->flags & 16))
        motion.velocity.y = (uint32)motion.velocity.y + (0x4000);
    model.angles.AddAngles(motion.angular_velocity);
    if (this->flags & 128)
        Shrink();
    if (!IsGameHeapUnderPressure() && (this->flags & 14) && (sint32)effect_count < 200)
    {
        angles.comp[0] = 512;
        angles.comp[1] = 4096;
        angles.comp[2] = 0;
        PrimitiveParticle::SetSpread(&angles);
        angles.SetDir(prev, model.pos);
        PrimitiveParticle::SetAngles(&angles);
        distance = model.pos.Distance(visual_lists.cameras->pos);
        PrimitiveParticle::SetSize(distance < 8193 ? (distance < 1025 ? 4 : 2) : 1);
        flags = this->flags;
        if (flags & 2)
        {
            r = GetGameTuning().death_particle_color[0];
            g = GetGameTuning().death_particle_color[1];
            b = GetGameTuning().death_particle_color[2];
            PrimitiveParticle::SetColor(r, g, b);
            r = GetGameTuning().death_particle_color[0] >> 5;
            g = GetGameTuning().death_particle_color[1] >> 5;
            b = GetGameTuning().death_particle_color[2] >> 5;
            PrimitiveParticle::SetColorStep(r, g, b);
        }
        else if (flags & 4)
        {
            PrimitiveParticle::SetColor(128, 128, 128);
            PrimitiveParticle::SetColorStep(6, 6, 6);
        }
        else if (flags & 8)
        {
            val = RandomRange(128);
            PrimitiveParticle::SetColor(val, 128, 0);
            PrimitiveParticle::SetColorStep(val >> 5, 6, 0);
        }
        use_small_effect_pool = false;
        for (idx = 0; idx < 4; ++idx)
        {
            reservation = EffectObject::AllocateStorage(88);
            if (reservation)
            {
                new (reservation) FadingParticle(&model.pos, 16, 8192, 16);
            }
        }
        use_small_effect_pool = true;
    }
    val = this->flags & 64;
    if (val)
    {
        player = visual_lists.players;
        if (player)
        {
            val = (sint32)model.pos.Distance(player->pos) < 128;
            if (val)
            {
                player = visual_lists.players;
                damage = this->damage;
                static_cast<VisualObject *>(player->owner)->Damage(damage, &zero_vector, 0);
                return VisualObject::Finish();
            }
        }
    }
    return val;
}

// Native extraction of the original6CCE0/6C774 scale operations
void Debris::Shrink()
{
    ShortVector3 delta;
    model.flags |= 512;
    for (uint32 axis = 0; axis < 3; ++axis)
        delta.comp[axis] = model.scale.comp[axis] >> 4;
    for (uint32 axis = 0; axis < 3; ++axis)
        model.scale.comp[axis] = (uint16)model.scale.comp[axis] - delta.comp[axis];
}
