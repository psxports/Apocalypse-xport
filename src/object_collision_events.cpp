#include "visual_object.h"
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

static CollisionRay shared_collision_ray;
static SVECTOR collision_normal;

CollisionRay &GetSharedCollisionRay()
{
    return shared_collision_ray;
}

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
    uint32 trig_angle;
    FUNCTION_MARKER(0x8004BA64, "SLUS_003.73");
    uint32 angle, radius, delta, sign, absolute, left, right, sine, offset;
    sint32 height;
    CollisionRay collision;
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
    trig_angle = angle & 4095;
    sine = (uint32)rsin(trig_angle);
    left = ReadCollisionRayWord(target, 0);
    WriteCollisionRayWord(output, 0, left + radius * sine);
    left = ReadCollisionRayWord(target, 4);
    WriteCollisionRayWord(output, 4, left);
    sine = (uint32)rcos(trig_angle);
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
            collision.pos[0][0] = r_u32(object + 4);
            collision.pos[0][1] = r_u32(object + 8);
            collision.pos[0][2] = r_u32(object + 12);
            if (!pass)
            {
                offset = (uint32)(sint32)(sint16)r_u16(object + 458) << 12;
                collision.pos[0][1] -= offset;
            }
            collision.pos[1][0] = ReadCollisionRayWord(output, 0);
            collision.pos[1][1] = ReadCollisionRayWord(output, 4);
            collision.pos[1][2] = ReadCollisionRayWord(output, 8);
            if (!pass)
            {
                offset = (uint32)(sint32)(sint16)r_u16(object + 458) << 12;
                collision.pos[1][1] -= offset;
            }
            InitializeCollisionRay(collision);
            collision.triggers = 0;
            w_u32(0x800FF970, 1);
            QueryCollisionRay(collision, 1);
            uint32 hit = collision.hit.GetObject();
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

ModelInstance *TraceSegmentCollision(const uint32 *start, const uint32 *end, ModelInstance *list)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    FUNCTION_MARKER(0x80067508, "SLUS_003.73");
    uint32 hit[4];
    ModelInstance *result = IntersectObjectSpheres(start, end, hit, list, 0);
    if (!result)
    {
        for (uint32 axis = 0; axis < 3; ++axis)
            shared_ray.pos[0][axis] = ReadSegmentPoint(start, axis);
        for (uint32 axis = 0; axis < 3; ++axis)
            shared_ray.pos[1][axis] = ReadSegmentPoint(end, axis);
        InitializeCollisionRay(shared_ray);
        ModelInstance *objects = visual_lists.solids;
        w_u32(0x800FF974, 1);
        VisitCollisionObjects(objects, shared_ray);
        result = shared_ray.hit.GetObject();
        w_u32(0x800FF974, 0);
    }
    return result;
}

uint32 IsSegmentVisible(const uint32 *start, const uint32 *end)
{
    FUNCTION_MARKER(0x800679A4, "SLUS_003.73");
    CollisionRay collision;
    collision.pos[0][0] = ReadCollisionRayWord(start, 0);
    collision.pos[0][1] = ReadCollisionRayWord(start, 4);
    collision.pos[0][2] = ReadCollisionRayWord(start, 8);
    collision.pos[1][0] = ReadCollisionRayWord(end, 0);
    collision.pos[1][1] = ReadCollisionRayWord(end, 4);
    collision.pos[1][2] = ReadCollisionRayWord(end, 8);
    InitializeCollisionRay(collision);
    QueryCollisionRay(collision, 1);
    return collision.hit.GetObject() == 0;
}

