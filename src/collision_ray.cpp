#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "model_bank_grid.h"
#include "model_instance.h"
#include "psx.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "scripted_level_object.h"
#include "collision.h"
#include "global.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "model_bank.h"
#include "resource_script.h"
#include "model_projection.h"
#include "player.h"
#include "collision_ray_internal.h"

CollisionVertex *col_vertices;
uint32 col_vertex_capacity;

static CollisionRay shared_col_ray;
SVECTOR col_normal;
MATRIX col_matrix;
uint32 col_trigger;
CollisionQueryOptions col_query;
static uint16 col_generation = 0xFFFF;
uint32 polygon_required_mask;
uint32 polygon_rejected_mask;

void CollisionRay::ResetSharedState()
{
    free(col_vertices);
    col_vertices = 0;
    col_vertex_capacity = 0;
    shared_col_ray = CollisionRay();
    col_normal = SVECTOR();
    memset(&col_matrix, 0, sizeof(col_matrix));
    col_trigger = 0;
    CollisionQueryOptions initial = {false, false, false, false};
    col_query = initial;
    col_generation = 0xFFFF;
    polygon_required_mask = 0;
    polygon_rejected_mask = 0;
}

CollisionRay &CollisionRay::GetShared()
{
    return shared_col_ray;
}

static uint32 ReadColSqSum()
{
    const uint32 x = xport_gte_read_data(25);
    const uint32 y = xport_gte_read_data(26);
    const uint32 z = xport_gte_read_data(27);
    return x + y + z;
}

static void SqColVector(const Vector3 &vector)
{
    xport_gte_write_data(9, (uint32)vector.x);
    xport_gte_write_data(10, (uint32)vector.y);
    xport_gte_write_data(11, (uint32)vector.z);
    xport_gte_execute(0xA00428);
}

static uint32 ReadColLeadingBits(uint32 val)
{
    xport_gte_write_data(30, val);
    return xport_gte_read_data(31);
}

ModelInstance *CollisionRay::TraceSeg(const Vector3 &start, const Vector3 &end, ModelInstance *list)
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    FUNCTION_MARKER(0x80067508, "SLUS_003.73");
    Vector3 hit;
    ModelInstance *result = CollisionRay::IntersectSpheres(start, end, &hit, list, 0);
    if (!result)
    {
        shared_ray.endpoints[0] = start;
        shared_ray.endpoints[1] = end;
        shared_ray.Initialize();
        ModelInstance *objects = visual_lists.solids;
        col_query.reject_surface40 = true;
        CollisionRay::VisitObjects(objects, shared_ray);
        result = shared_ray.hit.GetObject();
        col_query.reject_surface40 = false;
    }
    return result;
}

bool CollisionRay::IsSegVisible(const Vector3 &start, const Vector3 &end)
{
    FUNCTION_MARKER(0x800679A4, "SLUS_003.73");
    CollisionRay col;
    col.endpoints[0] = start;
    col.endpoints[1] = end;
    col.Initialize();
    col.Query(1);
    return col.hit.GetObject() == 0;
}

sint32 CollisionRay::FindGroundHeight(const Vector3 &pos, uint32 below, uint32 above)
{
    FUNCTION_MARKER(0x80067A18, "SLUS_003.73");
    CollisionRay col;
    col.endpoints[0].x = pos.x;
    col.endpoints[0].y = (uint32)pos.y - (below << 12);
    col.endpoints[0].z = pos.z;
    col.endpoints[1].x = pos.x;
    col.endpoints[1].y = (uint32)pos.y + (above << 12);
    col.endpoints[1].z = pos.z;
    col.Initialize();
    col.triggers = false;
    col.Query(1);
    if (col.hit.GetObject())
    {
        if (col.n.vy < -3071)
            return col.hit_pos.y;
    }
    return -1;
}

