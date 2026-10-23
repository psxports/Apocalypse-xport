#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include "model_instance.h"
#include "record_factory.h"
#include "enemy_list.h"
#include "record_data.h"
#include "ring_attack.h"
#include "player.h"
#include "game_tuning.h"
#include "game_diagnostics.h"
#include "character.h"
#include "object_damage.h"
#include "game_heap.h"
#include "model_bank.h"
#include "game_object_allocation.h"
#include "visual_object.h"
#include "global.h"
#include "activation_record.h"
#include "textured_quad.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "activation_record_base.h"
#include "pause_menu.h"
#include "cd_voice_event.h"
#include "level_session.h"
#include "scene_lifecycle.h"
#include "input_recording.h"
#include "game_pad_input.h"
#include "game_settings.h"
#include "game_tick.h"
#include "resource_script.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "cheat_input.h"
#include "resource_io.h"
#include "game_panel.h"
#include "menu_window.h"
#include "hud_text.h"
#include "hud_sprites.h"
#include "texture_resources.h"

VisualLists visual_lists;
uint32 visual_activation_distance;

struct HealthTracker
{
    ModelReference object;
    sint16 prev_health;
    uint16 x;
    uint16 y;
    sint16 initial_health;
    uint32 scale;
    uint8 flash;

    HealthTracker() : prev_health(0), x(0), y(0), initial_health(0), scale(0), flash(0)
    {
    }
};

static HealthTracker health_tracker;

void VisualObject::ResetLists()
{
    memset(&visual_lists, 0, sizeof(visual_lists));
}

void VisualObject::FlagNearRingAttack()
{
    FUNCTION_MARKER(0x80029CC4, "SLUS_003.73");
    RingAttack *reference = active_ring_attack.Get();
    if (!reference)
        return;
    sint32 distance = model.pos.Distance(reference->pos);
    reference = active_ring_attack.Get();
    if (!reference)
        return;
    sint32 width = reference->width;
    if (width < distance)
        return;
    lifecycle_flags |= 256;
}

void VisualObject::SmoothListPos(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80030168, "SLUS_003.73");
    Vector3 cur, prev_quarter, cur_quarter, weighted, output;
    uint32 shift = 2, weight = 3;
    if (instance && instance == visual_lists.players)
    {
        Player &player = *static_cast<Player *>(instance->owner);
        cur = player.surface.pos;
        prev_quarter.ShiftRight(player.saved_surface_pos, shift);
        cur_quarter.ShiftRight(cur, shift);
        weighted.Scale(weight, cur_quarter);
        output.Add(prev_quarter, weighted);
        player.surface.pos = output;
        player.saved_surface_pos = cur;
    }
    while (instance)
    {
        cur = instance->pos;
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        prev_quarter.ShiftRight(visual.pos_history.saved, shift);
        cur_quarter.ShiftRight(cur, shift);
        weighted.Scale(weight, cur_quarter);
        output.Add(prev_quarter, weighted);
        instance->pos = output;
        visual.pos_history.saved = cur;
        instance = instance->next;
    }
}

ModelInstance *VisualObject::SaveListPos(ModelInstance *instance)
{
    FUNCTION_MARKER(0x8003032C, "SLUS_003.73");
    ModelInstance *special_head = visual_lists.players;
    if (instance && instance == special_head)
    {
        Player &player = *static_cast<Player *>(instance->owner);
        player.saved_surface_pos = player.surface.pos;
    }
    while (instance)
    {
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        visual.pos_history.saved = instance->pos;
        instance = instance->next;
    }
    return special_head;
}

ModelInstance *VisualObject::RestoreListPos(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80030390, "SLUS_003.73");
    ModelInstance *special_head = visual_lists.players;
    if (instance && instance == special_head)
    {
        Player &player = *static_cast<Player *>(instance->owner);
        player.surface.pos = player.saved_surface_pos;
    }
    while (instance)
    {
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        instance->pos = visual.pos_history.saved;
        instance = instance->next;
    }
    return special_head;
}

