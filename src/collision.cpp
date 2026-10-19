#include <stdlib.h>
#include <string.h>

#include "geometry_commands.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "scripted_level_object.h"
#include "collision.h"
static void MarkArrayCollisionCandidates(ModelInstance *const *objects, uint32 count, const Vector3 bounds[2], uint32 mark);
static void MarkLinkedCollisionCandidates(ModelInstance *list, const Vector3 bounds[2], uint32 mark);
static uint32 IntersectCollisionObject(ModelInstance *object, CollisionRay &record);
static void VisitCollisionObjects(ModelInstance *list, CollisionRay &record);
#include "vector_arithmetic.h"
#include "game_random.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "model_resources.h"
#include "record_scripts.h"
#include "model_projection.h"
#include "player.h"

struct CollisionBounds
{
    Vector3 minimum, maximum;
};

struct CollisionVertex
{
    XY xy;
    uint16 z;
    uint16 clip;
};

static CollisionVertex *collision_vertices;
static uint32 collision_vertex_capacity;

static CollisionRay shared_collision_ray;
static SVECTOR collision_normal;
static MATRIX collision_matrix;
static uint32 collision_trigger;
CollisionQueryOptions collision_query;
static uint16 collision_generation = 0xFFFF;
static uint32 polygon_required_mask;
static uint32 polygon_rejected_mask;

void ResetCollisionState()
{
    free(collision_vertices);
    collision_vertices = 0;
    collision_vertex_capacity = 0;
    shared_collision_ray = CollisionRay();
    collision_normal = SVECTOR();
    memset(&collision_matrix, 0, sizeof(collision_matrix));
    collision_trigger = 0;
    CollisionQueryOptions initial = {false, false, false, false};
    collision_query = initial;
    collision_generation = 0xFFFF;
    polygon_required_mask = 0;
    polygon_rejected_mask = 0;
}

CollisionRay &GetSharedCollisionRay()
{
    return shared_collision_ray;
}

static uint32 ReadCollisionSquareSum()
{
    const uint32 x = xport_gte_read_data(25);
    const uint32 y = xport_gte_read_data(26);
    const uint32 z = xport_gte_read_data(27);
    return x + y + z;
}

static void SquareCollisionVector(const uint32 *vector)
{
    xport_gte_write_data(9, vector[0]);
    xport_gte_write_data(10, vector[1]);
    xport_gte_write_data(11, vector[2]);
    xport_gte_execute(0xA00428);
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

ModelInstance *TraceSegmentCollision(const uint32 *start, const uint32 *end, ModelInstance *list)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    FUNCTION_MARKER(0x80067508, "SLUS_003.73");
    Vector3 hit;
    ModelInstance *result = IntersectObjectSpheres(start, end, hit.words, list, 0);
    if (!result)
    {
        for (uint32 axis = 0; axis < 3; ++axis)
            shared_ray.endpoints[0].words[axis] = start[axis];
        for (uint32 axis = 0; axis < 3; ++axis)
            shared_ray.endpoints[1].words[axis] = end[axis];
        InitializeCollisionRay(shared_ray);
        ModelInstance *objects = visual_lists.solids;
        collision_query.reject_surface40 = true;
        VisitCollisionObjects(objects, shared_ray);
        result = shared_ray.hit.GetObject();
        collision_query.reject_surface40 = false;
    }
    return result;
}

uint32 IsSegmentVisible(const uint32 *start, const uint32 *end)
{
    FUNCTION_MARKER(0x800679A4, "SLUS_003.73");
    CollisionRay collision;
    collision.endpoints[0].words[0] = start[0];
    collision.endpoints[0].words[1] = start[1];
    collision.endpoints[0].words[2] = start[2];
    collision.endpoints[1].words[0] = end[0];
    collision.endpoints[1].words[1] = end[1];
    collision.endpoints[1].words[2] = end[2];
    InitializeCollisionRay(collision);
    QueryCollisionRay(collision, 1);
    return collision.hit.GetObject() == 0;
}

uint32 FindGroundHeight(const uint32 *position, uint32 below, uint32 above)
{
    FUNCTION_MARKER(0x80067A18, "SLUS_003.73");
    CollisionRay collision;
    collision.endpoints[0].words[0] = position[0];
    collision.endpoints[0].words[1] = position[1] - (below << 12);
    collision.endpoints[0].words[2] = position[2];
    collision.endpoints[1].words[0] = position[0];
    collision.endpoints[1].words[1] = position[1] + (above << 12);
    collision.endpoints[1].words[2] = position[2];
    InitializeCollisionRay(collision);
    collision.triggers = 0;
    QueryCollisionRay(collision, 1);
    if (collision.hit.GetObject())
    {
        if ((sint16)collision.n.vy < -3071)
            return collision.hit_pos.words[1];
    }
    return 0xFFFFFFFF;
}

