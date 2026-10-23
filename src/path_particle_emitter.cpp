#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "game_diagnostics.h"
#include "record_data.h"
#include "path_particle_emitter.h"
#include "moving_sprite.h"
#include "expanding_ring.h"
#include "global.h"
#include "effect_lifecycle.h"

void PathParticleEmitter::Spawn(uint32 identifier)
{
    EffectObject *node = effect_lists.controllers;
    GameHeapBlock *object;
    uint8 kind;
    FUNCTION_MARKER(0x8001F6A0, "SLUS_003.73");
    while (node)
    {
        kind = node->kind;
        if (node->IsAlive() && kind == 7)
        {
            if (static_cast<PathParticleEmitter *>(node)->identifier == identifier)
                return;
        }
        node = node->next;
    }
    object = EffectObject::AllocateStorage(88);
    if (object)
    {
        new (object) PathParticleEmitter(identifier);
    }
}

void PathParticleEmitter::DestroyByIdentifier(uint32 identifier)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8001F718, "SLUS_003.73");
    EffectObject *object = effect_lists.controllers;
    while (object)
    {
        uint8 kind = object->kind;
        EffectObject *next = object->next;
        if (object->IsAlive() && kind == 7 && static_cast<PathParticleEmitter *>(object)->identifier == identifier)
            object->Destroy(3);
        object = next;
    }
}

PathParticleEmitter::PathParticleEmitter(uint32 identifier) : EffectController(), start_rec(0), smoke_frequency(0), ring_frequency(0), identifier(0)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8001F8AC, "SLUS_003.73");
    RecordCursor rec(GetRecPayload(identifier));
    uint16 idx0 = (rec + 2).Read16();
    uint16 idx1 = (rec + 4).Read16();
    RecordCursor rec0(GetLevelRec(idx0));
    RecordCursor rec1(GetLevelRec(idx1));
    RecordCursor payload(0);
    if (rec0.Read16() == 10 && rec1.Read16() == 11)
    {
        start_rec = idx0;
        payload = rec1 + 2;
    }
    else if (rec0.Read16() == 11 && rec1.Read16() == 10)
    {
        start_rec = idx1;
        payload = rec0 + 2;
    }
    if (!payload)
    {
        GameDiagnostic(("Invalid path emitter record pair\n"));
        abort();
    }
    this->identifier = identifier;
    smoke_frequency = payload.Read16();
    kind = 7;
    ring_frequency = (payload + 2).Read16();
}

uint32 PathParticleEmitter::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001F9B8, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void PathParticleEmitter::Update()
{
    RecordDataBorrow rec_borrow;
    uint32 prev;
    uint16 cur;
    sint32 factor;
    uint16 colors[3];
    uint16 endpoint;
    sint32 channel;
    sint32 product;
    uint16 frequency;
    GameHeapBlock *child;
    RecordCursor rec;
    Vector3 origin, target, point, velocity;
    FUNCTION_MARKER(0x8001FA0C, "SLUS_003.73");
    prev = identifier;
    cur = start_rec;
    for (;;)
    {
        factor = RandomRange(256);
        rec = ReadRecPos(&origin, prev);
        colors[0] = (rec + 6).Read16();
        colors[1] = (rec + 8).Read16();
        colors[2] = (rec + 10).Read16();
        rec = ReadRecPos(&target, cur);
        for (channel = 0; channel < 3; ++channel)
        {
            endpoint = (rec + 6 + channel * 2).Read16();
            product = (endpoint - colors[channel]) * factor;
            colors[channel] += product >> 8;
        }
        point.Subtract(target, origin);
        point.ShiftRight(point, 12);
        point.Multiply(point, factor);
        point.ShiftRight(point, 8);
        point.ShiftLeft(point, 12);
        point.Add(origin, point);
        frequency = smoke_frequency;
        if (frequency && !RandomRange(frequency))
        {
            sint32 rise_speed = RandomRange(8);
            velocity.x = 0;
            velocity.y = -rise_speed * ONE;
            velocity.z = 0;
            MovingSprite *smoke = 0;
            child = EffectObject::AllocateStorage(112);
            if (child)
            {
                smoke = new (child) MovingSprite(&point, &velocity, "smoke", 1000, -60, 5);
            }
            if (!smoke)
            {
                // Preserve the existing native failure instead of dereferencing a null child
                GameDiagnostic(("Path emitter smoke allocation failed\n"));
                abort();
            }
            smoke->SetColors(colors[0] & 255, colors[1] & 255, colors[2] & 255);
        }
        frequency = ring_frequency;
        if (frequency && !RandomRange(frequency))
        {
            child = EffectObject::AllocateStorage(116);
            if (child)
            {
                new (child) ExpandingRing(&point, 150, colors[0], colors[1], colors[2], 20, 8);
            }
        }
        rec = GetRecPayload(cur);
        if (!rec.Read16())
            return;
        prev = cur;
        cur = (rec + 2).Read16();
    }
}
