#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include "game_tuning.h"
#include "game_diagnostics.h"

#include "character.h"
#include "object_damage.h"
#include "game_heap.h"
#include "model_resources.h"
#include "game_object_allocation.h"

// Native lookup for the mapped single-inheritance visual hierarchy

// Native binding for the original addressable list-head word

// Native adapter for the original adjusted suspend callback

// Native input adapters for original record word and halfword loads

// Native extraction of original signed color DIV corner behavior

// Native and legacy destruction share only the original buffer field operations

#include "visual_object.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "activation_record.h"

// Original list-level operation has a list-head receiver, not an object receiver

// List traversal is a free function, not an object method

#include "textured_quad.h"
#include "effect_lifecycle.h"

VisualLists visual_lists;
uint32 visual_activation_distance;

void ResetVisualLists()
{
    memset(&visual_lists, 0, sizeof(visual_lists));
}

static sint32 DivideVisualColor(sint32 numerator, uint32 divisor)
{
    if (!divisor)
        return numerator < 0 ? 1 : -1;
    return numerator / (sint32)divisor;
}

ScaledObject::ScaledObject()
{
    FUNCTION_MARKER(0x80062628, "SLUS_003.73");
    model.owner = this;
    model.scale.words[0] = ONE;
    model.scale.words[1] = ONE;
    model.scale.words[2] = ONE;
}

uint32 ScaledObject::Destroy(uint32 reason)
{
    uint32 flags, index;
    FUNCTION_MARKER(0x80062650, "SLUS_003.73");
    flags = model.flags & 1024;
    if (flags)
    {
        index = model.flash;
        ReleaseObjectFlash((uint8)index);
    }
    MarkDestroyed();
    model.InvalidateReferences();
    if (reason & 1)
        return ::ReleaseObjectStorage(this);
    return 0;
}

uint32 VisualObject::SetModelBank(const char *name)
{
    FUNCTION_MARKER(0x800626C8, "SLUS_003.73");
    uint32 result = FindModelBankByName(name);
    this->model.bank = result;
    this->model.model_idx = 0;
    return result;
}

uint32 VisualObject::SetColor(sint32 duration, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800626F8, "SLUS_003.73");
    uint32 flags, color, divisor, result;
    sint32 dr, dg, db;
    red &= 255;
    green &= 255;
    blue &= 255;
    color = red | (green << 8) | (blue << 16);
    if (!duration)
    {
        flags = this->model.flags;
        color_transition.timer = 0;
    }
    else if ((sint32)duration < 0)
    {
        divisor = (0 - (uint32)duration) & 65535;
        color_transition.timer = (uint16)divisor;
        dr = DivideVisualColor((sint32)red - 128, divisor);
        dg = DivideVisualColor((sint32)green - 128, color_transition.timer);
        db = DivideVisualColor((sint32)blue - 128, color_transition.timer);
        flags = this->model.flags;
        this->model.color = 0x808080;
        result = flags | 0x400;
        this->model.flags = (uint16)result;
        color_transition.step[0] = (uint16)dr;
        color_transition.step[1] = (uint16)dg;
        color_transition.step[2] = (uint16)db;
        return result;
    }
    else
    {
        dr = (128 - (sint32)red) / (sint32)duration;
        dg = (128 - (sint32)green) / (sint32)duration;
        db = (128 - (sint32)blue) / (sint32)duration;
        color_transition.step[2] = (uint16)db;
        flags = color_transition.saved;
        color_transition.timer = (uint16)((uint32)duration + 1);
        color_transition.step[1] = (uint16)dg;
        color_transition.step[0] = (uint16)dr;
        if (!(flags & 0x1000000))
        {
            uint32 old = this->model.color & 0xFFFFFF;
            flags = this->model.flags;
            color_transition.saved = old | 0x1000000;
            if (flags & 0x400)
                color_transition.saved = old | 0x3000000;
        }
        flags = this->model.flags;
    }
    this->model.color = color;
    result = flags | 0x400;
    this->model.flags = (uint16)result;
    return result;
}

uint32 VisualObject::AdvanceColor()
{
    FUNCTION_MARKER(0x80062858, "SLUS_003.73");
    uint32 timer = color_transition.timer;
    if (!timer)
        return timer;
    uint32 color = this->model.color;
    uint32 red_delta = (uint32)(sint32)(sint16)color_transition.step[0];
    uint32 green_delta = (uint32)(sint32)(sint16)color_transition.step[1];
    uint32 red = ((color & 255) + red_delta) & 255;
    uint32 green = ((((color >> 8) & 255) + green_delta) & 255) << 8;
    uint32 blue_delta = (uint32)(sint32)(sint16)color_transition.step[2];
    uint32 flags = this->model.flags;
    uint32 blue = ((((color >> 16) & 255) + blue_delta) & 255) << 16;
    this->model.flags = flags | 1024;
    timer = color_transition.timer;
    uint32 result = red | green | blue;
    this->model.color = result;
    timer = (timer - 1) & 65535;
    color_transition.timer = timer;
    if (!timer)
    {
        uint32 restore_flags = color_transition.saved;
        flags = this->model.flags;
        flags = (restore_flags & 0x2000000) ? flags | 1024 : flags & 0xFBFF;
        this->model.flags = flags;
        result = color_transition.saved;
        color_transition.saved = 0;
        this->model.color = result;
    }
    return result;
}