void CollisionRay::AdvanceGeneration()
{
    FUNCTION_MARKER(0x8007BAB0, "SLUS_003.73");
    ++col_generation;
    if (!col_generation)
    {
        ModelInstance *scene_object = ModelBank::GetSceneObjects();
        col_generation = 1;
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

uint16 CollisionRay::Initialize()
{
    FUNCTION_MARKER(0x8007BB24, "SLUS_003.73");
    uint32 horizontal, horizontal_root, full, full_root;
    sint32 coord0, coord1, delta;
    uint32 normalization, shift, full_shift;
    sint16 sine, cosine;
    uint32 axis;
    sint16 dx, dy, dz;
    const sint32 *start = endpoints[0].comp, *end = endpoints[1].comp;
    MATRIX *matrix = &basis;
    MATRIX yaw = {0};

    distance = 0x7FFFFFFF;
    coord1 = endpoints[1].comp[0];
    coord0 = endpoints[0].comp[0];
    hit.SetObject(0);
    endpoint = 0xFFFFFFFF;
    triggers = false;
    upward = false;
    delta = (sint32)((uint32)coord1 - (uint32)coord0) >> 12;
    dx = delta;
    horizontal = dx * dx;
    coord1 = endpoints[1].comp[1];
    coord0 = endpoints[0].comp[1];
    delta = (sint32)((uint32)coord1 - (uint32)coord0) >> 12;
    dy = delta;
    coord1 = endpoints[1].comp[2];
    coord0 = endpoints[0].comp[2];
    delta = (sint32)((uint32)coord1 - (uint32)coord0) >> 12;
    dz = delta;
    horizontal += dz * dz;
    if (horizontal)
    {
        normalization = ReadColLeadingBits(horizontal) - 1;
        shift = (uint32)((sint32)normalization >> 1);
        horizontal_root = LookupDistanceRoot(horizontal << (normalization & 30));
        sine = DivideSignedWord((uint32)dx << ((shift + 12) & 31), horizontal_root);
        cosine = DivideSignedWord((uint32)dz << ((shift + 12) & 31), horizontal_root);
        yaw.m[0][0] = cosine;
        yaw.m[0][1] = 0;
        yaw.m[0][2] = -sine;
        yaw.m[1][0] = 0;
        yaw.m[1][1] = ONE;
        yaw.m[1][2] = 0;
        yaw.m[2][0] = sine;
        yaw.m[2][1] = 0;
        yaw.m[2][2] = cosine;
        full = horizontal + dy * dy;
        normalization = ReadColLeadingBits(full) - 1;
        full_shift = (uint32)((sint32)normalization >> 1);
        full_root = LookupDistanceRoot(full << (normalization & 30));
        cosine = DivideSignedWord(horizontal_root << 12, full_root << ((shift - full_shift) & 31));
        len = (sint32)full_root >> (full_shift & 31);
        sine = DivideSignedWord((uint32)dy << ((full_shift + 12) & 31), full_root);
        basis.m[0][0] = ONE;
        basis.m[0][1] = 0;
        basis.m[0][2] = 0;
        basis.m[1][0] = 0;
        basis.m[1][1] = cosine;
        basis.m[1][2] = -sine;
        basis.m[2][0] = 0;
        basis.m[2][1] = sine;
        basis.m[2][2] = cosine;
        MulMatrix0(matrix, &yaw, matrix);
    }
    else
    {
        sine = dy < 0 ? -ONE : ONE;
        len = dy < 0 ? -dy : dy;
        if (dy >= 0)
            upward = true;
        basis.m[0][0] = ONE;
        basis.m[0][1] = 0;
        basis.m[0][2] = 0;
        basis.m[1][0] = 0;
        basis.m[1][1] = 0;
        basis.m[2][0] = 0;
        basis.m[2][2] = 0;
        basis.m[1][2] = -sine;
        basis.m[2][1] = sine;
    }
    SetRotMatrix(matrix);
    coord0 = endpoints[0].comp[0];
    coord1 = endpoints[1].comp[0];
    if (coord0 < coord1)
        min.comp[0] = coord0;
    else
    {
        coord0 = endpoints[0].comp[0];
        min.comp[0] = coord1;
        coord1 = coord0;
    }
    max.comp[0] = coord1;
    for (axis = 1; axis < 3; ++axis)
    {
        coord0 = start[axis];
        coord1 = end[axis];
        if (coord0 < coord1)
        {
            min.comp[axis] = coord0;
            coord1 = end[axis];
        }
        else
        {
            min.comp[axis] = coord1;
            coord1 = start[axis];
        }
        max.comp[axis] = coord1;
    }
    CollisionRay::AdvanceGeneration();
    generation = col_generation;
    return col_generation;
}

void CollisionRay::VisitObjects(ModelInstance *list, CollisionRay &rec)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007BEA0, "SLUS_003.73");
    uint16 generation;
    if (!list)
        return;
    if (!rec.len)
        return;
    SetRotMatrix(&rec.basis);
    generation = rec.generation;
    CollisionRay::MarkLinkedCandidates(list, rec.endpoints, generation);
    do
    {
        generation = rec.generation;
        if (list->generation != generation)
        {
            list->generation = generation;
            (void)list->IntersectCol(rec);
        }
        list = list->next;
    } while (list);
}

