extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "scripted_level_object.h"
#include "object_collision_events.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "model_render.h"
#include "model_resources.h"
#include "record_scripts.h"
#include "model_projection.h"
#include "player.h"
#include "native_object_storage.h"

static uint32 ReadSegmentPoint(const uint32 *point, uint32 axis)
{
    uint32 address = GuestMemoryAddress(point);
    return address ? r_u32(address + axis * 4) : xport_load_le32((const uint8 *)point + axis * 4);
}

static void StoreCollisionSquareResult()
{
    w_u32(0x800ED5E4, xport_gte_read_data(25));
    w_u32(0x800ED5E8, xport_gte_read_data(26));
    w_u32(0x800ED5EC, xport_gte_read_data(27));
}

static uint32 ReadCollisionSquareSum()
{
    uint32 x = r_u32(0x800ED5E4);
    uint32 y = r_u32(0x800ED5E8);
    uint32 z = r_u32(0x800ED5EC);
    return x + y + z;
}

static void SquareCollisionVector(uint32 vector)
{
    xport_gte_write_data(9, r_u32(vector));
    xport_gte_write_data(10, r_u32(vector + 4));
    xport_gte_write_data(11, r_u32(vector + 8));
    xport_gte_execute(0xA00428);
    StoreCollisionSquareResult();
}

static void WriteCollisionResult(uint32 *output, uint32 axis, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u32(address + axis * 4, value);
    else
        xport_store_le32((uint8 *)output + axis * 4, value);
}

static uint32 ReadCollisionRayWord(const void *record, uint32 offset)
{
    uint32 address = GuestMemoryAddress(record);
    return address ? r_u32(address + offset) : xport_load_le32((const uint8 *)record + offset);
}

static void WriteCollisionRayWord(void *record, uint32 offset, uint32 value)
{
    uint32 address = GuestMemoryAddress(record);
    if (address)
        w_u32(address + offset, value);
    else
        xport_store_le32((uint8 *)record + offset, value);
}

static void WriteCollisionRayHalf(void *record, uint32 offset, uint16 value)
{
    uint32 address = GuestMemoryAddress(record);
    if (address)
        w_u16(address + offset, value);
    else
        xport_store_le16((uint8 *)record + offset, value);
}

static void WriteCollisionRayByte(void *record, uint32 offset, uint8 value)
{
    uint32 address = GuestMemoryAddress(record);
    if (address)
        w_u8(address + offset, value);
    else
        ((uint8 *)record)[offset] = value;
}

static uint32 ReadCollisionLeadingBits(uint32 value)
{
    xport_gte_write_data(30, value);
    return xport_gte_read_data(31);
}

static sint32 DivideCollisionWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator >= 0 ? -1 : 1;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return (sint32)numerator;
    return (sint32)numerator / (sint32)denominator;
}

static sint32 RemainderCollisionWord(uint32 numerator, uint32 denominator)
{
    uint32 quotient = (uint32)DivideCollisionWord(numerator, denominator);
    return (sint32)(numerator - quotient * denominator);
}

static uint32 ReadCollisionRayByte(const void *record, uint32 offset)
{
    uint32 address = GuestMemoryAddress(record);
    return address ? r_u8(address + offset) : xport_load_u8((const uint8 *)record + offset);
}

static uint32 ReadCollisionRayHalf(const void *record, uint32 offset)
{
    uint32 address = GuestMemoryAddress(record);
    return address ? r_u16(address + offset) : xport_load_le16((const uint8 *)record + offset);
}

uint32 ChooseCandidatePosition(uint32 object, uint32 *output, const uint32 *target, uint32 minimum, uint32 maximum, uint32 height_limit, uint32 spread, uint32 ground, uint32 check_collision)
{
    FUNCTION_MARKER(0x8004BA64, "SLUS_003.73");
    uint32 angle, table, radius, delta, sign, absolute, left, right, sine, offset;
    sint32 height;
    uint32 collision[35];
    uint32 pass;
    left = r_u32(object + 12);
    right = ReadCollisionRayWord(target, 8);
    delta = left - right;
    left = r_u32(object + 4);
    right = ReadCollisionRayWord(target, 0);
    angle = 1024 - (uint32)ratan2((sint32)delta, (sint32)(left - right));
    angle += RandomRange((uint32)(sint32)(sint16)spread);
    angle -= (uint32)((sint32)(sint16)spread >> 1);
    radius = minimum + RandomRange(maximum - minimum);
    table = 0x800F863C + 4 * (angle & 4095);
    sine = (uint32)(sint32)(sint16)r_u16(table);
    left = ReadCollisionRayWord(target, 0);
    WriteCollisionRayWord(output, 0, left + radius * sine);
    left = ReadCollisionRayWord(target, 4);
    WriteCollisionRayWord(output, 4, left);
    sine = (uint32)(sint32)(sint16)r_u16(table + 2);
    left = ReadCollisionRayWord(target, 8);
    WriteCollisionRayWord(output, 8, left + radius * sine);
    if (ground != 0)
    {
        height = (sint32)FindGroundHeight(output, 512, 512);
        if (height == -1)
            return 0;
        offset = (uint32)(sint32)(sint16)r_u16(object + 456) << 12;
        WriteCollisionRayWord(output, 4, (uint32)height - offset);
    }
    if (check_collision != 0)
    {
        for (pass = 0; pass < 2; ++pass)
        {
            collision[0] = r_u32(object + 4);
            collision[1] = r_u32(object + 8);
            collision[2] = r_u32(object + 12);
            if (!pass)
            {
                offset = (uint32)(sint32)(sint16)r_u16(object + 458) << 12;
                collision[1] -= offset;
            }
            collision[3] = ReadCollisionRayWord(output, 0);
            collision[4] = ReadCollisionRayWord(output, 4);
            collision[5] = ReadCollisionRayWord(output, 8);
            if (!pass)
            {
                offset = (uint32)(sint32)(sint16)r_u16(object + 458) << 12;
                collision[4] -= offset;
            }
            InitializeCollisionRay((uint8 *)collision);
            ((uint8 *)collision)[136] = 0;
            w_u32(0x800FF970, 1);
            QueryCollisionRay((uint8 *)collision, 1);
            uint32 hit = ReadCollisionRayWord(collision, 104);
            w_u32(0x800FF970, 0);
            if (hit != 0)
                return 0;
        }
    }
    left = r_u32(object + 8);
    right = ReadCollisionRayWord(output, 4);
    delta = left - right;
    sign = (uint32)((sint32)delta >> 31);
    absolute = (delta ^ sign) - sign;
    return (uint32)(((sint32)absolute >> 12) < (sint32)height_limit);
}

uint32 TraceSegmentCollision(const uint32 *start, const uint32 *end, uint32 list)
{
    FUNCTION_MARKER(0x80067508, "SLUS_003.73");
    uint32 hit[4];
    uint32 result = IntersectObjectSpheres(start, end, hit, list, 0);
    if (!result)
    {
        for (uint32 axis = 0; axis < 3; ++axis)
            w_u32(0x800ED638 + axis * 4, ReadSegmentPoint(start, axis));
        for (uint32 axis = 0; axis < 3; ++axis)
            w_u32(0x800ED644 + axis * 4, ReadSegmentPoint(end, axis));
        sub_8007BB24(0x800ED638);
        uint32 objects = r_u32(0x800FF5DC);
        w_u32(0x800FF974, 1);
        sub_8007BEA0(objects, 0x800ED638);
        result = r_u32(0x800ED6A0);
        w_u32(0x800FF974, 0);
    }
    return result;
}

uint32 IsSegmentVisible(const uint32 *start, const uint32 *end)
{
    FUNCTION_MARKER(0x800679A4, "SLUS_003.73");
    uint32 collision[36];
    uint8 *record = (uint8 *)collision;
    collision[0] = ReadCollisionRayWord(start, 0);
    collision[1] = ReadCollisionRayWord(start, 4);
    collision[2] = ReadCollisionRayWord(start, 8);
    collision[3] = ReadCollisionRayWord(end, 0);
    collision[4] = ReadCollisionRayWord(end, 4);
    collision[5] = ReadCollisionRayWord(end, 8);
    InitializeCollisionRay(record);
    QueryCollisionRay(record, 1);
    return ReadCollisionRayWord(record, 104) == 0;
}

uint32 FindGroundHeight(const uint32 *position, uint32 below, uint32 above)
{
    FUNCTION_MARKER(0x80067A18, "SLUS_003.73");
    uint32 collision[36];
    uint8 *record = (uint8 *)collision;
    collision[0] = ReadCollisionRayWord(position, 0);
    collision[1] = ReadCollisionRayWord(position, 4) - (below << 12);
    collision[2] = ReadCollisionRayWord(position, 8);
    collision[3] = ReadCollisionRayWord(position, 0);
    collision[4] = ReadCollisionRayWord(position, 4) + (above << 12);
    collision[5] = ReadCollisionRayWord(position, 8);
    InitializeCollisionRay(record);
    WriteCollisionRayByte(record, 136, 0);
    QueryCollisionRay(record, 1);
    if (ReadCollisionRayWord(record, 104))
    {
        if ((sint16)ReadCollisionRayHalf(record, 122) < -3071)
            return ReadCollisionRayWord(record, 112);
    }
    return 0xFFFFFFFF;
}

