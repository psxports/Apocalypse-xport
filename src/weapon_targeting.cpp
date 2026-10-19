#include "visual_object.h"
#include "weapon_targeting.h"
#include "vector_arithmetic.h"
#include "angle_math.h"

ModelInstance *SelectWeaponTarget(const uint32 *position, const sint16 *original_angles, ModelInstance *candidate, uint32 range, uint32 aperture, uint32 excluded_flags)
{
    ModelInstance *selected = 0;
    uint32 flags, distance, yaw, difference;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80066DFC, "SLUS_003.73");
    excluded_flags &= 65535;
    while (candidate)
    {
        VisualObject &object = *static_cast<VisualObject *>(candidate->owner);
        flags = object.lifecycle_flags;
        if (!(flags & 64) && (flags & 16) && !object.target_disabled)
        {
            distance = CalculateDirectionAngles(angles.words, position, candidate->pos.words);
            if ((sint32)distance < 400)
                return candidate;
            yaw = original_angles[1];
            difference = angles.components[1] - yaw;
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

ModelInstance *SelectAimTarget(ShortVector3 &output, const Vector3 &origin, const ShortVector3 &original_angles, uint32 distance_limit, uint32 yaw_limit, uint32 pitch_limit, uint32 scale, uint32 exclude_mask, ModelInstance *list)
{
    FUNCTION_MARKER(0x80066F38, "SLUS_003.73");
    ModelInstance *selected = 0;
    sint32 best_distance = 1000000;
    Vector3 delta, target;
    ShortVector3 angles;
    while (list)
    {
        VisualObject &object = *static_cast<VisualObject *>(list->owner);
        uint32 excluded = object.activation_flags & (exclude_mask & 65535);
        uint32 flags = excluded ? 0 : object.lifecycle_flags;
        if (!excluded && !(flags & 64) && (flags & 16) && !object.target_disabled)
        {
            sint32 distance, yaw, pitch;
            MultiplyVector(delta.words, object.motion.velocity.words, scale);
            AddVectors(target.words, list->pos.words, delta.words);
            distance = (sint32)CalculateDirectionAngles(angles.words, origin.words, target.words);
            if (distance <= (sint32)distance_limit)
            {
                yaw = angles.components[1] - original_angles[1];
                if (yaw < -2048)
                    yaw += 4096;
                if (yaw >= 2049)
                    yaw -= 4096;
                if (yaw < 0)
                    yaw = -yaw;
                if ((sint32)distance_limit > 0)
                    yaw = (sint32)((uint32)yaw + (uint32)((sint32)(((uint32)yaw * (uint32)distance) << 4) / (sint32)distance_limit));
                if (yaw <= (sint32)yaw_limit)
                {
                    pitch = angles.components[0] - original_angles[0];
                    if (pitch < -2048)
                        pitch += 4096;
                    if (pitch >= 2049)
                        pitch -= 4096;
                    if (pitch < 0)
                        pitch = -pitch;
                    if ((sint32)distance_limit > 0)
                        pitch = (sint32)((uint32)pitch + (uint32)((sint32)(((uint32)pitch * (uint32)distance) << 4) / (sint32)distance_limit));
                    if (pitch <= (sint32)pitch_limit && distance < best_distance)
                    {
                        selected = list;
                        best_distance = distance;
                        output = angles;
                    }
                }
            }
        }
        list = list->next;
    }
    if (!selected)
        output = original_angles;
    return selected;
}