void CollisionRay::VisitCell(const ModelGridCell &cell, CollisionRay &rec)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007BF2C, "SLUS_003.73");
    ModelInstance *object;
    uint16 generation;
    if (!cell.present)
        return;
    if (!rec.len)
        return;
    SetRotMatrix(&rec.basis);
    generation = rec.generation;
    CollisionRay::MarkArrayCandidates(cell.objects, cell.count, rec.endpoints, generation);
    uint32 count = cell.count;
    ModelInstance *const *objects = cell.objects;
    if (!count)
        return;
    do
    {
        object = *objects;
        generation = rec.generation;
        if (object->active && object->generation != generation)
        {
            object->generation = generation;
            object = *objects;
            (void)object->IntersectCol(rec);
        }
        ++objects;
        --count;
    } while (count);
}

bool CollisionRay::ResolveHitNormal()
{
    FUNCTION_MARKER(0x8007BFD4, "SLUS_003.73");
    ModelInstance *object = hit.GetObject();
    if (!object)
        return false;
    object->angles.BuildRotationMatrix(col_matrix);
    SetRotMatrix(&col_matrix);
    const SVECTOR *normal = &col_normal;
    xport_gte_write_data(0, ((uint16)normal->vx | ((uint32)(uint16)normal->vy << 16)));
    xport_gte_write_data(1, ((uint16)normal->vz | ((uint32)(uint16)normal->pad << 16)));
    xport_gte_execute(0x486012);
    uint32 normal_x = xport_gte_read_data(9);
    uint32 normal_y = xport_gte_read_data(10);
    uint32 normal_z = xport_gte_read_data(11);
    n.vx = normal_x;
    n.vy = normal_y;
    n.vz = normal_z;
    return true;
}

CollisionVertex *GetColVertices(uint32 count)
{
    if (count > col_vertex_capacity)
    {
        if (count > (size_t)-1 / sizeof(*col_vertices))
        {
            GameDiagnostic(("Collision vertex allocation size overflow\n"));
            abort();
        }
        void *memory = realloc(col_vertices, count * sizeof(*col_vertices));
        if (!memory)
        {
            GameDiagnostic(("Cannot allocate collision vertices\n"));
            abort();
        }
        col_vertices = (CollisionVertex *)memory;
        col_vertex_capacity = count;
    }
    return col_vertices;
}