uint32 AdvanceCollisionGeneration()
{
    FUNCTION_MARKER(0x8007BAB0, "SLUS_003.73");
    uint32 generation = r_u16(0x800FF968) + 1;
    w_u16(0x800FF968, (uint16)generation);
    if (!(generation & 0xFFFF))
    {
        uint32 node = r_u32(0x800FF794);
        w_u16(0x800FF968, 1);
        while (node)
        {
            w_u16(node + 2, 0);
            node = r_u32(node + 28);
        }
        node = r_u32(0x800FF5DC);
        while (node)
        {
            w_u16(node + 2, 0);
            node = r_u32(node + 28);
        }
    }
    return 1;
}

uint32 InitializeCollisionRay(uint8 *record)
{
    FUNCTION_MARKER(0x8007BB24, "SLUS_003.73");
    uint32 guest_address = GuestMemoryAddress(record);
    uint32 first, second, delta, horizontal, horizontal_root, full, full_root;
    uint32 normalization, shift, full_shift, sine, cosine, length, generation;
    uint32 axis, start_address, end_address;
    sint32 dx, dy, dz;
    const uint8 *start, *end;
    MATRIX *matrix = (MATRIX *)((uint8 *)record + 72);

    WriteCollisionRayWord(record, 64, 0x7FFFFFFF);
    second = ReadCollisionRayWord(record, 12);
    first = ReadCollisionRayWord(record, 0);
    WriteCollisionRayWord(record, 104, 0);
    WriteCollisionRayWord(record, 132, 0xFFFFFFFF);
    WriteCollisionRayByte(record, 136, 0);
    WriteCollisionRayByte(record, 137, 0);
    delta = (uint32)((sint32)(second - first) >> 12);
    w_u16(0x800FFA3C, (uint16)delta);
    dx = (sint16)delta;
    horizontal = (uint32)(dx * dx);
    second = ReadCollisionRayWord(record, 16);
    first = ReadCollisionRayWord(record, 4);
    delta = (uint32)((sint32)(second - first) >> 12);
    w_u16(0x800FFA3E, (uint16)delta);
    dy = (sint16)delta;
    second = ReadCollisionRayWord(record, 20);
    first = ReadCollisionRayWord(record, 8);
    delta = (uint32)((sint32)(second - first) >> 12);
    dz = (sint16)delta;
    w_u16(0x800FFA40, (uint16)delta);
    horizontal += (uint32)(dz * dz);
    if (horizontal)
    {
        normalization = ReadCollisionLeadingBits(horizontal) - 1;
        shift = (uint32)((sint32)normalization >> 1);
        horizontal_root = LookupDistanceRoot(horizontal << (normalization & 30));
        dx = (sint16)r_u16(0x800FFA3C);
        sine = (uint32)DivideCollisionWord((uint32)dx << ((shift + 12) & 31), horizontal_root);
        dz = (sint16)r_u16(0x800FFA40);
        cosine = (uint32)DivideCollisionWord((uint32)dz << ((shift + 12) & 31), horizontal_root);
        w_u16(0x800FFA2E, (uint16)sine);
        w_u16(0x800FFA2C, (uint16)cosine);
        w_u16(0x800ED740, (uint16)cosine);
        w_u16(0x800ED742, 0);
        w_u16(0x800ED744, (uint16)(0 - sine));
        w_u16(0x800ED746, 0);
        w_u16(0x800ED748, 4096);
        w_u16(0x800ED74A, 0);
        w_u16(0x800ED74C, (uint16)sine);
        w_u16(0x800ED74E, 0);
        w_u16(0x800ED750, (uint16)cosine);
        dy = (sint16)r_u16(0x800FFA3E);
        full = horizontal + (uint32)(dy * dy);
        normalization = ReadCollisionLeadingBits(full) - 1;
        full_shift = (uint32)((sint32)normalization >> 1);
        full_root = LookupDistanceRoot(full << (normalization & 30));
        cosine = (uint32)DivideCollisionWord(horizontal_root << 12, full_root << ((shift - full_shift) & 31));
        length = (uint32)((sint32)full_root >> (full_shift & 31));
        WriteCollisionRayWord(record, 68, length);
        dy = (sint16)r_u16(0x800FFA3E);
        sine = (uint32)DivideCollisionWord((uint32)dy << ((full_shift + 12) & 31), full_root);
        w_u16(0x800FFA2C, (uint16)cosine);
        w_u16(0x800FFA2E, (uint16)sine);
        WriteCollisionRayHalf(record, 72, 4096);
        WriteCollisionRayHalf(record, 74, 0);
        WriteCollisionRayHalf(record, 76, 0);
        WriteCollisionRayHalf(record, 78, 0);
        WriteCollisionRayHalf(record, 80, (uint16)cosine);
        WriteCollisionRayHalf(record, 82, (uint16)(0 - sine));
        WriteCollisionRayHalf(record, 84, 0);
        WriteCollisionRayHalf(record, 86, (uint16)sine);
        WriteCollisionRayHalf(record, 88, (uint16)cosine);
        MulMatrix0(matrix, (MATRIX *)psx_addr(0x800ED740, sizeof(MATRIX)), matrix);
    }
    else
    {
        sine = dy < 0 ? 0xFFFFF000 : 4096;
        w_u16(0x800FFA2E, (uint16)sine);
        WriteCollisionRayWord(record, 68, dy < 0 ? 0 - (uint32)dy : (uint32)dy);
        if (dy >= 0)
            WriteCollisionRayByte(record, 137, 1);
        WriteCollisionRayHalf(record, 72, 4096);
        WriteCollisionRayHalf(record, 74, 0);
        WriteCollisionRayHalf(record, 76, 0);
        WriteCollisionRayHalf(record, 78, 0);
        WriteCollisionRayHalf(record, 80, 0);
        sine = r_u16(0x800FFA2E);
        WriteCollisionRayHalf(record, 84, 0);
        WriteCollisionRayHalf(record, 88, 0);
        WriteCollisionRayHalf(record, 82, (uint16)(0 - sine));
        WriteCollisionRayHalf(record, 86, (uint16)sine);
    }
    SetRotMatrix(matrix);
    first = ReadCollisionRayWord(record, 0);
    second = ReadCollisionRayWord(record, 12);
    if (guest_address)
    {
        w_u32(0x800FFA68, guest_address);
        w_u32(0x800FFA6C, guest_address + 12);
    }
    if ((sint32)first < (sint32)second)
        WriteCollisionRayWord(record, 24, first);
    else
    {
        first = ReadCollisionRayWord(record, 0);
        WriteCollisionRayWord(record, 24, second);
        second = first;
    }
    WriteCollisionRayWord(record, 36, second);
    for (axis = 1; axis < 3; ++axis)
    {
        if (guest_address)
        {
            start_address = r_u32(0x800FFA68);
            end_address = r_u32(0x800FFA6C);
            start = (const uint8 *)psx_addr(start_address, 12);
            end = (const uint8 *)psx_addr(end_address, 12);
        }
        else
        {
            /* Local C records retain native pointer carriers */
            start = (const uint8 *)record;
            end = start + 12;
        }
        first = ReadCollisionRayWord(start, axis * 4);
        second = ReadCollisionRayWord(end, axis * 4);
        if ((sint32)first < (sint32)second)
        {
            WriteCollisionRayWord(record, 24 + axis * 4, first);
            second = ReadCollisionRayWord(end, axis * 4);
        }
        else
        {
            WriteCollisionRayWord(record, 24 + axis * 4, second);
            second = ReadCollisionRayWord(start, axis * 4);
        }
        WriteCollisionRayWord(record, 36 + axis * 4, second);
    }
    AdvanceCollisionGeneration();
    generation = r_u16(0x800FF968);
    WriteCollisionRayHalf(record, 138, (uint16)generation);
    return generation;
}

void VisitCollisionObjects(uint32 list, uint8 *record)
{
    FUNCTION_MARKER(0x8007BEA0, "SLUS_003.73");
    uint32 generation;
    if (!list)
        return;
    if (!ReadCollisionRayWord(record, 68))
        return;
    SetRotMatrix((MATRIX *)((uint8 *)record + 72));
    generation = ReadCollisionRayHalf(record, 138);
    MarkLinkedCollisionCandidates(list, (const sint32 *)record, generation);
    do
    {
        generation = ReadCollisionRayHalf(record, 138);
        if (r_u16(list + 2) != generation)
        {
            w_u16(list + 2, generation);
            (void)IntersectCollisionObject(list, record);
        }
        list = r_u32(list + 28);
    } while (list);
}

void VisitCollisionCells(uint32 list, uint8 *record)
{
    FUNCTION_MARKER(0x8007BF2C, "SLUS_003.73");
    uint32 object, generation;
    if (!list)
        return;
    if (!ReadCollisionRayWord(record, 68))
        return;
    SetRotMatrix((MATRIX *)((uint8 *)record + 72));
    generation = ReadCollisionRayHalf(record, 138);
    MarkArrayCollisionCandidates(list, (const sint32 *)record, generation);
    if (!r_u32(list))
        return;
    do
    {
        object = r_u32(list);
        generation = ReadCollisionRayHalf(record, 138);
        if (r_u16(object + 2) != generation)
        {
            w_u16(object + 2, generation);
            object = r_u32(list);
            (void)IntersectCollisionObject(object, record);
        }
        list += 4;
    } while (r_u32(list));
}

