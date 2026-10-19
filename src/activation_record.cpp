#include <new>

#include "visual_object.h"
#include "game_diagnostics.h"
#include "activation_record.h"
#include "record_data.h"
#include "record_factory.h"
#include "vector_distance.h"

static ActivationRecord *activation_records;

ActivationRecord::ActivationRecord(uint32 index, uint32 state) : ActivationRecordBase(), record_idx(0), saved_health(0), next(0)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8006FBB0, "SLUS_003.73");
    ActivationRecord *head = activation_records;
    record_idx = index;
    activation_records = this;
    saved_health = (sint16)(uint16)state;
    next = head;
    ReadRecordPosition(pos.words, index & 65535);
}

uint32 ActivationRecord::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8006FC00, "SLUS_003.73");
    ActivationRecord *prev = 0;
    ActivationRecord *cur = activation_records;
    // Original requires membership in this list
    while (cur != this)
    {
        prev = cur;
        cur = prev->next;
    }
    if (prev)
        prev->next = next;
    else
        activation_records = next;
    if (reason & 1)
        ReleaseStorage();
    return 0;
}

uint32 ClearActivationRecords()
{
    FUNCTION_MARKER(0x8006FB04, "SLUS_003.73");
    while (activation_records)
        activation_records->Destroy(3);
    return 0;
}

void DeleteActivationRecords()
{
    FUNCTION_MARKER(0x8006FB60, "SLUS_003.73");
    ActivationRecord *node = activation_records;
    while (node)
    {
        ActivationRecord *next = node->next;
        node->Destroy(3);
        node = next;
    }
}

uint32 VisualObject::DeactivateIfDistant(uint32 distance)
{
    FUNCTION_MARKER(0x8006FC84, "SLUS_003.73");
    if (!(lifecycle_flags & 4) || health < 0 || distance < 9000)
        return 0;
    new ActivationRecord(record_idx, (uint32)(sint32)health);
    Finish();
    return 1;
}

void RestoreNearbyObjects()
{
    FUNCTION_MARKER(0x8006FD18, "SLUS_003.73");
    ActivationRecord *node = activation_records;
    while (node)
    {
        ModelInstance *player = visual_lists.players;
        ActivationRecord *next = node->next;
        if (MeasurePositionDistance(player->pos.words, node->pos.words) < 9000)
        {
            VisualObject *object = CreateObjectFromRecord(node->record_idx);
            if (object)
            {
                uint16 state = (uint16)node->saved_health;
                uint16 saved_state = (uint16)node->saved_health;
                if ((sint16)state >= 0)
                    object->health = (sint16)saved_state;
                uint32 flags = object->lifecycle_flags;
                object->lifecycle_flags = flags | 4;
                node->Destroy(3);
            }
        }
        node = next;
    }
}

uint32 ActivationRecordBase::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8006FDC8, "SLUS_003.73");
    if (reason & 1)
        ReleaseStorage();
    return 0;
}