void VisualObject::FinishSelEnemies()
{
    FUNCTION_MARKER(0x8005C338, "SLUS_003.73");
    ModelInstance *instance = visual_lists.actors;
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        uint16 kind = object.kind;
        ModelInstance *next = instance->next;
        switch (kind)
        {
            case 200:
            case 201:
            case 202:
            case 204:
            case 205:
            case 207:
            case 208:
            case 209:
            case 210:
                object.Finish();
                break;
            default:
                break;
        }
        instance = next;
    }
}

uint32 VisualObject::SetModelBank(const char *name)
{
    FUNCTION_MARKER(0x800626C8, "SLUS_003.73");
    uint32 result = ModelBank::FindByName(name);
    model.bank = result;
    model.model_idx = 0;
    return result;
}

uint32 VisualObject::SetColor(sint32 duration, uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x800626F8, "SLUS_003.73");
    uint16 flags;
    uint32 color = r | (g << 8) | (b << 16);
    if (!duration)
    {
        flags = model.flags;
        color_transition.timer = 0;
    }
    else if (duration < 0)
    {
        uint16 divisor = -(uint32)duration;
        color_transition.timer = divisor;
        sint16 dr = DivideSignedWord(r - 128, divisor);
        sint16 dg = DivideSignedWord(g - 128, color_transition.timer);
        sint16 db = DivideSignedWord(b - 128, color_transition.timer);
        flags = model.flags;
        model.color = 0x808080;
        uint32 result = flags | 0x400;
        model.flags = result;
        color_transition.step.r = dr;
        color_transition.step.g = dg;
        color_transition.step.b = db;
        return result;
    }
    else
    {
        sint16 dr = (128 - r) / duration;
        sint16 dg = (128 - g) / duration;
        sint16 db = (128 - b) / duration;
        color_transition.step.b = db;
        uint32 saved = color_transition.saved;
        color_transition.timer = (uint32)duration + 1;
        color_transition.step.g = dg;
        color_transition.step.r = dr;
        if (!(saved & 0x1000000))
        {
            uint32 old = model.color & 0xFFFFFF;
            flags = model.flags;
            color_transition.saved = old | 0x1000000;
            if (flags & 0x400)
                color_transition.saved = old | 0x3000000;
        }
        flags = model.flags;
    }
    model.color = color;
    uint32 result = flags | 0x400;
    model.flags = result;
    return result;
}

uint32 VisualObject::AdvanceColor()
{
    FUNCTION_MARKER(0x80062858, "SLUS_003.73");
    if (!color_transition.timer)
        return 0;
    RGBC color;
    color.rgbc = model.color;
    color.r += color_transition.step.r;
    color.g += color_transition.step.g;
    color.b += color_transition.step.b;
    model.flags |= 1024;
    uint32 result = color.rgbc & 0xFFFFFF;
    model.color = result;
    --color_transition.timer;
    if (!color_transition.timer)
    {
        if (color_transition.saved & 0x2000000)
            model.flags |= 1024;
        else
            model.flags &= 0xFBFF;
        result = color_transition.saved;
        color_transition.saved = 0;
        model.color = result;
    }
    return result;
}

VisualObject::VisualObject() : ScaledObject(), contact_strength(10), player_distance(0), kind(0), activation_flags(0), lifecycle_flags(0), target_mark(0), event_flags(0), rec_idx(0), col_radius(0), contact_flags(0), health(0), target_disabled(0), saved_head(0), list_head(0)
{
    FUNCTION_MARKER(0x80062924, "SLUS_003.73");
    memset(&surface, 0, sizeof(surface));
    rec_idx = 65535;
    shadow.radius = GetGameTuning().shadow_radius;
    lifecycle_flags |= 18;
    col_radius = 50;
    shadow.max_height = GetGameTuning().shadow_height;
}

uint32 VisualObject::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800629BC, "SLUS_003.73");
    if (shadow.effect)
        shadow.effect->Destroy(3);
    shadow.effect = 0;
    ScaledObject::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

