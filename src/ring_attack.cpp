#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "visual_object.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "ring_attack.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "glow_effect.h"
#include "audio_session.h"
#include "camera.h"
#include "object_damage.h"
#include "oscillating_projectile.h"
#include "projectile_line.h"
#include "line_projectile.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "object_reference.h"

ObjectReference<RingAttack> active_ring_attack;

RingAttack::RingAttack(uint32 start_damage, uint32 end_damage, sint32 falloff_distance) : EffectController(), ring(), radius(0), start_damage(start_damage), end_damage(end_damage), falloff_distance(falloff_distance ? falloff_distance : 1), width(0)
{
    FUNCTION_MARKER(0x80029D24, "SLUS_003.73");
    kind = 3;
    ModelInstance *player = visual_lists.players;
    pos = player->pos;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(116);
    if (!allocation)
    {
        GameDiagnostic(("Ring attack mesh allocation failed\n"));
        abort();
    }
    ring = new (allocation) MeshRingEffect(16, 0xCCE0187C);
    ring->persistent = true;
    ring->SetGeometry(&pos, 0, 0);
    uint32 u = RandomRange(64);
    uint32 v = RandomRange(64);
    ring->SetTexOffsets(u, v, 2);
    color.r = 255;
    color.g = 100;
    color.b = 0;
    ring->SetColors(255, 100, 0);
    ring->forced_depth = 1;
    ClearHitFlags(visual_lists.actors);
    ClearHitFlags(visual_lists.solids);
    for (uint32 i = 0; i < 16; ++i)
        phase[i] = RandomRange(4096);
    static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(1);
    active_ring_attack = this;
    PlaySoundEffect(10, 8192, 0);
}

RingAttack::~RingAttack()
{
    if (ring)
        ring->Destroy(3);
    if (active_ring_attack.Get() == this)
        active_ring_attack = 0;
}

uint32 RingAttack::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80029ED4, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    active_ring_attack = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void RingAttack::ClearHitFlags(ModelInstance *list)
{
    FUNCTION_MARKER(0x80029F58, "SLUS_003.73");
    while (list)
    {
        static_cast<VisualObject *>(list->owner)->lifecycle_flags &= 0xFEFF;
        list = list->next;
    }
}

void RingAttack::DamageObjects(ModelInstance *list, bool effects)
{
    FUNCTION_MARKER(0x80029F88, "SLUS_003.73");
    ModelBorrow model_borrow;
    while (list)
    {
        VisualObject &object = *static_cast<VisualObject *>(list->owner);
        uint16 flags = object.lifecycle_flags;
        if (object.IsAlive() && (flags & 16) && object.kind != 206 && !(flags & 256))
        {
            sint32 distance = pos.Distance(list->pos);
            if (width >= distance)
            {
                if (effects && !IsGameHeapUnderPressure())
                    RingBurstEffect::SpawnExplosion(&list->pos, 70, 240, 200, 0, 5, 0, 100);
                Vector3 dir;
                dir.Subtract(list->pos, pos);
                if (distance)
                    dir.Divide(distance);
                else
                    dir.x = dir.y = dir.z = 0;
                uint32 damage;
                if (falloff_distance < width)
                    damage = end_damage;
                else
                {
                    sint32 product = (start_damage - end_damage) * width;
                    damage = start_damage - DivideSignedWord(product, falloff_distance);
                }
                object.Damage(damage, &dir, 30);
                object.lifecycle_flags |= 256;
            }
        }
        list = list->next;
    }
}

void RingAttack::Update()
{
    FUNCTION_MARKER(0x8002A138, "SLUS_003.73");
    ModelBorrow model_borrow;
    sint32 inner_radius = width + radius;
    width += 130;
    if (width >= 2001)
        radius += 100;
    if (ring)
    {
        ring->SetGeometry(&pos, radius, width);
        ring->SetTexOffsets(ring->u_scroll, ring->v_scroll - 1, 2);
    }
    color.r -= 3;
    color.b -= 3;
    color.g -= 3;
    if (ring)
        ring->SetColors(color.r, color.g, color.b);
    if (color.r <= 0 && color.g <= 0 && color.b <= 0)
        Finish();
    if (width >= 2001 && ring)
    {
        uint32 tick = game_clock.tick * 400;
        for (sint32 i = 0; i < ring->seg_count; ++i)
        {
            sint32 wave = rsin((tick + phase[i]) & 4095) * 200 / ONE;
            ring->segs[i].v[2].y = (uint32)ring->segs[i].v[2].y + (wave * ONE);
        }
    }
    DamageObjects(visual_lists.actors, true);
    DamageObjects(visual_lists.solids, false);
    ModelInstance *node = visual_lists.projectiles;
    while (node)
    {
        VisualObject &visual = *static_cast<VisualObject *>(node->owner);
        if (visual.IsAlive() && !(visual.lifecycle_flags & 0x120) && visual.kind != 62 && visual.kind != 66 && visual.kind != 64)
        {
            uint32 distance = pos.Distance(node->pos);
            if ((uint32)width >= distance)
            {
                visual.VisualObject::Finish();
                HandleExplosionProximity(&node->pos, 0);
                visual.lifecycle_flags |= 256;
            }
        }
        node = node->next;
    }
    EffectObject *effect = effect_lists.controllers;
    while (effect)
    {
        if (effect->IsAlive() && effect->kind == 1)
        {
            uint32 distance = pos.Distance(effect->pos);
            if ((uint32)width >= distance)
            {
                GameHeapBlock *allocation = EffectObject::AllocateStorage(120);
                if (allocation)
                {
                    const RGBC &projectile_color = static_cast<OscillatingProjectile *>(effect)->rgb;
                    new (allocation) GlowEffect(&effect->pos, projectile_color.r, projectile_color.g, projectile_color.b, 8, 18, 50, 4);
                }
                effect->Finish();
            }
        }
        effect = effect->next;
    }
    effect = effect_lists.renderables;
    while (effect)
    {
        if (effect->IsAlive() && effect->kind == 4)
        {
            uint32 distance = pos.Distance(effect->pos);
            if ((uint32)width >= distance)
            {
                GameHeapBlock *allocation = EffectObject::AllocateStorage(120);
                if (allocation)
                {
                    const RGBC &projectile_color = static_cast<LineProjectile *>(effect)->rgb;
                    new (allocation) GlowEffect(&effect->pos, projectile_color.r, projectile_color.g, projectile_color.b, 8, 18, 50, effect->kind);
                }
                effect->Finish();
            }
        }
        effect = effect->next;
    }
    sint32 outer_radius = width + radius;
    sint32 damage;
    if (falloff_distance < width)
        damage = end_damage;
    else
    {
        sint32 product = (start_damage - end_damage) * width;
        damage = start_damage - DivideSignedWord(product, falloff_distance);
    }
    damage >>= 2;
    node = ModelBank::GetSceneObjects();
    while (node)
    {
        sint32 distance = node->pos.Distance(pos);
        if (distance >= inner_radius && outer_radius >= distance)
            DamageLevelObject(node, 0, damage);
        node = node->next;
    }
}
