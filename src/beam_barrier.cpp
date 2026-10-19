#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "object_initializers.h"

#include "visual_object.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "beam_barrier.h"
#include "game_object_allocation.h"
#include "collision.h"
#include "particle_settings.h"
#include "effect_lifecycle.h"
#include "fading_particle.h"
#include "player.h"
#include "beam_endpoint.h"
#include "record_data.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"

void BeamBarrier::SetMode(uint32 mode)
{
    FUNCTION_MARKER(0x8001A144, "SLUS_003.73");
    this->mode = mode;
}

void BeamBarrier::Initialize(uint32 mode)
{
    FUNCTION_MARKER(0x80019ED0, "SLUS_003.73");
    model.flags |= 1;
    lifecycle_flags &= 0xFFEF;
    AddToList(visual_lists.actors);
    uint32 left = model.pos.words[0];
    uint32 right = target.words[0];
    if ((sint32)left < (sint32)right)
    {
        bounds[0] = left;
        bounds[1] = right;
    }
    else
    {
        bounds[0] = right;
        bounds[1] = model.pos.words[0];
    }
    left = model.pos.words[2];
    right = target.words[2];
    if ((sint32)left < (sint32)right)
    {
        bounds[2] = left;
        bounds[3] = right;
    }
    else
    {
        bounds[2] = right;
        bounds[3] = model.pos.words[2];
    }
    bounds[0] -= 0x100000;
    bounds[1] += 0x100000;
    bounds[3] += 0x100000;
    bounds[2] -= 0x100000;
    Vector3 difference;
    SubtractVectors(difference.words, target.words, model.pos.words);
    sint32 count = (sint32)MeasureVectorLength(difference.words) / 512;
    if (count < 4)
        count = 4;
    uint32 green = mode ? 32 : 128;
    uint32 blue = mode ? 128 : 32;
    GameHeapBlock *allocation = AllocateEffectStorage(164);
    if (!allocation)
    {
        GameDiagnostic("Beam barrier effect allocation failed\n");
        abort();
    }
    effect = new (allocation) EndpointBeam(32, green, blue, count, 700, 32, 48, 440, 110, 800, 147);
    effect->SetEndpoints(model.pos.words, target.words);
    effect->persistent = 1;
    kind = 404;
    SetMode(mode);
}

BeamBarrier::BeamBarrier(const uint32 *data, uint32 index, uint32 mode) : VisualObject(), mode(0), effect()
{
    RecordDataBorrow record_borrow;
    uint32 value;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80019DA0, "SLUS_003.73");
    value = data[0];
    this->model.pos.words[0] = value << 12;
    value = data[1];
    this->model.pos.words[1] = value << 12;
    value = data[2];
    this->record_idx = index;
    this->model.pos.words[2] = value << 12;
    cursor = GetRecordPayload(index);
    ReadRecordPosition(target.words, (cursor + 2).Read16());
    Initialize(mode);
}

BeamBarrier::BeamBarrier(const uint32 *position, const uint32 *target) : VisualObject(), mode(0), effect()
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x80019E48, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    x = target[0];
    y = target[1];
    z = target[2];
    this->target.words[0] = x;
    this->target.words[1] = y;
    this->target.words[2] = z;
    Initialize(1);
}

BeamBarrier::~BeamBarrier()
{
    if (effect)
        effect->Destroy(3);
}

uint32 BeamBarrier::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001A0B4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (effect)
        effect->Destroy(3);
    effect = 0;
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseObjectStorage(this) : 0;
}

uint32 BeamBarrier::Update()
{
    FUNCTION_MARKER(0x8001A14C, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ModelInstance *player = visual_lists.players;
    if (!player)
        return 0;
    uint32 coord = player->pos.words[0];
    if (!((sint32)bounds[0] < (sint32)coord))
        return 0;
    if (!((sint32)coord < (sint32)bounds[1]))
        return 0;
    coord = player->pos.words[2];
    if (!((sint32)bounds[2] < (sint32)coord))
        return 0;
    if (!((sint32)coord < (sint32)bounds[3]))
        return 0;
    Vector3 collision;
    ModelInstance *hit = IntersectObjectSpheres(model.pos.words, target.words, collision.words, player, 0);
    player = visual_lists.players;
    if (hit != player)
        return hit != 0;
    const Vector3 zero = {0};
    static_cast<VisualObject *>(hit->owner)->Damage(10, zero.components, 0);
    if (!mode)
        return 0;
    static_cast<Player *>(visual_lists.players->owner)->Player::ApplyBeamContactImpulse(*this);
    SetParticleAngles(zero_angles.components);
    ShortVector3 angles = {{{512, ONE, 0}}};
    SetParticleSpread(angles.components);
    SetParticleSize(2);
    SetParticleColor(32, 32, 128);
    SetParticleColorStep(1, 1, 4);
    use_small_effect_pool = false;
    for (uint32 i = 0; i < 32; ++i)
    {
        GameHeapBlock *allocation = AllocateEffectStorage(88);
        if (allocation)
        {
            new (allocation) FadingParticle(visual_lists.players->pos.words, 16, 8192, 32);
        }
    }
    use_small_effect_pool = true;
    return 1;
}