ModelInstance *CollisionRay::IntersectSpheres(const Vector3 &start, const Vector3 &end, Vector3 *output, ModelInstance *list, ModelInstance *excluded)
{
    FUNCTION_MARKER(0x8007C398, "SLUS_003.73");
    ModelBorrow borrow;
    Vector3 difference, vector, shifted, dir;
    Vector3 min, max;
    uint32 shift = 12;
    sint32 best = 0x7FFFFFFF;
    ModelInstance *closest = 0;
    difference.Subtract(end, start);
    vector.ShiftRight(difference, shift);
    SqColVector(vector);
    const sint32 len = LookupDistanceRoot(ReadColSqSum());
    if (!len)
        return 0;
    shifted.ShiftLeft(vector, shift);
    dir.Divide(shifted, len);
    xport_gte_write_control(0, (uint32)dir.x);
    xport_gte_write_control(2, (uint32)dir.y);
    xport_gte_write_control(4, (uint32)dir.z);
    const uint32 packed_xy = (uint16)(uint32)dir.x | ((uint32)(uint16)(uint32)dir.y << 16);
    const uint32 packed_z = (uint16)(uint32)dir.z;
    xport_gte_write_control(8, packed_xy);
    xport_gte_write_control(9, packed_z);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        const sint32 coord0 = start.comp[axis];
        const sint32 coord1 = end.comp[axis];
        if (coord0 < coord1)
        {
            min.comp[axis] = coord0;
            max.comp[axis] = coord1;
        }
        else
        {
            min.comp[axis] = coord1;
            max.comp[axis] = coord0;
        }
    }
    for (ModelInstance *object = list; object; object = object->next)
    {
        VisualObject &visual = *static_cast<VisualObject *>(object->owner);
        if (visual.lifecycle_flags & 64)
            continue;
        if (object == excluded)
            continue;
        uint32 radius = visual.col_radius << 12;
        const uint32 x = object->pos.x;
        if ((sint32)(x + radius) < min.x || max.x < (sint32)(x - radius))
            continue;
        const uint32 y = object->pos.y;
        if ((sint32)(y + radius) < min.y || max.y < (sint32)(y - radius))
            continue;
        const uint32 z = object->pos.z;
        if ((sint32)(z + radius) < min.z || max.z < (sint32)(z - radius))
            continue;
        difference.Subtract(object->pos, start);
        vector.ShiftRight(difference, shift);
        xport_gte_write_data(11, (uint32)vector.z);
        xport_gte_write_data(9, (uint32)vector.x);
        xport_gte_write_data(10, (uint32)vector.y);
        xport_gte_execute(0x178000C);
        xport_gte_execute(0xA00428);
        radius = visual.col_radius;
        const sint32 sq_radius = radius * radius;
        sint32 sq = ReadColSqSum();
        if (sq >= sq_radius)
            continue;
        const uint32 packed_delta = (uint16)(uint32)vector.x | ((uint32)(uint16)(uint32)vector.y << 16);
        xport_gte_write_data(0, packed_delta);
        xport_gte_write_data(1, (uint32)vector.z);
        xport_gte_execute(0x4A6012);
        const sint32 proj = xport_gte_read_data(25);
        if (proj >= best)
            continue;
        bool test_endpoint = false;
        if (proj < 0)
        {
            SqColVector(vector);
            test_endpoint = true;
        }
        else if (len < proj)
        {
            difference.Subtract(object->pos, end);
            vector.ShiftRight(difference, shift);
            SqColVector(vector);
            test_endpoint = true;
        }
        if (test_endpoint)
        {
            sq = ReadColSqSum();
            if (sq >= sq_radius)
                continue;
        }
        closest = object;
        best = proj;
    }
    if (!closest)
        return 0;
    xport_gte_write_data(9, (uint32)dir.x);
    xport_gte_write_data(10, (uint32)dir.y);
    xport_gte_write_data(11, (uint32)dir.z);
    xport_gte_write_data(8, best);
    xport_gte_execute(0x198003D);
    for (uint32 axis = 0; axis < 3; ++axis)
        output->comp[axis] = xport_gte_read_data(25 + axis);
    output->ShiftLeft(shift);
    output->Add(start);
    return closest;
}