uint32 FindGroundHeight(const uint32 *position, uint32 below, uint32 above)
{
    FUNCTION_MARKER(0x80067A18, "SLUS_003.73");
    CollisionRay collision;
    collision.pos[0][0] = ReadCollisionRayWord(position, 0);
    collision.pos[0][1] = ReadCollisionRayWord(position, 4) - (below << 12);
    collision.pos[0][2] = ReadCollisionRayWord(position, 8);
    collision.pos[1][0] = ReadCollisionRayWord(position, 0);
    collision.pos[1][1] = ReadCollisionRayWord(position, 4) + (above << 12);
    collision.pos[1][2] = ReadCollisionRayWord(position, 8);
    InitializeCollisionRay(collision);
    collision.triggers = 0;
    QueryCollisionRay(collision, 1);
    if (collision.hit.GetObject())
    {
        if ((sint16)collision.n.vy < -3071)
            return collision.hit_pos[1];
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
        ModelInstance *scene_object = GetModelSceneObjects();
        w_u16(0x800FF968, 1);
        while (scene_object)
        {
            scene_object->generation = 0;
            scene_object = scene_object->next;
        }
        ModelInstance *node = visual_lists.solids;
        while (node)
        {
            node->generation = 0;
            node = node->next;
        }
    }
    return 1;
}

uint32 InitializeCollisionRay(CollisionRay &record)
{
    FUNCTION_MARKER(0x8007BB24, "SLUS_003.73");
    uint32 first, second, delta, horizontal, horizontal_root, full, full_root;
    uint32 normalization, shift, full_shift, sine, cosine, length, generation;
    uint32 axis;
    sint32 dx, dy, dz;
    const uint32 *start = record.pos[0], *end = record.pos[1];
    MATRIX *matrix = &record.basis;

    record.distance = (0x7FFFFFFF);
    second = record.pos[1][0];
    first = record.pos[0][0];
    record.hit.SetObject((0));
    record.endpoint = (0xFFFFFFFF);
    record.triggers = (uint8)(0);
    record.upward = (uint8)(0);
    delta = (uint32)((sint32)(second - first) >> 12);
    w_u16(0x800FFA3C, (uint16)delta);
    dx = (sint16)delta;
    horizontal = (uint32)(dx * dx);
    second = record.pos[1][1];
    first = record.pos[0][1];
    delta = (uint32)((sint32)(second - first) >> 12);
    w_u16(0x800FFA3E, (uint16)delta);
    dy = (sint16)delta;
    second = record.pos[1][2];
    first = record.pos[0][2];
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
        record.len = (length);
        dy = (sint16)r_u16(0x800FFA3E);
        sine = (uint32)DivideCollisionWord((uint32)dy << ((full_shift + 12) & 31), full_root);
        w_u16(0x800FFA2C, (uint16)cosine);
        w_u16(0x800FFA2E, (uint16)sine);
        record.basis.m[0][0] = (sint16)(4096);
        record.basis.m[0][1] = (sint16)(0);
        record.basis.m[0][2] = (sint16)(0);
        record.basis.m[1][0] = (sint16)(0);
        record.basis.m[1][1] = (sint16)((uint16)cosine);
        record.basis.m[1][2] = (sint16)((uint16)(0 - sine));
        record.basis.m[2][0] = (sint16)(0);
        record.basis.m[2][1] = (sint16)((uint16)sine);
        record.basis.m[2][2] = (sint16)((uint16)cosine);
        MulMatrix0(matrix, (MATRIX *)psx_addr(0x800ED740, sizeof(MATRIX)), matrix);
    }
    else
    {
        sine = dy < 0 ? 0xFFFFF000 : 4096;
        w_u16(0x800FFA2E, (uint16)sine);
        record.len = (dy < 0 ? 0 - (uint32)dy : (uint32)dy);
        if (dy >= 0)
            record.upward = (uint8)(1);
        record.basis.m[0][0] = (sint16)(4096);
        record.basis.m[0][1] = (sint16)(0);
        record.basis.m[0][2] = (sint16)(0);
        record.basis.m[1][0] = (sint16)(0);
        record.basis.m[1][1] = (sint16)(0);
        sine = r_u16(0x800FFA2E);
        record.basis.m[2][0] = (sint16)(0);
        record.basis.m[2][2] = (sint16)(0);
        record.basis.m[1][2] = (sint16)((uint16)(0 - sine));
        record.basis.m[2][1] = (sint16)((uint16)sine);
    }
    SetRotMatrix(matrix);
    first = record.pos[0][0];
    second = record.pos[1][0];
    if ((sint32)first < (sint32)second)
        record.min[0] = (first);
    else
    {
        first = record.pos[0][0];
        record.min[0] = (second);
        second = first;
    }
    record.max[0] = (second);
    for (axis = 1; axis < 3; ++axis)
    {
        first = ReadCollisionRayWord(start, axis * 4);
        second = ReadCollisionRayWord(end, axis * 4);
        if ((sint32)first < (sint32)second)
        {
            record.min[axis] = (first);
            second = ReadCollisionRayWord(end, axis * 4);
        }
        else
        {
            record.min[axis] = (second);
            second = ReadCollisionRayWord(start, axis * 4);
        }
        record.max[axis] = (second);
    }
    AdvanceCollisionGeneration();
    generation = r_u16(0x800FF968);
    record.generation = (uint16)((uint16)generation);
    return generation;
}

