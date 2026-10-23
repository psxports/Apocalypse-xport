#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "visual_object.h"
#include "game_tuning.h"
#include "game_tick.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "game_diagnostics.h"
#include "weapon_type.h"
#include "weapon_node_base.h"
#include "weapon_node.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "tracking_projectile.h"
#include "projectile_line.h"
#include "launched_projectile.h"
#include "ballistic_projectile.h"
#include "beam_segments.h"
#include "ring_burst_effect.h"
#include "global.h"
#include "collision.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "spiral_ray_weapon.h"
#include "machine_gun.h"
#include "flamethrower.h"

WeaponNode::WeaponNode(ModelInstance **list, ModelInstance **secondary_list) : WeaponNodeBase(), type(0), fire_active(0), last_fire_tick(0), start_tick(0), child_tick(0), damage(0), owner_list(0), secondary_list(0), sound(0), ammo(0)
{
    FUNCTION_MARKER(0x80022A24, "SLUS_003.73");
    muzzle_pos = zero_vector;
    owner_list = list;
    this->secondary_list = secondary_list;
    ammo = 1000;
}

uint32 WeaponNode::Destroy(uint32 reason)
{
    uint32 sound, result;
    FUNCTION_MARKER(0x80022A48, "SLUS_003.73");
    ClearChild();
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    result = reason & 1;
    if (result)
        result = ReleaseStorage();
    return result;
}

void WeaponNode::ClearChild()
{
    FUNCTION_MARKER(0x80022ABC, "SLUS_003.73");
    if (child)
    {
        ObjectBorrow borrow;
        child->Destroy(3);
    }
    child = 0;
}

uint32 WeaponNode::ConsumeAmmo(uint32 amount)
{
    sint32 result;
    FUNCTION_MARKER(0x80022B34, "SLUS_003.73");
    result = ammo - amount;
    ammo = result;
    if (result < 0)
        ammo = 0;
    return result;
}

Vector3 &WeaponNode::OffsetByAngles(Vector3 &pos, const ShortVector3 &angles)
{
    FUNCTION_MARKER(0x80022B54, "SLUS_003.73");
    Vector3 dir;
    dir.SetDir(100, angles);
    pos.Subtract(dir);
    return pos;
}

uint32 WeaponNode::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x8002BC98, "SLUS_003.73");
    return 0;
}

uint32 WeaponNode::Update(const Vector3 *)
{
    FUNCTION_MARKER(0x8002BCA0, "SLUS_003.73");
    return 0;
}

uint32 WeaponNode::Fire(const Vector3 &, const ShortVector3 &)
{
    FUNCTION_MARKER(0x8002BCA8, "SLUS_003.73");
    return 0;
}

ModelInstance *WeaponNode::SelectTarget(const Vector3 *pos, const ShortVector3 *original_angles, ModelInstance *candidate, sint32 range, sint32 aperture, uint16 excluded_flags)
{
    ModelInstance *sel = 0;
    uint32 flags;
    sint32 distance, yaw, difference;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80066DFC, "SLUS_003.73");
    while (candidate)
    {
        VisualObject &object = *static_cast<VisualObject *>(candidate->owner);
        flags = object.lifecycle_flags;
        if (!(flags & 64) && (flags & 16) && !object.target_disabled)
        {
            distance = angles.SetDir(*pos, candidate->pos);
            if (distance < 400)
                return candidate;
            yaw = original_angles->comp[1];
            difference = angles.comp[1] - yaw;
            if (difference < -2048)
                difference += 4096;
            if (difference >= 2049)
                difference -= 4096;
            if (difference < 0)
                difference = -difference;
            if (difference < aperture && distance < range && !(object.activation_flags & excluded_flags))
            {
                aperture = difference;
                sel = candidate;
            }
        }
        candidate = candidate->next;
    }
    return sel;
}

ModelInstance *WeaponNode::SelectAimTarget(ShortVector3 &output, const Vector3 &origin, const ShortVector3 &original_angles, sint32 distance_limit, sint32 yaw_limit, sint32 pitch_limit, uint32 scale, uint16 exclude_mask, ModelInstance *list)
{
    FUNCTION_MARKER(0x80066F38, "SLUS_003.73");
    ModelInstance *sel = 0;
    sint32 best_distance = 1000000;
    Vector3 delta, target;
    ShortVector3 angles;
    while (list)
    {
        VisualObject &object = *static_cast<VisualObject *>(list->owner);
        uint32 excluded = object.activation_flags & exclude_mask;
        uint32 flags = excluded ? 0 : object.lifecycle_flags;
        if (!excluded && !(flags & 64) && (flags & 16) && !object.target_disabled)
        {
            sint32 distance, yaw, pitch;
            delta.Multiply(object.motion.velocity, scale);
            target.Add(list->pos, delta);
            distance = angles.SetDir(origin, target);
            if (distance <= distance_limit)
            {
                yaw = angles.comp[1] - original_angles[1];
                if (yaw < -2048)
                    yaw += 4096;
                if (yaw >= 2049)
                    yaw -= 4096;
                if (yaw < 0)
                    yaw = -yaw;
                if (distance_limit > 0)
                {
                    uint32 adjust = yaw;
                    adjust = DivideSignedWord((adjust * distance) << 4, distance_limit);
                    yaw += adjust;
                }
                if (yaw <= yaw_limit)
                {
                    pitch = angles.comp[0] - original_angles[0];
                    if (pitch < -2048)
                        pitch += 4096;
                    if (pitch >= 2049)
                        pitch -= 4096;
                    if (pitch < 0)
                        pitch = -pitch;
                    if (distance_limit > 0)
                    {
                        uint32 adjust = pitch;
                        adjust = DivideSignedWord((adjust * distance) << 4, distance_limit);
                        pitch += adjust;
                    }
                    if (pitch <= pitch_limit && distance < best_distance)
                    {
                        sel = list;
                        best_distance = distance;
                        output = angles;
                    }
                }
            }
        }
        list = list->next;
    }
    if (!sel)
        output = original_angles;
    return sel;
}