ModelInstance *VisualObject::AddToList(ModelInstance *&head)
{
    FUNCTION_MARKER(0x80062A38, "SLUS_003.73");
    list_head = &head;
    model.prev = 0;
    model.next = head;
    head = &model;
    if (model.next)
        model.next->prev = &model;
    return model.next;
}

ModelInstance *VisualObject::RemoveFromList(ModelInstance *&head)
{
    FUNCTION_MARKER(0x80062A64, "SLUS_003.73");
    if ((lifecycle_flags & 1) && &head != &visual_lists.suspended)
        RestoreList();
    if (model.next)
        model.next->prev = model.prev;
    if (model.prev)
        model.prev->next = model.next;
    ModelInstance *result = head;
    if (result == &model)
    {
        result = model.next;
        head = result;
    }
    if (list_head == &head)
        list_head = 0;
    return result;
}

RecordCursor VisualObject::ReadPos(RecordCursor data)
{
    FUNCTION_MARKER(0x80062B0C, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 val = (data + axis * 4).Read32() << 12;
        pos_history.saved.comp[axis] = val;
        model.pos.comp[axis] = val;
    }
    return data + 12;
}

RecordCursor VisualObject::ReadAngles(RecordCursor data)
{
    FUNCTION_MARKER(0x80062B50, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        model.angles.comp[axis] = (data + axis * 2).Read16();
    }
    return data + 6;
}

uint32 VisualObject::MoveToTmpList(ModelInstance *&head)
{
    FUNCTION_MARKER(0x80062B7C, "SLUS_003.73");
    ReleaseResources();
    saved_head = &head;
    RemoveFromList(head);
    AddToList(visual_lists.suspended);
    uint32 result = lifecycle_flags | 1;
    lifecycle_flags = result;
    return result;
}

uint32 VisualObject::RestoreList()
{
    FUNCTION_MARKER(0x80062BF0, "SLUS_003.73");
    uint32 result = lifecycle_flags & 1;
    if (result)
    {
        RemoveFromList(visual_lists.suspended);
        AddToList(*saved_head);
        result = lifecycle_flags & 0xFFFE;
        lifecycle_flags = result;
    }
    return result;
}

Vector3 *VisualObject::ApplyContactImpulse(const VisualObject &object)
{
    FUNCTION_MARKER(0x80062C50, "SLUS_003.73");
    Vector3 delta, shifted, product, quotient, impulse;
    delta.Subtract(model.pos, object.model.pos);
    uint32 distance = delta.Len();
    if (!distance)
        distance = 1;
    shifted.ShiftRight(delta, 12);
    product.Scale(object.contact_strength, shifted);
    quotient.Divide(product, distance);
    impulse.ShiftLeft(quotient, 12);
    motion.velocity.Add(impulse);
    return &motion.velocity;
}

bool VisualObject::UpdateEventLatch(uint32 active, uint32 mask)
{
    FUNCTION_MARKER(0x80062D24, "SLUS_003.73");
    if (active == 1)
    {
        if (event_flags & mask)
            return false;
        event_flags |= mask;
        return true;
    }
    event_flags &= ~mask;
    return false;
}

uint32 VisualObject::EnableShadow()
{
    FUNCTION_MARKER(0x80062D70, "SLUS_003.73");
    uint32 result = lifecycle_flags | 8;
    lifecycle_flags = result;
    return result;
}

uint32 VisualObject::RemoveShadow()
{
    FUNCTION_MARKER(0x80062D84, "SLUS_003.73");
    uint32 result = lifecycle_flags & 0xFFF7;
    lifecycle_flags = result;
    if (shadow.effect)
    {
        result = shadow.effect->Destroy(3);
        shadow.effect = 0;
    }
    return result;
}