void VisitCollisionObjects(ModelInstance *list, CollisionRay &record)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007BEA0, "SLUS_003.73");
    uint32 generation;
    if (!list)
        return;
    if (!record.len)
        return;
    SetRotMatrix(&record.basis);
    generation = record.generation;
    MarkLinkedCollisionCandidates(list, (const sint32 *)record.pos, generation);
    do
    {
        generation = record.generation;
        if (list->generation != generation)
        {
            list->generation = generation;
            (void)IntersectCollisionObject(list, record);
        }
        list = list->next;
    } while (list);
}

void VisitCollisionCells(const ModelGridCell &cell, CollisionRay &record)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007BF2C, "SLUS_003.73");
    ModelInstance *object;
    uint32 generation;
    if (!cell.present)
        return;
    if (!record.len)
        return;
    SetRotMatrix(&record.basis);
    generation = record.generation;
    MarkArrayCollisionCandidates(cell.objects, cell.count, (const sint32 *)record.pos, generation);
    uint32 count = cell.count;
    ModelInstance *const *objects = cell.objects;
    if (!count)
        return;
    do
    {
        object = *objects;
        generation = record.generation;
        if (object->active && object->generation != generation)
        {
            object->generation = generation;
            object = *objects;
            (void)IntersectCollisionObject(object, record);
        }
        ++objects;
        --count;
    } while (count);
}

uint32 FinishCollisionRay(CollisionRay &record)
{
    FUNCTION_MARKER(0x8007BFD4, "SLUS_003.73");
    ModelInstance *object = record.hit.GetObject();
    if (!object)
        return 0;
    BuildModelRotationMatrix(object->angles, (uint16 *)psx_addr(0x800ED720, 18));
    SetRotMatrix((MATRIX *)psx_addr(0x800ED720, sizeof(MATRIX)));
    const SVECTOR *normal = &collision_normal;
    xport_gte_write_data(0, xport_load_le32(normal));
    xport_gte_write_data(1, xport_load_le32((const uint8 *)normal + 4));
    xport_gte_execute(0x486012);
    uint32 normal_x = xport_gte_read_data(9);
    uint32 normal_y = xport_gte_read_data(10);
    uint32 normal_z = xport_gte_read_data(11);
    record.n.vx = (sint16)((uint16)normal_x);
    record.n.vy = (sint16)((uint16)normal_y);
    record.n.vz = (sint16)((uint16)normal_z);
    return 1;
}