static void AdvanceCollisionGeneration()
{
    FUNCTION_MARKER(0x8007BAB0, "SLUS_003.73");
    ++collision_generation;
    if (!collision_generation)
    {
        ModelInstance *scene_object = GetModelSceneObjects();
        collision_generation = 1;
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
}

uint32 InitializeCollisionRay(CollisionRay &record)
{
    FUNCTION_MARKER(0x8007BB24, "SLUS_003.73");
    uint32 first, second, delta, horizontal, horizontal_root, full, full_root;
    uint32 normalization, shift, full_shift, sine, cosine, length, generation;
    uint32 axis;
    sint32 dx, dy, dz;
    const uint32 *start = record.endpoints[0].words, *end = record.endpoints[1].words;
    MATRIX *matrix = &record.basis;
    MATRIX yaw = {0};

    record.distance = (0x7FFFFFFF);
    second = record.endpoints[1].words[0];
    first = record.endpoints[0].words[0];
    record.hit.SetObject((0));
    record.endpoint = (0xFFFFFFFF);
    record.triggers = (uint8)(0);
    record.upward = (uint8)(0);
    delta = (uint32)((sint32)(second - first) >> 12);
    dx = (sint16)delta;
    horizontal = (uint32)(dx * dx);
    second = record.endpoints[1].words[1];
    first = record.endpoints[0].words[1];
    delta = (uint32)((sint32)(second - first) >> 12);
    dy = (sint16)delta;
    second = record.endpoints[1].words[2];
    first = record.endpoints[0].words[2];
    delta = (uint32)((sint32)(second - first) >> 12);
    dz = (sint16)delta;
    horizontal += (uint32)(dz * dz);
    if (horizontal)
    {
        normalization = ReadCollisionLeadingBits(horizontal) - 1;
        shift = (uint32)((sint32)normalization >> 1);
        horizontal_root = LookupDistanceRoot(horizontal << (normalization & 30));
        sine = (uint32)DivideCollisionWord((uint32)dx << ((shift + 12) & 31), horizontal_root);
        cosine = (uint32)DivideCollisionWord((uint32)dz << ((shift + 12) & 31), horizontal_root);
        yaw.m[0][0] = (uint16)cosine;
        yaw.m[0][1] = 0;
        yaw.m[0][2] = (uint16)(0 - sine);
        yaw.m[1][0] = 0;
        yaw.m[1][1] = ONE;
        yaw.m[1][2] = 0;
        yaw.m[2][0] = (uint16)sine;
        yaw.m[2][1] = 0;
        yaw.m[2][2] = (uint16)cosine;
        full = horizontal + (uint32)(dy * dy);
        normalization = ReadCollisionLeadingBits(full) - 1;
        full_shift = (uint32)((sint32)normalization >> 1);
        full_root = LookupDistanceRoot(full << (normalization & 30));
        cosine = (uint32)DivideCollisionWord(horizontal_root << 12, full_root << ((shift - full_shift) & 31));
        length = (uint32)((sint32)full_root >> (full_shift & 31));
        record.len = (length);
        sine = (uint32)DivideCollisionWord((uint32)dy << ((full_shift + 12) & 31), full_root);
        record.basis.m[0][0] = (sint16)(4096);
        record.basis.m[0][1] = (sint16)(0);
        record.basis.m[0][2] = (sint16)(0);
        record.basis.m[1][0] = (sint16)(0);
        record.basis.m[1][1] = (sint16)((uint16)cosine);
        record.basis.m[1][2] = (sint16)((uint16)(0 - sine));
        record.basis.m[2][0] = (sint16)(0);
        record.basis.m[2][1] = (sint16)((uint16)sine);
        record.basis.m[2][2] = (sint16)((uint16)cosine);
        MulMatrix0(matrix, &yaw, matrix);
    }
    else
    {
        sine = dy < 0 ? 0xFFFFF000 : 4096;
        record.len = (dy < 0 ? 0 - (uint32)dy : (uint32)dy);
        if (dy >= 0)
            record.upward = (uint8)(1);
        record.basis.m[0][0] = (sint16)(4096);
        record.basis.m[0][1] = (sint16)(0);
        record.basis.m[0][2] = (sint16)(0);
        record.basis.m[1][0] = (sint16)(0);
        record.basis.m[1][1] = (sint16)(0);
        sine = (uint16)sine;
        record.basis.m[2][0] = (sint16)(0);
        record.basis.m[2][2] = (sint16)(0);
        record.basis.m[1][2] = (sint16)((uint16)(0 - sine));
        record.basis.m[2][1] = (sint16)((uint16)sine);
    }
    SetRotMatrix(matrix);
    first = record.endpoints[0].words[0];
    second = record.endpoints[1].words[0];
    if ((sint32)first < (sint32)second)
        record.min.words[0] = (first);
    else
    {
        first = record.endpoints[0].words[0];
        record.min.words[0] = (second);
        second = first;
    }
    record.max.words[0] = (second);
    for (axis = 1; axis < 3; ++axis)
    {
        first = start[axis];
        second = end[axis];
        if ((sint32)first < (sint32)second)
        {
            record.min.words[axis] = (first);
            second = end[axis];
        }
        else
        {
            record.min.words[axis] = (second);
            second = start[axis];
        }
        record.max.words[axis] = (second);
    }
    AdvanceCollisionGeneration();
    generation = collision_generation;
    record.generation = generation;
    return generation;
}

static void VisitCollisionObjects(ModelInstance *list, CollisionRay &record)
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
    MarkLinkedCollisionCandidates(list, record.endpoints, generation);
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

static void VisitCollisionCells(const ModelGridCell &cell, CollisionRay &record)
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
    MarkArrayCollisionCandidates(cell.objects, cell.count, record.endpoints, generation);
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

static uint32 FinishCollisionRay(CollisionRay &record)
{
    FUNCTION_MARKER(0x8007BFD4, "SLUS_003.73");
    ModelInstance *object = record.hit.GetObject();
    if (!object)
        return 0;
    BuildModelRotationMatrix(object->angles.words, collision_matrix);
    SetRotMatrix(&collision_matrix);
    const SVECTOR *normal = &collision_normal;
    xport_gte_write_data(0, ((uint16)normal->vx | ((uint32)(uint16)normal->vy << 16)));
    xport_gte_write_data(1, ((uint16)normal->vz | ((uint32)(uint16)normal->pad << 16)));
    xport_gte_execute(0x486012);
    uint32 normal_x = xport_gte_read_data(9);
    uint32 normal_y = xport_gte_read_data(10);
    uint32 normal_z = xport_gte_read_data(11);
    record.n.vx = (sint16)((uint16)normal_x);
    record.n.vy = (sint16)((uint16)normal_y);
    record.n.vz = (sint16)((uint16)normal_z);
    return 1;
}

static void TransformCollisionMatrix(MATRIX &matrix)
{
    uint32 first = ((uint16)matrix.m[0][0] | ((uint32)(uint16)matrix.m[0][1] << 16)), second = ((uint16)matrix.m[0][2] | ((uint32)(uint16)matrix.m[1][0] << 16));
    uint32 fourth = ((uint16)matrix.m[2][0] | ((uint32)(uint16)matrix.m[2][1] << 16)), third, fifth, axis;
    Vector3 values[3];
    FUNCTION_MARKER(0x800855B4, "SLUS_003.73");
    xport_gte_write_data(9, first);
    xport_gte_write_data(10, second >> 16);
    xport_gte_write_data(11, fourth);
    xport_gte_execute(0x4DE012);
    third = ((uint16)matrix.m[1][1] | ((uint32)(uint16)matrix.m[1][2] << 16));
    for (axis = 0; axis < 3; ++axis)
        values[0].words[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(9, first >> 16);
    xport_gte_write_data(10, third);
    xport_gte_write_data(11, fourth >> 16);
    xport_gte_execute(0x4DE012);
    fifth = ((uint16)matrix.m[2][2]);
    for (axis = 0; axis < 3; ++axis)
        values[1].words[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(9, second);
    xport_gte_write_data(10, third >> 16);
    xport_gte_write_data(11, fifth);
    xport_gte_execute(0x4DE012);
    matrix.m[0][0] = values[0].words[0];
    matrix.m[0][1] = values[1].words[0];
    matrix.m[2][0] = values[0].words[2];
    matrix.m[2][1] = values[1].words[2];
    for (axis = 0; axis < 3; ++axis)
        values[2].words[axis] = xport_gte_read_data(25 + axis);
    matrix.m[0][2] = values[2].words[0];
    matrix.m[1][0] = values[0].words[1];
    matrix.m[1][1] = values[1].words[1];
    matrix.m[1][2] = values[2].words[1];
    matrix.m[2][2] = values[2].words[2];
}

static CollisionVertex *GetCollisionVertices(uint32 count)
{
    if (count > collision_vertex_capacity)
    {
        if (count > (size_t)-1 / sizeof(*collision_vertices))
        {
            GameDiagnostic("Collision vertex allocation size overflow\n");
            abort();
        }
        void *memory = realloc(collision_vertices, (size_t)count * sizeof(*collision_vertices));
        if (!memory)
        {
            GameDiagnostic("Cannot allocate collision vertices\n");
            abort();
        }
        collision_vertices = (CollisionVertex *)memory;
        collision_vertex_capacity = count;
    }
    return collision_vertices;
}

static uint32 TransformCollisionVertices(const ModelGeometry &geometry, CollisionVertex *output, uint32 depth_limit, const Vector3 &translation);
static void IntersectCollisionPolygons(const ModelGeometry &geometry, const CollisionVertex *output, CollisionRay &record, ModelInstance *object);

static uint32 IntersectCollisionObject(ModelInstance *object, CollisionRay &record)
{
    ResourceScriptBorrow script_borrow;
    FUNCTION_MARKER(0x8007C06C, "SLUS_003.73");
    if (!object->active)
        return 0;
    uint32 result, model, index, trigger, target, axis;
    uint32 first, second, height_mode;
    Vector3 translation;
    result = object->flags & 0x21;
    if (result)
        return result;
    SetColorMatrix(&record.basis);
    model = object->bank;
    index = object->model_idx;
    const ModelGeometry &geometry = GetModelGeometry(model, index);
    CollisionVertex *output = GetCollisionVertices(geometry.vertex_count);
    if (!((uint32)object->angles.words[0] | ((uint32)object->angles.words[1] << 16)) && !object->angles.words[2] && !(object->flags & 0x200))
    {
        for (axis = 0; axis < 3; ++axis)
        {
            first = object->pos.words[axis];
            second = record.endpoints[0].words[axis];
            translation.components[axis] = (sint32)(first - second) >> 12;
        }
        xport_gte_write_control(5, 0);
        xport_gte_write_control(6, 0);
        xport_gte_write_control(7, 0);
    }
    else
    {
        height_mode = record.upward;
        first = object->pos.words[0];
        second = record.endpoints[0].words[0];
        collision_matrix.t[0] = (sint32)(first - second) >> 12;
        if (height_mode)
        {
            first = record.endpoints[0].words[2];
            second = object->pos.words[2];
            collision_matrix.t[1] = (sint32)(first - second) >> 12;
            first = object->pos.words[1];
            second = record.endpoints[0].words[1];
            collision_matrix.t[2] = (sint32)(first - second) >> 12;
        }
        else
        {
            first = object->pos.words[1];
            second = record.endpoints[0].words[1];
            collision_matrix.t[1] = (sint32)(first - second) >> 12;
            first = object->pos.words[2];
            second = record.endpoints[0].words[2];
            collision_matrix.t[2] = (sint32)(first - second) >> 12;
            second = (uint16)collision_matrix.t[1];
            first = (uint16)collision_matrix.t[0];
            xport_gte_write_data(0, first | (second << 16));
            xport_gte_write_data(1, collision_matrix.t[2]);
            xport_gte_execute(0x4C6012);
            collision_matrix.t[0] = (sint32)xport_gte_read_data(25);
            collision_matrix.t[1] = (sint32)xport_gte_read_data(26);
            collision_matrix.t[2] = (sint32)xport_gte_read_data(27);
        }
        SetTransMatrix(&collision_matrix);
        translation.components[0] = translation.components[1] = translation.components[2] = 0;
        if (((uint32)object->angles.words[0] | ((uint32)object->angles.words[1] << 16)) || object->angles.words[2])
        {
            BuildModelRotationMatrix(object->angles.words, collision_matrix);
            if (object->flags & 0x200)
                ScaleModelRotationMatrix(object->scale.words, collision_matrix);
        }
        else
        {
            InitializeModelIdentityMatrix(collision_matrix);
            ScaleModelRotationMatrix(object->scale.words, collision_matrix);
        }
        TransformCollisionMatrix(collision_matrix);
        SetColorMatrix(&collision_matrix);
    }
    first = record.len;
    result = TransformCollisionVertices(geometry, output, first, translation);
    result &= 0x60F;
    if (result)
        return result;
    collision_trigger = 0xFFFFFFFF;
    (void)IntersectCollisionPolygons(geometry, output, record, object);
    result = collision_trigger;
    if (result == 0xFFFFFFFF)
        return result;
    index = FindActiveModelBankSlot(object->bank);
    trigger = collision_trigger;
    target = ReadModelKey(GetActiveModelScriptKeys(index), trigger);
    SelectModelBankReplacement(index ^ 1);
    collision_query.require_surface02 = false;
    return ActivateResourceScript(target) != 0;
}

ModelInstance *IntersectObjectSpheres(const uint32 *start, const uint32 *end, uint32 *output, ModelInstance *list, ModelInstance *excluded)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    ModelBorrow borrow;
    Vector3 difference, vector, shifted, direction;
    Vector3 min, max;
    uint32 shift = 12;
    uint32 best = 0x7FFFFFFF;
    ModelInstance *closest = 0;
    SubtractVectors(difference.words, end, start);
    ShiftVectorRight(vector.words, difference.words, shift);
    SquareCollisionVector(vector.words);
    const uint32 length = LookupDistanceRoot(ReadCollisionSquareSum());
    if (!length)
        return 0;
    ShiftVectorLeft(shifted.words, vector.words, shift);
    DivideVector(direction.words, shifted.words, length);
    xport_gte_write_control(0, direction.words[0]);
    xport_gte_write_control(2, direction.words[1]);
    xport_gte_write_control(4, direction.words[2]);
    const uint32 packed_xy = (uint16)direction.words[0] | ((uint32)(uint16)direction.words[1] << 16);
    const uint32 packed_z = (uint16)direction.words[2];
    xport_gte_write_control(8, packed_xy);
    xport_gte_write_control(9, packed_z);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        const uint32 first = start[axis];
        const uint32 second = end[axis];
        if ((sint32)first < (sint32)second)
        {
            min.words[axis] = first;
            max.words[axis] = second;
        }
        else
        {
            min.words[axis] = second;
            max.words[axis] = first;
        }
    }
    for (ModelInstance *object = list; object; object = object->next)
    {
        VisualObject &visual = *static_cast<VisualObject *>(object->owner);
        if (visual.lifecycle_flags & 64)
            continue;
        if (object == excluded)
            continue;
        uint32 radius = (uint32)visual.collision_radius << 12;
        const uint32 x = object->pos.words[0];
        if ((sint32)(x + radius) < (sint32)min.words[0] || (sint32)max.words[0] < (sint32)(x - radius))
            continue;
        const uint32 y = object->pos.words[1];
        if ((sint32)(y + radius) < (sint32)min.words[1] || (sint32)max.words[1] < (sint32)(y - radius))
            continue;
        const uint32 z = object->pos.words[2];
        if ((sint32)(z + radius) < (sint32)min.words[2] || (sint32)max.words[2] < (sint32)(z - radius))
            continue;
        SubtractVectors(difference.words, object->pos.words, start);
        ShiftVectorRight(vector.words, difference.words, shift);
        xport_gte_write_data(11, vector.words[2]);
        xport_gte_write_data(9, vector.words[0]);
        xport_gte_write_data(10, vector.words[1]);
        xport_gte_execute(0x178000C);
        xport_gte_execute(0xA00428);
        radius = visual.collision_radius;
        const uint32 squared_radius = radius * radius;
        uint32 squared = ReadCollisionSquareSum();
        if ((sint32)squared >= (sint32)squared_radius)
            continue;
        const uint32 packed_delta = (uint16)vector.words[0] | ((uint32)(uint16)vector.words[1] << 16);
        xport_gte_write_data(0, packed_delta);
        xport_gte_write_data(1, vector.words[2]);
        xport_gte_execute(0x4A6012);
        const uint32 projection = xport_gte_read_data(25);
        if ((sint32)projection >= (sint32)best)
            continue;
        bool test_endpoint = false;
        if ((sint32)projection < 0)
        {
            SquareCollisionVector(vector.words);
            test_endpoint = true;
        }
        else if ((sint32)length < (sint32)projection)
        {
            SubtractVectors(difference.words, object->pos.words, end);
            ShiftVectorRight(vector.words, difference.words, shift);
            SquareCollisionVector(vector.words);
            test_endpoint = true;
        }
        if (test_endpoint)
        {
            squared = ReadCollisionSquareSum();
            if ((sint32)squared >= (sint32)squared_radius)
                continue;
        }
        closest = object;
        best = projection;
    }
    if (!closest)
        return 0;
    xport_gte_write_data(9, direction.words[0]);
    xport_gte_write_data(10, direction.words[1]);
    xport_gte_write_data(11, direction.words[2]);
    xport_gte_write_data(8, best);
    ExecuteGeometryCommand(0x198003D);
    for (uint32 axis = 0; axis < 3; ++axis)
        output[axis] = xport_gte_read_data(25 + axis);
    ShiftVectorLeft(output, shift);
    AddVectors(output, start);
    return closest;
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
    polygon_rejected_mask = 0;
    polygon_required_mask = 0xFFFFFFFF;
    if (collision_query.reject_surface40)
        polygon_rejected_mask = 0x400000;
    if (!collision_query.allow_surface20)
        polygon_rejected_mask ^= 0x200000;
    if (collision_query.require_surface10)
        polygon_required_mask = 0xFFEFFFFF;
    if (collision_query.require_surface02)
        polygon_required_mask ^= 0x20000;
    if (include_objects)
        VisitCollisionObjects(visual_lists.solids, record);
    for (grid_index = 0; grid_index < 2; ++grid_index)
    {
        const ModelBankGrid grid = GetModelBankGrid(grid_index);
        if (!(((sint32)(grid.active))))
            goto NextGrid;
        start_x = ((sint32)(record.endpoints[0].words[0]));
        end_x = ((sint32)(record.endpoints[1].words[0]));
        start_z = ((sint32)(record.endpoints[0].words[2]));
        min_x = ((sint32)(grid.min_x));
        end_z = ((sint32)(record.endpoints[1].words[2]));

        max_x = ((sint32)(grid.max_x));
        min_z = ((sint32)(grid.min_z));
        max_z = ((sint32)(grid.max_z));
        cell_size = ((sint32)(grid.cell_size));
        if ((start_x < min_x) && (end_x < min_x))
            goto NextGrid;
        if (((((max_x < start_x) && (max_x < end_x)) || ((start_z < min_z) && (end_z < min_z))) || ((max_z < start_z) && (max_z < end_z))))
            goto NextGrid;
        if (((start_x == end_x) && (start_z == end_z)))
        {
            cell_x = DivideCollisionWord((uint32)start_x - (uint32)min_x, (uint32)cell_size);
            grid_columns = (sint16)grid.columns;
            initial_cell_z = DivideCollisionWord((uint32)start_z - (uint32)min_z, (uint32)cell_size);
            if (cell_x == grid_columns)
                cell_x = (sint32)((uint32)cell_x - 1);
            cell_index_z = initial_cell_z;
            if ((initial_cell_z != ((sint16)(grid.rows))))
                goto VisitFinalCell;
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
                    goto NextGrid;

                if (((cell_x >= ((sint16)(grid.columns))) || (cell_z < 0)))
                    break;
                if ((cell_z >= ((sint16)(grid.rows))))
                    goto CheckFinalCell;
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
                goto NextGrid;

        CheckFinalCell:
            if (((cell_x >= ((sint16)(grid.columns))) || (cell_z < 0)))
                goto NextGrid;

            out_of_grid = (cell_z >= ((sint16)(grid.rows)));

            if (out_of_grid)
                goto NextGrid;
        }
        cell_index_z = cell_z;
    VisitFinalCell:
        VisitCollisionCells(grid.cells[cell_x][cell_index_z], record);

    NextGrid:;
    }
    return FinishCollisionRay(record);
}

static void NormalizeCollisionBounds(Vector3 &low, Vector3 &high, uint32 *flip_bits)
{
    FUNCTION_MARKER(0x800847AC, "SLUS_003.73");
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 delta = (uint32)high.components[axis] - (uint32)low.components[axis];
        if ((sint32)delta < 0)
        {
            sint32 tmp = low.components[axis];
            low.components[axis] = high.components[axis];
            high.components[axis] = tmp;
            delta = (uint32)high.components[axis] - (uint32)low.components[axis];
            *flip_bits ^= 1 << axis;
        }
        xport_gte_write_control(axis * 2, delta);
    }
}