uint32 VisualObject::UpdateShadow()
{
    FUNCTION_MARKER(0x80062DD8, "SLUS_003.73");
    if (!(lifecycle_flags & 8))
        return RemoveShadow();
    if (!shadow.effect)
    {
        use_small_effect_pool = false;
        GameHeapBlock *allocation = EffectObject::AllocateStorage(156);
        if (allocation)
        {
            shadow.effect = new (allocation) TexturedQuad();
        }
        use_small_effect_pool = true;
        if (!shadow.effect)
        {
            GameDiagnostic(("Object shadow allocation failed\n"));
            abort();
        }
        shadow.effect->SelectResource("Shadow", 0);
        shadow.effect->AddFlags(4);
        shadow.effect->EnableSemiTransparency();
        shadow.effect->depth_bias = 32;
        shadow.effect->persistent = true;
    }
    const ShortVector3 dir = {{{0, -ONE, 0}}};
    shadow.effect->SetPos(&surface.pos, &dir, shadow.radius, shadow.radius);
    sint32 total = shadow.max_height;
    sint32 height = shadow.height;
    sint32 numerator = (total - height) * 128;
    sint32 a = DivideSignedWord(numerator, total);
    if (a < 0)
        a = 0;
    uint8 brightness = a;
    return shadow.effect->SetBrightness(brightness);
}

uint32 VisualObject::Finish()
{
    FUNCTION_MARKER(0x80062F0C, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        result = lifecycle_flags | 0x40;
        lifecycle_flags = result;
    }
    return result;
}

VisualObject::~VisualObject()
{
    if (shadow.effect)
        shadow.effect->Destroy(3);
    if (!list_head)
        return;
    if (model.next)
        model.next->prev = model.prev;
    if (model.prev)
        model.prev->next = model.next;
    if (*list_head == &model)
        *list_head = model.next;
}

bool VisualObject::IsFinished()
{
    FUNCTION_MARKER(0x80062F48, "SLUS_003.73");
    return (lifecycle_flags & 0x40) != 0;
}

uint32 VisualObject::Damage(uint32, const Vector3 *, uint32)
{
    FUNCTION_MARKER(0x80062F5C, "SLUS_003.73");
    return 1;
}

void VisualObject::UpdateAnimationPose()
{
    FUNCTION_MARKER(0x8006338C, "SLUS_003.73");
    bool animated = (model.flags & 2) != 0;
    if (animated)
        static_cast<Character *>(this)->AdvanceAnimation();
    if (pose_interpolation.count < 2)
    {
        if (animated)
            UpdateAnimationBehavior();
        else
            Update();
        return;
    }
    uint8 step = pose_interpolation.step + 1;
    uint8 count = pose_interpolation.count;
    pose_interpolation.step = step;
    if (step < count)
    {
        model.pos.Add(pose_interpolation.delta_pos);
        model.angles.AddAngles(pose_interpolation.delta_angles);
        model.angles.NormalizeAngles();
        return;
    }
    pose_interpolation.step = 0;
    model.pos = pose_interpolation.target_pos;
    pos_history.prev = model.pos;
    model.angles = pose_interpolation.target_angles;
    pose_interpolation.prev_angles = model.angles;
    if (model.flags & 2)
        UpdateAnimationBehavior();
    else
        Update();
    pose_interpolation.target_pos = model.pos;
    model.pos = pos_history.prev;
    Vector3 delta;
    delta.Subtract(pose_interpolation.target_pos, pos_history.prev);
    count = pose_interpolation.count;
    pose_interpolation.delta_pos.Divide(delta, count);
    pose_interpolation.target_angles = model.angles;
    model.angles = pose_interpolation.prev_angles;
    pose_interpolation.delta_angles.SubtractAngles(pose_interpolation.target_angles, pose_interpolation.prev_angles);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint16 &angle = pose_interpolation.delta_angles.comp[axis];
        if (angle < -2048)
            angle += 4096;
        if (angle > 2048)
            angle -= 4096;
    }
    count = pose_interpolation.count;
    pose_interpolation.delta_angles.DivideAngles(count);
}