uint32 FinishCollisionRay(uint8 *record)
{
    FUNCTION_MARKER(0x8007BFD4, "SLUS_003.73");
    uint32 object = ReadCollisionRayWord(record, 104);
    if (!object)
        return 0;
    BuildModelRotationMatrix((const uint32 *)psx_addr(object + 16, 8), (uint16 *)psx_addr(0x800ED720, 18));
    SetRotMatrix((MATRIX *)psx_addr(0x800ED720, sizeof(MATRIX)));
    uint32 normal = r_u32(0x800ED61C);
    xport_gte_write_data(0, r_u32(normal));
    xport_gte_write_data(1, r_u32(normal + 4));
    xport_gte_execute(0x486012);
    uint32 normal_x = xport_gte_read_data(9);
    uint32 normal_y = xport_gte_read_data(10);
    uint32 normal_z = xport_gte_read_data(11);
    WriteCollisionRayHalf(record, 120, (uint16)normal_x);
    WriteCollisionRayHalf(record, 122, (uint16)normal_y);
    WriteCollisionRayHalf(record, 124, (uint16)normal_z);
    return 1;
}

uint32 IntersectCollisionObject(uint32 object, uint8 *record)
{
    FUNCTION_MARKER(0x8007C06C, "SLUS_003.73");
    uint32 result, model, entry, geometry, count, output, index, trigger, target, axis;
    uint32 first, second, height_mode;
    sint32 translation[3];
    result = r_u16(object) & 0x21;
    if (result)
        return result;
    SetColorMatrix((MATRIX *)((uint8 *)record + 72));
    model = r_u8(object + 27);
    index = r_u16(object + 22);
    entry = r_u32(0x800EAEF8 + 64 * model + 16);
    geometry = r_u32(entry + 4 * index);
    count = r_u32(geometry + 4);
    w_u32(0x800FF9E8, geometry);
    output = count < 129 ? 0x1F800000 : r_u32(0x800FFAC0);
    if (!r_u32(object + 16) && !r_u16(object + 20) && !(r_u16(object) & 0x200))
    {
        for (axis = 0; axis < 3; ++axis)
        {
            first = r_u32(object + 4 + axis * 4);
            second = ReadCollisionRayWord(record, axis * 4);
            translation[axis] = (sint32)(first - second) >> 12;
        }
        xport_gte_write_control(5, 0);
        xport_gte_write_control(6, 0);
        xport_gte_write_control(7, 0);
    }
    else
    {
        height_mode = ReadCollisionRayByte(record, 137);
        first = r_u32(object + 4);
        second = ReadCollisionRayWord(record, 0);
        w_u32(0x800ED734, (uint32)((sint32)(first - second) >> 12));
        if (height_mode)
        {
            first = ReadCollisionRayWord(record, 8);
            second = r_u32(object + 12);
            w_u32(0x800ED738, (uint32)((sint32)(first - second) >> 12));
            first = r_u32(object + 8);
            second = ReadCollisionRayWord(record, 4);
            w_u32(0x800ED73C, (uint32)((sint32)(first - second) >> 12));
        }
        else
        {
            first = r_u32(object + 8);
            second = ReadCollisionRayWord(record, 4);
            w_u32(0x800ED738, (uint32)((sint32)(first - second) >> 12));
            first = r_u32(object + 12);
            second = ReadCollisionRayWord(record, 8);
            w_u32(0x800ED73C, (uint32)((sint32)(first - second) >> 12));
            second = r_u16(0x800ED738);
            first = r_u16(0x800ED734);
            xport_gte_write_data(0, first | (second << 16));
            xport_gte_write_data(1, r_u32(0x800ED73C));
            xport_gte_execute(0x4C6012);
            w_u32(0x800ED734, xport_gte_read_data(25));
            w_u32(0x800ED738, xport_gte_read_data(26));
            w_u32(0x800ED73C, xport_gte_read_data(27));
        }
        SetTransMatrix((MATRIX *)psx_addr(0x800ED720, 32));
        translation[0] = translation[1] = translation[2] = 0;
        if (r_u32(object + 16) || r_u16(object + 20))
        {
            BuildModelRotationMatrix((const uint32 *)psx_addr(object + 16, 8), (uint16 *)psx_addr(0x800ED720, 18));
            if (r_u16(object) & 0x200)
                ScaleModelRotationMatrix((const uint8 *)psx_addr(object, 42), (uint16 *)psx_addr(0x800ED720, 18));
        }
        else
        {
            InitializeModelIdentityMatrix((uint32 *)psx_addr(0x800ED720, 20));
            ScaleModelRotationMatrix((const uint8 *)psx_addr(object, 42), (uint16 *)psx_addr(0x800ED720, 18));
        }
        TransformPackedMatrixForProjection(0x800ED720, 0x800ED720);
        SetColorMatrix((MATRIX *)psx_addr(0x800ED720, 32));
    }
    geometry = r_u32(0x800FF9E8);
    first = ReadCollisionRayWord(record, 68);
    result = TransformCollisionVertices(geometry, output, first, translation);
    w_u16(0x800FFA32, result);
    result &= 0x60F;
    if (result)
        return result;
    geometry = r_u32(0x800FF9E8);
    w_u32(0x800FFA88, 0xFFFFFFFF);
    (void)IntersectCollisionPolygons(geometry, output, record, object);
    result = r_u32(0x800FFA88);
    if (result == 0xFFFFFFFF)
        return result;
    index = FindActiveModelBankSlot(r_u8(object + 27));
    trigger = r_u32(0x800FFA88);
    entry = r_u32(0x800FF7B4 + 4 * index);
    target = r_u32(entry + 4 * trigger);
    w_u32(0x800FF798, index ^ 1);
    w_u32(0x800FF978, 0);
    return ActivateResourceScript(target);
}