static void LoadCollisionBounds(const Vector3 bounds[2], Vector3 &low, Vector3 &high, uint32 *flip_bits)
{
    FUNCTION_MARKER(0x80084778, "SLUS_003.73");
    Vector3 saved[2];
    uint32 axis;
    saved[0] = bounds[0];
    saved[1] = bounds[1];
    for (axis = 0; axis < 3; ++axis)
    {
        low.components[axis] = saved[0].components[axis] >> 12;
        high.components[axis] = saved[1].components[axis] >> 12;
    }
    *flip_bits = 0;
    NormalizeCollisionBounds(low, high, flip_bits);
}

static void PrepareCollisionBounds(const ModelGeometry &geometry, uint32 mirror_flags, const CollisionBounds &reference, const Vector3 &translation, CollisionBounds &result)
{
    FUNCTION_MARKER(0x80084814, "SLUS_003.73");
    Vector3 minimum, maximum, offset, center;
    uint32 axis;
    // Snapshot the original register inputs before exposing the output
    for (axis = 0; axis < 3; ++axis)
    {
        offset.words[axis] = translation.words[axis];
        center.words[axis] = reference.minimum.words[axis] + reference.maximum.words[axis];
    }
    for (axis = 0; axis < 3; ++axis)
    {
        minimum.words[axis] = (uint32)(sint32)geometry.bounds[axis].max + offset.words[axis] + 2;
        maximum.words[axis] = (uint32)(sint32)geometry.bounds[axis].min + offset.words[axis] - 2;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        if (mirror_flags & (1 << axis))
        {
            uint32 mirrored_minimum = center.words[axis] - maximum.words[axis];
            maximum.words[axis] = center.words[axis] - minimum.words[axis];
            minimum.words[axis] = mirrored_minimum;
        }
    }
    result.minimum = minimum;
    result.maximum = maximum;
}

