#ifndef MODEL_ATTACHMENT_DATA_H
#define MODEL_ATTACHMENT_DATA_H

extern "C"
{
#include "psx.h"
}

struct ModelAttachmentPoint
{
    sint16 x;
    sint16 y;
    sint16 z;
    uint16 bone;
};

struct ModelAttachmentTable
{
    uint32 count;
    const ModelAttachmentPoint *points;
};

extern const ModelAttachmentTable WarAttachments;
extern const ModelAttachmentTable TheBeastAttachments;
extern const ModelAttachmentTable PlagueAttachments;
extern const ModelAttachmentTable DeathAttachments;
extern const ModelAttachmentTable SwatMode1Attachments;
extern const ModelAttachmentTable SwatMode2Attachments;
extern const ModelAttachmentTable SwatMode4Attachments;
extern const ModelAttachmentTable SwatMode256Attachments;
extern const ModelAttachmentTable SwatMode64Attachments;
extern const ModelAttachmentTable SwatMode16Attachments;
extern const ModelAttachmentTable SwatMode8Attachments;
extern const ModelAttachmentTable TankAttachments;
extern const ModelAttachmentTable TurretDefaultAttachments;
extern const ModelAttachmentTable TurretRaisedAttachments;
extern const ModelAttachmentTable TurretLevel4Attachments;
extern const ModelAttachmentTable TurretLevel10Attachments;
extern const ModelAttachmentTable PlayerAttachments;
extern const ModelAttachmentTable CrocAttachments;
extern const ModelAttachmentTable MiniTankAttachments;

const ModelAttachmentPoint &GetModelAttachmentPoint(const ModelAttachmentTable *table, uint32 index);

#endif
