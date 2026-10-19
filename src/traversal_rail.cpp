#include <new>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "traversal_rail.h"
#include "player.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "collision.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "game_diagnostics.h"

static uint32 GetTraversalRailBrightness(uint32 distance)
{
    uint32 result;
    if ((sint32)distance < 1025)
        result = 255;
    else if ((sint32)distance < 4096)
        result = 255 - (255 * (distance - 1024)) / 3072;
    else
        result = 0;
    return (sint32)result < 32 ? 32 : result;
}

static void AddShortNeighbors(ShortVector3 &output, const ShortVector3 &left, const ShortVector3 &right)
{
    FUNCTION_MARKER(0x8006CAC8, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        output.words[axis] = left.words[axis] + right.words[axis];
}

TraversalRail::TraversalRail(const uint32 *data, uint32 index) : VisualObject(), heading(0), length(0)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80018E74, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        model.pos.words[axis] = data[axis] << 12;
    record_idx = index;
    RecordCursor cursor(GetRecordPayload(index));
    ReadRecordPosition(target.words, (cursor + 2).Read16());
    Initialize();
}

uint32 TraversalRail::Initialize()
{
    FUNCTION_MARKER(0x80018F90, "SLUS_003.73");
    model.flags |= 1;
    lifecycle_flags &= 0xFFEF;
    AddToList(visual_lists.actors);
    heading = (1024 - (uint32)ratan2((sint32)(target.words[2] - model.pos.words[2]), (sint32)(target.words[0] - model.pos.words[0]))) & 4095;
    Vector3 delta;
    for (uint32 axis = 0; axis < 3; ++axis)
        delta.words[axis] = (sint32)(target.words[axis] - model.pos.words[axis]) >> 12;
    NormalizeShortVector(delta.words, dir.words);
    for (uint32 axis = 0; axis < 2; ++axis)
    {
        uint32 coord = axis * 2;
        if ((sint32)model.pos.words[coord] < (sint32)target.words[coord])
        {
            bounds[axis * 2] = model.pos.words[coord];
            bounds[axis * 2 + 1] = target.words[coord];
        }
        else
        {
            bounds[axis * 2] = target.words[coord];
            bounds[axis * 2 + 1] = model.pos.words[coord];
        }
        bounds[axis * 2] -= 0x100000;
        bounds[axis * 2 + 1] += 0x100000;
    }
    SubtractVectors(delta.words, target.words, model.pos.words);
    length = MeasureVectorLength(delta.words);
    sint32 count = (sint32)length / 512;
    if (count < 3)
        count = 3;
    GameHeapBlock *allocation = AllocateEffectStorage(100);
    if (!allocation)
    {
        GameDiagnostic("Traversal segment allocation failed\n");
        abort();
    }
    segment = new (allocation) SegmentEffect(count);
    segment->SetEndpoints(model.pos.words, target.words);
    segment->persistent = 1;
    segment->Enable();
    kind = 400;
    return 400;
}

TraversalRail::~TraversalRail()
{
    if (segment)
        segment->Destroy(3);
}

uint32 TraversalRail::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001919C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (segment)
        segment->Destroy(3);
    segment = 0;
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseObjectStorage(this) : 0;
}

uint32 TraversalRail::ForwardDot(const Vector3 &delta) const
{
    uint32 x = (uint32)((sint32)delta.words[0] >> 12) * dir.words[0];
    uint32 z = (uint32)((sint32)delta.words[2] >> 12) * dir.words[2];
    return x + z;
}

bool TraversalRail::Advance(const Vector3 &pos, uint32 scale, Vector3 &output) const
{
    FUNCTION_MARKER(0x8001922C, "SLUS_003.73");
    Vector3 scaled, vector, delta;
    MultiplyScalarVector(scaled.words, scale, dir.words);
    AddVectors(vector.words, pos.words, scaled.words);
    // The candidate is published even when the following bounds test fails
    output = vector;
    SubtractVectors(delta.words, output.words, model.pos.words);
    uint32 distance = MeasureVectorLength(delta.words);
    return (sint32)distance < (sint32)length && (sint32)ForwardDot(delta) >= 0;
}

bool TraversalRail::Project(const Vector3 &pos, Vector3 &output) const
{
    FUNCTION_MARKER(0x80019318, "SLUS_003.73");
    Vector3 delta, scaled, vector;
    SubtractVectors(delta.words, pos.words, model.pos.words);
    uint32 distance = MeasureVectorLength(delta.words);
    if ((sint32)distance >= (sint32)length || (sint32)ForwardDot(delta) < 0)
        return false;
    MultiplyVector(scaled.words, dir.words, distance);
    AddVectors(vector.words, model.pos.words, scaled.words);
    output = vector;
    return true;
}