uint32 IntersectObjectSpheres(const uint32 *start, const uint32 *end, uint32 *output, uint32 list, uint32 excluded)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    uint32 difference[3], vector[3], shifted[3], shift = 12, axis;
    w_u32(0x800FFA84, 0);
    w_u32(0x800ED634, 0x7FFFFFFF);
    SubtractVectors(difference, end, start);
    ShiftVectorRight(vector, difference, &shift);
    for (axis = 0; axis < 3; ++axis)
        w_u32(0x800ED5C0 + axis * 4, vector[axis]);
    SquareCollisionVector(0x800ED5C0);
    uint32 length = LookupDistanceRoot(ReadCollisionSquareSum());
    w_u32(0x800FFA14, length);
    if (!length)
        return 0;
    ShiftVectorLeft(shifted, (const uint32 *)psx_addr(0x800ED5C0, 12), &shift);
    DivideVector(vector, shifted, (const uint32 *)psx_addr(0x800FFA14, 4));
    for (axis = 0; axis < 3; ++axis)
        w_u32(0x800ED5C0 + axis * 4, vector[axis]);
    uint32 normal_x = r_u32(0x800ED5C0);
    uint32 normal_y = r_u32(0x800ED5C4);
    xport_gte_write_control(0, normal_x);
    uint32 normal_z = r_u32(0x800ED5C8);
    xport_gte_write_control(2, normal_y);
    xport_gte_write_control(4, normal_z);
    normal_x = r_u16(0x800ED5C0);
    normal_y = r_u16(0x800ED5C4);
    normal_z = r_u16(0x800ED5C8);
    w_u16(0x800FFA44, normal_x);
    w_u16(0x800FFA46, normal_y);
    w_u16(0x800FFA48, normal_z);
    normal_x = r_u32(0x800FFA44);
    normal_y = r_u32(0x800FFA48);
    xport_gte_write_control(8, normal_x);
    xport_gte_write_control(9, normal_y);
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 first = ReadSegmentPoint(start, axis);
        uint32 second = ReadSegmentPoint(end, axis);
        if ((sint32)first < (sint32)second)
        {
            w_u32(0x800ED5CC + axis * 4, first);
            second = ReadSegmentPoint(end, axis);
        }
        else
        {
            w_u32(0x800ED5CC + axis * 4, second);
            second = ReadSegmentPoint(start, axis);
        }
        w_u32(0x800ED5D8 + axis * 4, second);
    }
    for (uint32 object = list; object; object = r_u32(object + 28))
    {
        if (r_u16(object + 78) & 64)
            continue;
        if (object == excluded)
            continue;
        uint32 radius = (uint32)r_u16(object + 212) << 12;
        uint32 x = r_u32(object + 4);
        uint32 bound = r_u32(0x800ED5CC);
        w_u32(0x800FF9F0, radius);
        if ((sint32)(x + radius) < (sint32)bound)
            continue;
        bound = r_u32(0x800ED5D8);
        if ((sint32)bound < (sint32)(x - radius))
            continue;
        uint32 y = r_u32(object + 8);
        bound = r_u32(0x800ED5D0);
        if ((sint32)(y + radius) < (sint32)bound)
            continue;
        bound = r_u32(0x800ED5DC);
        if ((sint32)bound < (sint32)(y - radius))
            continue;
        uint32 z = r_u32(object + 12);
        bound = r_u32(0x800ED5D4);
        if ((sint32)(z + radius) < (sint32)bound)
            continue;
        bound = r_u32(0x800ED5E0);
        if ((sint32)bound < (sint32)(z - radius))
            continue;
        SubtractVectors(difference, (const uint32 *)psx_addr(object + 4, 12), start);
        ShiftVectorRight(vector, difference, &shift);
        for (axis = 0; axis < 3; ++axis)
            w_u32(0x800ED5F0 + axis * 4, vector[axis]);
        xport_gte_write_data(11, r_u32(0x800ED5F8));
        xport_gte_write_data(9, r_u32(0x800ED5F0));
        xport_gte_write_data(10, r_u32(0x800ED5F4));
        xport_gte_execute(0x178000C);
        xport_gte_execute(0xA00428);
        StoreCollisionSquareResult();
        radius = r_u16(object + 212);
        uint32 squared_radius = radius * radius;
        uint32 squared = ReadCollisionSquareSum();
        w_u32(0x800FFA10, squared_radius);
        if ((sint32)squared >= (sint32)squared_radius)
            continue;
        uint32 packed_y = r_u16(0x800ED5F4);
        uint32 packed_x = r_u16(0x800ED5F0);
        xport_gte_write_data(0, packed_x | (packed_y << 16));
        xport_gte_write_data(1, r_u32(0x800ED5F8));
        xport_gte_execute(0x4A6012);
        w_u32(0x800FFA18, xport_gte_read_data(25));
        uint32 projection = r_u32(0x800FFA18);
        uint32 best = r_u32(0x800ED634);
        if ((sint32)projection >= (sint32)best)
            continue;
        bool test_endpoint = false;
        if ((sint32)projection < 0)
        {
            SquareCollisionVector(0x800ED5F0);
            test_endpoint = true;
        }
        else
        {
            length = r_u32(0x800FFA14);
            if ((sint32)length < (sint32)projection)
            {
                SubtractVectors(difference, (const uint32 *)psx_addr(object + 4, 12), end);
                ShiftVectorRight(vector, difference, &shift);
                for (axis = 0; axis < 3; ++axis)
                    w_u32(0x800ED5FC + axis * 4, vector[axis]);
                SquareCollisionVector(0x800ED5FC);
                test_endpoint = true;
            }
        }
        if (test_endpoint)
        {
            squared = ReadCollisionSquareSum();
            squared_radius = r_u32(0x800FFA10);
            if ((sint32)squared >= (sint32)squared_radius)
                continue;
        }
        projection = r_u32(0x800FFA18);
        w_u32(0x800FFA84, object);
        w_u32(0x800ED634, projection);
    }
    uint32 result = r_u32(0x800FFA84);
    if (!result)
        return result;
    xport_gte_write_data(9, r_u32(0x800ED5C0));
    xport_gte_write_data(10, r_u32(0x800ED5C4));
    xport_gte_write_data(11, r_u32(0x800ED5C8));
    xport_gte_write_data(8, r_u32(0x800ED634));
    xport_draft_gte_execute(0x198003D);
    for (axis = 0; axis < 3; ++axis)
        WriteCollisionResult(output, axis, xport_gte_read_data(25 + axis));
    ShiftVectorLeft(output, &shift);
    AddVectors(output, start);
    return r_u32(0x800FFA84);
}