static uint32 IntersectCollisionBounds(const Vector3 &query_low, const Vector3 &query_high, const Vector3 &object_low, const Vector3 &object_high)
{
    FUNCTION_MARKER(0x800848D0, "SLUS_003.73");
    Vector3 lower_delta, upper_delta, cross;
    sint32 first, second;
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        if ((sint32)((uint32)query_high.components[axis] - (uint32)object_high.components[axis]) < 0)
            return 0;
    for (axis = 0; axis < 3; ++axis)
    {
        lower_delta.components[axis] = (sint32)((uint32)object_low.components[axis] - (uint32)query_low.components[axis]);
        if (lower_delta.components[axis] < 0)
            return 0;
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, (uint32)lower_delta.components[axis]);
    xport_gte_execute(0x170000C);
    for (axis = 0; axis < 3; ++axis)
        upper_delta.components[axis] = (sint32)((uint32)object_high.components[axis] - (uint32)query_low.components[axis]);
    for (axis = 0; axis < 3; ++axis)
        cross.components[axis] = (sint32)xport_gte_read_data(25 + axis);
    if (cross.components[0] < 0)
    {
        if (cross.components[1] < 0)
            goto CheckX;
        xport_gte_write_data(9, (uint32)upper_delta.components[0]);
        xport_gte_write_data(10, (uint32)upper_delta.components[1]);
        xport_gte_write_data(11, (uint32)lower_delta.components[2]);
        xport_gte_execute(0x170000C);
        first = (sint32)xport_gte_read_data(9);
        second = (sint32)xport_gte_read_data(10);
        return first < 0 || second > 0 ? 0 : 1;
    }
    if (cross.components[2] < 0)
    {
        xport_gte_write_data(9, (uint32)upper_delta.components[0]);
        xport_gte_write_data(10, (uint32)lower_delta.components[1]);
        xport_gte_write_data(11, (uint32)upper_delta.components[2]);
        xport_gte_execute(0x170000C);
        first = (sint32)xport_gte_read_data(9);
        second = (sint32)xport_gte_read_data(11);
        return first > 0 || second < 0 ? 0 : 1;
    }
CheckX:
    xport_gte_write_data(9, (uint32)lower_delta.components[0]);
    xport_gte_write_data(10, (uint32)upper_delta.components[1]);
    xport_gte_write_data(11, (uint32)upper_delta.components[2]);
    xport_gte_execute(0x170000C);
    first = (sint32)xport_gte_read_data(10);
    second = (sint32)xport_gte_read_data(11);
    return first < 0 || second > 0 ? 0 : 1;
}