bool CollisionRay::Query(uint32 include_objects)
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
    sint32 cell_idx_z;
    sint32 cell_z;
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
    sint32 cross_z;
    sint32 initial_error;
    sint32 error;
    sint32 step_x;
    sint32 step_z;
    uint32 grid_idx;
    polygon_rejected_mask = 0;
    polygon_required_mask = 0xFFFFFFFF;
    if (col_query.reject_surface40)
        polygon_rejected_mask = 0x400000;
    if (!col_query.allow_surface20)
        polygon_rejected_mask ^= 0x200000;
    if (col_query.require_surface10)
        polygon_required_mask = 0xFFEFFFFF;
    if (col_query.require_surface02)
        polygon_required_mask ^= 0x20000;
    if (include_objects)
        CollisionRay::VisitObjects(visual_lists.solids, *this);
    for (grid_idx = 0; grid_idx < 2; ++grid_idx)
    {
        // Collision scripts can replace cells while this query is running
        const ModelBankGrid &grid = ModelBankGrid::Get(grid_idx);
        if (!grid.active)
            goto NextGrid;
        start_x = endpoints[0].x;
        end_x = endpoints[1].x;
        start_z = endpoints[0].z;
        min_x = grid.min_x;
        end_z = endpoints[1].z;

        max_x = grid.max_x;
        min_z = grid.min_z;
        max_z = grid.max_z;
        cell_size = grid.cell_size;
        if (start_x < min_x && end_x < min_x)
            goto NextGrid;
        if ((max_x < start_x && max_x < end_x) || (start_z < min_z && end_z < min_z) || (max_z < start_z && max_z < end_z))
            goto NextGrid;
        if (start_x == end_x && start_z == end_z)
        {
            cell_x = DivideSignedWord((uint32)start_x - min_x, cell_size);
            grid_columns = grid.columns;
            initial_cell_z = DivideSignedWord((uint32)start_z - min_z, cell_size);
            if (cell_x == grid_columns)
                cell_x = (uint32)cell_x - 1;
            cell_idx_z = initial_cell_z;
            if (initial_cell_z != grid.rows)
                goto VisitFinalCell;
            cell_z = (uint32)initial_cell_z - 1;
        }
        else
        {
            if (start_x < min_x)
            {
                if (start_z >= end_z)
                    start_z = end_z + MultiplyDivideWord((uint32)end_x - min_x, (uint32)start_z - end_z, (uint32)end_x - start_x);
                else
                    start_z = start_z + MultiplyDivideWord((uint32)min_x - start_x, (uint32)end_z - start_z, (uint32)end_x - start_x);
                start_x = min_x;
            }
            if (end_x < min_x)
            {
                if (end_z >= start_z)
                    end_z = start_z + MultiplyDivideWord((uint32)start_x - min_x, (uint32)end_z - start_z, (uint32)start_x - end_x);
                else
                    end_z = end_z + MultiplyDivideWord((uint32)min_x - end_x, (uint32)start_z - end_z, (uint32)start_x - end_x);
                end_x = min_x;
            }
            if (max_x < start_x)
            {
                if (end_z >= start_z)
                    start_z = start_z + MultiplyDivideWord((uint32)start_x - max_x, (uint32)end_z - start_z, (uint32)start_x - end_x);
                else
                    start_z = end_z + MultiplyDivideWord((uint32)max_x - end_x, (uint32)start_z - end_z, (uint32)start_x - end_x);
                start_x = max_x;
            }
            if (max_x < end_x)
            {
                if (start_z >= end_z)
                    end_z = end_z + MultiplyDivideWord((uint32)end_x - max_x, (uint32)start_z - end_z, (uint32)end_x - start_x);
                else
                    end_z = start_z + MultiplyDivideWord((uint32)max_x - start_x, (uint32)end_z - start_z, (uint32)end_x - start_x);
                end_x = max_x;
            }
            if (start_z < min_z)
            {
                if (start_x >= end_x)
                    start_x = end_x + MultiplyDivideWord((uint32)end_z - min_z, (uint32)start_x - end_x, (uint32)end_z - start_z);
                else
                    start_x = start_x + MultiplyDivideWord((uint32)min_z - start_z, (uint32)end_x - start_x, (uint32)end_z - start_z);
                start_z = min_z;
            }
            if (end_z < min_z)
            {
                if (end_x >= start_x)
                    end_x = start_x + MultiplyDivideWord((uint32)start_z - min_z, (uint32)end_x - start_x, (uint32)start_z - end_z);
                else
                    end_x = end_x + MultiplyDivideWord((uint32)min_z - end_z, (uint32)start_x - end_x, (uint32)start_z - end_z);
                end_z = min_z;
            }
            if (max_z < start_z)
            {
                if (end_x >= start_x)
                    start_x = start_x + MultiplyDivideWord((uint32)start_z - max_z, (uint32)end_x - start_x, (uint32)start_z - end_z);
                else
                    start_x = end_x + MultiplyDivideWord((uint32)max_z - end_z, (uint32)start_x - end_x, (uint32)start_z - end_z);
                start_z = max_z;
            }
            if (max_z < end_z)
            {
                if (start_x >= end_x)
                    end_x = end_x + MultiplyDivideWord((uint32)end_z - max_z, (uint32)start_x - end_x, (uint32)end_z - start_z);
                else
                    end_x = start_x + MultiplyDivideWord((uint32)max_z - start_z, (uint32)end_x - start_x, (uint32)end_z - start_z);
                end_z = max_z;
            }
            delta_x = (uint32)end_x - start_x;
            if (delta_x >= 0)
            {
                step_x = 1;
            }
            else
            {
                step_x = -1;
                delta_x = (uint32)start_x - end_x;
            }
            delta_z = (uint32)end_z - start_z;
            if (delta_z >= 0)
            {
                step_z = 1;
            }
            else
            {
                step_z = -1;
                delta_z = (uint32)start_z - end_z;
            }
            target_cell_x = DivideSignedWord((uint32)end_x - min_x, cell_size);
            initial_cell_x = DivideSignedWord((uint32)start_x - min_x, cell_size);
            remainder_x = RemainderSignedWord((uint32)start_x - min_x, cell_size);
            cell_z = DivideSignedWord((uint32)start_z - min_z, cell_size);
            remainder_z = RemainderSignedWord((uint32)start_z - min_z, cell_size);

            target_cell_z = DivideSignedWord((uint32)end_z - min_z, cell_size);
            remaining_x = remainder_x;
            grid_columns = grid.columns;
            remaining_z = remainder_z;
            if (initial_cell_x == grid_columns)
            {
                initial_cell_x = (uint32)initial_cell_x - 1;
                remaining_x = (uint32)remaining_x + cell_size;
            }
            grid_rows = grid.rows;
            if (cell_z == grid_rows)
            {
                cell_z = (uint32)cell_z - 1;
                remaining_z = (uint32)remainder_z + cell_size;
            }
            if (target_cell_x == grid_columns)
                target_cell_x = (uint32)target_cell_x - 1;
            if (target_cell_z == grid_rows)
                target_cell_z = (uint32)target_cell_z - 1;
            cross_x = MultiplyDivideWord(delta_x, remaining_z, cell_size);
            cross_z = MultiplyDivideWord(delta_z, remaining_x, cell_size);
            cell_x = initial_cell_x;
            if (start_x >= end_x)
                initial_error = -(uint32)cross_z;
            else
                initial_error = (uint32)cross_z - delta_z;
            if (start_z >= end_z)
                error = (uint32)initial_error + cross_x;
            else
                error = (uint32)initial_error + delta_x - cross_x;
            while (cell_x != target_cell_x || cell_z != target_cell_z)
            {
                if (cell_x < 0)
                    goto NextGrid;

                if (cell_x >= grid.columns || cell_z < 0)
                    break;
                if (cell_z >= grid.rows)
                    goto CheckFinalCell;
                CollisionRay::VisitCell(grid.cells[cell_x][cell_z], *this);
                if (error < 0)
                {
                    error = (uint32)error + delta_x;
                    cell_z = (uint32)cell_z + step_z;
                }
                else
                {
                    error = (uint32)error - delta_z;
                    cell_x = (uint32)cell_x + step_x;
                }
            }

            if (cell_x < 0)
                goto NextGrid;

        CheckFinalCell:
            if (cell_x >= grid.columns || cell_z < 0)
                goto NextGrid;

            if (cell_z >= grid.rows)
                goto NextGrid;
        }
        cell_idx_z = cell_z;
    VisitFinalCell:
        CollisionRay::VisitCell(grid.cells[cell_x][cell_idx_z], *this);

    NextGrid:;
    }
    return ResolveHitNormal();
}