uint32 QueryCollisionRay(uint8 *record, uint32 include_objects)
{
    FUNCTION_MARKER(0x8007DD04, "SLUS_003.73");
    uint32 next_grid;
    sint32 start_x;
    sint32 end_x;
    sint32 start_z;
    sint32 min_x;
    sint32 end_z;
    uint32 grid_cursor;
    sint32 max_x;
    sint32 min_z;
    sint32 max_z;
    sint32 cell_size;
    sint32 cell_x;
    sint32 initial_cell_z;
    sint32 cell_index_z;
    sint32 cell_z;
    sint32 clipped_z0;
    sint32 clipped_z1;
    sint32 clipped_z2;
    sint32 clipped_z3;
    sint32 clipped_x0;
    sint32 clipped_x1;
    sint32 clipped_x2;
    sint32 clipped_x3;
    sint32 delta_x;
    sint32 delta_z;
    sint32 target_cell_x;
    sint32 initial_cell_x;
    sint32 remainder_x;
    sint32 remainder_z;
    uint32 dimension_grid;
    sint32 target_cell_z;
    sint32 remaining_x;
    sint32 grid_columns;
    sint32 remaining_z;
    sint32 grid_rows;
    sint32 cross_x;
    sint32 division_size;
    sint32 crossed_x;
    sint32 cross_z;
    sint32 step_delta_z;
    sint32 target_column;
    sint32 step_delta_x;
    sint32 initial_error;
    sint32 walk_error;
    sint32 error;
    sint32 out_of_grid;
    sint32 step_x;
    sint32 step_z;
    uint32 grid;
    sint32 grid_word_offset;
    sint32 saved_delta_z;
    sint32 call_delta_z;
    sint32 saved_target_column;
    sint32 call_target_column;
    sint32 saved_delta_x;
    sint32 call_delta_x;
    uint32 collision_mode = r_u32(0x800FF974);
    w_u32(0x800FF988, 0);
    w_u32(0x800FF984, -1);
    if (collision_mode)
        w_u32(0x800FF988, 0x400000);
    if (!r_u32(0x800FF970))
        w_u32(0x800FF988, (r_u32(0x800FF988) ^ (0x200000)));
    if (r_u32(0x800FF96C))
        w_u32(0x800FF984, -1048577);
    if (r_u32(0x800FF978))
        w_u32(0x800FF984, (r_u32(0x800FF984) ^ (0x20000)));
    if (include_objects)
        VisitCollisionObjects(r_u32(0x800FF5DC), record);
    grid = 0x800EDA30;
    grid_word_offset = 0;
    do
    {
        next_grid = (grid + (408) * 4);
        if (!(((sint32)(r_u32(grid)))))
            goto LABEL_106;
        start_x = ((sint32)(ReadCollisionRayWord(record, 0)));
        end_x = ((sint32)(ReadCollisionRayWord(record, 12)));
        start_z = ((sint32)(ReadCollisionRayWord(record, 8)));
        min_x = ((sint32)(r_u32((grid + (1) * 4))));
        end_z = ((sint32)(ReadCollisionRayWord(record, 20)));
        grid_cursor = grid;
        max_x = ((sint32)(r_u32((grid + (3) * 4))));
        min_z = ((sint32)(r_u32((grid + (2) * 4))));
        max_z = ((sint32)(r_u32((grid + (4) * 4))));
        cell_size = ((sint32)(r_u32((grid + (5) * 4))));
        if ((start_x < min_x) && (end_x < min_x))
            goto LABEL_105;
        if (((((max_x < start_x) && (max_x < end_x)) || ((start_z < min_z) && (end_z < min_z))) || ((max_z < start_z) && (max_z < end_z))))
            goto LABEL_104;
        if (((start_x == end_x) && (start_z == end_z)))
        {
            cell_x = DivideCollisionWord((uint32)start_x - (uint32)min_x, (uint32)cell_size);
            grid_columns = (sint16)r_u16(grid + 28);
            initial_cell_z = DivideCollisionWord((uint32)start_z - (uint32)min_z, (uint32)cell_size);
            if (cell_x == grid_columns)
                cell_x = (sint32)((uint32)cell_x - 1);
            cell_index_z = initial_cell_z;
            if ((initial_cell_z != ((sint16)(r_u16((((uint32)(grid)) + (15) * 2))))))
                goto LABEL_103;
            cell_z = ((sint32)((uint32)initial_cell_z - 1));
        }
        else
        {
            if ((start_x < min_x))
            {
                if ((start_z >= end_z))
                    clipped_z0 = (end_z + MultiplyDivideWord(((sint32)((uint32)end_x - (uint32)min_x)), ((sint32)((uint32)start_z - (uint32)end_z)), ((sint32)((uint32)end_x - (uint32)start_x))));
                else
                    clipped_z0 = (start_z + MultiplyDivideWord(((sint32)((uint32)min_x - (uint32)start_x)), ((sint32)((uint32)end_z - (uint32)start_z)), ((sint32)((uint32)end_x - (uint32)start_x))));
                start_z = clipped_z0;
                start_x = min_x;
            }
            if ((end_x < min_x))
            {
                if ((end_z >= start_z))
                    clipped_z1 = (start_z + MultiplyDivideWord(((sint32)((uint32)start_x - (uint32)min_x)), ((sint32)((uint32)end_z - (uint32)start_z)), ((sint32)((uint32)start_x - (uint32)end_x))));
                else
                    clipped_z1 = (end_z + MultiplyDivideWord(((sint32)((uint32)min_x - (uint32)end_x)), ((sint32)((uint32)start_z - (uint32)end_z)), ((sint32)((uint32)start_x - (uint32)end_x))));
                end_z = clipped_z1;
                end_x = min_x;
            }
            if ((max_x < start_x))
            {
                if ((end_z >= start_z))
                    clipped_z2 = (start_z + MultiplyDivideWord(((sint32)((uint32)start_x - (uint32)max_x)), ((sint32)((uint32)end_z - (uint32)start_z)), ((sint32)((uint32)start_x - (uint32)end_x))));
                else
                    clipped_z2 = (end_z + MultiplyDivideWord(((sint32)((uint32)max_x - (uint32)end_x)), ((sint32)((uint32)start_z - (uint32)end_z)), ((sint32)((uint32)start_x - (uint32)end_x))));
                start_z = clipped_z2;
                start_x = max_x;
            }
            if ((max_x < end_x))
            {
                if ((start_z >= end_z))
                    clipped_z3 = (end_z + MultiplyDivideWord(((sint32)((uint32)end_x - (uint32)max_x)), ((sint32)((uint32)start_z - (uint32)end_z)), ((sint32)((uint32)end_x - (uint32)start_x))));
                else
                    clipped_z3 = (start_z + MultiplyDivideWord(((sint32)((uint32)max_x - (uint32)start_x)), ((sint32)((uint32)end_z - (uint32)start_z)), ((sint32)((uint32)end_x - (uint32)start_x))));
                end_z = clipped_z3;
                end_x = max_x;
            }
            if ((start_z < min_z))
            {
                if ((start_x >= end_x))
                    clipped_x0 = (end_x + MultiplyDivideWord(((sint32)((uint32)end_z - (uint32)min_z)), ((sint32)((uint32)start_x - (uint32)end_x)), ((sint32)((uint32)end_z - (uint32)start_z))));
                else
                    clipped_x0 = (start_x + MultiplyDivideWord(((sint32)((uint32)min_z - (uint32)start_z)), ((sint32)((uint32)end_x - (uint32)start_x)), ((sint32)((uint32)end_z - (uint32)start_z))));
                start_x = clipped_x0;
                start_z = min_z;
            }
            if ((end_z < min_z))
            {
                if ((end_x >= start_x))
                    clipped_x1 = (start_x + MultiplyDivideWord(((sint32)((uint32)start_z - (uint32)min_z)), ((sint32)((uint32)end_x - (uint32)start_x)), ((sint32)((uint32)start_z - (uint32)end_z))));
                else
                    clipped_x1 = (end_x + MultiplyDivideWord(((sint32)((uint32)min_z - (uint32)end_z)), ((sint32)((uint32)start_x - (uint32)end_x)), ((sint32)((uint32)start_z - (uint32)end_z))));
                end_x = clipped_x1;
                end_z = min_z;
            }
            if ((max_z < start_z))
            {
                if ((end_x >= start_x))
                    clipped_x2 = (start_x + MultiplyDivideWord(((sint32)((uint32)start_z - (uint32)max_z)), ((sint32)((uint32)end_x - (uint32)start_x)), ((sint32)((uint32)start_z - (uint32)end_z))));
                else
                    clipped_x2 = (end_x + MultiplyDivideWord(((sint32)((uint32)max_z - (uint32)end_z)), ((sint32)((uint32)start_x - (uint32)end_x)), ((sint32)((uint32)start_z - (uint32)end_z))));
                start_x = clipped_x2;
                start_z = max_z;
            }
            if ((max_z < end_z))
            {
                if ((start_x >= end_x))
                    clipped_x3 = (end_x + MultiplyDivideWord(((sint32)((uint32)end_z - (uint32)max_z)), ((sint32)((uint32)start_x - (uint32)end_x)), ((sint32)((uint32)end_z - (uint32)start_z))));
                else
                    clipped_x3 = (start_x + MultiplyDivideWord(((sint32)((uint32)max_z - (uint32)start_z)), ((sint32)((uint32)end_x - (uint32)start_x)), ((sint32)((uint32)end_z - (uint32)start_z))));
                end_x = clipped_x3;
                end_z = max_z;
            }
            delta_x = ((sint32)((uint32)end_x - (uint32)start_x));
            if ((((sint32)((uint32)end_x - (uint32)start_x)) >= 0))
            {
                step_x = 1;
            }
            else
            {
                step_x = -1;
                delta_x = ((sint32)((uint32)start_x - (uint32)end_x));
            }
            delta_z = ((sint32)((uint32)end_z - (uint32)start_z));
            if ((((sint32)((uint32)end_z - (uint32)start_z)) >= 0))
            {
                step_z = 1;
            }
            else
            {
                step_z = -1;
                delta_z = ((sint32)((uint32)start_z - (uint32)end_z));
            }
            target_cell_x = DivideCollisionWord((uint32)end_x - (uint32)min_x, (uint32)cell_size);
            initial_cell_x = DivideCollisionWord((uint32)start_x - (uint32)min_x, (uint32)cell_size);
            remainder_x = RemainderCollisionWord((uint32)start_x - (uint32)min_x, (uint32)cell_size);
            cell_z = DivideCollisionWord((uint32)start_z - (uint32)min_z, (uint32)cell_size);
            remainder_z = RemainderCollisionWord((uint32)start_z - (uint32)min_z, (uint32)cell_size);
            dimension_grid = grid;
            target_cell_z = DivideCollisionWord((uint32)end_z - (uint32)min_z, (uint32)cell_size);
            remaining_x = remainder_x;
            grid_columns = ((sint16)(r_u16((((uint32)(grid)) + (14) * 2))));
            remaining_z = remainder_z;
            if ((initial_cell_x == grid_columns))
            {
                initial_cell_x = (sint32)((uint32)initial_cell_x - 1);
                remaining_x = (sint32)((uint32)remaining_x + (uint32)cell_size);
                dimension_grid = grid;
            }
            grid_rows = ((sint16)(r_u16((((uint32)(dimension_grid)) + (15) * 2))));
            if ((cell_z == grid_rows))
            {
                cell_z = (sint32)((uint32)cell_z - 1);
                remaining_z = ((sint32)((uint32)remainder_z + (uint32)cell_size));
            }
            if ((target_cell_x == grid_columns))
                target_cell_x = (sint32)((uint32)target_cell_x - 1);
            if ((target_cell_z == grid_rows))
                target_cell_z = (sint32)((uint32)target_cell_z - 1);
            saved_delta_z = delta_z;
            saved_target_column = target_cell_x;
            saved_delta_x = delta_x;
            cross_x = MultiplyDivideWord(delta_x, remaining_z, cell_size);
            division_size = cell_size;
            crossed_x = cross_x;
            cross_z = MultiplyDivideWord(saved_delta_z, remaining_x, division_size);
            step_delta_z = saved_delta_z;
            target_column = saved_target_column;
            step_delta_x = saved_delta_x;
            cell_x = initial_cell_x;
            if ((start_x >= end_x))
                initial_error = (sint32)(0 - (uint32)cross_z);
            else
                initial_error = ((sint32)((uint32)cross_z - (uint32)saved_delta_z));
            if ((start_z >= end_z))
                walk_error = ((sint32)((uint32)initial_error + (uint32)crossed_x));
            else
                walk_error = (sint32)((uint32)initial_error + (uint32)saved_delta_x - (uint32)crossed_x);
            error = walk_error;
            while (((cell_x != target_column) || (cell_z != target_cell_z)))
            {
                if ((cell_x < 0))
                    goto LABEL_104;
                grid_cursor = grid;
                if (((cell_x >= ((sint16)(r_u16((((uint32)(grid)) + (14) * 2))))) || (cell_z < 0)))
                    break;
                if ((cell_z >= ((sint16)(r_u16((((uint32)(grid)) + (15) * 2))))))
                    goto LABEL_99;
                call_delta_z = step_delta_z;
                call_target_column = target_column;
                call_delta_x = step_delta_x;
                VisitCollisionCells(((uint32)(r_u32((0x800EDA30 + (((((20 * (uint32)cell_x) + 8) + (uint32)cell_z) + (uint32)grid_word_offset)) * 4)))), record);
                step_delta_z = call_delta_z;
                target_column = call_target_column;
                step_delta_x = call_delta_x;
                if ((error < 0))
                {
                    error = (sint32)((uint32)error + (uint32)call_delta_x);
                    cell_z = (sint32)((uint32)cell_z + (uint32)step_z);
                }
                else
                {
                    error = (sint32)((uint32)error - (uint32)call_delta_z);
                    cell_x = (sint32)((uint32)cell_x + (uint32)step_x);
                }
            }

            if ((cell_x < 0))
                goto LABEL_104;
            grid_cursor = grid;
        LABEL_99:
            if (((cell_x >= ((sint16)(r_u16((((uint32)(grid_cursor)) + (14) * 2))))) || (cell_z < 0)))
                goto LABEL_105;

            out_of_grid = (cell_z >= ((sint16)(r_u16((((uint32)(grid_cursor)) + (15) * 2)))));
            next_grid = (grid_cursor + (408) * 4);
            if (out_of_grid)
                goto LABEL_106;
        }
        cell_index_z = cell_z;
    LABEL_103:
        VisitCollisionCells(((uint32)(r_u32((0x800EDA30 + (((((20 * (uint32)cell_x) + 8) + (uint32)cell_index_z) + (uint32)grid_word_offset)) * 4)))), record);

    LABEL_104:
        grid_cursor = grid;

    LABEL_105:
        next_grid = (grid_cursor + (408) * 4);

    LABEL_106:
        grid = next_grid;

        grid_word_offset = (sint32)((uint32)grid_word_offset + 408);
    } while ((((sint32)(next_grid)) < ((sint32)(0x800EE6F0))));
    return FinishCollisionRay(record);
}