VisualObject::VisualObject() : ScaledObject(), contact_strength(10), player_distance(0), kind(0), activation_flags(0), lifecycle_flags(0), target_mark(0), event_flags(0), record_idx(0), collision_radius(0), contact_flags(0), health(0), target_disabled(0), saved_head(0), list_head(0)
{
    uint32 flags, value;
    FUNCTION_MARKER(0x80062924, "SLUS_003.73");
    memset(&surface, 0, sizeof(surface));
    motion.drag[0] = 1;
    motion.drag[1] = 1;
    motion.drag[2] = 1;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    this->record_idx = 65535;
    value = GetGameTuning().shadow_radius;
    shadow.radius = value;
    flags = this->lifecycle_flags;
    value = GetGameTuning().shadow_height;
    this->lifecycle_flags = flags | 18;
    this->collision_radius = 50;
    shadow.max_height = value;
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

uint32 VisualObject::IsFinished()
{
    FUNCTION_MARKER(0x80062F48, "SLUS_003.73");
    return (lifecycle_flags & 0x40) != 0;
}

uint32 VisualObject::Update()
{
    FUNCTION_MARKER(0x800639AC, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::UpdateLighting()
{
    FUNCTION_MARKER(0x8006399C, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::ReleaseResources()
{
    FUNCTION_MARKER(0x80063994, "SLUS_003.73");
    return 0;
}

uint32 VisualObject::Damage(uint32, const sint32 *, uint32)
{
    FUNCTION_MARKER(0x80062F5C, "SLUS_003.73");
    return 1;
}

uint32 VisualObject::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800629BC, "SLUS_003.73");
    if (shadow.effect)
        shadow.effect->Destroy(3);
    shadow.effect = 0;
    ScaledObject::Destroy(0);
    return reason & 1 ? ::ReleaseObjectStorage(this) : 0;
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
    if ((this->lifecycle_flags & 1) && &head != &visual_lists.suspended)
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

RecordCursor VisualObject::ReadPosition(RecordCursor data)
{
    FUNCTION_MARKER(0x80062B0C, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 value = (data + axis * 4).Read32() << 12;
        pos_history.saved.words[axis] = value;
        this->model.pos.words[axis] = value;
    }
    return data + 12;
}

RecordCursor VisualObject::ReadAngles(RecordCursor data)
{
    FUNCTION_MARKER(0x80062B50, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 value = (data + axis * 2).Read16();
        this->model.angles.words[axis] = value;
    }
    return data + 6;
}

uint32 VisualObject::MoveToTemporaryList(ModelInstance *&head)
{
    FUNCTION_MARKER(0x80062B7C, "SLUS_003.73");
    ReleaseResources();
    saved_head = &head;
    RemoveFromList(head);
    AddToList(visual_lists.suspended);
    uint32 result = this->lifecycle_flags | 1;
    this->lifecycle_flags = result;
    return result;
}

uint32 VisualObject::RestoreList()
{
    FUNCTION_MARKER(0x80062BF0, "SLUS_003.73");
    uint32 result = this->lifecycle_flags & 1;
    if (result)
    {
        RemoveFromList(visual_lists.suspended);
        AddToList(*saved_head);
        result = this->lifecycle_flags & 0xFFFE;
        this->lifecycle_flags = result;
    }
    return result;
}

uint32 *VisualObject::ApplyContactImpulse(const VisualObject &object)
{
    FUNCTION_MARKER(0x80062C50, "SLUS_003.73");
    Vector3 delta, shifted, product, quotient, impulse;
    uint32 shift = 12;
    SubtractVectors(delta.words, model.pos.words, object.model.pos.words);
    uint32 distance = MeasureVectorLength(delta.words);
    if (!distance)
        distance = 1;
    ShiftVectorRight(shifted.words, delta.words, shift);
    MultiplyScalarVector(product.words, object.contact_strength, shifted.words);
    DivideVector(quotient.words, product.words, distance);
    ShiftVectorLeft(impulse.words, quotient.words, shift);
    AddVectors(motion.velocity.words, impulse.words);
    return motion.velocity.words;
}

uint32 VisualObject::UpdateEventLatch(uint32 active, uint32 mask)
{
    uint32 flags;
    FUNCTION_MARKER(0x80062D24, "SLUS_003.73");
    if (active == 1)
    {
        flags = this->event_flags;
        if (flags & mask)
            return 0;
        this->event_flags = flags | mask;
        return 1;
    }
    flags = this->event_flags;
    this->event_flags = flags & ~mask;
    return 0;
}

uint32 VisualObject::EnableShadow()
{
    FUNCTION_MARKER(0x80062D70, "SLUS_003.73");
    uint32 result = this->lifecycle_flags | 8;
    this->lifecycle_flags = result;
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
        GameHeapBlock *allocation = AllocateEffectStorage(156);
        if (allocation)
        {
            shadow.effect = new (allocation) TexturedQuad();
        }
        use_small_effect_pool = true;
        if (!shadow.effect)
        {
            GameDiagnostic("Object shadow allocation failed\n");
            abort();
        }
        shadow.effect->SelectResource("Shadow", 0);
        shadow.effect->AddFlags(4);
        shadow.effect->EnableSemiTransparency();
        shadow.effect->depth_bias = 32;
        shadow.effect->persistent = 1;
    }
    const sint16 direction[3] = {0, -4096, 0};
    shadow.effect->SetPosition(surface.pos.words, direction, shadow.radius, shadow.radius);
    sint32 total = (sint16)shadow.max_height;
    sint32 height = (sint16)shadow.height;
    uint32 numerator = ((uint32)total - (uint32)height) << 7;
    sint32 alpha = total ? (sint32)numerator / total : ((sint32)numerator < 0 ? 1 : -1);
    if (alpha < 0)
        alpha = 0;
    return shadow.effect->SetBrightness((uint32)alpha & 255);
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

ModelInstance *ModelReference::GetObject() const
{
    if (!object || !object->active)
        return 0;
    return !object->owner || object->owner->IsAlive() ? object : 0;
}

ModelPolygon *ModelReference::GetPolygon() const
{
    return GetObject() ? poly : 0;
}