void CollisionRay::LoadBounds(const Vector3 bounds[2], Vector3 &low, Vector3 &high, uint32 &flip_bits)
{
    FUNCTION_MARKER(0x80084778, "SLUS_003.73");
    Vector3 saved[2];
    uint32 axis;
    saved[0] = bounds[0];
    saved[1] = bounds[1];
    for (axis = 0; axis < 3; ++axis)
    {
        low.comp[axis] = saved[0].comp[axis] >> 12;
        high.comp[axis] = saved[1].comp[axis] >> 12;
    }
    flip_bits = 0;
    CollisionRay::NormalizeBounds(low, high, flip_bits);
}

void CollisionRay::NormalizeBounds(Vector3 &low, Vector3 &high, uint32 &flip_bits)
{
    FUNCTION_MARKER(0x800847AC, "SLUS_003.73");
    // Q12 endpoints and signed GTE IR values bound the differences
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
    {
        sint32 delta = high.comp[axis] - low.comp[axis];
        if (delta < 0)
        {
            sint32 tmp = low.comp[axis];
            low.comp[axis] = high.comp[axis];
            high.comp[axis] = tmp;
            delta = high.comp[axis] - low.comp[axis];
            flip_bits ^= 1 << axis;
        }
        xport_gte_write_control(axis * 2, delta);
    }
}