void LoadCollisionBounds(const sint32 bounds[6], sint32 low[3], sint32 high[3], uint32 *flip_bits)
{
    FUNCTION_MARKER(0x80084778, "SLUS_003.73");
    uint32 words[6];
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        words[axis] = ReadCollisionRayWord(bounds, axis * 4);
    for (axis = 0; axis < 3; ++axis)
        words[axis + 3] = ReadCollisionRayWord(bounds, 12 + axis * 4);
    for (axis = 0; axis < 3; ++axis)
    {
        low[axis] = (sint32)words[axis] >> 12;
        high[axis] = (sint32)words[axis + 3] >> 12;
    }
    *flip_bits = 0;
    NormalizeCollisionBounds(low, high, flip_bits);
}

void NormalizeCollisionBounds(sint32 low[3], sint32 high[3], uint32 *flip_bits)
{
    FUNCTION_MARKER(0x800847AC, "SLUS_003.73");
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 delta = (uint32)high[axis] - (uint32)low[axis];
        if ((sint32)delta < 0)
        {
            sint32 temporary = low[axis];
            low[axis] = high[axis];
            high[axis] = temporary;
            delta = (uint32)high[axis] - (uint32)low[axis];
            *flip_bits ^= 1 << axis;
        }
        xport_gte_write_control(axis * 2, delta);
    }
}

void PrepareCollisionBounds(uint32 geometry, uint32 mirror_flags, const xport_draft_bounds *reference, const uint32 translation[3], xport_draft_bounds *result)
{
    FUNCTION_MARKER(0x80084814, "SLUS_003.73");
    uint32 packed[3], minimum[3], maximum[3], offset[3], center[3];
    uint32 axis;
    // Snapshot the original register inputs before exposing the output
    for (axis = 0; axis < 3; ++axis)
    {
        offset[axis] = translation[axis];
        center[axis] = reference->minimum[axis] + reference->maximum[axis];
    }
    packed[0] = r_u32(geometry + 20);
    packed[1] = r_u32(geometry + 24);
    packed[2] = r_u32(geometry + 28);
    for (axis = 0; axis < 3; ++axis)
    {
        minimum[axis] = (uint32)(sint32)(sint16)packed[axis] + offset[axis] + 2;
        maximum[axis] = (uint32)(sint32)(sint16)(packed[axis] >> 16) + offset[axis] - 2;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        if (mirror_flags & (1 << axis))
        {
            uint32 mirrored_minimum = center[axis] - maximum[axis];
            maximum[axis] = center[axis] - minimum[axis];
            minimum[axis] = mirrored_minimum;
        }
    }
    for (axis = 0; axis < 3; ++axis)
    {
        result->minimum[axis] = minimum[axis];
        result->maximum[axis] = maximum[axis];
    }
}

uint32 IntersectCollisionBounds(const sint32 query_low[3], const sint32 query_high[3], const sint32 object_low[3], const sint32 object_high[3])
{
    FUNCTION_MARKER(0x800848D0, "SLUS_003.73");
    sint32 lower_delta[3], upper_delta[3], cross[3], first, second;
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        if ((sint32)((uint32)query_high[axis] - (uint32)object_high[axis]) < 0)
            return 0;
    for (axis = 0; axis < 3; ++axis)
    {
        lower_delta[axis] = (sint32)((uint32)object_low[axis] - (uint32)query_low[axis]);
        if (lower_delta[axis] < 0)
            return 0;
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, (uint32)lower_delta[axis]);
    xport_gte_execute(0x170000C);
    for (axis = 0; axis < 3; ++axis)
        upper_delta[axis] = (sint32)((uint32)object_high[axis] - (uint32)query_low[axis]);
    for (axis = 0; axis < 3; ++axis)
        cross[axis] = (sint32)xport_gte_read_data(25 + axis);
    if (cross[0] < 0)
    {
        if (cross[1] < 0)
            goto CheckX;
        xport_gte_write_data(9, (uint32)upper_delta[0]);
        xport_gte_write_data(10, (uint32)upper_delta[1]);
        xport_gte_write_data(11, (uint32)lower_delta[2]);
        xport_gte_execute(0x170000C);
        first = (sint32)xport_gte_read_data(9);
        second = (sint32)xport_gte_read_data(10);
        return first < 0 || second > 0 ? 0 : 1;
    }
    if (cross[2] < 0)
    {
        xport_gte_write_data(9, (uint32)upper_delta[0]);
        xport_gte_write_data(10, (uint32)lower_delta[1]);
        xport_gte_write_data(11, (uint32)upper_delta[2]);
        xport_gte_execute(0x170000C);
        first = (sint32)xport_gte_read_data(9);
        second = (sint32)xport_gte_read_data(11);
        return first > 0 || second < 0 ? 0 : 1;
    }
CheckX:
    xport_gte_write_data(9, (uint32)lower_delta[0]);
    xport_gte_write_data(10, (uint32)upper_delta[1]);
    xport_gte_write_data(11, (uint32)upper_delta[2]);
    xport_gte_execute(0x170000C);
    first = (sint32)xport_gte_read_data(10);
    second = (sint32)xport_gte_read_data(11);
    return first < 0 || second > 0 ? 0 : 1;
}

void MarkLinkedCollisionCandidates(uint32 object, const sint32 bounds[6], uint32 mark)
{
    FUNCTION_MARKER(0x800849FC, "SLUS_003.73");
    uint32 geometry, entry, object_flags, scale, axis, delta, flip_bits, index;
    uint32 translation[3] = {0, 0, 0};
    sint32 lower[3], upper[3], first[3], second[3];
    xport_draft_bounds reference, result;
    uint32 matrix[5];
    do
    {
        object_flags = r_u32(object);
        index = r_u16(object + 22);
        if (object_flags & 0x21)
            w_u16(object + 2, mark);
        else if ((object_flags >> 16) != mark)
        {
            entry = 0x800EAEF8 + 64 * r_u8(object + 27);
            geometry = r_u32(r_u32(entry + 16) + 4 * index);
            LoadCollisionBounds(bounds, lower, upper, &flip_bits);
            for (axis = 0; axis < 3; ++axis)
            {
                delta = (uint32)((sint32)r_u32(object + 4 + 4 * axis) >> 12);
                lower[axis] = (sint32)((uint32)lower[axis] - delta);
                upper[axis] = (sint32)((uint32)upper[axis] - delta);
            }
            delta = r_u32(object + 16);
            index = r_u16(object + 20);
            if (delta | index)
            {
                BuildModelRotationMatrix((const uint32 *)psx_addr(object + 16, 8), (uint16 *)psx_addr(0x1F800028, 18));
                TransposeRotationMatrix((uint16 *)psx_addr(0x1F800028, 18));
                for (axis = 0; axis < 5; ++axis)
                    matrix[axis] = r_u32(0x1F800028 + axis * 4);
                for (axis = 0; axis < 5; ++axis)
                    xport_gte_write_control(axis, matrix[axis]);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32)lower[axis]);
                xport_gte_execute(0x49E012);
                for (axis = 0; axis < 3; ++axis)
                    lower[axis] = (sint32)xport_gte_read_data(9 + axis);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32)upper[axis]);
                xport_gte_execute(0x49E012);
                for (axis = 0; axis < 3; ++axis)
                    upper[axis] = (sint32)xport_gte_read_data(9 + axis);
                NormalizeCollisionBounds(lower, upper, &flip_bits);
            }
            for (axis = 0; axis < 3; ++axis)
            {
                reference.minimum[axis] = (uint32)lower[axis];
                reference.maximum[axis] = (uint32)upper[axis];
            }
            PrepareCollisionBounds(geometry, flip_bits, &reference, translation, &result);
            for (axis = 0; axis < 3; ++axis)
            {
                first[axis] = (sint32)result.minimum[axis];
                second[axis] = (sint32)result.maximum[axis];
            }
            if (r_u32(object) & 0x200)
            {
                for (axis = 0; axis < 3; ++axis)
                {
                    scale = (uint32)(sint32)(sint16)r_u16(object + 36 + 2 * axis);
                    first[axis] = (sint32)((uint32)first[axis] * scale) >> 12;
                    second[axis] = (sint32)((uint32)second[axis] * scale) >> 12;
                }
            }
            if (!IntersectCollisionBounds(lower, upper, first, second))
                w_u16(object + 2, mark);
        }
        object = r_u32(object + 28);
    } while (object);
}

