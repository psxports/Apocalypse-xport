#include "visual_object.h"
#include "weapon_targeting.h"
#include "native_object_storage.h"
#include "vector_arithmetic.h"
#include "angle_math.h"
#include <string.h>
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

static sint32 ReadTargetAngle(const sint16 *angles, uint32 index);

ModelInstance *SelectWeaponTarget(const uint32 *position, const sint16 *original_angles, ModelInstance *candidate, uint32 range, uint32 aperture, uint32 excluded_flags)
{
    ModelInstance *selected = 0;
    uint32 flags, distance, yaw, difference;
    sint16 angles[3];
    FUNCTION_MARKER(0x80066DFC, "SLUS_003.73");
    excluded_flags &= 65535;
    while (candidate)
    {
        VisualObject &object = *static_cast<VisualObject *>(candidate->owner);
        flags = object.lifecycle_flags;
        if (!(flags & 64) && (flags & 16) && !r_u8(NativeObjectGuestAddress(&object) + 224))
        {
            distance = CalculateDirectionAngles((uint16 *)angles, position, candidate->pos);
            if ((sint32)distance < 400)
                return candidate;
            yaw = (uint32)ReadTargetAngle(original_angles, 1);
            difference = (uint32)(sint32)angles[1] - yaw;
            if ((sint32)difference < -2048)
                difference += 4096;
            if ((sint32)difference >= 2049)
                difference -= 4096;
            if ((sint32)difference < 0)
                difference = 0 - difference;
            if ((sint32)difference < (sint32)aperture && (sint32)distance < (sint32)range && !(object.activation_flags & excluded_flags))
            {
                aperture = difference;
                selected = candidate;
            }
        }
        candidate = candidate->next;
    }
    return selected;
}



extern "C" uint32 apocalypse_weapon_target_native(const uint32 *position, const sint16 *angles, uint32 list, uint32 range, uint32 aperture, uint32 excluded_flags)
{
    return SelectWeaponTarget(position, angles, list, range, aperture, excluded_flags);
}

static sint32 ReadTargetAngle(const sint16 *angles, uint32 index)
{
    sint16 value;
    memcpy(&value, (const uint8 *)angles + 2 * index, 2);
    return value;
}

static void CopyTargetAngles(sint16 *output, const sint16 *source)
{
    uint32 packed;
    uint16 tail;
    memcpy(&packed, source, 4);
    memcpy(&tail, (const uint8 *)source + 4, 2);
    memcpy(output, &packed, 4);
    memcpy((uint8 *)output + 4, &tail, 2);
}

ModelInstance *SelectAimTarget(sint16 output[3], const uint32 origin[3], const sint16 original_angles[3], uint32 distance_limit, uint32 yaw_limit, uint32 pitch_limit, uint32 scale, uint32 exclude_mask, ModelInstance *list)
{
    FUNCTION_MARKER(0x80066F38, "SLUS_003.73");
    ModelInstance *selected = 0;
    sint32 best_distance = 1000000;
    uint32 delta[3], target[3];
    sint16 angles[3];
    while (list)
    {
        VisualObject &object = *static_cast<VisualObject *>(list->owner);
        uint32 excluded = object.activation_flags & (exclude_mask & 65535);
        uint32 flags = excluded ? 0 : object.lifecycle_flags;
        if (!excluded && !(flags & 64) && (flags & 16) && !r_u8(NativeObjectGuestAddress(&object) + 224))
        {
            sint32 distance, yaw, pitch;
            MultiplyVector(delta, (const uint32 *)object.motion.velocity, &scale);
            AddVectors(target, list->pos, delta);
            distance = (sint32)CalculateDirectionAngles((uint16 *)angles, origin, target);
            if (distance <= (sint32)distance_limit)
            {
                yaw = (sint32)angles[1] - ReadTargetAngle(original_angles, 1);
                if (yaw < -2048)
                    yaw += 4096;
                if (yaw >= 2049)
                    yaw -= 4096;
                if (yaw < 0)
                    yaw = (sint32)(0 - (uint32)yaw);
                if ((sint32)distance_limit > 0)
                    yaw = (sint32)((uint32)yaw + (uint32)((sint32)(((uint32)yaw * (uint32)distance) << 4) / (sint32)distance_limit));
                if (yaw <= (sint32)yaw_limit)
                {
                    pitch = (sint32)angles[0] - ReadTargetAngle(original_angles, 0);
                    if (pitch < -2048)
                        pitch += 4096;
                    if (pitch >= 2049)
                        pitch -= 4096;
                    if (pitch < 0)
                        pitch = (sint32)(0 - (uint32)pitch);
                    if ((sint32)distance_limit > 0)
                        pitch = (sint32)((uint32)pitch + (uint32)((sint32)(((uint32)pitch * (uint32)distance) << 4) / (sint32)distance_limit));
                    if (pitch <= (sint32)pitch_limit && distance < best_distance)
                    {
                        selected = list;
                        best_distance = distance;
                        CopyTargetAngles(output, angles);
                    }
                }
            }
        }
        list = list->next;
    }
    if (!selected)
    {
        CopyTargetAngles(output, original_angles);
    }
    return selected;
}

extern "C" uint32 apocalypse_select_aim_target(sint16 output[3], const uint32 origin[3], const sint16 angles[3], uint32 distance_limit, uint32 yaw_limit, uint32 pitch_limit, uint32 scale, uint32 exclude_mask, uint32 list)
{
    return SelectAimTarget(output, origin, angles, distance_limit, yaw_limit, pitch_limit, scale, exclude_mask, list);
}


