#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"
#include "game_diagnostics.h"
#include "record_data.h"

#include "path_particle_emitter.h"
#include "effect_sprite.h"
#include "expanding_ring.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"

PathParticleEmitter::PathParticleEmitter(uint32 identifier) : EffectController(), start_rec(0), smoke_frequency(0), ring_frequency(0), identifier(0)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8001F8AC, "SLUS_003.73");
    RecordCursor record(GetRecordPayload(identifier));
    uint32 first = (record + 2).Read16();
    uint32 second = (record + 4).Read16();
    RecordCursor first_record(GetLevelRecord(first));
    RecordCursor second_record(GetLevelRecord(second));
    RecordCursor payload(0);
    if ((sint16)(first_record).Read16() == 10 && (sint16)(second_record).Read16() == 11)
    {
        start_rec = first;
        payload = second_record + 2;
    }
    else if ((sint16)(first_record).Read16() == 11 && (sint16)(second_record).Read16() == 10)
    {
        start_rec = second;
        payload = first_record + 2;
    }
    if (!payload)
    {
        GameDiagnostic("TODO Invalid path emitter record pair\n");
        abort();
    }
    this->identifier = identifier;
    smoke_frequency = (payload).Read16();
    this->kind = 7;
    ring_frequency = (payload + 2).Read16();
}

void SpawnPathParticleEmitter(uint32 identifier)
{
    EffectObject *node = effect_lists.controllers;
    GameHeapBlock *object;
    uint32 value;
    FUNCTION_MARKER(0x8001F6A0, "SLUS_003.73");
    while (node)
    {
        value = node->kind;
        if (node->IsAlive() && value == 7)
        {
            value = static_cast<PathParticleEmitter *>(node)->identifier;
            if (value == identifier)
                return;
        }
        node = node->next;
    }
    object = AllocateEffectStorage(88);
    if (object)
    {
        new (object) PathParticleEmitter(identifier);
    }
}

uint32 PathParticleEmitter::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001F9B8, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void PathParticleEmitter::Update()
{
    RecordDataBorrow record_borrow;
    uint32 previous;
    uint32 current;
    uint32 factor;
    uint32 colors[3];
    uint32 endpoint;
    uint32 channel;
    uint32 product;
    uint32 frequency;
    GameHeapBlock *child;
    uint32 result;
    RecordCursor record;
    Vector3 origin, target, difference, shifted, scaled, fractional, offset, point, velocity;
    uint32 shift12 = 12, shift8 = 8;
    FUNCTION_MARKER(0x8001FA0C, "SLUS_003.73");
    previous = identifier;
    current = start_rec;
    for (;;)
    {
        factor = RandomRange(256);
        record = ReadRecordPosition(origin.words, previous);
        colors[0] = (record + 6).Read16();
        colors[1] = (record + 8).Read16();
        colors[2] = (record + 10).Read16();
        record = ReadRecordPosition(target.words, current);
        for (channel = 0; channel < 3; ++channel)
        {
            endpoint = (record + 6 + channel * 2).Read16();
            product = (endpoint - colors[channel]) * factor;
            colors[channel] += (uint32)((sint32)product >> 8);
        }
        SubtractVectors(difference.words, target.words, origin.words);
        ShiftVectorRight(shifted.words, difference.words, shift12);
        MultiplyVector(scaled.words, shifted.words, factor);
        ShiftVectorRight(fractional.words, scaled.words, shift8);
        ShiftVectorLeft(offset.words, fractional.words, shift12);
        AddVectors(point.words, origin.words, offset.words);
        target = point;
        frequency = smoke_frequency;
        if (frequency && !RandomRange(frequency))
        {
            result = RandomRange(8);
            velocity.words[0] = 0;
            velocity.words[1] = ((uint32)0 - result) << 12;
            velocity.words[2] = 0;
            MovingSprite *smoke = 0;
            child = AllocateEffectStorage(112);
            if (child)
            {
                smoke = new (child) MovingSprite(target.words, velocity.words, "smoke", 1000, (uint32)-60, 5);
            }
            if (!smoke)
            {
                // Preserve the existing native failure instead of dereferencing a null child
                GameDiagnostic("Path emitter smoke allocation failed\n");
                abort();
            }
            smoke->SetColors(colors[0] & 255, colors[1] & 255, colors[2] & 255);
        }
        frequency = ring_frequency;
        if (frequency && !RandomRange(frequency))
        {
            child = AllocateEffectStorage(116);
            if (child)
            {
                new (child) ExpandingRing(target.components, 150, colors[0] & 255, colors[1] & 255, colors[2] & 255, 20, 8);
            }
        }
        record = GetRecordPayload(current);
        result = (record).Read16();
        previous = current;
        if (!result)
            return;
        current = (record + 2).Read16();
    }
}