void CollisionRay::PrepareBounds(const ModelGeometry &geometry, uint32 mirror_flags, const CollisionBounds &reference, const Vector3 &translation, CollisionBounds &result)
{
    FUNCTION_MARKER(0x80084814, "SLUS_003.73");
    Vector3 minimum, maximum, offset, center;
    uint32 axis;
    // Snapshot the original register inputs before exposing the output
    for (axis = 0; axis < 3; ++axis)
    {
        offset.comp[axis] = translation.comp[axis];
        center.comp[axis] = reference.minimum.comp[axis] + reference.maximum.comp[axis];
    }
    for (axis = 0; axis < 3; ++axis)
    {
        minimum.comp[axis] = geometry.bounds[axis].max + offset.comp[axis] + 2;
        maximum.comp[axis] = geometry.bounds[axis].min + offset.comp[axis] - 2;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        if (mirror_flags & (1 << axis))
        {
            sint32 mirrored_minimum = center.comp[axis] - maximum.comp[axis];
            maximum.comp[axis] = center.comp[axis] - minimum.comp[axis];
            minimum.comp[axis] = mirrored_minimum;
        }
    }
    result.minimum = minimum;
    result.maximum = maximum;
}

bool CollisionRay::IntersectBounds(const Vector3 &query_low, const Vector3 &query_high, const Vector3 &object_low, const Vector3 &object_high)
{
    FUNCTION_MARKER(0x800848D0, "SLUS_003.73");
    Vector3 lower_delta, upper_delta, cross;
    sint32 component0, component1;
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        if (query_high.comp[axis] - object_high.comp[axis] < 0)
            return 0;
    for (axis = 0; axis < 3; ++axis)
    {
        lower_delta.comp[axis] = object_low.comp[axis] - query_low.comp[axis];
        if (lower_delta.comp[axis] < 0)
            return 0;
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, lower_delta.comp[axis]);
    xport_gte_execute(0x170000C);
    for (axis = 0; axis < 3; ++axis)
        upper_delta.comp[axis] = object_high.comp[axis] - query_low.comp[axis];
    for (axis = 0; axis < 3; ++axis)
        cross.comp[axis] = xport_gte_read_data(25 + axis);
    if (cross.comp[0] < 0)
    {
        if (cross.comp[1] < 0)
            goto CheckX;
        xport_gte_write_data(9, upper_delta.comp[0]);
        xport_gte_write_data(10, upper_delta.comp[1]);
        xport_gte_write_data(11, lower_delta.comp[2]);
        xport_gte_execute(0x170000C);
        component0 = xport_gte_read_data(9);
        component1 = xport_gte_read_data(10);
        return component0 >= 0 && component1 <= 0;
    }
    if (cross.comp[2] < 0)
    {
        xport_gte_write_data(9, upper_delta.comp[0]);
        xport_gte_write_data(10, lower_delta.comp[1]);
        xport_gte_write_data(11, upper_delta.comp[2]);
        xport_gte_execute(0x170000C);
        component0 = xport_gte_read_data(9);
        component1 = xport_gte_read_data(11);
        return component0 <= 0 && component1 >= 0;
    }
CheckX:
    xport_gte_write_data(9, lower_delta.comp[0]);
    xport_gte_write_data(10, upper_delta.comp[1]);
    xport_gte_write_data(11, upper_delta.comp[2]);
    xport_gte_execute(0x170000C);
    component0 = xport_gte_read_data(10);
    component1 = xport_gte_read_data(11);
    return component0 >= 0 && component1 <= 0;
}