void TraversalRail::UpdateSegmentColor()
{
    ModelInstance *camera = visual_lists.cameras;
    uint32 color = GetTraversalRailBrightness(MeasurePositionDistance(model.pos.words, camera->pos.words));
    if (segment)
    {
        segment->rgbc.b = color;
        segment->rgbc.g = color;
        segment->rgbc.r = color;
        for (uint32 i = 0; i < segment->point_count; ++i)
        {
            EffectPoint *point = segment->points + i;
            camera = visual_lists.cameras;
            color = GetTraversalRailBrightness(MeasurePositionDistance(point->pos.words, camera->pos.words));
            point->rgbc.b = color;
            point->rgbc.g = color;
            point->rgbc.r = color;
        }
    }
}

void TraversalRail::UpdatePlayerContact()
{
    ModelBorrow borrow;
    ModelInstance *player = visual_lists.players;
    if ((sint32)bounds[0] >= (sint32)player->pos.words[0] || (sint32)player->pos.words[0] >= (sint32)bounds[1] || (sint32)bounds[2] >= (sint32)player->pos.words[2] || (sint32)player->pos.words[2] >= (sint32)bounds[3])
        return;
    if (static_cast<Player *>(player->owner)->on_rail)
        return;
    Vector3 collision;
    ModelInstance *hit = IntersectObjectSpheres(model.pos.words, target.words, collision.words, player, 0);
    player = visual_lists.players;
    if (hit == player)
        static_cast<Player *>(hit->owner)->rail = this;
}

uint32 TraversalRail::Update()
{
    FUNCTION_MARKER(0x800193F8, "SLUS_003.73");
    UpdateSegmentColor();
    UpdatePlayerContact();
    return 0;
}

FlexibleTraversalRail::FlexibleTraversalRail(const uint32 *data, uint32 index) : TraversalRail(data, index), waves(0), wave_count(segment->point_count - 1), deform_timer(0), wave_bank(0)
{
    FUNCTION_MARKER(0x800196D4, "SLUS_003.73");
    // The original reserves two banks of twenty-four short vectors
    if (wave_count > 24)
    {
        GameDiagnostic("Flexible rail wave count exceeds original capacity: %u\n", wave_count);
        abort();
    }
    waves = (ShortVector3 *)calloc(wave_count * 2, sizeof(*waves));
    if (!waves)
    {
        GameDiagnostic("Flexible rail wave allocation failed\n");
        abort();
    }
    kind = 401;
}

FlexibleTraversalRail::~FlexibleTraversalRail()
{
    free(waves);
}

uint32 FlexibleTraversalRail::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80019788, "SLUS_003.73");
    free(waves);
    waves = 0;
    uint32 result = TraversalRail::Destroy(0);
    return reason & 1 ? ReleaseObjectStorage(this) : result;
}

void FlexibleTraversalRail::ApplyRandomOffset(uint32 range)
{
    FUNCTION_MARKER(0x800197DC, "SLUS_003.73");
    uint32 index = RandomRange(segment->point_count - 1);
    uint32 bank = wave_bank;
    uint32 half_range = (sint32)range >> 1;
    uint32 x = RandomRange(range) - half_range;
    uint32 y = RandomRange(range) - half_range;
    uint32 z = RandomRange(range) - half_range;
    ShortVector3 &offset = waves[bank * wave_count + index];
    offset.components[2] = z;
    offset.components[0] = x;
    offset.components[1] = y;
    deform_timer = 64;
}

uint32 FlexibleTraversalRail::Update()
{
    FUNCTION_MARKER(0x800198A4, "SLUS_003.73");
    UpdateSegmentColor();
    if (deform_timer && segment)
    {
        --deform_timer;
        uint32 count = segment->point_count;
        for (uint32 i = 0; i < count - 1; ++i)
        {
            ShortVector3 &cur = waves[wave_bank * wave_count + i];
            const ShortVector3 &prev = waves[(wave_bank ^ 1) * wave_count + (i ? i - 1 : i + 1)];
            const ShortVector3 &next = waves[(wave_bank ^ 1) * wave_count + (i == count - 2 ? i - 1 : i + 1)];
            ShortVector3 neighbor;
            if (i && i != count - 2)
                AddShortNeighbors(neighbor, prev, next);
            else
                neighbor = i == 0 ? next : prev;
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                uint16 wave_delta = neighbor.words[axis] - (uint16)cur.components[axis];
                cur.components[axis] = wave_delta - (uint16)((sint16)wave_delta >> 3);
            }
        }
        Vector3 delta, step, pos;
        SubtractVectors(delta.words, target.words, model.pos.words);
        DivideVector(step.words, delta.words, count);
        pos = model.pos;
        segment->origin = pos;
        for (uint32 i = 0; i < count - 1; ++i)
        {
            AddVectors(pos.words, step.words);
            for (uint32 axis = 0; axis < 3; ++axis)
                pos.words[axis] += (uint32)(sint32)waves[wave_bank * wave_count + i].components[axis] << 12;
            segment->points[i].pos = pos;
        }
        segment->points[count - 1].pos = target;
        wave_bank ^= 1;
    }
    UpdatePlayerContact();
    return 0;
}
