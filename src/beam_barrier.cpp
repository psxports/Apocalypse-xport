#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "beam_barrier.h"
#include "collision.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "effect_lifecycle.h"
#include "fading_particle.h"
#include "player.h"
#include "endpoint_beam.h"
#include "record_data.h"

BeamBarrier::BeamBarrier(RecordCursor data, uint32 idx, bool repulsion) : VisualObject(), repulsion(false), effect()
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80019DA0, "SLUS_003.73");
    model.pos.x = data.Read32() << 12;
    model.pos.y = (data + 4).Read32() << 12;
    uint32 z = (data + 8).Read32();
    rec_idx = idx;
    model.pos.z = z << 12;
    cursor = GetRecPayload(idx);
    ReadRecPos(&target, (cursor + 2).Read16());
    Initialize(repulsion);
}

BeamBarrier::BeamBarrier(const Vector3 *pos, const Vector3 *target) : VisualObject(), repulsion(false), effect()
{
    FUNCTION_MARKER(0x80019E48, "SLUS_003.73");
    model.pos = *pos;
    this->target = *target;
    Initialize(true);
}

void BeamBarrier::Initialize(bool repulsion)
{
    FUNCTION_MARKER(0x80019ED0, "SLUS_003.73");
    model.flags |= 1;
    lifecycle_flags &= 0xFFEF;
    AddToList(visual_lists.actors);
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
    Vector3 difference;
    difference.Subtract(target, model.pos);
    sint32 count = difference.Len();
    count /= 512;
    if (count < 4)
        count = 4;
    uint8 g = repulsion ? 32 : 128;
    uint8 b = repulsion ? 128 : 32;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(164);
    if (!allocation)
    {
        GameDiagnostic(("Beam barrier effect allocation failed\n"));
        abort();
    }
    effect = new (allocation) EndpointBeam(32, g, b, count, 700, 32, 48, 440, 110, 800, 147);
    effect->SetEndpoints(&model.pos, &target);
    effect->persistent = true;
    kind = 404;
    SetRepulsion(repulsion);
}

uint32 BeamBarrier::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001A0B4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (effect)
        effect->Destroy(3);
    effect = 0;
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void BeamBarrier::SetRepulsion(bool enabled)
{
    FUNCTION_MARKER(0x8001A144, "SLUS_003.73");
    repulsion = enabled;
}

uint32 BeamBarrier::Update()
{
    FUNCTION_MARKER(0x8001A14C, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ModelInstance *player = visual_lists.players;
    if (!player)
        return 0;
    sint32 coord = player->pos.x;
    if (bounds.min_x >= coord)
        return 0;
    if (coord >= bounds.max_x)
        return 0;
    coord = player->pos.z;
    if (bounds.min_z >= coord)
        return 0;
    if (coord >= bounds.max_z)
        return 0;
    Vector3 col;
    ModelInstance *hit = CollisionRay::IntersectSpheres(model.pos, target, &col, player, 0);
    player = visual_lists.players;
    if (hit != player)
        return hit != 0;
    const Vector3 zero = {0};
    static_cast<VisualObject *>(hit->owner)->Damage(10, &zero, 0);
    if (!repulsion)
        return 0;
    static_cast<Player *>(visual_lists.players->owner)->Player::ApplyBeamContactImpulse(*this);
    PrimitiveParticle::SetAngles(&zero_angles);
    ShortVector3 angles = {{{512, 4096, 0}}};
    PrimitiveParticle::SetSpread(&angles);
    PrimitiveParticle::SetSize(2);
    PrimitiveParticle::SetColor(32, 32, 128);
    PrimitiveParticle::SetColorStep(1, 1, 4);
    use_small_effect_pool = false;
    for (uint32 i = 0; i < 32; ++i)
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(88);
        if (allocation)
        {
            new (allocation) FadingParticle(&visual_lists.players->pos, 16, 8192, 32);
        }
    }
    use_small_effect_pool = true;
    return 1;
}

BeamBarrier::~BeamBarrier()
{
    if (effect)
        effect->Destroy(3);
}
