#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "global.h"
#include "game_heap.h"
#include "traversal_rail.h"
#include "player.h"
#include "effect_lifecycle.h"
#include "collision.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "effect_object.h"

uint8 TraversalRail::GetBrightness(sint32 distance)
{
    sint32 result;
    if (distance < 1025)
        result = 255;
    else if (distance < 4096)
        result = 255 - (255 * (distance - 1024)) / 3072;
    else
        result = 0;
    return result < 32 ? 32 : result;
}

TraversalRail::TraversalRail(RecordCursor data, uint32 idx) : VisualObject(), heading(0), length(0)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80018E74, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        model.pos.comp[axis] = (data + axis * 4).Read32() << 12;
    rec_idx = idx;
    RecordCursor cursor(GetRecPayload(idx));
    ReadRecPos(&target, (cursor + 2).Read16());
    Initialize();
}

uint32 TraversalRail::Initialize()
{
    FUNCTION_MARKER(0x80018F90, "SLUS_003.73");
    model.flags |= 1;
    lifecycle_flags &= 0xFFEF;
    AddToList(visual_lists.actors);
    heading = (1024 - (uint32)ratan2((sint32)((uint32)target.z - model.pos.z), (sint32)((uint32)target.x - model.pos.x))) & 4095;
    Vector3 delta;
    for (uint32 axis = 0; axis < 3; ++axis)
        delta.comp[axis] = (sint32)((uint32)target.comp[axis] - model.pos.comp[axis]) >> 12;
    NormalizeShortVector(delta, dir);
    sint32 left = model.pos.x;
    sint32 right = target.x;
    if (left < right)
    {
        bounds.min_x = left;
        bounds.max_x = right;
    }
    else
    {
        bounds.min_x = right;
        bounds.max_x = model.pos.x;
    }
    left = model.pos.z;
    right = target.z;
    if (left < right)
    {
        bounds.min_z = left;
        bounds.max_z = right;
    }
    else
    {
        bounds.min_z = right;
        bounds.max_z = model.pos.z;
    }
    // Expanded coordinate words retain the original wrapping arithmetic
    bounds.min_x = (uint32)bounds.min_x - 0x100000;
    bounds.max_x = (uint32)bounds.max_x + 0x100000;
    bounds.max_z = (uint32)bounds.max_z + 0x100000;
    bounds.min_z = (uint32)bounds.min_z - 0x100000;
    delta.Subtract(target, model.pos);
    length = delta.Len();
    sint32 count = length / 512;
    if (count < 3)
        count = 3;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(100);
    if (!allocation)
    {
        GameDiagnostic(("Traversal segment allocation failed\n"));
        abort();
    }
    seg = new (allocation) SegmentEffect(count);
    seg->SetEndpoints(&model.pos, &target);
    seg->persistent = true;
    seg->Enable();
    kind = 400;
    return 400;
}

TraversalRail::~TraversalRail()
{
    if (seg)
        seg->Destroy(3);
}

uint32 TraversalRail::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001919C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (seg)
        seg->Destroy(3);
    seg = 0;
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

sint32 TraversalRail::ForwardDot(const Vector3 &delta) const
{
    uint32 x = (uint32)(delta.x >> 12) * (uint32)dir.x;
    uint32 z = (uint32)(delta.z >> 12) * (uint32)dir.z;
    return x + z;
}

bool TraversalRail::Advance(const Vector3 &pos, uint32 scale, Vector3 &output) const
{
    FUNCTION_MARKER(0x8001922C, "SLUS_003.73");
    Vector3 scaled, vector, delta;
    scaled.Scale(scale, dir);
    vector.Add(pos, scaled);
    // The candidate is published even when the following bounds test fails
    output = vector;
    delta.Subtract(output, model.pos);
    sint32 distance = delta.Len();
    return distance < length && ForwardDot(delta) >= 0;
}

bool TraversalRail::Project(const Vector3 &pos, Vector3 &output) const
{
    FUNCTION_MARKER(0x80019318, "SLUS_003.73");
    Vector3 delta, scaled, vector;
    delta.Subtract(pos, model.pos);
    sint32 distance = delta.Len();
    if (distance >= length || ForwardDot(delta) < 0)
        return false;
    scaled.Multiply(dir, distance);
    vector.Add(model.pos, scaled);
    output = vector;
    return true;
}

void TraversalRail::UpdateSegColor()
{
    ModelInstance *camera = visual_lists.cameras;
    uint8 color = TraversalRail::GetBrightness(model.pos.Distance(camera->pos));
    if (seg)
    {
        seg->rgbc.b = color;
        seg->rgbc.g = color;
        seg->rgbc.r = color;
        for (sint32 i = 0; i < seg->point_count; ++i)
        {
            EffectPoint *point = seg->points + i;
            camera = visual_lists.cameras;
            color = TraversalRail::GetBrightness(point->pos.Distance(camera->pos));
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
    if (bounds.min_x >= player->pos.x || player->pos.x >= bounds.max_x || bounds.min_z >= player->pos.z || player->pos.z >= bounds.max_z)
        return;
    if (static_cast<Player *>(player->owner)->on_rail)
        return;
    Vector3 col;
    ModelInstance *hit = CollisionRay::IntersectSpheres(model.pos, target, &col, player, 0);
    player = visual_lists.players;
    if (hit == player)
        static_cast<Player *>(hit->owner)->rail = this;
}

uint32 TraversalRail::Update()
{
    FUNCTION_MARKER(0x800193F8, "SLUS_003.73");
    UpdateSegColor();
    UpdatePlayerContact();
    return 0;
}