static void MarkLinkedCollisionCandidates(ModelInstance *object, const Vector3 bounds[2], uint32 mark)
{
    FUNCTION_MARKER(0x800849FC, "SLUS_003.73");
    uint32 object_flags, scale, axis, delta, flip_bits, index;
    Vector3 translation = {0};
    Vector3 lower, upper, first, second;
    CollisionBounds reference, result;
    MATRIX matrix = {0};
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
                delta = (uint32)((sint32)object->pos.words[axis] >> 12);
                lower.components[axis] = (sint32)((uint32)lower.components[axis] - delta);
                upper.components[axis] = (sint32)((uint32)upper.components[axis] - delta);
            }
            delta = ((uint32)object->angles.words[0] | ((uint32)object->angles.words[1] << 16));
            index = object->angles.words[2];
            if (delta | index)
            {
                BuildModelRotationMatrix(object->angles.words, matrix);
                TransposeRotationMatrix(matrix);
                SetRotMatrix(&matrix);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32)lower.components[axis]);
                xport_gte_execute(0x49E012);
                for (axis = 0; axis < 3; ++axis)
                    lower.components[axis] = (sint32)xport_gte_read_data(9 + axis);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32)upper.components[axis]);
                xport_gte_execute(0x49E012);
                for (axis = 0; axis < 3; ++axis)
                    upper.components[axis] = (sint32)xport_gte_read_data(9 + axis);
                NormalizeCollisionBounds(lower, upper, &flip_bits);
            }
            reference.minimum = lower;
            reference.maximum = upper;
            PrepareCollisionBounds(geometry, flip_bits, reference, translation, result);
            first = result.minimum;
            second = result.maximum;
            if (((uint32)object->flags | ((uint32)object->generation << 16)) & 0x200)
            {
                for (axis = 0; axis < 3; ++axis)
                {
                    scale = (uint32)(sint32)(sint16)object->scale.words[axis];
                    first.components[axis] = (sint32)((uint32)first.components[axis] * scale) >> 12;
                    second.components[axis] = (sint32)((uint32)second.components[axis] * scale) >> 12;
                }
            }
            if (!IntersectCollisionBounds(lower, upper, first, second))
                object->generation = mark;
        }
        object = object->next;
    } while (object);
}