void CollisionRay::MarkLinkedCandidates(ModelInstance *object, const Vector3 bounds[2], uint16 mark)
{
    FUNCTION_MARKER(0x800849FC, "SLUS_003.73");
    uint32 scale, axis, flip_bits, idx;
    uint16 flags, generation;
    sint32 delta;
    Vector3 translation = {0};
    Vector3 lower, upper, object_min, object_max;
    CollisionBounds reference, result;
    MATRIX matrix = {0};
    do
    {
        flags = object->flags;
        generation = object->generation;
        idx = object->model_idx;
        if (flags & 0x21)
            object->generation = mark;
        else if (generation != mark)
        {
            const ModelGeometry &geometry = ModelBank::GetGeometry(object->bank, idx);
            CollisionRay::LoadBounds(bounds, lower, upper, flip_bits);
            for (axis = 0; axis < 3; ++axis)
            {
                delta = object->pos.comp[axis] >> 12;
                lower.comp[axis] -= delta;
                upper.comp[axis] -= delta;
            }
            if (object->angles.x || object->angles.y || object->angles.z)
            {
                object->angles.BuildRotationMatrix(matrix);
                TransposeRotationMatrix(matrix);
                SetRotMatrix(&matrix);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, lower.comp[axis]);
                xport_gte_execute(0x49E012);
                for (axis = 0; axis < 3; ++axis)
                    lower.comp[axis] = xport_gte_read_data(9 + axis);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, upper.comp[axis]);
                xport_gte_execute(0x49E012);
                for (axis = 0; axis < 3; ++axis)
                    upper.comp[axis] = xport_gte_read_data(9 + axis);
                CollisionRay::NormalizeBounds(lower, upper, flip_bits);
            }
            reference.minimum = lower;
            reference.maximum = upper;
            CollisionRay::PrepareBounds(geometry, flip_bits, reference, translation, result);
            object_min = result.minimum;
            object_max = result.maximum;
            if (object->flags & 0x200)
            {
                for (axis = 0; axis < 3; ++axis)
                {
                    scale = object->scale.comp[axis];
                    // Keep the low product word before the Q12 shift
                    sint32 product = (uint32)object_min.comp[axis] * scale;
                    object_min.comp[axis] = product >> 12;
                    product = (uint32)object_max.comp[axis] * scale;
                    object_max.comp[axis] = product >> 12;
                }
            }
            if (!CollisionRay::IntersectBounds(lower, upper, object_min, object_max))
                object->generation = mark;
        }
        object = object->next;
    } while (object);
}

void CollisionRay::MarkArrayCandidates(ModelInstance *const *objects, uint32 count, const Vector3 bounds[2], uint16 mark)
{
    FUNCTION_MARKER(0x80084C50, "SLUS_003.73");
    Vector3 lower, upper, object_min, object_max;
    ModelInstance *object = count ? *objects : 0;
    uint32 flip_bits, axis, idx;
    Vector3 translation;
    CollisionBounds reference, result;
    if (!object)
        return;
    CollisionRay::LoadBounds(bounds, lower, upper, flip_bits);
    do
    {
        if (!object->active)
        {
            ++objects;
            --count;
            object = count ? *objects : 0;
            continue;
        }
        uint16 flags = object->flags;
        uint16 generation = object->generation;
        idx = object->model_idx;
        if (flags & 0x21)
            object->generation = mark;
        else if (generation != mark)
        {
            const ModelGeometry &geometry = ModelBank::GetGeometry(object->bank, idx);
            for (axis = 0; axis < 3; ++axis)
                translation.comp[axis] = object->pos.comp[axis] >> 12;
            reference.minimum = lower;
            reference.maximum = upper;
            CollisionRay::PrepareBounds(geometry, flip_bits, reference, translation, result);
            object_min = result.minimum;
            object_max = result.maximum;
            if (!CollisionRay::IntersectBounds(lower, upper, object_min, object_max))
                object->generation = mark;
        }
        ++objects;
        --count;
        object = count ? *objects : 0;
    } while (object);
}
