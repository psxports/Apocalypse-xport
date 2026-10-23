#include "visual_object.h"
#include "activation_record.h"
#include "activation_record_base.h"
#include "record_data.h"
#include "record_factory.h"

static ActivationRecord *activation_recs;

uint32 ActivationRecord::ClearAll()
{
    FUNCTION_MARKER(0x8006FB04, "SLUS_003.73");
    while (activation_recs)
        activation_recs->Destroy(3);
    return 0;
}

void ActivationRecord::DeleteAll()
{
    FUNCTION_MARKER(0x8006FB60, "SLUS_003.73");
    ActivationRecord *node = activation_recs;
    while (node)
    {
        ActivationRecord *next = node->next;
        node->Destroy(3);
        node = next;
    }
}

ActivationRecord::ActivationRecord(uint16 idx, sint16 health) : ActivationRecordBase(), rec_idx(0), saved_health(0), next(0)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8006FBB0, "SLUS_003.73");
    ActivationRecord *head = activation_recs;
    rec_idx = idx;
    activation_recs = this;
    saved_health = health;
    next = head;
    ReadRecPos(&pos, idx);
}

uint32 ActivationRecord::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8006FC00, "SLUS_003.73");
    ActivationRecord *prev = 0;
    ActivationRecord *cur = activation_recs;
    // Original requires membership in this list
    while (cur != this)
    {
        prev = cur;
        cur = prev->next;
    }
    if (prev)
        prev->next = next;
    else
        activation_recs = next;
    if (reason & 1)
        ReleaseStorage();
    return 0;
}

void ActivationRecord::RestoreNearby()
{
    FUNCTION_MARKER(0x8006FD18, "SLUS_003.73");
    ActivationRecord *node = activation_recs;
    while (node)
    {
        ModelInstance *player = visual_lists.players;
        ActivationRecord *next = node->next;
        if (player->pos.Distance(node->pos) < 9000)
        {
            VisualObject *object = CreateObjectFromRec(node->rec_idx);
            if (object)
            {
                if (node->saved_health >= 0)
                    object->health = node->saved_health;
                object->lifecycle_flags |= 4;
                node->Destroy(3);
            }
        }
        node = next;
    }
}