void MarkArrayCollisionCandidates(uint32 list, const sint32 bounds[6], uint32 mark)
{
    FUNCTION_MARKER(0x80084C50, "SLUS_003.73");
    sint32 lower[3], upper[3], first[3], second[3];
    uint32 object = r_u32(list), flip_bits, table, geometry, axis, index;
    uint32 translation[3];
    xport_draft_bounds reference, result;
    if (!object)
        return;
    LoadCollisionBounds(bounds, lower, upper, &flip_bits);
    table = r_u32(0x800EAEF8 + 64 * r_u8(object + 27) + 16);
    do
    {
        uint32 flags = r_u32(object);
        index = r_u16(object + 22);
        if (flags & 0x21)
            w_u16(object + 2, mark);
        else if ((flags >> 16) != mark)
        {
            geometry = r_u32(table + 4 * index);
            for (axis = 0; axis < 3; ++axis)
            {
                translation[axis] = (uint32)((sint32)r_u32(object + 4 + axis * 4) >> 12);
                reference.minimum[axis] = (uint32)lower[axis];
                reference.maximum[axis] = (uint32)upper[axis];
            }
            PrepareCollisionBounds(geometry, flip_bits, &reference, translation, &result);
            for (axis = 0; axis < 3; ++axis)
            {
                first[axis] = (sint32)result.minimum[axis];
                second[axis] = (sint32)result.maximum[axis];
            }
            if (!IntersectCollisionBounds(lower, upper, first, second))
                w_u16(object + 2, mark);
        }
        list += 4;
        object = r_u32(list);
    } while (object);
}

uint32 TransformCollisionVertices(uint32 geometry, uint32 output, uint32 depth_limit, const sint32 translation[3])
{
    FUNCTION_MARKER(0x80084D4C, "SLUS_003.73");
    uint32 translation_x = ReadCollisionRayWord(translation, 0);
    uint32 translation_y = ReadCollisionRayWord(translation, 4);
    uint32 translation_z = ReadCollisionRayWord(translation, 8);
    uint32 remaining = r_u32(geometry + 4), vertex = geometry + 32;
    uint32 packed = r_u32(vertex), zword = r_u32(vertex + 4);
    uint32 x = packed + translation_x;
    uint32 y = (uint32)((sint32)packed >> 16) + translation_y;
    uint32 z = zword + translation_z;
    uint32 result = 0x60F, mask;
    sint32 transformed_x, transformed_y, transformed_z;
    // TODO Original zero vertex count enters the loop and underflows
    do
    {
        xport_gte_write_data(9, x);
        xport_gte_write_data(10, y);
        xport_gte_write_data(11, z);
        vertex += 8;
        --remaining;
        xport_gte_execute(0x4D8012);
        // Prefetch the next vertex before storing the current result
        packed = r_u32(vertex);
        zword = r_u32(vertex + 4);
        x = packed + translation_x;
        y = (uint32)((sint32)packed >> 16) + translation_y;
        z = zword + translation_z;
        transformed_x = (sint32)xport_gte_read_data(25);
        transformed_y = (sint32)xport_gte_read_data(26);
        transformed_z = (sint32)xport_gte_read_data(27);
        mask = ((uint32)(transformed_x < 0) << 2) | ((uint32)(transformed_y < 0) << 1) | (uint32)(transformed_z < 0) | ((uint32)((sint32)depth_limit < transformed_z) << 3) | ((uint32)(transformed_x > 0) << 10) | ((uint32)(transformed_y > 0) << 9);
        w_u32(output, ((uint32)transformed_x & 0xFFFF) | ((uint32)transformed_y << 16));
        w_u32(output + 4, (uint32)transformed_z);
        output += 8;
        result &= mask;
        w_u16(output - 2, mask);
    } while (remaining);
    return result;
}

uint32 IntersectCollisionPolygons(uint32 geometry, uint32 output, uint8 *record, uint32 object)
{
    FUNCTION_MARKER(0x80084E24, "SLUS_003.73");
    uint32 required, rejected, count, vertex_count, normal_count;
    uint32 normals, polygon, header, flags, stride, length;
    uint32 vertices[4], packed[3], y[3], clip[4];
    uint32 cross0, cross1, cross2, normal, first_vertex;
    uint32 start, numerator, endpoint, denominator, distance;
    uint32 direction, trigger_enabled, trigger_index, axis;

    length = ReadCollisionRayWord(record, 68);
    xport_gte_write_control(11, 0);
    xport_gte_write_control(12, length);
    required = r_u32(0x800FF984);
    rejected = r_u32(0x800FF988);
    vertex_count = r_u32(geometry + 4);
    normal_count = r_u32(geometry + 8);
    count = r_u32(geometry + 12);
    normals = geometry + 32 + (vertex_count << 3);
    polygon = normals + (normal_count << 3);
    // TODO Original zero polygon count enters the loop and underflows
    do
    {
        flags = r_u32(polygon + 16);
        header = r_u32(polygon);
        if (!(flags & rejected) && (flags | required) == 0xFFFFFFFF && ((flags >> 16) & 3) != 1)
        {
            packed[0] = r_u32(polygon + 4);
            packed[1] = r_u32(polygon + 8);
            vertices[0] = output + (packed[0] & 0xFFFF);
            clip[0] = r_u16(vertices[0] + 6);
            vertices[1] = output + (packed[0] >> 16);
            clip[1] = r_u16(vertices[1] + 6);
            vertices[2] = output + (packed[1] & 0xFFFF);
            clip[2] = r_u16(vertices[2] + 6);
            vertices[3] = output + (packed[1] >> 16);
            clip[3] = r_u16(vertices[3] + 6);
            if (!(clip[0] & clip[1] & clip[2] & clip[3] & 0x60F))
            {
                for (axis = 0; axis < 3; ++axis)
                    packed[axis] = r_u32(vertices[axis]);
                for (axis = 0; axis < 3; ++axis)
                    y[axis] = (uint32)((sint32)packed[axis] >> 16);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(axis * 2, packed[axis]);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, y[axis]);
                xport_gte_execute(0x170000C);
                cross0 = xport_gte_read_data(25);
                cross1 = xport_gte_read_data(26);
                cross2 = xport_gte_read_data(27);
                if ((sint32)cross0 < 0)
                {
                    if (header & 0x10)
                        goto next_polygon;
                    packed[0] = r_u32(vertices[3]);
                    xport_gte_write_data(10, y[1]);
                    xport_gte_write_data(11, y[2]);
                    xport_gte_write_control(0, packed[0]);
                    xport_gte_write_data(9, (uint32)((sint32)packed[0] >> 16));
                    xport_gte_execute(0x170000C);
                    cross1 = xport_gte_read_data(26);
                    cross2 = xport_gte_read_data(27);
                    cross1 = 0 - cross1;
                    cross2 = 0 - cross2;
                }
                if ((sint32)(cross1 | cross2) >= 0)
                {
                    normal = normals + (flags & 0xFFFF);
                    xport_gte_write_data(0, r_u32(normal));
                    xport_gte_write_data(1, r_u32(normal + 4));
                    first_vertex = output + (r_u32(polygon + 4) & 0xFFFF);
                    start = r_u32(first_vertex);
                    endpoint = r_u16(first_vertex + 4);
                    xport_gte_execute(0x4C6012);
                    xport_gte_write_control(8, start);
                    xport_gte_write_control(9, endpoint);
                    denominator = xport_gte_read_data(11);
                    xport_gte_execute(0x43E012);
                    numerator = xport_gte_read_data(25);
                    endpoint = xport_gte_read_data(27);
                    if ((sint32)numerator > 0)
                        goto next_polygon;
                    distance = (uint32)DivideCollisionWord(numerator, denominator);
                    if ((sint32)numerator < (sint32)endpoint)
                        goto next_polygon;
                    trigger_enabled = ReadCollisionRayByte(record, 136);
                    if (flags & 0x20000)
                    {
                        trigger_index = r_u16(object + 22);
                        if (trigger_enabled)
                            w_u32(0x800FFA88, trigger_index);
                    }
                    else if ((sint32)(ReadCollisionRayWord(record, 64) - distance) > 0)
                    {
                        WriteCollisionRayWord(record, 104, object);
                        WriteCollisionRayWord(record, 64, distance);
                        WriteCollisionRayWord(record, 128, polygon);
                        WriteCollisionRayWord(record, 132, endpoint);
                        for (axis = 0; axis < 3; ++axis)
                            xport_gte_write_data(25 + axis, ReadCollisionRayWord(record, axis * 4));
                        direction = ReadCollisionRayWord(record, 84);
                        endpoint = ReadCollisionRayWord(record, 88);
                        xport_gte_write_data(9, direction);
                        xport_gte_write_data(10, direction >> 16);
                        xport_gte_write_data(11, endpoint);
                        xport_gte_write_data(8, distance);
                        xport_draft_gte_execute(0x1A0003E);
                        for (axis = 0; axis < 3; ++axis)
                            WriteCollisionRayWord(record, 108 + axis * 4, xport_gte_read_data(25 + axis));
                        normal = normals + (flags & 0xFFFF);
                        w_u32(0x800ED61C, normal);
                        w_u32(0x800ED618, polygon);
                    }
                }
            }
        }
    next_polygon:
        stride = header >> 16;
        polygon += stride;
        --count;
    } while (count);
    return stride;
}

