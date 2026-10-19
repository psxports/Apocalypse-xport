#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"

#include "effect_object.h"

#include "visual_object.h"
#include "model_resources.h"
#include "game_diagnostics.h"
#include "ring_attack.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_arithmetic.h"
#include "glow_effect.h"
#include "vector_distance.h"
#include "audio_session.h"
#include "camera.h"
#include "object_damage.h"
#include "projectile_oscillating.h"
#include "projectile_line.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"

ObjectReference<RingAttack> active_ring_attack;

static void ClearRingAttackHitFlags(ModelInstance *list)
{
    FUNCTION_MARKER(0x80029F58, "SLUS_003.73");
    while (list)
    {
        static_cast<VisualObject *>(list->owner)->lifecycle_flags &= 0xFEFF;
        list = list->next;
    }
}

RingAttack::RingAttack(uint32 start_damage, uint32 end_damage, uint32 falloff_distance) : EffectController(), ring(), radius(0), start_damage(start_damage), end_damage(end_damage), falloff_distance(falloff_distance ? falloff_distance : 1), width(0)
{
    FUNCTION_MARKER(0x80029D24, "SLUS_003.73");
    kind = 3;
    ModelInstance *player = visual_lists.players;
    pos.words[0] = player->pos.words[0];
    pos.words[1] = player->pos.words[1];
    pos.words[2] = player->pos.words[2];
    GameHeapBlock *allocation = AllocateEffectStorage(116);
    if (!allocation)
    {
        GameDiagnostic("Ring attack mesh allocation failed\n");
        abort();
    }
    ring = new (allocation) MeshRingEffect(16, 0xCCE0187C);
    ring->persistent = 1;
    ring->SetGeometry(pos.words, 0, 0);
    uint32 first = RandomRange(64);
    uint32 second = RandomRange(64);
    ring->SetTextureOffsets(first, second, 2);
    color.r = 255;
    color.g = 100;
    color.b = 0;
    ring->SetColors(255, 100, 0);
    ring->forced_depth = 1;
    ClearRingAttackHitFlags(visual_lists.actors);
    ClearRingAttackHitFlags(visual_lists.solids);
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
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

static uint32 DivideRingAttack(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

void RingAttack::Update()
{
    FUNCTION_MARKER(0x8002A138, "SLUS_003.73");
    ModelBorrow model_borrow;
    uint32 inner_radius = width + radius;
    width += 130;
    if ((sint32)width >= 2001)
        radius += 100;
    if (ring)
    {
        ring->SetGeometry(pos.words, radius, width);
        ring->SetTextureOffsets(ring->u_scroll, ring->v_scroll - 1, 2);
    }
    color.r -= 3;
    color.b -= 3;
    color.g -= 3;
    if (ring)
        ring->SetColors(color.r, color.g, color.b);
    if ((sint32)color.r <= 0 && (sint32)color.g <= 0 && (sint32)color.b <= 0)
        Finish();
    if ((sint32)width >= 2001 && ring)
    {
        uint32 tick = game_clock.tick * 400;
        for (uint32 i = 0; i < ring->seg_count; ++i)
        {
            uint32 sine = rsin((tick + phase[i]) & 4095);
            uint32 product = sine * 200;
            if ((sint32)product < 0)
                product += 4095;
            product = (uint32)((sint32)product >> 12) << 12;
            ring->segs[i].v[2].words[1] += product;
        }
    }
    DamageObjects(visual_lists.actors, 1);
    DamageObjects(visual_lists.solids, 0);
    ModelInstance *node = visual_lists.projectiles;
    while (node)
    {
        VisualObject &visual = *static_cast<VisualObject *>(node->owner);
        if (visual.IsAlive() && !(visual.lifecycle_flags & 0x120) && visual.kind != 62 && visual.kind != 66 && visual.kind != 64)
        {
            uint32 distance = MeasurePositionDistance(pos.words, node->pos.words);
            if (width >= distance)
            {
                visual.VisualObject::Finish();
                HandleExplosionProximity(node->pos.words, 0);
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
            uint32 distance = MeasurePositionDistance(pos.words, effect->pos.words);
            if (width >= distance)
            {
                GameHeapBlock *allocation = AllocateEffectStorage(120);
                if (allocation)
                {
                    const RGBC &projectile_color = static_cast<OscillatingProjectile *>(effect)->rgb;
                    new (allocation) GlowEffect(effect->pos.words, projectile_color.r, projectile_color.g, projectile_color.b, 8, 18, 50, 4);
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
            uint32 distance = MeasurePositionDistance(pos.words, effect->pos.words);
            if (width >= distance)
            {
                GameHeapBlock *allocation = AllocateEffectStorage(120);
                if (allocation)
                {
                    const RGBC &projectile_color = static_cast<LineProjectile *>(effect)->rgb;
                    new (allocation) GlowEffect(effect->pos.words, projectile_color.r, projectile_color.g, projectile_color.b, 8, 18, 50, effect->kind);
                }
                effect->Finish();
            }
        }
        effect = effect->next;
    }
    uint32 outer_radius = width + radius;
    uint32 damage;
    if ((sint32)falloff_distance < (sint32)width)
        damage = (sint32)end_damage >> 2;
    else
    {
        uint32 product = (start_damage - end_damage) * width;
        uint32 value = start_damage - DivideRingAttack(product, falloff_distance);
        damage = (sint32)value >> 2;
    }
    node = GetModelSceneObjects();
    while (node)
    {
        uint32 distance = MeasurePositionDistance(node->pos.words, pos.words);
        if ((sint32)distance >= (sint32)inner_radius && (sint32)outer_radius >= (sint32)distance)
            DamageLevelObject(node, 0, (sint32)damage);
        node = node->next;
    }
}

void RingAttack::DamageObjects(ModelInstance *list, uint32 effects)
{
    FUNCTION_MARKER(0x80029F88, "SLUS_003.73");
    ModelBorrow model_borrow;
    while (list)
    {
        VisualObject &object = *static_cast<VisualObject *>(list->owner);
        uint32 flags = object.lifecycle_flags;
        if (object.IsAlive() && (flags & 16) && object.kind != 206 && !(flags & 256))
        {
            uint32 distance = MeasurePositionDistance(pos.words, list->pos.words);
            if ((sint32)width >= (sint32)distance)
            {
                if (effects && !IsGameHeapUnderPressure())
                    SpawnExplosionRing(list->pos.words, 70, 240, 200, 0, 5, 0, 100);
                Vector3 vector;
                SubtractVectors(vector.words, list->pos.words, pos.words);
                if (distance)
                    DivideVector(vector.words, distance);
                else
                    vector.words[0] = vector.words[1] = vector.words[2] = 0;
                uint32 damage;
                if ((sint32)falloff_distance < (sint32)width)
                    damage = end_damage;
                else
                {
                    uint32 product = (start_damage - end_damage) * width;
                    damage = start_damage - DivideRingAttack(product, falloff_distance);
                }
                object.Damage(damage, vector.components, 30);
                object.lifecycle_flags |= 256;
            }
        }
        list = list->next;
    }
}