static void MarkArrayCollisionCandidates(ModelInstance *const *objects, uint32 count, const Vector3 bounds[2], uint32 mark)
{
    FUNCTION_MARKER(0x80084C50, "SLUS_003.73");
    Vector3 lower, upper, first, second;
    ModelInstance *object = count ? *objects : 0;
    uint32 flip_bits, axis, index;
    Vector3 translation;
    CollisionBounds reference, result;
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
                translation.words[axis] = (uint32)((sint32)object->pos.words[axis] >> 12);
            reference.minimum = lower;
            reference.maximum = upper;
            PrepareCollisionBounds(geometry, flip_bits, reference, translation, result);
            first = result.minimum;
            second = result.maximum;
            if (!IntersectCollisionBounds(lower, upper, first, second))
                object->generation = mark;
        }
        ++objects;
        --count;
        object = count ? *objects : 0;
    } while (object);
}

static uint32 TransformCollisionVertices(const ModelGeometry &geometry, CollisionVertex *output, uint32 depth_limit, const Vector3 &translation)
{
    FUNCTION_MARKER(0x80084D4C, "SLUS_003.73");
    uint32 translation_x = (uint32)translation.components[0];
    uint32 translation_y = (uint32)translation.components[1];
    uint32 translation_z = (uint32)translation.components[2];
    uint32 remaining = geometry.vertex_count;
    if (!remaining)
        return 0x60F;
    const SVECTOR *vertex = geometry.vertices;
    uint32 packed = ((uint16)vertex->vx | ((uint32)(uint16)vertex->vy << 16)), zword = ((uint16)vertex->vz | ((uint32)(uint16)vertex->pad << 16));
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
        packed = remaining ? ((uint16)vertex->vx | ((uint32)(uint16)vertex->vy << 16)) : 0;
        zword = remaining ? ((uint16)vertex->vz | ((uint32)(uint16)vertex->pad << 16)) : 0;
        x = packed + translation_x;
        y = (uint32)((sint32)packed >> 16) + translation_y;
        z = zword + translation_z;
        transformed_x = (sint32)xport_gte_read_data(25);
        transformed_y = (sint32)xport_gte_read_data(26);
        transformed_z = (sint32)xport_gte_read_data(27);
        mask = ((uint32)(transformed_x < 0) << 2) | ((uint32)(transformed_y < 0) << 1) | (uint32)(transformed_z < 0) | ((uint32)((sint32)depth_limit < transformed_z) << 3) | ((uint32)(transformed_x > 0) << 10) | ((uint32)(transformed_y > 0) << 9);
        output->xy.xy = ((uint32)transformed_x & 0xFFFF) | ((uint32)transformed_y << 16);
        output->z = transformed_z;
        result &= mask;
        output->clip = mask;
        ++output;
    } while (remaining);
    return result;
}

