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

uint32 SelectWeaponTarget(const uint32 *position, const sint16 *original_angles, uint32 candidate, uint32 range, uint32 aperture, uint32 excluded_flags)
{
    uint32 selected = 0, flags, distance, yaw, difference;
    sint16 angles[3];
    FUNCTION_MARKER(0x80066DFC, "SLUS_003.73");
    excluded_flags &= 65535;
    while (candidate)
    {
        flags = r_u16(candidate + 78);
        if (!(flags & 64) && (flags & 16) && !r_u8(candidate + 224))
        {
            distance = CalculateDirectionAngles((uint16 *)angles, position, (const uint32 *)psx_addr(candidate + 4, 12));
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
            if ((sint32)difference < (sint32)aperture && (sint32)distance < (sint32)range && !(r_u16(candidate + 76) & excluded_flags))
            {
                aperture = difference;
                selected = candidate;
            }
        }
        candidate = r_u32(candidate + 28);
    }
    return selected;
}

extern "C" uint32 sub_80066DFC(uint32 position, uint32 angles, uint32 list, uint32 range, uint32 aperture, uint32 excluded_flags)
{
    FUNCTION_MARKER(0x80066DFC, "SLUS_003.73");
    return SelectWeaponTarget((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), list, range, aperture, excluded_flags);
}

extern "C" uint32 apocalypse_weapon_target_native(const uint32 *position, const sint16 *angles, uint32 list, uint32 range, uint32 aperture, uint32 excluded_flags)
{
    return SelectWeaponTarget(position, angles, list, range, aperture, excluded_flags);
}

static sint32 ReadTargetAngle(const sint16 *angles, uint32 index)
{
    uint32 address = GuestMemoryAddress(angles);
    if (address)
        return (sint16)r_u16(address + 2 * index);
    sint16 value;
    memcpy(&value, (const uint8 *)angles + 2 * index, 2);
    return value;
}

static void CopyTargetAngles(sint16 *output, const sint16 *source)
{
    uint32 input_address = GuestMemoryAddress(source);
    uint32 output_address = GuestMemoryAddress(output);
    uint32 packed;
    uint16 tail;
    if (input_address)
    {
        packed = r_u32(input_address);
        tail = r_u16(input_address + 4);
    }
    else
    {
        memcpy(&packed, source, 4);
        memcpy(&tail, (const uint8 *)source + 4, 2);
    }
    if (output_address)
    {
        w_u32(output_address, packed);
        w_u16(output_address + 4, tail);
    }
    else
    {
        memcpy(output, &packed, 4);
        memcpy((uint8 *)output + 4, &tail, 2);
    }
}

uint32 SelectAimTarget(sint16 output[3], const uint32 origin[3], const sint16 original_angles[3], uint32 distance_limit, uint32 yaw_limit, uint32 pitch_limit, uint32 scale, uint32 exclude_mask, uint32 list)
{
    FUNCTION_MARKER(0x80066F38, "SLUS_003.73");
    uint32 selected = 0;
    sint32 best_distance = 1000000;
    uint32 delta[3], target[3];
    sint16 angles[3];
    while (list)
    {
        uint32 excluded = r_u16(list + 76) & (exclude_mask & 65535);
        uint32 flags = excluded ? 0 : r_u16(list + 78);
        if (!excluded && !(flags & 64) && (flags & 16) && !r_u8(list + 224))
        {
            sint32 distance, yaw, pitch;
            MultiplyVector(delta, (const uint32 *)psx_addr(list + 104, 12), &scale);
            AddVectors(target, (const uint32 *)psx_addr(list + 4, 12), delta);
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
        list = r_u32(list + 28);
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

extern "C" uint32 sub_80066F38(uint32 output, uint32 origin, uint32 angles, uint32 distance_limit, uint32 yaw_limit, uint32 pitch_limit, uint32 scale, uint32 exclude_mask, uint32 list)
{
    FUNCTION_MARKER(0x80066F38, "SLUS_003.73");
    return SelectAimTarget((sint16 *)psx_addr(output, 6), (const uint32 *)psx_addr(origin, 12), (const sint16 *)psx_addr(angles, 6), distance_limit, yaw_limit, pitch_limit, scale, exclude_mask, list);
}