uint32 IntersectCollisionObject(ModelInstance *object, CollisionRay &record)
{
    FUNCTION_MARKER(0x8007C06C, "SLUS_003.73");
    if (!object->active)
        return 0;
    uint32 result, model, count, output, index, trigger, target, axis;
    uint32 first, second, height_mode;
    sint32 translation[3];
    result = object->flags & 0x21;
    if (result)
        return result;
    SetColorMatrix(&record.basis);
    model = object->bank;
    index = object->model_idx;
    const ModelGeometry &geometry = GetModelGeometry(model, index);
    count = geometry.vertex_count;
    output = count < 129 ? 0x1F800000 : r_u32(0x800FFAC0);
    if (!((uint32)object->angles[0] | ((uint32)object->angles[1] << 16)) && !object->angles[2] && !(object->flags & 0x200))
    {
        for (axis = 0; axis < 3; ++axis)
        {
            first = object->pos[axis];
            second = record.pos[0][axis];
            translation[axis] = (sint32)(first - second) >> 12;
        }
        xport_gte_write_control(5, 0);
        xport_gte_write_control(6, 0);
        xport_gte_write_control(7, 0);
    }
    else
    {
        height_mode = record.upward;
        first = object->pos[0];
        second = record.pos[0][0];
        w_u32(0x800ED734, (uint32)((sint32)(first - second) >> 12));
        if (height_mode)
        {
            first = record.pos[0][2];
            second = object->pos[2];
            w_u32(0x800ED738, (uint32)((sint32)(first - second) >> 12));
            first = object->pos[1];
            second = record.pos[0][1];
            w_u32(0x800ED73C, (uint32)((sint32)(first - second) >> 12));
        }
        else
        {
            first = object->pos[1];
            second = record.pos[0][1];
            w_u32(0x800ED738, (uint32)((sint32)(first - second) >> 12));
            first = object->pos[2];
            second = record.pos[0][2];
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
        if (((uint32)object->angles[0] | ((uint32)object->angles[1] << 16)) || object->angles[2])
        {
            BuildModelRotationMatrix(object->angles, (uint16 *)psx_addr(0x800ED720, 18));
            if (object->flags & 0x200)
                ScaleModelRotationMatrix(object->scale, (uint16 *)psx_addr(0x800ED720, 18));
        }
        else
        {
            InitializeModelIdentityMatrix((uint32 *)psx_addr(0x800ED720, 20));
            ScaleModelRotationMatrix(object->scale, (uint16 *)psx_addr(0x800ED720, 18));
        }
        TransformPackedMatrixForProjection(0x800ED720, 0x800ED720);
        SetColorMatrix((MATRIX *)psx_addr(0x800ED720, 32));
    }
    first = record.len;
    result = TransformCollisionVertices(geometry, output, first, translation);
    w_u16(0x800FFA32, result);
    result &= 0x60F;
    if (result)
        return result;
    w_u32(0x800FFA88, 0xFFFFFFFF);
    (void)IntersectCollisionPolygons(geometry, output, record, object);
    result = r_u32(0x800FFA88);
    if (result == 0xFFFFFFFF)
        return result;
    index = FindActiveModelBankSlot(object->bank);
    trigger = r_u32(0x800FFA88);
    target = ReadModelKey(GetActiveModelScriptKeys(index), trigger);
    SelectModelBankReplacement(index ^ 1);
    w_u32(0x800FF978, 0);
    return ActivateResourceScript(target);
}

ModelInstance *IntersectObjectSpheres(const uint32 *start, const uint32 *end, uint32 *output, ModelInstance *list, ModelInstance *excluded)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    uint32 difference[3], vector[3], shifted[3], shift = 12, axis;
    ModelInstance *closest = 0;
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
    for (ModelInstance *object = list; object; object = object->next)
    {
        VisualObject &visual = *static_cast<VisualObject *>(object->owner);
        const uint32 record = NativeObjectGuestAddress(&visual);
        if (visual.lifecycle_flags & 64)
            continue;
        if (object == excluded)
            continue;
        uint32 radius = (uint32)r_u16(record + 212) << 12;
        uint32 x = object->pos[0];
        uint32 bound = r_u32(0x800ED5CC);
        w_u32(0x800FF9F0, radius);
        if ((sint32)(x + radius) < (sint32)bound)
            continue;
        bound = r_u32(0x800ED5D8);
        if ((sint32)bound < (sint32)(x - radius))
            continue;
        uint32 y = object->pos[1];
        bound = r_u32(0x800ED5D0);
        if ((sint32)(y + radius) < (sint32)bound)
            continue;
        bound = r_u32(0x800ED5DC);
        if ((sint32)bound < (sint32)(y - radius))
            continue;
        uint32 z = object->pos[2];
        bound = r_u32(0x800ED5D4);
        if ((sint32)(z + radius) < (sint32)bound)
            continue;
        bound = r_u32(0x800ED5E0);
        if ((sint32)bound < (sint32)(z - radius))
            continue;
        SubtractVectors(difference, object->pos, start);
        ShiftVectorRight(vector, difference, &shift);
        for (axis = 0; axis < 3; ++axis)
            w_u32(0x800ED5F0 + axis * 4, vector[axis]);
        xport_gte_write_data(11, r_u32(0x800ED5F8));
        xport_gte_write_data(9, r_u32(0x800ED5F0));
        xport_gte_write_data(10, r_u32(0x800ED5F4));
        xport_gte_execute(0x178000C);
        xport_gte_execute(0xA00428);
        StoreCollisionSquareResult();
        radius = r_u16(record + 212);
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
                SubtractVectors(difference, object->pos, end);
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
        closest = object;
        w_u32(0x800ED634, projection);
    }
    ModelInstance *result = closest;
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

uint32 QueryCollisionRay(CollisionRay &record, uint32 include_objects)
{
    FUNCTION_MARKER(0x8007DD04, "SLUS_003.73");
    ModelBorrow borrow;
    sint32 start_x;
    sint32 end_x;
    sint32 start_z;
    sint32 min_x;
    sint32 end_z;
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
    uint32 grid_index;
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
        VisitCollisionObjects(visual_lists.solids, record);
    for (grid_index = 0; grid_index < 2; ++grid_index)
    {
        const ModelBankGrid grid = GetModelBankGrid(grid_index);
        if (!(((sint32)(grid.active))))
            goto LABEL_106;
        start_x = ((sint32)(record.pos[0][0]));
        end_x = ((sint32)(record.pos[1][0]));
        start_z = ((sint32)(record.pos[0][2]));
        min_x = ((sint32)(grid.min_x));
        end_z = ((sint32)(record.pos[1][2]));

        max_x = ((sint32)(grid.max_x));
        min_z = ((sint32)(grid.min_z));
        max_z = ((sint32)(grid.max_z));
        cell_size = ((sint32)(grid.cell_size));
        if ((start_x < min_x) && (end_x < min_x))
            goto LABEL_105;
        if (((((max_x < start_x) && (max_x < end_x)) || ((start_z < min_z) && (end_z < min_z))) || ((max_z < start_z) && (max_z < end_z))))
            goto LABEL_104;
        if (((start_x == end_x) && (start_z == end_z)))
        {
            cell_x = DivideCollisionWord((uint32)start_x - (uint32)min_x, (uint32)cell_size);
            grid_columns = (sint16)grid.columns;
            initial_cell_z = DivideCollisionWord((uint32)start_z - (uint32)min_z, (uint32)cell_size);
            if (cell_x == grid_columns)
                cell_x = (sint32)((uint32)cell_x - 1);
            cell_index_z = initial_cell_z;
            if ((initial_cell_z != ((sint16)(grid.rows))))
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

            target_cell_z = DivideCollisionWord((uint32)end_z - (uint32)min_z, (uint32)cell_size);
            remaining_x = remainder_x;
            grid_columns = ((sint16)(grid.columns));
            remaining_z = remainder_z;
            if ((initial_cell_x == grid_columns))
            {
                initial_cell_x = (sint32)((uint32)initial_cell_x - 1);
                remaining_x = (sint32)((uint32)remaining_x + (uint32)cell_size);

            }
            grid_rows = ((sint16)(grid.rows));
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

                if (((cell_x >= ((sint16)(grid.columns))) || (cell_z < 0)))
                    break;
                if ((cell_z >= ((sint16)(grid.rows))))
                    goto LABEL_99;
                call_delta_z = step_delta_z;
                call_target_column = target_column;
                call_delta_x = step_delta_x;
                VisitCollisionCells(grid.cells[cell_x][cell_z], record);
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

        LABEL_99:
            if (((cell_x >= ((sint16)(grid.columns))) || (cell_z < 0)))
                goto LABEL_105;

            out_of_grid = (cell_z >= ((sint16)(grid.rows)));

            if (out_of_grid)
                goto LABEL_106;
        }
        cell_index_z = cell_z;
    LABEL_103:
        VisitCollisionCells(grid.cells[cell_x][cell_index_z], record);

    LABEL_104:


    LABEL_105:


    LABEL_106:
        ;
    }
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

void PrepareCollisionBounds(const ModelGeometry &geometry, uint32 mirror_flags, const xport_draft_bounds *reference, const uint32 translation[3], xport_draft_bounds *result)
{
    FUNCTION_MARKER(0x80084814, "SLUS_003.73");
    uint32 minimum[3], maximum[3], offset[3], center[3];
    uint32 axis;
    // Snapshot the original register inputs before exposing the output
    for (axis = 0; axis < 3; ++axis)
    {
        offset[axis] = translation[axis];
        center[axis] = reference->minimum[axis] + reference->maximum[axis];
    }
    for (axis = 0; axis < 3; ++axis)
    {
        minimum[axis] = (uint32)(sint32)geometry.bounds[axis].max + offset[axis] + 2;
        maximum[axis] = (uint32)(sint32)geometry.bounds[axis].min + offset[axis] - 2;
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

void MarkLinkedCollisionCandidates(ModelInstance *object, const sint32 bounds[6], uint32 mark)
{
    FUNCTION_MARKER(0x800849FC, "SLUS_003.73");
    uint32 object_flags, scale, axis, delta, flip_bits, index;
    uint32 translation[3] = {0, 0, 0};
    sint32 lower[3], upper[3], first[3], second[3];
    xport_draft_bounds reference, result;
    uint32 matrix[5];
    do
    {
        object_flags = ((uint32)object->flags | ((uint32)object->generation << 16));
        index = object->model_idx;
        if (object_flags & 0x21)
            object->generation = mark;
        else if ((object_flags >> 16) != mark)
        {
            const ModelGeometry &geometry = GetModelGeometry(object->bank, index);
            LoadCollisionBounds(bounds, lower, upper, &flip_bits);
            for (axis = 0; axis < 3; ++axis)
            {
                delta = (uint32)((sint32)object->pos[axis] >> 12);
                lower[axis] = (sint32)((uint32)lower[axis] - delta);
                upper[axis] = (sint32)((uint32)upper[axis] - delta);
            }
            delta = ((uint32)object->angles[0] | ((uint32)object->angles[1] << 16));
            index = object->angles[2];
            if (delta | index)
            {
                BuildModelRotationMatrix(object->angles, (uint16 *)psx_addr(0x1F800028, 18));
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
            if (((uint32)object->flags | ((uint32)object->generation << 16)) & 0x200)
            {
                for (axis = 0; axis < 3; ++axis)
                {
                    scale = (uint32)(sint32)(sint16)object->scale[axis];
                    first[axis] = (sint32)((uint32)first[axis] * scale) >> 12;
                    second[axis] = (sint32)((uint32)second[axis] * scale) >> 12;
                }
            }
            if (!IntersectCollisionBounds(lower, upper, first, second))
                object->generation = mark;
        }
        object = object->next;
    } while (object);
}

void MarkArrayCollisionCandidates(ModelInstance *const *objects, uint32 count, const sint32 bounds[6], uint32 mark)
{
    FUNCTION_MARKER(0x80084C50, "SLUS_003.73");
    sint32 lower[3], upper[3], first[3], second[3];
    ModelInstance *object = count ? *objects : 0;
    uint32 flip_bits, axis, index;
    uint32 translation[3];
    xport_draft_bounds reference, result;
    if (!object)
        return;
    LoadCollisionBounds(bounds, lower, upper, &flip_bits);
    do
    {
        if (!object->active)
        {
            ++objects;
            --count;
            object = count ? *objects : 0;
            continue;
        }
        uint32 flags = ((uint32)object->flags | ((uint32)object->generation << 16));
        index = object->model_idx;
        if (flags & 0x21)
            object->generation = mark;
        else if ((flags >> 16) != mark)
        {
            const ModelGeometry &geometry = GetModelGeometry(object->bank, index);
            for (axis = 0; axis < 3; ++axis)
            {
                translation[axis] = (uint32)((sint32)object->pos[axis] >> 12);
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
                object->generation = mark;
        }
        ++objects;
        --count;
        object = count ? *objects : 0;
    } while (object);
}

uint32 TransformCollisionVertices(const ModelGeometry &geometry, uint32 output, uint32 depth_limit, const sint32 translation[3])
{
    FUNCTION_MARKER(0x80084D4C, "SLUS_003.73");
    uint32 translation_x = ReadCollisionRayWord(translation, 0);
    uint32 translation_y = ReadCollisionRayWord(translation, 4);
    uint32 translation_z = ReadCollisionRayWord(translation, 8);
    uint32 remaining = geometry.vertex_count;
    if (!remaining)
        return 0x60F;
    const SVECTOR *vertex = geometry.vertices;
    uint32 packed = xport_load_le32(vertex), zword = xport_load_le32((const uint8 *)vertex + 4);
    uint32 x = packed + translation_x;
    uint32 y = (uint32)((sint32)packed >> 16) + translation_y;
    uint32 z = zword + translation_z;
    uint32 result = 0x60F, mask;
    sint32 transformed_x, transformed_y, transformed_z;
    do
    {
        xport_gte_write_data(9, x);
        xport_gte_write_data(10, y);
        xport_gte_write_data(11, z);
        ++vertex;
        --remaining;
        xport_gte_execute(0x4D8012);
        // Prefetch the next vertex before storing the current result
        packed = remaining ? xport_load_le32(vertex) : 0;
        zword = remaining ? xport_load_le32((const uint8 *)vertex + 4) : 0;
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

void IntersectCollisionPolygons(const ModelGeometry &geometry, uint32 output, CollisionRay &record, ModelInstance *object)
{
    FUNCTION_MARKER(0x80084E24, "SLUS_003.73");
    uint32 required, rejected, count;
    uint32 header, flags, length;
    ModelPolygon *polygon;
    uint32 vertices[4], packed[3], y[3], clip[4];
    uint32 cross0, cross1, cross2, first_vertex;
    const SVECTOR *normal;
    uint32 start, numerator, endpoint, denominator, distance;
    uint32 direction, trigger_enabled, trigger_index, axis;

    length = record.len;
    xport_gte_write_control(11, 0);
    xport_gte_write_control(12, length);
    required = r_u32(0x800FF984);
    rejected = r_u32(0x800FF988);
    count = geometry.poly_count;
    if (!count)
        return;
    polygon = geometry.polys;
    do
    {
        flags = (((uint32)polygon->n << 3) | ((uint32)polygon->surface << 16));
        header = (((uint32)polygon->source_stride << 16) | polygon->flags);
        if (!(flags & rejected) && (flags | required) == 0xFFFFFFFF && ((flags >> 16) & 3) != 1)
        {
            packed[0] = (((uint32)polygon->v[0] << 3) | ((uint32)polygon->v[1] << 19));
            packed[1] = (((uint32)polygon->v[2] << 3) | ((uint32)polygon->v[3] << 19));
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
                    normal = geometry.normals + polygon->n;
                    xport_gte_write_data(0, xport_load_le32(normal));
                    xport_gte_write_data(1, xport_load_le32((const uint8 *)normal + 4));
                    first_vertex = output + ((((uint32)polygon->v[0] << 3) | ((uint32)polygon->v[1] << 19)) & 0xFFFF);
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
                    trigger_enabled = record.triggers;
                    if (flags & 0x20000)
                    {
                        trigger_index = object->model_idx;
                        if (trigger_enabled)
                            w_u32(0x800FFA88, trigger_index);
                    }
                    else if ((sint32)(record.distance - distance) > 0)
                    {
                        record.hit.SetObject((object));
                        record.distance = (distance);
                        record.hit.SetPolygon((polygon));
                        record.endpoint = (endpoint);
                        for (axis = 0; axis < 3; ++axis)
                            xport_gte_write_data(25 + axis, record.pos[0][axis]);
                        direction = ((uint16)record.basis.m[2][0] | ((uint32)(uint16)record.basis.m[2][1] << 16));
                        endpoint = (uint32)(uint16)record.basis.m[2][2];
                        xport_gte_write_data(9, direction);
                        xport_gte_write_data(10, direction >> 16);
                        xport_gte_write_data(11, endpoint);
                        xport_gte_write_data(8, distance);
                        xport_draft_gte_execute(0x1A0003E);
                        for (axis = 0; axis < 3; ++axis)
                            record.hit_pos[axis] = (xport_gte_read_data(25 + axis));
                        normal = geometry.normals + polygon->n;
                        collision_normal = *normal;
                    }
                }
            }
        }
    next_polygon:
        polygon = polygon->next;
        --count;
    } while (count);
    return;
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















































extern "C" void apocalypse_mark_linked_collision_candidates(uint32 list, const sint32 bounds[6], uint32 mark) { MarkLinkedCollisionCandidates(list, bounds, mark); }





extern "C" uint32 apocalypse_ground_native(const void *position, uint32 below, uint32 above) { return FindGroundHeight((const uint32 *)position, below, above); }

extern "C" uint32 sub_80067A18(uint32 position, uint32 below, uint32 above)
{
    FUNCTION_MARKER(0x80067A18, "SLUS_003.73");
    return FindGroundHeight((const uint32 *)psx_addr(position, 12), below, above);
}

extern "C" uint32 apocalypse_visibility_native(const void *start, uint32 end) { return IsSegmentVisible((const uint32 *)start, (const uint32 *)psx_addr(end, 12)); }



extern "C" uint32 sub_8004BA64(uint32 object, uint32 output, uint32 target, uint32 minimum, uint32 maximum, uint32 height, uint32 spread, uint32 ground, uint32 collision)
{
    FUNCTION_MARKER(0x8004BA64, "SLUS_003.73");
    return ChooseCandidatePosition(object, (uint32 *)psx_addr(output, 12), (const uint32 *)psx_addr(target, 12), minimum, maximum, height, spread, ground, collision);
}

extern "C" uint32 apocalypse_candidate_native(uint32 object, uint32 output[3], uint32 target, uint32 minimum, uint32 maximum, uint32 height, uint32 spread, uint32 ground, uint32 collision) { return ChooseCandidatePosition(object, output, (const uint32 *)psx_addr(target, 12), minimum, maximum, height, spread, ground, collision); }

extern "C" uint32 apocalypse_candidate_vectors_native(uint32 object, uint32 output[3], const uint32 target[3], uint32 minimum, uint32 maximum, uint32 height, uint32 spread, uint32 ground, uint32 collision) { return ChooseCandidatePosition(object, output, target, minimum, maximum, height, spread, ground, collision); }
