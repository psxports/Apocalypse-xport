#include "visual_object.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <stdio.h>
#include <stdlib.h>
#include "object_force.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "native_object_storage.h"
#include "weapon_nodes.h"
#include <new>

ObjectForce::ObjectForce(uint32 mode, uint32 owner, uint32 first_list, uint32 second_list) : Force()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 previous;
    FUNCTION_MARKER(0x8003A154, "SLUS_003.73");
    w_u8(object + 8, mode);
    previous = r_u32(0x800FF470);
    w_u32(object, 0x800A1DD0);
    w_u32(object + 28, first_list);
    w_u32(object + 36, owner);
    w_u32(0x800FF470, object);
    w_u32(object + 32, second_list);
    w_u32(object + 4, previous);
}

uint32 ObjectForce::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 previous = 0, current;
    FUNCTION_MARKER(0x8003A188, "SLUS_003.73");
    current = r_u32(0x800FF470);
    w_u32(object, 0x800A1DD0);
    while (current && current != object)
    {
        previous = current;
        current = r_u32(previous + 4);
    }
    if (previous)
        w_u32(previous + 4, r_u32(object + 4));
    else
        w_u32(0x800FF470, r_u32(object + 4));
    w_u32(object, 0x800A1DE0);
    if (reason & 1)
        return ReleaseWeaponStorage(object);
    return 0;
}

void ObjectForce::SetPosition(uint32 position)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003A214, "SLUS_003.73");
    w_u32(object + 16, position);
}

void ObjectForce::SetStrength(uint32 strength, uint32 range_shift)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003A21C, "SLUS_003.73");
    w_u16(object + 10, strength);
    w_u8(object + 9, range_shift);
}

uint32 ObjectForce::SetDirection(const uint16 *angles)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 direction[3], x, y, z;
    FUNCTION_MARKER(0x8003A228, "SLUS_003.73");
    BuildDirectionVector(direction, 1, angles);
    x = direction[0] & 65535;
    y = direction[1] & 65535;
    z = direction[2] & 65535;
    w_u16(object + 20, x);
    w_u16(object + 22, y);
    w_u16(object + 24, z);
    return x;
}

uint32 ObjectForce::SetRadius(uint32 radius)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8003A274, "SLUS_003.73");
    radius &= 65535;
    result = radius * radius;
    w_u32(object + 12, result);
    return result;
}


uint32 ObjectForce::ApplyDirectional(uint32 list)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 current, result, delta[3], shifted[3], shift = 12, axis, dot, projection;
    uint32 coefficient, range, strength, amount, mode, product, velocity, range_shift, large_product;
    FUNCTION_MARKER(0x8003A288, "SLUS_003.73");
    result = (uint32)(sint32)(sint16)r_u16(object + 10);
    if (!result)
        return result;
    current = r_u32(list);
    while (current)
    {
        result = r_u32(object + 36);
        if (current != result)
        {
            SubtractVectors(delta, (const uint32 *)psx_addr(current + 4, 12), (const uint32 *)psx_addr(r_u32(object + 16), 12));
            ShiftVectorRight(shifted, delta, &shift);
            coefficient = (uint32)(sint32)(sint16)r_u16(object + 20);
            dot = shifted[0] * coefficient;
            dot += shifted[1] * (uint32)(sint32)(sint16)r_u16(object + 22);
            dot += shifted[2] * (uint32)(sint32)(sint16)r_u16(object + 24);
            projection = (uint32)((sint32)dot >> 12);
            delta[0] = (shifted[0] << 12) - coefficient * projection;
            delta[1] = (shifted[1] << 12) - (uint32)(sint32)(sint16)r_u16(object + 22) * projection;
            result = (uint32)(sint32)(sint16)r_u16(object + 24);
            delta[2] = (shifted[2] << 12) - result * projection;
            if ((sint32)projection > 0)
            {
                result = MeasureSquaredVectorLength(delta);
                result = result < r_u32(object + 12);
                if (result)
                {
                    range_shift = r_u8(object + 9);
                    range = (uint32)1 << (range_shift & 31);
                    result = (sint32)projection < (sint32)range;
                    if (result)
                    {
                        if (current == visual_lists.players)
                            w_u32(current + 528, object);
                        strength = (uint32)(sint32)(sint16)r_u16(object + 10);
                        amount = (range - projection) * strength;
                        range_shift = r_u8(object + 9);
                        mode = r_u8(object + 8);
                        large_product = (sint32)amount > 262143;
                        if (mode != 3 && large_product)
                            amount = (uint32)((sint32)amount >> 8);
                        for (axis = 0; axis < 3; ++axis)
                        {
                            coefficient = (uint32)(sint32)(sint16)r_u16(object + 20 + axis * 2);
                            if (mode == 3)
                            {
                                if (axis)
                                    strength = (uint32)(sint32)(sint16)r_u16(object + 10);
                                product = coefficient * strength;
                            }
                            else
                            {
                                product = coefficient * amount;
                                // The large-product path changes the shift before multiplication
                                if (large_product)
                                {
                                    if (range_shift < 8)
                                        product <<= (8 - range_shift) & 31;
                                    else
                                        product = (uint32)((sint32)product >> ((range_shift - 8) & 31));
                                }
                                else
                                    product = (uint32)((sint32)product >> (range_shift & 31));
                            }
                            velocity = VisualObject::Attach(current).motion.velocity[axis];
                            result = velocity + product;
                            VisualObject::Attach(current).motion.velocity[axis] = result;
                        }
                    }
                }
            }
        }
        current = r_u32(current + 28);
    }
    return result;
}