void VisualObject::ClearActivationFlags(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80063738, "SLUS_003.73");
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        object.activation_flags &= 1;
        instance = next;
    }
}

void VisualObject::UpdateList(ModelInstance *&head)
{
    ModelBorrow model_borrow;
    FUNCTION_MARKER(0x80063770, "SLUS_003.73");
    ModelInstance *instance = head;
    ModelInstance *next = instance ? instance->next : 0;
    while (instance)
    {
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        if (!visual.IsAlive())
        {
            instance = next;
            next = instance ? instance->next : 0;
            continue;
        }
        visual.AdvanceColor();
        uint16 flags = visual.lifecycle_flags;
        if (flags & 64)
        {
            if (flags & 128)
                visual.Destroy(3);
            else
                visual.lifecycle_flags = flags | 128;
        }
        else
        {
            ModelInstance *player = visual_lists.players;
            if (!player)
            {
                GameDiagnostic(("Visual update has no player\n"));
                abort();
            }
            uint32 distance = player->pos.Distance(instance->pos);
            if (!visual.DeactivateIfDistant(distance))
            {
                flags = visual.lifecycle_flags;
                visual.player_distance = distance;
                if ((flags & 2) && visual_activation_distance < distance)
                    visual.MoveToTmpList(head);
                else
                {
                    visual.UpdateAnimationPose();
                    visual.UpdateShadow();
                }
            }
        }
        instance = next;
        if (!next)
            return;
        next = next->next;
    }
}

void VisualObject::RestoreNearbySuspended()
{
    FUNCTION_MARKER(0x800638A4, "SLUS_003.73");
    ModelInstance *instance = visual_lists.suspended;
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        uint16 flags = object.lifecycle_flags;
        if (flags & 2)
        {
            uint32 distance = visual_lists.players->pos.Distance(instance->pos);
            if (visual_activation_distance >= distance)
                object.RestoreList();
        }
        instance = next;
    }
}