extern "C" uint32 sub_80067508(uint32 start, uint32 end, uint32 list)
{
    FUNCTION_MARKER(0x80067508, "SLUS_003.73");
    return TraceSegmentCollision((const uint32 *)psx_addr(start, 12), (const uint32 *)psx_addr(end, 12), list);
}

extern "C" uint32 apocalypse_collision_sphere_segment_native(const void *start, const void *end, void *output, uint32 list, uint32 excluded)
{
    return IntersectObjectSpheres((const uint32 *)start, (const uint32 *)end, (uint32 *)output, list, excluded);
}

extern "C" uint32 apocalypse_collision_sphere_segment_native_end(uint32 start, const void *end, void *output, uint32 list, uint32 excluded)
{
    return IntersectObjectSpheres((const uint32 *)psx_addr(start, 12), (const uint32 *)end, (uint32 *)output, list, excluded);
}

extern "C" uint32 apocalypse_collision_sphere_segment(uint32 start, uint32 end, void *output, uint32 list, uint32 excluded)
{
    return IntersectObjectSpheres((const uint32 *)psx_addr(start, 12), (const uint32 *)psx_addr(end, 12), (uint32 *)output, list, excluded);
}

extern "C" uint32 sub_8007C398(uint32 start, uint32 end, uint32 output, uint32 list, uint32 excluded)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    return IntersectObjectSpheres((const uint32 *)psx_addr(start, 12), (const uint32 *)psx_addr(end, 12), (uint32 *)psx_addr(output, 12), list, excluded);
}

extern "C" uint32 apocalypse_collision_init(void *record)
{
    return InitializeCollisionRay((uint8 *)record);
}

extern "C" uint32 sub_8007BB24(uint32 record)
{
    FUNCTION_MARKER(0x8007BB24, "SLUS_003.73");
    return InitializeCollisionRay((uint8 *)psx_addr(record, 140));
}

extern "C" uint32 apocalypse_collision_query(void *record, uint32 include_objects)
{
    return QueryCollisionRay((uint8 *)record, include_objects);
}

extern "C" uint32 sub_8007DD04(uint32 record, uint32 include_objects)
{
    FUNCTION_MARKER(0x8007DD04, "SLUS_003.73");
    return QueryCollisionRay((uint8 *)psx_addr(record, 140), include_objects);
}

extern "C" uint32 sub_8007BAB0(void)
{
    FUNCTION_MARKER(0x8007BAB0, "SLUS_003.73");
    return AdvanceCollisionGeneration();
}

extern "C" uint32 apocalypse_collision_finish(void *record) { return FinishCollisionRay((uint8 *)record); }

extern "C" uint32 sub_8007BFD4(uint32 record)
{
    FUNCTION_MARKER(0x8007BFD4, "SLUS_003.73");
    return FinishCollisionRay((uint8 *)psx_addr(record, 140));
}

extern "C" uint32 apocalypse_collision_transform(uint32 geometry, uint32 output, uint32 depth_limit, const sint32 translation[3]) { return TransformCollisionVertices(geometry, output, depth_limit, translation); }

extern "C" uint32 sub_80084D4C(uint32 geometry, uint32 output, uint32 depth_limit, uint32 translation)
{
    FUNCTION_MARKER(0x80084D4C, "SLUS_003.73");
    return TransformCollisionVertices(geometry, output, depth_limit, (const sint32 *)psx_addr(translation, 12));
}

extern "C" uint32 apocalypse_collision_polygons(uint32 geometry, uint32 output, void *record, uint32 object) { return IntersectCollisionPolygons(geometry, output, (uint8 *)record, object); }

extern "C" uint32 sub_80084E24(uint32 geometry, uint32 output, uint32 record, uint32 object)
{
    FUNCTION_MARKER(0x80084E24, "SLUS_003.73");
    return IntersectCollisionPolygons(geometry, output, (uint8 *)psx_addr(record, 140), object);
}

extern "C" void apocalypse_collision_objects(uint32 list, void *record) { VisitCollisionObjects(list, (uint8 *)record); }

extern "C" void sub_8007BEA0(uint32 list, uint32 record)
{
    FUNCTION_MARKER(0x8007BEA0, "SLUS_003.73");
    VisitCollisionObjects(list, (uint8 *)psx_addr(record, 140));
}

extern "C" void apocalypse_collision_cells(uint32 list, void *record) { VisitCollisionCells(list, (uint8 *)record); }

extern "C" void sub_8007BF2C(uint32 list, uint32 record)
{
    FUNCTION_MARKER(0x8007BF2C, "SLUS_003.73");
    VisitCollisionCells(list, (uint8 *)psx_addr(record, 140));
}

extern "C" uint32 apocalypse_collision_object(uint32 object, void *record) { return IntersectCollisionObject(object, (uint8 *)record); }

extern "C" uint32 sub_8007C06C(uint32 object, uint32 record)
{
    FUNCTION_MARKER(0x8007C06C, "SLUS_003.73");
    return IntersectCollisionObject(object, (uint8 *)psx_addr(record, 140));
}

extern "C" uint32 sub_800848D0(const sint32 query_low[3], const sint32 query_high[3], const sint32 object_low[3], const sint32 object_high[3])
{
    FUNCTION_MARKER(0x800848D0, "SLUS_003.73");
    return IntersectCollisionBounds(query_low, query_high, object_low, object_high);
}

extern "C" void sub_80084814(uint32 geometry, uint32 mirror_flags, const xport_draft_bounds *reference, const uint32 translation[3], xport_draft_bounds *result)
{
    FUNCTION_MARKER(0x80084814, "SLUS_003.73");
    PrepareCollisionBounds(geometry, mirror_flags, reference, translation, result);
}

extern "C" void sub_800847AC(sint32 low[3], sint32 high[3], uint32 *flip_bits)
{
    FUNCTION_MARKER(0x800847AC, "SLUS_003.73");
    NormalizeCollisionBounds(low, high, flip_bits);
}

extern "C" void apocalypse_mark_array_collision_candidates(uint32 list, const sint32 bounds[6], uint32 mark) { MarkArrayCollisionCandidates(list, bounds, mark); }

extern "C" void sub_80084C50(uint32 list, uint32 unused, uint32 bounds, uint32 mark)
{
    FUNCTION_MARKER(0x80084C50, "SLUS_003.73");
    MarkArrayCollisionCandidates(list, (const sint32 *)psx_addr(bounds, 24), mark);
}

extern "C" void apocalypse_mark_linked_collision_candidates(uint32 list, const sint32 bounds[6], uint32 mark) { MarkLinkedCollisionCandidates(list, bounds, mark); }

extern "C" void sub_800849FC(uint32 list, uint32 unused, uint32 bounds, uint32 mark)
{
    FUNCTION_MARKER(0x800849FC, "SLUS_003.73");
    MarkLinkedCollisionCandidates(list, (const sint32 *)psx_addr(bounds, 24), mark);
}

extern "C" void sub_80084778(uint32 bounds, sint32 low[3], sint32 high[3], uint32 *flip_bits)
{
    FUNCTION_MARKER(0x80084778, "SLUS_003.73");
    LoadCollisionBounds((const sint32 *)psx_addr(bounds, 24), low, high, flip_bits);
}

extern "C" uint32 apocalypse_ground_native(const void *position, uint32 below, uint32 above) { return FindGroundHeight((const uint32 *)position, below, above); }

extern "C" uint32 sub_80067A18(uint32 position, uint32 below, uint32 above)
{
    FUNCTION_MARKER(0x80067A18, "SLUS_003.73");
    return FindGroundHeight((const uint32 *)psx_addr(position, 12), below, above);
}

extern "C" uint32 apocalypse_visibility_native(const void *start, uint32 end) { return IsSegmentVisible((const uint32 *)start, (const uint32 *)psx_addr(end, 12)); }

extern "C" uint32 sub_800679A4(uint32 start, uint32 end)
{
    FUNCTION_MARKER(0x800679A4, "SLUS_003.73");
    return IsSegmentVisible((const uint32 *)psx_addr(start, 12), (const uint32 *)psx_addr(end, 12));
}

extern "C" uint32 sub_8004BA64(uint32 object, uint32 output, uint32 target, uint32 minimum, uint32 maximum, uint32 height, uint32 spread, uint32 ground, uint32 collision)
{
    FUNCTION_MARKER(0x8004BA64, "SLUS_003.73");
    return ChooseCandidatePosition(object, (uint32 *)psx_addr(output, 12), (const uint32 *)psx_addr(target, 12), minimum, maximum, height, spread, ground, collision);
}

extern "C" uint32 apocalypse_candidate_native(uint32 object, uint32 output[3], uint32 target, uint32 minimum, uint32 maximum, uint32 height, uint32 spread, uint32 ground, uint32 collision) { return ChooseCandidatePosition(object, output, (const uint32 *)psx_addr(target, 12), minimum, maximum, height, spread, ground, collision); }

extern "C" uint32 apocalypse_candidate_vectors_native(uint32 object, uint32 output[3], const uint32 target[3], uint32 minimum, uint32 maximum, uint32 height, uint32 spread, uint32 ground, uint32 collision) { return ChooseCandidatePosition(object, output, target, minimum, maximum, height, spread, ground, collision); }
