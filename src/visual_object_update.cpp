#include <stdio.h>
#include <stdlib.h>

#include "game_diagnostics.h"

#include "visual_object.h"
#include "player.h"
#include "ring_attack.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "activation_record.h"

// Original list-level operation has a list-head receiver, not an object receiver

// List traversal is a free function, not an object method

void FlagObjectNearReference(VisualObject &object)
{
    FUNCTION_MARKER(0x80029CC4, "SLUS_003.73");
    RingAttack *reference = active_ring_attack.Get();
    if (!reference)
        return;
    uint32 distance = MeasurePositionDistance(object.model.pos.words, reference->pos.words);
    reference = active_ring_attack.Get();
    if (!reference || (sint32)reference->width < (sint32)distance)
        return;
    object.lifecycle_flags |= 256;
}

void SmoothObjectPositions(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80030168, "SLUS_003.73");
    Vector3 cur, prev_quarter, cur_quarter, weighted, output;
    uint32 shift = 2, weight = 3;
    if (instance && instance == visual_lists.players)
    {
        Player &player = *static_cast<Player *>(instance->owner);
        cur = player.surface.pos;
        ShiftVectorRight(prev_quarter.words, player.saved_surface_pos.words, shift);
        ShiftVectorRight(cur_quarter.words, cur.words, shift);
        MultiplyScalarVector(weighted.words, weight, cur_quarter.words);
        AddVectors(output.words, prev_quarter.words, weighted.words);
        player.surface.pos = output;
        player.saved_surface_pos = cur;
    }
    while (instance)
    {
        cur = instance->pos;
        VisualObject &visual = *static_cast<VisualObject *>(instance->owner);
        ShiftVectorRight(prev_quarter.words, visual.pos_history.saved.words, shift);
        ShiftVectorRight(cur_quarter.words, cur.words, shift);
        MultiplyScalarVector(weighted.words, weight, cur_quarter.words);
        AddVectors(output.words, prev_quarter.words, weighted.words);
        instance->pos = output;
        visual.pos_history.saved = cur;
        instance = instance->next;
    }
}

ModelInstance *SaveObjectPositions(ModelInstance *instance)
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

ModelInstance *RestoreObjectPositions(ModelInstance *instance)
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

void UpdateVisualObjects(ModelInstance *&head)
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
        uint32 flags = visual.lifecycle_flags;
        if (flags & 64)
        {
            if (flags & 128)
                visual.Destroy(3);
            else
                visual.lifecycle_flags = flags | 128;
        }
        else
        {
            uint32 distance = MeasurePositionDistance(visual_lists.players->pos.words, instance->pos.words);
            if (!visual.DeactivateIfDistant(distance))
            {
                flags = visual.lifecycle_flags;
                visual.player_distance = distance;
                if ((flags & 2) && visual_activation_distance < distance)
                    visual.MoveToTemporaryList(head);
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