uint32 ObjectForce::ApplyRadial(uint32 list, uint32 directional)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 current, result, delta[3], normalized[3], magnitude, shift = 12, axis;
    uint32 dot, coefficient, range_shift, range, amount, product, velocity, player;
    result = (uint32)(sint32)(sint16)r_u16(object + 10);
    if (!result)
        return result;
    current = r_u32(list);
    while (current)
    {
        result = r_u32(object + 36);
        if (current != result)
        {
            SubtractVectors(delta, (const uint32 *)psx_addr(current + 4, 12), (const uint32 *)psx_addr(r_u32(object + 16), 12));
            magnitude = MeasureVectorLength(delta);
            if (magnitude)
                DivideVector(normalized, delta, &magnitude);
            dot = 1;
            if (directional)
            {
                ShiftVectorRight(delta, &shift);
                dot = delta[0] * (uint32)(sint32)(sint16)r_u16(object + 20);
                dot += delta[1] * (uint32)(sint32)(sint16)r_u16(object + 22);
                dot += delta[2] * (uint32)(sint32)(sint16)r_u16(object + 24);
            }
            result = 1;
            if ((sint32)dot > 0)
            {
                range_shift = r_u8(object + 9);
                range = (uint32)1 << (range_shift & 31);
                result = (sint32)magnitude < (sint32)range;
                if (result)
                {
                    if (!magnitude)
                    {
                        // Human-approved zero impulse at the force center
                        normalized[0] = 0;
                        normalized[1] = 0;
                        normalized[2] = 0;
                    }
                    coefficient = (uint32)(sint32)(sint16)r_u16(object + 10);
                    amount = (range - magnitude) * coefficient;
                    for (axis = 0; axis < 3; ++axis)
                    {
                        product = normalized[axis] * amount;
                        velocity = VisualObject::Attach(current).motion.velocity[axis];
                        if (axis == 2)
                            player = visual_lists.players;
                        result = velocity + (uint32)((sint32)product >> (range_shift & 31));
                        VisualObject::Attach(current).motion.velocity[axis] = result;
                    }
                    if (current == player)
                        w_u32(current + 528, object);
                }
            }
        }
        current = r_u32(current + 28);
    }
    return result;
}

uint32 ObjectForce::ApplyDirectionalRadial(uint32 list)
{
    FUNCTION_MARKER(0x8003A5CC, "SLUS_003.73");
    return ApplyRadial(list, 1);
}

uint32 ObjectForce::ApplyOmnidirectional(uint32 list)
{
    FUNCTION_MARKER(0x8003A76C, "SLUS_003.73");
    return ApplyRadial(list, 0);
}

uint32 ObjectForce::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 mode, list, result;
    FUNCTION_MARKER(0x8003A8B0, "SLUS_003.73");
    mode = r_u8(object + 8);
    result = mode < 2 ? 1 : 3;
    if (mode > 3)
        return result;
    list = r_u32(object + 28);
    if (list)
    {
        if (mode == 1)
            result = ApplyDirectionalRadial(list);
        else if (mode == 2)
            result = ApplyOmnidirectional(list);
        else
            result = ApplyDirectional(list);
    }
    list = r_u32(object + 32);
    if (list)
    {
        if (mode == 1)
            return ApplyDirectionalRadial(list);
        if (mode == 2)
            return ApplyOmnidirectional(list);
        return ApplyDirectional(list);
    }
    return result;
}


extern "C" uint32 sub_8003A188(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003A188, "SLUS_003.73");
    return ObjectForce::Attach(object).ObjectForce::Destroy(reason);
}









void UpdateObjectForces(void)
{
    uint32 object;
    FUNCTION_MARKER(0x8003A9AC, "SLUS_003.73");
    object = r_u32(0x800FF470);
    while (object)
    {
        ObjectForce::Attach(object).Update();
        object = r_u32(object + 4);
    }
    // TODO Classify callers that consume the original undefined return when the list is empty
}

uint32 Force::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003A9EC, "SLUS_003.73");
    w_u32(object, 0x800A1DE0);
    if (reason & 1)
        return ::ReleaseWeaponStorage(object);
    return 0x800A1DE0;
}


extern "C" uint32 sub_8003A9EC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003A9EC, "SLUS_003.73");
    return Force::Attach(object).Force::Destroy(reason);
}

Force &Force::Attach(uint32 object)
{
    return *static_cast<Force *>(FindNativeObjectStorage(object));
}

ObjectForce &ObjectForce::Attach(uint32 object)
{
    return *static_cast<ObjectForce *>(FindNativeObjectStorage(object));
}

// Receiver ownership is unresolved; retain both explicit object arguments
uint32 *ApplyObjectContactImpulse(uint32 hero, uint32 object)
{
    uint32 delta[3], shifted[3], product[3], quotient[3], impulse[3];
    uint32 shift = 12, distance;
    FUNCTION_MARKER(0x80062C50, "SLUS_003.73");
    SubtractVectors(delta, (const uint32 *)psx_addr(hero + 4, 12), (const uint32 *)psx_addr(object + 4, 12));
    distance = MeasureVectorLength(delta);
    if (!distance)
        distance = 1;
    ShiftVectorRight(shifted, delta, &shift);
    MultiplyScalarVector(product, (const uint32 *)psx_addr(object + 208, 4), shifted);
    DivideVector(quotient, product, &distance);
    ShiftVectorLeft(impulse, quotient, &shift);
    AddVectors((uint32 *)VisualObject::Attach(hero).motion.velocity, impulse);
    return VisualObject::Attach(hero).motion.velocity;
}