uint32 VisualObject::ReleaseResources()
{
    FUNCTION_MARKER(0x80063994, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::UpdateLighting()
{
    FUNCTION_MARKER(0x8006399C, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::Update()
{
    FUNCTION_MARKER(0x800639AC, "SLUS_003.73");
    return 0;
}

void VisualObject::ActivateByIdx(ModelInstance *instance, uint32 idx)
{
    FUNCTION_MARKER(0x80064E10, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.rec_idx == idx)
            object.activation_flags |= 1;
        instance = instance->next;
    }
}

void VisualObject::UpdateAnimationBehavior()
{
    Character &character = *static_cast<Character *>(this);
    sint8 frame = model.frame;
    character.playback.prev_frame = frame;
    Update();
    frame = model.frame;
    character.playback.cur_frame = frame;
    character.playback.prev_animation = model.animation;
    character.playback.prev_dir = character.playback.dir;
}

sint32 VisualObject::TrackHealth(uint16 x, uint16 y, uint32 scale)
{
    FUNCTION_MARKER(0x8006D390, "SLUS_003.73");
    sint32 health = this->health;
    health_tracker.object.SetObject(&model);
    health_tracker.x = x;
    health_tracker.y = y;
    health_tracker.scale = scale;
    health_tracker.initial_health = health;
    health_tracker.prev_health = health;
    return health;
}

void VisualObject::ClearTrackedHealth()
{
    FUNCTION_MARKER(0x8006D3B4, "SLUS_003.73");
    health_tracker.object.Reset();
}

void VisualObject::DrawTrackedHealth()
{
    VisualObject *object;
    uint8 *packet, *cursor, *next, *limit;
    sint32 health, product;
    uint32 scale;
    uint8 flash;
    uint32 x, y, right, top, bottom, left;
    sint16 initial_health;
    FUNCTION_MARKER(0x8006D3C0, "SLUS_003.73");
    ModelInstance *instance = health_tracker.object.GetObject();
    if (!instance)
        return;
    object = static_cast<VisualObject *>(instance->owner);
    if (object->lifecycle_flags & 64)
    {
        VisualObject::ClearTrackedHealth();
        return;
    }
    health = object->health;
    if (health < 0)
        health = 0;
    if (health < health_tracker.prev_health)
    {
        health_tracker.prev_health = health;
        health_tracker.flash = 8;
    }
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)(sizeof(POLY_F4)))
        return;
    next = packet + sizeof(POLY_F4);
    scene_render.cursor = next;
    POLY_F4 *health_poly = (POLY_F4 *)packet;
    setPolyF4(health_poly);
    health_poly->rgbc.r = 128;
    flash = health_tracker.flash;
    health_poly->rgbc.g = flash << 4;
    flash = health_tracker.flash;
    health_poly->rgbc.b = flash << 4;
    scale = health_tracker.scale;
    x = health_tracker.x;
    product = (health >> 2) * scale;
    y = health_tracker.y;
    setSemiTrans(health_poly, 1);
    health_poly->p[0].x = x;
    health_poly->p[2].x = x;
    health_poly->p[0].y = y;
    health_poly->p[1].y = y;
    y += 6;
    health_poly->p[2].y = y;
    health_poly->p[3].y = y;
    cursor = scene_render.cursor;
    right = x + (product >> 12);
    health_poly->p[1].x = right;
    health_poly->p[3].x = right;
    limit = scene_render.limit;
    setaddr(health_poly, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)health_poly));
    if (limit - cursor < (sint32)(sizeof(DR_TPAGE)))
        return;
    next = cursor + sizeof(DR_TPAGE);
    scene_render.cursor = next;
    DR_TPAGE *mode = (DR_TPAGE *)cursor;
    setlen(mode, 1);
    mode->code[0] = 0xE1000220;
    flash = health_tracker.flash;
    setaddr(mode, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)mode));
    if (flash)
        health_tracker.flash = flash - 1;
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)(sizeof(LINE_F4)))
        return;
    next = packet + sizeof(LINE_F4);
    scene_render.cursor = next;
    LINE_F4 *outline = (LINE_F4 *)packet;
    setLineF4(outline);
    outline->rgbc.r = 240;
    outline->rgbc.g = 0;
    outline->rgbc.b = 0;
    initial_health = health_tracker.initial_health;
    scale = health_tracker.scale;
    y = health_tracker.y;
    cursor = scene_render.cursor;
    product = (initial_health >> 2) * scale;
    top = y - 1;
    bottom = y + 6;
    x = health_tracker.x;
    outline->p[2].y = bottom;
    outline->p[3].y = bottom;
    outline->p[0].y = top;
    outline->p[1].y = top;
    left = x - 1;
    outline->p[0].x = left;
    outline->p[3].x = left;
    right = x + (product >> 12) + 1;
    outline->p[1].x = right;
    outline->p[2].x = right;
    limit = scene_render.limit;
    setaddr(outline, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)outline));
    if (limit - cursor < (sint32)(sizeof(LINE_F2)))
        return;
    next = cursor + sizeof(LINE_F2);
    scene_render.cursor = next;
    LINE_F2 *left_line = (LINE_F2 *)cursor;
    setLineF2(left_line);
    left_line->rgbc.r = 240;
    left_line->rgbc.g = 0;
    left_line->rgbc.b = 0;
    y = health_tracker.y;
    x = health_tracker.x;
    top = y - 1;
    bottom = y + 6;
    left_line->p[1].y = bottom;
    left = x - 1;
    left_line->p[0].x = left;
    left_line->p[1].x = left;
    left_line->p[0].y = top;
    setaddr(left_line, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)left_line));
}

bool VisualObject::DeactivateIfDistant(uint32 distance)
{
    FUNCTION_MARKER(0x8006FC84, "SLUS_003.73");
    if (!(lifecycle_flags & 4) || health < 0 || distance < 9000)
        return false;
    new ActivationRecord(rec_idx, health);
    Finish();
    return true;
}