static void IntersectCollisionPolygons(const ModelGeometry &geometry, const CollisionVertex *output, CollisionRay &record, ModelInstance *object)
{
    FUNCTION_MARKER(0x80084E24, "SLUS_003.73");
    uint32 required, rejected, count;
    uint32 header, flags, length;
    ModelPolygon *polygon;
    const CollisionVertex *vertices[4];
    uint32 packed[3], y[3], clip[4];
    uint32 cross0, cross1, cross2;
    const CollisionVertex *first_vertex;
    const SVECTOR *normal;
    uint32 start, numerator, endpoint, denominator, distance;
    uint32 direction, trigger_enabled, trigger_index, axis;

    length = record.len;
    xport_gte_write_control(11, 0);
    xport_gte_write_control(12, length);
    required = polygon_required_mask;
    rejected = polygon_rejected_mask;
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
            vertices[0] = output + polygon->v[0];
            clip[0] = vertices[0]->clip;
            vertices[1] = output + polygon->v[1];
            clip[1] = vertices[1]->clip;
            vertices[2] = output + polygon->v[2];
            clip[2] = vertices[2]->clip;
            vertices[3] = output + polygon->v[3];
            clip[3] = vertices[3]->clip;
            if (!(clip[0] & clip[1] & clip[2] & clip[3] & 0x60F))
            {
                for (axis = 0; axis < 3; ++axis)
                    packed[axis] = vertices[axis]->xy.xy;
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
                    packed[0] = vertices[3]->xy.xy;
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
                    xport_gte_write_data(0, ((uint16)normal->vx | ((uint32)(uint16)normal->vy << 16)));
                    xport_gte_write_data(1, ((uint16)normal->vz | ((uint32)(uint16)normal->pad << 16)));
                    first_vertex = output + polygon->v[0];
                    start = first_vertex->xy.xy;
                    endpoint = first_vertex->z;
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
                            collision_trigger = trigger_index;
                    }
                    else if ((sint32)(record.distance - distance) > 0)
                    {
                        record.hit.SetObject((object));
                        record.distance = (distance);
                        record.hit.SetPolygon((polygon));
                        record.endpoint = (endpoint);
                        for (axis = 0; axis < 3; ++axis)
                            xport_gte_write_data(25 + axis, record.endpoints[0].words[axis]);
                        direction = ((uint16)record.basis.m[2][0] | ((uint32)(uint16)record.basis.m[2][1] << 16));
                        endpoint = (uint32)(uint16)record.basis.m[2][2];
                        xport_gte_write_data(9, direction);
                        xport_gte_write_data(10, direction >> 16);
                        xport_gte_write_data(11, endpoint);
                        xport_gte_write_data(8, distance);
                        ExecuteGeometryCommand(0x1A0003E);
                        for (axis = 0; axis < 3; ++axis)
                            record.hit_pos.words[axis] = (xport_gte_read_data(25 + axis));
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
